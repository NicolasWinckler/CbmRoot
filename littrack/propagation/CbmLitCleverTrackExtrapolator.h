#ifndef CBMLITCLEVERTRACKEXTRAPOLATOR_H_
#define CBMLITCLEVERTRACKEXTRAPOLATOR_H_

#include "CbmLitTrackExtrapolator.h"
#include "CbmLitPtrTypes.h"

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
		   myf zOut);

	virtual LitStatus Extrapolate(
		   CbmLitTrackParam *par,
		   myf zOut );

	virtual void TransportMatrix(
		   std::vector<myf>& F);

private:
	TrackExtrapolatorPtr fLineExtrapolator;
	TrackExtrapolatorPtr fRK4Extrapolator;

	myf fZMax;
	int fOption;
};

#endif /* CBMLITCLEVERTRACKEXTRAPOLATOR_H_ */
