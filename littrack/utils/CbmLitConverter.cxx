#include "CbmLitConverter.h"

#include "CbmLitHit.h"
#include "CbmLitPixelHit.h"
#include "CbmLitStripHit.h"
#include "CbmLitTrack.h"
#include "CbmLitTrackParam.h"

#include "CbmTrack.h"
#include "CbmBaseHit.h"
#include "CbmPixelHit.h"
#include "CbmStripHit.h"
#include "FairTrackParam.h"
#include "CbmStsTrack.h"
#include "CbmMuchTrack.h"
#include "CbmTrdTrack.h"
#include "CbmLitEnvironment.h"
#include "CbmGlobalTrack.h"

#include "TClonesArray.h"

#include <iostream>
#include <cmath>
#include <set>

CbmLitConverter::CbmLitConverter()
{

}

CbmLitConverter::~CbmLitConverter()
{

}

void CbmLitConverter::TrackParamToLitTrackParam(
		const FairTrackParam* par,
		CbmLitTrackParam* litPar)
{
	litPar->SetX((myf)par->GetX());
	litPar->SetY((myf)par->GetY());
	litPar->SetZ((myf)par->GetZ());
	litPar->SetTx((myf)par->GetTx());
	litPar->SetTy((myf)par->GetTy());
	litPar->SetQp((myf)par->GetQp());
	double cov[15];
	par->CovMatrix(cov);
	std::vector<myf> covVec(15);
	for (unsigned int i = 0; i < 15; ++i) covVec[i] = (myf) cov[i];
	litPar->SetCovMatrix(covVec);
}

void CbmLitConverter::LitTrackParamToTrackParam(
		const CbmLitTrackParam* litPar,
		FairTrackParam* par)
{
	par->SetX(litPar->GetX());
	par->SetY(litPar->GetY());
	par->SetZ(litPar->GetZ());
	par->SetTx(litPar->GetTx());
	par->SetTy(litPar->GetTy());
	par->SetQp(litPar->GetQp());
	double cov[15];
	std::vector<myf> covVec(litPar->GetCovMatrix());
	for (unsigned int i = 0; i < 15; ++i) cov[i] = (double) covVec[i];
	par->SetCovMatrix(cov);
}

void CbmLitConverter::PixelHitToLitPixelHit(
		const CbmPixelHit* hit,
		Int_t index,
		CbmLitPixelHit* litHit)
{
	litHit->SetX(hit->GetX());
	litHit->SetY(hit->GetY());
	litHit->SetZ(hit->GetZ());
	litHit->SetDx(hit->GetDx());
	litHit->SetDy(hit->GetDy());
	litHit->SetDz(hit->GetDz());
	litHit->SetDxy(hit->GetDxy());
	litHit->SetPlaneId(hit->GetPlaneId() - 1);
	litHit->SetRefId(index);
}

void CbmLitConverter::StripHitToLitStripHit(
		const CbmStripHit* hit,
		Int_t index,
		CbmLitStripHit* litHit)
{
	litHit->SetU(hit->GetU());
	litHit->SetDu(hit->GetDu());
	litHit->SetZ(hit->GetZ());
	litHit->SetDz(hit->GetDz());
	litHit->SetPhi(hit->GetPhi());
	litHit->SetCosPhi(std::cos(hit->GetPhi()));
	litHit->SetSinPhi(std::sin(hit->GetPhi()));
	litHit->SetPlaneId(hit->GetPlaneId() - 1);
	litHit->SetRefId(index);
}

void CbmLitConverter::StsTrackToLitTrack(
		const CbmStsTrack* stsTrack,
		CbmLitTrack* litTrack)
{
	// TODO note that hits are not copied now

	litTrack->SetQuality(kLITGOOD);
	litTrack->SetChi2(stsTrack->GetChi2());
	litTrack->SetNDF(stsTrack->GetNDF());
	litTrack->SetPreviousTrackId(-1);
	CbmLitTrackParam paramFirst, paramLast;
	//TODO remove this const typecasting
	CbmLitConverter::TrackParamToLitTrackParam((const_cast<CbmStsTrack*> (stsTrack))->GetParamFirst(), &paramFirst);
	CbmLitConverter::TrackParamToLitTrackParam((const_cast<CbmStsTrack*> (stsTrack))->GetParamLast(), &paramLast);
	litTrack->SetParamFirst(&paramFirst);
	litTrack->SetParamLast(&paramLast);
}

