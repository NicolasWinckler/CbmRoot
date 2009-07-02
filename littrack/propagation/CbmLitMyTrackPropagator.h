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
		   myf zOut,
		   int pdg);

	virtual LitStatus Propagate(
		   CbmLitTrackParam *par,
		   myf zOut,
		   int pdg);

	virtual void TransportMatrix(
		   std::vector<myf>& F);

	void UpdateF(
			std::vector<myf>& F,
			const std::vector<myf>& newF);

private:
	GeoNavigatorPtr fNavigator; // Geo Navigator tool
	TrackExtrapolatorPtr fExtrapolator; // Track extrapolator tool
	MaterialEffectsPtr fMaterial; // Material Effects tool

	bool fDownstream; // if true than downstream propagation
	int fPDG; // PDG of the particle

	std::vector<myf> fFm; // Transport matrix
	bool fCalcTransportMatrix; // if true than transport matrix is calculated
};

#endif /* CBMLITMYTRACKPROPAGATOR_H_ */
