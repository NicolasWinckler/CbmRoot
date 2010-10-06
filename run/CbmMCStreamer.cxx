/** CbmMCStreamer.cxx
 **@author Volker Friese <v.friese@gsi.de>
 **@since 13.11.2009
 **version 1.0
 **/


#include "CbmMCStreamer.h"

#include "CbmMCEpoch.h"
#include "CbmMCEvent.h"

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
CbmMCStreamer::CbmMCStreamer(Double_t eventRate, 
			     Int_t beamProfile,
			     Double_t epochLength,
			     Bool_t persistence, 
			     Int_t verbosity, 
			     const char* name) 
  : FairTask(name, verbosity), 
    fEventRate(eventRate),
    fEventTau(1.e9 / eventRate),
    fBeamProfile(beamProfile),
    fPersistence(persistence),
    fEpochLength(epochLength),
    fEvent(NULL),
    fEventId(-1),
    fEventTime(0.),
    fEpoch(NULL) {
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
  fEvent      = (CbmMCEvent*)   ioman->GetObject("MCEvent");
  fStsPoints  = (TClonesArray*) ioman->GetObject("StsPoint");
  fMuchPoints = (TClonesArray*) ioman->GetObject("MuchPoint");
  

  // Check MC event header
  if ( ! fEvent ) {
    cout << "-E- No MC event header branch found in file!" << endl;
    return kFATAL;
  }

  // Register output array
  fEpoch = new CbmMCEpoch(0., fEpochLength);
  fEpochIsChanged = kFALSE;
  ioman->Register("MCEpoch.", "MC epoch", fEpoch, fPersistence); 




  cout << endl;
  cout << "-I-" << endl;
  cout << "-I- Average event spacing " << fEventTau << " ns" << endl;
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
  

  // Determine event time and event ID
  if ( ! fEvent ) Fatal("ReadEvent", "No MC event header");
  fEventId = fEvent->GetEventID();
  Double_t deltaT = 0;
  if ( ! fBeamProfile ) deltaT = fEventTau;
  else                  deltaT = TMath::Nint(gRandom->Exp(fEventTau));
  fEventTime += deltaT;
  cout << "-I- " << GetName() << ": New event " << fEventId << " at " 
       << fEventTime << " ns" << endl;


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
			(pTime, CbmStsPoint(*stsPoint, fEventId, 
					    fEventTime, 0.) ) );
      nPointsAdded++;
    }
  }

  if ( fMuchPoints ) {
    Int_t    nMuchPoints = 0;
    CbmMuchPoint* muchPoint = NULL;
    nMuchPoints = fMuchPoints->GetEntriesFast();
    for (Int_t iPoint = 0; iPoint < nMuchPoints; iPoint++) {
      muchPoint = (CbmMuchPoint*) fMuchPoints->At(iPoint);
      Double_t pTime = muchPoint->GetTime() + fEventTime;
      muchBuffer.insert( pair<Double_t,CbmMuchPoint>
			(pTime, CbmMuchPoint(*muchPoint, fEventId, 
					     fEventTime, 0.) ) );
      nPointsAdded++;
    }
  }

  cout << "-I- " << GetName() << ": " << nPointsAdded << 
    " points added to buffer" << endl;

  return nPointsAdded;
}
// ---------------------------------------------------------------------------




// -----   Print buffer content (for debug)   --------------------------------
void CbmMCStreamer::PrintBuffer() {

  cout << "-I- " << GetName() << ": Entries in buffer: ";
  cout << "STS " << stsBuffer.size();
  cout << " MUCH " << muchBuffer.size() << endl;

}
// ---------------------------------------------------------------------------

  


// -----   Process buffer   --------------------------------------------------
void CbmMCStreamer::ProcessBuffer() {


  // --- STS
  map<Double_t,CbmStsPoint>::iterator stsIt;
  while ( ! stsBuffer.empty() ) {
    stsIt = stsBuffer.begin();
    
    // Error if point time is before current epoch
    if ( ( (*stsIt).second).GetTime() < fEpoch->GetStartTime() ) {
      cout << "-E- " << GetName() 
	   << "::Exec: MCPoint time before current epoch!" << endl;
      cout << "    " << "Point time: " << ((*stsIt).second).GetTime() 
	   << " ns" << endl;
      cout << "    " << "Current epoch time " << fEpoch->GetStartTime()
	   << " ns" << endl;
      Fatal("Exec", "Cannot treat MCPoint time");
    }

    // Exit loop if point time is after current epoch
    if ( ((*stsIt).second).GetTime() > 
	 ( fEpoch->GetStartTime() + fEpochLength ) ) {
      cout << "-I- " << GetName() << ": Point time " 
	   << (*stsIt).second.GetTime() << " outside epoch" << endl;
      break;
    }

    // Add (copy) point to current epoch. Time relative to epoch start is
    // calculated by CbmMCEpoch.
    fEpoch->AddPoint(kSTS, &((*stsIt).second), fEventId, 0.);
    cout <<  "-I- " << GetName() << ": Adding STS point to epoch at " 
	 << ((*stsIt).second).GetTime() << endl;
    stsBuffer.erase(stsIt);
    fEpochIsChanged = kTRUE;

  }

  // --- MUCH
  /* I know it's not elegant copying the code, but do you have a better suggestion? */
  map<Double_t,CbmMuchPoint>::iterator muchIt;
  while ( ! muchBuffer.empty() ) {
    muchIt = muchBuffer.begin();
    
    // Error if point time is before current epoch
    if ( ( (*muchIt).second).GetTime() < fEpoch->GetStartTime() ) {
      cout << "-E- " << GetName() 
	   << "::Exec: MCPoint time before current epoch!" << endl;
      cout << "    " << "Point time: " << ((*muchIt).second).GetTime() 
	   << " ns" << endl;
      cout << "    " << "Current epoch time " << fEpoch->GetStartTime()
	   << " ns" << endl;
      Fatal("Exec", "Cannot treat MCPoint time");
    }

    // Exit loop if point time is after current epoch
    if ( ((*muchIt).second).GetTime() > 
	 ( fEpoch->GetStartTime() + fEpochLength ) ) {
      cout << "-I- " << GetName() << ": Point time " 
	   << (*muchIt).second.GetTime() << " outside epoch" << endl;
      break;
    }

    // Add (copy) point to current epoch. Time relative to epoch start is
    // calculated by CbmMCEpoch.
    fEpoch->AddPoint(kMUCH, &((*muchIt).second), fEventId, 0.);
    cout <<  "-I- " << GetName() << ": Adding MUCH point to epoch at " 
	 << ((*muchIt).second).GetTime() << endl;
    muchBuffer.erase(muchIt);
    fEpochIsChanged = kTRUE;

  }



}
// ---------------------------------------------------------------------------

    


ClassImp(CbmMCStreamer)
