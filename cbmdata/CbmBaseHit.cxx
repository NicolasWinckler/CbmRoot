/**
 * \file CbmBaseHit.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 **/
#include "CbmBaseHit.h"

CbmBaseHit::CbmBaseHit():
    TObject(),
	fType(kHIT),
	fAddress(-1),
	fZ(0.),
	fDz(0.),
	fRefId(-1),
	fLinks(NULL)
{
}

CbmBaseHit::~CbmBaseHit()
{
}

ClassImp(CbmBaseHit);
