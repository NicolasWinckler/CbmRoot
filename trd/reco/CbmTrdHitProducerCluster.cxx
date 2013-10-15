#include "CbmTrdHitProducerCluster.h"

#include "CbmTrdDigiPar.h"
#include "CbmTrdDigi.h"
#include "CbmTrdPoint.h"
#include "CbmTrdCluster.h"
#include "CbmTrdHit.h"
#include "CbmTrdModule.h"
#include "CbmTrdGeoHandler.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TGeoManager.h"
#include "TClonesArray.h"
#include "TArray.h"
#include "TF1.h"
#include "TH2F.h"
#include "TProfile.h"
#include "TLine.h"
#include "TCanvas.h"
#include "TImage.h"
#include "TLegend.h"
#include "TStopwatch.h"

//#include <map>
#include <iostream>
#include <iomanip>
#include <cmath>
using std::cout;
using std::endl;

// ---- Default constructor -------------------------------------------
CbmTrdHitProducerCluster::CbmTrdHitProducerCluster()
  :FairTask("CbmTrdHitProducerCluster",1),
   //fTrdPoints(NULL),
   fDigis(NULL),
   fClusters(NULL),
   fHits(NULL),
   fDigiPar(NULL),
   fModuleInfo(NULL),
   fGeoHandler(new CbmTrdGeoHandler()),
   /*
     fPrfSingleRecoCounter(-1.),
   fPrfDoubleRecoCounter(-1.),
   fSimpleRecoCounter(-1.),
   fStation(-1.),
   */
   fLayer(-1.),
   fModuleAddress(-1.),
   /*
   fxPos(0.),
   fyPos(0.),
   fzPos(0.),
   */
   fnCol(-1.),
   fnRow(-1.),
   fNoSectors(-1.),
   fClusterId(-1.),
   fHitCounter(0),
   /*
   fSectorSizeX(),
   fSectorSizeY(),
   fPadSizeX(),
   fPadSizeY(),
   fSecCol(),
   fSecRow(),
   fPadSizeXArray(),
   fPadSizeYArray(),
   fModuleSize(),
   fModulePosition(),
   fPadSizeLongMap(),
   ModuleHitMap(),
   moduleDigiMap(),
   fHitOutOfModuleCounter(0)
   */
   fHitMap()
{
}

// ---- Destructor ----------------------------------------------------
CbmTrdHitProducerCluster::~CbmTrdHitProducerCluster()
{
  /*
  if(fTrdPoints){
    fTrdPoints->Clear("C");
    fTrdPoints->Delete();
    delete fTrdPoints;
  }
  */
  if(fClusters){
    fClusters->Clear("C");
    fClusters->Delete();
    delete fClusters;
  }
  if(fDigis){
    fDigis->Clear("C");
    fDigis->Delete();
    delete fDigis;
  }    
  if(fHits){
    fHits->Clear("C");
    fHits->Delete();
    delete fHits;
  }    
  if(fDigiPar){
    delete fDigiPar;
  }
  if(fModuleInfo){
    delete fModuleInfo;
  }
}

// ----  Initialisation  ----------------------------------------------
void CbmTrdHitProducerCluster::SetParContainers()
{
  // Get Base Container
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  cout << " * CbmTrdHitProducerCluster * :: SetParContainers()" << endl;
  fDigiPar = (CbmTrdDigiPar*)(rtdb->getContainer("CbmTrdDigiPar"));
}
// ---- ReInit  -------------------------------------------------------
InitStatus CbmTrdHitProducerCluster::ReInit()
{  
  cout << " * CbmTrdHitProducerCluster * :: ReInit()" << endl;
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  
  fDigiPar = (CbmTrdDigiPar*)(rtdb->getContainer("CbmTrdDigiPar"));

  

  return kSUCCESS;
}
// ---- Init ----------------------------------------------------------
InitStatus CbmTrdHitProducerCluster::Init()
{
  cout << " * CbmTrdHitProducerCluster * :: Init()" << endl;
  FairRootManager *ioman = FairRootManager::Instance();

  fDigis =(TClonesArray *)  ioman->GetObject("TrdDigi");
  if ( ! fDigis ) {
    cout << "-W CbmTrdHitProducerCluster::Init: No TrdDigi array!" << endl;
    cout << "                             Task will be inactive" << endl;
    return kERROR;
  }
  fClusters =(TClonesArray *)  ioman->GetObject("TrdCluster");
  if ( ! fClusters ) {
    cout << "-W CbmTrdHitProducerCluster::Init: No TrdCluster array!" << endl;
    cout << "                             Task will be inactive" << endl;
    return kERROR;
  }    
  fHits = new TClonesArray("CbmTrdHit", 100);
  ioman->Register("TrdHit","TRD Hit",fHits,kTRUE);
  
  // Extract information about the number of TRD stations and
  // the number of layers per TRD station from the geomanager.
  // Store the information about the number of layers at the entrance
  // of subsequent stations in a vector. 
  // This allows to calculate the layer number starting with 1 for the
  // first layer of the first station at a later stage by only adding 
  // the layer number in the station to the number of layers in 
  // previous stations 

  fGeoHandler->Init();

  return kSUCCESS;
  
}

