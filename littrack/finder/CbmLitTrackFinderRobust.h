#ifndef CBMLITTRACKFINDERROBUST_H_
#define CBMLITTRACKFINDERROBUST_H_

#include "CbmLitTrackFinderImp.h"

//class CbmLitWeightCalculator;
//class CbmLitEffHitCalculator;

class CbmLitTrackFinderRobust : public CbmLitTrackFinderImp
{
public:
	CbmLitTrackFinderRobust();
	virtual ~CbmLitTrackFinderRobust();
	
	LitStatus DoFind(
			const HitVector& hits,
			const TrackVector& trackSeeds,
			TrackVector& tracks);
	
	virtual LitStatus Initialize();
	virtual LitStatus Finalize();

private:
	
//	CbmLitWeightCalculator* fWeightCalc;
//	CbmLitEffHitCalculator* fEffHitCalc;
	
	void FollowTracks(
			TrackIterator itBegin,
			TrackIterator itEnd);
	
    void FollowTrack(
    		CbmLitTrack *track);
	
	ClassDef(CbmLitTrackFinderRobust, 1);
};

#endif /*CBMLITTRACKFINDERROBUST_H_*/
