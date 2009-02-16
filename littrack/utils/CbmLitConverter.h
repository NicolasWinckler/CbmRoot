#ifndef CBMLITCONVERTER_H_
#define CBMLITCONVERTER_H_

#include "CbmLitTypes.h"

class CbmLitPixelHit;
class CbmLitStripHit;
class CbmLitTrack;
class CbmLitTrackParam;
class CbmTrkHit;
class FairTrackParam;
class CbmTrdHit;
class CbmMuchTrack;
class CbmMuchHit;
class CbmTrdTrack;
class CbmStsTrack;
class TClonesArray;

#include "TObject.h"

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

	static void TrkHitToLitPixelHit(
			const CbmTrkHit* trkHit,
			Int_t index,
			CbmLitPixelHit* litHit);

	static void MuchHitToLitStripHit(
			CbmMuchHit* muchHit,
			Int_t index,
			CbmLitStripHit* litHit);

	static void LitHitToTrdHit(
			const CbmLitHit* litHit,
			CbmTrdHit* trdHit);

	static void StsTrackToLitTrack(
			const CbmStsTrack* stsTrack,
			CbmLitTrack* litTrack);

	static void MuchTrackToLitTrack(
			const CbmMuchTrack* muchTrack,
			CbmLitTrack* litTrack,
			TClonesArray* hits);

	static void LitTrackToMuchTrack(
			const CbmLitTrack* litTrack,
			CbmMuchTrack* muchTrack);

	static void TrdTrackToLitTrack(
			const CbmTrdTrack* trdTrack,
			CbmLitTrack* litTrack,
			TClonesArray* hits);

	static void LitTrackToTrdTrack(
			const CbmLitTrack* litTrack,
			CbmTrdTrack* trdTrack);

	static void TrkHitArrayToPixelHitVector(
			const TClonesArray* hits,
			HitPtrVector& litHits);

	static void MuchHitArrayToHitVector(
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
			TClonesArray* muchTracks);

	ClassDef(CbmLitConverter, 1);
};

#endif /*CBMLITCONVERTER_H_*/
