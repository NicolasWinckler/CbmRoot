#include "CbmEcalRecoSimple.h"

#include "TTree.h"
#include "TChain.h"
#include "TClonesArray.h"

#include "CbmRootManager.h"
#include "CbmTrackParam.h"

#include "CbmEcalStructure.h"
#include "CbmEcalInf.h"
#include "CbmEcalSCurveLib.h"
#include "CbmEcalCell.h"
#include "CbmEcalRecParticle.h"
#include "CbmEcalClusterV1.h"

#include <iostream>

using namespace std;

void CbmEcalRecoSimple::Exec(Option_t* option)
{
  Int_t i;

  fN=0;
  fReco->Delete();
  fClusters->Delete();
  if (fToTree&&fOutTree==NULL)
    CreateTree();
  fEventN++;
  if (fVerbose>0) 
    Info("Exec", "Event %d .", fEventN);
  if (fChain) ImportTracks();
  ExcludeMaximums();
  FindMaximums();
  for(i=0;fMaximums[i]!=NULL;i++)
    Reco(fMaximums[i]);
  if (fVerbose>1)
    Info("Exec", "%d photons reconstructed in calorimeter.", fN);
}

void CbmEcalRecoSimple::FindMaximums()
{
  list<CbmEcalCell*> cells;
  list<CbmEcalCell*>::const_iterator p;
  list<CbmEcalCell*>::const_iterator p1;
  list<CbmEcalCell*> neib;
  Int_t n=0;
  Int_t i;
  Double_t e;

  fStr->GetCells(cells);
  p=cells.begin();
  for(;p!=cells.end();++p)
  {
    e=(*p)->GetTotalEnergy();
    (*p)->GetNeighborsList(0, neib);
    p1=neib.begin();
    for(;p1!=neib.end();++p1)
      if ((*p1)->GetTotalEnergy()>=e) break;
    if (p1==neib.end())
    {
      for(i=0;fExcluded[i]!=NULL;i++)
	if (fExcluded[i]==(*p)) break;
      if (fExcluded[i]==NULL) fMaximums[n++]=(*p);
    }
  }
  if (fVerbose>1)
    Info("FindMaximums", "%d local maximums found in calorimeter.", n);
  fMaximums[n++]=NULL;
}

/** Returns incoming particle energy **/
/** TODO: Need class with calibration! **/
Double_t CbmEcalRecoSimple::GetEnergy(Double_t e2, CbmEcalCell* cell)
{
  Int_t type=cell->GetType();
  Double_t x=cell->GetCenterX();
  Double_t y=cell->GetCenterY();
  Double_t tantheta=TMath::Sqrt(x*x+y*y)/fInf->GetZPos();
  Double_t a=fP0a[type]+fP1a[type]*tantheta;
  Double_t b=fP0b[type]+fP1b[type]*tantheta;
  Double_t d=TMath::Sqrt(a*a+4.0*e2*b);
  Double_t res=(-a+d)/2.0/b;
  return res*res;
}

  /** Reconstruct photon from maximum **/
