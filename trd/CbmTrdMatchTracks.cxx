// -------------------------------------------------------------------------
// -----                  CbmTrdMatchTracks source file                -----
// -----                  Created 01/12/05  by V. Friese               -----
// -------------------------------------------------------------------------

#include "CbmTrdMatchTracks.h"

#include "CbmTrackMatch.h"
#include "CbmTrdHit.h"
#include "CbmTrdTrack.h"

#include "FairMCPoint.h"
#include "FairRootManager.h"

#include "TClonesArray.h"

#include <iostream>
#include <map>
using std::endl;
using std::cout;
using std::map;

// -----   Default constructor   -------------------------------------------
CbmTrdMatchTracks::CbmTrdMatchTracks()
  : FairTask("TRD track match")
{
	fTracks  = NULL;
	fPoints  = NULL;
	fHits    = NULL;
	fMatches = NULL;
	fVerbose = 1;
}
// -------------------------------------------------------------------------

// -----   Constructor with verbosity level   ------------------------------
CbmTrdMatchTracks::CbmTrdMatchTracks(
		Int_t verbose)
  : FairTask("TRD track match")
{
	fTracks  = NULL;
	fPoints  = NULL;
	fHits    = NULL;
	fMatches = NULL;
	fVerbose = verbose;
}
// -------------------------------------------------------------------------

// -----   Constructor with name, title and verbosity  ---------------------
CbmTrdMatchTracks::CbmTrdMatchTracks(
		const char* name,
		const char* title,
		Int_t verbose)
  : FairTask(name)
{
	fTracks  = NULL;
	fPoints  = NULL;
	fHits    = NULL;
	fMatches = NULL;
	fVerbose = verbose;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmTrdMatchTracks::~CbmTrdMatchTracks()
{
}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus CbmTrdMatchTracks::Init()
{
	// Get FairRootManager
	FairRootManager* ioman = FairRootManager::Instance();
	if (ioman == NULL) Fatal("CbmTrdMatchTracks::Init", "RootManager not instantised!");

	// Get TRD hit Array
	fHits  = (TClonesArray*) ioman->GetObject("TRDHit");
	if (fHits == NULL) Fatal("CbmTrdMatchTracks::Init", "No TRDHit array!");

	// Get TrdTrack Array
	fTracks = (TClonesArray*) ioman->GetObject("TRDTrack");
	if (fTracks == NULL) Fatal("CbmTrdMatchTracks::Init", "No TRDTrack array!");

	// Get TRDPoint array
	fPoints = (TClonesArray*) ioman->GetObject("TRDPoint");
	if (fPoints == NULL) Fatal("CbmTrdMatchTracks::Init", "No TRDPoint array!");

	// Create and register TrdTrackMatch array
	fMatches = new TClonesArray("CbmTrackMatch",100);
	ioman->Register("TRDTrackMatch", "TRD", fMatches, kTRUE);

	return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void CbmTrdMatchTracks::Exec(Option_t* opt)
{
	// Clear output array
	fMatches->Clear();

	// Create some pointers and variables
	CbmTrdTrack*      track = NULL;
	CbmTrdHit*        hit   = NULL;
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

	// Loop over TrdTracks
	Int_t nTracks = fTracks->GetEntriesFast();
	for (Int_t iTrack=0; iTrack<nTracks; iTrack++) {
	track = (CbmTrdTrack*) fTracks->At(iTrack);
	if (!track) {
		cout << "-W- CbmTrdMatchTracks::Exec: Empty TrdTrack at " << iTrack << endl;
		continue;
	}
	nHits = track->GetNofHits();
	nAll = nTrue = nWrong = nFake = nMCTracks = 0;
	fMatchMap.clear();
	if (fVerbose > 2) cout << endl << "Track " << iTrack << ", TrdHits " << nHits << endl;

	// Loop over TRDHits of track
	for (Int_t iHit=0; iHit<nHits; iHit++) {
		hit = (CbmTrdHit*) fHits->At(track->GetHitIndex(iHit));
		if (!hit) {
			cout << "-E- CbmTrdMatchTracks::Exec: " << "No TrdHit " << iHit << " for track " << iTrack << endl;
			continue;
		}
		iPoint = hit->GetRefId();
		if (iPoint < 0) { // Fake or background hit
			nFake++;
			continue;
		}
		point = (FairMCPoint*) fPoints->At(iPoint);
		if (!point ) {
			cout << "-E- CbmTrdMatchTracks::Exec: "  << "Empty MCPoint " << iPoint
				<< " from TrdHit " << iHit << " (track " << iTrack << ")" << endl;
			continue;
		}
		iMCTrack = point->GetTrackID();
		if ( fVerbose > 2 )
			cout << "Track " << iTrack << ", TRD hit " << track->GetHitIndex(iHit)
				   << ", TRDPoint " << iPoint << ", MCTrack " << iMCTrack << endl;
		fMatchMap[iMCTrack]++;
	}

	// Search for best matching MCTrack
	iMCTrack = -1;
	for (it=fMatchMap.begin(); it!=fMatchMap.end(); it++) {
		if (fVerbose > 2)
			cout << it->second << " common points wth MCtrack " << it->first << endl;
		nMCTracks++;
		nAll += it->second;
		if ( it->second > nTrue ) {
			iMCTrack = it->first;
			nTrue    = it->second;
		}
	}
	nWrong = nAll - nTrue;
	if (fVerbose>1) cout << "-I- CbmTrdMatchTracks: TRDTrack " << iTrack
			 << ", MCTrack " << iMCTrack << ", true "
			 << nTrue << ", wrong " << nWrong << ", fake "
			 << nFake << ", #MCTracks " << nMCTracks << endl;

	// Create TrdTrackMatch
	new ((*fMatches)[iTrack]) CbmTrackMatch(iMCTrack, nTrue, nWrong, nFake, nMCTracks);

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
	if (fVerbose) {
		Double_t qWrong = Double_t(nWrongSum) / Double_t(nHitSum) * 100.;
		Double_t qFake  = Double_t(nFakeSum)  / Double_t(nHitSum) * 100.;
		Double_t qMC    = Double_t(nMCTrackSum) / Double_t(nTracks);
		cout << endl;
		cout << "-------------------------------------------------------"
		 << endl;
		cout << "-I-              TRD Track Matching                 -I-"<< endl;
		cout << "Reconstructed TRDTracks : " << nTracks << endl;;
		cout << "True  hit assignments   : " << qTrue  << " %" << endl;
		cout << "Wrong hit assignments   : " << qWrong << " %" << endl;
		cout << "Fake  hit assignments   : " << qFake  << " %" << endl;
		cout << "MCTracks per TRDTrack   : " << qMC << endl;
		cout << "--------------------------------------------------------" << endl;
	}
	else cout << "-I- CbmTrdMatchTracks: rec. " << nTracks << ", quota "
		<< qTrue << " % " << endl;

}
// -------------------------------------------------------------------------

// -----   Public method Finish   ------------------------------------------
void CbmTrdMatchTracks::Finish()
{

}
// -------------------------------------------------------------------------

ClassImp(CbmTrdMatchTracks)
