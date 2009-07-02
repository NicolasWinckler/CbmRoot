#include "CbmLitFitNode.h"

CbmLitFitNode::CbmLitFitNode()
{
	fF.resize(25);
}

CbmLitFitNode::~CbmLitFitNode()
{
}

void CbmLitFitNode::SetF(
		const std::vector<myf>& F)
{
	fF.assign(F.begin(), F.end());
}
