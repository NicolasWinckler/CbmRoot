#include "CbmLitCleverTrackExtrapolator.h"
#include "CbmLitTrackExtrapolator.h"
#include "CbmLitToolFactory.h"
#include "CbmLitTrackParam.h"
#include "CbmLitEnums.h"
#include "CbmLitMath.h"
#include "CbmLitMatrixMath.h"

CbmLitCleverTrackExtrapolator::CbmLitCleverTrackExtrapolator():
	fZMax(450.),
	fOption(-1)
{
}

CbmLitCleverTrackExtrapolator::~CbmLitCleverTrackExtrapolator()
{
}

LitStatus CbmLitCleverTrackExtrapolator::Initialize()
{
	CbmLitToolFactory* factory = CbmLitToolFactory::Instance();
	fLineExtrapolator = factory->CreateTrackExtrapolator("line");
	fRK4Extrapolator = factory->CreateTrackExtrapolator("rk4");

	return kLITSUCCESS;
}

LitStatus CbmLitCleverTrackExtrapolator::Finalize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitCleverTrackExtrapolator::Extrapolate(
		const CbmLitTrackParam *parIn,
        CbmLitTrackParam *parOut,
        myf zOut)
{
   *parOut = *parIn;
   return Extrapolate(parOut, zOut);
}

LitStatus CbmLitCleverTrackExtrapolator::Extrapolate(
		CbmLitTrackParam *par,
        myf zOut)
{
	myf zIn = par->GetZ();
	fOption = -1;

	if (zIn >= fZMax && zOut >= fZMax) {
		fOption = 0;
		return fLineExtrapolator->Extrapolate(par, zOut);
	} else
	if (zIn < fZMax && zOut < fZMax) {
		fOption = 1;
		return fRK4Extrapolator->Extrapolate(par, zOut);
	} else
	if (zOut > zIn && zIn < fZMax && zOut > fZMax) {
		fOption = 2;
		LitStatus result = fRK4Extrapolator->Extrapolate(par, fZMax);
		if (result == kLITERROR) return result;
		else return fLineExtrapolator->Extrapolate(par, zOut);
	} else
	if (zOut < zIn && zIn > fZMax && zOut < fZMax) {
		fOption = 3;
		LitStatus result = fLineExtrapolator->Extrapolate(par, fZMax);
		if (result == kLITERROR) return result;
		else return fRK4Extrapolator->Extrapolate(par, zOut);
	}

	return kLITSUCCESS;
}

void CbmLitCleverTrackExtrapolator::TransportMatrix(
		std::vector<myf>& F)
{
	if (fOption == 0) {
		fLineExtrapolator->TransportMatrix(F);
		return;
	} else
	if (fOption == 1) {
		fRK4Extrapolator->TransportMatrix(F);
		return;
	} else
	if (fOption == 2) {
		std::vector<myf> F1(25), F2(25);
		fRK4Extrapolator->TransportMatrix(F1);
		fLineExtrapolator->TransportMatrix(F2);
		Mult25(F1, F2, F);
		return;
	} else
	if (fOption == 3) {
		std::vector<myf> F1(25), F2(25);
		fLineExtrapolator->TransportMatrix(F1);
		fRK4Extrapolator->TransportMatrix(F2);
		Mult25(F1, F2, F);
		return;
	}
}
