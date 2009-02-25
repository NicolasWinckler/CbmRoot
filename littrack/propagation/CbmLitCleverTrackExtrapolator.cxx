#include "CbmLitCleverTrackExtrapolator.h"
#include "CbmLitTrackExtrapolator.h"
#include "CbmLitToolFactory.h"
#include "CbmLitTrackParam.h"
#include "CbmLitEnums.h"

CbmLitCleverTrackExtrapolator::CbmLitCleverTrackExtrapolator():
	fZMax(450.),
	fOption(-1)
{
}

CbmLitCleverTrackExtrapolator::~CbmLitCleverTrackExtrapolator()
{
//	delete fLineExtrapolator;
//	delete fRK4Extrapolator;
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
        Double_t zOut)
{
   *parOut = *parIn;
   return Extrapolate(parOut, zOut);
}

LitStatus CbmLitCleverTrackExtrapolator::Extrapolate(
		CbmLitTrackParam *par,
        Double_t zOut)
{
	Double_t zIn = par->GetZ();
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
		   std::vector<Double_t>& F)
{
	TMatrixD F1;
	F1.Use(5, 5, &F[0]);
	TransportMatrix(F1);
}

void CbmLitCleverTrackExtrapolator::TransportMatrix(
		   TMatrixD& F)
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
		TMatrixD F1(5,5), F2(5,5);
		fRK4Extrapolator->TransportMatrix(F1);
		fLineExtrapolator->TransportMatrix(F2);
		F = F1 * F2;
		return;
	} else
	if (fOption == 3) {
		TMatrixD F1(5,5), F2(5,5);
		fLineExtrapolator->TransportMatrix(F1);
		fRK4Extrapolator->TransportMatrix(F2);
		F = F1 * F2;
		return;
	}
}

ClassImp(CbmLitCleverTrackExtrapolator);
