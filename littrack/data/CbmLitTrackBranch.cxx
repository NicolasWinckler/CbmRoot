#include "CbmLitTrackBranch.h"

#include "CbmLitHit.h"
#include "CbmLitComparators.h"
#include "CbmLitMemoryManagment.h"

#include <sstream>
#include <algorithm>

CbmLitTrackBranch::CbmLitTrackBranch():
	fChi2(0),
	fNDF(0)
{
}

CbmLitTrackBranch::~CbmLitTrackBranch()
{
}

void CbmLitTrackBranch::AddHit(
		CbmLitHit* hit)
{
	fHits.push_back(hit);
}

std::string CbmLitTrackBranch::ToString() const
{
	std::stringstream ss;
	ss << "TrackCand: chi2=" << fChi2 << ", ndf=" << fNDF
		<< ", nofHits=" << fHits.size() << std::endl;
	return ss.str();
}

void CbmLitTrackBranch::SortHits(
		Bool_t downstream)
{
	if (downstream) std::sort(fHits.begin(), fHits.end(), CompareHitPtrPlaneIdLess());
	else std::sort(fHits.begin(), fHits.end(), CompareHitPtrPlaneIdMore());
}

ClassImp(CbmLitTrackBranch);
