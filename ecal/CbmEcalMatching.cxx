#include "CbmEcalMatching.h"

#include "CbmRootManager.h"
#include "CbmMCTrack.h"

#include "TClonesArray.h"
#include "TMath.h"
#include "TTree.h"

#include "CbmEcalRecParticle.h"
#include "CbmEcalPoint.h"
#include "CbmEcalClusterV1.h"
#include "CbmEcalStructure.h"
#include "CbmEcalShowerLib.h"

#include <iostream>

using namespace std;


/** Standard constructor **/
CbmEcalMatching::CbmEcalMatching(const char* name, const Int_t verbose)
  : CbmTask(name, verbose)
{
  fThreshold=0.8;
  fPhotonThr=0.95;
  fTree=NULL;
}


/** Task initialization **/
InitStatus CbmEcalMatching::Init()
{
  CbmRootManager* io=CbmRootManager::Instance();
  if (!io)
  {
    Fatal("Init", "Can't find IOManager.");
    return kFATAL;
  }
  fReco=(TClonesArray*)io->GetObject("EcalReco");
  if (!fReco)
  {
    Fatal("Init", "Can't find an array of reconstructed photons.");
    return kFATAL;
  }
  fMCTracks=(TClonesArray*)io->ActivateBranch("MCTrack");
  if (!fMCTracks)
  {
    Fatal("Init", "Can't find array of MC tracks");
    return kFATAL;
  }
  fPoints=(TClonesArray*)io->ActivateBranch("ECALPoint");
  if (!fPoints)
  {
    Fatal("Init", "Can't find array of Ecal Points");
    return kFATAL;
  }
  fStr=(CbmEcalStructure*)io->GetObject("EcalStructure");
  if (!fStr)
  {
    Fatal("Init","Can't find calorimeter structure in the system.");
    return kFATAL;
  }
  fClusters=(TClonesArray*)io->GetObject("EcalClusters");
  if (!fClusters)
  {
    Fatal("Init", "Can't find array of clusters");
    return kFATAL;
  }
  fShLib=(CbmEcalShowerLib*)io->GetObject("EcalShowerLib");
  if (!fShLib)
  {
    Info("Init", "No shower library found in system. Will continue without chi2 calculation.");
  }

  fEv=0; 
  return kSUCCESS;
}

/** Exec for task **/
void CbmEcalMatching::Exec(Option_t* opt)
{
  fEv++;
  if (fVerbose>0)
    Info("Exec", "Event number %d. ", fEv);
  Int_t i;
  Int_t n=fReco->GetEntriesFast();
  CbmEcalRecParticle* p;
  if (fToTree) InitTree();
  for(i=0;i<n;i++)
  {
    p=(CbmEcalRecParticle*)fReco->At(i);
    FormEpred(p);
    MatchP(p);
/*
    FormPreCluster(p);
    FormE();
    Match(p);
*/
  }
}

/** Get form of energy deposition of the particle **/
void CbmEcalMatching::FormEpred(CbmEcalRecParticle* p)
{
  CbmEcalClusterV1* cluster=(CbmEcalClusterV1*)fClusters->At(p->ClusterNum());
  Int_t k=0;
  Int_t type;
  static Double_t module=fStr->GetEcalInf()->GetModuleSize();
  Double_t cellsize;
  CbmEcalCell* cell;
  Double_t x;
  Double_t y;
  Double_t cx;
  Double_t cy;
  Double_t r;
  Double_t theta;
  Double_t phi;
  
  fEpred.Set(cluster->Size());
  fEsum=0;
  for(k=0;k<cluster->Size();k++)
  {
    cell=fStr->GetHitCell(cluster->CellNum(k));
    type=cell->GetType();
    cellsize=module/type;
    cx=cell->GetCenterX(); x=cx;
    cy=cell->GetCenterY(); y=cy;
    r=TMath::Sqrt(x*x+y*y);
    x-=p->X(); 
    y-=p->Y();

    /** TODO: should be Z of the cell**/
    theta=TMath::ATan(r/fStr->GetEcalInf()->GetZPos());
    theta*=TMath::RadToDeg();
    phi=TMath::ACos(cx/r)*TMath::RadToDeg();
    if (cy<0) phi=360.0-phi;

    fEpred[k]=fShLib->GetSumEThetaPhi(x, y, cellsize, p->E(), theta, phi);
    fEsum+=fEpred[k];
  }
  fS=0;
  for(k=0;k<cluster->Size();k++)
  {
    cell=fStr->GetHitCell(cluster->CellNum(k));
    fS+=fEpred[k]/fEsum*cell->GetTotalEnergy();
  }
}

