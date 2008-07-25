#include "CbmEcalClusterFinderV1.h"

#include "TClonesArray.h"

#include "CbmRootManager.h"
#include "CbmTrackParam.h"

#include "CbmEcalStructure.h"
#include "CbmEcalCell.h"
#include "CbmEcalInf.h"
#include "CbmEcalClusterV1.h"

#include <iostream>
#include <list>

using namespace std;

class __CbmEcalPreCluster
{
public:
  __CbmEcalPreCluster(const list<CbmEcalCell*> cells, CbmEcalCell* max, CbmEcalCell* min) 
    : fCells(cells), fMaximum(max), fMinimum(min), fMark(0)
  {
    ;
  }

  list<CbmEcalCell*> fCells;
  CbmEcalCell* fMaximum;
  CbmEcalCell* fMinimum;
  Int_t fMark;
};

/** Exec a task **/
void CbmEcalClusterFinderV1::Exec(Option_t* option)
{
  fEv++;

  ExcludeMaximums();
  FindMaximums();
  FormPreClusters();
  FormClusters();
}

InitStatus CbmEcalClusterFinderV1::Init()
{
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
  fClusters=new TClonesArray("CbmEcalClusterV1", 2000);
  io->Register("EcalClusters", "ECAL", fClusters, kFALSE);
  fEv=0;
  fMaximums=new CbmEcalCell*[5000];
  fExcluded=new CbmEcalCell*[5000];
  return kSUCCESS;
}

/** Finish a task **/
void CbmEcalClusterFinderV1::Finish()
{
  ;
}

/** Destructor **/
CbmEcalClusterFinderV1::~CbmEcalClusterFinderV1()
{
  if (fClusters)
  {
    fClusters->Delete();
    delete fClusters;
  }
  delete fMaximums;
  delete fExcluded;
}

/** Exclude maximums belong to charged tracks **/
void CbmEcalClusterFinderV1::ExcludeMaximums()
{
  Int_t n=fTracks->GetEntriesFast();
  Int_t i;
  Int_t mn=0;
  CbmTrackParam* tr;
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
    tr=(CbmTrackParam*)fTracks->At(i);
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

/** Locate clusters not belong to charged tracks **/
void CbmEcalClusterFinderV1::FindMaximums()
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

/** Form a preclusters.
 ** A precluster --- a group of cells neighbor to maximum cell.
 ** A cluster is a group of preclusters with common cells. **/
void CbmEcalClusterFinderV1::FormPreClusters()
{
  Int_t i;
  Int_t j;
  Int_t max;
  CbmEcalCell* cell;
  Double_t mine;
  Double_t maxe;
  Double_t e;
  list<CbmEcalCell*> cells;
  list<CbmEcalCell*>::const_iterator p;
  CbmEcalCell* min;

  ClearPreClusters();
  for(i=0;fMaximums[i]!=NULL;i++)
  {
    cell=fMaximums[i];
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
    /** TODO: we need a calibration here. Not such a limit! **/
    if (maxe<0.05) continue;
    cell->GetNeighborsList(max, cells);
    cells.push_back(cell);
    if (find(cells.begin(), cells.end(), min)==cells.end())
      cells.push_back(min);
    fPreClusters.push_back(new __CbmEcalPreCluster(cells, cell, min));
  }
}

/** Form clusters from precluster **/
void CbmEcalClusterFinderV1::FormClusters()
{
  /** CbmEcalClusterV1 needs a destructor call :-( **/
  fClusters->Delete();
  Int_t fN=0;
  list<__CbmEcalPreCluster*>::const_iterator p1=fPreClusters.begin();
  list<__CbmEcalPreCluster*>::const_iterator p2;
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
    cluster.clear(); oldsize==0;
    minimums.clear();
    cluster=(*p1)->fCells;
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
	  if ((*pc)==(*p2)->fMinimum) continue;
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
	  if (find(minimums.begin(), minimums.end(), (*p2)->fMinimum)==minimums.end())
	    minimums.push_back((*p2)->fMinimum);
	  max++;
	}
      }
    }
    (*p1)->fMark=1;
    if (cluster.size()>MaxSize)
      MaxSize=cluster.size();
    if (max>Maximums) Maximums=max;
    CbmEcalClusterV1* cls=new ((*fClusters)[fN]) CbmEcalClusterV1(fN, cluster); fN++;
    cls->Init();
  }
  if (fVerbose>0)
  {
    Info("FormClusters", "Total %d clusters formed.", fN);
    Info("FormClusters", "Maximum size of cluster is %d cells.",  MaxSize);
    Info("FormClusters", "Maximum number of photons per cluster is %d.",  Maximums);
  }
}

/** Clear a preclusters list **/
void CbmEcalClusterFinderV1::ClearPreClusters()
{
  list<__CbmEcalPreCluster*>::const_iterator p=fPreClusters.begin();
  for(;p!=fPreClusters.end();++p)
    delete (*p);
  fPreClusters.clear();
}

/** Standard constructor **/
CbmEcalClusterFinderV1::CbmEcalClusterFinderV1(const char* name, const Int_t verbose)
  : CbmTask(name, verbose)
{
  fTracks=NULL;
  fClusters=NULL;
}


/** Only to comply with frame work. **/
CbmEcalClusterFinderV1::CbmEcalClusterFinderV1()
{
  ;
}

ClassImp(CbmEcalClusterFinderV1)
