#include "LitTrackFinderNNScalarElectron.h"

#include "../LitTypes.h"
#include "../LitHit.h"
#include "../LitAddMaterial.h"
#include "../LitExtrapolation.h"
#include "../LitFiltration.h"
#include "../LitField.h"
#include "../LitMath.h"
#include "../LitConverter.h"

#include "CbmLitEnvironment.h"
#include "CbmLitMapField.h"
#include "CbmLitToolFactory.h"

#include <algorithm>
#include <iostream>
#include <limits>


#ifdef LIT_USE_TBB
#undef LIT_USE_TBB
#endif

//#define LIT_USE_TBB // TBB will be used for multithreading

#ifdef LIT_USE_TBB
#include "tbb/task_scheduler_init.h"
#include "tbb/parallel_for.h"
#include "tbb/blocked_range.h"
#include "tbb/parallel_sort.h"
#endif // LIT_USE_TBB


#ifdef LIT_USE_TBB
class FollowTrackClass
{
	LitScalTrack** fTracks;
	LitTrackFinderNNScalarElectron* fFinder;
public:
	void operator() ( const tbb::blocked_range<unsigned int>& r ) const {
		for (unsigned int iTrack = r.begin(); iTrack != r.end(); ++iTrack) {
			LitScalTrack* track = fTracks[iTrack];
			fFinder->PropagateToFirstStation(track);
			fFinder->FollowTrack(track);
		}
	}
	FollowTrackClass(
			LitTrackFinderNNScalarElectron* finder,
			LitScalTrack* tracks[]) :
		fFinder(finder),
		fTracks(tracks) {}
};
#endif

LitTrackFinderNNScalarElectron::LitTrackFinderNNScalarElectron():
	fMaxNofMissingHits(3)
{
	CbmLitEnvironment* env = CbmLitEnvironment::Instance();
	fField = new CbmLitMapField(env->GetField());

	CbmLitToolFactory* factory = CbmLitToolFactory::Instance();
	fExtrapolator = factory->CreateTrackExtrapolator("lit");
	fPropagator = factory->CreateTrackPropagator("lit");

	SetSigmaCoef(5.);
	SetMaxCovSq(20.*20.);

#ifdef LIT_USE_TBB
	tbb::task_scheduler_init init;
#endif
}

LitTrackFinderNNScalarElectron::~LitTrackFinderNNScalarElectron()
{
}

void LitTrackFinderNNScalarElectron::DoFind(
		LitScalPixelHit* hits[],
		unsigned int nofHits,
		LitScalTrack* trackSeeds[],
		unsigned int nofTrackSeeds,
		LitScalTrack* tracks[],
		unsigned int &nofTracks)
{
	ArrangeHits(hits, nofHits);
	InitTrackSeeds(trackSeeds, nofTrackSeeds);
	FollowTracks();

	//Copy tracks to output
	nofTracks = 0;
	for (unsigned int i = 0; i < fNofTracks; i++) {
		LitScalTrack* track = fTracks[i];
		tracks[nofTracks++] = new LitScalTrack(*track);
	}

	for (unsigned int i = 0; i < fNofTracks; i++) {
		delete fTracks[i];
	}
	fNofTracks = 0;
//	for_each(fTracks.begin(), fTracks.end(), DeleteObject());
//	fTracks.clear();
	fHitData.Clear();
}

void LitTrackFinderNNScalarElectron::ArrangeHits(
		LitScalPixelHit* hits[],
		unsigned int nofHits)
{
	// TODO : add threads here
    for(unsigned int i = 0; i < nofHits; i++) {
    	LitScalPixelHit* hit = hits[i];
//    	if (fUsedHitsSet.find(hit->GetRefId()) != fUsedHitsSet.end()) continue;
     	fHitData.AddHit(hit->planeId, hit);
    }
//    std::cout << fHitData;

    // TODO : add threads here
    for (int i = 0; i < fLayout.GetNofStationGroups(); i++){
    	for (int j = 0; j < fLayout.GetNofStations(i); j++){
			LitScalPixelHit** shits = fHitData.GetHits(i, j);
			LitScalPixelHit** begin = &shits[0];
			LitScalPixelHit** end = &shits[0] + fHitData.GetNofHits(i, j);

#ifdef LIT_USE_TBB
    		tbb::parallel_sort(begin, end, ComparePixelHitXLess());
#else
    		std::sort(begin, end, ComparePixelHitXLess());
#endif
    	}
    }

//    std::cout << fHitData;
}

