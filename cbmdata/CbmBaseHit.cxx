/** CbmBaseHit.cxx
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 **/
#include "CbmBaseHit.h"

CbmBaseHit::CbmBaseHit():
    FairMultiLinkedData(),
	fType(kHIT),
	fDetectorId(-1),
	fZ(0.),
	fDz(0.),
	fRefId(-1)
{
}

CbmBaseHit::CbmBaseHit(
		Int_t detectorId,
		Double_t z,
		Double_t dz,
		Int_t refId):
	fType(kHIT),
	fDetectorId(detectorId),
	fZ(z),
	fDz(dz),
	fRefId(refId)
{
}

CbmBaseHit::~CbmBaseHit()
{
}

ClassImp(CbmBaseHit);
