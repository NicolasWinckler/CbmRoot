#include "CbmEcalRecoSlow.h"

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

void FCNEcalCluster::SetCluster(CbmEcalClusterV1* cluster)
{
  fCluster=cluster;
  SetN(fCluster->Maxs());
  fClusterResp=fCluster->Energy();
  fClusterEnergy=fCal->GetERough(fClusterResp);
}

void FCNEcalCluster::SetN(Int_t n)
{
  Int_t i;
  fN=n;
}

vector<Double_t> FCNEcalCluster::Gradient(const vector<Double_t>& par) const
{
  Int_t i;
  static vector<Double_t> grad;
  static vector<Double_t> celle_gr;

//  if (par.size()!=fN*3)
//    Fatal("Gradient", "Size of input vector is wrong (%d, should be %d)!", par.size(), fN*3);

  if (grad.size()!=fN*3) grad.resize(fN*3);
  if (celle_gr.size()<fN*3) celle_gr.resize(fN*3);

  /** A copy of chi2 almost **/
  list<CbmEcalCell*>::const_iterator cell=fCluster->Begin();
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
  Double_t clustere=fClusterResp;
  Double_t clenergy;
  Double_t sigma2;
  Double_t cellerr;
  Double_t epred;
  Double_t sin4theta;
  Double_t chi2=0;
  Double_t emeas;


  clenergy=fClusterEnergy;

  for(i=0;i<fN*3;i++)
    grad[i]=0;

  for(cell=fCluster->Begin();cell!=fCluster->End();++cell)
  {
    type=(*cell)->GetType();
    cellsize=module/type;
    cellerr=0; celle=0;
    for(i=0;i<fN;i++)
    {
      cl=fCells[i]; cx=cl->GetCenterX(); cy=cl->GetCenterY(); 
      x=(*cell)->GetCenterX(); x-=par[3*i+1]; //part->X(); // x-=cx; 
      y=(*cell)->GetCenterY(); y-=par[3*i+2]; //part->Y(); // y-=cy; 

      r=TMath::Sqrt(cx*cx+cy*cy);

      /** TODO: should be Z of the cell**/
      theta=TMath::ATan(r/fInf->GetZPos());
      theta*=TMath::RadToDeg();
      phi=TMath::ACos(cx/r)*TMath::RadToDeg();
      if (cy<0) phi=360.0-phi;

      celle+=fShLib->GetSumEThetaPhi(x, y, cellsize, par[3*i], theta, phi);
      celle_gr[i*3]=fShLib->GetGradE()*fEStep;
      celle_gr[i*3+1]=-fShLib->GetGradX()*fCStep;
      celle_gr[i*3+2]=-fShLib->GetGradX()*fCStep;
    }
    cx=(*cell)->GetCenterX(); cy=(*cell)->GetCenterY();
    r=TMath::Sqrt(cx*cx+cy*cy);
    theta=TMath::ATan(r/fInf->GetZPos());

    epred=fCal->GetEnergy(celle, *cell);
    emeas=fCal->GetEnergy((*cell)->GetTotalEnergy(), *cell);
    {
      fSigma->SetParameter(0, clenergy);	//Ecluster
      fSigma->SetParameter(1, emeas);		//Emeas
      fSigma->SetParameter(2, epred);		//Epred
      fSigma->SetParameter(3, theta);		//Theta
      cellerr=fSigma->Eval(0);
      de=fCal->GetEnergy(celle, *cell)-fCal->GetEnergy((*cell)->GetTotalEnergy(), *cell);
      de*=de; chi2+=de/cellerr;
    }
    for(i=0;i<fN*3;i++)
    {
      celle_gr[i]+=celle;
      epred=fCal->GetEnergy(celle_gr[i], *cell);
      fSigma->SetParameter(2, epred);		//Epred
      cellerr=fSigma->Eval(0);
      de=fCal->GetEnergy(celle, *cell)-fCal->GetEnergy((*cell)->GetTotalEnergy(), *cell);
      de*=de; grad[i]+=de/cellerr;
    }
  }
  
  for(i=0;i<fN*3;i++)
    grad[i]-=chi2;

  for(i=0;i<fN;i++)
  {
    grad[i*3]/=fEStep;
    grad[i*3+1]/=fCStep;
    grad[i*3+2]/=fCStep;
  }

  return grad;
}

