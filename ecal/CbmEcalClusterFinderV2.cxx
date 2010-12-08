#include "CbmEcalClusterFinderV2.h"

#include "CbmEcalStructure.h"
#include "CbmEcalCell.h"
#include "CbmEcalInf.h"
#include "CbmEcalParam.h"
#include "CbmEcalPreCluster.h"
#include "CbmEcalClusterV1.h"
#include "CbmEcalCalibration.h"

#include "FairRootManager.h"
#include "FairTrackParam.h"

#include "TClonesArray.h"
#include "TVector2.h"

#include <iostream>

using namespace std;

/** Exec a task **/
void CbmEcalClusterFinderV2::Exec(Option_t* option)
{
  fEv++;

  if (fCalibration)
    if (!fTree)
      CreateTree();
  if (fRemoveCharged==1)
    ExcludeMaximums();
  FindMaximums();
  FormPreClusters();

  if (fCalibration==0)
    FormClusters();
}

/** Exclude maximums belong to charged tracks **/
// TODO: An old algorithm. Probably, will not work for calorimeter before much

void CbmEcalClusterFinderV2::ExcludeMaximums()
{
  Int_t n=fTracks->GetEntriesFast();
  Int_t i;
  Int_t mn=0;
  FairTrackParam* tr;
  CbmEcalCell* cell;
  list<CbmEcalCell*> cells;
  list<CbmEcalCell*>::const_iterator p;
  list<CbmEcalCell*> ocells;
  list<CbmEcalCell*>::const_iterator op;
  Double_t e;
  Float_t x;
  Float_t y;
  Float_t dst;
  Float_t t;

  if (fVerbose>1) 
    Info("ExcludeMaximums", "Find %d charged tracks in event.", n);
  for(i=0;i<n;i++)
  {
    tr=(FairTrackParam*)fTracks->At(i);
    cell=fStr->GetCell(tr->GetX(), tr->GetY());
    if (cell==NULL) continue;
    cell->GetNeighborsList(0, cells);
    /** Check cell corresponds to track position for maximum **/
    e=cell->GetTotalEnergy();
    for(p=cells.begin();p!=cells.end();++p)
      if ((*p)->GetTotalEnergy()>e)
	break;
    if (p==cells.end())
    {
      fExcluded[mn++]=cell;
      continue;
    }
    cell->GetNeighborsList(0, cells);
    e=cell->GetTotalEnergy();
    /** Check cells near given for maximums **/
    for(p=cells.begin();p!=cells.end();++p)
    {
      x=(*p)->GetCenterX()-tr->GetX();
      y=(*p)->GetCenterY()-tr->GetY();
      dst=TMath::Sqrt(x*x+y*y);
//      if (dst>fInf->GetModuleSize()*TMath::Sqrt(2.0)+0.001) continue;
      x=tr->GetX(); x*=x; y=tr->GetY(); y*=y;
      t=TMath::Sqrt(x+y);
      x=(*p)->GetCenterX()-cell->GetCenterX();
      y=(*p)->GetCenterY()-cell->GetCenterY();
      if (x*tr->GetTx()/t+y*tr->GetTy()/t<TMath::Sqrt(2.0)*fInf->GetModuleSize()/cell->GetType()+0.001)
        if (x*tr->GetTx()/t+y*tr->GetTy()/t<-0.01&&dst>fInf->GetModuleSize()/cell->GetType()+0.001) continue;
      (*p)->GetNeighborsList(0, ocells);
      e=(*p)->GetTotalEnergy();
      for(op=ocells.begin(); op!=ocells.end();++op)
        if ((*op)->GetTotalEnergy()>e) break;
      if (op==ocells.end())
	fExcluded[mn++]=*p;
    }
  }
  fExcluded[mn]=NULL;
}
/** Form a preclusters.
 ** A precluster --- a group of cells neighbor to maximum cell.
 ** A cluster is a group of preclusters with common cells. **/
