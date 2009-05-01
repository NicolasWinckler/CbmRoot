#ifndef CBMLITCONVERTER_H_
#define CBMLITCONVERTER_H_

#include "CbmLitTypes.h"

class CbmLitPixelHit;
class CbmLitStripHit;
class CbmLitTrack;
class CbmLitTrackParam;
class CbmHit;
class FairTrackParam;
class CbmTrdHit;
class CbmMuchTrack;
class CbmMuchHit;
class CbmTrdTrack;
class CbmStsTrack;
class CbmTofHit;
class CbmGlobalTrack;
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
			const CbmHit* trkHit,
			Int_t index,
			CbmLitPixelHit* litHit);

	static void MuchHitToLitStripHit(
			CbmMuchHit* muchHit,
			Int_t index,
			CbmLitStripHit* litHit);

	static void TofHitToLitPixelHit(
			const CbmTofHit* trkHit,
			Int_t index,
			CbmLitPixelHit* litHit);

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

	static void GlobalTrackToLitTrack(
			const CbmGlobalTrack* globalTrack,
			CbmLitTrack* litTrack);

	static void LitTrackVectorToGlobalTrackArray(
			const TrackPtrVector& litTracks,
			TClonesArray* globalTracks,
			TClonesArray* trdTracks,
			TClonesArray* muchTracks);

	static void TrdHitArrayToPixelHitVector(
			const TClonesArray* hits,
			HitPtrVector& litHits);

	static void TofHitArrayToPixelHitVector(
			const TClonesArray* hits,
			HitPtrVector& litHits);

	static void MuchHitArrayToHitVector(
			const TClonesArray* hits,
			HitPtrVector& litHits);

	static void StsTrackArrayToTrackVector(
			const TClonesArray* tracks,
			TrackPtrVector& litTracks);

	static void TrdTrackArrayToTrackVector(
			const TClonesArray* tracks,
			TrackPtrVector& litTracks,
			TClonesArray* hits);

	static void TrackVectorToMuchTrackArray(
			TrackPtrVector& tracks,
			TClonesArray* muchTracks);

	static void TrackVectorToTrdTrackArray(
			TrackPtrVector& tracks,
			TClonesArray* trdTracks);

	ClassDef(CbmLitConverter, 1);
};

#endif /*CBMLITCONVERTER_H_*/
