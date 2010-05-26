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
    :FairTask("TrdDigitizer",1)
{
 fRadiator = new CbmTrdRadiator();
 fEfficiency = 1;
}
// --------------------------------------------------------------------

// ---- Constructor ----------------------------------------------------
CbmTrdDigitizer::CbmTrdDigitizer(const char *name, const char *title,
                 CbmTrdRadiator *radiator, Int_t iVerbose)
	:FairTask(name, iVerbose)
{
  fRadiator = radiator;
  fEfficiency = 1;
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
      ioman->ActivateBranch("TrdPoint");
 
    if ( ! fTrdPoints ) {
      cout << "-W CbmTrdDigitizer::Init: No TrdPoints array!" << endl;
      cout << "                            Task will be inactive" << endl;
      return kERROR;
    }

    fMCStack = (TClonesArray*)ioman->ActivateBranch("MCTrack");

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

    // Calculate point in the middle of the detector. This is
    // for safety reasons, because the point at exit is slightly
    // outside of the active volume. If one does not use a point
    // in the detector one will get a wrong volume from the
    // geomanager. Navigate to the correct volume to get all
    // necessary information about this volume

    Double_t x_mean = (pt->GetXIn()+pt->GetXOut())/2.;
    Double_t y_mean = (pt->GetYIn()+pt->GetYOut())/2.;
    Double_t z_mean = (pt->GetZIn()+pt->GetZOut())/2.;
    gGeoManager->FindNode(x_mean, y_mean, z_mean);

    if ( fVerbose > 2 ) {
      cout<<"*** CbmTrdDigitizer::Exec ***"<<endl;
      cout<<setprecision(5)<<"xmean: "<<x_mean<<endl;
      cout<<setprecision(5)<<"ymean: "<<y_mean<<endl;
      cout<<setprecision(5)<<"zmean: "<<z_mean<<endl;
    }

    // Get the local point in local MC coordinates from
    // the geomanager. This coordinate system is rotated
    // if the chamber is rotated. This is corrected in 
    // GetModuleInformationFromDigiPar(..) to have a
    // the same local coordinate system in all the chambers
    const Double_t *global_point = gGeoManager->GetCurrentPoint();
    Double_t local_point[3];  // global_point[3];
    gGeoManager->MasterToLocal(global_point, local_point);

    fModuleID = pt->GetDetectorID();

    GetModuleInformationFromDigiPar(fModuleID, local_point);

    CalculatePixel();
	
    AddDigi();

  }

  cout <<"--II-- CbmTrdDigitizer: Used " << fTrdPoints->GetEntriesFast()<<
    " points."<<endl;

  cout <<"--II-- CbmTrdDigitizer: Create " << fDigiMap.size() <<
    " digis."<<endl;

  Int_t iDigi=0; 

  for ( fDigiMapIt=fDigiMap.begin() ; fDigiMapIt != fDigiMap.end(); fDigiMapIt++ ){
    new ((*fDigiCollection)[iDigi]) CbmTrdDigi(fDigiMapIt->second->GetDetId(), fDigiMapIt->second->GetCol(), fDigiMapIt->second->GetRow(), fDigiMapIt->second->GetCharge());
;
   CbmTrdDigiMatch *p = new ((*fDigiMatchCollection)[iDigi]) CbmTrdDigiMatch(); 
   //   cout<<"DetID: "<<fDigiMapIt->second->GetDetId()<<endl;
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

/*
void CbmTrdDigitizer::GetModuleInformation()
{

  // Extract the information about station, layer, module type
  // and cpoy number of the module from the full path to the
  // node.
  // The full path is tokenized at the "/" which diide the different
  // levels of the geometry.
  // Knowing the nameing scheme of the volumes one gets the required
  // information with simple string manipulation.
  // This is probably not the fastes way, but the speed has to be checked.
  // The methode works only for versions of Root > 5.20.0, before the
  // class TStringTocken is not implemented


  TString path = gGeoManager->GetPath();
  cout<<"Path: "<<path<<endl;
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,20,0)
  TStringToken* bla = new TStringToken(path,"/");
#else
  CbmTrdStringToken* bla = new CbmTrdStringToken(path,"/");
#endif

  while (bla->NextToken()) {
    if (bla->Contains("layer")) {
      TString bla3 = (TString) *bla;
      Ssiz_t pos = bla3.Last('_');
      Ssiz_t substringLength=bla3.Length()-pos-1;
      TString bla2 = bla3((bla3.Last('_')+1),substringLength);
      TString bla1 = bla3(3,1);
      fStation=bla1.Atoi();
      fLayer=bla2.Atoi();
    }
    if (bla->Contains("mod")){
      TString bla3 = (TString) *bla;
      Ssiz_t pos = bla3.Last('_');
      Ssiz_t substringLength=bla3.Length()-pos-1;
      TString bla2 = bla3(pos+1,substringLength);
      substringLength=pos-7;
      TString bla1 = bla3(7,substringLength);     
      fModuleType = bla1.Atoi();
      fModuleCopy = bla2.Atoi();
      break;
    } 
  }

}
*/

void CbmTrdDigitizer::GetModuleInformationFromDigiPar(Int_t VolumeID, Double_t *local_point)
{

  fModuleInfo = fDigiPar->GetModule(VolumeID);
  Int_t detID = fModuleInfo->GetDetectorId();

  if (detID != VolumeID ){
    cout<<" -E- This is wrong!!!!!!!!!!!!!!!!!!!!!"<<endl;
  }

  /*
  Int_t* detInfo = fTrdId.GetDetectorInfo(VolumeID); 
  fStation = detInfo[1];
  fLayer = detInfo[2];
  fModuleType =  detInfo[3];
  fModuleCopy = detInfo[4];
  */

  fpadsizex = fModuleInfo->GetPadSizex();
  fpadsizey = fModuleInfo->GetPadSizey();
  fsizex    = fModuleInfo->GetSizex();
  fsizey    = fModuleInfo->GetSizey();

  // Calculate the position in the chamber with the origin of
  // the local coordinate system in the lower left (right???) corner
  // of the chamber. x goes to the left looking in beam direction
  // y goes upward

  if (fModuleInfo->IsRotated()){
    Double_t tempx = local_point[0];
    Double_t tempy = local_point[1];
    local_point[1] = -tempx;
    local_point[0] =  tempy;

    tempx = fsizex;
    tempy = fsizey;
    fsizey = tempx;
    fsizex = tempy;
  }

  fPosX=local_point[0]+fsizex;
  fPosY=local_point[1]+fsizey;
 
  if ( fVerbose > 2 ) {
    cout<<setprecision(5)<<"*** CbmTrdDigitizer::GetModuleInformationFromDigiPar ***"<<endl;
    cout<<setprecision(5)<<"sizex: "<< fsizex <<endl;
    cout<<setprecision(5)<<"sizey: "<< fsizey <<endl;
    cout<<setprecision(5)<<"padsizex: "<< fpadsizex <<endl;
    cout<<setprecision(5)<<"padsizey: "<< fpadsizey <<endl;
  
    cout<<setprecision(5)<<"X: "<< local_point[0] <<endl;
    cout<<setprecision(5)<<"Y: "<< local_point[1] <<endl;
    cout<<setprecision(5)<<"fPosX: "<< fPosX <<endl;
    cout<<setprecision(5)<<"fPosY: "<< fPosY <<endl;
    cout<<"DetID: "<< detID <<endl;
  
    cout<<setprecision(5)<<"CPosX: "<< fModuleInfo->GetX() <<endl;
    cout<<setprecision(5)<<"CPosY: "<< fModuleInfo->GetY() <<endl;
    cout<<setprecision(5)<<"CPosZ: "<< fModuleInfo->GetZ() <<endl;
  }
  
}

// --------------------------------------------------------------------

void CbmTrdDigitizer::AddDigi() {
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
      CbmTrdDigi* digi = new CbmTrdDigi(fModuleID, fCol, fRow, fELoss, fMCindex);
      fDigiMap[b] = digi;
    }

    // Pixel already in map -> Add charge
    else {
        CbmTrdDigi* digi = (CbmTrdDigi*)fDigiMapIt->second;
        if ( ! digi ) Fatal("AddChargeToPixel", "Zero pointer in digi map!");
        digi->AddCharge(fELoss);
        digi->AddMCIndex(fMCindex);
    }

}

