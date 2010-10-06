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
  
  fStsPoints = new TClonesArray("CbmStsPoint", 1000);

}
// ---------------------------------------------------------------------------



// -----    Standard  constructor   ------------------------------------------
CbmMCEpoch::CbmMCEpoch(Double_t startTime,
		       Double_t epochLength) : TNamed("MCEpoch", "MCEpoch"),
					       fStartTime(startTime),
					       fEpochLength(epochLength) {
  
  fStsPoints = new TClonesArray("CbmStsPoint", 1000);

}
// ---------------------------------------------------------------------------



// -----   Destructor   ------------------------------------------------------
CbmMCEpoch::~CbmMCEpoch() {
}
// ---------------------------------------------------------------------------



// -----   Add a MCPoint to the epoch   --------------------------------------
void CbmMCEpoch::AddPoint(DetectorId det, CbmStsPoint& stsPoint, 
			  Int_t eventId, Double_t eventTime) {

  new ((*fStsPoints)[GetNofPoints(kSTS)]) CbmStsPoint(stsPoint,
						      eventId,
						      eventTime,
						      fStartTime);

}
// ---------------------------------------------------------------------------



// -----   Clear epoch   -----------------------------------------------------
void CbmMCEpoch::Clear() {
  fStartTime = 0.;
  fStsPoints->Clear("C");
}
// ---------------------------------------------------------------------------



// -----   Get number of points in epoch   -----------------------------------
Int_t CbmMCEpoch::GetNofPoints(DetectorId det) {

  Int_t nPoints = 0;

  switch (det) {

    case kSTS: nPoints = fStsPoints->GetEntriesFast(); break;
      
      cout << "-W- CbmMCEpoch:: Detector " << det << " not available"
	   << endl;
    
  }

  return nPoints;

}
// ---------------------------------------------------------------------------



// -----   Get a MCPoint from the array   ------------------------------------
FairMCPoint* CbmMCEpoch::GetPoint(DetectorId det, Int_t index) {

  if ( index >= 0  &&  index < GetNofPoints(det) ) {
    switch (det) {
    case kSTS: return ( (FairMCPoint*) fStsPoints->At(index) ); break;
      default:   return NULL;
    } 
  }
  cout << "-W- " << GetName() << "::GetPoint: Index " << index 
       << "out of range for system " << det << endl;
  return NULL;

}
// ---------------------------------------------------------------------------
      



// -----   Check for empty epoch   -------------------------------------------
Bool_t CbmMCEpoch::IsEmpty() {

  if (fStsPoints->GetEntriesFast()) return kFALSE;
  return kTRUE;

}
// ---------------------------------------------------------------------------



// -----   Print epoch info   ------------------------------------------------
void CbmMCEpoch::Print(Option_t* opt) const {
  cout << "-I- Epoch: Start time " << fStartTime << ", Points: ";
  cout << "STS " << fStsPoints->GetEntriesFast() << endl;
}
// ---------------------------------------------------------------------------



ClassImp(CbmMCEpoch)

