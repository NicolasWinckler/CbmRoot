#include "CbmTrdDigitizer.h"

#include "CbmTrdDigiPar.h"
#include "CbmTrdPoint.h"
#include "CbmTrdDigi.h"
#include "CbmTrdDigiMatch.h"
#include "CbmTrdModule.h"
#include "CbmTrdRadiator.h"

#include "CbmMCTrack.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"

#include "TRandom.h"
#include "TMath.h"
#include "TVector3.h"
#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TGeoMaterial.h"
#include "TGeoNode.h"
#include "TGeoMatrix.h"
#include "TGeoBBox.h"
#include "TPRegexp.h"
#if ROOT_VERSION_CODE < ROOT_VERSION(5,20,0)
#include "CbmTrdStringToken.h"
#endif

#include <iostream>
#include <iomanip>
using std::cout;
using std::endl;
using std::pair;
using std::setprecision;


// ---- Default constructor -------------------------------------------
CbmTrdDigitizer::CbmTrdDigitizer()
  : FairTask("TrdDigitizer",1),
    fCol(-1),
    fRow(-1),
    fModuleID(-1),
    fMCindex(-1),
    fELoss(-1.),
    fTime(-1.),
    fEfficiency(1.),
    fTrdPoints(NULL),
    fDigiCollection(NULL),
    fDigiMatchCollection(NULL),
    fMCStack(NULL),
    fDigiPar(NULL),
    fModuleInfo(NULL),
    fRadiator(new CbmTrdRadiator),
    fTrdId(),
    fDigiMap(),
    fDigiMapIt()
{
}
// --------------------------------------------------------------------

// ---- Constructor ----------------------------------------------------
CbmTrdDigitizer::CbmTrdDigitizer(const char *name, const char *title,
                 CbmTrdRadiator *radiator, Int_t iVerbose)
  : FairTask(name, iVerbose),
    fCol(-1),
    fRow(-1),
    fModuleID(-1),
    fMCindex(-1),
    fELoss(-1.),
    fTime(-1.),
    fEfficiency(1.),
    fTrdPoints(NULL),
    fDigiCollection(NULL),
    fDigiMatchCollection(NULL),
    fMCStack(NULL),
    fDigiPar(NULL),
    fModuleInfo(NULL),
    fRadiator(radiator),
    fTrdId(),
    fDigiMap(),
    fDigiMapIt()
{
}
// --------------------------------------------------------------------

// ---- Destructor ----------------------------------------------------
CbmTrdDigitizer::~CbmTrdDigitizer()
{
  //    FairRootManager *ioman =FairRootManager::Instance();
  //ioman->Write();
  fDigiCollection->Clear("C");
  delete fDigiCollection;
  fDigiMatchCollection->Clear("C");
  delete fDigiMatchCollection;

}
// --------------------------------------------------------------------

// ----  Initialisation  ----------------------------------------------
void CbmTrdDigitizer::SetParContainers()
{
    cout<<" * CbmTrdDigitizer * :: SetParContainers() "<<endl;


    // Get Base Container
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb=ana->GetRuntimeDb();

    fDigiPar = (CbmTrdDigiPar*)
               (rtdb->getContainer("CbmTrdDigiPar"));

}
// --------------------------------------------------------------------

// ---- ReInit  -------------------------------------------------------
InitStatus CbmTrdDigitizer::ReInit(){

  cout<<" * CbmTrdDigitizer * :: ReInit() "<<endl;


  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();

  fDigiPar = (CbmTrdDigiPar*)
      (rtdb->getContainer("CbmTrdDigiPar"));
  
  return kSUCCESS;
}
// --------------------------------------------------------------------

// ---- Init ----------------------------------------------------------
InitStatus CbmTrdDigitizer::Init()
{

    cout<<" * CbmTrdDigitizer * :: Init() "<<endl;

    FairRootManager *ioman = FairRootManager::Instance();
    if ( ! ioman ) Fatal("Init", "No FairRootManager");
    
    fTrdPoints=(TClonesArray *)  
      ioman->GetObject("TrdPoint");
 
    if ( ! fTrdPoints ) {
      cout << "-W CbmTrdDigitizer::Init: No TrdPoints array!" << endl;
      cout << "                            Task will be inactive" << endl;
      return kERROR;
    }

    fMCStack = (TClonesArray*)ioman->GetObject("MCTrack");

    fDigiCollection = new TClonesArray("CbmTrdDigi", 100);
    ioman->Register("TrdDigi","TRD Digis",fDigiCollection,kTRUE);

    fDigiMatchCollection = new TClonesArray("CbmTrdDigiMatch", 100);
    ioman->Register("TrdDigiMatch","TRD Digis",fDigiMatchCollection,kTRUE);

    fRadiator->Init();

    return kSUCCESS;

}
// --------------------------------------------------------------------


