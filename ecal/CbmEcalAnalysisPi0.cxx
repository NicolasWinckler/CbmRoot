#include "CbmEcalAnalysisPi0.h"

#include "CbmRootManager.h"
#include "CbmMCTrack.h"
#include "CbmTrackParam.h"

#include "CbmEcalRecParticle.h"
#include "CbmEcalCell.h"
#include "CbmEcalClusterV1.h"
#include "CbmEcalStructure.h"

#include "TTree.h"
#include "TClonesArray.h"
#include "TLorentzVector.h"

#include <iostream>

using std::cout;
using std::cerr;
using std::endl;
using std::list;

/** Loop procedure **/
void CbmEcalAnalysisPi0::Exec(Option_t* option)
{
  Int_t i;
  Int_t j;
  Int_t n=fReco->GetEntriesFast();
  Int_t trn=fTracks->GetEntriesFast();
  CbmEcalRecParticle* p1;
  CbmEcalRecParticle* p2;
  CbmMCTrack* tr1;
  CbmMCTrack* tr2;
  CbmMCTrack* mtr;
  CbmTrackParam* trp;
  CbmEcalClusterV1* cls;
  CbmEcalCell* cell;
  TLorentzVector v1;
  TLorentzVector v2;
  TLorentzVector p;
  Double_t r;
  Int_t mother;
  
  fEvent++;
  fEntries=0;
  if (fVerbose>0)
    Info("Exec", "Event %d.", fEvent);
  InitTree();

  for(i=0;i<n;i++)
  {
    p1=(CbmEcalRecParticle*)fReco->At(i);
    if (p1->MCTrack()==-1111) continue;
    tr1=(CbmMCTrack*)fMC->At(p1->MCTrack());
    mother=tr1->GetMotherId();
    if (mother<0) continue;
    for(j=i+1;j<n;j++)
    {
      p2=(CbmEcalRecParticle*)fReco->At(j);
      if (p2->MCTrack()==-1111) continue;
      if (p2->MCTrack()==p1->MCTrack()&&fVerbose>1)
      {
	Info("Exec", "Two reconstructed partiles belong to the same MC track %d.", p1->MCTrack());
	continue;
      }
      tr2=(CbmMCTrack*)fMC->At(p2->MCTrack());
      if (mother==tr2->GetMotherId()) break;
    }
    if (j==n) continue;
    mtr=(CbmMCTrack*)fMC->At(mother);

    fMotherID=mtr->GetPdgCode();
    fMX=mtr->GetStartX();
    fMY=mtr->GetStartY();
    fMZ=mtr->GetStartZ();
    fMPx=mtr->GetPx();
    fMPy=mtr->GetPy();
    fMPz=mtr->GetPz();
    fME=mtr->GetEnergy();

    fE1=p1->E(); fPx1=p1->Px(); fPy1=p1->Py(); fPz1=p1->Pz();
    cls=(CbmEcalClusterV1*)fClusters->At(p1->ClusterNum());
    fX1=p1->X(); fY1=p1->Y(); fChi21=cls->Chi2();
    cell=fStr->GetHitCell(p1->CellNum());
    fCellX1=cell->GetCenterX(); fCellY1=cell->GetCenterY();
    fCellType1=cell->GetType();
    fMCE1=tr1->GetEnergy(); fMCPx1=tr1->GetPx(); fMCPy1=tr1->GetPy(); fMCPz1=tr1->GetPz();
    fPdg1=tr1->GetPdgCode();
    fR1=1111;
    for(j=0;j<trn;j++)
    {
      trp=(CbmTrackParam*)fTracks->At(j);
      r=(fX1-trp->GetX())*(fX1-trp->GetX())+(fY1-trp->GetY())*(fY1-trp->GetY());
      r=TMath::Sqrt(r);
      if (r<fR1) fR1=r;
    }
    v1.SetPxPyPzE(fPx1, fPy1, fPz1, fE1);

    fE2=p2->E(); fPx2=p2->Px(); fPy2=p2->Py(); fPz2=p2->Pz();
    cls=(CbmEcalClusterV1*)fClusters->At(p2->ClusterNum());
    fX2=p2->X(); fY2=p2->Y(); fChi22=cls->Chi2();
    cell=fStr->GetHitCell(p2->CellNum());
    fCellX2=cell->GetCenterX(); fCellY2=cell->GetCenterY();
    fCellType2=cell->GetType();
    fMCE2=tr2->GetEnergy(); fMCPx2=tr2->GetPx(); fMCPy2=tr2->GetPy(); fMCPz2=tr2->GetPz();
    fPdg2=tr2->GetPdgCode();
    fR2=1111;
    for(j=0;j<trn;j++)
    {
      trp=(CbmTrackParam*)fTracks->At(j);
      r=(fX2-trp->GetX())*(fX2-trp->GetX())+(fY2-trp->GetY())*(fY2-trp->GetY());
      r=TMath::Sqrt(r);
      if (r<fR2) fR2=r;
    }
    v2.SetPxPyPzE(fPx2, fPy2, fPz2, fE2);

    p=v1; p+=v2;
    fM=p.M();

    v1.SetPxPyPzE(fMCPx1, fMCPy1, fMCPz1, fMCE1);
    v2.SetPxPyPzE(fMCPx2, fMCPy2, fMCPz2, fMCE2);
    p=v1; p+=v2;
    fMCM=p.M();
    fEntries++;
    fTree->Fill();
  }
  if (fVerbose>0)
    Info("Exec", "%d found particles in event.", fEntries);
}


