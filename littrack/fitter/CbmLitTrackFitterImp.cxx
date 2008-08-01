#include "CbmLitTrackFitterImp.h"

#include "CbmLitTrackParam.h"
#include "CbmLitTrack.h"
#include "CbmLitHit.h"
#include "CbmLitTrackPropagator.h"
#include "CbmLitTrackUpdate.h"

#include <iostream>

CbmLitTrackFitterImp::CbmLitTrackFitterImp(
		CbmLitTrackPropagator* propagator,
		CbmLitTrackUpdate* update):
	fDownstream(true)
{
	fPropagator = propagator;
	fUpdate = update;
	//Properties().AddProperty("fDownstream", true);
}

CbmLitTrackFitterImp::~CbmLitTrackFitterImp()
{

}

LitStatus CbmLitTrackFitterImp::Initialize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitTrackFitterImp::Finalize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitTrackFitterImp::Fit(
		CbmLitTrack *pTrack)
{
//	Properties().GetProperty("fDownstream", fDownstream);

	pTrack->SortHits(fDownstream);
	pTrack->SetChi2(0.0);
	Int_t nofHits = pTrack->GetNofHits();
	std::vector<CbmLitFitNode> nodes(nofHits);
	CbmLitTrackParam par;
	TMatrixD F(5,5);
	
	if (fDownstream) {
	    pTrack->SetParamLast(pTrack->GetParamFirst());
	    par = *pTrack->GetParamLast();
	} else {
		pTrack->SetParamFirst(pTrack->GetParamLast());
		par = *pTrack->GetParamFirst();
	}
	for (Int_t i = 0; i < nofHits; i++) {                
		const CbmLitHit* pHit = pTrack->GetHit(i);
	    Double_t Ze = pHit->GetZ();
	    if (fPropagator->Propagate(&par, Ze, pTrack->GetPDG()) == kLITERROR) {
	    	std::cout << "ERROR CbmLitTrackFitterImp::Fit propagation failed" << std::endl;
	    	return kLITERROR;
	    }
	    nodes[i].SetPredictedParam(&par);
	    fPropagator->TransportMatrix(F);
	    nodes[i].SetF(F);
	    if (fUpdate->Update(&par, pHit) == kLITERROR) {
	    	std::cout << "ERROR CbmLitTrackFitterImp::Fit track update failed" << std::endl;
	    	return kLITERROR;
	    }
	    nodes[i].SetUpdatedParam(&par);
	    Double_t chi2Hit = CalcChi2(&par, pHit);
	    pTrack->SetChi2(pTrack->GetChi2() + chi2Hit);  
	}

	if (fDownstream) pTrack->SetParamLast(&par);
	else pTrack->SetParamFirst(&par);

	pTrack->SetFitNodes(nodes);
	
	// TODO check NDF
	if (nofHits > 2) pTrack->SetNDF( 2 * nofHits - 5);
	else pTrack->SetNDF(1);
		
	return kLITSUCCESS;
}

Double_t CbmLitTrackFitterImp::CalcChi2(						
		const CbmLitTrackParam *pParam, 						
		const CbmLitHit* pHit)
{
	Double_t dxx = pHit->GetDx() * pHit->GetDx();
	Double_t dxy = pHit->GetDxy();
	Double_t dyy = pHit->GetDy() * pHit->GetDy();
	Double_t C0 = pParam->GetCovariance(0);
	Double_t C5 = pParam->GetCovariance(5);
	Double_t C1 = pParam->GetCovariance(1);
	
	Double_t dx = pHit->GetX() - pParam->GetX();
	Double_t dy = pHit->GetY() - pParam->GetY();

	Double_t norm = -dxx * dyy + dxx * C5 + dyy * C0 - C0 * C5 + dxy * dxy -
						2 * dxy * C1 + C1 * C1;
	if (norm == 0.) norm = 0.0000001;
	
	Double_t chi2 = (-dx * dx * (dyy - C5) - dy * dy * (dxx - C0) + 2 * dx * dy * (dxy - C1)) / norm;

	return chi2;
}


ClassImp(CbmLitTrackFitterImp);
