#include "CbmTrdSimpleHitProducerCluster.h"

#include "CbmTrdDigiPar.h"
#include "CbmTrdRadiator.h"
#include "CbmTrdPoint.h"
#include "CbmTrdCluster.h"
#include "CbmTrdHit.h"
#include "CbmTrdDigi.h"
#include "CbmTrdModule.h"
#include "CbmTrdDigiMatch.h"

#include "FairRootManager.h"
#include "CbmMCTrack.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"

#include "TRandom.h"
#include "TMath.h"
#include "TVector3.h"
#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TGeoVolume.h"

#include <iostream>
#include <iomanip>
using std::cout;
using std::endl;
using std::setprecision;

// ---- Default constructor -------------------------------------------
CbmTrdSimpleHitProducerCluster::CbmTrdSimpleHitProducerCluster()
  : FairTask("TrdHitProducer",1),
    fTrdCluster(NULL),
    fTrdDigi(NULL),
    fHitCollection(NULL),
    fMCStack(NULL),
    fDigiPar(NULL),
    fModuleInfo(NULL),
    fTrdId(),
    fLayersBeforeStation()
{
}
// --------------------------------------------------------------------

// ---- Constructor ----------------------------------------------------
CbmTrdSimpleHitProducerCluster::CbmTrdSimpleHitProducerCluster(const char *name, const char *title, Int_t iVerbose)
  : FairTask(name, iVerbose),
    fTrdCluster(NULL),
    fTrdDigi(NULL),
    fHitCollection(NULL),
    fMCStack(NULL),
    fDigiPar(NULL),
    fModuleInfo(NULL),
    fTrdId(),
    fLayersBeforeStation()
{

}
// --------------------------------------------------------------------

// ---- Destructor ----------------------------------------------------
CbmTrdSimpleHitProducerCluster::~CbmTrdSimpleHitProducerCluster()
{
    fHitCollection->Clear("C");
    delete fHitCollection;

}
// --------------------------------------------------------------------

// ----  Initialisation  ----------------------------------------------
void CbmTrdSimpleHitProducerCluster::SetParContainers()
{
    cout<<" * HitProducer * :: SetParContainers() "<<endl;


    // Get Base Container
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb=ana->GetRuntimeDb();

    fDigiPar = (CbmTrdDigiPar*)(rtdb->getContainer("CbmTrdDigiPar"));

}
// --------------------------------------------------------------------

// ---- ReInit  -------------------------------------------------------
InitStatus CbmTrdSimpleHitProducerCluster::ReInit(){

    cout<<" * HitProducer * :: ReInit() "<<endl;


    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb=ana->GetRuntimeDb();

    fDigiPar = (CbmTrdDigiPar*)(rtdb->getContainer("CbmTrdDigiPar"));

    return kSUCCESS;
}
// --------------------------------------------------------------------

// ---- Init ----------------------------------------------------------
InitStatus CbmTrdSimpleHitProducerCluster::Init()
{

    cout<<" * HitProducer * :: Init() "<<endl;

    FairRootManager *ioman = FairRootManager::Instance();

    fTrdDigi =(TClonesArray *)  ioman->GetObject("TrdDigi");
    if ( ! fTrdDigi ) {
      cout << "-W CbmTrdSimpleHitProducerCluster::Init: No TrdDigi array!" << endl;
      cout << "                            Task will be inactive" << endl;
      return kERROR;
    }


    fTrdCluster =(TClonesArray *)  ioman->GetObject("TrdCluster");
    if ( ! fTrdDigi ) {
      cout << "-W CbmTrdSimpleHitProducerCluster::Init: No TrdCluster array!" << endl;
      cout << "                            Task will be inactive" << endl;
      return kERROR;
    }

    fHitCollection = new TClonesArray("CbmTrdHit", 100);
    ioman->Register("TrdHit","TRD",fHitCollection,kTRUE);

    // Extract information about the number of TRD stations and
    // the number of layers per TRD station from the geomanager.
    // Store the information about the number of layers at the entrance
    // of subsequent stations in a vector. 
    // This allows to calculate the layer number starting with 1 for the
    // first layer of the first station at a later stage by only adding 
    // the layer number in the station to the number of layers in 
    // previous stations 
    TGeoVolume *fm=NULL;
    Int_t stationNr = 1;
    Int_t totalNrOfLayers = 0;
    fLayersBeforeStation.push_back(totalNrOfLayers);
    char volumeName[10];
    sprintf(volumeName, "trd%d", stationNr);
    fm = (TGeoVolume *)gGeoManager->GetListOfVolumes()->FindObject(volumeName);  
    if (fm){
      Int_t nrOfLayers = fm->GetNdaughters();
      totalNrOfLayers += nrOfLayers;
      fLayersBeforeStation.push_back(totalNrOfLayers);
      do {
	stationNr++;
        sprintf(volumeName, "trd%d", stationNr);
	fm = (TGeoVolume *)gGeoManager->GetListOfVolumes()->FindObject(volumeName);  
        if (fm) {
          nrOfLayers = fm->GetNdaughters();
	  totalNrOfLayers += nrOfLayers;
	  fLayersBeforeStation.push_back(totalNrOfLayers);
	}
      } while (fm);
    } else {
      cout << "***************************************" <<endl;
      cout << "                                       " <<endl;
      cout << " - FATAL ERROR Unknown geometry version" <<endl;
      cout << "   in CbmTrdHitProducerSmearing        " <<endl;
      cout << " No TRD stations found in the geometry " <<endl;
      cout << "                                       " <<endl;
      cout << "***************************************" <<endl;
      return kFATAL;
    }

    return kSUCCESS;

}
// --------------------------------------------------------------------