// --------------------------------------------------------------------

void CbmTrdDigitizer::CalculatePixel() {

  fCol = 1+(Int_t)(fPosX/fpadsizex);
  fRow = 1+(Int_t)(fPosY/fpadsizey);

  cout<<"#######################################"<<endl;
  cout<<fModuleID<<","<<fCol<<","<<fRow<<endl;
  cout<<fModuleID<<","<<setprecision(5)<<fPosX<<","<<fPosY<<endl;

  if ( fVerbose > 2 ){
    cout<<"*** CbmTrdDigitizer::CalculatePixel ***"<<endl;
    cout<<setprecision(5)<<"fPosX: "<< fPosX <<endl;
    cout<<setprecision(5)<<"fPosY: "<< fPosY <<endl;
    cout<<setprecision(5)<<"fPadX: "<< fpadsizex <<endl;
    cout<<setprecision(5)<<"fPadY: "<< fpadsizey <<endl;
    cout<<"Col: "<< fCol <<endl;
    cout<<"Row: "<< fRow <<endl;
  } 

}

// ---- Register ------------------------------------------------------
void CbmTrdDigitizer::Register(){

  FairRootManager::Instance()->Register("TrdDigi","Trd Digi", fDigiCollection, kTRUE);
  FairRootManager::Instance()->Register("TrdDigiMatch","Trd Digi Match", fDigiMatchCollection, kTRUE);

}
// --------------------------------------------------------------------

ClassImp(CbmTrdDigitizer)
