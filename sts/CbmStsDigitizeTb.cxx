/** @file CbmStsDigitizeTb.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 10 December 2012
 */




// Includes from ROOT
#include "TClonesArray.h"
#include "TObjArray.h"
#include "TMath.h"
#include "TF1.h"
#include "TRandom3.h"

#include "TGeoManager.h"
#include "TGeoNode.h"

// Includes from base
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "CbmMCBuffer.h"
#include "CbmDaqBuffer.h"

// Includes from STS
#include "CbmGeoStsPar.h"
#include "CbmStsDigiLight.h"
#include "CbmStsDigiMatch.h"
#include "CbmStsDigiPar.h"
#include "CbmStsDigiScheme.h"
#include "CbmStsDigitizeTb.h"
#include "CbmStsPoint.h"
#include "CbmStsSensor.h"
#include "CbmStsSector.h"
#include "CbmStsStation.h"

#include <iostream>
#include <iomanip>
#include <map>
#include <vector>
using std::cout;
using std::cerr;
using std::endl;
using std::flush;
using std::pair;
using std::setw;
using std::left;
using std::right;
using std::fixed;
using std::setprecision;
using std::set;
using std::map;
using std::ios_base;
using std::vector;

// -----   Default constructor   ------------------------------------------
CbmStsDigitizeTb::CbmStsDigitizeTb()
  : FairTask("StsDigitize", 1),
  fGeoPar(NULL),
  fDigiPar(NULL),
  fPoints(NULL),
  fDigis(NULL),
  fDigiMatches(NULL),
  fRealistic(kFALSE),
  fDigiScheme(NULL),
  fNDigis(0),
  fNMulti(0),
  fNEvents(0),
  fNPoints(0),
  fNOutside(0),
  fNDigisFront(0),
  fNDigisBack(0),
  fFNoiseWidth(0.1),
  fBNoiseWidth(0.1),
  fStripDeadTime(10),
  fTime(0.),
  fTimer(),
  fFChannelPointsMap(),
  fBChannelPointsMap()
{
  fDigiScheme  = new CbmStsDigiScheme();
  Reset();
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
CbmStsDigitizeTb::~CbmStsDigitizeTb() {
  if ( fGeoPar)    delete fGeoPar;
  if ( fDigiPar)   delete fDigiPar;
  if ( fDigis ) {
    fDigis->Delete();
    delete fDigis;
  }
  if ( fDigiMatches ) {
    fDigiMatches->Delete();
    delete fDigiMatches;
  }
  if ( fDigiScheme ) delete fDigiScheme;
  Reset();
}
// -------------------------------------------------------------------------



// =====   Perform digitisation of one StsPoint   ==========================
void CbmStsDigitizeTb::DigitizePoint(const CbmStsPoint* point,
                                     Int_t& nFront, Int_t& nBack) {

  // Reset counters
  nFront = nBack = 0;


  // Get corresponding sensor
  Double_t xPoint = 0.5 * ( point->GetXOut() + point->GetXIn() );
  Double_t yPoint = 0.5 * ( point->GetYOut() + point->GetYIn() );
  Double_t zPoint = 0.5 * ( point->GetZOut() + point->GetZIn() );
  gGeoManager->FindNode(xPoint, yPoint, zPoint);
  TGeoNode* curNode = gGeoManager->GetCurrentNode();
  CbmStsSensor* sensor = fDigiScheme->GetSensorByName(curNode->GetName());
  fLogger->Debug(MESSAGE_ORIGIN,
               "Point (%d, %d, %d)", xPoint, yPoint, zPoint);
  sensor->Print();


  // Length of trajectory in the sensor
  Double_t deltaX = point->GetXOut() - point->GetXIn();
  Double_t deltaY = point->GetYOut() - point->GetYIn();
  Double_t deltaZ = point->GetZOut() - point->GetZIn();
  Double_t lTraj = TMath::Sqrt( deltaX*deltaX +
                                deltaY*deltaY +
                                deltaZ*deltaZ );
  Int_t nSteps = Int_t( lTraj / fStepSize ) + 1;


  // Total and fractional charge
  Double_t charge = point->GetEnergyLoss() / fPairCreationEnergy;
  Double_t stepCharge = charge / (nSteps + 1);


  // Arrays for the strip charges
  map<Int_t, Double_t> frontSignals;
  map<Int_t, Double_t> backSignals;


  // Produce charge on the sensor strips
  // The trajectory is decomposed into steps of size fStepSize. The total
  // energy loss is equally divided into the steps and projected to the
  // readout strips.
  Double_t xPair = point->GetXIn();
  Double_t yPair = point->GetYIn();
  Double_t zPair = point->GetZIn();
  for (Int_t iStep = 0; iStep <=nSteps; iStep++) {
    Int_t iFChan = sensor->GetFrontChannel(xPair, yPair, zPair);
    frontSignals[iFChan] += stepCharge;
    Int_t iBChan = sensor->GetBackChannel(xPair, yPair, zPair);
    backSignals[iBChan] += stepCharge;
    xPair += deltaX / Double_t(nSteps);
    yPair += deltaY / Double_t(nSteps);
    zPair += deltaZ / Double_t(nSteps);
  }

  // Digitise the charge on the front side strips
  map<Int_t, Double_t>::iterator it;
  for (it = frontSignals.begin(); it != frontSignals.end(); it++) {
    if ( (*it).second < fThreshold ) continue;
    Int_t iAdc = -1;
    if ( (*it).second >= fQMax ) iAdc = fNAdcChannels - 1;
    else iAdc = Int_t( (*it).second / fQMax ) * fNAdcChannels;
    CbmStsDigiLight* digi =
        new CbmStsDigiLight(sensor->GetStationNr(),  // station number
                            sensor->GetSectorNr(),   // sector number
                            0,                       // front side
                            (*it).first,             // channel number
                            iAdc,                    // ADC channel
                            0);                      // time
    CbmDaqBuffer::Instance()->InsertData(digi);
    nFront++;
  }


  // Digitise the charge on the back side strips
  for (it = backSignals.begin(); it != backSignals.end(); it++) {
    if ( (*it).second < fThreshold ) continue;
    Int_t iAdc = -1;
    if ( (*it).second >= fQMax ) iAdc = fNAdcChannels - 1;
    else iAdc = Int_t( (*it).second / fQMax ) * fNAdcChannels;
    CbmStsDigiLight* digi =
        new CbmStsDigiLight(sensor->GetStationNr(),  // station number
                            sensor->GetSectorNr(),   // sector number
                            1,                       // back side
                            (*it).first,             // channel number
                            iAdc,                    // ADC channel
                            0);                      // time
    CbmDaqBuffer::Instance()->InsertData(digi);
    nBack++;
  }


}
// ==========================================================================






// =====  Intialisation (private)   =========================================
InitStatus CbmStsDigitizeTb::Init() {

  // Check for presence of MCBuffer and DaqBuffer
  if ( ! ( CbmMCBuffer::Instance() && CbmDaqBuffer::Instance() ) ) {
    fLogger->Fatal(MESSAGE_ORIGIN, "No MCBuffer or DaqBuffer present!");
    return kFATAL;
  }

  // Pair creation energy in GeV
  fPairCreationEnergy = 3.68e-9;

  // Number of ADC channels
  fNAdcChannels = 1 << ( fNAdcBits + 1 );

  // Build digitisation scheme
  if ( ! fDigiScheme->Init(fGeoPar, fDigiPar) ) {
    fLogger->Error(MESSAGE_ORIGIN,
                   "Error in building STS digitisation scheme");
    return kFATAL;
  }

  if      (fVerbose == 1 || fVerbose == 2) fDigiScheme->Print(kFALSE);
  else if (fVerbose >  2) fDigiScheme->Print(kTRUE);
  cout << "-I- " << fName << "::Init: "
       << "STS digitisation scheme succesfully initialised" << endl;
  cout << "    Stations: " << fDigiScheme->GetNStations()
       << ", Sectors: " << fDigiScheme->GetNSectors() << ", Channels: "
       << fDigiScheme->GetNChannels() << endl;


  return kSUCCESS;
}
// ==========================================================================




// =====   Task execution   =================================================
void CbmStsDigitizeTb::Exec(Option_t* opt) {

  // Counters
  Int_t nFront = 0;
  Int_t nBack  = 0;
  fTimer.Start();

  // Loop over StsPoints from MCBuffer
  const CbmStsPoint* point =
      dynamic_cast<const CbmStsPoint*>(CbmMCBuffer::Instance()->GetNextPoint(kSTS));
  while ( point ) {


    DigitizePoint(point, nFront, nBack);
    fLogger->Info(MESSAGE_ORIGIN,
        "Digis created: %i front, %i back", nFront, nBack);

    // Next StsPoint
    point =  dynamic_cast<const CbmStsPoint*>(CbmMCBuffer::Instance()->GetNextPoint(kSTS));

  }


  fTimer.Stop();
  cout << "+ " << flush;
  cout << setw(15) << left << fName << ": " << setprecision(4) << setw(8)
       << fixed << right << fTimer.RealTime() 
       << " s"  << endl;
  
  fNEvents     += 1.;
  fTime        += fTimer.RealTime();

}
// ==========================================================================





// -----   Private method SetParContainers   -------------------------------
void CbmStsDigitizeTb::SetParContainers() {

  // Get run and runtime database
  FairRunAna* run = FairRunAna::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");

  // Get STS geometry parameter container
  fGeoPar = (CbmGeoStsPar*) db->getContainer("CbmGeoStsPar");

  // Get STS digitisation parameter container
  fDigiPar = (CbmStsDigiPar*) db->getContainer("CbmStsDigiPar");
 
}
// -------------------------------------------------------------------------






// -----   Private method ReInit   -----------------------------------------
InitStatus CbmStsDigitizeTb::ReInit() {

  // Clear digitisation scheme
  fDigiScheme->Clear();

  // Build new digitisation scheme
  if ( fDigiScheme->Init(fGeoPar, fDigiPar) ) {
    return kSUCCESS;
  }

  return kERROR;

}
// -------------------------------------------------------------------------



// -----   Private method Reset   ------------------------------------------
void CbmStsDigitizeTb::Reset() {
  //  fNPoints = fNOutside = fNDigisFront = fNDigisBack = fTime = 0.;
  fNDigis = fNMulti = 0;
  fFChannelPointsMap.clear();
  fBChannelPointsMap.clear();
  //  if ( fDigis ) fDigis->Clear();
  //  if ( fDigiMatches ) fDigiMatches->Clear();
  if ( fDigis ) fDigis->Delete();
  if ( fDigiMatches ) fDigiMatches->Delete();
}
// -------------------------------------------------------------------------


// -----   Virtual method Finish   -----------------------------------------
void CbmStsDigitizeTb::Finish() {
  cout << endl;
  cout << "============================================================"
       << endl;
  cout << "===== " << fName << ": Run summary " << endl;
  cout << "===== " << endl;
  cout << "===== Events processed          : " << setw(8) << fNEvents << endl;
  cout.setf(ios_base::fixed, ios_base::floatfield);
  cout << "===== Real time per event       : " 
       << setw(8) << setprecision(4) 
       << fTime / fNEvents << " s" << endl;
  cout << "===== StsPoints per event       : " 
       << setw(8) << setprecision(2) 
       << fNPoints / fNEvents << endl;
  cout << "===== Outside hits per event    : " 
       << setw(8) << setprecision(2) 
       << fNOutside / fNEvents << " = " 
       << setw(6) << setprecision(2) 
       << fNOutside / fNPoints * 100. << " %" << endl;
  cout << "===== Front digis per point     : " 
       << setw(8) << setprecision(2) 
       << fNDigisFront / (fNPoints-fNOutside) << endl;
  cout << "===== Back digis per point      : " 
       << setw(8) << setprecision(2) 
       << fNDigisBack / (fNPoints-fNOutside) << endl;
  cout << "============================================================"
       << endl;
	
}					       
// -------------------------------------------------------------------------



ClassImp(CbmStsDigitizeTb)

