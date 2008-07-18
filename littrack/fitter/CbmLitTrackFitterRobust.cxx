#include "CbmLitTrackFitterRobust.h"

#include "CbmLitHit.h"
#include "CbmLitTrack.h"
#include "CbmLitTrackParam.h"
#include "CbmLitWeightCalculator.h"
#include "CbmLitEffHitCalculator.h"
#include "CbmLitComparators.h"
#include "CbmLitWeightCalculatorSimple.h"
#include "CbmLitEffHitCalculatorImp.h"

#include <vector>
#include <cmath> 

CbmLitTrackFitterRobust::CbmLitTrackFitterRobust(
		CbmLitTrackFitter* fitter)
{   
	fFitter = fitter;
	fWeightCalc = new CbmLitWeightCalculatorSimple;
	fWeightCalcSimple = new CbmLitWeightCalculatorSimple;
	fEffHitCalc = new CbmLitEffHitCalculatorImp;
}

CbmLitTrackFitterRobust::~CbmLitTrackFitterRobust()
{
	delete fWeightCalc;
	delete fWeightCalcSimple;
	delete fEffHitCalc;
}

LitStatus CbmLitTrackFitterRobust::Initialize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitTrackFitterRobust::Finalize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitTrackFitterRobust::Fit(
		CbmLitTrack *pTrack)
{
	pTrack->SortHits();
	std::vector<HitIteratorPair> bounds = pTrack->GetHitBounds();

	int nofIterations = 10;

	CbmLitTrack track;
	track.SetParamFirst(pTrack->GetParamFirst());
	std::vector<CbmLitTrackParam> params;
	
	for(int iter = 0; iter < nofIterations; iter++){
		track.ClearHits();
		for (Int_t i = 0; i < bounds.size(); i++) {
			// if number of hits on the station more than 1,
			// calculate the effective hit
			if (bounds[i].second - bounds[i].first > 1) {
				if (iter == 0) {
					CbmLitTrackParam par;
					fWeightCalcSimple->DoCalculate(&par, bounds[i].first, bounds[i].second);
				} else {
					TukeyWeight(&params[i], bounds[i].first, bounds[i].second, iter);
				}
				CbmLitHit ehit = fEffHitCalc->DoCalculate(bounds[i].first, bounds[i].second);
				track.AddHit(&ehit);
			} else {
				track.AddHit(*bounds[i].first);
			}			
		}
		//fFitter->Properties().SetProperty("fDownstream", true);
		fFitter->Fit(&track);
		//fFitter->Properties().SetProperty("fDownstream", false);
		//fFitter->Fit(&track);
		
		params = track.GetParams();
		//std::sort(params.begin(), params.end(), CompareTrackParamZLess());
	}
}

LitStatus CbmLitTrackFitterRobust::KoshiWeight(
		const CbmLitTrackParam* par,
		HitIterator itBegin,
		HitIterator itEnd,
		Int_t iter)
{
	Double_t a = 5 - 0.5*iter;
	Double_t b = 0.16;
	//Double_t T = 10;
	//Double_t t = 9 * iter;
	
	Double_t tT = 7./Double_t(iter); 
	
	Double_t w = b + (a - b) * std::exp(-tT);
	
	for(HitIterator it = itBegin; it != itEnd; it++) {
		Double_t dx = (*it)->GetX() - par->GetX();
		Double_t dy = (*it)->GetY() - par->GetY();
		Double_t ee = dx * dx + dy * dy;
		Double_t weight = (w * w) / (ee + w * w);
		(*it)->SetW(weight);
		//std::cout << ee << " ee ";
	}
	//std::cout << std::endl;
	Double_t sum = 0;
	for(HitIterator it = itBegin; it != itEnd; it++) 
		sum += (*it)->GetW();
	
	for(HitIterator it = itBegin; it != itEnd; it++) {
		(*it)->SetW((*it)->GetW() / sum);
		if (itEnd - itBegin > 1)std::cout << (*it)->GetW() << " ";
	}
	if (itEnd - itBegin > 1) std::cout << std::endl;
	return kLITSUCCESS;
}

LitStatus CbmLitTrackFitterRobust::OptimalWeight(
		const CbmLitTrackParam* par,
		HitIterator itBegin,
		HitIterator itEnd,
		Int_t iter)
{
	Double_t c = 0.8;
	
	Double_t sumWEE = 0;
	Double_t sumW = 0;
	for(HitIterator it = itBegin; it != itEnd; it++) {
		Double_t dx = (*it)->GetX() - par->GetX();
		Double_t dy = (*it)->GetY() - par->GetY();
		Double_t ee = dx * dx + dy * dy;
		sumWEE += (*it)->GetW() * ee;
		sumW += (*it)->GetW();
	}
	Double_t sigmaSq = sumWEE / sumW;	
	
	sumW = 0;
	for(HitIterator it = itBegin; it != itEnd; it++) {
		Double_t dx = (*it)->GetX() - par->GetX();
		Double_t dy = (*it)->GetY() - par->GetY();
		Double_t ee = dx * dx + dy * dy;
		Double_t w = (1 + c) / (1 + c * std::exp(ee / (2 * sigmaSq)));
		(*it)->SetW(w);
		sumW+=w;
	}
	for(HitIterator it = itBegin; it != itEnd; it++) {
		(*it)->SetW((*it)->GetW() / sumW);
		if (itEnd - itBegin > 1)std::cout << (*it)->GetW() << " ";
	}
	if (itEnd - itBegin > 1) std::cout << std::endl;
	return kLITSUCCESS;
}

LitStatus CbmLitTrackFitterRobust::TukeyWeight(
		const CbmLitTrackParam* par,
		HitIterator itBegin,
		HitIterator itEnd,
		Int_t iter)
{
	Double_t t = 9 - iter;
	Double_t ct = 2 + t;
	Double_t ctSq = ct * ct;
	
	Double_t sumWEE = 0;
	Double_t sumW = 0;
	for(HitIterator it = itBegin; it != itEnd; it++) {
		Double_t dx = (*it)->GetX() - par->GetX();
		Double_t dy = (*it)->GetY() - par->GetY();
		Double_t ee = dx * dx + dy * dy;
		sumWEE += (*it)->GetW() * ee;
		sumW += (*it)->GetW();
	}
	//std::cout << "sumW = " << sumW << std::endl;
	Double_t sigmaSq = sumWEE / sumW;	
	
	sumW = 0.;
	for(HitIterator it = itBegin; it != itEnd; it++) {
		Double_t dx = (*it)->GetX() - par->GetX();
		Double_t dy = (*it)->GetY() - par->GetY();
		Double_t ee = dx * dx + dy * dy;
		Double_t w = 0;
		if (ctSq * sigmaSq > ee) {
			Double_t t = 1 - ee / (ctSq * sigmaSq);
			w = t * t;
		}
		
		(*it)->SetW(w);
		sumW+=w;
	}
	for(HitIterator it = itBegin; it != itEnd; it++) {
		//std::cout << "tukey 3" << " sumW = " << sumW << " GetW=" << (*it)->GetW()<< std::endl;	
		if (sumW == 0.) (*it)->SetW(0.000001);
		else (*it)->SetW((*it)->GetW() / sumW);
		//std::cout << "tukey 4" << std::endl;
		//if (itEnd - itBegin > 1)std::cout << (*it)->GetW() << " ";
	}
	//if (itEnd - itBegin > 1) std::cout << std::endl;
	return kLITSUCCESS;
}

ClassImp(CbmLitTrackFitterRobust)