// ---- Exec ----------------------------------------------------------
void CbmTrdDigitizer::Exec(Option_t * option)
{

  CbmTrdPoint *pt=NULL;
  
  TVector3 mom;
  Double_t ELoss;
  Double_t ELossTR;     // TR energy loss for e- & e+
  Double_t ELossdEdX;   // ionization energy loss
  
  for (int j=0; j <  fTrdPoints->GetEntriesFast() ; j++ ) {

    // if random value above fEfficency reject point
    if (gRandom->Rndm() > fEfficiency ) continue;
 
    pt = (CbmTrdPoint*) fTrdPoints->At(j);
  
    if(NULL == pt) continue;

    pt->Momentum(mom);
    fMCindex=pt->GetTrackID();

    CbmMCTrack *p= (CbmMCTrack*) fMCStack->At(fMCindex);
    if(NULL == p) continue;

    Int_t pdgCode = p->GetPdgCode();

    ELossdEdX = pt->GetEnergyLoss();
    ELoss     = ELossdEdX;

    // Calculate TR
    // Sorry, Electrons & Positrons only
    //	if(TMath::Abs(pdgCode) == 11 && mom.Z() > 0.5){  //0.5
    if(TMath::Abs(pdgCode) == 11){ 

       ELossTR = fRadiator->GetTR(mom);

       ELoss += ELossTR;
    }
    fELoss = ELoss;

    fTime = pt->GetTime();

    Int_t Sector;

    // Get pointer to the correct TRD module 
    // Extract the information which digi
    // Row/Col/Sector has fired for the TrdPoint
    // Encode the Sector info into the detector ID and
    // finally add the digi to the map
    fModuleInfo = fDigiPar->GetModule(pt->GetDetectorID());
    fModuleInfo->GetPadInfo(pt, fCol, fRow, Sector);
    fModuleID = fTrdId.SetSector(pt->GetDetectorID(), Sector);

    AddDigi(j);

  }

  // Fill data from internaly used stl map into output TClonesArray
  // Fill also the DigiMatch output which holds for each digi the MC indexes
  // which give a contribution to the digi
  Int_t iDigi=0; 
  for ( fDigiMapIt=fDigiMap.begin() ; fDigiMapIt != fDigiMap.end(); 
        fDigiMapIt++ ) {
    new ((*fDigiCollection)[iDigi]) 
        CbmTrdDigi(fDigiMapIt->second->GetDetId(), 
        fDigiMapIt->second->GetCol(), fDigiMapIt->second->GetRow(), 
        fDigiMapIt->second->GetCharge(),fDigiMapIt->second->GetTime());

    CbmTrdDigiMatch *p = new ((*fDigiMatchCollection)[iDigi]) CbmTrdDigiMatch(); 

    std::vector<int> arr=fDigiMapIt->second->GetMCIndex();
    std::vector<int>::iterator it;

    for (it=arr.begin() ; it <arr.end(); it++  ) {
      Int_t bla = p->AddPoint((Int_t)*it);
    }

    iDigi++;

  }


}
// --------------------------------------------------------------------

// ---- FinishTask-----------------------------------------------------
void CbmTrdDigitizer::FinishEvent()
{
  fDigiMap.clear();
  if ( fDigiCollection ) fDigiCollection->Clear();
  if ( fDigiMatchCollection ) fDigiMatchCollection->Clear();

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------

void CbmTrdDigitizer::AddDigi(const Int_t pointID) {
  // Add digi for pixel(x,y) in module to map for intermediate storage
  // In case the pixel for this pixel/module combination does not exists
  // it is added to the map.
  // In case it exists already the information about another hit in this
  // pixel is added. Also the additional energy loss is added to the pixel.


    // Look for pixel in charge map
    pair<Int_t, Int_t> a(fCol, fRow);
    pair<Int_t, pair<Int_t,Int_t> > b(fModuleID, a);
    fDigiMapIt = fDigiMap.find(b);

    //    cout<<"DetID: "<<fModuleID<<endl;

    // Pixel not yet in map -> Add new pixel
    if ( fDigiMapIt == fDigiMap.end() ) {
      CbmTrdDigi* digi = new CbmTrdDigi(fModuleID, fCol, fRow, fELoss, 
                                        fTime, pointID);
      fDigiMap[b] = digi;
    }

    // Pixel already in map -> Add charge
    else {
        CbmTrdDigi* digi = (CbmTrdDigi*)fDigiMapIt->second;
        if ( ! digi ) Fatal("AddChargeToPixel", "Zero pointer in digi map!");
        digi->AddCharge(fELoss);
        digi->AddMCIndex(pointID);
        if( fTime > (digi->GetTime()) ) digi->SetTime(fTime);
    }

}


// ---- Register ------------------------------------------------------
void CbmTrdDigitizer::Register(){

  FairRootManager::Instance()->Register("TrdDigi","Trd Digi", fDigiCollection, kTRUE);
  FairRootManager::Instance()->Register("TrdDigiMatch","Trd Digi Match", fDigiMatchCollection, kTRUE);

}
// --------------------------------------------------------------------

ClassImp(CbmTrdDigitizer)
