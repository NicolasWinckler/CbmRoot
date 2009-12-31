#include "CbmMuchMatchTracks.h"

#include "CbmMuchStrawHit.h"
#include "CbmMuchPixelHit.h"
#include "CbmMuchTrack.h"
#include "CbmTrackMatch.h"
#include "CbmMuchDigiMatch.h"
#include "CbmMuchCluster.h"

#include "FairMCPoint.h"
#include "FairRootManager.h"

#include "TClonesArray.h"

#include <iostream>
#include <iomanip>
#include <map>

CbmMuchMatchTracks::CbmMuchMatchTracks()
  : FairTask("CbmMuchMatchTracks"),
	fNofHits(0),
	fNofTrueHits(0),
	fNofWrongHits(0),
	fNofFakeHits(0),
	fNEvents(0),
	fPoints(NULL),
	fPixelHits(NULL),
	fStrawHits(NULL),
	fMatches(NULL),
	fPixelDigiMatches(NULL),
	fStrawDigiMatches(NULL),
	fClusters(NULL)
{
}

CbmMuchMatchTracks::~CbmMuchMatchTracks()
{
}

InitStatus CbmMuchMatchTracks::Init()
{
	FairRootManager* ioman = FairRootManager::Instance();
	if (ioman == NULL) Fatal("CbmMuchMatchTracks::Init", "RootManager not instantised!");

	fPixelHits = (TClonesArray*) ioman->GetObject("MuchPixelHit");
	fStrawHits = (TClonesArray*) ioman->GetObject("MuchStrawHit");
	if (fPixelHits == NULL && fStrawHits == NULL) Fatal("CbmMuchMatchTracks::Init", "No MuchPixelHit && MuchStrawHit array!");

	fTracks = (TClonesArray*) ioman->GetObject("MuchTrack");
	if (fTracks == NULL) Fatal("CbmMuchMatchTracks::Init", "No MuchTrack array!");

	fPoints = (TClonesArray*) ioman->GetObject("MuchPoint");
	if (fPoints == NULL) Fatal("CbmMuchMatchTracks::Init", "No MuchPoint array!");

	fPixelDigiMatches = (TClonesArray*) ioman->GetObject("MuchDigiMatch");
	fStrawDigiMatches = (TClonesArray*) ioman->GetObject("MuchStrawDigiMatch");
	if (fPixelDigiMatches == NULL && fStrawDigiMatches == NULL) Fatal("CbmMuchMatchTracks::Init", "No MuchDigiMatch && MuchStrawDigiMatch array!");

	fClusters = (TClonesArray*) ioman->GetObject("MuchCluster");
	if (fClusters == NULL) Info("CbmMuchMatchTracks::Init", "No cluster array -- simple hit to digi matching will be used");

	fMatches = new TClonesArray("CbmTrackMatch",100);
	ioman->Register("MuchTrackMatch", "MUCH", fMatches, kTRUE);

	return kSUCCESS;
}

void CbmMuchMatchTracks::Exec(
		Option_t* opt)
{
	fMatches->Clear();

	Int_t nofTracks = fTracks->GetEntriesFast();
	for (Int_t iTrack = 0; iTrack < nofTracks; iTrack++) {	// Loop over tracks
		std::map<Int_t, Int_t> matchMap;

		CbmMuchTrack* pTrack = (CbmMuchTrack*) fTracks->At(iTrack);
		if (pTrack == NULL) continue;

		Int_t nofHits = pTrack->GetNofHits();
		for (Int_t iHit = 0; iHit < nofHits; iHit++) { // Loop over hits
			HitType hitType = pTrack->GetHitType(iHit);
			if (hitType == kMUCHPIXELHIT) {
				ExecPixel(matchMap, pTrack->GetHitIndex(iHit));
			} else if (hitType == kMUCHSTRAWHIT){
				ExecStraw(matchMap, pTrack->GetHitIndex(iHit));
			} else {
				TObject::Fatal("CbmMuchMatchTracks::Exec","Hit type not supported!");
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
			nPoints+=it->second;
		}
		
		Int_t nofFake = 0;//matchMap[-1];
		Int_t nofWrong = nPoints - nofTrue - nofFake;
		Int_t nofMcTracks = matchMap.size() - 1;

		new ((*fMatches)[iTrack]) CbmTrackMatch(
				bestMcTrackId, nofTrue, nofWrong, nofFake, nofMcTracks);

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

	fNEvents++;
}

void CbmMuchMatchTracks::Finish()
{
	Double_t trueHits = 100. * Double_t(fNofTrueHits)/Double_t(fNofHits);
	Double_t wrongHits = 100. * Double_t(fNofWrongHits)/Double_t(fNofHits);
	Double_t fakeHits = 100. * Double_t(fNofFakeHits)/Double_t(fNofHits);
	std::cout << "=================================================" << std::endl;
	std::cout << "=====   " << GetName() << ": Run summary " << std::endl;
	std::cout << "True hits: " << trueHits << "%" << std::endl;
	std::cout << "Wrong hits: " << wrongHits << "%" << std::endl;
	std::cout << "Fake hits: " << fakeHits << "%" << std::endl;
	std::cout << "=================================================" << std::endl;
}

void CbmMuchMatchTracks::ExecPixel(
		std::map<Int_t, Int_t> &matchMap,
		Int_t index)
{
	CbmMuchPixelHit* hit = (CbmMuchPixelHit*) fPixelHits->At(index);
	if (hit == NULL) return;
	Int_t clusterId = hit->GetRefId();
	CbmMuchCluster* cluster = (CbmMuchCluster*) fClusters->At(clusterId);
	for (Int_t iDigi = 0; iDigi < cluster->GetNDigis(); iDigi++){
		Int_t digiIndex = cluster->GetDigiIndex(iDigi);
		DigiToTrackMatch(fPixelDigiMatches, digiIndex, matchMap);
	}
}

void CbmMuchMatchTracks::ExecStraw(
		std::map<Int_t, Int_t> &matchMap,
		Int_t index)
{
	CbmMuchStrawHit* hit = (CbmMuchStrawHit*) fStrawHits->At(index);
	if (hit == NULL) return;
	Int_t digiIndex = hit->GetRefId();
	DigiToTrackMatch(fStrawDigiMatches, digiIndex, matchMap);
}

void CbmMuchMatchTracks::DigiToTrackMatch(
		const TClonesArray* digiMatches,
		Int_t digiIndex,
		std::map<Int_t, Int_t> &matchMap)
{
	CbmMuchDigiMatch* digiMatch = (CbmMuchDigiMatch*) digiMatches->At(digiIndex);
	if (digiMatch == NULL) return;
	for (Int_t iPoint = 0; iPoint < digiMatch->GetNPoints(); iPoint++) {
		Int_t pointIndex = digiMatch->GetRefIndex(iPoint);
		if (pointIndex < 0) { // Fake or background hit
			matchMap[-1]++;
			continue;
		}
		FairMCPoint* point = (FairMCPoint*) fPoints->At(pointIndex);
		if (point == NULL) continue;
		matchMap[point->GetTrackID()]++;
	}
}

ClassImp(CbmMuchMatchTracks);
