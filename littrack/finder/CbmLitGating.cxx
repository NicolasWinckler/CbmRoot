#include "CbmLitGating.h"
#include "CbmLitMath.h"
#include "CbmLitTrackParam.h"
#include "CbmLitHit.h"
#include "CbmLitHitData.h"
#include "CbmLitDetectorLayout.h"
#include "CbmLitStation.h"
#include "CbmLitPixelHit.h"
#include "CbmLitComparators.h"

#include <algorithm>
#include <cmath>
#include <utility>

CbmLitGating::CbmLitGating()
{
	fSigmaCoef = 7.0;
	fMaxCovSq = 400.;
	fChiSqStripHitCut = 10.;
	fChiSqPixelHitCut = 25.;
	fUseFastSearch = false;//true;
}

CbmLitGating::~CbmLitGating()
{
}

LitStatus CbmLitGating::Initialize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitGating::Finalize()
{
	return kLITSUCCESS;
}

bool CbmLitGating::IsHitInValidationGate(
		const CbmLitTrackParam* par,
		const CbmLitHit* hit) const
{
	double chiSq = ChiSq(par, hit);
	if (hit->GetType() == kLITSTRIPHIT) return chiSq < fChiSqStripHitCut;
	if (hit->GetType() == kLITPIXELHIT) return chiSq < fChiSqPixelHitCut;
	return false;
}


//HitPtrIteratorPair CbmLitGating::MinMaxIndex(
//		const CbmLitTrackParam* par,
//		HitPtrVector& hits,
//		const CbmLitStation& station,
//		const std::pair<double, Char_t>& maxErr) const
//{
//	HitPtrIteratorPair bounds;
//	if (station.GetType() == kLITSTRIPHIT || station.GetType() == kLITMIXHIT || !fUseFastSearch) {
//		bounds = HitPtrIteratorPair(hits.begin(), hits.end());
//	} else
//	if (station.GetType() == kLITPIXELHIT) {
//		CbmLitPixelHit hit;
//		double C0 = par->GetCovariance(0);
//		if(C0 > fMaxCovSq || C0 < 0.) return bounds;
//		double devX = fSigmaCoef * (std::sqrt(C0) + maxErr.first);
//	    hit.SetX(par->GetX() - devX);
//	    bounds.first = std::lower_bound(hits.begin(), hits.end(), &hit, CompareHitPtrXULess());
//	    hit.SetX(par->GetX() + devX);
//	    bounds.second =	std::lower_bound(hits.begin(), hits.end(), &hit, CompareHitPtrXULess());
//	}
//    return bounds;
//}

HitPtrIteratorPair CbmLitGating::MinMaxIndex(
		const CbmLitTrackParam* par,
		CbmLitHitData& hitData, //TODO : investigate const here
		const CbmLitDetectorLayout& layout,
		int stationGroup,
		int station,
		int substation) const
{
	HitPtrIteratorPair bounds;
	CbmLitStation st = layout.GetStationGroup(stationGroup).GetStation(station);
	if (st.GetType() == kLITSTRIPHIT || st.GetType() == kLITMIXHIT || !fUseFastSearch) {
		bounds = hitData.GetHits(stationGroup, station, substation);
	} else if (st.GetType() == kLITPIXELHIT){
		CbmLitPixelHit hit;
		HitPtrIteratorPair hits = hitData.GetHits(stationGroup, station, substation);
		double C0 = par->GetCovariance(0);
		if(C0 > fMaxCovSq || C0 < 0.) return bounds;
		double devX = fSigmaCoef * (std::sqrt(C0) + hitData.GetMaxErr(stationGroup, station, substation).first);
//		double devX = fSigmaCoef * (std::sqrt(C0) + maxErr.first);
//		double devX = CalcDevX(par, hitData, stationGroup, station, substation);
//		if (devX == 0.) return bounds;
	    hit.SetX(par->GetX() - devX);
	    bounds.first = std::lower_bound(hits.first, hits.second, &hit, CompareHitPtrXULess());
	    hit.SetX(par->GetX() + devX);
	    bounds.second =	std::lower_bound(hits.first, hits.second, &hit, CompareHitPtrXULess());
	}
    return bounds;
}

//double CbmLitGating::CalcDevX(
//		const CbmLitTrackParam* par,
//		const CbmLitHitData& hitData,
//		int stationGroup,
//		int station,
//		int substation) const
//{
//	double C0 = par->GetCovariance(0);
//	if(C0 > fMaxCovSq || C0 < 0.) return 0.;
//	return fSigmaCoef * (std::sqrt(C0) + hitData.GetMaxErr(stationGroup, station, substation).first);
//}


//bool CbmLitGating::IsHitInValidationGate(
//		const CbmLitTrackParam* par,
//		const CbmLitHit* hit) const
//{
//    double C0 = par->GetCovariance(0);
//    double C5 = par->GetCovariance(5);
//
//	if(C0 > fMaxCovSq || C5 > fMaxCovSq) return kFALSE;
//	if(C0 < 0. || C5 < 0.) return kFALSE;
//
//	return ChiSq(par, hit) < fChiSqPixelHitCut;
//
//	if (hit->GetType() == kLITSTRIPHIT) {
//		const CbmLitStripHit* stripHit = static_cast<const CbmLitStripHit*>(hit);
//		return ChiSq(par, stripHit) < fChiSqStripHitCut;
//	} else if (hit->GetType() == kLITPIXELHIT) {
//		const CbmLitPixelHit* pixelHit = static_cast<const CbmLitPixelHit*>(hit);
//		return ChiSq(par, pixelHit) < fChiSqPixelHitCut;
//	}

//	else if (hit->GetType() == kLITPIXELHIT) {
//		const CbmLitPixelHit* pixelHit = static_cast<const CbmLitPixelHit*>(hit);
//		double x1 = par->GetX();
//	   	double x2 = pixelHit->GetX();
//	    double dx2 = pixelHit->GetDx();
//	   	double y1 = par->GetY();
//	   	double y2 = pixelHit->GetY();
//	   	double dy2 = pixelHit->GetDy();
////	   	double devX = fSigmaCoef * std::sqrt(C0 + dx2 * dx2);
////		double devY = fSigmaCoef * std::sqrt(C5 + dy2 * dy2);
//	    double devX = fSigmaCoef * (std::sqrt(C0) + dx2);
//	    double devY = fSigmaCoef * (std::sqrt(C5) + dy2);
//
//	    return ( ( (x1 + devX) >= x2 ) &&
//	             ( (x1 - devX) <= x2 ) &&
//	             ( (y1 + devY) >= y2 ) &&
//		         ( (y1 - devY) <= y2 ) );
//	}
//}

