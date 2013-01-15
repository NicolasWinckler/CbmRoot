/** @file CbmMCTimeSim.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 8 February 2012
 **/


#include "TClonesArray.h"
#include "TRandom3.h"

#include "CbmMCBuffer.h"
#include "CbmMCEventHeader.h"
#include "CbmMCTimeSim.h"


using namespace std;






// -----   Default constructor   ---------------------------------------------
CbmMCTimeSim::CbmMCTimeSim() { 
  fLogger->Fatal(MESSAGE_ORIGIN, 
		 "Default constructor should not be used; specify interaction rate!");
}
// ---------------------------------------------------------------------------



// -----   Standard constructor   --------------------------------------------
CbmMCTimeSim::CbmMCTimeSim(Double_t rate, Int_t profile, const char* name) 
  : FairTask(name), 
    fEventRate(rate),
    fBeamProfile(profile),
    fMaxBufferSize(1000.),
    fEventId(-1),
    fEventTime(0.),
    fEvent(NULL)
{
  for (Int_t iDet = 0; iDet < kNOFDETS; iDet++) fPointArrays.push_back(NULL);
}
// ---------------------------------------------------------------------------



// -----   Destructor   ------------------------------------------------------
CbmMCTimeSim::~CbmMCTimeSim() { }
// ---------------------------------------------------------------------------



// -----   CreateEventTime   -------------------------------------------------
Double_t CbmMCTimeSim::CreateEventTime() {
  
  Double_t deltaT = 0.;
  Double_t tau = 1.e9 / fEventRate;  // average time between events in [ns]
  switch ( fBeamProfile ) {
  case 1 :   // Constant time interval 
    deltaT = tau; 
    break;
  case 2 :   // Poisson profile: exponential distribution of time interval
    deltaT = gRandom->Exp(tau);
    break;
  default :
    fLogger->Error(MESSAGE_ORIGIN, 
		   "Beam profile type %d not defined!", fBeamProfile);
  }

  return ( fEventTime + deltaT );
}
// ---------------------------------------------------------------------------



// -----   Exec   ------------------------------------------------------------
void CbmMCTimeSim::Exec(Option_t* opt) {

  // CbmMCBuffer instance
  CbmMCBuffer* buffer = CbmMCBuffer::Instance();

  // Check for overflow
  if ( buffer->GetSize() > fMaxBufferSize ) 
    fLogger->Fatal(MESSAGE_ORIGIN, 
		   "Buffer size %.2f MB exceeds maximum (%.2f)",
		   buffer->GetSize(), fMaxBufferSize);

  // Get event ID
  fEventId = fEvent->GetEventID();

  // Create and set the new event time
  fEventTime = CreateEventTime();

  // Read MCPoints from event into buffers
  for (Int_t iDet = 0; iDet < kNOFDETS; iDet++)
    if ( fPointArrays[iDet] ) buffer->Fill(fPointArrays[iDet], 
					   iDet,
					   fEventId, 
					   fEventTime);

  Print();

}
// ---------------------------------------------------------------------------




// -----   Finish   ----------------------------------------------------------
void CbmMCTimeSim::Finish() {

  CbmMCBuffer::Instance()->Clear();

}
// ---------------------------------------------------------------------------




// -----   Init   ------------------------------------------------------------
InitStatus CbmMCTimeSim::Init() {

  // Get FairRootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) 
    fLogger->Fatal(MESSAGE_ORIGIN, "No FairRootManager");

  // Get event header
  fEvent = (CbmMCEventHeader*) ioman->GetObject("MCEventHeader.");

  //CbmMCEventHeader* event = (CbmMCEventHeader*) ioman->GetObject("MCEventHeader.");
  //if ( ! event ) fLogger->Fatal(MESSAGE_ORIGIN, "No MC event header!");

  // Get MCPoint arrays
  fPointArrays[kMVD]  = (TClonesArray*) ioman->GetObject("MvdPoint");
  fPointArrays[kSTS]  = (TClonesArray*) ioman->GetObject("StsPoint");
  fPointArrays[kRICH] = (TClonesArray*) ioman->GetObject("RichPoint");
  fPointArrays[kMUCH] = (TClonesArray*) ioman->GetObject("MuchPoint");
  fPointArrays[kTRD]  = (TClonesArray*) ioman->GetObject("TrdPoint");
  fPointArrays[kTOF]  = (TClonesArray*) ioman->GetObject("TofPoint");
  fPointArrays[kECAL] = (TClonesArray*) ioman->GetObject("EcalPoint");
  fPointArrays[kPSD]  = (TClonesArray*) ioman->GetObject("PsdPoint");

  // Screen output
  TString config = "";
  for (Int_t iDet = 1; iDet < kNOFDETS; iDet++) {
    if ( fPointArrays[iDet] ) {
      TString sysName;
      CbmDetectorList::GetSystemNameCaps(iDet, sysName);
      config += " " + sysName;
    }
  }
  fLogger->Info(MESSAGE_ORIGIN, "Configuration %s", config.Data()); 

  // Get CbmMCBuffer instance
  CbmMCBuffer::Instance()->Clear();

  return kSUCCESS;
}
// ---------------------------------------------------------------------------



// -----   Print   -----------------------------------------------------------
void CbmMCTimeSim::Print(Option_t* opt) {

  fLogger->Info(MESSAGE_ORIGIN, "");
  fLogger->Info(MESSAGE_ORIGIN, "Current event ID: %d", fEventId);
  fLogger->Info(MESSAGE_ORIGIN, "Current event time: %f ns", fEventTime);
  CbmMCBuffer::Instance()->Print();

}
// ---------------------------------------------------------------------------



   


  

ClassImp(CbmMCTimeSim)