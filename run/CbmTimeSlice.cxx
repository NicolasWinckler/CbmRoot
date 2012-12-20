/*
 * CbmTimeSlice.cxx
 *
 *  Created on: 17.07.2012
 *      Author: friese
 */

#include "CbmTimeSlice.h"

#include "CbmDetectorList.h"

#include "FairLogger.h"


// -----   Default constructor   ---------------------------------------------
CbmTimeSlice::CbmTimeSlice() : fStartTime(0.), fDuration(0.) {
  FairLogger::GetLogger()->Warning(MESSAGE_ORIGIN,
      "Do not use the default constructor of CbmTimeSlice!");
}
// ---------------------------------------------------------------------------



// -----   Standard constructor   --------------------------------------------
CbmTimeSlice::CbmTimeSlice(Double_t start, Double_t duration)
: fStartTime(start), fDuration(duration) {
}
// ---------------------------------------------------------------------------



// -----   Destructor   ------------------------------------------------------
CbmTimeSlice::~CbmTimeSlice() {
}
// ---------------------------------------------------------------------------



// -----   Get data object   ------------------------------------------------
CbmDigi* CbmTimeSlice::GetData(DetectorId iDet, Int_t index) {

  CbmDigi* digi = NULL;

  switch (iDet) {

    case kSTS:
      if ( index < fStsData.size() ) digi = &(fStsData[index]);
      break;

    default:
      break;
  }

  return digi;
}
// ---------------------------------------------------------------------------



// -----   Copy data into the time slice   -----------------------------------
void CbmTimeSlice::InsertData(CbmDigi* data) {


  // --- Check whether time of data fits into time slice
  if ( data->GetTime() < fStartTime || data->GetTime() > GetEndTime() ) {
    FairLogger::GetLogger()->Warning(MESSAGE_ORIGIN,
        "Try to insert data at %d into time slice from %d to %d",
        data->GetTime(), fStartTime, GetEndTime());
    return;
  }


  // --- If yes, copy the object into the STL vector
  Int_t iDet = data->GetSystemId();
  switch ( iDet ) {

    case kSTS: {
      CbmStsDigiLight* digi = static_cast<CbmStsDigiLight*>(data);
      fStsData.push_back(*digi);
    }
    break;

    default:
      TString sysName;
      CbmDetectorList::GetSystemName(iDet, sysName);
      FairLogger::GetLogger()->Warning(MESSAGE_ORIGIN,
          "System %s not implemented yet", sysName.Data());
      break;

  }  // detector switch

}
// ---------------------------------------------------------------------------


// ----- Reset time slice   --------------------------------------------------
void CbmTimeSlice::Reset(Double_t start, Double_t duration) {

  fStsData.clear();
  fStartTime = start;
  fDuration = duration;

}
// ---------------------------------------------------------------------------


ClassImp(CbmTimeSlice)
