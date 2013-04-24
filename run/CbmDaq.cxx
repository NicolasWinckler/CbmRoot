/** @file CbmDaq.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 20 July 2012
 **/


#include <iomanip>

#include "CbmMCBuffer.h"
#include "CbmDaq.h"
#include "CbmDaqBuffer.h"
#include "CbmTimeSlice.h"



// =====   Constructor   =====================================================
CbmDaq::CbmDaq(Double_t timeSliceSize) : FairTask("Daq"),
                   fCurrentStartTime (0.),
                   fDuration (timeSliceSize),
                   fStoreEmptySlices(kTRUE),
                   fTimeSlice(NULL),
                   fBuffer(NULL) {
}
// ===========================================================================



// =====   Destructor   ======================================================
CbmDaq::~CbmDaq() {
}
// ===========================================================================



// =====   Close the current time slice and fill it to the tree   ============
void CbmDaq::CloseTimeSlice() {

  // --- Time slice status
  fTimeSlice->Print();
  LOG(INFO) << fName << ": Closing current time slice" << FairLogger::endl;

  // --- Fill current time slice into tree
  FairRootManager::Instance()->Fill();

  // --- Reset time slice with new time interval
  fCurrentStartTime += fDuration;
  fTimeSlice->Reset(fCurrentStartTime, fDuration);

}
// ===========================================================================



// =====   Task execution   ==================================================
void CbmDaq::Exec(Option_t* opt) {

  // --- Debug info
  LOG(DEBUG) << fName << ": Current time slice [" << fixed
             << setprecision(3) << fTimeSlice->GetStartTime() << ", "
             << fTimeSlice->GetEndTime() << "] ns" << FairLogger::endl;

  while ( kTRUE ) {        /* Loop until time slice cannot be closed */

    FillTimeSlice();


    // --> Exit if current time slice cannot yet be closed
    if ( CbmMCBuffer::Instance()->GetTime() <
        fTimeSlice->GetEndTime() + 2. * fDuration ) break;

    CloseTimeSlice();

  }     /* Loop over time slices */

  // --- Print time slice and buffer status
  fTimeSlice->Print();
  fBuffer->PrintStatus();

}
// ===========================================================================



// =====   Fill current time slice with data from buffers   ==================
void CbmDaq::FillTimeSlice() {

  // --- Loop over all detector systems
  for (Int_t iDet = kREF; iDet < kNOFDETS; iDet++) {

    // --- Loop over digis from DaqBuffer and fill them into current time slice
    CbmDigi* digi = fBuffer->GetNextData(iDet, fTimeSlice->GetEndTime());
    while ( digi ) {
      LOG(DEBUG2) << fName << ": Inserting digi with detector ID "
                  << digi->GetAddress() << " at time " << digi->GetTime()
                  << " into timeslice [" << fixed << setprecision(3)
                  << fTimeSlice->GetStartTime() << ", "
                  << fTimeSlice->GetEndTime() << ") ns"
                  << FairLogger::endl;
      fTimeSlice->InsertData(digi);
      delete digi;
      digi = fBuffer->GetNextData(iDet, fTimeSlice->GetEndTime());
    }  /* Valid digi from buffer */

  }  /* Detector loop */

}
// ===========================================================================



// =====   End-of-run action   ===============================================
void CbmDaq::Finish() {

  LOG(INFO) << FairLogger::endl;
  LOG(INFO) << fName << ": End of run" << FairLogger::endl;

  while (kTRUE) {  // time slice loop until buffer is emptied

    FillTimeSlice();
    CloseTimeSlice();
    if ( ! fBuffer->GetSize() ) break;
    // --- Print time slice and buffer status
    fTimeSlice->Print();
    fBuffer->PrintStatus();

  }

  LOG(INFO) << fName << ": DAQ buffer empty. Run finished.";

}
// ===========================================================================



// =====   Task initialisation   =============================================
InitStatus CbmDaq::Init() {

  // Set initial start time
  fCurrentStartTime = 0.;

  // Set time slice duration
  fDuration = 1000;

  // Register output array TimeSlice
  fTimeSlice = new CbmTimeSlice(fCurrentStartTime, fDuration);
  FairRootManager::Instance()->Register("TimeSlice.",
      "DAQ", fTimeSlice, kTRUE);

  // Get MCBuffer
  fBuffer = CbmDaqBuffer::Instance();

  return kSUCCESS;
}
// ===========================================================================


ClassImp(CbmDaq)

