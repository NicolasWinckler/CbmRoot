#include "CbmEcalShowerLib.h"

#include "FairRootManager.h"

#include "TSystem.h"
#include "TFile.h"
#include "TTree.h"
#include "TMath.h"

#include <iostream>
#include <vector>

using std::cout;
using std::endl;
using std::vector;

/** Return sum from (ix, iy) to (ix+cellx, iy+celly) **/
Float_t CbmEcalShowerLib::GetSum(Int_t ix, Int_t iy, Int_t cellx, Int_t celly, Int_t n)
{
  Int_t sx=ix;
  if (sx>=fIX) return 0;
  if (sx<0) sx=0;
  Int_t sy=iy;
  if (sy>=fIY) return 0;
  if (sy<0) sy=0;
  Int_t ex=ix; ex+=cellx;
  if (ex<0) return 0;
  if (ex>fIX) ex=fIX;
  Int_t ey=iy; ey+=celly;
  if (ey<0) return 0;
  if (ey>fIY) ey=fIY;
  Int_t sn=sy; sn*=fIX; sn+=sx;
  Int_t stx=ex; stx-=sx;
//  Int_t sty=ey; sty-=sy;
  Double_t res=0;
  Int_t j;
  for(;sy<ey;sy++)
  {
    for(j=0;j<stx;j++)
      res+=fLib[n].fData[sn+j];
    sn+=fIX;
  }
  return res;
}

/** Return sum for cell with center (x,y) **/
Float_t CbmEcalShowerLib::GetSum(Float_t x, Float_t y, Float_t cell, Int_t n)
{
  fPreGradX=0;
  fPreGradY=0;
  Int_t cellx=(Int_t)((cell+0.0001)/fCellX);
  Int_t celly=(Int_t)((cell+0.0001)/fCellY);
  Float_t sx=x; sx-=fStartX;  sx-=cell/2.0;// sx-=fCellX/2.0;
  Float_t sy=y; sy-=fStartY;  sy-=cell/2.0;// sy-=fCellY/2.0;
  Int_t isx=(Int_t)(sx/fCellX);
  if (sx<0) isx--;
  Int_t isy=(Int_t)(sy/fCellY);
  if (sy<0) isy--;
  if (isx>=fIX||isy>=fIY) return 0;
  Int_t iex=isx+cellx;
  Int_t iey=isy+celly;
  if (iex<0||iey<0) return 0;
  Float_t dx=sx/fCellX-isx;
  Float_t dy=sy/fCellY-isy;
  Double_t r0=0;
  Double_t r1=0;
  Double_t r2=0;
  Double_t r3=0;
  Double_t r5=0;
  Double_t r6=0;
  Double_t r7=0;
  Double_t r8=0;
  Int_t i;
  Int_t ni1;
  Int_t f;
  Int_t l;
  if (isx>=0&&isy>=0)
    r0=fLib[n].fData[isx+isy*fIX];
  if (isx>=0&&iey<fIY)
    r6=fLib[n].fData[isx+iey*fIX];
  if (iex<fIX&&isy>=0)
    r2=fLib[n].fData[iex+isy*fIX];
  if (iex<fIX&&iey<fIY)
    r8=fLib[n].fData[iex+iey*fIX];
  Float_t r4=GetSum(isx+1, isy+1, cellx-1, celly-1, n);
  if (isy>=0)
  {
    f=isx+1; l=iex;
    if (f<0) f=0;
    if (l>fIX) l=fIX;
    ni1=isy*fIX;
    for(i=f;i<l;i++)
      r1+=fLib[n].fData[ni1+i];
  }
  if (isx>=0)
  {
    f=isy+1; l=iey;
    if (f<0) f=0;
    if (l>fIY) l=fIY;
    for(i=f;i<l;i++)
      r3+=fLib[n].fData[isx+i*fIX];
  }
  if (iex<fIX)
  {
    f=isy+1; l=iey;
    if (f<0) f=0;
    if (l>fIY) l=fIY;
    for(i=f;i<l;i++)
      r5+=fLib[n].fData[iex+i*fIX];
  }
  if (iey<fIY)
  {
    f=isx+1; l=iex;
    if (f<0) f=0;
    if (l>fIX) l=fIX;
    ni1=iey*fIX;
    for(i=f;i<l;i++)
      r7+=fLib[n].fData[ni1+i];
  }

  fPreGradX=((r2-r0)*(1-dy)+(r5-r3)+(r8-r6)*dy)/fCellX;
  fPreGradY=((r6-r0)*(1-dx)+(r7-r1)+(r8-r2)*dx)/fCellY;
  return r0*(1-dx)*(1-dy)+r1*(1-dy)+r2*(1-dy)*dx+r3*(1-dx)+r4+r5*dx+r6*dy*(1-dx)+r7*dy+r8*dx*dy; 
}

