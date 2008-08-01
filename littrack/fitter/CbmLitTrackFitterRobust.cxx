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
#include <iostream>

CbmLitTrackFitterRobust::CbmLitTrackFitterRobust(
		CbmLitTrackFitter* fitter,
		CbmLitTrackFitter* smoother)
{   
	fFitter = fitter;
	fSmoother = smoother;
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

	int nofIterations = 5;

	CbmLitTrack track;
	track.SetParamFirst(pTrack->GetParamFirst());
	std::cout << "---FIT---" << std::endl;
	for(Int_t iter = 0; iter < nofIterations; iter++){
		track.ClearHits();
		std::cout << "---iter: " << iter << std::endl;
		for (Int_t i = 0; i < bounds.size(); i++) {
			// if number of hits on the station more than 1,
			// calculate the effective hit
			if (bounds[i].second - bounds[i].first > 1) {
				if (iter == 0) {
					CbmLitTrackParam par;
					fWeightCalcSimple->DoCalculate(&par, bounds[i].first, bounds[i].second);
				} else {
					const CbmLitTrackParam* param = track.GetFitNode(i)->GetSmoothedParam();
					std::cout << "smoothed: "; param->Print();
					if (DAFWeight(param, bounds[i].first, bounds[i].second, iter) == kLITERROR) {
						std::cout << "ERROR in DAFWeight" << std::endl;
						return kLITERROR;
					}
				}
				if (!AreAllOutliers(bounds[i].first, bounds[i].second)) {
					CbmLitHit ehit = fEffHitCalc->DoCalculate(bounds[i].first, bounds[i].second);
					track.AddHit(&ehit);
					std::cout << "eff hit:"; ehit.Print();
				} else {
					std::cout << "all hit outliers " << std::endl;
				}
				for (HitIterator j = bounds[i].first; j != bounds[i].second; j++)
				(*j)->Print();				
			} else {
				track.AddHit(*bounds[i].first);
			}	
		}
		
		if (fFitter->Fit(&track) == kLITERROR) {
			std::cout << "ERROR fFitter->Fit " << iter << std::endl; 
			return kLITERROR;
		}		
		if (fSmoother->Fit(&track) == kLITERROR) return kLITERROR;
	}
}

Bool_t CbmLitTrackFitterRobust::AreAllOutliers(
		HitIterator itBegin,
		HitIterator itEnd)
{
	for(HitIterator it = itBegin; it != itEnd; it++) {
		if (!(*it)->IsOutlier()) {
			return false;
		}
	}
	return true;
}

void CbmLitTrackFitterRobust::MarkOutliers(
		HitIterator itBegin,
		HitIterator itEnd)
{
	Double_t outlierCut = 1e-20;
	for(HitIterator it = itBegin; it != itEnd; it++) {
		if ((*it)->GetW() < outlierCut) (*it)->IsOutlier(true); 
	}
}

LitStatus CbmLitTrackFitterRobust::DAFWeight(
		const CbmLitTrackParam* par,
		CbmLitHit* hit,
		Double_t beta)
{
	Double_t dim = 2.;
	const Double_t PI = 3.14159265;
	
	Double_t dx = hit->GetX() - par->GetX();
	Double_t dy = hit->GetY() - par->GetY();
	Double_t dxx = hit->GetDx() * hit->GetDx();
	Double_t dyy = hit->GetDy() * hit->GetDy();
	Double_t dxy = hit->GetDxy();
	Double_t det = dxx * dyy - dxy * dxy;
	Double_t s = (dx*dx*dyy - 2*dx*dy*dxy + dy*dy*dxx) / det;
	
	Double_t w = (1./(std::pow(2.*PI, dim/2.) * std::sqrt(beta*det))) *
					std::exp(-s/(2.*beta));
	hit->SetW(w);
	
	return kLITSUCCESS;
}


Double_t CbmLitTrackFitterRobust::DAFCut(
		const CbmLitHit* hit,
		Double_t beta,
		Double_t cutValue)
{
	Double_t dim = 2.;
	const Double_t PI = 3.14159265;

	Double_t dxx = hit->GetDx() * hit->GetDx();
	Double_t dyy = hit->GetDy() * hit->GetDy();
	Double_t dxy = hit->GetDxy();
	Double_t det = dxx * dyy - dxy * dxy;
	
	Double_t cut = (1./(std::pow(2.*PI, dim/2.) * std::sqrt(beta*det))) *
					std::exp(-cutValue/(2.*beta));
	return cut;
}

LitStatus CbmLitTrackFitterRobust::DAFWeight(
		const CbmLitTrackParam* par,
		HitIterator itBegin,
		HitIterator itEnd,
		Int_t iter)
{
	Double_t annealing[] = {81., 9., 4., 1., 1., 1.};
	//Double_t outlierCut = 1e-20;
	
	Double_t beta = annealing[iter-1];
	Double_t cutValue = 3.;
	Double_t sumW = 0.;
	Double_t sumCut = 0.;
	for(HitIterator it = itBegin; it != itEnd; it++) {
		CbmLitHit* hit = *it; 
		if (hit->IsOutlier()) continue;
		DAFWeight(par, hit, beta);
		sumW += hit->GetW();
		//std::cout << "sumW=" << sumW << " W=" << (*it)->GetW() << std::endl;
		//sumCut += DAFCut(hit, beta, cutValue);
	}
	//std::cout << "sumW=" << sumW << " sumCut=" << sumCut << std::endl;
	
	if (sumW == 0.) {
		std::cout << "ERROR sumW=" << sumW << std::endl;
		//return kLITERROR;
	} else {
		for(HitIterator it = itBegin; it != itEnd; it++) {
			if ((*it)->IsOutlier()) continue;
			(*it)->SetW((*it)->GetW() / (sumW + sumCut));
		}
	}
	
	//TODO check if some of the hit were marked as outliers,
	// is it neccessary to recalculate the weights
	MarkOutliers(itBegin,itEnd);
	
	return kLITSUCCESS;
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
	Double_t c = 0.8 - iter*0.1;
	
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
	Double_t t = 4 - iter;
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
		if (sumW == 0.) (*it)->SetW(0.000001);
		else (*it)->SetW((*it)->GetW() / sumW);
		//std::cout << "tukey 4" << std::endl;
		//if (itEnd - itBegin > 1)std::cout << (*it)->GetW() << " ";
	}
	//if (itEnd - itBegin > 1) std::cout << std::endl;
	return kLITSUCCESS;
}

ClassImp(CbmLitTrackFitterRobust)
