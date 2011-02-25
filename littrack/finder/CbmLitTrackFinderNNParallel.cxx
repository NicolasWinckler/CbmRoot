#include "finder/CbmLitTrackFinderNNParallel.h"

#include "base/CbmLitEnvironment.h"
#include "data/CbmLitHit.h"
#include "data/CbmLitPixelHit.h"
#include "data/CbmLitTrackParam.h"
#include "data/CbmLitTrack.h"
#include "utils/CbmLitMemoryManagment.h"

#include "parallel/LitTypes.h"
#include "parallel/LitHit.h"
#include "parallel/muon/LitDetectorGeometryMuon.h"
#include "parallel/LitTrack.h"
#include "parallel/LitConverter.h"
#include "parallel/muon/LitTrackFinderNNVecMuon.h"
#include "parallel/electron/LitTrackFinderNNScalarElectron.h"
#include "parallel/electron/LitTrackFinderNNVecElectron.h"

#include <iostream>

#include "tbb/tick_count.h"

CbmLitTrackFinderNNParallel::CbmLitTrackFinderNNParallel(
		const std::string& trackingType):
	fTime(0.),
	fEventNo(1),
	fTrackingType(trackingType)
{
	CbmLitEnvironment* env = CbmLitEnvironment::Instance();

	if (fTrackingType == "nn_parallel_muon") {
		LitDetectorLayoutMuonVec layout;
		env->GetMuchLayoutVec(layout);
		fTFParallelMuon = new LitTrackFinderNNVecMuon();
		fTFParallelMuon->SetDetectorLayout(layout);
	} else if (fTrackingType == "nn_scalar_electron") {
		LitDetectorLayoutElectronScal layout;
		env->GetTrdLayoutScal(layout);
		fTFScalElectron = new LitTrackFinderNNScalarElectron();
		fTFScalElectron->SetDetectorLayout(layout);
	} else if (fTrackingType == "nn_vec_electron") {
		LitDetectorLayoutElectronVec layout;
		env->GetTrdLayoutVec(layout);
		fTFVecElectron = new LitTrackFinderNNVecElectron();
		fTFVecElectron->SetDetectorLayout(layout);
	} else {
		std::cout << "-E- TRACKING TYPE NOT FOUND" << std::endl;
		exit(0);
	}
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
	LitScalTrack* lseeds[NTRACKS];
	LitScalTrack* ltracks[NTRACKS];

	ConvertHits(hits, lhits);
	ConvertSeeds(trackSeeds, lseeds);
	unsigned int nofTracks = 0;

	tbb::tick_count t0 = tbb::tick_count::now();
	if (fTrackingType == "nn_parallel_muon")
		fTFParallelMuon->DoFind(lhits, hits.size(), lseeds, trackSeeds.size(), ltracks, nofTracks);
	else if (fTrackingType == "nn_scalar_electron")
		fTFScalElectron->DoFind(lhits, hits.size(), lseeds, trackSeeds.size(), ltracks, nofTracks);
	else if (fTrackingType == "nn_vec_electron")
		fTFVecElectron->DoFind(lhits, hits.size(), lseeds, trackSeeds.size(), ltracks, nofTracks);

	tbb::tick_count t1 = tbb::tick_count::now();

	double dtime = (t1-t0).seconds();
	fTime += dtime;
	std::cout << "Time: " << dtime << " sec" << std::endl;
	std::cout << "Total:" << fTime << " sec / " << fEventNo << " events." << std::endl;
	std::cout << "Average per event:" << fTime / fEventNo << " sec." << std::endl;

	ConvertTracks(ltracks, nofTracks, tracks);

//	std::cout << "NOFTRACKS=" << nofTracks << " tracks.size=" << tracks.size() << std::endl;
//	for (unsigned int i = 0; i < tracks.size(); i++) {
//		std::cout << tracks[i]->ToString();
//	}

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
		LitScalTrack* lseeds[])
{
	for (unsigned int i = 0; i < seeds.size(); i++){
		CbmLitTrack* track = (CbmLitTrack*) seeds[i];
		const CbmLitTrackParam* par = track->GetParamLast();
		LitScalTrack* ltrack = new LitScalTrack;

		CbmLitTrackParamToLitTrackParamScal(par, &ltrack->paramFirst);
		CbmLitTrackParamToLitTrackParamScal(par, &ltrack->paramLast);

//		ltrack->previouseTrackId = std::distance(seeds.begin(), it);

		lseeds[i] = ltrack;
	}
}

void CbmLitTrackFinderNNParallel::ConvertTracks(
		LitScalTrack* ltracks[],
		unsigned int nofTracks,
		TrackPtrVector& tracks)
{
	for (unsigned int i = 0; i < nofTracks; i++){
		LitScalTrack* ltrack = ltracks[i];
		CbmLitTrack* track = new CbmLitTrack;
		if (fTrackingType == "nn_parallel_muon")
			LitScalTrackToCbmLitTrack(ltrack, track, kLITMUCH);
		else if (fTrackingType == "nn_scalar_electron" || fTrackingType == "nn_vec_electron")
			LitScalTrackToCbmLitTrack(ltrack, track, kLITTRD);
		tracks.push_back(track);
	}
}