//----------------------------------------------------------------------
bool digiCidSorter(MyDigi *a, MyDigi *b)
{ return (a->combiId < b->combiId); }

// ---- Exec ----------------------------------------------------------
void CbmTrdHitProducerCluster::Exec(Option_t * option)
{ 
  TStopwatch timer;
  timer.Start();
  cout << "================CbmTrdHitProducerCluster==============" << endl;

  if (fClusters == NULL){
    cout << " DEBUG: fCluster is NULL" << endl;
    return;
  }
  fHitCounter = 0;
  Int_t nCluster = fClusters->GetEntries(); // Number of clusters found by CbmTrdClusterFinderFast
  cout << " Found " << nCluster << " Cluster in Collection" << endl;

  for (Int_t iCluster = 0; iCluster < nCluster; iCluster++) {
    //cout << iCluster << endl;
    CbmTrdCluster *cluster = (CbmTrdCluster*) fClusters->At(iCluster);//pointer to the acvit cluster
    fClusterId = iCluster;
    CenterOfCharge(cluster);

  }  

  printf("\n   Created %i TrdHits  %7.3f Hits/Cluster\n",fHitCounter, Double_t(fHitCounter/nCluster));
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();

  printf("\n\n******************** Reading Test  **********************\n");
  printf("   RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  printf("*********************************************************\n\n");

}
void CbmTrdHitProducerCluster::CenterOfCharge(const CbmTrdCluster* cluster)
{
  CbmTrdDigi *digi = NULL;
  Int_t nDigisInCluster = cluster->GetNofDigis();
  Double_t totalCharge(0), iCharge(0);
  //Double_t hit_pos[3] = {0,0,0};
  TVector3 hit_posV;
  Int_t moduleAddress = 0;

  TVector3 local_pad_posV;
  TVector3 local_pad_dposV;

  for (Int_t iDigi = 0; iDigi < nDigisInCluster; iDigi++){  
    digi = (CbmTrdDigi*)fDigis->At(cluster->GetDigi(iDigi));
   
    //if (iDigi == 0){
      moduleAddress = CbmTrdAddress::GetModuleAddress(digi->GetAddress());
      fModuleInfo = fDigiPar->GetModule(moduleAddress);  
      gGeoManager->FindNode(fModuleInfo->GetX(), fModuleInfo->GetY(), fModuleInfo->GetZ()); 
      //}
    if (!fModuleInfo){
      printf("No CbmTrdModule found!  digiAddress:%i  ModuleAddress:%i\n",digi->GetAddress(),CbmTrdAddress::GetModuleAddress(digi->GetAddress()));
      return;
    }
    iCharge = digi->GetCharge();
    totalCharge += iCharge;
    //Double_t local_pad_pos[3];
    fModuleInfo->GetPadPosition(digi->GetAddress(), local_pad_posV, local_pad_dposV);//local_pad_pos[0], local_pad_pos[1], local_pad_pos[2]);
    fModuleInfo->TransformHitError(local_pad_dposV);
    //printf("digi:%i charge:%E  (%7.3f, %7.3f, %7.3f)\n", iDigi, iCharge, local_pad_pos[0], local_pad_pos[1], local_pad_pos[2]);
    for (Int_t iDim = 0; iDim < 3; iDim++){
      hit_posV[iDim] += local_pad_posV[iDim] * iCharge;
    }
  }
  //printf("secId %i\n",CbmTrdAddress::GetSectorId(digi->GetAddress()));
  Double_t hit_pos[3];
  for (Int_t iDim = 0; iDim < 3; iDim++){
    hit_posV[iDim] /= totalCharge;
    hit_pos[iDim] = hit_posV[iDim]; //TVector3->Double_t[3]
  }
  Double_t global_hit[3];
  gGeoManager->LocalToMaster(hit_pos, global_hit);

  for (Int_t iDim = 0; iDim < 3; iDim++){
    hit_posV[iDim] = global_hit[iDim]; //Double_t[3]->TVector3 
  }
  fHitCounter++;
  new ((*fHits)[fClusterId]) CbmTrdHit( moduleAddress,
					hit_posV,
					local_pad_dposV,
					0,
					fClusterId,
					0,
					0,
					totalCharge);
}

  // ---- Register ------------------------------------------------------
  void CbmTrdHitProducerCluster::Register()
  {
    FairRootManager::Instance()->Register("TrdHit","Trd Hit", fHits, kTRUE);
  }
  // --------------------------------------------------------------------


  // ---- Finish --------------------------------------------------------
  void CbmTrdHitProducerCluster::Finish()
  {
  }

// -----   Public method EndOfEvent   --------------------------------------
void CbmTrdHitProducerCluster::FinishEvent() {
  if (fHits) fHits->Clear();
  if (fDigis) fDigis->Clear();
  if (fClusters) fClusters->Clear();
}
// -------------------------------------------------------------------------

  ClassImp(CbmTrdHitProducerCluster)
