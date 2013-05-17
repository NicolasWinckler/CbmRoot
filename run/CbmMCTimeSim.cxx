/** @file CbmMCTimeSim.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 8 February 2012
 **/


#include <iomanip>

#include "TClonesArray.h"
#include "TRandom3.h"

#include "CbmDetectorList.h"
#include "CbmMCBuffer.h"
#include "CbmMCEventHeader.h"
#include "CbmMCTimeSim.h"


using namespace std;


// -----   Default constructor   ---------------------------------------------
CbmMCTimeSim::CbmMCTimeSim() 
 : FairTask(),
   fEventRate(0.),
   fBeamProfile(0),
   fMaxBufferSize(0.),
   fEventId(0),
   fEventTime(0.),
   fNofEvents(0),
   fEvent(NULL),
   fPointArrays()
{ 
  gLogger->Fatal(MESSAGE_ORIGIN, 
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
    fNofEvents(0),
    fEvent(NULL),
    fPointArrays()
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
    gLogger->Error(MESSAGE_ORIGIN, 
		   "Beam profile type %d not defined!", fBeamProfile);
  }

  return ( fEventTime + deltaT );
}
// ---------------------------------------------------------------------------



// -----   Exec   ------------------------------------------------------------
void CbmMCTimeSim::Exec(Option_t* opt) {

  // Get CbmMCBuffer instance and remove already used points
  CbmMCBuffer* buffer = CbmMCBuffer::Instance();
  buffer->Clear();

  // Check for overflow
  if ( buffer->GetSize() > fMaxBufferSize ) 
    gLogger->Fatal(MESSAGE_ORIGIN, 
		   "Buffer size %.2f MB exceeds maximum (%.2f)",
		   buffer->GetSize(), fMaxBufferSize);

  // Get event ID
  fEventId = fEvent->GetEventID();

  // Create and set the new event time
  fEventTime = CreateEventTime();

  // Log output
  cout << endl;
  LOG(INFO) << fName << ": Event ID " << fEventId << ", event time "
            << fixed << setprecision(3) << fEventTime << " ns"
            << FairLogger::endl;

  // Read MCPoints from event into buffers
  LOG(INFO) << fName << ": Points read from input: ";
  for (Int_t iDet = 0; iDet < kNOFDETS; iDet++) {

    Int_t nPoints = 0;
    if ( fPointArrays[iDet] ) {
      nPoints = buffer->Fill(fPointArrays[iDet], iDet,
                             fEventId, fEventTime);
      TString sysName;
      CbmDetectorList::GetSystemNameCaps(iDet, sysName);
      LOG(INFO) << sysName << " " << nPoints << "  ";
    }

  }
  LOG(INFO) << FairLogger::endl;

  // Log buffer status
  CbmMCBuffer::Instance()->Print();

  // Increment event counter
  fNofEvents++;

}
// ---------------------------------------------------------------------------




// -----   Finish   ----------------------------------------------------------
void CbmMCTimeSim::Finish() {

  cout << endl;
  LOG(INFO) << fName << ": End of run" << FairLogger::endl;
  LOG(INFO) << fName << ": Events processed: " << fNofEvents
            << FairLogger::endl << FairLogger::endl;

  CbmMCBuffer::Instance()->Clear();
  CbmMCBuffer::Instance()->SetEndOfRun();

}
// ---------------------------------------------------------------------------




// -----   Init   ------------------------------------------------------------
InitStatus CbmMCTimeSim::Init() {

  // Get FairRootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) 
    gLogger->Fatal(MESSAGE_ORIGIN, "No FairRootManager");

  // Get event header
  fEvent = (CbmMCEventHeader*) ioman->GetObject("MCEventHeader.");

  //CbmMCEventHeader* event = (CbmMCEventHeader*) ioman->GetObject("MCEventHeader.");
  //if ( ! event ) gLogger->Fatal(MESSAGE_ORIGIN, "No MC event header!");

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
  gLogger->Info(MESSAGE_ORIGIN, "Configuration %s", config.Data()); 

  // Clear CbmMCBuffer
  CbmMCBuffer::Instance()->Clear();

  return kSUCCESS;
}
// ---------------------------------------------------------------------------

   


  

ClassImp(CbmMCTimeSim)