Double_t FCNEcalCluster::operator()(const vector<Double_t>& par) const
{
//  if (par.size()!=fN*3)
//    Fatal("()", "Size of input vector is wrong (%d, should be %d)!", par.size(), fN*3);

  list<CbmEcalCell*>::const_iterator cell=fCluster->Begin();
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
  Int_t i;
  CbmEcalRecParticle* part;
  Double_t clustere=fClusterResp;
  Double_t clenergy;
  Double_t sigma2;
  Double_t cellerr;
  Double_t epred;
  Double_t sin4theta;
  Double_t chi2=0;
  Double_t emeas;


  clenergy=fClusterEnergy;

  for(cell=fCluster->Begin();cell!=fCluster->End();++cell)
  {
    type=(*cell)->GetType();
    cellsize=module/type;
    cellerr=0; celle=0;
    for(i=0;i<fN;i++)
    {
      cl=fCells[i]; cx=cl->GetCenterX(); cy=cl->GetCenterY(); 
      x=(*cell)->GetCenterX(); x-=par[3*i+1]; //part->X(); // x-=cx; 
      y=(*cell)->GetCenterY(); y-=par[3*i+2]; //part->Y(); // y-=cy; 

      r=TMath::Sqrt(cx*cx+cy*cy);

      /** TODO: should be Z of the cell**/
      theta=TMath::ATan(r/fInf->GetZPos());
      theta*=TMath::RadToDeg();
      phi=TMath::ACos(cx/r)*TMath::RadToDeg();
      if (cy<0) phi=360.0-phi;

      celle+=fShLib->GetSumEThetaPhi(x, y, cellsize, par[3*i], theta, phi);
//      celle+=fShLib->GetSumEThetaPhi(x, y, cellsize, part->E(), theta, phi);
    }
    cx=(*cell)->GetCenterX(); cy=(*cell)->GetCenterY();
    r=TMath::Sqrt(cx*cx+cy*cy);
    theta=TMath::ATan(r/fInf->GetZPos());

    epred=fCal->GetEnergy(celle, *cell);
    emeas=fCal->GetEnergy((*cell)->GetTotalEnergy(), *cell);
    {
      fSigma->SetParameter(0, clenergy);	//Ecluster
      fSigma->SetParameter(1, emeas);		//Emeas
      fSigma->SetParameter(2, epred);		//Epred
      fSigma->SetParameter(3, theta);		//Theta
      cellerr=fSigma->Eval(0);
      de=fCal->GetEnergy(celle, *cell)-fCal->GetEnergy((*cell)->GetTotalEnergy(), *cell);
      de*=de; chi2+=de/cellerr;
    }
  }
  chi2/=fCluster->Size();
  return chi2;
}

/***************************************************************************/

void CbmEcalRecoSlow::Exec(Option_t* option)
{
  Int_t i;
  Int_t n=fClusters->GetEntriesFast();
  CbmEcalClusterV1* cluster;
  list<CbmEcalCell*>::const_iterator p;
  Double_t chi2;

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
    chi2=CalculateChi2(cluster);
    if (chi2>fChi2Th)
      FitCluster(cluster);
    if (fStoreClusterInfo)
      WriteClusterInfo(cluster);
    if (fOutTree)
      FillTree();
  }
  if (fVerbose>1)
    Info("Exec", "%d photons reconstructed in calorimeter.", fN);
}