void LitTrackFinderNNScalarElectron::InitTrackSeeds(
		LitScalTrack* trackSeeds[],
		unsigned int nofTrackSeeds)
{
//	tbb::parallel_for(tbb::blocked_range<unsigned int>(0, seeds.size()),
//	    				InitTrackSeedsClass(seeds, fTracks), tbb::auto_partitioner());

	fNofTracks = 0;
	fscal QpCut = 1./0.1;
	//TODO : add threads here
	for (unsigned int i = 0; i < nofTrackSeeds; i++) {
		LitScalTrack* track = trackSeeds[i];
		if (fabs(track->paramLast.Qp) > QpCut) continue;
//		if (fUsedSeedsSet.find((*track)->GetPreviousTrackId()) != fUsedSeedsSet.end()) continue;
//		track->SetPDG(fPDG);
		track->previouseTrackId = i;
//		std::cout << track->previouseTrackId << "   ";

		LitScalTrack* newTrack = new LitScalTrack(*track);
//		newTrack->paramFirst = newTrack->paramLast;
		newTrack->paramLast = newTrack->paramFirst;
		fTracks[fNofTracks++] = newTrack;
//		fTracks.push_back(newTrack);
	}
//	std::cout << "TrackSeed.size=" << seeds.size() << ", created:" << fTracks.size() << std::endl;
}

void LitTrackFinderNNScalarElectron::FollowTracks()
{
#ifdef LIT_USE_TBB
	tbb::parallel_for(tbb::blocked_range<unsigned int>(0, fNofTracks),
	FollowTrackClass(this, fTracks), tbb::auto_partitioner());
#else
	for (unsigned int iTrack = 0; iTrack < fNofTracks; iTrack++) {
		LitScalTrack* track = fTracks[iTrack];
		PropagateToFirstStation(track);
		FollowTrack(track);
	}
#endif
}

void LitTrackFinderNNScalarElectron::PropagateToFirstStation(
		LitScalTrack* track)
{
//    for (unsigned char ivp = 0; ivp < fLayout.GetNofVirtualPlanes()-1; ivp++) {
//    	LitVirtualPlaneElectron<fscal>& vp1 = fLayout.virtualPlanes[ivp];
//    	LitVirtualPlaneElectron<fscal>& vp2 = fLayout.virtualPlanes[ivp+1];
//
//    	LitTrackParamScal lpar = track->paramLast;
//    	CbmLitTrackParam par;
//    	LitTrackParamScalToCbmLitTrackParam(&lpar, &par);
//    	fPropagator->Propagate(&par, vp2.Z, 11, NULL);
////    	fExtrapolator->Extrapolate(&par, vp2.Z);
//        CbmLitTrackParamToLitTrackParamScal(&par, &lpar);
//        track->paramLast = lpar;
//
////    	LitRK4ExtrapolationElectron(track->paramLast, vp2.Z, vp1.fieldSlice, vp1.fieldSliceMid, vp2.fieldSlice);
////    	LitRK4ExtrapolationElectron1(track->paramLast, vp2.Z, fField);
////    	LitAddMaterial(track->paramLast, vp2.material);
//    }

    const LitStationElectron<fscal>& st = fLayout.GetStation(0, 0);

	LitTrackParamScal lpar = track->paramLast;
	CbmLitTrackParam par;
	LitTrackParamScalToCbmLitTrackParam(&lpar, &par);
	fPropagator->Propagate(&par, st.Z, 11, NULL);
//    	fExtrapolator->Extrapolate(&par, vp2.Z);
    CbmLitTrackParamToLitTrackParamScal(&par, &lpar);
    track->paramLast = lpar;


}

