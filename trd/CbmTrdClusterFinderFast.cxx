#include "CbmTrdClusterFinderFast.h"

#include "CbmTrdDigiPar.h"
#include "CbmTrdDigi.h"
#include "CbmTrdCluster.h"
#include "CbmTrdModule.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"
#include "TArray.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TImage.h"

//#include <map>
#include <iostream>
#include <iomanip>
#include <cmath>
using std::cout;
using std::endl;

// ---- Default constructor -------------------------------------------
CbmTrdClusterFinderFast::CbmTrdClusterFinderFast()
  :FairTask("TrdClusterFinder",1),
   fDigis(NULL),
   fClusters(NULL),
   fDigiPar(NULL),
   fModuleInfo(NULL),
   fTrdId()
{

}
// --------------------------------------------------------------------

// ---- Destructor ----------------------------------------------------
CbmTrdClusterFinderFast::~CbmTrdClusterFinderFast()
{

  if(fClusters){
    fClusters->Clear("C");
    fClusters->Delete();
    delete fClusters;
  }
  if(fDigis){
    fDigis->Delete();
    delete fDigis;
  }
  if(fDigiPar){
    delete fDigiPar;
  }
  if(fModuleInfo){
    delete fModuleInfo;
  }

}

// ----  Initialisation  ----------------------------------------------
void CbmTrdClusterFinderFast::SetParContainers()
{
  // Get Base Container
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  cout << " * CbmTrdClusterFinderFast * :: SetParContainers()" << endl;
  fDigiPar = (CbmTrdDigiPar*)(rtdb->getContainer("CbmTrdDigiPar"));
  
}
// --------------------------------------------------------------------

// ---- ReInit  -------------------------------------------------------
InitStatus CbmTrdClusterFinderFast::ReInit(){
  
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  
  fDigiPar = (CbmTrdDigiPar*)(rtdb->getContainer("CbmTrdDigiPar"));
  
  return kSUCCESS;
}
// --------------------------------------------------------------------

// ---- Init ----------------------------------------------------------
InitStatus CbmTrdClusterFinderFast::Init()
{

  FairRootManager *ioman = FairRootManager::Instance();
  
  fDigis =(TClonesArray *)  ioman->ActivateBranch("TrdDigi");
  if ( ! fDigis ) {
    cout << "-W CbmTrdHitProducerDigi::Init: No TrdDigi array!" << endl;
    cout << "                            Task will be inactive" << endl;
    return kERROR;
  }
  
  fClusters = new TClonesArray("CbmTrdCluster", 100);
  ioman->Register("TrdCluster","TRD",fClusters,kTRUE);
  
  return kSUCCESS;
  
} 
// --------------------------------------------------------------------

// ---- Exec ----------------------------------------------------------
void CbmTrdClusterFinderFast::Exec(Option_t * option)
{
 cout << "================CbmTrdClusterFinderFast===============" << endl;
  Int_t nentries = fDigis->GetEntries();
  Int_t digiCounter = 0;
  cout << " Found " << nentries << " Digis in Collection" << endl;
  std::map<Int_t, MyDigiList*> modules;

  for (Int_t iDigi=0; iDigi < nentries; iDigi++ ) {
    CbmTrdDigi *digi = (CbmTrdDigi*) fDigis->At(iDigi);
    if (digi->GetCharge() > minimumChargeTH)
      {
	digiCounter++;
	MyDigi *d = new MyDigi;
	Int_t moduleId = digi->GetDetId();
	fModuleInfo = fDigiPar->GetModule(moduleId);
	d->digiId = iDigi;
	d->rowId = digi->GetRow();
	d->colId = digi->GetCol();
	d->charge = digi->GetCharge();
	//unrotate rotated modules in x- and y-direction (row <-> col)
	Int_t* detInfo = fTrdId.GetDetectorInfo(moduleId); 
	Int_t Station  = detInfo[1];
	Int_t Layer    = detInfo[2]; 
	if (Layer%2 == 0) {
	  d->combiId = d->rowId * (fModuleInfo->GetnRow() + 1) + d->colId;
	}
	else {
	  d->combiId = d->rowId * (fModuleInfo->GetnCol() + 1) + d->colId;
	}
	if (modules.find(moduleId) == modules.end()) {
	  modules[moduleId] = new MyDigiList;
	} 
	modules[moduleId]->push_back(d);
      }
  }
  cout << " Used  " << digiCounter << " Digis after Minimum Charge Cut (" << minimumChargeTH << ")" << endl;
  std::map<Int_t, ClusterList*> fModClusterMap;
  for (std::map<Int_t, MyDigiList*>::iterator it = modules.begin(); it != modules.end(); ++it) {
    fModClusterMap[it->first] = clusterModule(it->second);
    
    //drawCluster(it->first, fModClusterMap[it->first]);
  }
  addCluster(fModClusterMap);
  // clean up
  for (std::map<Int_t, ClusterList*>::iterator it = fModClusterMap.begin(); 
       it != fModClusterMap.end(); ++it) {
    for (ClusterList::iterator clusterIt = it->second->begin(); 
	 clusterIt != it->second->end(); ++clusterIt) {
      for (MyDigiList::iterator digisIt = (*clusterIt)->begin(); 
	   digisIt != (*clusterIt)->end(); ++digisIt) {
	delete *digisIt;
      }
      delete *clusterIt;
    }
    delete it->second;
  }
}

