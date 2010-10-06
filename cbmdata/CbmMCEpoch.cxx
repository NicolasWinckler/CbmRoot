/** CbmMCEpoch.cxx
 *@author Volker Friese v.friese@gsi.de
 *@since 13.11.2009
 *@version 1.0
 **/



#include "CbmMCEpoch.h"

#include <iostream>

#include "TClonesArray.h"

using namespace std;


// -----    Default  constructor   -------------------------------------------
CbmMCEpoch::CbmMCEpoch() : TNamed("MCEpoch", "MCEpoch") {

  CreateArrays();

}
// ---------------------------------------------------------------------------



// -----    Standard  constructor   ------------------------------------------
CbmMCEpoch::CbmMCEpoch(Double_t startTime,
		       Double_t epochLength) : TNamed("MCEpoch", "MCEpoch"),
					       fStartTime(startTime),
					       fEpochLength(epochLength) {

  CreateArrays();

}
// ---------------------------------------------------------------------------



// -----   Destructor   ------------------------------------------------------
CbmMCEpoch::~CbmMCEpoch() {
}
// ---------------------------------------------------------------------------



// -----   Add a MCPoint to the epoch   --------------------------------------
void CbmMCEpoch::AddPoint(DetectorId det, CbmStsPoint& stsPoint, 
			  Int_t eventId, Double_t eventTime) {

  switch (det) {
  case kSTS: new ((*(fPoints[det]))[GetNofPoints(det)]) CbmStsPoint(stsPoint,
								    eventId,
								    eventTime,
								    fStartTime);
    break;
  }

}
// ---------------------------------------------------------------------------



// -----   Clear epoch   -----------------------------------------------------
void CbmMCEpoch::Clear() {

  /* Note: The loop over the detetcor id works only if the corresponding
   * enum is continuous. Did not find a better solution yet. V.F.  */

  fStartTime = 0.;
  for (Int_t iDet=kREF; iDet<kTutDet; iDet++) {
    DetectorId det = DetectorId(iDet);
    if ( fPoints[det] ) fPoints[det]->Clear("C");
  }

}
// ---------------------------------------------------------------------------



// -----   Get number of points in epoch   -----------------------------------
Int_t CbmMCEpoch::GetNofPoints(DetectorId det) const {

  if ( ! fPoints[det] ) {
    cout << "-W- " << GetName() << "::GetNofPoints: "
	 << "No array for detector system " << det << endl;
    return 0;
  }

  return fPoints[det]->GetEntriesFast();

}
// ---------------------------------------------------------------------------



// -----   Get a MCPoint from the array   ------------------------------------
FairMCPoint* CbmMCEpoch::GetPoint(DetectorId det, Int_t index) {

  if ( ! fPoints[det] ) {
    cout << "-W- " << GetName() << "::GetPoint: "
	 << "No array for detector system " << det << endl;
    return NULL;
  }

  if ( index < 0  ||  index >= GetNofPoints(det) ) {
    cout << "-W- " << GetName() << "::GetPoint: Index " << index 
	 << "out of range for system " << det << endl;
    return NULL;
  }

  return ( (FairMCPoint*) fPoints[det]->At(index) );

}
// ---------------------------------------------------------------------------
      



// -----   Check for empty epoch   -------------------------------------------
Bool_t CbmMCEpoch::IsEmpty() {

  Int_t nTotal = 0;

  for (Int_t iDet=kREF; iDet<kTutDet; iDet++) {
    DetectorId det = DetectorId(iDet);
    if ( fPoints[iDet] ) nTotal += GetNofPoints(det);
  }

  if (nTotal > 0) return kTRUE;
  return kFALSE;

}
// ---------------------------------------------------------------------------



// -----   Print epoch info   ------------------------------------------------
void CbmMCEpoch::Print(Option_t* opt) const { 

  cout << "-I- Epoch: Start time " << fStartTime << ", Points: ";
  TString sysName;
  for (Int_t iDet = kREF; iDet<kTutDet; iDet++) {
    DetectorId det = DetectorId(iDet);
    if ( fPoints[iDet] ) {
      CbmDetectorList::GetSystemName(DetectorId(iDet), sysName);
      cout << sysName << " " << GetNofPoints(det) << " ";
    }
  }
  cout << endl;

}
// ---------------------------------------------------------------------------



// -----   Create MCPoint arrays   -------------------------------------------
void CbmMCEpoch::CreateArrays() {

  for (Int_t iDet=kREF; iDet<kTutDet; iDet++) {
    DetectorId det = DetectorId(iDet);
    switch(det) {
    case kSTS:  fPoints[det] = new TClonesArray("CbmStsPoint", 1000); break;
    case kMUCH: fPoints[det] = new TClonesArray("CbmMuchPoint", 1000); break;
    default:    fPoints[det] = NULL;
    }
  }

}
// ---------------------------------------------------------------------------


ClassImp(CbmMCEpoch)

