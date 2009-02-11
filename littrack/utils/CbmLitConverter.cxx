
#include "CbmLitConverter.h"

#include "CbmLitHit.h"
#include "CbmLitPixelHit.h"
#include "CbmLitStripHit.h"
#include "CbmLitTrack.h"
#include "CbmLitTrackParam.h"

#include "CbmTrkHit.h"
#include "CbmMuchHit.h"
#include "FairTrackParam.h"
#include "CbmTrdHit.h"
#include "CbmStsTrack.h"
#include "CbmTrdTrack.h"
#include "CbmMuchTrack.h"
#include "CbmLitEnvironment.h"

#include "TClonesArray.h"

#include <iostream>
#include <cmath>

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
	litPar->SetX(par->GetX());
	litPar->SetY(par->GetY());
	litPar->SetZ(par->GetZ());
	litPar->SetTx(par->GetTx());
	litPar->SetTy(par->GetTy());
	litPar->SetQp(par->GetQp());
	std::vector<Double_t> cov(15);
	par->CovMatrix(&cov[0]);
	litPar->SetCovMatrix(cov);
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
	par->SetCovMatrix((Double_t *)&(litPar->GetCovMatrix()[0]));
}

void CbmLitConverter::TrkHitToLitPixelHit(
		const CbmTrkHit* trkHit, 
		Int_t index,
		CbmLitPixelHit* litHit)
{
	litHit->SetX(trkHit->GetX()); 	
	litHit->SetY(trkHit->GetY());
	litHit->SetZ(trkHit->GetZ());
	litHit->SetDx(trkHit->GetDx());
	litHit->SetDy(trkHit->GetDy());
	litHit->SetDz(trkHit->GetDz());
	litHit->SetDxy(trkHit->GetCovXY());
	litHit->SetPlaneId(trkHit->GetStationNr() - 1);
	litHit->SetRefId(index);
}

void CbmLitConverter::MuchHitToLitStripHit(
		CbmMuchHit* muchHit, 
		Int_t index,
		CbmLitStripHit* litHit)
{
	litHit->SetU(muchHit->GetTime(0));
	litHit->SetDu(muchHit->GetDx());
	litHit->SetZ(muchHit->GetZ());
	litHit->SetDz(muchHit->GetDz());
	litHit->SetPhi(std::asin(muchHit->GetCovXY()));
	litHit->SetCosPhi(std::cos(litHit->GetPhi()));
	litHit->SetSinPhi(muchHit->GetCovXY());
	litHit->SetPlaneId(muchHit->GetStationNr() - 1);
	litHit->SetRefId(index);
	
//	litHit->SetY(trkHit->GetY());
//	litHit->SetZ(trkHit->GetZ());
//	litHit->SetDx(trkHit->GetDx());
//	litHit->SetDy(trkHit->GetDy());
//	litHit->SetDz(trkHit->GetDz());
//	litHit->SetDxy(trkHit->GetCovXY());
//	litHit->SetPlaneId(trkHit->GetStationNr() - 1);
//	litHit->SetRefId(index);
}