/** Return sum for cell with center (x,y) **/
Float_t CbmEcalShowerLib::GetSumPhi(Float_t x, Float_t y, Float_t cell, Int_t pren, Float_t phi)
{
  Float_t ex=x;
  Float_t ey=y;
  Float_t t;
  Float_t ep=phi;
  Int_t nf;
  Int_t nl;
  Float_t r1;
  Float_t r2;
  Float_t a1;
  Float_t a2;
  Float_t grx1;
  Float_t grx2;
  Float_t gra[4]={1, 0, 0, 1};
  Float_t gry1;
  Float_t gry2;

  if (phi<0||phi>=360)
  {
    if (Verb()>99)
      cout << "-I- CbmEcalShowerLib: Get phi " << phi << endl; 
    return -1;
  }
  if (ep>180)
  {
    ey=-ey;
    gra[3]=-1;
    ep=360-ep;
  }
  if (ep>90)
  {
    ep=180-ep;
    ex=-ex;
    gra[0]=-1;
  }
  if (ep>45)
  {
    ep=90-ep;
    gra[1]=gra[0];
    gra[2]=gra[3];
    gra[0]=gra[3]=0;
    t=ex;
    ex=ey;
    ey=t;
  }
  nf=GetPhiNum(ep);
  if (nf==(Int_t)fPhiS)
  {
    nf--;
    t=2.0*(45-fPhis[nf]);
    a2=(ep-fPhis[nf])/t;
    a1=1-a2;
    nf+=pren;
    r1=GetSum(ex, ey, cell, nf);
    grx1=fPreGradX*gra[0]+fPreGradY*gra[1];
    gry1=fPreGradX*gra[2]+fPreGradY*gra[3];
    r2=GetSum(ey, ex, cell, nf);
    gry2=fPreGradX*gra[0]+fPreGradY*gra[1];
    grx2=fPreGradX*gra[2]+fPreGradY*gra[3];
    fPreGradX=grx1*a1+grx2*a2;
    fPreGradY=gry1*a1+gry2*a2;
    return r1*a1+r2*a2;
  }
  if (nf==0) nf=1;
  nl=nf; nl--;
  r1=GetSum(ex, ey, cell, nl+pren);
  grx1=fPreGradX*gra[0]+fPreGradY*gra[1];
  gry1=fPreGradX*gra[2]+fPreGradY*gra[3];
  r2=GetSum(ex, ey, cell, nf+pren);
  grx2=fPreGradX*gra[0]+fPreGradY*gra[1];
  gry2=fPreGradX*gra[2]+fPreGradY*gra[3];
  t=fPhis[nf]-fPhis[nl];
  a2=(ep-fPhis[nl])/t;
  a1=(fPhis[nf]-ep)/t;
  fPreGradX=grx1*a1+grx2*a2;
  fPreGradY=gry1*a1+gry2*a2;
  return r1*a1+r2*a2;
}

