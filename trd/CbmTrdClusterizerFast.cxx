#include "CbmTrdClusterizerFast.h"

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

#include "TRandom.h"
#include "TMath.h"
#include "TVector3.h"
#include "TF1.h"
#include "TH2F.h"
#include "TH1F.h"
#include "TProfile.h"
#include "TCanvas.h"
#include "TImage.h"
#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TStopwatch.h"

#include "TPRegexp.h"
#if ROOT_VERSION_CODE < ROOT_VERSION(5,20,0)
#include "CbmTrdStringToken.h"
#endif

//#include "sqlite/sqlite3.h" // used for the lookup table option

#include <iostream>
#include <cmath>
using std::cout;
using std::cin;
using std::endl;
using std::pair;
using std::fabs;
// ---- Default constructor -------------------------------------------
CbmTrdClusterizerFast::CbmTrdClusterizerFast()
  : FairTask("TrdCluster"),
   Digicounter(-1),
    fLayerZ(),
    fTime(-1.),
    fModuleType(-1),
    fModuleCopy(-1),
    fModuleID(-1),
    fMCindex(-1),
       fEfficiency(1.),
    fTrdPoints(NULL),
    fDigiCollection(new TClonesArray("CbmTrdDigi")),
    fDigiMatchCollection(NULL),
    fMCStacks(NULL),
    fDigiPar(NULL),
    fModuleInfo(NULL),
    fRadiators(NULL),
    fTrdId(),
    fDigiMap(),
    fDigiMapIt(),
    fModuleClusterMap(),
    fModuleClusterMapIt()
{
}
// --------------------------------------------------------------------

// ---- Constructor ----------------------------------------------------
CbmTrdClusterizerFast::CbmTrdClusterizerFast(const char *name, const char *title,
                 CbmTrdRadiator *radiator)
  :FairTask(name),
  Digicounter(-1),
    fLayerZ(),
    fTime(-1.),
    fModuleType(-1),
    fModuleCopy(-1),
    fModuleID(-1),
    fMCindex(-1),
       fEfficiency(1.),
    fTrdPoints(NULL),
    fDigiCollection(NULL),
    fDigiMatchCollection(NULL),
    fMCStacks(NULL),
    fDigiPar(NULL),
    fModuleInfo(NULL),
    fRadiators(NULL),
    fTrdId(),
    fDigiMap(),
    fDigiMapIt(),
    fModuleClusterMap(),
    fModuleClusterMapIt()
{
}
// --------------------------------------------------------------------

// ---- Destructor ----------------------------------------------------
CbmTrdClusterizerFast::~CbmTrdClusterizerFast()
{
  FairRootManager *ioman =FairRootManager::Instance();
  ioman->Write();
  fDigiCollection->Clear("C");
  delete fDigiCollection;
  fDigiMatchCollection->Clear("C");
  delete fDigiMatchCollection;
  fTrdPoints->Clear("C");
  fTrdPoints->Delete();
  delete fTrdPoints;
  fMCStacks->Clear("C");
  fMCStacks->Delete();
  delete fMCStacks;
  if(fDigiPar)
    delete fDigiPar;
  if(fModuleInfo)
    delete fModuleInfo;
}
// --------------------------------------------------------------------

// ----  Initialisation  ----------------------------------------------
void CbmTrdClusterizerFast::SetParContainers()
{
    cout<<" * CbmTrdClusterizerFast * :: SetParContainers() "<<endl;


    // Get Base Container
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb=ana->GetRuntimeDb();

    fDigiPar = (CbmTrdDigiPar*)
               (rtdb->getContainer("CbmTrdDigiPar"));

}
// --------------------------------------------------------------------

// ---- ReInit  -------------------------------------------------------
InitStatus CbmTrdClusterizerFast::ReInit(){

  cout<<" * CbmTrdClusterizer * :: ReInit() "<<endl;


  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();

  fDigiPar = (CbmTrdDigiPar*)
      (rtdb->getContainer("CbmTrdDigiPar"));
  
  return kSUCCESS;
}
// --------------------------------------------------------------------

// ---- Init ----------------------------------------------------------
InitStatus CbmTrdClusterizerFast::Init()
{

  cout<<" * CbmTrdClusterizerFast * :: Init() "<<endl;

  FairRootManager *ioman = FairRootManager::Instance();
  if ( ! ioman ) Fatal("Init", "No FairRootManager");
    
  fTrdPoints=(TClonesArray *)ioman->GetObject("TrdPoint"); 
  if ( ! fTrdPoints ) {
    cout << "-W CbmTrdClusterFast::Init: No TrdPoints array!" << endl;
    cout << "                            Task will be inactive" << endl;
    return kERROR;
  }

  fMCStacks = (TClonesArray*)ioman->GetObject("MCTrack");
  if ( ! fMCStacks ) {
    cout << "-W CbmTrdClusterFast::Init: No MCTrack array!" << endl;
    cout << "                            Task will be inactive" << endl;
    return kERROR;
  }

  fDigiCollection = new TClonesArray("CbmTrdDigi", 100);
  ioman->Register("TrdDigi","TRD Digis",fDigiCollection,kTRUE);

  fDigiMatchCollection = new TClonesArray("CbmTrdDigiMatch", 100);
  ioman->Register("TrdDigiMatch","TRD Digis",fDigiMatchCollection,kTRUE);

  fRadiators->Init();

  return kSUCCESS;

}
// --------------------------------------------------------------------