void CbmLitConverter::LitHitToTrdHit(
		const CbmLitHit* litHit,
		CbmTrdHit* trdHit)
{
	if (litHit->GetType() == kLITPIXELHIT) {
		const CbmLitPixelHit* hit = static_cast<const CbmLitPixelHit*>(litHit);
		trdHit->SetX(hit->GetX()); 	
		trdHit->SetY(hit->GetY());
		trdHit->SetZ(hit->GetZ());
		trdHit->SetDx(hit->GetDx());
		trdHit->SetDy(hit->GetDy());
		trdHit->SetDz(hit->GetDz());
	} else if (litHit->GetType() == kLITSTRIPHIT) {
		std::cout << "LitHitToTrdHit NOT IMPLEMENTED FOR STRIP" << std::cout;
	}
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

void CbmLitConverter::MuchTrackToLitTrack(
		const CbmMuchTrack* muchTrack,
		CbmLitTrack* litTrack,
		TClonesArray* hits)
{
	for (int iHit = 0; iHit < muchTrack->GetNHits(); iHit++) {
		Int_t index = muchTrack->GetHitIndex(iHit);
		CbmMuchHit* hit = (CbmMuchHit*) hits->At(index);
		if (hit->GetTime(2) == -77777) {
			CbmLitStripHit litHit;
			MuchHitToLitStripHit(hit, index, &litHit);
			litTrack->AddHit(&litHit);
		} else {
			CbmLitPixelHit litHit;
			TrkHitToLitPixelHit(hit, index, &litHit);
			litTrack->AddHit(&litHit);
		}
	}
	
	litTrack->SetQuality(kLITGOOD);
	litTrack->SetChi2(muchTrack->GetChi2());
	litTrack->SetNDF(muchTrack->GetNDF());
	litTrack->SetPreviousTrackId(muchTrack->GetStsTrackID());	
	CbmLitTrackParam paramFirst, paramLast;
	//TODO remove this const typecasting
	CbmLitConverter::TrackParamToLitTrackParam((const_cast<CbmMuchTrack*> (muchTrack))->GetMuchTrack(), &paramFirst);
	CbmLitConverter::TrackParamToLitTrackParam((const_cast<CbmMuchTrack*> (muchTrack))->GetMuchTrack(), &paramLast);
	litTrack->SetParamFirst(&paramFirst);
	litTrack->SetParamLast(&paramLast);
}
void CbmLitConverter::LitTrackToMuchTrack(
		const CbmLitTrack* litTrack,
		CbmMuchTrack* muchTrack)
{
	for (int iHit = 0; iHit < litTrack->GetNofHits(); iHit++) {
		muchTrack->AddHitIndex(litTrack->GetHit(iHit)->GetRefId());
	}
		     
	Int_t nofLayers = CbmLitEnvironment::Instance()->GetMuchLayout().GetNofPlanes();
	muchTrack->SetNMissedHits(nofLayers - litTrack->GetNofHits());
	muchTrack->SetNMissedStations(0);
	muchTrack->SetChi2(litTrack->GetChi2());
	muchTrack->SetNDF(litTrack->GetNDF());
	muchTrack->SetStsTrackID(litTrack->GetPreviousTrackId());
	muchTrack->SetFlag(litTrack->GetLastPlaneId());//litTrack->GetQuality());
	FairTrackParam par;
	CbmLitConverter::LitTrackParamToTrackParam(litTrack->GetParamLast(), &par);
	muchTrack->SetMuchTrack(&par);	
}

void CbmLitConverter::LitTrackToTrdTrack(
		const CbmLitTrack* litTrack,
		CbmTrdTrack* trdTrack)
{   
	for (int iHit = 0; iHit < litTrack->GetNofHits(); iHit++) {
		CbmTrdHit hit;
		CbmLitConverter::LitHitToTrdHit(litTrack->GetHit(iHit), &hit);
		trdTrack->AddHit(litTrack->GetHit(iHit)->GetRefId(), &hit);
	}
	
	trdTrack->SortHits();
	trdTrack->SetFlag(litTrack->GetQuality());
	trdTrack->SetChi2(litTrack->GetChi2());
	trdTrack->SetNDF(litTrack->GetNDF());
	trdTrack->SetStsTrackIndex(litTrack->GetPreviousTrackId());
	
    FairTrackParam parFirst, parLast;
    CbmLitConverter::LitTrackParamToTrackParam(litTrack->GetParamLast(), &parLast);
    CbmLitConverter::LitTrackParamToTrackParam(litTrack->GetParamFirst(), &parFirst); 
	trdTrack->SetParamFirst(parFirst);
	trdTrack->SetParamLast(parLast);
}

void CbmLitConverter::TrkHitArrayToPixelHitVector(
		const TClonesArray* hits,
		HitPtrVector& litHits)
{
	Int_t nofHits = hits->GetEntriesFast();
	for(Int_t iHit = 0; iHit < nofHits; iHit++) {
		CbmTrkHit* hit = (CbmTrkHit*) hits->At(iHit);
	    if(NULL == hit) continue;	      
	    CbmLitPixelHit* litHit = new CbmLitPixelHit;
	    TrkHitToLitPixelHit(hit, iHit, litHit);
	    litHits.push_back(litHit);
	}	
}

void CbmLitConverter::MuchHitArrayToHitVector(
		const TClonesArray* hits,
		HitPtrVector& litHits)
{
	Int_t nofHits = hits->GetEntriesFast();
	for(Int_t iHit = 0; iHit < nofHits; iHit++) {
		CbmMuchHit* hit = (CbmMuchHit*) hits->At(iHit);
	    if(NULL == hit) continue;	
	    if (hit->GetTime(2) == -77777) {
	    	CbmLitStripHit* litHit = new CbmLitStripHit();
	    	MuchHitToLitStripHit(hit, iHit, litHit);
	    	litHits.push_back(litHit);
	    } else {
	    	CbmLitPixelHit* litHit = new CbmLitPixelHit();
	    	TrkHitToLitPixelHit(hit, iHit, litHit);
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
       //if (track->GetParamLast()->GetQp() > 1.) continue;
       CbmLitTrack* litTrack = new CbmLitTrack;
       StsTrackToLitTrack(track, litTrack);
       litTrack->SetPreviousTrackId(iTrack);
       litTracks.push_back(litTrack);
    }
}

void CbmLitConverter::TrackVectorToMuchTrackArray(
		TrackPtrVector& tracks,
		TClonesArray* muchTracks)
{
	Int_t trackNo = muchTracks->GetEntriesFast();
	for(TrackPtrIterator iTrack = tracks.begin(); iTrack != tracks.end(); iTrack++) {
		if (!(*iTrack)->CheckParams()) continue;
	    CbmMuchTrack track;
	    LitTrackToMuchTrack(*iTrack, &track);
        new ((*muchTracks)[trackNo++]) CbmMuchTrack(track);
	}
}

void CbmLitConverter::TrackVectorToTrdTrackArray(
		TrackPtrVector& tracks,
		TClonesArray* muchTracks)
{
	Int_t trackNo = muchTracks->GetEntriesFast();
	for(TrackPtrIterator iTrack = tracks.begin();iTrack != tracks.end(); iTrack++) {
	   CbmTrdTrack track;
	   LitTrackToTrdTrack(*iTrack, &track);
       new ((*muchTracks)[trackNo++]) CbmTrdTrack(track);
	}
}

ClassImp(CbmLitConverter)
