#include "CbmEcalRecoSimple.h"

#include "TTree.h"
#include "TClonesArray.h"
#include "TFormula.h"

#include "CbmRootManager.h"
#include "CbmTrackParam.h"

#include "CbmEcalStructure.h"
#include "CbmEcalInf.h"
#include "CbmEcalSCurveLib.h"
#include "CbmEcalCell.h"
#include "CbmEcalRecParticle.h"
#include "CbmEcalClusterV1.h"
#include "CbmEcalShowerLib.h"
#include "CbmEcalCalibration.h"
#include "CbmEcalParam.h"

#include <iostream>

using namespace std;

void CbmEcalRecoSimple::Exec(Option_t* option)
{
  Int_t i;
  Int_t n=fClusters->GetEntriesFast();
  CbmEcalClusterV1* cluster;
  list<CbmEcalCell*>::const_iterator p;

  fN=0;
  fReco->Delete();
  if (fToTree&&fOutTree==NULL)
    CreateTree();
  fEventN++;
  if (fVerbose>0) 
    Info("Exec", "Event %d, %d clusters in event.", fEventN, n);
  for(i=0;i<n;i++)
  {
    fNOld=fN;
    cluster=(CbmEcalClusterV1*)fClusters->At(i);
    p=cluster->PeaksBegin();
    for(;p!=cluster->PeaksEnd();++p)
      Reco(*p, cluster);
    CalculateChi2(cluster);
  }
  if (fVerbose>1)
    Info("Exec", "%d photons reconstructed in calorimeter.", fN);
}

/** Calculate a chi2 for just reconstructed photons **/
void CbmEcalRecoSimple::CalculateChi2(CbmEcalClusterV1* cluster)
{
  if (!fShLib) return;
  
  list<CbmEcalCell*>::const_iterator cell=cluster->Begin();
  static Double_t module=fInf->GetModuleSize();
  Int_t type;
  Double_t de;
  Double_t x;
  Double_t y;
  Double_t theta;
  Double_t phi;
  Double_t cellsize;
  Double_t cx;
  Double_t cy;
  Double_t r;
  Double_t celle;
  CbmEcalCell* cl;
  Int_t p;
  CbmEcalRecParticle* part;
  Double_t clustere=0;
  Double_t clenergy;
  Double_t sigma2;
  Double_t cellerr;
  Double_t epred;
  Double_t sin4theta;
  Double_t chi2=0;
  Double_t emeas;


  for(;cell!=cluster->End();++cell)
    clustere+=(*cell)->GetTotalEnergy();
  clenergy=fCal->GetERough(clustere);

  for(cell=cluster->Begin();cell!=cluster->End();++cell)
  {
    type=(*cell)->GetType();
    cellsize=module/type;
    cellerr=0; celle=0;
    for(p=fNOld;p<fN;p++)
    {
      part=(CbmEcalRecParticle*)fReco->At(p);
      cl=part->Cell(); cx=cl->GetCenterX(); cy=cl->GetCenterY(); 
      x=(*cell)->GetCenterX(); x-=part->X(); // x-=cx; 
      y=(*cell)->GetCenterY(); y-=part->Y(); // y-=cy; 

      r=TMath::Sqrt(cx*cx+cy*cy);

      /** TODO: should be Z of the cell**/
      theta=TMath::ATan(r/fInf->GetZPos());
      theta*=TMath::RadToDeg();
      phi=TMath::ACos(cx/r)*TMath::RadToDeg();
      if (cy<0) phi=360.0-phi;

      celle+=fShLib->GetSumEThetaPhi(x, y, cellsize, part->E(), theta, phi);
    }
//    cout << "---> " << x << "	" << y << "	" << cellsize << "	" << part->E() << "	" << theta << "	" << phi << endl;
//    cout << "-> celle " << celle << endl;
    cx=(*cell)->GetCenterX(); cy=(*cell)->GetCenterY();
    r=TMath::Sqrt(cx*cx+cy*cy);
    theta=TMath::ATan(r/fInf->GetZPos());

    epred=fCal->GetEnergy(celle, *cell);
    emeas=fCal->GetEnergy((*cell)->GetTotalEnergy(), *cell);
//    if (epred>clenergy) epred=clenergy;
//    if (celle>0)
    {
      fSigma->SetParameter(0, clenergy);	//Ecluster
      fSigma->SetParameter(1, emeas);		//Emeas
      fSigma->SetParameter(2, epred);		//Epred
      fSigma->SetParameter(3, theta);		//Theta
      cellerr=fSigma->Eval(0);
      de=fCal->GetEnergy(celle, *cell)-fCal->GetEnergy((*cell)->GetTotalEnergy(), *cell);
//      cout << "->" << celle << "	" << de << "	" << cellerr << endl;
//      cellerr=1;
      de*=de; chi2+=de/cellerr;
    }
  }
  chi2/=cluster->Size();
  for(p=fNOld;p<fN;p++)
  {
    part=(CbmEcalRecParticle*)fReco->At(p);
    part->SetChi2(chi2);
  }
  cluster->fChi2=chi2;
//  cout << fNOld << "	" << fN << "	" << chi2 << endl;
}

