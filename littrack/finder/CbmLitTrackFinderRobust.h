#ifndef CBMLITTRACKFINDERROBUST_H_
#define CBMLITTRACKFINDERROBUST_H_

#include "CbmLitTrackFinderBase.h"

//class CbmLitWeightCalculator;
//class CbmLitEffHitCalculator;

class CbmLitTrackFinderRobust : public CbmLitTrackFinderBase
{
public:
	CbmLitTrackFinderRobust();
	virtual ~CbmLitTrackFinderRobust();
	
	LitStatus DoFind(
			const HitPtrVector& hits,
			const TrackPtrVector& trackSeeds,
			TrackPtrVector& tracks);
	
	virtual LitStatus Initialize();
	virtual LitStatus Finalize();

private:
	
//	CbmLitWeightCalculator* fWeightCalc;
//	CbmLitEffHitCalculator* fEffHitCalc;
	
	void FollowTracks(
			TrackPtrIterator itBegin,
			TrackPtrIterator itEnd);
	
    void FollowTrack(
    		CbmLitTrack *track);
	
	ClassDef(CbmLitTrackFinderRobust, 1);
};

#endif /*CBMLITTRACKFINDERROBUST_H_*/