/** Return sum for cell with center (x,y) **/
Float_t CbmEcalShowerLib::GetSumThetaPhi(Float_t x, Float_t y, Float_t cell, Int_t pren, Float_t theta, Float_t phi)
{
  if (theta<fThetas[0])
  {
    if (Verb()>99)
      cout << "-I- CbmEcalShowerLib: Get theta " << theta << endl;
    return GetSumPhi(x, y, cell, pren, phi);
  }
  Int_t n=GetThetaNum(theta);
  if (n==0)
    return GetSumPhi(x, y, cell, pren, phi);
  if (n==(Int_t)fThetaS)
  {
    if (Verb()>99)
      cout << "-I- CbmEcalShowerLib: Get theta " << theta << endl;
    return GetSumPhi(x, y, cell, pren+fPhiS*(fThetaS-1), phi);
  }
  Float_t r1=GetSumPhi(x, y, cell, pren+fPhiS*(n-1), phi);
  Float_t grx1=fPreGradX;
  Float_t gry1=fPreGradY;
  Float_t r2=GetSumPhi(x, y, cell, pren+fPhiS*n, phi);
  Float_t grx2=fPreGradX;
  Float_t gry2=fPreGradY;
  Float_t t=fThetas[n]-fThetas[n-1];
  Float_t a1=(fThetas[n]-theta)/t;
  Float_t a2=(theta-fThetas[n-1])/t;

  fPreGradX=grx1*a1+grx2*a2;
  fPreGradY=gry1*a1+gry2*a2;
  return r1*a1+r2*a2;
}

/** Return sum for cell with center (x,y) **/
Float_t CbmEcalShowerLib::GetSumEThetaPhi(Float_t x, Float_t y, Float_t cell, Float_t e, Float_t theta, Float_t phi)
{
  Int_t n=GetENum(e);
  if (n==0)
  {
    fGradE=GetSumThetaPhi(x, y, cell, 0, theta, phi)/fEs[0];
    fGradX=fPreGradX;
    fGradY=fPreGradY;
    return e*fGradE;
  }
  if (n==(Int_t)fES)
  {
    fGradE=GetSumThetaPhi(x, y, cell, (fES-1)*fPhiS*fThetaS, theta, phi)/fEs[fES-1];
    fGradX=fPreGradX;
    fGradY=fPreGradY;
    return e*fGradE;
  }
  Float_t r1=GetSumThetaPhi(x, y, cell, fPhiS*fThetaS*(n-1), theta, phi);
  Float_t grx1=fPreGradX;
  Float_t gry1=fPreGradY;
  Float_t r2=GetSumThetaPhi(x, y, cell, fPhiS*fThetaS*n, theta, phi);
  Float_t grx2=fPreGradX;
  Float_t gry2=fPreGradY;
  Float_t t=fEs[n]-fEs[n-1];
  Float_t a1=(fEs[n]-e)/t;
  Float_t a2=(e-fEs[n-1])/t;

  fGradX=grx1*a1+grx2*a2;
  fGradY=gry1*a1+gry2*a2;
  fGradE=(r2-r1)/t;
  return r1*a1+r2*a2;
}

/** Constructor to use **/
CbmEcalShowerLib::CbmEcalShowerLib(const char* libname, Int_t verbose)
  : FairTask("CbmEcalShowerLib", verbose), fLibName(libname), fVerb(verbose)
{
  fSize=0;
  fIX=361;
  fIY=361;
  fCellX=0.1;
  fCellY=0.1;
  fStartX=-12.05;
  fStartY=-12.05;
  fCaloZ=1200;
  
  fEs=NULL;
  fThetas=NULL;
  fPhis=NULL;
}

/** Default constructor **/
CbmEcalShowerLib::CbmEcalShowerLib()
  : FairTask(), fLibName(""), fVerb(0)
{
  fSize=0;
  fEs=NULL;
  fThetas=NULL;
  fPhis=NULL;
}