void CbmEcalRecoSimple::Reco(CbmEcalCell* cell)
{
  Double_t x;
  Double_t y;
  Double_t z;
  Double_t amp;
  Double_t en;
  Double_t maxe;
  Double_t px;
  Double_t py;
  Double_t pz;
  Int_t type;
  list<CbmEcalCell*> cells;
  list<CbmEcalCell*>::const_iterator p;
  Int_t cnum;

  cells.clear();

  fType=cell->GetType();
  fDiffType=0;

  cell->GetNeighborsList(0,cells);
  fE=cell->GetEnergy();
  en=cell->GetEnergy();
  type=cell->GetType();
  fAX=0; fAY=0;
  for(p=cells.begin();p!=cells.end();++p) 
  {
    if (type!=(*p)->GetType())
    {
      fAX=-1111;
      fAY=-1111;
      type=-1111;
      fDiffType=1;
    } else
    {
      if (cell->GetCenterX()+0.001<(*p)->GetCenterX())
	fAX+=(*p)->GetEnergy();
      else
      if (cell->GetCenterX()-0.001>(*p)->GetCenterX())
	fAX-=(*p)->GetEnergy();
      if (cell->GetCenterY()+0.001<(*p)->GetCenterY())
	fAY+=(*p)->GetEnergy();
      else
      if (cell->GetCenterY()-0.001>(*p)->GetCenterY())
	fAY-=(*p)->GetEnergy();
    }
    en+=(*p)->GetEnergy();
  }
  fE3x3=en;

  if (fAX!=-1111)
  {
    fAX/=fE3x3;
    fAY/=fE3x3;
  }

  maxe=0;
  for(Int_t i=1;i<5;i++)
  {
    cell->GetNeighborsList(i,cells);
    en=cell->GetEnergy();
    for(p=cells.begin();p!=cells.end();++p) 
      en+=(*p)->GetEnergy();
    if (maxe<en)
    {
      maxe=en;
      cnum=i;
    }
  }
  cell->GetNeighborsList(cnum, cells);
  cells.push_back(cell);
  fE2x2=maxe;

  fCellX=cell->GetCenterX();
  fCellY=cell->GetCenterY();
  
  fPSE=cell->GetPSEnergy();

  if (fE2x2<0.05) return; 
  fEReco=GetEnergy(fE2x2+fPSE, cell);
  if (fAX!=-1111)
    fXReco=fLib->GetX(fAX, fEReco, cell);
  else
    fXReco=-1111;
  if (fXReco!=-1111)
    fXReco+=cell->GetCenterX();
  
  if (fAY!=-1111)
    fYReco=fLib->GetY(fAY, fEReco, cell);
  else
    fYReco=-1111;
  
  if (fYReco!=-1111)
    fYReco+=cell->GetCenterY();
  if (fOutTree) fOutTree->Fill();
  if (fXReco!=-1111)
    x=fXReco;
  else
    x=cell->GetCenterX();
  if (fYReco!=-1111)
    y=fYReco;
  else
    y=cell->GetCenterY();
  z=fInf->GetZPos();
  amp=TMath::Sqrt(x*x+y*y+z*z);
  px=fEReco*x/amp;
  py=fEReco*y/amp;
  pz=fEReco*z/amp;
  CbmEcalClusterV1* cluster=new((*fClusters)[fN]) CbmEcalClusterV1(fN, cells); 
  new((*fReco)[fN++]) CbmEcalRecParticle(px, py, pz, fEReco, x, y, z, 22, -1111, cluster, fType);
}

void CbmEcalRecoSimple::ExcludeMaximums()
{
  Int_t n=fTracks->GetEntriesFast();
  Int_t i;
  Int_t mn=0;
  CbmTrackParam* tr;
  CbmEcalCell* cell;
  list<CbmEcalCell*> cells;
  list<CbmEcalCell*>::const_iterator p;
  CbmEcalCell* maxcell;
  Double_t e;

  if (fVerbose>1) 
    Info("ExcludeMaximums", "Find %d charged tracks in event.", n);
  for(i=0;i<n;i++)
  {
    tr=(CbmTrackParam*)fTracks->At(i);
    cell=fStr->GetCell(tr->GetX(), tr->GetY());
    if (cell==NULL) continue;
    cell->GetNeighborsList(0, cells);
    e=cell->GetTotalEnergy();
    maxcell=cell;
    for(p=cells.begin();p!=cells.end();++p)
      if ((*p)->GetTotalEnergy()>e)
      {
	e=(*p)->GetTotalEnergy();
	maxcell=(*p);
      }
    if (cell==maxcell)
    {
      fExcluded[mn++]=cell;
      continue;
    }
    cell->GetNeighborsList(0, cells);
    e=cell->GetTotalEnergy();
    for(p=cells.begin();p!=cells.end();++p)
      if ((*p)->GetTotalEnergy()>e) break;
    if (p==cells.end())
      fExcluded[mn++]=(*p);
  }
  fExcluded[mn]=NULL;
}

void CbmEcalRecoSimple::ImportTracks()
{
  fTracks->Delete();

  Int_t nTr=0;
  TMatrixFSym mat(5);
  if (fChEntry>=fChEntries)
  {
    Warning("ImportTracks", "No tracks found in file for event %d!", fEventN);
    return;
  }
  while(fTrEv<fEventN)
  {
    /** TODO: In general this is not correct is case of inclined calorimeter *
     ** Should be corrected to fStr->GetCell(fTrX, fTrY)->GetCenterZ() for
     ** example. **/
    new ((*fTracks)[nTr++]) CbmTrackParam(fTrX, fTrY, fStr->GetEcalInf()->GetZPos(), fTrTx, fTrTy, fTrQp, mat);
    if (fChEntry<fChEntries)
      fChain->GetEntry(fChEntry++);
    else
      break;
  }
}

