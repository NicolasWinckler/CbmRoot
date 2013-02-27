#include "CbmTrdHitProducerDigi.h"

#include "CbmTrdDigiPar.h"
#include "CbmTrdRadiator.h"
#include "CbmTrdPoint.h"
#include "CbmTrdHit.h"
#include "CbmTrdDigi.h"
#include "CbmTrdModule.h"
#include "CbmTrdDigiMatch.h"
#include "CbmTrdGeoHandler.h"

#include "FairRootManager.h"
#include "CbmMCTrack.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"

#include "TRandom.h"
#include "TMath.h"
#include "TVector3.h"
#include "TClonesArray.h"

#include <iostream>
#include <iomanip>
using std::cout;
using std::endl;
using std::setprecision;

// ---- Default constructor -------------------------------------------
CbmTrdHitProducerDigi::CbmTrdHitProducerDigi()
  :FairTask("TrdHitProducer",1),
   fTrdDigi(NULL),
   fTrdDigiMatch(NULL),
   fHitCollection(new TClonesArray("CbmTrdHit", 100)),
   fMCStack(NULL),
   fDigiPar(NULL),
   fModuleInfo(NULL),
   fGeoHandler(new CbmTrdGeoHandler())
{
}
// --------------------------------------------------------------------

// ---- Constructor ----------------------------------------------------
CbmTrdHitProducerDigi::CbmTrdHitProducerDigi(const char *name, const char *title, Int_t iVerbose)
  :FairTask(name, iVerbose),
   fTrdDigi(NULL),
   fTrdDigiMatch(NULL),
   fHitCollection(new TClonesArray("CbmTrdHit", 100)),
   fMCStack(NULL),
   fDigiPar(NULL),
   fModuleInfo(NULL),
   fGeoHandler(new CbmTrdGeoHandler())
{
}
// --------------------------------------------------------------------

// ---- Destructor ----------------------------------------------------
CbmTrdHitProducerDigi::~CbmTrdHitProducerDigi()
{
  //    FairRootManager *ioman =FairRootManager::Instance();
  //    ioman->Write();
    fHitCollection->Clear("C");
    delete fHitCollection;

}
// --------------------------------------------------------------------

// ----  Initialisation  ----------------------------------------------
void CbmTrdHitProducerDigi::SetParContainers()
{
    cout<<" * HitProducer * :: SetParContainers() "<<endl;


    // Get Base Container
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb=ana->GetRuntimeDb();

    fDigiPar = (CbmTrdDigiPar*)(rtdb->getContainer("CbmTrdDigiPar"));

}
// --------------------------------------------------------------------

// ---- ReInit  -------------------------------------------------------
InitStatus CbmTrdHitProducerDigi::ReInit(){

    cout<<" * HitProducer * :: ReInit() "<<endl;


    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb=ana->GetRuntimeDb();

    fDigiPar = (CbmTrdDigiPar*)(rtdb->getContainer("CbmTrdDigiPar"));

    return kSUCCESS;
}
// --------------------------------------------------------------------

// ---- Init ----------------------------------------------------------
InitStatus CbmTrdHitProducerDigi::Init()
{

    cout<<" * HitProducer * :: Init() "<<endl;

    FairRootManager *ioman = FairRootManager::Instance();

    fTrdDigi =(TClonesArray *)  ioman->GetObject("TrdDigi");
    if ( ! fTrdDigi ) {
      cout << "-W CbmTrdHitProducerDigi::Init: No TrdDigi array!" << endl;
      cout << "                            Task will be inactive" << endl;
      return kERROR;
    }


    fTrdDigiMatch =(TClonesArray *)  ioman->GetObject("TrdDigiMatch");
    if ( ! fTrdDigi ) {
      cout << "-W CbmTrdHitProducerDigi::Init: No TrdDigiMatch array!" << endl;
      cout << "                            Task will be inactive" << endl;
      return kERROR;
    }

    fMCStack = (TClonesArray*)ioman->GetObject("MCTrack");

    ioman->Register("TrdHit","TRD",fHitCollection,kTRUE);

    // Extract information about the number of TRD stations and
    // the number of layers per TRD station from the geomanager.
    // Store the information about the number of layers at the entrance
    // of subsequent stations in a vector. 
    // This allows to calculate the layer number starting with 1 for the
    // first layer of the first station at a later stage by only adding 
    // the layer number in the station to the number of layers in 
    // previous stations 
    fGeoHandler->Init();
  
    cout<<"********** End of TRD Hitproducer init ********"<<endl;
    
    return kSUCCESS;


}
// --------------------------------------------------------------------


// ---- Exec ----------------------------------------------------------
void CbmTrdHitProducerDigi::Exec(Option_t * option)
{

  fHitCollection->Clear();
  CbmTrdDigi *digi=NULL;
  CbmTrdDigiMatch *digimatch=NULL;
  
  Int_t Col, Row, DetId, moduleId, Plane;
  Int_t Station, Layer, ModuleType, ModuleCopy, Sector;
  Double_t xHit, yHit, zHit;
  Double_t xHitErr, yHitErr, zHitErr;
  Double_t ELoss;
  TVector3 posHit;
  TVector3 padSize;
    
  Int_t nentries = fTrdDigi->GetEntries();
  cout<<" ** "<<nentries<<" Trd hits to be created in this event** "<<endl;
  
  for (int j=0; j < nentries; j++ ) {
    
    digi =  (CbmTrdDigi*) fTrdDigi->At(j);
    
    Col = digi->GetCol();
    Row = digi->GetRow();
    ELoss = digi-> GetCharge();
    DetId = digi->GetDetId();

    // The digi contains the information of the detector Id + the
    // sector number. The digitization parameters are stored per
    // module with arrays holding the information about the sectors.
    // So we have to extract the information about the module Id and
    // the sector from the detector Id.

    Station = fGeoHandler->GetStation(DetId);
    Layer = fGeoHandler->GetLayer(DetId);
    ModuleType = fGeoHandler->GetModuleType(DetId);
    ModuleCopy = fGeoHandler->GetModuleCopyNr(DetId);
    Sector = fGeoHandler->GetSector(DetId);
    moduleId = fGeoHandler->GetModuleId(DetId);
    Plane = fGeoHandler->GetPlane(DetId);

    fModuleInfo = fDigiPar->GetModule(moduleId);
    fModuleInfo->GetPosition(Col, Row, moduleId, Sector, posHit, padSize);
        
    // Calculate the hit error from the pad sizes
    padSize*=(1/TMath::Sqrt(12.));
        
    AddHit(posHit, padSize, DetId, Plane, j , ELoss, 0., 0.);
    
  }
  
}
// --------------------------------------------------------------------

// ---- Add Hit to HitCollection --------------------------------------

void CbmTrdHitProducerDigi::AddHit(TVector3 &posHit, TVector3 &posHitErr,
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
void CbmTrdHitProducerDigi::Finish()
{
}
// --------------------------------------------------------------------

// ---- Register ------------------------------------------------------
void CbmTrdHitProducerDigi::Register(){

  FairRootManager::Instance()->Register("TrdHit","Trd", fHitCollection, kTRUE);

}

ClassImp(CbmTrdHitProducerDigi)
