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
			CbmLitTrack *pTrack);
private:
	
	void Smooth(
			CbmLitFitNode* thisNode,
			const CbmLitFitNode* prevNode);
	
	ClassDef(CbmLitKalmanSmoother,1);
};

#endif /*CBMLITKALMANSMOOTHER_H_*/