// ---- Exec ----------------------------------------------------------
void CbmTrdClusterizerFast::Exec(Option_t * option)
{
  TStopwatch timer;
  timer.Start();
  cout << "================CbmTrdClusterizerFast=====================" << endl;
  Digicounter = 0;
  CbmTrdPoint *pt=NULL;
  MyPoint* point = new MyPoint;
  delete point;


  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();

  printf("\n\n******************** Reading Test  **********************\n");
  printf("   RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  printf("*********************************************************\n\n");

  //fIntegralTest->Draw();
}
   // --------------------------------------------------------------------

void CbmTrdClusterizerFast::GetModuleInformationFromDigiPar(Int_t VolumeID)
{

}
   // --------------------------------------------------------------------
void CbmTrdClusterizerFast::GetModuleInformation(){

}
   // --------------------------------------------------------------------
  void CbmTrdClusterizerFast::AddDigi(const Int_t pointID, Int_t iCol, Int_t iRow, Int_t nCol, Int_t nRow, Double_t iCharge)
  {

  }
   // --------------------------------------------------------------------
  void CbmTrdClusterizerFast::SplitPathSlices()
  {

  }
   // --------------------------------------------------------------------
  void CbmTrdClusterizerFast::WireQuantisation()
  {

  }
   // --------------------------------------------------------------------
  void CbmTrdClusterizerFast::ChargeConservation()
  {

  }
   // --------------------------------------------------------------------
Double_t CbmTrdClusterizerFast::CalcMathieson(Double_t x, Double_t W)
{
  Float_t K3 = 0.525;  //Mathieson parameter for 2nd MuBu prototype -> Parametrisation for chamber parameter
  //Float_t K3 = (-0.7/1.6 * (h/s) + 0.7) + ((exp(-9.74350e+02 * ra/s) * 5.64791e-01 + 3.32737e-01));// aproximation of 'E. Mathieson 'Cathode Charge Distributions in Multiwire Chambers' Nuclear Instruments and Methods in Physics Research A270,1988
  //K3 = 5.25407e-01; // MS 336 fit
  //K3 = 6.14949e-01; // MS 444 fit

  //Float_t K2 = 3.14159265 / 2.* ( 1. - sqrt(K3)/2.);
  //Float_t K1 = (K2 * sqrt(K3)) / (4. * atan(sqrt(K3)));
  //Float_t W = 5;
  //Float_t par = 1;
  Float_t h = 3;
  Double_t SqrtK3 = sqrt(K3);
  /*
    Char_t formula[500];
    sprintf(formula," -1. / (2. * atan(sqrt(%f))) * (atan(sqrt(%f) *tanh(3.14159265 * (-2. + sqrt(%f) ) * (%f + 2.* x * %f) / (8.* %f) )) +  atan(sqrt(%f) *  tanh(3.14159265 * (-2. + sqrt(%f) ) * (%f - 2.* x * %f) / (8.* %f) )) )",K3,K3,K3,W,par,h,K3,K3,W,par,h);
    TF1* mathiesonPRF = new TF1("mathieson",formula, -15, 15);
  */
  Double_t mathiesonPRF = fabs(
			       -1. / (2. * atan(SqrtK3)) * (
							    atan(SqrtK3 * tanh(TMath::Pi() * (-2. + SqrtK3 ) * (W + 2.* x) / (8.* h) )) +  
							    atan(SqrtK3 * tanh(TMath::Pi() * (-2. + SqrtK3 ) * (W - 2.* x) / (8.* h) )) 
							    )
			       );
  /*
    -1. / (2. * atan(sqrt(K3))) * (
    atan(sqrt(K3) * tanh(3.14159265 * (-2. + sqrt(K3) ) * (W + 2.* x) / (8.* h) )) +
    atan(sqrt(K3) * tanh(3.14159265 * (-2. + sqrt(K3) ) * (W - 2.* x) / (8.* h) )) 
    );
  */
  return mathiesonPRF;
}
   // --------------------------------------------------------------------
  void CbmTrdClusterizerFast::TransformLL2C(Double_t* LLCoordinate, Double_t* CCoordinate, Double_t* StrucDim)
  {

  }
    // --------------------------------------------------------------------
  voidCbmTrdClusterizerFast:: TransformC2LL(Double_t* CCoordinate, Double_t* LLCoordinate, Double_t* StrucDim)
  {

  }
    // ---- FinishTask-----------------------------------------------------
void CbmTrdClusterizer::FinishEvent()
{
 
  for (fModuleClusterMapIt = fModuleClusterMap.begin();
       fModuleClusterMapIt != fModuleClusterMap.end(); ++fModuleClusterMapIt) {
    fModuleClusterMapIt->second->SectorSizeX.clear();
    fModuleClusterMapIt->second->SectorSizeX.clear();
    fModuleClusterMapIt->second->SectorSizeY.clear();
    fModuleClusterMapIt->second->PadSizeX.clear();
    fModuleClusterMapIt->second->PadSizeY.clear();
    fModuleClusterMapIt->second->SecCol.clear();
    fModuleClusterMapIt->second->SecRow.clear();
    for (Int_t x = 0; x < PadPlane.size(); x++) {
      for(Int_t y = 0; y < PadPlane[x].size(); y++) {
	delete PadPlane[x][y];
      }
      PadPlane[x].clear();
    }
    PadPlane.clear();
    delete fModuleClusterMapIt->second;
  }
  fModuleClusterMap.clear();
  
  if ( fDigiCollection ) fDigiCollection->Clear();
  if ( fDigiMatchCollection ) fDigiMatchCollection->Clear();
}

    // ---- Register ------------------------------------------------------
void CbmTrdClusterizer::Register()
{
  FairRootManager::Instance()->Register("TrdDigi","Trd Digi", fDigiCollection, kTRUE);
  FairRootManager::Instance()->Register("TrdDigiMatch","Trd Digi Match", fDigiMatchCollection, kTRUE);
}
    // --------------------------------------------------------------------

ClassImp(CbmTrdClusterizerFast)