void CbmEcalClusterFinderV2::FormPreClusters()
{
  Int_t i;
  Int_t j;
  CbmEcalCell* cell;
  Double_t x;
  Double_t y;
  Double_t r;
  Double_t a;
  static Double_t z=fInf->GetZPos();

  ClearPreClusters();
  for(i=0;fMaximums[i]!=NULL;i++)
  {
    cell=fMaximums[i];
    x=cell->GetCenterX();
    y=cell->GetCenterY();
    x*=x; y*=y;
    x+=y; r=TMath::Sqrt(x);
    a=TMath::ATan2(r, z)*TMath::RadToDeg();
    for(j=0;j<fRegions;j++)
      if (fTheta[j]>a)
	break;
    if (fCalibration==0)
    {
      if (fPreClusterAlgo[j]==0)
        FormPreCluster(cell);
      if (fPreClusterAlgo[j]==1)
        FormPreClusterNew(cell);
    }
    else
    {
      fAlgo=j;
      fAlgoType=fPreClusterAlgo[j];
      fX=cell->GetCenterX();
      fY=cell->GetCenterY();
      fThetaOut=a;
      for(j=0;j<fRegions;j++)
      {
	fE[j]=-1111;
	fCurAlgo=j;
        if (fPreClusterAlgo[j]==0)
          FormPreCluster(cell);
        if (fPreClusterAlgo[j]==1)
          FormPreClusterNew(cell);
      }
      fTree->Fill();
    }
  }
}

/** A default algorithm of precluster formation **/
void CbmEcalClusterFinderV2::FormPreCluster(CbmEcalCell* cell)
{
  Int_t reg=GetRegion(cell);
  if (fCalibration==1) reg=fCurAlgo;
  Int_t i;
  Int_t j;
  Int_t max;
  Double_t mine;
  Double_t maxe;
  Double_t e;
  list<CbmEcalCell*> cells;
  list<CbmEcalCell*> cs;
  list<CbmEcalCell*>::const_iterator p;
  CbmEcalCell* min;
  
  mine=cell->GetTotalEnergy(); min=cell;
  maxe=-1.0;
  /** Find minimum cell **/
  cell->GetNeighborsList(0, cells);
  for(p=cells.begin();p!=cells.end();++p)
    if ((*p)->GetTotalEnergy()<mine)
    {
      mine=(*p)->GetTotalEnergy();
      min=(*p);
    }
  for(j=1;j<5;j++)
  {
    cell->GetNeighborsList(j, cells);
    e=0;
    for(p=cells.begin();p!=cells.end();++p)
      e+=(*p)->GetTotalEnergy();
    if (e>maxe)
    {
      maxe=e;
      max=j;
    }
  }
  maxe+=cell->GetTotalEnergy();
  if (fCal)
    if (fCal->GetEnergy(maxe, cell)<fMinClusterE[reg]) return;
  else
    if (maxe<fMinClusterE[reg]) return;
  cell->GetNeighborsList(max, cells);
  cells.push_back(cell);
  if (find(cells.begin(), cells.end(), min)==cells.end())
    cells.push_back(min);
  if (fCalibration==0)
  {
    fPreClusters.push_back(new CbmEcalPreCluster(cells, cell, min)); 
    return;
  }
  else
  {
    fE[fCurAlgo]=maxe;
    return;
  }
}