/** Add to particle constants to track and all its mothers **/
void CbmEcalMatching::AddTrackP(Int_t track, Double_t e, Int_t cell)
{
  CbmMCTrack* tr;
  Int_t num;

  fP[track]+=e*fEpred[cell]/fEsum;

  tr=(CbmMCTrack*)fMCTracks->At(track);
  if (tr==NULL)
  {
    Warning("AddTrackE", "Can't find MCTrack number %d.", track);
    return;
  }
  num=tr->GetMotherId();
  if (num<0) return;
  tr=(CbmMCTrack*)fMCTracks->At(num);
  if (tr==NULL)
  {
    Warning("AddTrackE", "Can't find MCTrack number %d.", num);
    return;
  }
  if (tr->GetPdgCode()==22||TMath::Abs(tr->GetPdgCode())==11)
    AddTrackP(num, e, cell);
}

  
/** Match MCTrack and reconstructed partile using shower shape **/
void CbmEcalMatching::MatchP(CbmEcalRecParticle* p)
{
  CbmEcalClusterV1* cluster=(CbmEcalClusterV1*)fClusters->At(p->ClusterNum());
  fP.clear();
  Int_t k;
  CbmEcalCell* cell;
  map<Int_t, Float_t>::const_iterator q;
  map<Int_t, Double_t>::const_iterator i;
  pair<Int_t, Double_t> max; max.first=-1111; max.second=0;
  pair<Int_t, Double_t> max_photon; max_photon.second=0;
  CbmMCTrack* tr;
  CbmEcalClusterV1* cls;

  for(k=0;k<cluster->Size();k++)
  {
    cell=fStr->GetHitCell(cluster->CellNum(k));
    for(q=cell->GetTrackEnergyBegin();q!=cell->GetTrackEnergyEnd();++q)
      AddTrackP((*q).first, (*q).second, k);
    for(q=cell->GetTrackPSEnergyBegin();q!=cell->GetTrackPSEnergyEnd();++q)
      AddTrackP((*q).first, (*q).second, k);
  }

  for(i=fP.begin(); i!=fP.end();++i)
  {
    if ((*i).second/fS<fThreshold) continue;
    if ((*i).second>max.second)
      max=(*i);
    tr=(CbmMCTrack*)fMCTracks->At((*i).first);
    if (tr==NULL)
    {
      Warning("Match", "Can't find MCTrack number %d.", (*i).first);
      return;
    }
    if (tr->GetPdgCode()!=22) continue;
    if ((*i).second>max_photon.second)
      max_photon=(*i);
  }
  if (max.first==-1111)
  {
    if (fVerbose>2) Info("Match", "Matching failed for reconstructed photon (%f, %f).", p->X(), p->Y());
    return;
  }
  if (max_photon.second>fPhotonThr*max.second)
    p->SetMCTrack(max_photon.first);
  else
    p->SetMCTrack(max.first);
  if (!fTree) return;
  fMCNum=p->MCTrack();
  tr=(CbmMCTrack*)fMCTracks->At(fMCNum);
  fPx=p->Px();
  fPy=p->Py();
  fPz=p->Pz();
  fRE=p->E();
  cls=(CbmEcalClusterV1*)fClusters->At(p->ClusterNum());
  fChi2=cls->Chi2();
  fNM=cls->Maxs();
  fNC=cls->Size();
  fPDG=tr->GetPdgCode();
  fMCE=tr->GetEnergy();
  fMCPx=tr->GetPx();
  fMCPy=tr->GetPy();
  fMCPz=tr->GetPz();
  fTree->Fill();
}

/** Form a precluster **/
void CbmEcalMatching::FormPreCluster(CbmEcalRecParticle* p)
{
  list<CbmEcalCell*> neib;
  CbmEcalCell* celli;
  Int_t i;
  CbmEcalClusterV1* cls=(CbmEcalClusterV1*)fClusters->At(p->ClusterNum());
  CbmEcalCell* cell;

  fPreCluster.clear();
  fClusterE=0;
  cell=fStr->GetHitCell(p->CellNum());
  cell->GetNeighborsList(0, neib);
  for(i=0;i<cls->Size();i++)
  {
    celli=fStr->GetHitCell(cls->CellNum(i));
    if (find(neib.begin(), neib.end(), celli)!=neib.end())
    {
      fPreCluster.push_back(celli);
      fClusterE+=celli->GetTotalEnergy();
    }
  }
  fPreCluster.push_back(cell);
  fClusterE+=cell->GetTotalEnergy();
}