InitStatus CbmEcalShowerLib::Init()
{
  TFile* f;
  TTree* t;
  TString lib(fLibName);
  vector<Float_t> es;
  vector<Float_t> thetas;
  vector<Float_t> phis;
  Float_t e;
  Float_t theta;
  Float_t phi;
  Float_t* en;
  Int_t eq;
  Int_t thetaq;
  Int_t phiq; 
  Int_t i;
  Int_t n;
  UInt_t j;

  es.clear(); thetas.clear(); phis.clear();
  gSystem->ExpandPathName(lib);
  if (Verb()>9)
    cout << "-I- CbmEcalShowerLib: Using shower library " << lib << endl;
  f=new TFile(fLibName);
  if (f->IsZombie())
    Fatal("CbmEcalShowerLib", "Can't find library file %s.", lib.Data());
  t=(TTree*)f->Get("showerlib");
  if (t==NULL)
  {
    Fatal("CbmEcalShowerLib", "Can't find library in file %s.", lib.Data());
    return kFATAL;
  }

  fRSize=fIX*fIY;
  en=new Float_t[fRSize];
  t->SetBranchAddress("e", &e);
  t->SetBranchAddress("theta", &theta);
  t->SetBranchAddress("phi", &phi);
  t->SetBranchAddress("es", en);
  
  fSize=t->GetEntries();
  if (Verb()>9)
    cout << "-I- CbmEcalShowerLib: " << fSize << " records in shower lib. " << endl;
 
  
  for(i=0;i<fSize;i++)
  {
    t->GetEntry(i);

    for(j=0;j<es.size();j++)
      if (es[j]==e) break;
    if (j==es.size()) es.push_back(e);
    
    for(j=0;j<phis.size();j++)
      if (phis[j]==phi) break;
    if (j==phis.size()) phis.push_back(phi);
    
    for(j=0;j<thetas.size();j++)
      if (thetas[j]==theta) break;
    if (j==thetas.size()) thetas.push_back(theta);
  }
  if (es.size()==0)
    Fatal("CbmEcalShowerLib", "No single record found in the library.");
  if (Verb()>9)
  {
    cout << "Energies: " << es[0];
    for(j=1;j<es.size();j++)
      cout << ", " << es[j];
    cout << " GeV" << endl;
    cout << "Thetas: " << thetas[0];
    for(j=1;j<thetas.size();j++)
      cout << ", " << thetas[j];
    cout << " grad" << endl;
    cout << "Phis: " << phis[0];
    for(j=1;j<phis.size();j++)
      cout << ", " << phis[j];
    cout << " grad" << endl;
  }

  fES=es.size();
  fEs=new Float_t[fES];
  for(j=0;j<fES;j++)
    fEs[j]=es[j];
  
  fThetaS=thetas.size();
  fThetas=new Float_t[fThetaS];
  for(j=0;j<fThetaS;j++)
    fThetas[j]=thetas[j];
  
  fPhiS=phis.size();
  fPhis=new Float_t[fPhiS];
  for(j=0;j<fPhiS;j++)
    fPhis[j]=phis[j];
 
  fLib=new CbmEcalShowerLibRecord[fSize];
  for(i=0;i<fSize;i++)
  {
    t->GetEntry(i);
    
    eq=GetENum(e);
    if (eq==(Int_t)fES||fEs[eq]!=e)
      Fatal("CbmEcalShowerLib", "Error during library creation.");
    
    thetaq=GetThetaNum(theta);
    if (thetaq==(Int_t)fThetaS||fThetas[thetaq]!=theta)
      Fatal("CbmEcalShowerLib", "Error during library creation.");
    
    phiq=GetPhiNum(phi);
    if (phiq==(Int_t)fPhiS||fPhis[phiq]!=phi)
      Fatal("CbmEcalShowerLib", "Error during library creation.");
    n=GetNum(eq, thetaq, phiq);
    if (n<0)
      Fatal("CbmEcalShowerLib", "Error during library creation.");
    fLib[n].fE=e;
    fLib[n].fTheta=theta;
    fLib[n].fPhi=phi;
    if (fLib[n].fData!=NULL)
      Fatal("CbmEcalShowerLib", "Error during library creation.");
    fLib[n].fData=new Float_t[fRSize];
    for(j=0;j<(UInt_t)fRSize;j++)
      fLib[n].fData[j]=en[j];
  }
  
  if (Verb()>9)
  {
    cout << "-I- CbmEcalShowerLib: Shower library size is " << fSize*(sizeof(CbmEcalShowerLibRecord)+fRSize*sizeof(Float_t))/1024/1024 <<  " megabytes." << endl;
    cout << "-I- CbmEcalShowerLib: Shower library initialization complete." << endl;
  }
  f->Close();
  delete en;

  FairRootManager* fManager=FairRootManager::Instance();
  fManager->Register("EcalShowerLib", "ECAL", this, kFALSE);

  return kSUCCESS;
}

CbmEcalShowerLib::~CbmEcalShowerLib()
{
  delete fEs;
  delete fThetas;
  delete fPhis;
  Int_t i;

  for(i=0;i<fSize;i++)
    fLib[i].Clear();

  delete fLib;
}

ClassImp(CbmEcalShowerLib)
