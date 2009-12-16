/*
 * CbmMCPointsDraw.cpp
 *
 *  Created on: Apr 17, 2009
 *      Author: stockman
 */

#include "CbmPixelHitSetDraw.h"
#include "CbmPixelHit.h"

#include <iostream>

CbmPixelHitSetDraw::CbmPixelHitSetDraw()
{
	// TODO Auto-generated constructor stub

}

CbmPixelHitSetDraw::~CbmPixelHitSetDraw()
{
	// TODO Auto-generated destructor stub
}

TVector3 CbmPixelHitSetDraw::GetVector(TObject* obj)
{
	CbmPixelHit* p = (CbmPixelHit*)obj;
	if (fVerbose > 1)
		std::cout << "-I- CbmPixelHitSetDraw::GetVector: " << p->GetX() << " " << p->GetY() << " " << p->GetZ() << std::endl;
	return TVector3(p->GetX(), p->GetY(), p->GetZ());
}


ClassImp(CbmPixelHitSetDraw)
