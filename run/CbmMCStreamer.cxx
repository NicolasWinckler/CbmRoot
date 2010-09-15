/** CbmMCStreamer.cxx
 **@author Volker Friese <v.friese@gsi.de>
 **@since 13.11.2009
 **version 1.0
 **/


#include "CbmMCStreamer.h"

#include "CbmMCEpoch.h"

#include "FairRootManager.h"

#include "TClonesArray.h"
#include "TMath.h"
#include "TRandom3.h"

#include <iostream>


using namespace std;



// -----   Default constructor   ---------------------------------------------
CbmMCStreamer::CbmMCStreamer() { }
// ---------------------------------------------------------------------------



// -----   Standard constructor   --------------------------------------------
CbmMCStreamer::CbmMCStreamer(Double_t interactionRate, 
			       Int_t beamProfile,
			       Double_t epochLength,
			       Bool_t persistence, 
			       Int_t verbosity, 
			       const char* name) 
  : FairTask(name, verbosity), 
    fBeamRate(interactionRate), 
    fBeamProfile(beamProfile),
    fEpochLength(epochLength),
    fPersistence(persistence) {

  fBeamTau = 1.e9 / fBeamRate;
}
// ---------------------------------------------------------------------------



// -----   Destructor   ------------------------------------------------------
CbmMCStreamer::~CbmMCStreamer() { }
// ---------------------------------------------------------------------------




// -----   Initialisation   --------------------------------------------------
InitStatus CbmMCStreamer::Init() {

  cout << endl;
  cout << "---------------------------------------------" << endl;
  cout << "-I- Initialising " << GetName() << " ...." << endl; 
  cout << "-I-" << endl;
  cout << "-I- Configuration: ";

  // Get input MCPoint arrays
  FairRootManager* ioman = FairRootManager::Instance();
  fInMvd  = (TClonesArray*) ioman->GetObject("MVDPoint");
  fStsPoints  = (TClonesArray*) ioman->GetObject("StsPoint");
  fInRich = (TClonesArray*) ioman->GetObject("RICHPoint");
  fInTrd  = (TClonesArray*) ioman->GetObject("TRDPoint");
  fInMuch = (TClonesArray*) ioman->GetObject("MUCHPoint");
  fInTof  = (TClonesArray*) ioman->GetObject("TOFPoint");
  fInEcal = (TClonesArray*) ioman->GetObject("ECALPOint");
  fInPsd  = (TClonesArray*) ioman->GetObject("PSDPOint");

  // Register output array
  fEpoch = new CbmMCEpoch(0., fEpochLength);
  fEpochIsChanged = kFALSE;
  ioman->Register("MCEpoch.", "MC epoch", fEpoch, fPersistence); 




  cout << endl;
  cout << "-I-" << endl;
  cout << "-I- Average event spacing " << fBeamTau << " ns" << endl;
  if ( ! fBeamProfile ) cout << "-I- Beam model: constant" << endl;
  else                  cout << "-I- Beam model: exponential" << endl;
  cout << "---------------------------------------------" << endl;

  return kSUCCESS;

}
// ---------------------------------------------------------------------------




// -----   Execution   -------------------------------------------------------
void CbmMCStreamer::Exec(Option_t* opt) {

  Int_t nPoints = 0;
  cout.precision(8);

  // Unset epoch change marker
  fEpochIsChanged = kFALSE;
  

  // Determine event time
  Double_t deltaT = 0;
  if ( ! fBeamProfile ) deltaT = fBeamTau;
  else                  deltaT = TMath::Nint(gRandom->Exp(fBeamTau));
  fEventTime += deltaT;
  cout << "-I- " << GetName() << ": New event at " << fEventTime
       << " ns" << endl;


  // Read event into buffer. If empty, skip execution (next event)
  if ( ! ReadEvent() ) {
    cout << "-I- " << GetName() << ": No points in event; stop execution"
	 << endl;
    return;
  }
  PrintBuffer();

  // Execute this loop until something is filled into the current epoch
  while ( kTRUE ) {
\
    // Fill points from buffer into current epoch
    ProcessBuffer();
    PrintBuffer();

    // If epoch changed (points added): end event execution (go to next event)
    if ( fEpochIsChanged ) {
      cout << "-I- " << GetName() << ": Epoch was updated. End excution."
	   << endl;
      break;
    }

    // Nothing added to epoch: fill tree and start next epoch
      if ( ! fEpoch->IsEmpty() ) {
	cout << "-I- " << GetName() << ": Filling epoch to output" << endl;
	fEpoch->Print();
	FairRootManager::Instance()->Fill();
      }
    Double_t newEpochTime = fEpoch->GetStartTime() + fEpochLength;;
    fEpoch->Clear();
    fEpoch->SetStartTime(newEpochTime);
    fEpochIsChanged = kFALSE;
    cout << "-I- " << GetName() << ": New epoch at " << newEpochTime << endl;

  }

}
// ---------------------------------------------------------------------------




