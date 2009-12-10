/** LitTrack.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 *
 * Track data class for fast parallel version of the Littrack tracking.
 **/

#ifndef LITTRACK_H_
#define LITTRACK_H_

#include "LitTypes.h"
#include "LitTrackParam.h"
#include "LitHit.h"

const unsigned char MAX_NOF_HITS_IN_TRACK = 30;

class LitTrack {
public:
	LitTrack():
		chiSq(0.),
		NDF(1),
		nofHits(0),
		nofMissingHits(0),
		previouseTrackId(0)
	{
	}

	void AddHit(LitScalPixelHit* hit) {
		hits[nofHits++] = hit;
	}

	LitTrackParam<fscal> paramFirst;
	LitTrackParam<fscal> paramLast;
	fscal chiSq;
	unsigned short NDF;
	LitScalPixelHit* hits[MAX_NOF_HITS_IN_TRACK];
	unsigned short nofHits;
	unsigned short nofMissingHits;
	unsigned short previouseTrackId;

	friend std::ostream & operator<<(std::ostream &strm, const LitTrack &track){
		strm << "LitTrack: " << "nofHits=" << track.nofHits << " chiSq=" << track.chiSq
			<< " NDF=" << track.NDF << " nofMissingHits=" << track.nofMissingHits
			<< " previouseTrackId=" << track.previouseTrackId
			<< " paramFirst=" << track.paramFirst << " paramLast=" << track.paramLast
			<< std::endl;
		return strm;
	}
};


#endif /* LITTRACK_H_ */
