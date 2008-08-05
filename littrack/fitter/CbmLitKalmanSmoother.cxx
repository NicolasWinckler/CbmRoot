#include "CbmLitKalmanSmoother.h"

#include "CbmLitTrack.h"

#include "TMatrixD.h"
#include "TMatrixDSym.h"

#include <iostream>

CbmLitKalmanSmoother::CbmLitKalmanSmoother()
{
}

CbmLitKalmanSmoother::~CbmLitKalmanSmoother()
{
}

LitStatus CbmLitKalmanSmoother::Initialize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitKalmanSmoother::Finalize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitKalmanSmoother::Fit(
		CbmLitTrack *track,
		Bool_t downstream)
{
	Int_t n = track->GetNofHits();
	std::vector<CbmLitFitNode> nodes = track->GetFitNodes();
	nodes[n-1].SetSmoothedParam(nodes[n-1].GetUpdatedParam());

	// start with the before the last detector plane 
	for (Int_t i = n - 1; i > 0; i--) {	
		Smooth(&nodes[i - 1], &nodes[i]);
	}
	
	track->SetParamFirst(nodes[0].GetSmoothedParam());	
	track->SetFitNodes(nodes);
		
	return kLITSUCCESS;
}

// We are going in the upstream direction
// this Node (k) , prevNode (k+1)
void CbmLitKalmanSmoother::Smooth(
		CbmLitFitNode* thisNode,
		const CbmLitFitNode* prevNode)
{
	TMatrixDSym invPrevPredC(5);
	prevNode->GetPredictedParam()->GetCovMatrix(invPrevPredC);
	invPrevPredC.Invert();
	
	TMatrixD Ft(5, 5);
	prevNode->GetF(Ft);
	Ft.T();
	
	TMatrixDSym thisUpdC(5);
	thisNode->GetUpdatedParam()->GetCovMatrix(thisUpdC);
	
	TMatrixD A(5, 5);
	A = thisUpdC * Ft * invPrevPredC;
	
	TVectorD thisUpdX(5), prevSmoothedX(5), prevPredX(5);
	thisNode->GetUpdatedParam()->GetStateVector(thisUpdX);		
	prevNode->GetSmoothedParam()->GetStateVector(prevSmoothedX);
	prevNode->GetPredictedParam()->GetStateVector(prevPredX);
	
	TVectorD thisSmoothedX(thisUpdX + A * (prevSmoothedX - prevPredX));
	
	TMatrixDSym prevSmoothedC(5), prevPredC(5), Cdiff(5);
	prevNode->GetSmoothedParam()->GetCovMatrix(prevSmoothedC);
	prevNode->GetPredictedParam()->GetCovMatrix(prevPredC);
	Cdiff = prevSmoothedC - prevPredC;
	
	TMatrixDSym thisSmoothedC(5);
	thisSmoothedC = thisUpdC + Cdiff.Similarity(A);
	
	CbmLitTrackParam par;
	
	par.SetStateVector(thisSmoothedX);
	par.SetCovMatrix(thisSmoothedC);
	par.SetZ(thisNode->GetUpdatedParam()->GetZ());
	
	thisNode->SetSmoothedParam(&par);
}

ClassImp(CbmLitKalmanSmoother)
