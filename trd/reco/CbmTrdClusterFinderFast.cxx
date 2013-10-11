#include "CbmTrdClusterFinderFast.h"

#include "CbmTrdDigiPar.h"
#include "CbmTrdDigi.h"
#include "CbmTrdCluster.h"
#include "CbmTrdModule.h"
#include "CbmTrdGeoHandler.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"
#include "TArray.h"
#include "TH2F.h"
#include "TH1F.h"
#include "TProfile.h"
#include "TCanvas.h"
#include "TImage.h"
#include "TStopwatch.h"

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
   fGeoHandler(new CbmTrdGeoHandler()),
   ClusterSum(-1),
   fMinimumChargeTH(1.0e-08),
   fMultiHit(true),
   fRowClusterMerger(false),
   fNeighbourReadout(true)
{
}
CbmTrdClusterFinderFast::CbmTrdClusterFinderFast(Bool_t MultiHit, Bool_t NeighbourReadout, Bool_t RowClusterMerger, Double_t MinimumChargeTH)
  :FairTask("TrdClusterFinderFast",1),
   fDigis(NULL),
   fClusters(NULL),
   fDigiPar(NULL),
   fModuleInfo(NULL),
   fGeoHandler(new CbmTrdGeoHandler()),
   ClusterSum(-1),
   fMinimumChargeTH(MinimumChargeTH),
   fMultiHit(MultiHit),
   fRowClusterMerger(RowClusterMerger),
   fNeighbourReadout(NeighbourReadout)
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
  
  fDigis =(TClonesArray *)  ioman->GetObject("TrdDigi");
  if ( ! fDigis ) {
    cout << "-W CbmTrdHitProducerDigi::Init: No TrdDigi array!" << endl;
    cout << "                            Task will be inactive" << endl;
    return kERROR;
  }
 
  fClusters = new TClonesArray("CbmTrdCluster", 100);
  ioman->Register("TrdCluster","Trd Cluster",fClusters,kTRUE);
  
  fGeoHandler->Init();

  return kSUCCESS;
  
} 
// --------------------------------------------------------------------