/** A new algorithm of precluster formation **/
void CbmEcalClusterFinderV2::FormPreClusterNew(CbmEcalCell* cell)
{
  Int_t i;
  Double_t e2m;
  Double_t e2=0;
  Int_t imax;
  list<CbmEcalCell*>::const_iterator p;
  list<CbmEcalCell*> cells;
  Double_t x;
  Double_t y;
  Double_t max;
  Int_t reg=GetRegion(cell);
  if (fCalibration==1) reg=fCurAlgo;
  Int_t cCl2Size=fParI[reg];
  Int_t ix;
  Int_t iy;
  CbmEcalCell* cll;
  Double_t tx;
  Double_t ty;
  TVector2 pv;
  Double_t r;
  CbmEcalCell* cls[cCl2Size];
  Double_t rc[cCl2Size];
  Int_t j;
  Double_t phi;
  Double_t tp;
  Double_t tq;
  Double_t e;

  max=cell->GetTotalEnergy();
  x=cell->GetCenterX()*max;
  y=cell->GetCenterY()*max;

  for(i=1;i<5;i++)
  {
    cell->GetNeighborsList(i, cells);
    e2m=max;
    for(p=cells.begin();p!=cells.end();++p)
      e2m+=(*p)->GetTotalEnergy();
    if (e2m>e2)
    { 
      e2=e2m;
      imax=i;
    }
  }
  cell->GetNeighborsList(imax, cells);
  for(p=cells.begin();p!=cells.end();++p)
  {
    x+=(*p)->GetCenterX()*(*p)->GetTotalEnergy();
    y+=(*p)->GetCenterY()*(*p)->GetTotalEnergy();
  }
  x/=e2; y/=e2;

  for(i=0;i<cCl2Size;i++)
  {
    cls[i]=NULL;
    rc[i]=-1111;
  }
  for(ix=-4;ix<5;ix++)
  for(iy=-4;iy<5;iy++)
  {
    tx=cell->GetCenterX(); tx+=ix*(cell->X2()-cell->X1());
    ty=cell->GetCenterY(); ty+=iy*(cell->Y2()-cell->Y1());
    cll=fStr->GetCell(tx, ty);
    if (cll==NULL) continue;
    r=SolveEllipse(cll, x, y, reg);
    for(i=0;i<cCl2Size;i++)
      if (rc[i]<r)
	break;
    if (i==cCl2Size) continue;
    for(j=cCl2Size-1;j>i;j--)
    {
      rc[j]=rc[j-1];
      cls[j]=cls[j-1];
    }
    rc[i]=r;
    cls[i]=cll;
  }

  e=0;
  for(j=0;j<cCl2Size;j++)
    e+=cls[j]->GetTotalEnergy();
  if (fCal)
    if (fCal->GetEnergy(e, cell)<fMinClusterE[reg]) return;
  else
    if (e<fMinClusterE[reg]) return;

  if (fCalibration==0)
  {
    fPreClusters.push_back(new CbmEcalPreCluster(cls, cCl2Size, cell, NULL)); 
    return;
  }
  else
  {
    fE[fCurAlgo]=e;
    return;
  }
}

Double_t CbmEcalClusterFinderV2::SolveEllipse(CbmEcalCell* cell, Double_t cx, Double_t cy, Int_t reg)
{
  Double_t cRx=fPar2[reg];		//48*1.6
  Double_t cRy=fPar3[reg];
  Int_t cSt=fIntSteps;
  TVector2 centr(cx, cy);
  TVector2 t(centr);
  Double_t gm=centr.Phi();
  Double_t sgm=TMath::Sin(gm);
  Double_t cgm=TMath::Cos(gm);
  Double_t x;
  Double_t y;
  Double_t p=sgm*sgm/cRx+cgm*cgm/cRy;
  Double_t q=cgm*sgm*(1.0/cRx-1.0/cRy);
  Double_t r=cgm*cgm/cRx+sgm*sgm/cRy;
  Double_t d;
  Double_t ex;
  Double_t ey;
  Double_t y1;
  Double_t y2;
  Double_t inte=0;
  Double_t step=(cell->X2()-cell->X1())/cSt;
  Double_t fx=step/2.0+cell->X1();
  Int_t i;
  
// cout << fCurAlgo << " " << reg << " " << fPar1[reg] << " " << cRx << " " << cRy << endl;
  t/=t.Mod(); centr+=t*TMath::Sqrt(fPar1[reg]);
  for(i=0;i<cSt;i++)
  {
    x=step; x*=i; x+=fx;
    ex=x; ex-=centr.X();
    d=q*q*ex*ex-p*(r*ex*ex-1);
    if (d<0) continue;
    d=TMath::Sqrt(d);
    y1=-q*ex/p; y2=y1;
    y1-=d/p; y2+=d/p;
    y1+=centr.Y(); y2+=centr.Y();
    if (y1>cell->Y2()) continue;
    if (y2<cell->Y1()) continue;
    if (y1<cell->Y1()) y1=cell->Y1();
    if (y2>cell->Y2()) y2=cell->Y2();
    inte+=(y2-y1)*step;
  }

  return inte;
}

