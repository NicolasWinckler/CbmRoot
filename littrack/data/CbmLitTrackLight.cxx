#include "CbmLitTrackLight.h"

#include <sstream>

CbmLitTrackLight::CbmLitTrackLight():
	fChi2(0),
	fNDF(0)
{
}

CbmLitTrackLight::~CbmLitTrackLight()
{
}

void CbmLitTrackLight::AddHit(
		const CbmLitHit* hit)
{
	fHits.push_back(hit);
}

std::string CbmLitTrackLight::ToString() const
{
	std::stringstream ss;
	ss << "Track: chi2=" << fChi2 << ", ndf=" << fNDF 
		<< ", nofHits=" << fHits.size() << std::endl;
	return ss.str();
}

void CbmLitTrackLight::SortHits(
		Bool_t downstream)
{
	if (downstream) std::sort(fHits.begin(), fHits.end(), CompareHitPtrPlaneIdLess());
	else std::sort(fHits.begin(), fHits.end(), CompareHitPtrPlaneIdMore());
}



ClassImp(CbmLitTrackLight);
