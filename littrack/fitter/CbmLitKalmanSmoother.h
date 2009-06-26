#ifndef CBMLITKALMANSMOOTHER_H_
#define CBMLITKALMANSMOOTHER_H_

#include "CbmLitTrackFitter.h"

#include <vector>

class CbmLitFitNode;

class CbmLitKalmanSmoother : public CbmLitTrackFitter
{
public:
	CbmLitKalmanSmoother();
	virtual ~CbmLitKalmanSmoother();

	virtual LitStatus Initialize();
	virtual LitStatus Finalize();

	virtual LitStatus Fit(
			CbmLitTrack *track,
			bool downstream = false);
private:

	void Smooth(
			CbmLitFitNode* thisNode,
			const CbmLitFitNode* prevNode);
};

#endif /*CBMLITKALMANSMOOTHER_H_*/
