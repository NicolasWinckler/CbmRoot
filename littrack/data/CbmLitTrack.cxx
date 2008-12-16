#include "CbmLitTrack.h"

#include "CbmLitHit.h"
#include "CbmLitPixelHit.h"
#include "CbmLitStripHit.h"

#include "CbmLitComparators.h"
#include "CbmLitMemoryManagment.h"

#include <algorithm>
#include <sstream>

CbmLitTrack::CbmLitTrack() :
	fQuality(kLITGOOD),
	fChi2(0),
	fNDF(0),
	fPreviousTrackId(0),
	fLastPlaneId(0),
	fPDG(211),
	fNofMissingHits(0)
{
}

CbmLitTrack::~CbmLitTrack()
{
	ClearHits();
}

CbmLitTrack& CbmLitTrack::operator=(
		const CbmLitTrack& track)
{
	for_each(fHits.begin(), fHits.end(), DeleteObject());
	fHits.clear();
	for (Int_t iHit = 0; iHit < track.GetNofHits(); iHit++)
		AddHit(track.GetHit(iHit));
	
	SetParamFirst(track.GetParamFirst());
	SetParamLast(track.GetParamLast());	
	SetQuality(track.GetQuality());
	SetChi2(track.GetChi2());
	SetNDF(track.GetNDF());
	SetPreviousTrackId(track.GetPreviousTrackId());	
	SetLastPlaneId(track.GetLastPlaneId());
	SetPDG(track.GetPDG());
	SetFitNodes(track.GetFitNodes());
	SetNofMissingHits(track.GetNofMissingHits());
	
	return *this;
}

void CbmLitTrack::AddHit(
		const CbmLitHit* hit)
{
	if (hit->GetType() == kLITSTRIPHIT){
		// TODO do we need typecasting here?!?!?
		CbmLitStripHit* newHit = new CbmLitStripHit(*static_cast<const CbmLitStripHit*>(hit));
		fHits.push_back(newHit);
	}else if (hit->GetType() == kLITPIXELHIT){
		CbmLitPixelHit* newHit = new CbmLitPixelHit(*static_cast<const CbmLitPixelHit*>(hit));
		fHits.push_back(newHit);
	}
}

void CbmLitTrack::RemoveHit(
		Int_t index)
{
	delete fHits[index];
	fHits.erase(fHits.begin() + index);
	if (!fFitNodes.empty()) fFitNodes.erase(fFitNodes.begin() + index);
}

void CbmLitTrack::ClearHits() 
{
	std::for_each(fHits.begin(), fHits.end(), DeleteObject());
	fHits.clear();
}

std::string CbmLitTrack::ToString() const
{
	std::stringstream ss;
	ss << "Track: quality=" <<  fQuality << ", chi2=" << fChi2 
		<< ", ndf=" << fNDF << ", previousTrackId=" << fPreviousTrackId 
		<< ", lastPlaneId=" << fLastPlaneId << ", pdg=" << fPDG
		<< ", nofHits=" << fHits.size() << ", nofFitNodes=" << fFitNodes.size() << std::endl;
	return ss.str();
}

void CbmLitTrack::SortHits(
		Bool_t downstream)
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
		if (std::abs(covFirst[i]) > 10000. ||
			std::abs(covLast[i]) > 10000.) return false;	
	}
	return true;
}

Int_t CbmLitTrack::GetNofHits(
		Int_t planeId)
{
	CbmLitHit value;
	value.SetPlaneId(planeId);
	HitPtrIteratorPair bounds = 
			std::equal_range(fHits.begin(), fHits.end(), &value, CompareHitPtrPlaneIdLess());
	return bounds.second - bounds.first;
}

HitPtrIteratorPair CbmLitTrack::GetHits(
		Int_t planeId)
{
	CbmLitHit value;
	value.SetPlaneId(planeId);
	HitPtrIteratorPair bounds = 
			std::equal_range(fHits.begin(), fHits.end(), &value, CompareHitPtrPlaneIdLess());
	return bounds;
}

void CbmLitTrack::GetHitBounds(
		std::vector<HitPtrIteratorPair>& bounds)
{
	bounds.clear();
	Int_t min = fHits.front()->GetPlaneId();
	Int_t max = fHits.back()->GetPlaneId();
	for (Int_t i = min; i <= max; i++) {
		HitPtrIteratorPair b = GetHits(i);
		if(b.first == b.second) continue;
		bounds.push_back(b);
	}
}

ClassImp(CbmLitTrack);