void CbmLitConverter::TrackToLitTrack(
		const CbmTrack* track,
		CbmLitTrack* litTrack,
		const TClonesArray* pixelHits,
		const TClonesArray* stripHits)
{
	for (int iHit = 0; iHit < track->GetNofHits(); iHit++) {
		Int_t index = track->GetHitIndex(iHit);
		Int_t type = track->GetHitType(iHit);
		if (type == kMUCHSTRAWHIT) {
			CbmStripHit* stripHit = (CbmStripHit*) stripHits->At(index);
			CbmLitStripHit litHit;
			StripHitToLitStripHit(stripHit, index, &litHit);
			litTrack->AddHit(&litHit);
		} else {
			CbmPixelHit* pixelHit = (CbmPixelHit*) pixelHits->At(index);
			CbmLitPixelHit litHit;
			PixelHitToLitPixelHit(pixelHit, index, &litHit);
			litTrack->AddHit(&litHit);
		}
	}

	litTrack->SetQuality(kLITGOOD);
	litTrack->SetChi2(track->GetChiSq());
	litTrack->SetNDF(track->GetNDF());
	litTrack->SetPreviousTrackId(track->GetPreviousTrackId());
	litTrack->SetLastPlaneId(track->GetFlag());
	litTrack->SetPDG(track->GetPidHypo());
	CbmLitTrackParam paramFirst, paramLast;
	CbmLitConverter::TrackParamToLitTrackParam(track->GetParamFirst(), &paramFirst);
	CbmLitConverter::TrackParamToLitTrackParam(track->GetParamLast(), &paramLast);
	litTrack->SetParamFirst(&paramFirst);
	litTrack->SetParamLast(&paramLast);
}

void CbmLitConverter::LitTrackToTrack(
		const CbmLitTrack* litTrack,
		CbmTrack* track)
{
	for (int iHit = 0; iHit < litTrack->GetNofHits(); iHit++) {
		const CbmLitHit* hit = litTrack->GetHit(iHit);
		LitHitType type = hit->GetType();
		LitDetectorId det = hit->GetDetectorId();
		if (det == kLITMUCH && type == kLITPIXELHIT) track->AddHit(hit->GetRefId(), kMUCHPIXELHIT);
		else if (det == kLITMUCH && type == kLITSTRIPHIT) track->AddHit(hit->GetRefId(), kMUCHSTRAWHIT);
		else if (det == kLITTRD) track->AddHit(hit->GetRefId(), kTRDHIT);
	}

	track->SetChiSq(litTrack->GetChi2());
	track->SetNDF(litTrack->GetNDF());
	track->SetPreviousTrackId(litTrack->GetPreviousTrackId());
	track->SetFlag(litTrack->GetQuality());
	FairTrackParam parLast, parFirst;
	CbmLitConverter::LitTrackParamToTrackParam(litTrack->GetParamLast(), &parLast);
	CbmLitConverter::LitTrackParamToTrackParam(litTrack->GetParamFirst(), &parFirst);
	track->SetParamLast(&parLast);
	track->SetParamFirst(&parFirst);
}

