

#include "CbmLitTrackParam.h"

#include <iostream>

CbmLitTrackParam::CbmLitTrackParam():
	fX(0.),
	fY(0.),
	fZ(0.),
	fTx(0.),
	fTy(0.),
	fQp(0.)
{
	fCovMatrix.resize(15);
}

CbmLitTrackParam::~CbmLitTrackParam()
{
	
}

void CbmLitTrackParam::Print() const
{
	std::cout << "TrackParam: pos=(" << fX << "," << fY << "," << fZ 
    	      << ") tx=" << fTx << " ty=" << fTy << " qp=" << fQp << std::endl;    	
//	std::cout << "cov: ";
//	for (Int_t i = 0; i < 15; i++) std::cout << fCovMatrix[i] << " ";
//	std::cout << std::endl;
}

ClassImp(CbmLitTrackParam)
