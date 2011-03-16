#ifndef CBMLITCONVERTER_H_
#define CBMLITCONVERTER_H_

#include "base/CbmLitTypes.h"

class CbmLitPixelHit;
class CbmLitStripHit;
class CbmLitTrack;
class CbmLitTrackParam;
class CbmTrack;
class CbmBaseHit;
class CbmPixelHit;
class CbmStripHit;
class CbmHit;
class FairTrackParam;
class CbmStsTrack;
class CbmGlobalTrack;
class TClonesArray;

class CbmLitConverter
{
public:
	CbmLitConverter();
	virtual ~CbmLitConverter();

	static void TrackParamToLitTrackParam(
			const FairTrackParam* par,
			CbmLitTrackParam* litPar);

	static void LitTrackParamToTrackParam(
			const CbmLitTrackParam* litPar,
			FairTrackParam* par);

	static void PixelHitToLitPixelHit(
			const CbmPixelHit* hit,
			int index,
			CbmLitPixelHit* litHit);

	static void StripHitToLitStripHit(
			const CbmStripHit* hit,
			int index,
			CbmLitStripHit* litHit);

	static void CbmHitToLitPixelHit(
			const CbmHit* hit,
			int index,
			CbmLitPixelHit* litHit);

	static void StsTrackToLitTrack(
			const CbmStsTrack* stsTrack,
			CbmLitTrack* litTrack);

	static void TrackToLitTrack(
			const CbmTrack* track,
			CbmLitTrack* litTrack,
			const TClonesArray* pixelHits,
			const TClonesArray* stripHits);

	static void LitTrackToTrack(
			const CbmLitTrack* litTrack,
			CbmTrack* track);

//	static void GlobalTrackArrayToLitTrackVector(
//			const TClonesArray* globalTracks,
//			const TClonesArray* stsTracks,
//			const TClonesArray* trdTracks,
//			const TClonesArray* muchTracks,
//			const TClonesArray* tofHits,
//			const TClonesArray* trdHits,
//			const TClonesArray* muchHits,
//			TrackPtrVector& litTracks);

	static void LitTrackVectorToGlobalTrackArray(
			const TrackPtrVector& litTracks,
			TClonesArray* globalTracks,
			TClonesArray* stsTracks,
			TClonesArray* trdTracks,
			TClonesArray* muchTracks);

	static void HitArrayToHitVector(
			const TClonesArray* hits,
			HitPtrVector& litHits);

	static void MvdHitArrayToHitVector(
			const TClonesArray* hits,
			HitPtrVector& litHits);

	static void StsTrackArrayToTrackVector(
			const TClonesArray* tracks,
			TrackPtrVector& litTracks);

	static void TrackVectorToMuchTrackArray(
			TrackPtrVector& tracks,
			TClonesArray* muchTracks);

	static void TrackVectorToTrdTrackArray(
			TrackPtrVector& tracks,
			TClonesArray* trdTracks);
};

#endif /*CBMLITCONVERTER_H_*/