//void CbmLitConverter::GlobalTrackArrayToLitTrackVector(
//		const TClonesArray* globalTracks,
//		const TClonesArray* stsTracks,
//		const TClonesArray* trdTracks,
//		const TClonesArray* muchTracks,
//		const TClonesArray* tofHits,
//		const TClonesArray* trdHits,
//		const TClonesArray* muchHits,
//		TrackPtrVector& litTracks)
//{
//	Int_t nofGlobalTracks = globalTracks->GetEntriesFast();
//	for (Int_t iTrack = 0; iTrack < nofGlobalTracks; iTrack++) {
//		CbmGlobalTrack* globalTrack = (CbmGlobalTrack*) globalTracks->At(iTrack);
//
//		CbmLitTrack* litTrack = new CbmLitTrack();
//
//		Int_t stsId = globalTrack->GetStsTrackIndex();
//		Int_t trdId = globalTrack->GetTrdTrackIndex();
//		Int_t muchId = globalTrack->GetRichRingIndex(); //TODO change to MUCH
//		Int_t tofId = globalTrack->GetTofHitIndex();
//
//		// STS: set initial track parameters from STS track
//		if (stsId > -1) {
//			CbmStsTrack* stsTrack = (CbmStsTrack*) stsTracks->At(stsId);
//			CbmLitTrackParam par;
//			CbmLitConverter::TrackParamToLitTrackParam((const_cast<CbmStsTrack*> (stsTrack))->GetParamLast(), &par);
//			litTrack->SetParamLast(&par);
//		}
//		//TRD: attach hits from TRD track
//		if (trdId > -1) {
//			CbmTrdTrack* trdTrack = (CbmTrdTrack*) trdTracks->At(trdId);
//			for (int iHit = 0; iHit < trdTrack->GetNofTrdHits(); iHit++) {
//				Int_t index = trdTrack->GetTrdHitIndex(iHit);
//				CbmTrdHit* hit = (CbmTrdHit*) trdHits->At(index);
//				CbmLitPixelHit litHit;
//				TrkHitToLitPixelHit(hit, index, &litHit);
//				litTrack->AddHit(&litHit);
//			}
//		}
//		//MUCH: attach hits from MUCH track
//		if (muchId > -1) {
//			CbmMuchTrack* muchTrack = (CbmMuchTrack*) muchTracks->At(muchId);
//			for (int iHit = 0; iHit < muchTrack->GetNHits(); iHit++) {
//				Int_t index = muchTrack->GetHitIndex(iHit);
//				CbmMuchHit* hit = (CbmMuchHit*) muchHits->At(index);
//				if (hit->GetTime(2) == -77777) {
//					CbmLitStripHit litHit;
//					MuchHitToLitStripHit(hit, index, &litHit);
//					litTrack->AddHit(&litHit);
//				} else {
//					CbmLitPixelHit litHit;
//					TrkHitToLitPixelHit(hit, index, &litHit);
//					litTrack->AddHit(&litHit);
//				}
//			}
//		}
//		//TOF: attach TOF hit
//		if (tofId > -1) {
//			CbmTofHit* tofHit = (CbmTofHit*) tofHits->At(tofId);
//			CbmLitPixelHit litHit;
//			TofHitToLitPixelHit(tofHit, tofId, &litHit);
//			litTrack->AddHit(&litHit);
//		}
//	}
//}


void CbmLitConverter::LitTrackVectorToGlobalTrackArray(
		const TrackPtrVector& litTracks,
		TClonesArray* globalTracks,
		TClonesArray* stsTracks,
		TClonesArray* trdTracks,
		TClonesArray* muchTracks)
{
	Int_t globalTrackNo = globalTracks->GetEntriesFast();
	Int_t trdTrackNo, muchTrackNo;
	if (trdTracks != NULL) trdTrackNo = trdTracks->GetEntriesFast();
	if (muchTracks != NULL) muchTrackNo = muchTracks->GetEntriesFast();

	std::set<Int_t> stsTracksSet;
	for (Int_t iTrack = 0; iTrack < litTracks.size(); iTrack++) {
		CbmLitTrack* litTrack = litTracks[iTrack];

		CbmGlobalTrack* globalTrack = new ((*globalTracks)[globalTrackNo++]) CbmGlobalTrack();

		Bool_t isCreateMuchTrack = false, isCreateTrdTrack = false;
		for (int iHit = 0; iHit < litTrack->GetNofHits(); iHit++) {
			LitDetectorId detId = litTrack->GetHit(iHit)->GetDetectorId();
			if (detId == kLITMUCH && muchTracks != NULL) isCreateMuchTrack = true;
			if (detId == kLITTRD && trdTracks != NULL) isCreateTrdTrack = true;
		}
		CbmMuchTrack* muchTrack = NULL;
		CbmTrdTrack* trdTrack = NULL;
		if (isCreateMuchTrack) muchTrack = new ((*muchTracks)[muchTrackNo++]) CbmMuchTrack();
		if (isCreateTrdTrack) trdTrack = new ((*trdTracks)[trdTrackNo++]) CbmTrdTrack();

		for (int iHit = 0; iHit < litTrack->GetNofHits(); iHit++) {
			const CbmLitHit* hit = litTrack->GetHit(iHit);
			LitDetectorId detId = hit->GetDetectorId();
			if (detId == kLITTRD && isCreateTrdTrack) {
				trdTrack->AddHit(hit->GetRefId(), kTRDHIT);
			}
			if (detId == kLITMUCH && isCreateMuchTrack) {
				if (hit->GetType() == kLITPIXELHIT)
					muchTrack->AddHit(hit->GetRefId(), kMUCHPIXELHIT);
				else
					muchTrack->AddHit(hit->GetRefId(), kMUCHSTRAWHIT);
			}
			if (detId == kLITTOF) {
				globalTrack->SetTofHitIndex(hit->GetRefId());
			}
		}

		globalTrack->SetStsTrackIndex(litTrack->GetPreviousTrackId());
		stsTracksSet.insert(litTrack->GetPreviousTrackId());
		if (isCreateMuchTrack) {
			globalTrack->SetMuchTrackIndex(muchTrackNo - 1);
			muchTrack->SetPreviousTrackId(litTrack->GetPreviousTrackId());
		}
		if (isCreateTrdTrack) {
			globalTrack->SetTrdTrackIndex(trdTrackNo - 1);
			trdTrack->SetPreviousTrackId(litTrack->GetPreviousTrackId());
		}
	}

	// Loop over STS tracks in order to create additional CbmGlobalTrack,
	// consisting only CbmStsTrack
	for (Int_t iTrack = 0; iTrack < stsTracks->GetEntriesFast(); iTrack++) {
		// if CbmGlobalTrack for STS track was not created, than create one
		if (stsTracksSet.find(iTrack) == stsTracksSet.end()) {
			CbmGlobalTrack* globalTrack = new ((*globalTracks)[globalTrackNo++]) CbmGlobalTrack();
			globalTrack->SetStsTrackIndex(iTrack);
		}
	}
}

