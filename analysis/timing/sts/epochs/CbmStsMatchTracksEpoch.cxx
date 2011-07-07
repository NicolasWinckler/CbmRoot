#include "CbmStsMatchTracksEpoch.h"

#include "CbmStsHit.h"
#include "CbmStsTrack.h"
#include "CbmTrackMatch.h"
#include "CbmMCEpoch.h"

#include "FairMCPoint.h"
#include "FairRootManager.h"

#include "TClonesArray.h"

#include <iostream>
#include <iomanip>
#include <map>

using std::cout;
using std::endl;
using std::left;
using std::right;
using std::setw;
using std::fixed;
using std::setprecision;
using std::map;

// -----   Default constructor   -------------------------------------------
CbmStsMatchTracksEpoch::CbmStsMatchTracksEpoch()
  : FairTask("STSMatchTracks"), 
  fTracks(NULL),
  fPoints(NULL),
  fHits(NULL),
  fMatches(NULL),
  fTime(0.),
  fNTrackMatches(0.),
  fNAllHits(0.),
  fNTrueHits(0.),
  fNEvents(0),
  fNEventsFailed(0),
  fTimer(),
  fMatchMap()
{}
// -------------------------------------------------------------------------



// -----   Standard constructor  -------------------------------------------
CbmStsMatchTracksEpoch::CbmStsMatchTracksEpoch(Int_t iVerbose)
  : FairTask("STSMatchTracks", iVerbose),
  fTracks(NULL),
  fPoints(NULL),
  fHits(NULL),
  fMatches(NULL),
  fTime(0.),
  fNTrackMatches(0.),
  fNAllHits(0.),
  fNTrueHits(0.),
  fNEvents(0),
  fNEventsFailed(0),
  fTimer(),
  fMatchMap()
{}
// -------------------------------------------------------------------------



// -----   Constructor with task name   ------------------------------------
CbmStsMatchTracksEpoch::CbmStsMatchTracksEpoch(const char* name, Int_t iVerbose)
  : FairTask(name, iVerbose),
  fTracks(NULL),
  fPoints(NULL),
  fHits(NULL),
  fMatches(NULL),
  fTime(0.),
  fNTrackMatches(0.),
  fNAllHits(0.),
  fNTrueHits(0.),
  fNEvents(0),
  fNEventsFailed(0),
  fTimer(),
  fMatchMap()
{}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmStsMatchTracksEpoch::~CbmStsMatchTracksEpoch() { }
// -------------------------------------------------------------------------



// -----   Virtual public method Exec   ------------------------------------
void CbmStsMatchTracksEpoch::Exec(Option_t* opt) {
  fPoints = fMcEpoch->GetPoints(kSTS);

  // Timer
  fTimer.Start();
  Bool_t warn = kFALSE;

  // Clear output array
  //  fMatches->Clear();
  fMatches->Delete();

  // Create some pointers and variables
  CbmStsTrack* track = NULL;
  CbmStsHit*   hit  = NULL;
  FairMCPoint*  point = NULL;
  Int_t nHits    = 0;
  Int_t nMCTracks = 0;
  Int_t iPoint    = 0;
  Int_t iMCTrack  = 0;
  Int_t nAll      = 0;
  Int_t nTrue     = 0;
  Int_t nWrong    = 0;
  Int_t nFake     = 0;
  Int_t nHitSum     = 0;
  Int_t nTrueSum    = 0;
  Int_t nWrongSum   = 0;
  Int_t nFakeSum    = 0;
  Int_t nMCTrackSum = 0;
  map<Int_t, Int_t>::iterator it;

  // Loop over StsTracks
  Int_t nTracks = fTracks->GetEntriesFast();
  for (Int_t iTrack=0; iTrack<nTracks; iTrack++) {
    track = (CbmStsTrack*) fTracks->At(iTrack);
    if ( ! track) {
      cout << "-W- CbmStsMatchTracksEpoch::Exec: Empty StsTrack at "
	   << iTrack << endl;
      warn = kTRUE;
      continue;
    }
    nHits = track->GetNStsHits();
    nAll = nTrue = nWrong = nFake = nMCTracks = 0;
    fMatchMap.clear();
    if (fVerbose > 2) cout << endl << "Track " << iTrack << ", Hits "
			   << nHits << endl;

    // Loop over StsHits of track
    for (Int_t iHit=0; iHit<nHits; iHit++) {
      hit = (CbmStsHit*) fHits->At(track->GetStsHitIndex(iHit));
      if ( ! hit ) {
	cout << "-E- CbmStsMatchTracksEpoch::Exec: "
	     << "No StsHit " << iHit << " for track " << iTrack << endl;
	warn = kTRUE;
	continue;
      }
      iPoint = hit->GetRefIndex();
      if ( iPoint < 0 ) {        // Fake or background hit
	nFake++;
	continue;
      }
      point = (FairMCPoint*) fPoints->At(iPoint);
      if ( ! point ) {
	cout << "-E- CbmStsMatchTracksEpoch::Exec: "
	     << "Empty MCPoint " << iPoint << " from MapsHit " << iHit
	     << " (track " << iTrack << ")" << endl;
	warn = kTRUE;
	continue;
      }
      iMCTrack = point->GetTrackID();
      if ( fVerbose > 2 ) cout << "Track " << iTrack << ", MAPS hit "
			       << track->GetStsHitIndex(iHit)
			       << ", StsPoint " << iPoint << ", MCTrack "
			       << iMCTrack << endl;
      fMatchMap[iMCTrack]++;
    }


    // Search for best matching MCTrack
    iMCTrack = -1;
    for (it=fMatchMap.begin(); it!=fMatchMap.end(); it++) {
      if (fVerbose > 2) cout << it->second
			     << " common points wth MCtrack "
			     << it->first << endl;
      nMCTracks++;
      nAll += it->second;
      if ( it->second > nTrue ) {
	iMCTrack = it->first;
	nTrue    = it->second;
      }
    }
    nWrong = nAll - nTrue;
    if (fVerbose>1) cout << "-I- CbmStsMatchTracksEpoch: StsTrack " << iTrack
			 << ", MCTrack " << iMCTrack << ", true "
			 << nTrue << ", wrong " << nWrong << ", fake "
			 << nFake << ", #MCTracks " << nMCTracks << endl;

    // Create StsTrackMatch
    new ((*fMatches)[iTrack]) CbmTrackMatch(iMCTrack, nTrue,
					       nWrong, nFake,
					       nMCTracks);

    // Some statistics
    nHitSum     += nHits;
    nTrueSum    += nTrue;
    nWrongSum   += nWrong;
    nFakeSum    += nFake;
    nMCTrackSum += nMCTracks;

  } // Track loop

  // Event statistics
  fTimer.Stop();
  Double_t qTrue = 0.;
  if ( nHitSum) qTrue  = Double_t(nTrueSum)  / Double_t(nHitSum) * 100.;
  if (fVerbose > 1) {
    Double_t qWrong = Double_t(nWrongSum) / Double_t(nHitSum) * 100.;
    Double_t qFake  = Double_t(nFakeSum)  / Double_t(nHitSum) * 100.;
    Double_t qMC    = Double_t(nMCTrackSum) / Double_t(nTracks);
    cout << endl;
    cout << "-------------------------------------------------------"
	 << endl;
    cout << "-I-              Sts Track Matching                 -I-"
	 << endl;
    cout << "Reconstructed StsTracks : " << nTracks << endl;;
    cout << "True  hit assignments   : " << qTrue  << " %" << endl;
    cout << "Wrong hit assignments   : " << qWrong << " %" << endl;
    cout << "Fake  hit assignments   : " << qFake  << " %" << endl;
    cout << "MCTracks per StsTrack   : " << qMC << endl;
    cout << "--------------------------------------------------------"
	 << endl;
  }
  if (fVerbose == 1) {
    if ( warn) cout << "- ";
    else       cout << "+ ";
    cout << setw(15) << left << fName << ": " << setprecision(4) << setw(8)
	 << fixed << right << fTimer.RealTime() << " s, matches "
	 << nTracks << ", hit quota " << qTrue << " %" << endl;
  }

  // Run statistics
  if ( warn ) fNEventsFailed++;
  else {
    fNEvents++;
    fTime          += fTimer.RealTime();
    fNTrackMatches += Double_t(nTracks);
    fNAllHits      += Double_t(nHitSum);
    fNTrueHits     += Double_t(nTrueSum);
  }

}
// -------------------------------------------------------------------------