/** Form clusters from precluster **/
void CbmEcalClusterFinderV2::FormClusters()
{
  /** CbmEcalClusterV1 needs a destructor call :-( **/
  fClusters->Delete();
  Int_t fN=0;
  list<CbmEcalPreCluster*>::const_iterator p1=fPreClusters.begin();
  list<CbmEcalPreCluster*>::const_iterator p2;
  list<CbmEcalCell*> cluster;
  list<CbmEcalCell*> minimums;
  list<CbmEcalCell*>::const_iterator pc;
  list<CbmEcalCell*>::const_iterator pc1;
  UInt_t oldsize;
  Int_t MaxSize=0;
  Int_t Maximums=0;
  Int_t max;
  
  for(;p1!=fPreClusters.end();++p1)
  if ((*p1)->fMark==0)
  {
    cluster.clear(); oldsize=0;
    minimums.clear();
    cluster=(*p1)->fCells;
    if ((*p1)->fMinimum!=NULL)
      minimums.push_back((*p1)->fMinimum);
    max=1;
    while(cluster.size()!=oldsize)
    {
      oldsize=cluster.size();
      p2=p1;
      for(++p2;p2!=fPreClusters.end();++p2)
      if ((*p2)->fMark==0)
      {
        pc=cluster.begin();
	for(;pc!=cluster.end();++pc)
	{
	  if ((*p2)->fMinimum!=NULL&&(*pc)==(*p2)->fMinimum) continue;
	  pc1=find((*p2)->fCells.begin(), (*p2)->fCells.end(), (*pc));
	  if (pc1==(*p2)->fCells.end()) continue;
	  pc1=find(minimums.begin(), minimums.end(), (*pc));
	  if (pc1!=minimums.end()) continue;
	  break;
	}
	if (pc!=cluster.end())
	{
	  (*p2)->fMark=1;
	  pc=(*p2)->fCells.begin();
	  for(;pc!=(*p2)->fCells.end();++pc)
	    if (find(cluster.begin(), cluster.end(), (*pc))==cluster.end())
	      cluster.push_back(*pc);
	  if ((*p2)->fMinimum!=NULL&&find(minimums.begin(), minimums.end(), (*p2)->fMinimum)==minimums.end())
	    minimums.push_back((*p2)->fMinimum);
	  max++;
	}
      }
    }
    (*p1)->fMark=1;
    if ((Int_t)cluster.size()>MaxSize)
      MaxSize=cluster.size();
    if (max>Maximums) Maximums=max;
    CbmEcalClusterV1* cls=new ((*fClusters)[fN]) CbmEcalClusterV1(fN, cluster); fN++;
    cls->Init(cluster);
  }
  if (fVerbose>0)
  {
    Info("FormClusters", "Total %d clusters formed.", fN);
    Info("FormClusters", "Maximum size of cluster is %d cells.",  MaxSize);
    Info("FormClusters", "Maximum number of photons per cluster is %d.",  Maximums);
  }
}

/** Get region number of the cell **/
Int_t CbmEcalClusterFinderV2::GetRegion(CbmEcalCell* cls)
{
  Double_t x=cls->GetCenterX(); x*=x;
  Double_t y=cls->GetCenterY(); y*=y;
  x+=y;
  Double_t r=TMath::Sqrt(x);
  static Double_t z=fInf->GetZPos();
  Double_t a=TMath::ATan2(r, z)*TMath::RadToDeg();
  Int_t i=0;
  for(;i<fRegions;i++)
    if (fTheta[i]>a)
      break;

  return i;
}

/** Locate clusters not belong to charged tracks **/
void CbmEcalClusterFinderV2::FindMaximums()
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
    if (fCal)
    {
      if (fCal->GetEnergy(e, *p)<fMinMaxE[GetRegion(*p)]) continue;
    }
    else
    {
      if (e<fMinMaxE[GetRegion(*p)]) continue;
    }
//    cout << e << " " << fMinMaxE[GetRegion(*p)] << endl;
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

