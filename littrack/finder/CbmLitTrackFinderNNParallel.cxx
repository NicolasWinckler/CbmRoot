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

#include "tbb/tick_count.h"

CbmLitTrackFinderNNParallel::CbmLitTrackFinderNNParallel():
	fEventNo(1),
	fTime(0.)
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
	const unsigned int NHITS = hits.size();
	LitScalPixelHit* lhits[NHITS];
	const unsigned int NTRACKS = trackSeeds.size();
	LitTrack* lseeds[NTRACKS];
	LitTrack* ltracks[NTRACKS];

	ConvertHits(hits, lhits);
	ConvertSeeds(trackSeeds, lseeds);
	unsigned int nofTracks = 0;

	tbb::tick_count t0 = tbb::tick_count::now();
	fTrackFinder.DoFind(lhits, hits.size(), lseeds, trackSeeds.size(), ltracks, nofTracks);
	tbb::tick_count t1 = tbb::tick_count::now();

	double dtime = (t1-t0).seconds();
	fTime += dtime;
	std::cout << "Time: " << dtime << " sec" << std::endl;
	std::cout << "Total:" << fTime << " sec / " << fEventNo << " events." << std::endl;
	std::cout << "Average per event:" << fTime / fEventNo << " sec." << std::endl;

	ConvertTracks(ltracks, nofTracks, tracks);

	for (unsigned int i = 0; i < NHITS; i++) {
		delete lhits[i];
	}
//	for_each(lhits.begin(), lhits.end(), DeleteObject());
//	lhits.clear();
	for (unsigned int i = 0; i < NTRACKS; i++) {
		delete lseeds[i];
	}
//	for_each(lseeds.begin(), lseeds.end(), DeleteObject());
//	lseeds.clear();
	for (unsigned int i = 0; i < nofTracks; i++) {
		delete ltracks[i];
	}
//	for_each(ltracks.begin(), ltracks.end(), DeleteObject());
//	ltracks.clear();

	std::cout << "Event: " << fEventNo++ << std::endl;

	return kLITSUCCESS;
}

void CbmLitTrackFinderNNParallel::ConvertHits(
		HitPtrVector& hits,
		LitScalPixelHit* lhits[])
{
	for (unsigned int i = 0; i < hits.size(); i++){
		CbmLitPixelHit* hit = (CbmLitPixelHit*) hits[i];
		LitScalPixelHit* lhit = new LitScalPixelHit;
		CbmLitPixelHitToLitScalPixelHit(hit, lhit);
		lhits[i] = lhit;
	}
}

void CbmLitTrackFinderNNParallel::ConvertSeeds(
		TrackPtrVector& seeds,
		LitTrack* lseeds[])
{
	for (unsigned int i = 0; i < seeds.size(); i++){
		CbmLitTrack* track = (CbmLitTrack*) seeds[i];
		const CbmLitTrackParam* par = track->GetParamLast();
		LitTrack* ltrack = new LitTrack;

		CbmLitTrackParamToLitScalTrackParam(par, &ltrack->paramFirst);
		CbmLitTrackParamToLitScalTrackParam(par, &ltrack->paramLast);

//		ltrack->previouseTrackId = std::distance(seeds.begin(), it);

		lseeds[i] = ltrack;
	}
}

void CbmLitTrackFinderNNParallel::ConvertTracks(
		LitTrack* ltracks[],
		unsigned int nofTracks,
		TrackPtrVector& tracks)
{
	for (unsigned int i = 0; i < nofTracks; i++){
		LitTrack* ltrack = ltracks[i];
		CbmLitTrack* track = new CbmLitTrack;
		LitTrackToCbmLitTrack(ltrack, track);
		tracks.push_back(track);
	}
}