/** Initializes tree **/
void CbmEcalAnalysisPi0::InitTree()
{
  if (fTree) return;
  fTree=new TTree("Pi0Tree","Pi0 and eta tree");
  fTree->Branch("ev",&fEvent,"ev/I");
  fTree->Branch("mother",&fMotherID,"mother/I");
  fTree->Branch("m",&fM,"m/D");
  fTree->Branch("mx",&fMX,"mx/D");
  fTree->Branch("my",&fMY,"my/D");
  fTree->Branch("mz",&fMZ,"mz/D");
  fTree->Branch("mpx",&fMPx,"mpx/D");
  fTree->Branch("mpy",&fMPy,"mpy/D");
  fTree->Branch("mpz",&fMPz,"mpz/D");
  fTree->Branch("me",&fME,"me/D");
  fTree->Branch("mcm",&fMCM,"mcm/D");

  fTree->Branch("e1",&fE1,"e1/D");
  fTree->Branch("px1",&fPx1,"px1/D");
  fTree->Branch("py1",&fPy1,"py1/D");
  fTree->Branch("pz1",&fPz1,"pz1/D");
  fTree->Branch("mce1",&fMCE1,"mce1/D");
  fTree->Branch("mcpx1",&fMCPx1,"mcpx1/D");
  fTree->Branch("mcpy1",&fMCPy1,"mcpy1/D");
  fTree->Branch("mcpz1",&fMCPz1,"mcpz1/D");
  fTree->Branch("x1",&fX1,"x1/D");
  fTree->Branch("y1",&fY1,"y1/D");
  fTree->Branch("r1",&fR1,"r1/D");
  fTree->Branch("cx1",&fCellX1,"cx1/D");
  fTree->Branch("cy1",&fCellY1,"cy1/D");
  fTree->Branch("chi21",&fChi21,"chi21/D");
  fTree->Branch("ctype1",&fCellType1,"ctype1/I");
  fTree->Branch("pdg1",&fPdg1,"pdg1/I");

  fTree->Branch("e2",&fE2,"e2/D");
  fTree->Branch("px2",&fPx2,"px2/D");
  fTree->Branch("py2",&fPy2,"py2/D");
  fTree->Branch("pz2",&fPz2,"pz2/D");
  fTree->Branch("mce2",&fMCE2,"mce2/D");
  fTree->Branch("mcpx2",&fMCPx2,"mcpx2/D");
  fTree->Branch("mcpy2",&fMCPy2,"mcpy2/D");
  fTree->Branch("mcpz2",&fMCPz2,"mcpz2/D");
  fTree->Branch("x2",&fX2,"x2/D");
  fTree->Branch("y2",&fY2,"y2/D");
  fTree->Branch("r2",&fR2,"r2/D");
  fTree->Branch("cx2",&fCellX2,"cx2/D");
  fTree->Branch("cy2",&fCellY2,"cy2/D");
  fTree->Branch("chi22",&fChi22,"chi22/D");
  fTree->Branch("ctype2",&fCellType2,"ctype2/I");
  fTree->Branch("pdg2",&fPdg2,"pdg2/I");
}

CbmEcalAnalysisPi0::CbmEcalAnalysisPi0(const char* name, const Int_t iVerbose)
  : CbmTask(name, iVerbose)
{
  fTree=NULL;
}

/** Initing routine **/
InitStatus CbmEcalAnalysisPi0::Init()
{
  CbmRootManager* fManager=CbmRootManager::Instance();
  if (!fManager)
  {
    Fatal("Init", "Can't find a Root Manager.");
    return kFATAL;
  }
  fReco=(TClonesArray*)fManager->GetObject("EcalReco");
  if (!fReco)
  {
    Fatal("Init", "Can't find an array of reco parcles.");
    return kFATAL;
  }
  fMC=(TClonesArray*)fManager->GetObject("MCTrack");
  if (!fMC)
  {
    Fatal("Init", "Can't find an array of MC tracks.");
    return kFATAL;
  }
  fTracks=(TClonesArray*)fManager->GetObject("EcalTrackParam");
  if (!fTracks)
  {
    Fatal("Init", "Can't find array of reconstructed tracks");
    return kFATAL;
  }
  fStr=(CbmEcalStructure*)fManager->GetObject("EcalStructure");
  if (!fStr)
  {
    Fatal("Init", "Can't find calorimeter structure in the system");
    return kFATAL;
  }
  fClusters=(TClonesArray*)fManager->GetObject("EcalClusters");
  if (!fClusters)
  {
    Fatal("Init", "Can't find array of clusters");
    return kFATAL;
  }
  fTree=NULL;

  return kSUCCESS;
}

/** Finishing routine **/
void CbmEcalAnalysisPi0::Finish()
{
  if (fTree)
    fTree->Write();
}

ClassImp(CbmEcalAnalysisPi0)
