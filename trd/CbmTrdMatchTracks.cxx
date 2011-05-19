// -------------------------------------------------------------------------
// -----                  CbmTrdMatchTracks source file                -----
// -----                  Created 01/12/05  by V. Friese               -----
// -------------------------------------------------------------------------

#include "CbmTrdMatchTracks.h"

#include "CbmTrackMatch.h"
#include "CbmTrdHit.h"
#include "CbmTrdTrack.h"
#include "CbmTrdDigiMatch.h"
#include "CbmTrdDigi.h"
#include "CbmTrdCluster.h"

#include "FairMCPoint.h"
#include "FairRootManager.h"

#include "TClonesArray.h"

#include <iostream>
#include <map>

// -----   Default constructor   -------------------------------------------
CbmTrdMatchTracks::CbmTrdMatchTracks() :
	FairTask("TRD track match", 1),
	fTracks(NULL),
	fPoints(NULL),
	fHits(NULL),
	fClusters(NULL),
	fDigiMatches(NULL),
//	fDigis(NULL),
	fMatches(NULL),
	fNofHits(0),
	fNofTrueHits(0),
	fNofWrongHits(0),
	fNofFakeHits(0),
	fNEvents(0),
	fUseDigis(kTRUE),
	fUseClusters(kTRUE)
{
}
// -------------------------------------------------------------------------

// -----   Constructor with verbosity level   ------------------------------
CbmTrdMatchTracks::CbmTrdMatchTracks(
		Int_t verbose) :
	FairTask("TRD track match", verbose),
	fTracks(NULL),
	fPoints(NULL),
	fHits(NULL),
	fClusters(NULL),
	fDigiMatches(NULL),
//	fDigis(NULL),
	fMatches(NULL),
	fNofHits(0),
	fNofTrueHits(0),
	fNofWrongHits(0),
	fNofFakeHits(0),
	fNEvents(0),
	fUseDigis(kTRUE),
	fUseClusters(kTRUE)
{
}
// -------------------------------------------------------------------------

// -----   Constructor with name, title and verbosity  ---------------------
CbmTrdMatchTracks::CbmTrdMatchTracks(
		const char* name,
		const char* title,
		Int_t verbose) :
	FairTask(name, verbose),
	fTracks(NULL),
	fPoints(NULL),
	fHits(NULL),
	fClusters(NULL),
	fDigiMatches(NULL),