void CbmLitConverter::HitArrayToHitVector(
		const TClonesArray* hits,
		HitPtrVector& litHits)
{
	Int_t nofHits = hits->GetEntriesFast();
	for(Int_t iHit = 0; iHit < nofHits; iHit++) {
		CbmBaseHit* hit = (CbmBaseHit*) hits->At(iHit);
	    if(NULL == hit) continue;
	    if (hit->GetType() == kMUCHSTRAWHIT) {
	    	CbmLitStripHit* litHit = new CbmLitStripHit();
	    	CbmStripHit* stripHit = static_cast<CbmStripHit*>(hit);
	    	StripHitToLitStripHit(stripHit, iHit, litHit);
	    	litHit->SetDetectorId(kLITMUCH);
	    	litHits.push_back(litHit);
	    } else {
	    	CbmLitPixelHit* litHit = new CbmLitPixelHit();
	    	CbmPixelHit* pixelHit = static_cast<CbmPixelHit*>(hit);
	    	PixelHitToLitPixelHit(pixelHit, iHit, litHit);
	    	if (hit->GetType() == kMUCHPIXELHIT) litHit->SetDetectorId(kLITMUCH);
	    	if (hit->GetType() == kTRDHIT) litHit->SetDetectorId(kLITTRD);
	    	if (hit->GetType() == kTOFHIT) litHit->SetDetectorId(kLITTOF);
	    	litHits.push_back(litHit);
	    }
	}
}

void CbmLitConverter::StsTrackArrayToTrackVector(
		const TClonesArray* tracks,
		TrackPtrVector& litTracks)
{
    Int_t nofTracks = tracks->GetEntriesFast();
    for(Int_t iTrack = 0; iTrack < nofTracks; iTrack++) {
       CbmStsTrack* track = (CbmStsTrack*) tracks->At(iTrack);
       if (track == NULL) continue;
       if (track->GetParamLast()->GetQp() == 0) continue;
       CbmLitTrack* litTrack = new CbmLitTrack;
       StsTrackToLitTrack(track, litTrack);
       litTrack->SetPreviousTrackId(iTrack);
       litTrack->SetRefId(iTrack);
       litTracks.push_back(litTrack);
    }
}

void CbmLitConverter::TrackVectorToMuchTrackArray(
		TrackPtrVector& tracks,
		TClonesArray* muchTracks)
{
	Int_t trackNo = muchTracks->GetEntriesFast();
	for(TrackPtrIterator iTrack = tracks.begin(); iTrack != tracks.end(); iTrack++) {
		CbmMuchTrack track;
	    LitTrackToTrack(*iTrack, &track);
        new ((*muchTracks)[trackNo++]) CbmMuchTrack(track);
	}
}

void CbmLitConverter::TrackVectorToTrdTrackArray(
		TrackPtrVector& tracks,
		TClonesArray* trdTracks)
{
	Int_t trackNo = trdTracks->GetEntriesFast();
	for(TrackPtrIterator iTrack = tracks.begin();iTrack != tracks.end(); iTrack++) {
	   CbmTrdTrack track;
	   LitTrackToTrack(*iTrack, &track);
       new ((*trdTracks)[trackNo++]) CbmTrdTrack(track);
	}
}