/** Reconstruct photon from maximum **/
void CbmEcalRecoSimple::Reco(CbmEcalCell* cell, CbmEcalClusterV1* clstr)
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
  
//  fPSE=cell->GetPSEnergy();
  fPSE=0;
  for(p=cells.begin();p!=cells.end();++p)
    fPSE+=(*p)->GetPSEnergy();

  if (fCal->GetERough(fE2x2)<0.3) return; 
  fEReco=fCal->GetEnergy(fE2x2+fPSE, cell);
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
  new((*fReco)[fN++]) CbmEcalRecParticle(px, py, pz, fEReco, x, y, z, 22, -1111, clstr, cell, fType);
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
CbmEcalRecoSimple::CbmEcalRecoSimple(const char *name, const Int_t iVerbose, const char* configname)
  : CbmTask(name, iVerbose)
{
  fToTree=kFALSE;
  fOutTree=NULL;
  fMaximums=NULL;
  fExcluded=NULL;

  fRecoName="EcalReco";

  Int_t i;
  TString nm;

  fParNames[0]="Ecluster";
  fParNames[1]="Emeas";
  fParNames[2]="Epred";
  fParNames[3]="Theta";
  fParOffset=3;
  for(i=0;i<10;i++)
  {
    fParNames[i+fParOffset]="c";
    fParNames[i+fParOffset]+=i;
  }

  CbmEcalParam* par=new CbmEcalParam("RecoSimpleParam", configname);

  for(i=0;i<10;i++)
    fC[i]=-1111;
  for(i=0;i<10;i++)
  {
    fC[i]=par->GetDouble(fParNames[i+fParOffset]);
    if (fC[i]==-1111) break;
  }
  fSigmaFormula=par->GetString("sigma");
  for(i=0;i<10;i++)
  {
    if (fC[i]==-1111) break;
    fSigmaFormula.ReplaceAll(fParNames[i+fParOffset], par->GetString(fParNames[i+fParOffset]));
  }
  Info("Init", "Formula for sigma (with constants): %s. ", fSigmaFormula.Data());
  for(i=0;i<fParOffset;i++)
  {
    nm="["; nm+=i; nm+="]";
    fSigmaFormula.ReplaceAll(fParNames[i], nm);
  }
  Info("Init", "Used formula: %s.", fSigmaFormula.Data());
  fSigma=new TFormula("RecoSimpleFormula", fSigmaFormula.Data());
  delete par;
}

CbmEcalRecoSimple::~CbmEcalRecoSimple()
{
  fReco->Delete();
  delete fReco;
}

void CbmEcalRecoSimple::Finish()
{
  if (fOutTree) fOutTree->Write();
}

/** Init **/
InitStatus CbmEcalRecoSimple::Init()
{
  fEventN=0;
  CbmRootManager* io=CbmRootManager::Instance();
  if (!io)
  {
    Fatal("Init", "Can't find IOManager.");
    return kFATAL;
  }
  fStr=(CbmEcalStructure*)io->GetObject("EcalStructure");
  if (!fStr) 
  {
    Fatal("Init()", "Can't find calorimeter structure in the system.");
    return kFATAL;
  }
  fInf=fStr->GetEcalInf();
  fTracks=(TClonesArray*)io->GetObject("EcalTrackParam");
  if (!fTracks)
  {
    Fatal("Init", "Can't find EcalTrackParam.");
    return kFATAL;
  }
  fClusters=(TClonesArray*)io->GetObject("EcalClusters");
  if (!fClusters)
  {
    Fatal("Init", "Can't find EcalClusters");
    return kFATAL;
  }
  fCal=(CbmEcalCalibration*)io->GetObject("EcalCalibration");
  if (!fCal)
  {
    Fatal("Init", "Can't find EcalCalibration");
    return kFATAL;
  }

  fLib=(CbmEcalSCurveLib*)io->GetObject("EcalSCurveLib");
  if (!fLib)
  {
    Fatal("Init", "Can't find EcalSCurveLib");
    return kFATAL;
  }

  fShLib=(CbmEcalShowerLib*)io->GetObject("EcalShowerLib");
  if (!fShLib)
  {
    Info("Init", "No shower library found in system. Will continue without chi2 calculation.");
  }
  fReco=new TClonesArray("CbmEcalRecParticle", 2000);
  io->Register(fRecoName, "ECAL", fReco, kFALSE);
  return kSUCCESS;
}

ClassImp(CbmEcalRecoSimple)
