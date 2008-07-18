
#include "CbmLitTrack.h"

#include "CbmLitComparators.h"
#include "CbmLitMemoryManagment.h"

#include "TMath.h"

#include <algorithm>

CbmLitTrack::CbmLitTrack() :
	fFlag(0),
	fChi2(0),
	fNDF(0),
	fPreviousTrackId(0),
	fLastPlaneId(0)
{

}

CbmLitTrack::~CbmLitTrack()
{
	ClearHits();
}

void CbmLitTrack::SortHits(Bool_t downstream)
{
	if (downstream) std::sort(fHits.begin(), fHits.end(), CompareHitPtrPlaneIdLess());
	else std::sort(fHits.begin(), fHits.end(), CompareHitPtrPlaneIdMore());
}

Bool_t CbmLitTrack::CheckParams() const
{
	//TODO improve parameters check for the track

	std::vector<Double_t> covFirst(fParamFirst.GetCovMatrix());
	std::vector<Double_t> covLast(fParamLast.GetCovMatrix());
	for (int i = 0; i < 15; i++) {
		if (TMath::Abs(covFirst[i]) > 10000. ||
			TMath::Abs(covLast[i]) > 10000.) return false;	
	}

	return true;
}

HitIteratorPair CbmLitTrack::GetHits(Int_t planeId)
{
	CbmLitHit value;
	value.SetPlaneId(planeId);
	HitIteratorPair bounds = 
			std::equal_range(fHits.begin(), fHits.end(), &value, CompareHitPtrPlaneIdLess());
	return bounds;
}

std::vector<HitIteratorPair> CbmLitTrack::GetHitBounds()
{
	std::vector<HitIteratorPair> bounds;
	Int_t min = fHits.front()->GetPlaneId();
	Int_t max = fHits.back()->GetPlaneId();
	for (Int_t i = min; i <= max; i++) {
		HitIteratorPair b = GetHits(i);
		if(b.first == b.second) continue;
		bounds.push_back(b);
	}
	return bounds;
}

CbmLitTrack& CbmLitTrack::operator=(const CbmLitTrack& track)
{
	for_each(fHits.begin(), fHits.end(), DeleteObject());
	fHits.clear();
	for (Int_t iHit = 0; iHit < track.GetNofHits(); iHit++)
		AddHit(track.GetHit(iHit));
	
	SetParamFirst(track.GetParamFirst());
	SetParamLast(track.GetParamLast());	
	SetFlag(track.GetFlag());
	SetChi2(track.GetChi2());
	SetNDF(track.GetNDF());
	SetPreviousTrackId(track.GetPreviousTrackId());	
	SetLastPlaneId(track.GetLastPlaneId());
	
	return *this;
}

ClassImp(CbmLitTrack);