//----------------------------------------------------------------------
bool digiSorter(MyDigi *a, MyDigi *b)
{ return (a->combiId < b->combiId); }

//----------------------------------------------------------------------
ClusterList *CbmTrdClusterFinderFast::clusterModule(MyDigiList *digis)
{
  std::list<RowCluster*> closedList;
  std::list<RowCluster*> openList;
  std::list<RowCluster*> currentList;
  RowCluster *currentCluster = new RowCluster;

  digis->sort(digiSorter);
  for (MyDigiList::iterator it = digis->begin(); it != digis->end(); ++it) {
    if (currentCluster->digis->empty() || 
	currentCluster->digis->back()->combiId + 1 == (*it)->combiId) {
      currentCluster->digis->push_back(*it);
    }
    else {
      mergeRowCluster(currentCluster, &openList);
      currentList.push_back(currentCluster);
      // did we move into a new row?
      if ((*it)->rowId > currentCluster->row) {
	closedList.splice(closedList.end(), openList);
	// did we skip at least one line?
	if ((*it)->rowId > currentCluster->row + 1) {
	  closedList.splice(closedList.end(), currentList);
	}
	else {
	  openList.splice(openList.begin(), currentList);
	}
      }
      currentCluster = new RowCluster;
      currentCluster->digis->push_back(*it);
    }
  }
  // handle last row
  mergeRowCluster(currentCluster, &openList);
  currentList.push_back(currentCluster);
  closedList.splice(closedList.end(), openList);
  closedList.splice(closedList.end(), currentList);

  return findCluster(&closedList);
}

//----------------------------------------------------------------------
void CbmTrdClusterFinderFast::mergeRowCluster(RowCluster *currentCluster,
					      std::list<RowCluster*> *openList)
{
  // finish up the current rowCluster
  currentCluster->minCol = currentCluster->digis->front()->colId;
  currentCluster->maxCol = currentCluster->digis->back()->colId;
  currentCluster->row = currentCluster->digis->back()->rowId;
  // merge current RowCluster with openList
  for (std::list<RowCluster*>::iterator openIt = openList->begin(); 
       openIt != openList->end(); ++openIt) {
    if (currentCluster->maxCol < (*openIt)->minCol - 1)
      break;
    else if (currentCluster->minCol > (*openIt)->maxCol + 1)
      continue;
    else {
      currentCluster->parents.push_back(*openIt);
      (*openIt)->children.push_back(currentCluster);
    }
  }
}					      

//----------------------------------------------------------------------
ClusterList *CbmTrdClusterFinderFast::findCluster(std::list<RowCluster*> *rowClusterList)
{
  RowCluster *currentRowCluster;
  ClusterList *clusterList = new ClusterList;
  while (!rowClusterList->empty()) {
    currentRowCluster = *rowClusterList->begin();
    MyDigiList *cluster = new MyDigiList;
    walkCluster(rowClusterList, currentRowCluster, cluster);
    clusterList->push_back(cluster);
  }
  return clusterList;
}