// ---- Exec ----------------------------------------------------------
void CbmTrdClusterFinderFast::Exec(Option_t *option)
{
  TStopwatch timer;
  timer.Start();
  cout << "================CbmTrdClusterFinderFast===============" << endl;
  Int_t counterI = 0;
  Int_t counterJ = 0;
  Float_t mRMax = 7500;
  Bool_t dynamic = true;
  Bool_t optimization = false;//true;//false;//
  if (optimization) {
    dynamic = false;
    fRowClusterMerger = true;
    cout << "  minimum charge threshold optimization run" << endl <<"  fRowClusterMerger: on" << endl;
  }
  else {
    if (fMultiHit){
      cout << "  multi hit detection: on" << endl;
    }
    else {
      cout << "  multi hit detection: off" << endl;
      if (dynamic) {
	cout << "  fRowClusterMerger: on (as function of module distance (r_min = " << mRMax <<" mm) from beam pipe)"<< endl;
      }
      else {
	if (fRowClusterMerger) {
	  cout << "  fRowClusterMerger: on" << endl;
	}
	else {
	  cout << "  fRowClusterMerger: off" << endl;
	}
      }
    }
  }
  //optimization ist only usefull if rowClusters are merged !!!
 
  Float_t mChargeTH[84] = { 0.003, 
			    /*2e-06,*/ //for gausssian distributed charge
			    0, 
			    1e-09, 2e-09, 3e-09, 4e-09, 5e-09, 6e-09, 7e-09, 8e-09, 9e-09,
			    1e-08, 2e-08, 3e-08, 4e-08, 5e-08, 6e-08, 7e-08, 8e-08, 9e-08,
			    1e-07, 2e-07, 3e-07, 4e-07, 5e-07, 6e-07, 7e-07, 8e-07, 9e-07,
			    1e-06, 2e-06, 3e-06, 4e-06, 5e-06, 6e-06, 7e-06, 8e-06, 9e-06,
			    1e-05, 2e-05, 3e-05, 4e-05, 5e-05, 6e-05, 7e-05, 8e-05, 9e-05,
			    1e-04, 2e-04, 3e-04, 4e-04, 5e-04, 6e-04, 7e-04, 8e-04, 9e-04,
			    1e-03, 2e-03, 3e-03, 4e-03, 5e-03, 6e-03, 7e-03, 8e-03, 9e-03,
			    1e-02, 2e-02, 3e-02, 4e-02, 5e-02, 6e-02, 7e-02, 8e-02, 9e-02,
			    1e-01, 2e-01, 3e-01, 4e-01, 5e-01, 6e-01, 7e-01, 8e-01, 9e-01,
			    1e-00  };
  Int_t nChargeTH = 1;

  TH1F* DigiChargeSpectrum = NULL;
  TProfile* OptimalTH = NULL;

  if (optimization)
    {
      nChargeTH = 84;
      OptimalTH = new TProfile("OptimalTH","Optimization of the fMinimumChargeTH",Int_t(1/mChargeTH[2]),0,mChargeTH[nChargeTH-1]);
      OptimalTH->SetXTitle("fMinimumChargeTH value");
      OptimalTH->SetYTitle("Number of found clusters");
      OptimalTH->SetStats(kFALSE);
      OptimalTH->GetXaxis()->SetLabelSize(0.02);
      OptimalTH->GetYaxis()->SetLabelSize(0.02);
      OptimalTH->GetXaxis()->SetTitleSize(0.02);
      OptimalTH->GetXaxis()->SetTitleOffset(1.5);
      OptimalTH->GetYaxis()->SetTitleSize(0.02);
      OptimalTH->GetYaxis()->SetTitleOffset(2);
      OptimalTH->SetMarkerStyle(4);
      DigiChargeSpectrum = new TH1F("DigiChargeSpectrum","DigiChargeSpectrum",1e6,0,1);
      DigiChargeSpectrum->SetXTitle("Digi charge [a.u.]");
      DigiChargeSpectrum->SetYTitle("Number of Digis");
      DigiChargeSpectrum->SetStats(kFALSE);
      DigiChargeSpectrum->GetXaxis()->SetLabelSize(0.02);
      DigiChargeSpectrum->GetYaxis()->SetLabelSize(0.02);
      DigiChargeSpectrum->GetXaxis()->SetTitleSize(0.02);
      DigiChargeSpectrum->GetXaxis()->SetTitleOffset(1.5);
      DigiChargeSpectrum->GetYaxis()->SetTitleSize(0.02);
      DigiChargeSpectrum->GetYaxis()->SetTitleOffset(2);
      //DigiChargeSpectrum->SetMarkerStyle(4);
    }
  //  MyDigi *d;

  for (Int_t iChargeTH = 0; iChargeTH < nChargeTH; iChargeTH++)
    {
      /*
	if (fMultiHit)
	fMinimumChargeTH = 1.0e-08;
	else
	fMinimumChargeTH = mChargeTH[iChargeTH];
      */
      //fMinimumChargeTH = 0.0;
      /*
       * Digis are sorted according to the moduleAddress. A combiId is calculted based 
       * on the rowId and the colId to have a neighbouring criterion for digis within 
       * the same pad row. The digis of each module are sorted according to this combiId.
       * All sorted digis of one pad row are 'clustered' into rowCluster. For a new row
       * the rowClusters are compared to the rowClusters of the last row. If an overlap 
       * is found they are marked to be parents(last row) and childrens(activ row) 
       * (mergeRowCluster()). After this, the finale clusters are created. Therefor 
       * walkCluster() walks along the list of marked parents and markes every visited 
       * rowCluster to avoid multiple usage of one rowCluster. drawCluster() can be used to
       * get a visual output.
       */
      
      Int_t nentries = fDigis->GetEntries();
      Int_t digiCounter = 0;
      cout << " Found " << nentries << " Digis in Collection" << endl;
      std::map<Int_t, MyDigiList*> modules; //map of <moduleAddress, List of struct 'MyDigi' pointer>
      std::map<Int_t, MyDigiList*> ModuleNeighbourDigiMap; //map of <moduleAddress, List of struct 'MyDigi' pointer>

      for (Int_t iDigi=0; iDigi < nentries; iDigi++ ) {
	CbmTrdDigi *digi = (CbmTrdDigi*) fDigis->At(iDigi);
	
	/*
	//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
	Int_t size = digi->GetMCIndex().size();
	std::vector<int> MCIndex = digi->GetMCIndex();
	cout << size << ": ";
	for (Int_t i = 0; i < size; i++) {
	cout << " ," << MCIndex[i];
	}
	cout << endl;
	//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
	*/
	/*
	  if (digi->GetCharge() > fMinimumChargeTH)
	  {
	*/
	/*
	 *unrotate rotated modules in x- and y-direction (row <-> col)
	 */
	Int_t digiAddress  = digi->GetAddress();
	Int_t layerId    = CbmTrdAddress::GetLayerId(digiAddress);
	Int_t moduleId = CbmTrdAddress::GetModuleId(digiAddress);
	Int_t moduleAddress = CbmTrdAddress::GetModuleAddress(digiAddress);

	//Int_t Station  = fGeoHandler->GetStation(moduleId);
	//Int_t Layer    = fGeoHandler->GetLayer(moduleId);
	//printf("digiAddress:%i  modId:%i  layer:%i\n",digiAddress,moduleId,layerId);
	MyDigi *d = new MyDigi;
	//d = new MyDigi;
	    
	fModuleInfo = fDigiPar->GetModule(CbmTrdAddress::GetModuleAddress(digiAddress));
	if (fModuleInfo){
	  printf("digiAddress %i found\n",digiAddress);
	} else {
	  printf("digi %3i digiAddress %i layer %i and modId %i  Sec%i Row:%i Col%i not found\n",
		 iDigi,digiAddress,layerId,moduleId,CbmTrdAddress::GetSectorId(digi->GetAddress()),
		 CbmTrdAddress::GetRowId(digi->GetAddress()),CbmTrdAddress::GetColumnId(digi->GetAddress())); 
	  continue;
	}
	d->digiId = iDigi;
	/*
	 *'rotated' modules are backrotated to get consistent notation of Row and Col
	 *//* // not longer needed since all modules ar in a local not rotated cs
	      if (layerId%2 == 0) {
	      d->colId = digi->GetRow();
	      d->rowId = digi->GetCol();
	      d->combiId = d->rowId * (fModuleInfo->GetNofRows() + 1) + d->colId;
	      }
	      else {
	      d->rowId = digi->GetRow();
	      d->colId = digi->GetCol();
	      d->combiId = d->rowId * (fModuleInfo->GetNofColumns() + 1) + d->colId;
	      }
	   */
	Int_t iSector = CbmTrdAddress::GetSectorId(digi->GetAddress());
	Int_t globalRow = CbmTrdAddress::GetRowId(digi->GetAddress());
	for (Int_t iS = 0; iS < iSector; iS++)
	  globalRow += fModuleInfo->GetNofRowsInSector(iS);

	d->colId = CbmTrdAddress::GetColumnId(digi->GetAddress());
	d->rowId = globalRow;
	d->combiId = d->rowId * (fModuleInfo->GetNofRows() + 1) + d->colId;

	d->charge = digi->GetCharge();

	if (optimization && fMinimumChargeTH == 0)
	  {
	    DigiChargeSpectrum->Fill(digi->GetCharge());
	  }
 
	if (ModuleNeighbourDigiMap.find(moduleAddress) == ModuleNeighbourDigiMap.end()) {
	     
	  ModuleNeighbourDigiMap[moduleAddress] = new MyDigiList;
	} 
	ModuleNeighbourDigiMap[moduleAddress]->push_back(d);
	if (digi->GetCharge() > fMinimumChargeTH)
	  {
	    digiCounter++;
	    if (modules.find(moduleAddress) == modules.end()) {
	     
	      modules[moduleAddress] = new MyDigiList;
	    } 
	    modules[moduleAddress]->push_back(d);
	  }
	// Since the pointer is stored in a stl container which is used later out of the scope of this loop
	// the pointer is set to NULL here to make clear that the objects must deleted elswhere. If the objects 
	// are not deleted later this is a perfect memory leak. 
	// The problem here is that the pointers are stored in several stl containers, so one has to use the stl
	// container holding all pointers to delete the objects. 
	d = NULL;
	//	delete d;
      }
      cout << " Used  " << digiCounter << " Digis after Minimum Charge Cut (" << fMinimumChargeTH << ")" << endl;
      std::map<Int_t, ClusterList*> fModClusterMap; //map of <moduleAddress, pointer of Vector of List of struct 'MyDigi' >
      for (std::map<Int_t, MyDigiList*>::iterator it = modules.begin(); it != modules.end(); ++it) {
	if (dynamic) {
	  fModuleInfo = fDigiPar->GetModule(it->first);
	  
	  Float_t mSizeX   = (fModuleInfo->GetSizeX()) * 2 * 10;
	  Float_t mSizeY   = (fModuleInfo->GetSizeY()) * 2 * 10;
	  Float_t mPosXC   = fModuleInfo->GetX() * 10;
	  Float_t mPosYC   = fModuleInfo->GetY() * 10;
	  Float_t mR = sqrt(pow(mPosXC,2) + pow(mPosYC,2));
	  counterJ++;
	  if (mR > mRMax) {
	    fRowClusterMerger = true;
	    counterI++;
	    //cout << it->first << "   "<< mR << endl;
	  }
	  else {
	    fRowClusterMerger = false;
	    //cout << "  " << it->first << "   " << mR <<endl;
	  }
	}
	fModClusterMap[it->first] = clusterModule(it->second, ModuleNeighbourDigiMap[it->first]);
	//drawCluster(it->first, fModClusterMap[it->first]);
      }
      cout << "addCluster(fModClusterMap)" << endl;
      addCluster(fModClusterMap);
      /*
       * clean up
       */
      // Here is now the point to delete all objects which are stored in stl containers. 
      // ModuleNeighbourDigiMap hols all pointers to objects of type MyDigi, so this is
      // the correct container to delete all of them. For the other stl containers one
      // has only to delete the lists and not the MyDigi stored in the lists.
      for (std::map<Int_t, MyDigiList*>::iterator it =  ModuleNeighbourDigiMap.begin(); 
	   it !=  ModuleNeighbourDigiMap.end(); ++it) {
	for (MyDigiList::iterator digisIt = it->second->begin(); 
	     digisIt != it->second->end(); ++digisIt) {
	  delete *digisIt;
	}
	delete it->second;
      }
      ModuleNeighbourDigiMap.clear();

      for (std::map<Int_t, ClusterList*>::iterator it = fModClusterMap.begin(); 
	   it != fModClusterMap.end(); ++it) {
	for (ClusterList::iterator clusterIt = it->second->begin(); 
	     clusterIt != it->second->end(); ++clusterIt) {
	  /*
	    for (MyDigiList::iterator digisIt = (*clusterIt)->begin(); 
	    digisIt != (*clusterIt)->end(); ++digisIt) {
	    delete *digisIt;
	    }
	  */
	  delete *clusterIt;
	}
	delete it->second;
      }
      fModClusterMap.clear();
      
      for (std::map<Int_t, MyDigiList*>::iterator it = modules.begin();
	   it != modules.end(); ++it) {
	/*
	  for (MyDigiList::iterator digisIt = it->second->begin(); 
	  digisIt != it->second->end(); ++digisIt) {
	  delete *digisIt;
	  }
	*/
	delete it->second;
      }
      modules.clear();
      if (optimization)
	{
	  OptimalTH->Fill(mChargeTH[iChargeTH],ClusterSum);
	}
    } //for (iChargeTH)
  //     delete d;
  if (optimization)
    {
      TCanvas* c = new TCanvas ("c","c",1600,800);
      c->Divide(2,1);
      c->cd(1)->SetLogx();
      OptimalTH->DrawCopy();
      c->cd(2)->SetLogx();
      c->cd(2)->SetLogy();
      DigiChargeSpectrum->DrawCopy();
      delete OptimalTH;
      delete DigiChargeSpectrum;
    }
  //cout << "  " << counterI << " (" << counterI*100.0/Float_t(counterJ) << "%)" <<  " are reconstructed with fRowClusterMerger" << endl;
  printf("   %4d modules (%6.3f%%) are reconstructed with fRowClusterMerger\n",counterI,counterI*100/Float_t(counterJ));
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();

  printf("\n\n******************** Reading Test  **********************\n");
  printf("   RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  printf("*********************************************************\n\n");

}

  //----------------------------------------------------------------------
  bool digiSorter(MyDigi *a, MyDigi *b)
  { return (a->combiId < b->combiId); }

  //----------------------------------------------------------------------
  ClusterList *CbmTrdClusterFinderFast::clusterModule(MyDigiList *digis, MyDigiList *neighbours)
  {
    /*
     *  3 new Lists are initialized: 
     *  currentList for the activ row, 
     *  openList for the previous row and 
     *  closedList for finished rows.
     *  Digis in the TCloneArray digis are sorted according to their combiId (rowId * (nCol+1) + colId).
     */
    std::list<RowCluster*> closedList;
    std::list<RowCluster*> openList;
    std::list<RowCluster*> currentList;
    RowCluster *currentCluster = new RowCluster;

    digis->sort(digiSorter);
    neighbours->sort(digiSorter);
    MyDigiList::iterator left  = neighbours->begin();
    MyDigiList::iterator right = neighbours->end();

    for (MyDigiList::iterator it = digis->begin(); it != digis->end(); ++it) {

      if (currentCluster->digis->empty() || 
	  currentCluster->digis->back()->combiId + 1 == (*it)->combiId) {
	/*
	 * Digis within the same row are related to the 'currentCluster' if they are closed-by.
	 */

	currentCluster->digis->push_back(*it);
      
      }
      else {
  
	/*
	 * the currentCluster is stored to the currentList
	 * (and merged to the clusters of the previous row) 
	 * if a break between the activ digi and the currentCluster is found.
	 */
	if (fRowClusterMerger) {
	  mergeRowCluster(currentCluster, &openList);
	}
	currentList.push_back(currentCluster);
 
	if ((*it)->rowId > currentCluster->row) {
	  /*
	   * did we move into a new row?      
	   * The openList is spliced to the end of the closedList if the next row is reached
	   */
	  closedList.splice(closedList.end(), openList);

	  if ((*it)->rowId > currentCluster->row + 1) {
	    /*
	     *  did we skip at least one line?
	     *  Then the currentList is directly spliced to the end of the closedList since the 
	     *  cluster is definitely finished.
	     */
	    closedList.splice(closedList.end(), currentList);
	  }
	  else {
	    /*
	     * if we didn't skip a line, the currentList ist spliced to the end of the openList.
	     */
	    openList.splice(openList.begin(), currentList);
	  }
	}
	/*
	 * A new currentCluster is initialized if the activ digi is not closed-by the currentCluster.
	 */
	currentCluster = new RowCluster;
	currentCluster->digis->push_back(*it);
      }
    }
    /*
     * handle last row
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
     *  finish up the current rowCluster
     *  Limitations of the currentCluster are compared to the limitations of the RowClusters of the openList.
     *  They are associated to be 'child' (currentList = activ row) and 'parent' (openList = previous row) 
     *  by pointer if a overlap of the limitations are found.
     */
    currentCluster->minCol = currentCluster->digis->front()->colId;
    currentCluster->maxCol = currentCluster->digis->back()->colId;
    currentCluster->row = currentCluster->digis->back()->rowId;
    /*
     * merge current RowCluster with openList
     */
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
     *  all found RowClusters are merged to clusters and copyed to the finale clusterList
     *  since the rowClusterList is empty
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
     * the rowClusters are sorted in the neighbour list if conected by a cild oder parent pointer.
     * This pointer is removed and the rowCluster is marked as 'hasBeenVisited'. 
     * Therefor it can not be merged to any cluster.
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
  void CbmTrdClusterFinderFast::drawCluster(Int_t moduleAddress, ClusterList *clusterList)
  {
    Char_t name[50];
    Char_t title[50];
    sprintf(name,"C%d",moduleAddress);
    sprintf(title,"%s",name);
    Char_t Canfile[100];
    sprintf(Canfile,"Pics/ModuleAddress%s.png",name);
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
  ClusterSum = 0;
  Float_t Charge;
  Float_t qMax;
  Float_t deltaCharge;
  Float_t preDelta;
  for (std::map<Int_t, ClusterList*>::iterator iMod = fModClusterMap.begin(); iMod != fModClusterMap.end(); ++iMod) 
    {
      for (ClusterList::iterator iCluster = (iMod->second)->begin(); iCluster != (iMod->second)->end(); iCluster++) 
	{
	  ClusterSum++;
	  std::vector<Int_t> digiIndices( 3/*(*iCluster)->size()*/ ); // andrey
	  Int_t idigi = 0;
	  Int_t i = 1;
	  Charge = 0;
	  qMax = 0;
	  deltaCharge = 0;
	  preDelta = 0;
	  MyDigiList::iterator lastDigi = (*iCluster)->begin();
	  MyDigiList::iterator nextDigi = (*iCluster)->begin();
	  for (MyDigiList::iterator iDigi = (*iCluster)->begin(); iDigi != (*iCluster)->end(); iDigi++) 
	    {
	      if (fMultiHit){
		if (idigi < (*iCluster)->size()-1) {
		  nextDigi++;
		}
		if (nextDigi != (*iCluster)->end()) {
		  //cout << (*iDigi)->charge << " " << (*nextDigi)->charge << "    ";
		  deltaCharge = (*nextDigi)->charge - (*iDigi)->charge;
		  if (preDelta >= 0 && deltaCharge <= 0) { // local maximum in charge distribution
		    //cout << "pre" << preDelta << " delta" << deltaCharge << endl;
		    //cout << (*lastDigi)->charge << " " << (*iDigi)->charge << " " << (*nextDigi)->charge << endl;
		    i = 1;
		    if ((*lastDigi) != (*iDigi))
		      i++;
		    if ((*nextDigi) != (*iDigi))
		      i++;

		    digiIndices.resize(i, 0.); // andrey
//		    digiIndices.Reset();
//		    digiIndices.Set(i);

		    i = 0;
		    if ((*lastDigi) != (*iDigi)){
		      digiIndices[i] = (*lastDigi)->digiId; // andrey
//		      digiIndices.SetAt((*lastDigi)->digiId,i);
		      Charge += (*lastDigi)->charge;
		      i++;
		    }

		    digiIndices[i] = (*iDigi)->digiId; // andrey
//		    digiIndices.SetAt((*iDigi)->digiId,i);
		    Charge += (*iDigi)->charge;
		    i++;
 
		    if ((*nextDigi) != (*iDigi)) {
		      digiIndices[i] = (*nextDigi)->digiId; // andrey
//		      digiIndices.SetAt((*nextDigi)->digiId,i);
		      Charge += (*nextDigi)->charge;
		    }

		    qMax = (*iDigi)->charge;

		    TClonesArray& clref = *fClusters;
		    Int_t size = clref.GetEntriesFast();

		    CbmTrdCluster* cluster = new ((*fClusters)[size]) CbmTrdCluster();
		    cluster->SetDigis(digiIndices);
		    //cluster->SetCharge(Charge);
		    //cluster->SetMaxCharge(qMax);
		    digiIndices.clear();
		    //digiIndices.Reset();
		    //digiIndices.Set(3);
		    Charge = 0;
		   
		  }
		  preDelta = deltaCharge;	     
		  lastDigi = iDigi;
		}
		//cout << endl;
	      }
	      else {
	    	  digiIndices[idigi] = (*iDigi)->digiId; // andrey
//		digiIndices.SetAt((*iDigi)->digiId,idigi);
		Charge += (*iDigi)->charge;
	
		if (qMax < (*iDigi)->charge)
		  {
		    qMax = (*iDigi)->charge;
		  }
	      }
	      idigi++;
	    }	 	  
	  if (!fMultiHit){
	    TClonesArray& clref = *fClusters;
	    Int_t size = clref.GetEntriesFast();

	    CbmTrdCluster* cluster = new ((*fClusters)[size]) CbmTrdCluster(); // andrey
	    cluster->SetDigis(digiIndices); // andrey
	    //cluster->SetCharge(Charge); // andrey
	    //cluster->SetMaxCharge(qMax); // andrey
	  }	  
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

  // -----   Public method EndOfEvent   --------------------------------------
  void CbmTrdClusterFinderFast::FinishEvent() {
    //  cout<<"In CbmTrdClusterFinderFast::FinishEvent()"<<endl;  
    if (fClusters) fClusters->Clear("C");
  }
  // -------------------------------------------------------------------------

  ClassImp(CbmTrdClusterFinderFast)