/** Add energy to track and all its mothers **/
void CbmEcalMatching::AddTrackE(Int_t track, Double_t e)
{
  CbmMCTrack* tr;
  Int_t num;

  fE[track]+=e;

  tr=(CbmMCTrack*)fMCTracks->At(track);
  if (tr==NULL)
  {
    Warning("AddTrackE", "Can't find MCTrack number %d.", track);
    return;
  }
  num=tr->GetMotherId();
  if (num<0) return;
  tr=(CbmMCTrack*)fMCTracks->At(num);
  if (tr==NULL)
  {
    Warning("AddTrackE", "Can't find MCTrack number %d.", num);
    return;
  }
  if (tr->GetPdgCode()==22||TMath::Abs(tr->GetPdgCode())==11)
    AddTrackE(num, e);
}

/** Form a map of energies for precluster **/
void CbmEcalMatching::FormE()
{
  list<CbmEcalCell*>::const_iterator i;
  map<Int_t, Float_t>::const_iterator p;

  fE.clear();
  for(i=fPreCluster.begin();i!=fPreCluster.end();++i)
  {
    fCell=(*i);
    for(p=(*i)->GetTrackEnergyBegin();p!=(*i)->GetTrackEnergyEnd();++p)
      AddTrackE((*p).first, (*p).second);
    for(p=(*i)->GetTrackPSEnergyBegin();p!=(*i)->GetTrackPSEnergyEnd();++p)
      AddTrackE((*p).first, (*p).second);
  }
}

/** Match MCTrack and reconstructed particle **/
void CbmEcalMatching::Match(CbmEcalRecParticle* p)
{
  map<Int_t, Double_t>::const_iterator i;
  pair<Int_t, Double_t> max; max.first=-1111; max.second=0;
  pair<Int_t, Double_t> max_photon; max_photon.second=0;
  CbmMCTrack* tr;
  CbmEcalClusterV1* cls;

  for(i=fE.begin(); i!=fE.end();++i)
  {
    if ((*i).second/fClusterE<fThreshold) continue;
    if ((*i).second>max.second)
      max=(*i);
    tr=(CbmMCTrack*)fMCTracks->At((*i).first);
    if (tr==NULL)
    {
      Warning("Match", "Can't find MCTrack number %d.", (*i).first);
      return;
    }
    if (tr->GetPdgCode()!=22) continue;
    if ((*i).second>max_photon.second)
      max_photon=(*i);
  }
  if (max.first==-1111)
  {
    if (fVerbose>2) Info("Match", "Matching failed for reconstructed photon (%f, %f).", p->X(), p->Y());
    return;
  }
  if (max_photon.second>fPhotonThr*max.second)
    p->SetMCTrack(max_photon.first);
  else
    p->SetMCTrack(max.first);
  if (!fTree) return;
  fMCNum=p->MCTrack();
  tr=(CbmMCTrack*)fMCTracks->At(fMCNum);
  fPx=p->Px();
  fPy=p->Py();
  fPz=p->Pz();
  fRE=p->E();
  cls=(CbmEcalClusterV1*)fClusters->At(p->ClusterNum());
  fChi2=cls->Chi2();
  fNM=cls->Maxs();
  fNC=cls->Size();
  fPDG=tr->GetPdgCode();
  fMCE=tr->GetEnergy();
  fMCPx=tr->GetPx();
  fMCPy=tr->GetPy();
  fMCPz=tr->GetPz();
  fTree->Fill();
}

/** Initialization of output tree **/
void CbmEcalMatching::InitTree()
{
  if (fTree) return;
  fTree=new TTree("ecal_matching","Ecal matching tree");
  fTree->Branch("ev", &fEv, "ev/I");
  fTree->Branch("mcn", &fMCNum, "mcn/I");
  fTree->Branch("nm", &fNM, "nm/I");
  fTree->Branch("nc", &fNC, "nc/I");
  fTree->Branch("pdg", &fPDG, "PDG/I");
  fTree->Branch("chi2", &fChi2, "chi2/D");
  fTree->Branch("px", &fPx, "px/D");
  fTree->Branch("py", &fPy, "py/D");
  fTree->Branch("pz", &fPz, "pz/D");
  fTree->Branch("e", &fRE, "e/D");
  fTree->Branch("mcpx", &fMCPx, "mcpx/D");
  fTree->Branch("mcpy", &fMCPy, "mcpy/D");
  fTree->Branch("mcpz", &fMCPz, "mcpz/D");
  fTree->Branch("mce", &fMCE, "mce/D");
}

/** Finish task **/
void CbmEcalMatching::Finish()
{
  if (fTree) fTree->Write();
}

/** virtual destructor **/
CbmEcalMatching::~CbmEcalMatching()
{
  ;
}
  
/** Only to comply with framework **/
CbmEcalMatching::CbmEcalMatching()
{
  ;
}
ClassImp(CbmEcalMatching)
