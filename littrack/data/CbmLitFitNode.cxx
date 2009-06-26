#include "CbmLitFitNode.h"

#include "TMatrixD.h"

CbmLitFitNode::CbmLitFitNode()
{
	fF.resize(25);
}

CbmLitFitNode::~CbmLitFitNode()
{
}

void CbmLitFitNode::GetF(
		TMatrixD& F) const
{
	const double* a = &fF[0];
	// TODO why TMatrixD cannot call const Use ?!?!
	F.Use(5, 5, (double*)a);
}

void CbmLitFitNode::SetF(
		const std::vector<double>& F)
{
	fF.assign(F.begin(), F.end());
}

void CbmLitFitNode::SetF(
		const TMatrixD& F)
{
	fF.assign(F.GetMatrixArray(), F.GetMatrixArray() + F.GetNoElements());
}
