#ifndef CBMLITMYTRACKPROPAGATOR_H_
#define CBMLITMYTRACKPROPAGATOR_H_

#include "CbmLitTrackPropagator.h"
#include "CbmLitPtrTypes.h"

class CbmLitMyTrackPropagator : public CbmLitTrackPropagator
{
public:
	CbmLitMyTrackPropagator(
			TrackExtrapolatorPtr extrapolator);
	virtual ~CbmLitMyTrackPropagator();

	// derived from CbmTool
	virtual LitStatus Initialize();
	virtual LitStatus Finalize();

	// Propagator
	virtual LitStatus Propagate(
		   const CbmLitTrackParam *parIn,
		   CbmLitTrackParam *parOut,
		   Double_t zOut,
		   Int_t pdg);

	virtual LitStatus Propagate(
		   CbmLitTrackParam *par,
		   Double_t zOut,
		   Int_t pdg);

	virtual void TransportMatrix(
		   std::vector<Double_t>& F);

	virtual void TransportMatrix(
		   TMatrixD& F);

	ClassDef(CbmLitMyTrackPropagator, 1);
};

#endif /* CBMLITMYTRACKPROPAGATOR_H_ */
