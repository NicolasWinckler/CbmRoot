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
  :FairTask("TrdClusterFinderFast",1),
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
  ioman->Register("TrdCluster","Trd Cluster",fClusters,kTRUE);
  
  return kSUCCESS;
  
} 
// --------------------------------------------------------------------

// ---- Exec ----------------------------------------------------------
void CbmTrdClusterFinderFast::Exec(Option_t * option)
{
  /*
    Digis are sorted according to the moduleId. A combiId is calculted based 
    on the rowId and the colId to have a neighbouring criterion for digis within 
    the same pad row. The digis of each module are sorted according to this combiId.
    All sorted digis of one pad row are 'clustered' into rowCluster. For a new row
    the rowClusters are compared to the rowClusters of the last row. If an overlap 
    is found they are marked to be parents(last row) and childrens(activ row) 
    (mergeRowCluster()). After this, the finale clusters are created. Therefor 
    walkCluster() walks along the list of marked parents and markes every visited 
    rowCluster to avoid multiple usage of one rowCluster. drawCluster() can be used to
    get a visual output.
  */
  cout << "================CbmTrdClusterFinderFast===============" << endl;
  Int_t nentries = fDigis->GetEntries();
  Int_t digiCounter = 0;
  cout << " Found " << nentries << " Digis in Collection" << endl;
  std::map<Int_t, MyDigiList*> modules; //map of <moduleId, List of struct 'MyDigi' pointer>

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

	//'rotated' modules are backrotated to get consistent notation of Row and Col
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
  std::map<Int_t, ClusterList*> fModClusterMap; //map of <moduleId, pointer of Vector of List of struct 'MyDigi' >
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
  /*
    3 new Lists are initialized: 
    currentList for the activ row, 
    openList for the previous row and 
    closedList for finished rows.
    Digis in the TCloneArray digis are sorted according to their combiId (rowId * (nCol+1) + colId).
  */
  std::list<RowCluster*> closedList;
  std::list<RowCluster*> openList;
  std::list<RowCluster*> currentList;
  RowCluster *currentCluster = new RowCluster;

  digis->sort(digiSorter);
 
  for (MyDigiList::iterator it = digis->begin(); it != digis->end(); ++it) {
    if (currentCluster->digis->empty() || 
	currentCluster->digis->back()->combiId + 1 == (*it)->combiId) {
      /*
	Digis within the same row are related to the 'currentCluster' if they are closed-by.
      */
      currentCluster->digis->push_back(*it);
    }
    else {
      /*
	the currentCluster is stored to the currentList
	(and merged to the clusters of the previous row) 
	if a break between the activ digi and the currentCluster is found.
      */
      mergeRowCluster(currentCluster, &openList);
      currentList.push_back(currentCluster);
 
      if ((*it)->rowId > currentCluster->row) {
	/*
	  did we move into a new row?      
	  The openList is spliced to the end of the closedList if the next row is reached
	*/
	closedList.splice(closedList.end(), openList);

	if ((*it)->rowId > currentCluster->row + 1) {
	  /*
	    did we skip at least one line?
	    Then the currentList is directly spliced to the end of the closedList since the 
	    cluster is definitely finished.
	  */
	  closedList.splice(closedList.end(), currentList);
	}
	else {
	  /*
	    if we didn't skip a line, the currentList ist spliced to the end of the openList.
	  */
	  openList.splice(openList.begin(), currentList);
	}
      }
      /*
	A new currentCluster is initialized if the activ digi is not closed-by the currentCluster.
      */
      currentCluster = new RowCluster;
      currentCluster->digis->push_back(*it);
    }
  }
  /*
    handle last row
  */
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
  /*
    finish up the current rowCluster
    Limitations of the currentCluster are compared to the limitations of the RowClusters of the openList.
    They are associated to be 'child' (currentList = activ row) and 'parent' (openList = previous row) 
    by pointer if a overlap of the limitations are found.
  */
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
  /*
    all found RowClusters are merged to clusters and copyed to the finale clusterList
    since the rowClusterList is empty
  */
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
  /*
    the rowClusters are sorted in the neighbour list if conected by a cild oder parent pointer.
    This pointer is removed and the rowCluster is marked as 'hasBeenVisited'. 
    Therefor it can not be merged to any cluster.
   */
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
  Int_t ClusterSum = 0;
  Float_t Charge;
  Float_t qMax;
  for (std::map<Int_t, ClusterList*>::iterator iMod = fModClusterMap.begin(); iMod != fModClusterMap.end(); ++iMod) 
    {
      for (ClusterList::iterator iCluster = (iMod->second)->begin(); iCluster != (iMod->second)->end(); iCluster++) 
	{
	  ClusterSum++;
	  TArrayI digiIndices( (*iCluster)->size() );
	  Int_t i = 0;
	  Charge = 0;
	  qMax = 0;
	  for (MyDigiList::iterator iDigi = (*iCluster)->begin(); iDigi != (*iCluster)->end(); iDigi++) 
	    {
              digiIndices.SetAt((*iDigi)->digiId,i);
	      Charge += (*iDigi)->charge;
	      i++;
	      if (qMax < (*iDigi)->charge)
		{
		  qMax = (*iDigi)->charge;
		}
	    }
	  //here is an error creating an segmentation violation in the following classes. 
          TClonesArray& clref = *fClusters;
          Int_t size = clref.GetEntriesFast();

	  new ((*fClusters)[size]) CbmTrdCluster(digiIndices, Charge, qMax);
	}
       
    }
 
  cout << " Found " << ClusterSum << " Cluster" << endl;
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