// -----   Finish run   ------------------------------------------------------
void CbmMCStreamer::Finish() {

  // Fill last epoch into output
  if ( ! fEpoch->IsEmpty() ) {
	cout << "-I- " << GetName() << ": Filling epoch to output" << endl;
	fEpoch->Print();
	FairRootManager::Instance()->Fill();
  }

}
// ---------------------------------------------------------------------------




// -----   Read one event into the buffer   ----------------------------------
Int_t CbmMCStreamer::ReadEvent() {

  Int_t nPointsAdded = 0;

  if ( fStsPoints ) {
    Int_t    nStsPoints = 0;
    CbmStsPoint* stsPoint = NULL;
    nStsPoints = fStsPoints->GetEntriesFast();
    for (Int_t iPoint = 0; iPoint < nStsPoints; iPoint++) {
      stsPoint = (CbmStsPoint*) fStsPoints->At(iPoint);
      Double_t pTime = stsPoint->GetTime() + fEventTime;
      stsBuffer.insert( pair<Double_t,CbmStsPoint>
			(pTime, CbmStsPoint(*stsPoint, fEventTime, 0.) ) );
      nPointsAdded++;
    }

    cout << "-I- " << GetName() << ": " << nStsPoints << 
      " points added to buffer" << endl;
  }

  return nPointsAdded;
}
// ---------------------------------------------------------------------------




// -----   Print buffer content (for debug)   --------------------------------
void CbmMCStreamer::PrintBuffer() {

  cout << "-I- " << GetName() << ": Entries in buffer: ";
  cout << "STS " << stsBuffer.size() << endl;

}
// ---------------------------------------------------------------------------

  


// -----   Process buffer   --------------------------------------------------
void CbmMCStreamer::ProcessBuffer() {

  map<Double_t,CbmStsPoint>::iterator it;
  map<Double_t,CbmStsPoint>::iterator eraseUntilHere = stsBuffer.begin();
  for (it = stsBuffer.begin(); it != stsBuffer.end(); it++) {

    // Error if point time is before current epoch
    if ( ( (*it).second).GetTime() < fEpoch->GetStartTime() ) {
      cout << "-E- " << GetName() 
	   << "::Exec: MCPoint time before current epoch!" << endl;
      cout << "    " << "Point time: " << ((*it).second).GetTime() 
	   << " ns" << endl;
      cout << "    " << "Current epoch time " << fEpoch->GetStartTime()
	   << " ns" << endl;
      Fatal("Exec", "Cannot treat MCPoint time");
    }

    // Exit loop if point time is after current epoch
    if ( ((*it).second).GetTime() > 
	 ( fEpoch->GetStartTime() + fEpochLength ) ) {
      cout << "-I- " << GetName() << ": Point time " 
	   << (*it).second.GetTime() << " outside epoch" << endl;
      break;
    }

    // Add (copy) point to current epoch
    fEpoch->AddPoint(kSTS, (*it).second, 0.);
    cout <<  "-I- " << GetName() << ": Adding point to epoch at " 
	 << ((*it).second).GetTime() << endl;
    fEpochIsChanged = kTRUE;

    // Mark for deletion
    eraseUntilHere = it;

  }

  // Remove copied points from the buffer
  if ( fEpochIsChanged ) stsBuffer.erase(stsBuffer.begin(), ++eraseUntilHere);

}
// ---------------------------------------------------------------------------

    


ClassImp(CbmMCStreamer)
