/*
 * CbmDaq.cxx
 *
 *  Created on: 20.07.2012
 *      Author: friese
 */

#include "CbmDaq.h"

#include "CbmDaqBuffer.h"
#include "CbmDigi.h"
#include "CbmMCBuffer.h"
#include "FairRootManager.h"
#include "CbmTimeSlice.h"

#include <iostream>
#include <iomanip>

CbmDaq::CbmDaq() :
  FairTask("Daq"),
  fCurrentStartTime (0.),
  fDuration (0.){
  // TODO Auto-generated constructor stub

}

CbmDaq::~CbmDaq() {
  // TODO Auto-generated destructor stub
}


InitStatus CbmDaq::Init() {

  // Set initial start time
  fCurrentStartTime = 0.;

  // Set time slice duration
  fDuration = 1000;

  // Register output array TimeSlice
  fTimeSlice = new CbmTimeSlice(fCurrentStartTime, fDuration);
  FairRootManager::Instance()->Register("TimeSlice",
      "DAQ", fTimeSlice, kTRUE);

  // Get MCBuffer
  fBuffer = CbmDaqBuffer::Instance();

}

void CbmDaq::Exec(Option_t* opt) {

  // --- Log info
  LOG(DEBUG) << fName << ": Current time slice [" << fixed
             << setprecision(3) << fTimeSlice->GetStartTime() << ", "
             << fTimeSlice->GetEndTime() << "] ns" << FairLogger::endl;

  while ( kTRUE ) {        /* Loop until time slice cannot be closed */

    // --- Loop over all detector systems
    for (Int_t iDet = kREF; iDet < kNOFDETS; iDet++) {

      // --- Loop over digis from DaqBuffer and fill them into current time slice
      CbmDigi* digi = fBuffer->GetNextData(iDet, fTimeSlice->GetEndTime());
      while ( digi ) {
        LOG(DEBUG2) << fName << ": Inserting digi with detector ID "
                    << digi->GetDetectorId() << " at time " << digi->GetTime()
                    << " into timslice [" << fixed << setprecision(3)
                    << fTimeSlice->GetStartTime() << ", "
                    << fTimeSlice->GetEndTime() << ") ns"
                    << FairLogger::endl;
        fTimeSlice->InsertData(digi);
        delete digi;
        digi = fBuffer->GetNextData(iDet, fTimeSlice->GetEndTime());
      }  /* Valid digi from buffer */

    }  /* Detector loop */


    // --> Exit if current time slice cannot yet be closed
    if ( CbmMCBuffer::Instance()->GetTime() <
        fTimeSlice->GetEndTime() + 2. * fDuration ) return;


    // ---> Close current time slice, fill the tree and reset time slice
    LOG(INFO) << fName << ": Closing current time slice" << FairLogger::endl;
    fTimeSlice->Print();
    FairRootManager::Instance()->Fill();
    fCurrentStartTime += fDuration;
    fTimeSlice->Reset(fCurrentStartTime, fDuration);

  }     /* Loop over time slices */


}

ClassImp(CbmDaq)

