#ifndef CBMLITCLEVERTRACKEXTRAPOLATOR_H_
#define CBMLITCLEVERTRACKEXTRAPOLATOR_H_

#include "CbmLitTrackExtrapolator.h"

class CbmLitTrackExtrapolator;
class CbmLitTrackParam;

class CbmLitCleverTrackExtrapolator : public CbmLitTrackExtrapolator
{
public:
	CbmLitCleverTrackExtrapolator();
	virtual ~CbmLitCleverTrackExtrapolator();

	// Derived from CbmTool
	virtual LitStatus Initialize();
	virtual LitStatus Finalize();

	virtual LitStatus Extrapolate(
		   const CbmLitTrackParam *parIn,
		   CbmLitTrackParam *parOut,
		   Double_t zOut);

	virtual LitStatus Extrapolate(
		   CbmLitTrackParam *par,
		   Double_t zOut );

	virtual void TransportMatrix(
		   std::vector<Double_t>& F);
	virtual void TransportMatrix(
		   TMatrixD& F);

private:
	CbmLitTrackExtrapolator* fLineExtrapolator;
	CbmLitTrackExtrapolator* fRK4Extrapolator;

	Double_t fZMax;
	Int_t fOption;

	ClassDef(CbmLitCleverTrackExtrapolator, 1);
};

#endif /* CBMLITCLEVERTRACKEXTRAPOLATOR_H_ */
