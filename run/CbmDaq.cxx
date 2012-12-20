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

CbmDaq::CbmDaq() :
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

  // Register output array TimeSlice
  fTimeSlice = new CbmTimeSlice(fCurrentStartTime, fDuration);
  FairRootManager::Instance()->Register("TimeSlice",
      "TimeSlice", fTimeSlice, kTRUE);

}

void CbmDaq::Exec(Option_t* opt) {


  while ( kTRUE ) {        /* Loop until time slice cannot be closed */

    // ---> Get digis from buffer and fill into current time slice
    for (Int_t iDet = kREF; iDet < kNOFDETS; iDet++) {

      CbmDigi* digi = fBuffer->GetNextData(iDet, fTimeSlice->GetEndTime());
      while ( digi ) {
        fTimeSlice->InsertData(digi);
        delete digi;
        digi = fBuffer->GetNextData(iDet, fTimeSlice->GetEndTime());
      }  /* Valid digi from buffer */

    }  /* Detector loop */


    // --> Exit if current time slice cannot yet be closed
    if ( CbmMCBuffer::Instance()->GetTime() <
        fTimeSlice->GetEndTime() + 2. * fDuration ) return;


    // ---> Close current time slice, fill the tree and reset time slice
    FairRootManager::Instance()->Fill();
    fCurrentStartTime += fDuration;
    fTimeSlice->Reset(fCurrentStartTime, fDuration);

  }     /* Loop over time slices */


}

ClassImp(CbmDaq)