// -----   Virtual private method Init   -----------------------------------
InitStatus CbmStsMatchTracksEpoch::Init() {

  // Get FairRootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if (! ioman) {
    cout << "-E- CbmStsMatchTracksEpoch::Init: "
	 << "RootManager not instantised!" << endl;
    return kFATAL;
  }

  // Get StsHit Array
  fHits = (TClonesArray*) ioman->GetObject("StsHit");
  if ( ! fHits) {
    cout << "-W- CbmStsMatchTracksEpoch::Init: No StsHit array!"
	 << endl;
  }

  // Get StsTrack Array
  fTracks = (TClonesArray*) ioman->GetObject("StsTrack");
  if ( ! fTracks ) {
    cout << "-E- CbmStsMatchTracksEpoch::Init: No StsTrack array!" << endl;
    return kERROR;
  }

  // Get StsPoint array
//  fPoints = (TClonesArray*) ioman->GetObject("StsPoint");
//  if ( ! fPoints ) {
//    cout << "-E- CbmStsMatchTracksEpoch::Init: No StsPoint array!" << endl;
//    return kERROR;
//  }
  fMcEpoch = (CbmMCEpoch*) ioman->GetObject("MCEpoch.");
  
  // Create and register StsTrackMatch array
  fMatches = new TClonesArray("CbmTrackMatch",100);
  ioman->Register("StsTrackMatch", "STS", fMatches, kTRUE);

  return kSUCCESS;

}
// -------------------------------------------------------------------------




// -----   Virtual private method Finish   ---------------------------------
void CbmStsMatchTracksEpoch::Finish() {

  cout << endl;
  cout << "============================================================"
       << endl;
  cout << "=====   " << GetName() << ": Run summary " << endl;
  cout << "===== " << endl;
  cout << "===== Good events   : " << setw(6) << fNEvents << endl;
  cout << "===== Failed events : " << setw(6) << fNEventsFailed << endl;
  cout << "===== Average time  : " << setprecision(4) << setw(8) << right
       << fTime / Double_t(fNEvents)  << " s" << endl;
  cout << "===== " << endl;
  cout << "===== Tracks per event  : " << fixed << setprecision(0)
       << fNTrackMatches / Double_t(fNEvents) << endl;
  cout << setprecision(2);
  cout << "===== True hits         : " << fixed << setw(6) << right
       << fNTrueHits / fNAllHits * 100. << " %" << endl;
  cout << "============================================================"
       << endl;

}
// -------------------------------------------------------------------------



ClassImp(CbmStsMatchTracksEpoch)





