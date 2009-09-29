#include "CbmLitTrackFinderNNParallel.h"

#include "CbmLitEnvironment.h"
#include "CbmLitHit.h"
#include "CbmLitPixelHit.h"
#include "CbmLitTrackParam.h"
#include "CbmLitTrack.h"
#include "CbmLitMemoryManagment.h"

#include "parallel/LitTypes.h"
#include "parallel/LitHit.h"
#include "parallel/LitDetectorGeometry.h"
#include "parallel/LitTrack.h"
#include "parallel/LitConverter.h"

#include <iostream>

//class Follow
//{
//	TrackPtrVector& fTracks;
//	CbmLitTrackFinderNNParallel* fFinder;
//public:
//	void operator() ( const tbb::blocked_range<unsigned int>& r ) const {
//		for (unsigned int i = r.begin(); i != r.end(); ++i) {
//			CbmLitTrack* track = fTracks[i];
//			fFinder->FollowTrack(track);
//		}
//	}
//	Follow(CbmLitTrackFinderNNParallel* finder, TrackPtrVector& tracks) :
//		fFinder(finder),
//		fTracks(tracks){}
//};

CbmLitTrackFinderNNParallel::CbmLitTrackFinderNNParallel():
	fEventNo(0)
{
	CbmLitEnvironment* env = CbmLitEnvironment::Instance();
	LitDetectorLayout layout;
	env->GetMuchLayout(layout);
	fTrackFinder.SetDetectorLayout(layout);
}

CbmLitTrackFinderNNParallel::~CbmLitTrackFinderNNParallel()
{
}

LitStatus CbmLitTrackFinderNNParallel::Initialize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitTrackFinderNNParallel::Finalize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitTrackFinderNNParallel::DoFind(
		HitPtrVector& hits,
		TrackPtrVector& trackSeeds,
		TrackPtrVector& tracks)
{
	ScalPixelHitVector lhits;
	TrackVector lseeds;
	TrackVector ltracks;

	ConvertHits(hits, lhits);
	ConvertSeeds(trackSeeds, lseeds);

	fTrackFinder.DoFind(lhits, lseeds, ltracks);

	ConvertTracks(ltracks, tracks);

	for_each(lhits.begin(), lhits.end(), DeleteObject());
	lhits.clear();
	for_each(lseeds.begin(), lseeds.end(), DeleteObject());
	lseeds.clear();
	for_each(ltracks.begin(), ltracks.end(), DeleteObject());
	ltracks.clear();

	std::cout << "Event: " << fEventNo++ << std::endl;

	return kLITSUCCESS;
}

void CbmLitTrackFinderNNParallel::ConvertHits(
		HitPtrVector& hits,
		ScalPixelHitVector& lhits)
{
	for (HitPtrIterator it = hits.begin(); it != hits.end(); it++){
		CbmLitPixelHit* hit = (CbmLitPixelHit*) *it;
		LitScalPixelHit* lhit = new LitScalPixelHit;
		CbmLitPixelHitToLitScalPixelHit(hit, lhit);
		lhits.push_back(lhit);
	}
}

void CbmLitTrackFinderNNParallel::ConvertSeeds(
		TrackPtrVector& seeds,
		TrackVector& lseeds)
{
	for (TrackPtrIterator it = seeds.begin(); it != seeds.end(); it++){
		CbmLitTrack* track = (CbmLitTrack*) *it;
		const CbmLitTrackParam* par = track->GetParamLast();
		LitTrack* ltrack = new LitTrack;

		CbmLitTrackParamToLitScalTrackParam(par, &ltrack->paramFirst);
		CbmLitTrackParamToLitScalTrackParam(par, &ltrack->paramLast);

//		ltrack->previouseTrackId = std::distance(seeds.begin(), it);

		lseeds.push_back(ltrack);
	}
}

void CbmLitTrackFinderNNParallel::ConvertTracks(
		TrackVector& ltracks,
		TrackPtrVector& tracks)
{
	for (TrackIterator it = ltracks.begin(); it != ltracks.end(); it++){
		LitTrack* ltrack = *it;
		CbmLitTrack* track = new CbmLitTrack;
		LitTrackToCbmLitTrack(ltrack, track);
		tracks.push_back(track);
	}
}
