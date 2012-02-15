// -------------------------------------------------------------------------
// -----                  CbmSttMatchTracks source file                -----
// -----                  Created 01/12/05  by V. Friese               -----
// -----          Adapted for STT 13/03/08  by A. Zinchenko            -----
// -------------------------------------------------------------------------

#include "CbmSttMatchTracks.h"

#include "CbmSttTrackMatch.h"
#include "CbmSttHit.h"
#include "CbmSttTrack.h"

#include "FairMCPoint.h"
#include "FairRootManager.h"

#include "TClonesArray.h"

#include <iostream>
#include <map>
using std::endl;
using std::cout;
using std::map;

// -----   Default constructor   -------------------------------------------
CbmSttMatchTracks::CbmSttMatchTracks() 
  : FairTask("STT track match"),
    fTracks(NULL),
    fPoints(NULL),
    fHits(NULL),
    fMatches(NULL),
    fMatchMap(),
    fVerbose(1)
{
}
// -------------------------------------------------------------------------

// -----   Constructor with verbosity level   ------------------------------
CbmSttMatchTracks::CbmSttMatchTracks(Int_t verbose) 
  : FairTask("STT track match"),
    fTracks(NULL),
    fPoints(NULL),
    fHits(NULL),
    fMatches(NULL),
    fMatchMap(),
    fVerbose(verbose)
{
}
// -------------------------------------------------------------------------

