/** CbmLitFindGlobalTracksIdeal.cxx
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 **/

#include "CbmLitFindGlobalTracksIdeal.h"
#include "CbmLitEnvironment.h"

#include "CbmTrackMatch.h"
#include "CbmTofHit.h"
#include "CbmTofPoint.h"
#include "CbmGlobalTrack.h"
#include "CbmMCTrack.h"
#include "FairRootManager.h"

#include "TClonesArray.h"

#include <iostream>

CbmLitFindGlobalTracksIdeal::CbmLitFindGlobalTracksIdeal()
{
	fEventNo = 0;
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
	if (fIsSts) FillMapSts();
	if (fIsTrd) FillMapTrd();
	if (fIsMuch) FillMapMuch();
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
    	fStsMatches = (TClonesArray*) ioman->GetObject("STSTrackMatch");
    	if (NULL == fStsMatches) Fatal("Init","No STSTrackMatch array!");
    }

	//MUCH data
    if (!fIsMuch) {
    	fMuchMatches = (TClonesArray*) ioman->GetObject("MuchTrackMatch");
    	if (NULL == fMuchMatches) Fatal("Init","No MuchTrackMatch array!");
    }

    //TRD data
    if (fIsTrd) {
    	fTrdMatches = (TClonesArray*) ioman->GetObject("TRDTrackMatch");
    	if (NULL == fTrdMatches) Fatal("Init","No TRDTrackMatch array!");
    }

    //TOF data
    if (fIsTof) {
		fTofMCPoints = (TClonesArray*) ioman->GetObject("TOFPoint");
		if (NULL == fTofMCPoints ) Fatal("Init","No TOFPoint array!");
		fTofHits = (TClonesArray*) ioman->GetObject("TofHit");
		if (NULL == fTofHits) Fatal("Init", "No TofHit array!");
    }

	// Create and register CbmGlobalTrack array
	fGlobalTracks = new TClonesArray("CbmGlobalTrack",100);
	ioman->Register("GlobalTrack", "Global", fGlobalTracks, kTRUE);
}

void CbmLitFindGlobalTracksIdeal::FillMapSts()
{
	fMcStsMap.clear();
	Int_t nofStsTracks = fStsMatches->GetEntriesFast();
	for(Int_t iStsTrack = 0; iStsTrack < nofStsTracks; iStsTrack++) {
		CbmTrackMatch* stsTrackMatch = (CbmTrackMatch*) fStsMatches->At(iStsTrack);
		if (stsTrackMatch == NULL) continue;
		Int_t mcId = stsTrackMatch->GetMCTrackId();
		if(mcId == -1) continue;
		fMcStsMap.insert(std::pair<Int_t, Int_t>(mcId, iStsTrack));
	}
}

void CbmLitFindGlobalTracksIdeal::FillMapTrd()
{
	fMcTrdMap.clear();
	Int_t nofTrdTracks = fTrdMatches->GetEntriesFast();
	for(Int_t iTrdTrack = 0; iTrdTrack < nofTrdTracks; iTrdTrack++) {
		CbmTrackMatch* trdTrackMatch = (CbmTrackMatch*) fTrdMatches->At(iTrdTrack);
		if (trdTrackMatch == NULL) continue;
		Int_t mcId = trdTrackMatch->GetMCTrackId();
		if(mcId == -1) continue;
		fMcTrdMap.insert(std::pair<Int_t, Int_t>(mcId, iTrdTrack));
	}
}

void CbmLitFindGlobalTracksIdeal::FillMapMuch()
{
	fMcMuchMap.clear();
	Int_t nofMuchTracks = fMuchMatches->GetEntriesFast();
	for(Int_t iMuchTrack = 0; iMuchTrack < nofMuchTracks; iMuchTrack++) {
		CbmTrackMatch* muchTrackMatch = (CbmTrackMatch*) fMuchMatches->At(iMuchTrack);
		if (muchTrackMatch == NULL) continue;
		Int_t mcId = muchTrackMatch->GetMCTrackId();
		if(mcId == -1) continue;
		fMcMuchMap.insert(std::pair<Int_t, Int_t>(mcId, iMuchTrack));
	}
}

void CbmLitFindGlobalTracksIdeal::FillMapTof()
{
	fMcTofMap.clear();
	Int_t nofTofHits = fTofHits->GetEntriesFast();
	for(Int_t iTofHit = 0; iTofHit < nofTofHits; iTofHit++) {
		CbmTofHit* tofHit = (CbmTofHit*) fTofHits->At(iTofHit);
		if (tofHit == NULL) continue;
		CbmTofPoint* tofPoint = (CbmTofPoint*) fTofMCPoints->At(tofHit->GetRefIndex());
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
		globalTrack->SetRichRingIndex(muchId); //TODO change to MUCH
	}
}

ClassImp(CbmLitFindGlobalTracksIdeal);
