#ifndef CBMLITCONVERTER_H_
#define CBMLITCONVERTER_H_

#include "CbmLitTypes.h"

class CbmLitHit;
class CbmLitTrack;
class CbmLitTrackParam;
class CbmTrkHit;
class CbmTrackParam;
class CbmTrdHit;
class CbmMuchTrack;
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
			const CbmTrackParam* par,
			CbmLitTrackParam* litPar);
	
	static void LitTrackParamToTrackParam(
			const CbmLitTrackParam* litPar,
			CbmTrackParam* par);

	static void TrkHitToLitHit(
			const CbmTrkHit* trkHit, 
			Int_t index,
			CbmLitHit* litHit);
	
	static void LitHitToTrdHit(
			const CbmLitHit* litHit,
			CbmTrdHit* trdHit);
	
	static void StsTrackToLitTrack(
			const CbmStsTrack* stsTrack,
			CbmLitTrack* litTrack);
	
	void MuchTrackToLitTrack(
			const CbmMuchTrack* muchTrack,
			CbmLitTrack* litTrack,
			TClonesArray* hits);
	
	static void LitTrackToMuchTrack(
			const CbmLitTrack* litTrack,
			CbmMuchTrack* muchTrack);
	
	static void LitTrackToTrdTrack(
			const CbmLitTrack* litTrack,
			CbmTrdTrack* trdTrack);
	
	static void TrkHitArrayToHitVector(
			const TClonesArray* hits,
			HitVector& litHits);
	
	static void StsTrackArrayToTrackVector(
			const TClonesArray* tracks,
			TrackVector& litTracks);
	
	static void TrackVectorToMuchTrackArray(
			TrackVector& tracks,
			TClonesArray* muchTracks);
	
	static void TrackVectorToTrdTrackArray(
			TrackVector& tracks,
			TClonesArray* muchTracks);
	
	ClassDef(CbmLitConverter, 1);
};

#endif /*CBMLITCONVERTER_H_*/