// -----   Constructor with name, title and verbosity  ---------------------
CbmSttMatchTracks::CbmSttMatchTracks(const char* name, const char* title,
				     Int_t verbose) 
  : FairTask(name),
    fTracks(NULL),
    fPoints(NULL),
    fHits(NULL),
    fMatches(NULL),
    fMatchMap(),
    fVerbose(verbose)
 {
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmSttMatchTracks::~CbmSttMatchTracks() { }
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus CbmSttMatchTracks::Init() {
  
  // Get FairRootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if (! ioman) {
    cout << "-E- CbmSttMatchTracks::Init: "
	 << "RootManager not instantised!" << endl;
    return kFATAL;
  }

  // Get Stt hit Array
  fHits  = (TClonesArray*) ioman->GetObject("SttHit");
  if ( ! fHits) {
    cout << "-W- CbmSttMatchTracks::Init: No SttHit array!"
	 << endl;
    return kERROR;
  }

  // Get SttTrack Array
  fTracks = (TClonesArray*) ioman->GetObject("SttTrack");
  if ( ! fTracks ) {
    cout << "-E- CbmSttMatchTracks::Init: No SttTrack array!" << endl;
    return kERROR;
  }

  // Get SttPoint array
  fPoints = (TClonesArray*) ioman->GetObject("SttPoint");
  if ( ! fPoints ) {
    cout << "-E- CbmSttMatchTracks::Init: No SttPoint array!" << endl;
    return kERROR;
  }

  // Create and register TrdTrackMatch array
  fMatches = new TClonesArray("CbmSttTrackMatch",100);
  ioman->Register("SttTrackMatch", "STT", fMatches, kTRUE);

  return kSUCCESS;

}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void CbmSttMatchTracks::Exec(Option_t* opt) {

  // Clear output array
  fMatches->Clear();

  // Create some pointers and variables
  CbmSttTrack*      track = NULL;
  CbmSttHit*        hit   = NULL;
  FairMCPoint*       point = NULL;
  Int_t nHits       = 0;
  Int_t nMCTracks   = 0;
  Int_t iPoint      = 0;
  Int_t iMCTrack    = 0;
  Int_t nAll        = 0;
  Int_t nTrue       = 0;
  Int_t nWrong      = 0;
  Int_t nFake       = 0;
  Int_t nHitSum     = 0;
  Int_t nTrueSum    = 0;
  Int_t nWrongSum   = 0;
  Int_t nFakeSum    = 0;
  Int_t nMCTrackSum = 0;
  map<Int_t, Int_t>::iterator it;

  // Loop over SttTracks
  Int_t nTracks = fTracks->GetEntriesFast();
  for (Int_t iTrack=0; iTrack<nTracks; ++iTrack) {
    track = (CbmSttTrack*) fTracks->UncheckedAt(iTrack);
    if ( ! track) {
      cout << "-W- CbmSttMatchTracks::Exec: Empty SttTrack at " 
	   << iTrack << endl;
      continue;
    }
    //nHits = track->GetNofSttHits();
    nHits = track->GetNHits();
    nAll = nTrue = nWrong = nFake = nMCTracks = 0;
    fMatchMap.clear();
    if (fVerbose > 2) cout << endl << "Track " << iTrack << ", SttHits " 
			   << nHits << endl;

    // Loop over STTHits of track
    for (Int_t iHit=0; iHit<nHits; ++iHit) {
      //hit = (CbmSttHit*) fHits->UncheckedAt(track->GetSttHitIndex(iHit));
      hit = (CbmSttHit*) fHits->UncheckedAt(track->GetHitIndex(iHit));
      if ( ! hit ) {
	cout << "-E- CbmSttMatchTracks::Exec: "
	     << "No SttHit " << iHit << " for track " << iTrack << endl;
	continue;
      }
      iPoint = hit->GetRefIndex();
      if ( iPoint < 0 ) {        // Fake or background hit
	nFake++;
	continue;
      }
      point = (FairMCPoint*) fPoints->At(iPoint);
      if ( ! point ) {
	cout << "-E- CbmSttMatchTracks::Exec: "
	     << "Empty MCPoint " << iPoint << " from TrdHit " << iHit
	     << " (track " << iTrack << ")" << endl;
	continue;
      }
      iMCTrack = point->GetTrackID();
      if ( fVerbose > 2 ) cout << "Track " << iTrack << ", STT hit "
			    //			       << track->GetSttHitIndex(iHit) 
			       << track->GetHitIndex(iHit) 
			       << ", STTPoint " << iPoint << ", MCTrack "
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
    if (fVerbose>1) cout << "-I- CbmSttMatchTracks: SttTrack " << iTrack 
			 << ", MCTrack " << iMCTrack << ", true " 
			 << nTrue << ", wrong " << nWrong << ", fake " 
			 << nFake << ", #MCTracks " << nMCTracks << endl;

    // Create SttTrackMatch
    new ((*fMatches)[iTrack]) CbmSttTrackMatch(iMCTrack, nTrue, 
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
  Double_t qTrue = 0.;
  if ( nHitSum) qTrue  = Double_t(nTrueSum)  / Double_t(nHitSum) * 100.;
  if (nHitSum && fVerbose) {
    Double_t qWrong = Double_t(nWrongSum) / Double_t(nHitSum) * 100.;
    Double_t qFake  = Double_t(nFakeSum)  / Double_t(nHitSum) * 100.;
    Double_t qMC    = Double_t(nMCTrackSum) / Double_t(nTracks);
    cout << endl;
    cout << "-------------------------------------------------------" 
	 << endl;
    cout << "-I-              STT Track Matching                 -I-"
	 << endl;
    cout << "Reconstructed SttTracks : " << nTracks << endl;;
    cout << "True  hit assignments   : " << qTrue  << " %" << endl;
    cout << "Wrong hit assignments   : " << qWrong << " %" << endl;
    cout << "Fake  hit assignments   : " << qFake  << " %" << endl;
    cout << "MCTracks per SttTrack   : " << qMC << endl;
    cout << "--------------------------------------------------------" 
	 << endl;
  }
  else cout << "-I- CbmSttMatchTracks: rec. " << nTracks << ", quota "
	    << qTrue << " % " << endl;

}
// -------------------------------------------------------------------------
    
// -----   Public method Finish   ------------------------------------------
void CbmSttMatchTracks::Finish() { }
// -------------------------------------------------------------------------

ClassImp(CbmSttMatchTracks)


      

      
