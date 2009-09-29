/*
 * LitTrack.h
 *
 *  Created on: Sep 24, 2009
 *      Author: andrey
 */

#ifndef LITTRACK_H_
#define LITTRACK_H_

#include "LitTypes.h"
#include "LitTrackParam.h"
#include "LitHit.h"

class LitTrack {
public:
	LitTrack():
		chiSq(0.),
		NDF(1),
		nofMissingHits(0),
		previouseTrackId(0)
	{
	}

	LitScalTrackParam paramFirst;
	LitScalTrackParam paramLast;
	fscal chiSq;
	unsigned short NDF;
	ScalPixelHitVector hits;
	unsigned char nofMissingHits;
	unsigned short previouseTrackId;

	friend std::ostream & operator<<(std::ostream &strm, const LitTrack &track){
		strm << "LitTrack: " << "nofHits=" << track.hits.size() << " chiSq=" << track.chiSq
			<< " NDF=" << track.NDF << " nofMissingHits=" << (int)track.nofMissingHits
			<< " previouseTrackId=" << track.previouseTrackId
			<< " paramFirst=" << track.paramFirst << " paramLast=" << track.paramLast
			<< std::endl;
		return strm;
	}

	friend std::ostream & operator<<(std::ostream &strm, const LitTrack *track){
		strm << "LitTrack: " << "nofHits=" << track->hits.size() << " chiSq=" << track->chiSq
			<< " NDF=" << track->NDF << " nofMissingHits=" << (int)track->nofMissingHits
			<< " previouseTrackId=" << track->previouseTrackId
			<< " paramFirst=" << track->paramFirst << " paramLast=" << track->paramLast
			<< std::endl;
		return strm;
	}
};


#endif /* LITTRACK_H_ */
