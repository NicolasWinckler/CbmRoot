#include "CbmLitTrackBranch.h"

#include "CbmLitHit.h"
#include "CbmLitComparators.h"

#include <sstream>
#include <algorithm>

CbmLitTrackBranch::CbmLitTrackBranch():
	fChi2(0.),
	fNDF(0),
	fNofMissingHits(0),
	fRefId(-1),
	fQuality(kLITBAD),
	fPreviousTrackId(-1),
	fLastPlaneId(0)
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
	ss << "TrackBranch: chi2=" << fChi2 << ", ndf=" << fNDF
		<< ", nofHits=" << fHits.size() << std::endl;
	return ss.str();
}

void CbmLitTrackBranch::SortHits(
		bool downstream)
{
	if (downstream) std::sort(fHits.begin(), fHits.end(), CompareHitPtrZLess());
	else std::sort(fHits.begin(), fHits.end(), CompareHitPtrZMore());
}
