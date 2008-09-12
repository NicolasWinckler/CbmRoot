#include "CbmLitTrackFitterImp.h"

#include "CbmLitTrackParam.h"
#include "CbmLitTrack.h"
#include "CbmLitHit.h"
#include "CbmLitTrackPropagator.h"
#include "CbmLitTrackUpdate.h"
#include "CbmLitMath.h"

#include <iostream>

CbmLitTrackFitterImp::CbmLitTrackFitterImp(
		CbmLitTrackPropagator* propagator,
		CbmLitTrackUpdate* update):
	fDownstream(true)
{
	fPropagator = propagator;
	fUpdate = update;
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
		CbmLitTrack *track,
		Bool_t downstream)
{
	fDownstream = downstream;
	track->SortHits(fDownstream);
	track->SetChi2(0.0);
	Int_t nofHits = track->GetNofHits();
	std::vector<CbmLitFitNode> nodes(nofHits);
	CbmLitTrackParam par;
	TMatrixD F(5,5);
	
	if (fDownstream) {
	    track->SetParamLast(track->GetParamFirst());
	    par = *track->GetParamLast();
	} else {
		track->SetParamFirst(track->GetParamLast());
		par = *track->GetParamFirst();
	}
	for (Int_t i = 0; i < nofHits; i++) {                
		const CbmLitHit* hit = track->GetHit(i);
	    Double_t Ze = hit->GetZ();
	    if (fPropagator->Propagate(&par, Ze, track->GetPDG()) == kLITERROR) {
	    	//std::cout << "ERROR CbmLitTrackFitterImp::Fit propagation failed" << std::endl;
	    	return kLITERROR;
	    }
	    nodes[i].SetPredictedParam(&par);
	    fPropagator->TransportMatrix(F);
	    nodes[i].SetF(F);
	    if (fUpdate->Update(&par, hit) == kLITERROR) {
	    	//std::cout << "ERROR CbmLitTrackFitterImp::Fit track update failed" << std::endl;
	    	return kLITERROR;
	    }
	    nodes[i].SetUpdatedParam(&par);
	    Double_t chi2Hit = ChiSq(&par, hit);
	    nodes[i].SetChiSqFiltered(chi2Hit);
	    track->SetChi2(track->GetChi2() + chi2Hit);  
	}

	if (fDownstream) track->SetParamLast(&par);
	else track->SetParamFirst(&par);

	track->SetFitNodes(nodes);
	track->SetNDF(NDF(nofHits));
		
	return kLITSUCCESS;
}

ClassImp(CbmLitTrackFitterImp);