/** Write a cluster info **/
void CbmEcalRecoSlow::WriteClusterInfo(CbmEcalClusterV1* clstr)
{
  list<CbmEcalCell*>::const_iterator p;  
  list<CbmEcalCell*>::const_iterator cell;
  CbmEcalRecParticle* part;
  Int_t i=0;
  Int_t j;
  Double_t celle;
  Double_t cellsize;
  Double_t e[3];
  Double_t x;
  Double_t y;
  Double_t r;
  Double_t theta;
  Double_t phi;
  CbmEcalCell* cl;
  Double_t cx;
  Double_t cy;
  static Double_t module=fInf->GetModuleSize();

  fECluster=fCal->GetEnergy(clstr->Energy(), *(clstr->PeaksBegin()));
  for(p=clstr->Begin();p!=clstr->End();++p)
  {
    e[i++]=(*p)->GetTotalEnergy();
    if (i==3) break;
  }
  fECls=e[0]/clstr->Energy();
  fE2Cls=(e[0]+e[1])/clstr->Energy();
  fE3Cls=(e[0]+e[1]+e[2])/clstr->Energy();
  i=0;

  for(cell=clstr->Begin();cell!=clstr->End();++cell)
  {
    fTypes[i]=(*cell)->GetType();
    cellsize=module/fTypes[i];
    celle=0;
    for(j=fNOld;j<fN;j++)
    {
      part=(CbmEcalRecParticle*)fReco->At(j);
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
    fEpred[i]=celle;
    fEmeas[i]=(*cell)->GetTotalEnergy();
    fCX[i]=(*cell)->GetCenterX();
    fCY[i]=(*cell)->GetCenterY();
    i++;
  }
  for(j=i;j<20;j++)
    fTypes[j]=-1111;
}

void CbmEcalRecoSlow::FillTree()
{
  Int_t n=fN-fNOld;
  CbmEcalRecParticle* p;
  Int_t i;

  for(i=0;i<n;i++)
  {
    p=(CbmEcalRecParticle*)fReco->At(i+fNOld);
    fChi2=p->Chi2();
    fEReco=p->E();
    fXReco=p->X();
    fYReco=p->Y();
    fOutTree->Fill();
  }

}

/** Fit a given cluster. A first approximation should be available **/
void CbmEcalRecoSlow::FitCluster(CbmEcalClusterV1* clstr)
{
  vector<CbmEcalCell*> cells(fN-fNOld);
  Int_t i;
  Int_t n=fN-fNOld;
  Int_t ret;
  Double_t oldchi2=clstr->Chi2();
  CbmEcalRecParticle* p;
  TString par;

  if (n>6)
  {
    Info("FitCluster", "Give up fitting the cluster with %d maximums. ", n);
    return;
  }
  fFCN->SetCluster(clstr);
  fFitter->Clear();
  fFitter->SetPrintLevel(-1);
  fFitter->SetMaxIterations(fMaxIterations);
  for(i=0;i<n;i++)
  {
    p=(CbmEcalRecParticle*)fReco->At(i+fNOld);
    cells[i]=p->Cell();
    par="e"; par+=i; fFitter->SetParameter(i*3   ,par.Data(), p->E(), 0.01*p->E(), p->E()*0.2, p->E()*2.0);
    par="x"; par+=i; fFitter->SetParameter(i*3+1 ,par.Data(), p->X(), 0.1 ,p->X()-12., p->X()+12.);
    par="y"; par+=i; fFitter->SetParameter(i*3+2 ,par.Data(), p->Y(), 0.1 ,p->Y()-12., p->Y()+12.);
  }
  fFCN->SetParticleSeeds(cells);
  fFitter->CreateMinimizer();
  ret=fFitter->Minimize();

  Double_t chi2;
  Double_t edm;
  Double_t errdef;
  Int_t nvpar;
  Int_t nparx;

  fFitter->GetStats(chi2, edm, errdef, nvpar, nparx);
  if (ret!=0)
  {
    Info("FitCluster", "Minimization failed! Last chi2 %f, old chi2 %f.", chi2, oldchi2);
  }

  clstr->fChi2=chi2;

  for(i=0;i<n;i++)
  {
    p=(CbmEcalRecParticle*)fReco->At(i+fNOld);
    p->fE=fFitter->GetParameter(i*3  );
    p->fX=fFitter->GetParameter(i*3+1);
    p->fY=fFitter->GetParameter(i*3+2);
    p->fChi2=chi2;
  }
  
}

/** Calculate a chi2 for just reconstructed photons **/
Double_t CbmEcalRecoSlow::CalculateChi2(CbmEcalClusterV1* cluster)
{
  if (!fShLib) return -1111;
  
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
  return chi2;
}

/** Reconstruct photon from maximum **/
void CbmEcalRecoSlow::Reco(CbmEcalCell* cell, CbmEcalClusterV1* clstr)
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

void CbmEcalRecoSlow::CreateTree()
{
  fOutTree=new TTree("slow_reco", "Reconstruction by fitting with shower library");
  fOutTree->Branch("ev", &fEventN, "ev/I");
  fOutTree->Branch("type", &fType, "type/S");
  fOutTree->Branch("difftype", &fDiffType, "difftype/S");
  fOutTree->Branch("x", &fXReco, "x/D");
  fOutTree->Branch("y", &fYReco, "y/D");
  fOutTree->Branch("e", &fEReco, "e/D");
  fOutTree->Branch("chi2", &fChi2, "chi2/D");
  fOutTree->Branch("ecluster", &fECluster, "ecluster/D");
  if (fStoreClusterInfo==kFALSE) return;
  fOutTree->Branch("cls_e", &fECls, "cls_e/D");
  fOutTree->Branch("cls_e2", &fE2Cls, "cls_e2/D");
  fOutTree->Branch("cls_e3", &fE3Cls, "cls_e3/D");
  fOutTree->Branch("ctypes", fTypes, "ctypes[20]/S");
  fOutTree->Branch("cemeas", fEmeas, "cemeas[20]/D");
  fOutTree->Branch("cepred", fEpred, "cepred[20]/D");
  fOutTree->Branch("cx", fCX, "cx[20]/D");
  fOutTree->Branch("cy", fCY, "cy[20]/D");
}

/** Default constructor. Requirement of ROOT system **/
CbmEcalRecoSlow::CbmEcalRecoSlow() 
  : CbmTask()
{
  ;
}

/** Standard constructor **/
CbmEcalRecoSlow::CbmEcalRecoSlow(const char *name, const Int_t iVerbose, const char* configname)
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
  Info("Constructor", "Formula for sigma (with constants): %s. ", fSigmaFormula.Data());
  for(i=0;i<fParOffset;i++)
  {
    nm="["; nm+=i; nm+="]";
    fSigmaFormula.ReplaceAll(fParNames[i], nm);
  }
  Info("Constructor", "Used formula: %s.", fSigmaFormula.Data());
  fSigma=new TFormula("RecoSimpleFormula", fSigmaFormula.Data());
  fChi2Th=par->GetDouble("chi2th");
  fEStep=par->GetDouble("estep");
  fCStep=par->GetDouble("cstep");
  fMaxIterations=par->GetInteger("maxiterations");

  Info("Constructor", "chi2 threshold is %f, Estep is %f, and Cstep is %f.", fChi2Th, fEStep, fCStep);
  fStoreClusterInfo=kFALSE;
  delete par;
}

CbmEcalRecoSlow::~CbmEcalRecoSlow()
{
  fReco->Delete();
  delete fReco;
}

void CbmEcalRecoSlow::Finish()
{
  if (fOutTree) fOutTree->Write();
}

/** Init **/
InitStatus CbmEcalRecoSlow::Init()
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

  fFCN=new FCNEcalCluster(fCal, fShLib, fSigma, fInf);
  fFCN->SetEStep(fEStep);
  fFCN->SetCStep(fCStep);
  fFitter=new TFitterMinuit(18);
  fFitter->SetPrintLevel(-1);
  fFitter->SetMinuitFCN(fFCN);
  fFitter->SetMaxIterations(fMaxIterations);

  return kSUCCESS;
}

ClassImp(CbmEcalRecoSlow)