void CbmEcalRecoSimple::CreateTree()
{
  fOutTree=new TTree("simple_reco", "Reconstruction by simple methods");
  fOutTree->Branch("ev", &fEventN, "ev/I");
  fOutTree->Branch("type", &fType, "type/S");
  fOutTree->Branch("difftype", &fDiffType, "difftype/S");
  fOutTree->Branch("cellx", &fCellX, "cellx/D");
  fOutTree->Branch("celly", &fCellY, "celly/D");
  fOutTree->Branch("x", &fXReco, "x/D");
  fOutTree->Branch("y", &fYReco, "y/D");
  fOutTree->Branch("e", &fEReco, "e/D");
  fOutTree->Branch("ecell", &fE, "ecell/D");
  fOutTree->Branch("e2", &fE2x2, "e2/D");
  fOutTree->Branch("e3", &fE3x3, "e3/D");
  fOutTree->Branch("ax", &fAX, "ax/D");
  fOutTree->Branch("ay", &fAY, "ay/D");
  fOutTree->Branch("pse", &fPSE, "pse/D");
}

/** Default constructor. Requirement of ROOT system **/
CbmEcalRecoSimple::CbmEcalRecoSimple() 
  : CbmTask()
{
  ;
}

/** Standard constructor **/
CbmEcalRecoSimple::CbmEcalRecoSimple(const char *name, const Int_t iVerbose)
  : CbmTask(name, iVerbose)
{
  fTracksFileName="";
  fChain=NULL;
  fToTree=kFALSE;
  fOutTree=NULL;
  fChEntry=1;
  fMaximums=NULL;
  fExcluded=NULL;

  fRecoName="EcalReco";

  fLib=new CbmEcalSCurveLib(0);
  fLib->AddFile("3x3.root");
  fLib->AddFile("6x6.root");
  fLib->AddFile("12x12.root");

  fP0a[1]=0.002833;
  fP0a[2]=0.003345;
  fP0a[4]=0.003587;
  fP0b[1]=0.08788;
  fP0b[2]=0.08496;
  fP0b[4]=0.07886;

  fP1a[1]=0.003398;
  fP1a[2]=0.003108;
  fP1a[4]=0.001337;
  fP1b[1]=-0.005302;
  fP1b[2]=-0.005155;
  fP1b[4]=-0.006937;
}

CbmEcalRecoSimple::~CbmEcalRecoSimple()
{
  if (fTracksFileName!="") 
  {
    fTracks->Delete();
    delete fTracks;
  }
  fReco->Delete();
  fClusters->Delete();
  delete fClusters;
  delete fReco;
  delete fMaximums;
  delete fExcluded;
}

void CbmEcalRecoSimple::Finish()
{
  if (fOutTree) fOutTree->Write();
}

/** Init **/
InitStatus CbmEcalRecoSimple::Init()
{
  fEventN=0;
  fMaximums=new CbmEcalCell*[5000];
  fExcluded=new CbmEcalCell*[2000];
  CbmRootManager* io=CbmRootManager::Instance();
  if (!io)
  {
    Fatal("Init", "Can't find IOManager.");
    return kFATAL;
  }
  fStr=(CbmEcalStructure*)io->GetObject("EcalStructure");
  fInf=fStr->GetEcalInf();
  if (!fStr) 
  {
    Fatal("Init()", "Can't find calorimeter structure in the system.");
    return kFATAL;
  }
  if (fTracksFileName!="")
  {
    // Read tracks from file
    fChain=new TChain("ecaltracks");
    fChain->AddFile(fTracksFileName);
    fChain->SetBranchAddress("ev", &fTrEv);
    fChain->SetBranchAddress("x", &fTrX);
    fChain->SetBranchAddress("y", &fTrY);
    fChain->SetBranchAddress("tx", &fTrTx);
    fChain->SetBranchAddress("ty", &fTrTy);
    fChain->SetBranchAddress("qp", &fTrQp);
    fChEntries=fChain->GetEntries();
    fTracks=new TClonesArray("CbmTrackParam", 1000);
    TClonesArray* tr=(TClonesArray*)io->GetObject("EcalTrackParam");
    if (tr==NULL)
      io->Register("EcalTrackParam", "ECAL", fTracks, kFALSE);
    fChain->GetEntry(0);
  } else
  {
    fTracks=(TClonesArray*)io->GetObject("EcalTrackParam");
    if (!fTracks)
    {
      Fatal("Init", "Can't find EcalTrackParam.");
      return kFATAL;
    }
  }
  fReco=new TClonesArray("CbmEcalRecParticle", 2000);
  io->Register(fRecoName, "ECAL", fReco, kFALSE);
  fClusters=new TClonesArray("CbmEcalClusterV1", 2000);
  return kSUCCESS;
}

ClassImp(CbmEcalRecoSimple)