//	fDigis(NULL),
	fMatches(NULL),
	fNofHits(0),
	fNofTrueHits(0),
	fNofWrongHits(0),
	fNofFakeHits(0),
	fNEvents(0),
	fUseDigis(kTRUE),
	fUseClusters(kTRUE)
{
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmTrdMatchTracks::~CbmTrdMatchTracks() {
}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus CbmTrdMatchTracks::Init() {
	// Get FairRootManager
	FairRootManager* ioman = FairRootManager::Instance();
	if (ioman == NULL) Fatal("CbmTrdMatchTracks::Init", "RootManager not instantised!");

	// Get TrdHit array
	fHits = (TClonesArray*) ioman->GetObject("TrdHit");
	if (fHits == NULL) Fatal("CbmTrdMatchTracks::Init", "No TrdHit array!");

	// Get TrdCluster array
	fClusters = (TClonesArray*) ioman->GetObject("TrdCluster");
	fUseClusters = (fClusters == NULL) ? kFALSE : kTRUE;

	// Get TrdTrack array
	fTracks = (TClonesArray*) ioman->GetObject("TrdTrack");
	if (fTracks == NULL) Fatal("CbmTrdMatchTracks::Init", "No TrdTrack array!");

	// Get TrdPoint array
	fPoints = (TClonesArray*) ioman->GetObject("TrdPoint");
	if (fPoints == NULL) Fatal("CbmTrdMatchTracks::Init", "No TrdPoint array!");

	// Get TrdDigiMatch array
	fDigiMatches = (TClonesArray*) ioman->GetObject("TrdDigiMatch");
	fUseDigis = (fDigiMatches == NULL) ? kFALSE : kTRUE;

	// Get TrdDigi array
//	fDigis = (TClonesArray*) ioman->GetObject("TrdDigi");
	//if (fDigis == NULL) Fatal("CbmTrdMatchTracks::Init", "No TrdDigi array!");

	// Create and register TrdTrackMatch array
	fMatches = new TClonesArray("CbmTrackMatch", 100);
	ioman->Register("TrdTrackMatch", "TRD", fMatches, kTRUE);

	return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void CbmTrdMatchTracks::Exec(
		Option_t* opt) {
	if (!fUseDigis)
		ExecSmearing(opt);
	else {
	    if (!fUseClusters) ExecDigi(opt);
	    else ExecCluster(opt);
	}
	std::cout << "CbmTrdMatchTracks::Exec: event=" << fNEvents++ << std::endl;
}

// -----   Private method ExecSmearing   --------------------------------------------
void CbmTrdMatchTracks::ExecSmearing(
		Option_t* opt) {
	// Clear output array
	fMatches->Clear();

	// Create some pointers and variables
	CbmTrdTrack* track = NULL;
	CbmTrdHit* hit = NULL;
	FairMCPoint* point = NULL;
	Int_t nHits = 0;
	Int_t nMCTracks = 0;
	Int_t iPoint = 0;
	Int_t iMCTrack = 0;
	Int_t nAll = 0;
	Int_t nTrue = 0;
	Int_t nWrong = 0;
	Int_t nFake = 0;
	Int_t nHitSum = 0;
	Int_t nTrueSum = 0;
	Int_t nWrongSum = 0;
	Int_t nFakeSum = 0;
	Int_t nMCTrackSum = 0;

	/** Map from MCTrackID to number of common hits **/
	std::map<Int_t, Int_t> matchMap;
	std::map<Int_t, Int_t>::iterator it;

	// Loop over TrdTracks
	Int_t nTracks = fTracks->GetEntriesFast();
	for (Int_t iTrack = 0; iTrack < nTracks; iTrack++) {
		track = (CbmTrdTrack*) fTracks->At(iTrack);
		if (!track) {
			std::cout << "-W- CbmTrdMatchTracks::Exec: Empty TrdTrack at "
					<< iTrack << std::endl;
			continue;
		}
		nHits = track->GetNofHits();
		nAll = nTrue = nWrong = nFake = nMCTracks = 0;
		matchMap.clear();
		if (fVerbose > 2)
			std::cout << std::endl << "Track " << iTrack << ", TrdHits "
					<< nHits << std::endl;

		// Loop over TRD hits of track
		for (Int_t iHit = 0; iHit < nHits; iHit++) {
			hit = (CbmTrdHit*) fHits->At(track->GetHitIndex(iHit));
			if (!hit) {
				std::cout << "-E- CbmTrdMatchTracks::Exec: " << "No TrdHit "
						<< iHit << " for track " << iTrack << std::endl;
				continue;
			}
			iPoint = hit->GetRefId();
			if (iPoint < 0) { // Fake or background hit
				nFake++;
				continue;
			}
			point = (FairMCPoint*) fPoints->At(iPoint);
			if (!point) {
				std::cout << "-E- CbmTrdMatchTracks::Exec: "
						<< "Empty MCPoint " << iPoint << " from TrdHit "
						<< iHit << " (track " << iTrack << ")" << std::endl;
				continue;
			}
			iMCTrack = point->GetTrackID();
			if (fVerbose > 2)
				std::cout << "Track " << iTrack << ", TRD hit "
						<< track->GetHitIndex(iHit) << ", TRDPoint " << iPoint
						<< ", MCTrack " << iMCTrack << std::endl;
			matchMap[iMCTrack]++;
		}

		// Search for best matching MCTrack
		iMCTrack = -1;
		for (it = matchMap.begin(); it != matchMap.end(); it++) {
			if (fVerbose > 2)
				std::cout << it->second << " common points wth MCtrack "
						<< it->first << std::endl;
			nMCTracks++;
			nAll += it->second;
			if (it->second > nTrue) {
				iMCTrack = it->first;
				nTrue = it->second;
			}
		}
		nWrong = nAll - nTrue;
		if (fVerbose > 1)
			std::cout << "-I- CbmTrdMatchTracks: TrdTrack " << iTrack
					<< ", MCTrack " << iMCTrack << ", true " << nTrue
					<< ", wrong " << nWrong << ", fake " << nFake
					<< ", #MCTracks " << nMCTracks << std::endl;

		// Create TrdTrackMatch
		new ((*fMatches)[iTrack]) CbmTrackMatch(iMCTrack, nTrue, nWrong, nFake,	nMCTracks);

		// Some statistics
		nHitSum += nHits;
		nTrueSum += nTrue;
		nWrongSum += nWrong;
		nFakeSum += nFake;
		nMCTrackSum += nMCTracks;

	} // Track loop

	// Event statistics
	Double_t qTrue = 0.;
	if (nHitSum)
		qTrue = Double_t(nTrueSum) / Double_t(nHitSum) * 100.;
	if (fVerbose) {
		Double_t qWrong = Double_t(nWrongSum) / Double_t(nHitSum) * 100.;
		Double_t qFake = Double_t(nFakeSum) / Double_t(nHitSum) * 100.;
		Double_t qMC = Double_t(nMCTrackSum) / Double_t(nTracks);
		std::cout << std::endl;
		std::cout << "-------------------------------------------------------" << std::endl;
		std::cout << "-I-              TRD Track Matching                 -I-" << std::endl;
		std::cout << "Reconstructed TrdTracks : " << nTracks << std::endl;
		std::cout << "True  hit assignments   : " << qTrue << " %" << std::endl;
		std::cout << "Wrong hit assignments   : " << qWrong << " %" << std::endl;
		std::cout << "Fake  hit assignments   : " << qFake << " %" << std::endl;
		std::cout << "MCTracks per TrdTrack   : " << qMC << std::endl;
		std::cout << "--------------------------------------------------------" << std::endl;
	} else
		std::cout << "-I- CbmTrdMatchTracks: rec. " << nTracks << ", quota "
				<< qTrue << " % " << std::endl;
}
// -------------------------------------------------------------------------

// -----   Private method ExecDigi   --------------------------------------------
void CbmTrdMatchTracks::ExecDigi(
		Option_t* opt) {
	fMatches->Clear();

    Int_t nofTracks = fTracks->GetEntriesFast();
	for (Int_t iTrack = 0; iTrack < nofTracks; iTrack++) { // Loop over tracks
		std::map<Int_t, Int_t> matchMap;

		CbmTrdTrack* pTrack = (CbmTrdTrack*) fTracks->At(iTrack);
		if (pTrack == NULL)	continue;
		Int_t nofHits = pTrack->GetNofHits();
		for (Int_t iHit = 0; iHit < nofHits; iHit++) { // Loop over hits
			Int_t index = pTrack->GetHitIndex(iHit);
			CbmTrdHit* hit = (CbmTrdHit*) fHits->At(index);
			if (hit == NULL) continue;
			Int_t digiId = hit->GetRefId();
			CbmTrdDigiMatch* digiMatch = (CbmTrdDigiMatch*) fDigiMatches->At(digiId);
			if (digiMatch == NULL) continue;
			for (Int_t iPoint = 0; iPoint < digiMatch->GetNofRefs(); iPoint++) {
				Int_t pointIndex = digiMatch->GetRefIndex(iPoint);
				if (pointIndex < 0) { // Fake or background hit
					matchMap[-1]++;
					continue;
				}
				FairMCPoint* point = (FairMCPoint*) fPoints->At(pointIndex);
				if (point == NULL) continue;
				matchMap[point->GetTrackID()]++;
			}
		} // Loop over hits

		Int_t nofTrue = 0;
		Int_t bestMcTrackId = -1;
		Int_t nPoints = 0;
		for (std::map<Int_t, Int_t>::iterator it = matchMap.begin(); it != matchMap.end(); it++) {
			if (it->first != -1 && it->second > nofTrue) {
				bestMcTrackId = it->first;
				nofTrue = it->second;
			}
			nPoints += it->second;
		}

		Int_t nofFake = 0;
		Int_t nofWrong = nPoints - nofTrue - nofFake;
		Int_t nofMcTracks = matchMap.size() - 1;

		new ((*fMatches)[iTrack]) CbmTrackMatch(bestMcTrackId, nofTrue,
				nofWrong, nofFake, nofMcTracks);

		fNofHits += nPoints;
		fNofTrueHits += nofTrue;
		fNofWrongHits += nofWrong;
		fNofFakeHits += nofFake;

		if (fVerbose > 1)
			std::cout << "iTrack=" << iTrack << " mcTrack=" << bestMcTrackId
					<< " nPoints=" << nPoints << " nofTrue=" << nofTrue
					<< " nofWrong=" << nofWrong << " nofFake=" << nofFake
					<< " nofMcTracks=" << nofMcTracks << std::endl;
	} // Loop over tracks
}
// -------------------------------------------------------------------------

// -----   Private method ExecCluster   --------------------------------------------
void CbmTrdMatchTracks::ExecCluster(
		Option_t* opt) {
	fMatches->Clear();

	Int_t nofTracks = fTracks->GetEntriesFast();
	for (Int_t iTrack = 0; iTrack < nofTracks; iTrack++) { // Loop over tracks
		// std::map stores MC track id to number of contributions of this MC track
	   std::map<Int_t, Int_t> matchMap;

		CbmTrdTrack* pTrack = (CbmTrdTrack*) fTracks->At(iTrack);
		if (pTrack == NULL) continue;

		Int_t nofHits = pTrack->GetNofHits();
		for (Int_t iHit = 0; iHit < nofHits; iHit++) { // Loop over hits
		   // std::set stores MC track indices contributed to a certain hit
		   std::set<Int_t> mcIdHit;
			Int_t index = pTrack->GetHitIndex(iHit);
			CbmTrdHit* hit = (CbmTrdHit*) fHits->At(index);
			if (hit == NULL) continue;

			Int_t clusterId = hit->GetRefId();
         CbmTrdCluster* cluster = (CbmTrdCluster*) fClusters->At(clusterId);
			if (cluster == NULL) continue;
			for (Int_t iDigi = 0; iDigi < cluster->GetNDigis(); iDigi++){
             Int_t digiId = cluster->GetDigiIndex(iDigi);
			    CbmTrdDigiMatch* digiMatch = (CbmTrdDigiMatch*) fDigiMatches->At(digiId);
			    if (digiMatch == NULL) continue;
			    for (Int_t iPoint = 0; iPoint < digiMatch->GetNofRefs(); iPoint++) {
               Int_t pointIndex = digiMatch->GetRefIndex(iPoint);
               if (pointIndex < 0) { // Fake or background hit
                  mcIdHit.insert(-1);
                  continue;
               }
               FairMCPoint* point = (FairMCPoint*) fPoints->At(pointIndex);
               if (point == NULL) continue;
               mcIdHit.insert(point->GetTrackID());
			    }
			} // loop over cluster

			for (std::set<Int_t>::iterator it = mcIdHit.begin(); it != mcIdHit.end(); it++) {
			   matchMap[*it]++;
			}

		} // Loop over hits

		Int_t nofTrue = 0;
		Int_t bestMcTrackId = -1;
		Int_t nPoints = 0;
		for (std::map<Int_t, Int_t>::iterator it = matchMap.begin(); it != matchMap.end(); it++) {
			if (it->first != -1 && it->second >= nofTrue) {
				bestMcTrackId = it->first;
				nofTrue = it->second;
			}
			nPoints += it->second;
		}

		Int_t nofFake = 0;
		Int_t nofWrong = nofHits - nofTrue - nofFake;
		Int_t nofMcTracks = matchMap.size() - 1;

		new ((*fMatches)[iTrack]) CbmTrackMatch(bestMcTrackId, nofTrue,
				nofWrong, nofFake, nofMcTracks);

		fNofHits += nofHits;
		fNofTrueHits += nofTrue;
		fNofWrongHits += nofWrong;
		fNofFakeHits += nofFake;

		if (fVerbose > 1)
			std::cout << "iTrack=" << iTrack << " mcTrack=" << bestMcTrackId
					<< " nofHits=" << nofHits << " nPoints=" << nPoints << " nofTrue=" << nofTrue
					<< " nofWrong=" << nofWrong << " nofFake=" << nofFake
					<< " nofMcTracks=" << nofMcTracks << std::endl;
	} // Loop over tracks
}
// -------------------------------------------------------------------------


// -----   Public method Finish   ------------------------------------------
void CbmTrdMatchTracks::Finish() {
	Double_t trueHits = 100. * Double_t(fNofTrueHits) / Double_t(fNofHits);
	Double_t wrongHits = 100. * Double_t(fNofWrongHits) / Double_t(fNofHits);
	Double_t fakeHits = 100. * Double_t(fNofFakeHits) / Double_t(fNofHits);
	std::cout << "=================================================" << std::endl;
	std::cout << "=====   " << GetName() << ": Run summary " << std::endl;
	std::cout << "True hits: " << trueHits << "%" << std::endl;
	std::cout << "Wrong hits: " << wrongHits << "%" << std::endl;
	std::cout << "Fake hits: " << fakeHits << "%" << std::endl;
	std::cout << "=================================================" << std::endl;
}
// -------------------------------------------------------------------------

ClassImp( CbmTrdMatchTracks)