// ---- Exec ----------------------------------------------------------
void CbmTrdSimpleHitProducerCluster::Exec(Option_t * option)
{

  fHitCollection->Clear();
  CbmTrdDigi *digi=NULL;
  CbmTrdCluster *cluster=NULL;
  
  Int_t Col, Row, DetId, moduleId, Plane;
  Int_t Station, Layer, ModuleType, ModuleCopy, Sector;
  Double_t xHit, yHit, zHit;
  Double_t xHitErr, yHitErr, zHitErr;
  Double_t ELoss;
  TVector3 posHit;
  TVector3 padSize;
    
  Int_t nentries = fTrdCluster->GetEntries();
  cout<<" ** "<<nentries<<" Trd hits to be created in this event** "<<endl;
  
  for (int j=0; j < nentries; j++ ) {
    
    cluster =  (CbmTrdCluster*) fTrdCluster->At(j);

    Int_t nofDigis=cluster->GetNDigis();

    if(nofDigis != 1 ) {
      cout<<"--E-- CbmTrdSimpleHitProducerCluster can only work with one digi per cluster. For this cluster ther have been "<<nofDigis<<" clusters"<<endl;

    }

    for (int k=0; k < nofDigis; k++ ) {
      Int_t digiIndex=cluster->GetDigiIndex(k);
      digi =  (CbmTrdDigi*) fTrdDigi->At(digiIndex);
    
      Col = digi->GetCol();
      Row = digi->GetRow();
      ELoss = digi-> GetCharge();
      DetId = digi->GetDetId();

      // The digi contains the information of the detector Id + the
      // sector number. The digitization parameters are stored per
      // module with arrays holding the information about the sectors.
      // So we have to extract the information about the module Id and
      // the sector from the detector Id.
      Int_t* bla = fTrdId.GetDetectorInfo(DetId);
      Station = bla[1];
      Layer = bla[2];
      ModuleType = bla[3];
      ModuleCopy = bla[4];
      Sector = bla[5];
      moduleId= fTrdId.GetModuleId(DetId);

      //TODO: This has to be done in a correct way. In the moment
      //      it is assumed that all stations have 4 layers which
      //      has not to be true for each geometry.
      //      Plane=fLayersBeforeStation[Station-1]+Layer;
      Plane= ((Station-1)*4) + Layer;

      fModuleInfo = fDigiPar->GetModule(moduleId);
      fModuleInfo->GetPosition(Col, Row, moduleId, Sector, posHit, padSize);
        
      // Calculate the hit error from the pad sizes
      padSize*=(1/TMath::Sqrt(12.));
      
      AddHit(posHit, padSize, DetId, Plane, j , ELoss, 0., 0.);
    }
  }
  
}
// --------------------------------------------------------------------

// ---- Add Hit to HitCollection --------------------------------------

void CbmTrdSimpleHitProducerCluster::AddHit(TVector3 &posHit, TVector3 &posHitErr,
			       Int_t detId, Int_t Plane, Int_t ref,
			       Double_t ELoss, Double_t ELossTR,
			       Double_t ELossdEdX){

    TClonesArray& clref = *fHitCollection;
    Int_t size = clref.GetEntriesFast();

    
//    new((*fHitCollection)[size]) CbmTrdHit(fDetId, posHit, posHitErr, ref,
//					   Plane, 0., 0., ELoss);

    new((*fHitCollection)[size]) CbmTrdHit(detId, posHit, posHitErr, 0., 
                            ref, Plane, 0., 0., ELoss);
}

// --------------------------------------------------------------------

// ---- Finish --------------------------------------------------------
void CbmTrdSimpleHitProducerCluster::Finish()
{
}
// --------------------------------------------------------------------

// ---- Register ------------------------------------------------------
void CbmTrdSimpleHitProducerCluster::Register(){

  FairRootManager::Instance()->Register("TrdHit","Trd", fHitCollection, kTRUE);

}

ClassImp(CbmTrdSimpleHitProducerCluster)
