#ifndef CBMLITCONVERTER_H_
#define CBMLITCONVERTER_H_

#include "CbmLitTypes.h"

class CbmLitPixelHit;
class CbmLitStripHit;
class CbmLitTrack;
class CbmLitTrackParam;
class CbmHit;
class CbmTrdHit;
class CbmBaseHit;
class CbmPixelHit;
class CbmStripHit;
class FairTrackParam;
class CbmMuchTrack;
class CbmTrdTrack;
class CbmStsTrack;
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

	static void PixelHitToLitPixelHit(
			const CbmPixelHit* hit,
			Int_t index,
			CbmLitPixelHit* litHit);

	static void StripHitToLitStripHit(
			const CbmStripHit* hit,
			Int_t index,
			CbmLitStripHit* litHit);

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