//----------------------------------------------------------------------
void CbmTrdClusterFinderFast::walkCluster(std::list<RowCluster*> *rowClusterList, 
					  RowCluster *currentRowCluster,
					  MyDigiList *cluster)
{
  cluster->splice(cluster->end(), *(currentRowCluster->digis));
  rowClusterList->remove(currentRowCluster);
  std::list<RowCluster*> neighbours;
  for (std::list<RowCluster*>::iterator it = currentRowCluster->parents.begin();
       it != currentRowCluster->parents.end();) {
    if (!(*it)->hasBeenVisited) {
      neighbours.push_back(*it);
      (*it)->hasBeenVisited = true;
    }
    (*it)->children.remove(currentRowCluster);
    it = currentRowCluster->parents.erase(it);
  }
  for (std::list<RowCluster*>::iterator it = currentRowCluster->children.begin();
       it != currentRowCluster->children.end();) {
    if (!(*it)->hasBeenVisited) {
      neighbours.push_back(*it);
      (*it)->hasBeenVisited = true;
    }
    (*it)->parents.remove(currentRowCluster);
    it = currentRowCluster->children.erase(it);
  }
  delete currentRowCluster;
  currentRowCluster = NULL;
  for (std::list<RowCluster*>::iterator it = neighbours.begin();
       it != neighbours.end(); ++it) {
    walkCluster(rowClusterList, *it, cluster);
  }
}

//----------------------------------------------------------------------
void CbmTrdClusterFinderFast::drawCluster(Int_t moduleId, ClusterList *clusterList)
{
  Char_t name[50];
  Char_t title[50];
  sprintf(name,"C%d",moduleId);
  sprintf(title,"%s",name);
  Char_t Canfile[100];
  sprintf(Canfile,"Pics/ModuleID%s.png",name);
  TH2F* Test = new TH2F(title,name,200,0,200,30,0,30);
  Test->SetContour(99);
 
  Int_t Row;
  Int_t Col;
  Int_t counter = 0;
  for (ClusterList::iterator iCluster = clusterList->begin(); 
       iCluster != clusterList->end(); iCluster++, counter++)
    {
      for (MyDigiList::iterator iPad = (*iCluster)->begin(); iPad != (*iCluster)->end(); iPad++)
	{
	  Row = (*iPad)->rowId+1;
	  Col = (*iPad)->colId+1;
	  Test->SetBinContent( Col, Row, counter+1);
	}
    }
  TCanvas* c = new TCanvas(title,name,900,900);
  c->Divide(1,1);
  c->cd(1);
  Test->Draw("colz");
  TImage *Outimage = TImage::Create();
  Outimage->FromPad(c);
  Outimage->WriteImage(Canfile);
  delete Test;
  delete c;
}

//--------------------------------------------------------------------
void CbmTrdClusterFinderFast::addCluster(std::map<Int_t, ClusterList*> fModClusterMap)
{
  Int_t ClusterSumm = 0;
  Float_t Charge;
  Float_t qMax;
  for (std::map<Int_t, ClusterList*>::iterator iMod = fModClusterMap.begin(); iMod != fModClusterMap.end(); ++iMod) 
    {
      for (ClusterList::iterator iCluster = (iMod->second)->begin(); iCluster != (iMod->second)->end(); iCluster++) 
	{
	  ClusterSumm++;
	  TArrayI* digiIndices = new TArrayI( Int_t((*iCluster)->size()) );
	  Int_t i = 0;
	  Charge = 0;
	  qMax = 0;
	  for (MyDigiList::iterator iDigi = (*iCluster)->begin(); iDigi != (*iCluster)->end(); iDigi++) 
	    {
	      (*digiIndices)[i] = (*iDigi)->digiId;
	      Charge += (*iDigi)->charge;
	      i++;
	      if (qMax < (*iDigi)->charge)
		{
		  qMax = (*iDigi)->charge;
		}
	    }
	  CbmTrdCluster* Cluster = new CbmTrdCluster(*digiIndices, Charge, qMax);
	  delete digiIndices;
	}
      
    }
 
  cout << " Found " << ClusterSumm << " Cluster" << endl;
}

// ---- Register ------------------------------------------------------
void CbmTrdClusterFinderFast::Register()
{
  FairRootManager::Instance()->Register("TrdCluster","Trd Cluster", fClusters, kTRUE);
}
// --------------------------------------------------------------------


// ---- Finish --------------------------------------------------------
void CbmTrdClusterFinderFast::Finish()
{
}

ClassImp(CbmTrdClusterFinderFast)
