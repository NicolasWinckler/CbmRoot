/** CbmLitFindGlobalTracksIdeal.cxx
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 **/

#include "qa/CbmLitFindGlobalTracksIdeal.h"

#include "base/CbmLitEnvironment.h"

#include "CbmTrackMatch.h"
#include "CbmTofHit.h"
#include "CbmTofPoint.h"
#include "CbmGlobalTrack.h"
#include "CbmMCTrack.h"
#include "FairRootManager.h"

#include "TClonesArray.h"

#include <iostream>

CbmLitFindGlobalTracksIdeal::CbmLitFindGlobalTracksIdeal():
	fIsElectronSetup(false),
	fIsSts(false),
	fIsTrd(false),
	fIsMuch(false),
	fIsTof(false),

	fMCTracks(NULL),
	fStsMatches(NULL),
	fMuchMatches(NULL),
	fTrdMatches(NULL),
	fTofMCPoints(NULL),
	fTofHits(NULL),
	fGlobalTracks(NULL),

	fMcStsMap(),
	fMcTrdMap(),
	fMcMuchMap(),
	fMcTofMap(),

	fEventNo(0)
{
}

CbmLitFindGlobalTracksIdeal::~CbmLitFindGlobalTracksIdeal()
{

}

InitStatus CbmLitFindGlobalTracksIdeal::Init()
{
	DetermineSetup();
	ReadDataBranches();

	return kSUCCESS;
}

void CbmLitFindGlobalTracksIdeal::Exec(
		Option_t* opt)
{
	if (fIsSts) FillTrackMap(fMcStsMap, fStsMatches);
	if (fIsTrd) FillTrackMap(fMcTrdMap, fTrdMatches);
	if (fIsMuch) FillTrackMap(fMcMuchMap, fMuchMatches);
	if (fIsTof) FillMapTof();

	CreateGlobalTracks();

	std::cout << "Event: " << fEventNo++ << std::endl;
}

void CbmLitFindGlobalTracksIdeal::Finish()
{
}

void CbmLitFindGlobalTracksIdeal::DetermineSetup()
{
    CbmLitEnvironment* env = CbmLitEnvironment::Instance();
    fIsElectronSetup = env->IsElectronSetup();
	fIsSts = true;
    fIsTrd = env->IsTrd();
    fIsMuch = env->IsMuch();
    fIsTof = env->IsTof();

    if (fIsElectronSetup) std::cout << "-I- CBM electron setup detected" << std::endl;
    else std::cout << "-I- CBM muon setup detected" << std::endl;
    std::cout << "-I- The following detectors were found in the CBM setup:" << std::endl;
    if (fIsTrd) std::cout << "TRD" << std::endl;
    if (fIsMuch) std::cout << "MUCH" << std::endl;
    if (fIsTof) std::cout << "TOF" << std::endl;
}

void CbmLitFindGlobalTracksIdeal::ReadDataBranches()
{
    FairRootManager* ioman = FairRootManager::Instance();
    if (NULL == ioman) Fatal("Init","CbmRootManager is not instantiated");

    fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
    if (NULL == fMCTracks) Fatal("Init","No MCTrack array!");

    //STS data
    if (fIsSts) {
    	fStsMatches = (TClonesArray*) ioman->GetObject("StsTrackMatch");
    	if (NULL == fStsMatches) Fatal("Init","No StsTrackMatch array!");
    }

	//MUCH data
    if (fIsMuch) {
    	fMuchMatches = (TClonesArray*) ioman->GetObject("MuchTrackMatch");
    	if (NULL == fMuchMatches) Fatal("Init","No MuchTrackMatch array!");
    }

    //TRD data
    if (fIsTrd) {
    	fTrdMatches = (TClonesArray*) ioman->GetObject("TrdTrackMatch");
    	if (NULL == fTrdMatches) Fatal("Init","No TrdTrackMatch array!");
    }

    //TOF data
    if (fIsTof) {
		fTofMCPoints = (TClonesArray*) ioman->GetObject("TofPoint");
		if (NULL == fTofMCPoints ) Fatal("Init","No TofPoint array!");
		fTofHits = (TClonesArray*) ioman->GetObject("TofHit");
		if (NULL == fTofHits) Fatal("Init", "No TofHit array!");
    }

	// Create and register CbmGlobalTrack array
	fGlobalTracks = new TClonesArray("CbmGlobalTrack",100);
	ioman->Register("GlobalTrack", "Global", fGlobalTracks, kTRUE);
}

void CbmLitFindGlobalTracksIdeal::FillTrackMap(
		std::map<Int_t, Int_t>& mcMap,
		const TClonesArray* matches)
{
	mcMap.clear();
	Int_t nofTracks = matches->GetEntriesFast();
	for(Int_t iTrack = 0; iTrack < nofTracks; iTrack++) {
		CbmTrackMatch* trackMatch = (CbmTrackMatch*) matches->At(iTrack);
		if (trackMatch == NULL) continue;
		Int_t mcId = trackMatch->GetMCTrackId();
		if(mcId == -1) continue;
		mcMap.insert(std::pair<Int_t, Int_t>(mcId, iTrack));
	}
}

void CbmLitFindGlobalTracksIdeal::FillMapTof()
{
	fMcTofMap.clear();
	Int_t nofTofHits = fTofHits->GetEntriesFast();
	for(Int_t iTofHit = 0; iTofHit < nofTofHits; iTofHit++) {
		CbmTofHit* tofHit = (CbmTofHit*) fTofHits->At(iTofHit);
		if (tofHit == NULL) continue;
		CbmTofPoint* tofPoint = (CbmTofPoint*) fTofMCPoints->At(tofHit->GetRefId());
		if (tofPoint == NULL) continue;
		Int_t mcId = tofPoint->GetTrackID();
		if(mcId == -1) continue;
		fMcTofMap.insert(std::pair<Int_t, Int_t>(mcId, iTofHit));
	}
}

void CbmLitFindGlobalTracksIdeal::CreateGlobalTracks()
{
	fGlobalTracks->Clear();
	Int_t nGlobalTracks = 0;
	Int_t nofMCTracks = fMCTracks->GetEntriesFast();
	for (Int_t iMCTrack = 0; iMCTrack < nofMCTracks; iMCTrack++) {
		CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(iMCTrack);
		if (mcTrack==NULL) continue;
		Int_t stsId = -1, trdId = -1, muchId = -1, tofId = -1;
		if (fIsSts && (fMcStsMap.find(iMCTrack) != fMcStsMap.end())) stsId = fMcStsMap[iMCTrack];
		if (fIsTrd && (fMcTrdMap.find(iMCTrack) != fMcTrdMap.end())) trdId = fMcTrdMap[iMCTrack];
		if (fIsMuch && (fMcMuchMap.find(iMCTrack) != fMcMuchMap.end())) muchId = fMcMuchMap[iMCTrack];
		if (fIsTof && (fMcTofMap.find(iMCTrack) != fMcTofMap.end())) tofId = fMcTofMap[iMCTrack];

		if (stsId == -1 && trdId == -1 && muchId == -1 && tofId == -1) continue;

		CbmGlobalTrack* globalTrack = new ((*fGlobalTracks)[nGlobalTracks++]) CbmGlobalTrack();
		globalTrack->SetStsTrackIndex(stsId);
		globalTrack->SetTrdTrackIndex(trdId);
		globalTrack->SetTofHitIndex(tofId);
		globalTrack->SetMuchTrackIndex(muchId);
	}
}

ClassImp(CbmLitFindGlobalTracksIdeal);