void LitTrackFinderNNScalarElectron::FollowTrack(
		LitScalTrack *track)
{
	int nofStationGroups = fLayout.GetNofStationGroups();
	for(int iStationGroup = 0; iStationGroup < nofStationGroups; iStationGroup++) {
		if (!ProcessStationGroup(track, iStationGroup)) return;
	}
}

bool LitTrackFinderNNScalarElectron::ProcessStationGroup(
		LitScalTrack *track,
		int stationGroup)
{
	int nofStations = fLayout.GetNofStations(stationGroup);
	for(int iStation = 0; iStation < nofStations; iStation++){
		if (!ProcessStation(track, stationGroup, iStation)) {
			track->nofMissingHits++;
			if (track->nofMissingHits > fMaxNofMissingHits) return false;
		}
	}
	return true;
}

bool LitTrackFinderNNScalarElectron::ProcessStation(
		LitScalTrack *track,
		int stationGroup,
		int station)
{
	bool hitAdded = false;

	LitTrackParamScal& par = track->paramLast;

	const LitStationElectronScal& st = fLayout.GetStation(stationGroup, station);

	LitTrackParamScal lpar = track->paramLast;
	CbmLitTrackParam par1;
	LitTrackParamScalToCbmLitTrackParam(&lpar, &par1);
	fPropagator->Propagate(&par1, st.Z, 11, NULL);
//	fExtrapolator->Extrapolate(&par1, st.Z);
    CbmLitTrackParamToLitTrackParamScal(&par1, &lpar);
    track->paramLast = lpar;
    par = lpar;

//	LitLineExtrapolation(par, st.Z);
//
//	for (unsigned char im = 0; im < st.GetNofMaterialsBefore(); im++)
//		LitAddMaterial(par, st.materialsBefore[im]);

	std::pair<unsigned int, unsigned int> hits;
	LitScalPixelHit** hitvec = fHitData.GetHits(stationGroup, station);
	unsigned int nh = fHitData.GetNofHits(stationGroup, station);
	fscal err = fHitData.GetMaxErr(stationGroup, station);

	MinMaxIndex(&par, hitvec, nh, err, hits.first, hits.second);

	hitAdded = AddNearestHit(track, hits, &par, stationGroup, station);

//	for (unsigned char im = 0; im < st.GetNofMaterialsAfter(); im++)
//		LitAddMaterial(par, st.materialsAfter[im]);

	return hitAdded;
}

bool LitTrackFinderNNScalarElectron::AddNearestHit(
		LitScalTrack* track,
		const std::pair<unsigned int, unsigned int>& hits,
		LitTrackParamScal* par,
		int stationGroup,
		int station)
{
	bool hitAdded = false;
	LitScalPixelHit* hita = NULL;
	LitTrackParamScal param;
	fscal chiSq = std::numeric_limits<fscal>::max();

	LitScalPixelHit** hitvec = fHitData.GetHits(stationGroup, station);
	for (unsigned int iHit = hits.first; iHit < hits.second; iHit++) {
		LitScalPixelHit* hit = hitvec[iHit];

		LitTrackParamScal upar = *par;
		LitPixelHitScal lhit;
		lhit.X = hit->X;
		lhit.Y = hit->Y;
		lhit.Dx = hit->Dx;
		lhit.Dy = hit->Dy;
		lhit.Dxy = hit->Dxy;

		//First update track parameters with KF, than check whether the hit is in the validation gate.
		LitFiltration(upar, lhit);
		static const fscal CHISQCUT = 50.;
		fscal chisq = ChiSq(upar, lhit);

		if (chisq < CHISQCUT && chisq < chiSq) {
			chiSq = chisq;
			hita = hit;
			param = upar;
		}
	}

	// if hit was attached than change track information
	if (hita != NULL) {
		track->AddHit(hita);
		track->paramLast = param;
		track->chiSq += chiSq;
		track->NDF = NDF(*track);
		hitAdded = true;
	}
	return hitAdded;
}