/** Initialization **/
InitStatus CbmEcalClusterFinderV2::Init()
{
  FairRootManager* io=FairRootManager::Instance();
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
  if (fRemoveCharged==1)
  {
    fTracks=(TClonesArray*)io->GetObject("EcalTrackParam");
    if (!fTracks)
    {
      Fatal("Init", "Can't find EcalTrackParam.");
      return kFATAL;
    }
  }
  else
    fTracks=NULL;
  if (fUseCalibration==1)
  {
    fCal=(CbmEcalCalibration*)io->GetObject("EcalCalibration");
    if (!fCal)
    {
      Fatal("Init", "Can't find EcalCalibration");
      return kFATAL;
    }
  }
  else
    fCal=NULL;

  fClusters=new TClonesArray("CbmEcalClusterV1", 2000);
  io->Register("EcalClusters", "ECAL", fClusters, kTRUE);
  fEv=0;
  fMaximums=new CbmEcalCell*[5000];
  fExcluded=new CbmEcalCell*[5000];
  fExcluded[0]=NULL;
  return kSUCCESS;
}

/** Destructor **/
CbmEcalClusterFinderV2::~CbmEcalClusterFinderV2()
{
  if (fClusters)
  {
    fClusters->Delete();
    delete fClusters;
  }
  delete fMaximums;
  delete fExcluded;
  if (fTree)
    delete fTree;
}

/** Only to comply with frame work. **/
CbmEcalClusterFinderV2::CbmEcalClusterFinderV2()
{
  ;
}

/** Finish a task **/
void CbmEcalClusterFinderV2::Finish()
{
  if (fTree)
    fTree->Write();
}

CbmEcalClusterFinderV2::CbmEcalClusterFinderV2(const char* name, const Int_t verbose, const char* cfg)
  : FairTask(name, verbose)
{
  Int_t i;
  TString st;
  CbmEcalParam* par=new CbmEcalParam("ClusterParam", cfg);

  fRemoveCharged=par->GetInteger("removecharged");
  fUseCalibration=par->GetInteger("usecalibration");
  fRegions=par->GetInteger("regions");
  if (fRegions<0||fRegions>10)
  {
    Fatal("CbmEcalClusterFinderV2","Number of calorimeter regions to high.");
    delete par;
    return;
  }
  fCalibration=par->GetInteger("calibrationmode");

  fIntSteps=par->GetInteger("intsteps");
  for(i=0;i<fRegions;i++)
  {
    st="preclusteralgo["; st+=i; st+="]";
    fPreClusterAlgo[i]=par->GetInteger(st);
    st="minclustere["; st+=i; st+="]";
    fMinClusterE[i]=par->GetDouble(st);
    st="minmaxe["; st+=i; st+="]";
    fMinMaxE[i]=par->GetDouble(st);
    st="theta["; st+=i; st+="]";
    fTheta[i]=par->GetDouble(st);
    st="par1["; st+=i; st+="]";
    fPar1[i]=par->GetDouble(st);
    st="par2["; st+=i; st+="]";
    fPar2[i]=par->GetDouble(st);
    st="par3["; st+=i; st+="]";
    fPar3[i]=par->GetDouble(st);
    st="pari["; st+=i; st+="]";
    fParI[i]=par->GetInteger(st);
  }
  delete par;
  fTree=NULL;
}


/** Clear a preclusters list **/
void CbmEcalClusterFinderV2::ClearPreClusters()
{
  list<CbmEcalPreCluster*>::const_iterator p=fPreClusters.begin();
  for(;p!=fPreClusters.end();++p)
    delete (*p);
  fPreClusters.clear();
}


void CbmEcalClusterFinderV2::CreateTree()
{
  TString st;

  fTree=new TTree("c2","Preclusters information");
  fTree->Branch("ev", &fEv, "ev/I");
  fTree->Branch("algo", &fAlgo, "algo/I");
  fTree->Branch("algot", &fAlgoType, "algot/I");
  fTree->Branch("x", &fX, "x/D");
  fTree->Branch("y", &fY, "y/D");
  fTree->Branch("theta", &fThetaOut, "theta/D");
  st="e["; st+=fRegions; st+="]/D";
  fTree->Branch("e", fE, st);
}

ClassImp(CbmEcalClusterFinderV2)
