#include "LitTrackFinderNNParallel.h"

#include "LitTypes.h"
#include "LitHit.h"
#include "LitAddMaterial.h"
#include "LitExtrapolation.h"
#include "LitFiltration.h"
#include "LitDetectorGeometry.h"
#include "LitHitData.h"
#include "LitField.h"
#include "LitMath.h"
#include "LitVecPack.h"

#include "CbmLitMemoryManagment.h"

//#define LIT_USE_TBB // TBB will be used for multithreading

#ifdef LIT_USE_TBB
#include "tbb/task_scheduler_init.h"
#include "tbb/parallel_for.h"
#include "tbb/blocked_range.h"
#include "tbb/parallel_sort.h"
#endif // LIT_USE_TBB

#include <algorithm>
#include <iostream>

#define cnst static const fvec


class ComparePixelHitXLess :
	public std::binary_function<
			const LitScalPixelHit*,
		    const LitScalPixelHit*,
		    bool>
{
public:
	bool operator()(const LitScalPixelHit* hit1, const LitScalPixelHit* hit2) const {
		return hit1->X < hit2->X;
	}
};

#ifdef LIT_USE_TBB
class PropagateThroughAbsorberClass
{
	LitTrack** fTracks;
	LitTrackFinderNNParallel* fFinder;
	LitAbsorber<fvec>& fAbsorber;
	unsigned int* fTracksId;
public:
	void operator() ( const tbb::blocked_range<unsigned int>& r ) const {
		for (unsigned int iTrack = r.begin(); iTrack != r.end(); ++iTrack) {
			unsigned int start = fvecLen * iTrack;
			// Collect track group
			LitTrack* tracks[fvecLen];
			for(unsigned int i = 0; i < fvecLen; i++) tracks[i] = fTracks[fTracksId[start + i]];
			fFinder->PropagateThroughAbsorber(tracks, fAbsorber);
		}
	}
	PropagateThroughAbsorberClass(
			LitTrackFinderNNParallel* finder,
			LitTrack* tracks[],
			unsigned int* tracksId,
			LitAbsorber<fvec>& absorber) :
		fFinder(finder),
		fTracks(tracks),
		fTracksId(tracksId),
		fAbsorber(absorber) {}
};



class ProcessStationClass
{
	LitTrack** fTracks;
	LitTrackFinderNNParallel* fFinder;
	unsigned int* fTracksId;
	unsigned char fStationGroup;
	unsigned char fStation;
public:
	void operator() ( const tbb::blocked_range<unsigned int>& r ) const {
		for (unsigned int iTrack = r.begin(); iTrack != r.end(); ++iTrack) {
			unsigned int start = fvecLen * iTrack;
			// Collect track group
			LitTrack* tracks[fvecLen];
			for(unsigned int i = 0; i < fvecLen; i++) tracks[i] = fTracks[fTracksId[start + i]];
			fFinder->ProcessStation(tracks, fStationGroup, fStation);
		}
	}
	ProcessStationClass(
			LitTrackFinderNNParallel* finder,
			LitTrack** tracks,
			unsigned int* tracksId,
			unsigned char stationGroup,
			unsigned char station) :
		fFinder(finder),
		fTracks(tracks),
		fTracksId(tracksId),
		fStationGroup(stationGroup),
		fStation(station){}
};


class PropagateToSubstationClass
{
	LitTrackParam<fvec>* flpar;
	LitStation<fvec>& fStation;
	const LitFieldRegion<fvec>& fField;
public:
	void operator() ( const tbb::blocked_range<unsigned int>& r ) const {
		// Loop over the substations
		for (unsigned int i = r.begin(); i != r.end(); ++i) {
			LitSubstation<fvec>& substation = fStation.substations[i];
			// Propagation through station
			LitRK4Extrapolation<fvec>(flpar[i], substation.Z, fField);
			//		LitLineExtrapolation(lpar[iSubstation], substation.Z);
			LitAddMaterial(flpar[i], substation.material);
//			if (iSubstation < nofSubstations - 1) lpar[iSubstation + 1] = lpar[iSubstation];
		}
	}
	PropagateToSubstationClass(
			LitTrackParam<fvec>* lpar,
			LitStation<fvec>& station,
			const LitFieldRegion<fvec>& field) :
		flpar(lpar),
		fStation(station),
		fField(field){}
};



class CollectHitsClass
{
    LitTrackFinderNNParallel* fFinder;
    LitTrackParam<fvec>* flpar;
    LitTrack** fTracks;
    unsigned char fStationGroup;
    unsigned char fStation;
    unsigned int fNofSubstations;

public:
	void operator() ( const tbb::blocked_range<unsigned int>& r ) const {
		// Loop over the substations
	    for (unsigned int i = r.begin(); i != r.end(); ++i) {
	    	LitTrackParam<fscal> spar[fNofSubstations];
            for (unsigned char iSubstation = 0; iSubstation < fNofSubstations; iSubstation++) {
               UnpackTrackParam(i, flpar[iSubstation], spar[iSubstation]);
            }

		    fFinder->CollectHits(spar, fTracks[i], fStationGroup, fStation, fNofSubstations);
	    }
	}

	CollectHitsClass(
			 LitTrackFinderNNParallel* finder,
		     LitTrackParam<fvec>* lpar,
			 LitTrack** tracks,
			 unsigned char stationGroup,
			 unsigned char station,
             unsigned char nofSubstations):
	    fFinder(finder),
	    flpar(lpar),
	    fTracks(tracks),
	    fStationGroup(stationGroup),
	    fStation(station),
        fNofSubstations(nofSubstations){}
};

#endif //LIT_USE_TBB


LitTrackFinderNNParallel::LitTrackFinderNNParallel():
	fMaxNofMissingHits(2),
	fSigmaCoef(3.5),
	fMaxCovSq(20.*20.)
{
#ifdef LIT_USE_TBB
	tbb::task_scheduler_init init;
#endif
}

LitTrackFinderNNParallel::~LitTrackFinderNNParallel()
{
}

void LitTrackFinderNNParallel::DoFind(
		LitScalPixelHit* hits[],
		unsigned int nofHits,
		LitTrack* trackSeeds[],
		unsigned int nofTrackSeeds,
		LitTrack* tracks[],
		unsigned int &nofTracks)
{
	ArrangeHits(hits, nofHits);
	InitTrackSeeds(trackSeeds, nofTrackSeeds);
	FollowTracks();

	//Copy tracks to output
	nofTracks = 0;
	for (unsigned int i = 0; i < fNofTracks; i++) {
		LitTrack* track = fTracks[i];
//		std::cout << track;
		if (track->nofHits < 11) continue;
//		std::cout << *track;
		tracks[nofTracks++] = new LitTrack(*track);
	}

//	for (unsigned int i = 0; i < nofTracks; i++)
//		std::cout << *tracks[i];

	for (unsigned int i = 0; i < fNofTracks; i++) {
		delete fTracks[i];
	}
	fNofTracks = 0;
//	for_each(fTracks.begin(), fTracks.end(), DeleteObject());
//	fTracks.clear();
	fHitData.Clear();
}

void LitTrackFinderNNParallel::ArrangeHits(
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
//    		LitStation& station = fLayout.GetStation(i, j);
//    		if (station.type == kLITPIXELHIT) {
    			for (int k = 0; k < fLayout.GetNofSubstations(i, j); k++){
    				unsigned int nh = fHitData.GetNofHits(i, j, k);
    				LitScalPixelHit** shits = fHitData.GetHits(i, j, k);
    				LitScalPixelHit** begin = &shits[0];
    				LitScalPixelHit** end = &shits[0] + fHitData.GetNofHits(i, j, k);
#ifdef LIT_USE_TBB
    				tbb::parallel_sort(begin, end, ComparePixelHitXLess());
#else
    				std::sort(begin, end, ComparePixelHitXLess());
#endif
//    				std::cout << "station group " << i << " station " << j
//    					<< " substation " << k << std::endl;
//    				for(unsigned int i = 0; i < nh; i++)
//    					std::cout << *shits[i];
    			}
//    		}
    	}
    }
}

void LitTrackFinderNNParallel::MinMaxIndex(
		const LitTrackParam<fscal>* par,
		LitScalPixelHit** hits,
		unsigned int nofHits,
		fscal maxErr,
		unsigned int &first,
		unsigned int &last)
{
	LitScalPixelHit hit;
	first = 0;
	last = 0;
	if (par->C0 > fMaxCovSq || par->C0 < 0.) return;
	if (nofHits == 0) return;

	fscal devX = fSigmaCoef * (std::sqrt(par->C0) + maxErr);
//	fscal devX = fSigmaCoef * std::sqrt(par->C0 + maxErr * maxErr);
	LitScalPixelHit** begin = &hits[0];
	LitScalPixelHit** end = &hits[0] + nofHits;
	hit.X = par->X - devX;
	first = std::distance(begin, std::lower_bound(begin, end, &hit, ComparePixelHitXLess()));
//	std::cout << "---first:" << "hit.X=" << hit.X << " hits[first]->X" << hits[first]->X << std::endl;

	if (first >= nofHits) {
//		std::cout << "----EEEEE----- firstID >= nofHits" << first << " " << nofHits  << std::endl;
		first = 0;
		last = 0;
		return;
	}

	hit.X = par->X + devX;
	last = std::distance(begin, std::lower_bound(begin, end, &hit, ComparePixelHitXLess()));
//	std::cout << "---last:" << "hit.X=" << hit.X << " hits[last]->X" << hits[last]->X << std::endl;
	if (last >= nofHits) {
//		std::cout << "----EEEEE----- lastID >= nofHits"  << last << " " << nofHits << std::endl;
//		first = 0;
		last = nofHits;
	}
}

void LitTrackFinderNNParallel::InitTrackSeeds(
		LitTrack* trackSeeds[],
		unsigned int nofTrackSeeds)
{
//	tbb::parallel_for(tbb::blocked_range<unsigned int>(0, seeds.size()),
//	    				InitTrackSeedsClass(seeds, fTracks), tbb::auto_partitioner());

	fNofTracks = 0;
	fscal QpCut = 1./1.5;
	//TODO : add threads here
	for (unsigned int i = 0; i < nofTrackSeeds; i++) {
		LitTrack* track = trackSeeds[i];
		if (fabs(track->paramLast.Qp) > QpCut) continue;
//		if (fUsedSeedsSet.find((*track)->GetPreviousTrackId()) != fUsedSeedsSet.end()) continue;
//		track->SetPDG(fPDG);
		track->previouseTrackId = i;
//		std::cout << track->previouseTrackId << "   ";

		LitTrack* newTrack = new LitTrack(*track);
//		newTrack->paramFirst = newTrack->paramLast;
		newTrack->paramLast = newTrack->paramFirst;
		fTracks[fNofTracks++] = newTrack;
//		fTracks.push_back(newTrack);
	}
//	std::cout << "TrackSeed.size=" << seeds.size() << ", created:" << fTracks.size() << std::endl;
}


void LitTrackFinderNNParallel::FollowTracks()
{
	// temporary arrays to store track indexes from the fTracks array
	unsigned int tracksId1[fNofTracks];
	unsigned int tracksId2[fNofTracks];
	unsigned int nofTracksId1 = fNofTracks;
	unsigned int nofTracksId2 = 0;

	//Initially use all tracks from fTracks array
	for (unsigned int i = 0; i < nofTracksId1; i++) tracksId1[i] = i;

	// Main loop over station groups for track following
	unsigned char nofStationGroups = fLayout.GetNofStationGroups();
	for(unsigned char iStationGroup = 0; iStationGroup < nofStationGroups; iStationGroup++) { // loop over station groups
		LitStationGroup<fvec> stg = fLayout.stationGroups[iStationGroup];

		unsigned int nofTracks = nofTracksId1; // number of tracks
		unsigned int nofTracksVec = nofTracks / fvecLen; // number of tracks grouped in vectors
		unsigned int dTracks = nofTracks - fvecLen * nofTracksVec; // number of tracks remained after grouping in vectors
//		std::cout << "nofTracks=" << nofTracks << " nofTracksVec=" << nofTracksVec
//			<< " dTracks=" << dTracks << std::endl;

		// loop over fTracks, pack data and propagate through the absorber
#ifdef LIT_USE_TBB
		tbb::parallel_for(tbb::blocked_range<unsigned int>(0, nofTracksVec),
		PropagateThroughAbsorberClass(this, fTracks, tracksId1, stg.absorber), tbb::auto_partitioner());
#else
		for (unsigned int iTrack = 0; iTrack < nofTracksVec; iTrack++) {
			unsigned int start = fvecLen * iTrack;
			// Collect track group
			LitTrack* tracks[fvecLen];
			for(unsigned int i = 0; i < fvecLen; i++) tracks[i] = fTracks[tracksId1[start + i]];
			PropagateThroughAbsorber(tracks, stg.absorber);
		} // loop over tracks
#endif
		// Propagate remaining dTracks through the absorber
		if (dTracks > 0){
			LitTrack* tracks[fvecLen];
			LitTrack dummyTracks[fvecLen - dTracks];
			unsigned int start = fvecLen * nofTracksVec;
			for(unsigned int i = 0; i < dTracks; i++) tracks[i] = fTracks[tracksId1[start + i]];
			// Check if the number of remaining tracks is less than fvecLen.
			if (dTracks < fvecLen) {
				for(unsigned int i = 0; i < fvecLen - dTracks; i++) tracks[dTracks+i] = &dummyTracks[i];//tracks[dTracks-1];
			}
			PropagateThroughAbsorber(tracks, stg.absorber);
		}
		// end propagation through the absorber

		// Loop over stations, and propagate tracks from station to station
		unsigned char nofStations = stg.GetNofStations();
		for(unsigned char iStation = 0; iStation < nofStations; iStation++) { // loop over stations

//			std::cout << "tracksId1.size()=" << tracksId1.size() << std::endl;
			unsigned int nofTracks = nofTracksId1; // number of tracks
			unsigned int nofTracksVec = nofTracks / fvecLen; // number of tracks grouped in vectors
			unsigned int dTracks = nofTracks - fvecLen * nofTracksVec; // number of tracks remained after grouping in vectors
//			std::cout << "iStation --> nofTracks=" << nofTracks << " nofTracksVec=" << nofTracksVec
//				<< " dTracks=" << dTracks << std::endl;
#ifdef LIT_USE_TBB
			tbb::parallel_for(tbb::blocked_range<unsigned int>(0, nofTracksVec),
			ProcessStationClass(this, fTracks, tracksId1, iStationGroup, iStation), tbb::auto_partitioner());
#else
			for (unsigned int iTrack = 0; iTrack < nofTracksVec; iTrack++) { // loop over tracks
				unsigned int start = fvecLen * iTrack;
				// Collect track group
				LitTrack* tracks[fvecLen];
				for(unsigned int i = 0; i < fvecLen; i++) tracks[i] = fTracks[tracksId1[start + i]];
				ProcessStation(tracks, iStationGroup, iStation);
			} // loop over tracks
#endif
			// Propagate remaining dTracks
			if (dTracks > 0){
				LitTrack* tracks[fvecLen];
				LitTrack dummyTracks[fvecLen - dTracks];
				unsigned int start = fvecLen * nofTracksVec;
				for(unsigned int i = 0; i < dTracks; i++) tracks[i] = fTracks[tracksId1[start + i]];
				// Check if the number of remaining tracks is less than fvecLen.
				if (dTracks < fvecLen) {
					for(unsigned int i = 0; i < fvecLen - dTracks; i++) tracks[dTracks+i] = &dummyTracks[i];//tracks[dTracks-1];
				}
				ProcessStation(tracks, iStationGroup, iStation);
			}

			for (unsigned int iTrack = 0; iTrack < nofTracks; iTrack++) {
				unsigned int id = tracksId1[iTrack];
				if (fTracks[id]->nofMissingHits <= fMaxNofMissingHits) {
					tracksId2[nofTracksId2++]= id;
				}
			}
			for (unsigned int i = 0; i < nofTracksId2; i++) tracksId1[i] = tracksId2[i];
			nofTracksId1 = nofTracksId2;
			nofTracksId2 = 0;
		} // loop over stations

	} // loop over station groups
}

void LitTrackFinderNNParallel::PropagateThroughAbsorber(
		LitTrack* tracks[],
		LitAbsorber<fvec>& absorber)
{
	// Pack track parameters
	LitTrackParam<fscal> par[fvecLen];
	for(unsigned int i = 0; i < fvecLen; i++) par[i] = tracks[i]->paramLast;
	LitTrackParam<fvec> lpar;
	PackTrackParam(par, lpar);

	// Propagate through the absorber
	LitFieldRegion<fvec> field;
	LitFieldValue<fvec> v1, v2;
//	LitAbsorber absorber = stg.absorber;
	absorber.fieldSliceFront.GetFieldValue(lpar.X, lpar.Y, v1);
	absorber.fieldSliceBack.GetFieldValue(lpar.X, lpar.Y, v2);
	field.Set(v1, absorber.fieldSliceBack.Z, v2, absorber.fieldSliceFront.Z);
	LitRK4Extrapolation(lpar, absorber.Z, field);
	LitAddMaterial(lpar, absorber.material);

	// Unpack track parameters
	UnpackTrackParam(lpar, par);
	for(unsigned int i = 0; i < fvecLen; i++) tracks[i]->paramLast = par[i];
}

void LitTrackFinderNNParallel::ProcessStation(
		LitTrack* tracks[],
		unsigned char stationGroup,
		unsigned char station)
{
	LitStationGroup<fvec>& stg = fLayout.stationGroups[stationGroup];
	LitStation<fvec>& sta = stg.stations[station];
	unsigned char nofSubstations = sta.GetNofSubstations();

	// Pack track parameters
	LitTrackParam<fscal> par[nofSubstations][fvecLen];
	for(unsigned int i = 0; i < fvecLen; i++) par[0][i] = tracks[i]->paramLast;
	LitTrackParam<fvec> lpar[nofSubstations];
	PackTrackParam(par[0], lpar[0]);

	//Approximate the field between the absorbers
	LitFieldRegion<fvec> field;
	LitFieldValue<fvec> v1, v2;
	LitSubstation<fvec>& ss1 = stg.stations[0].substations[0];
	LitSubstation<fvec>& ss2 = stg.stations[1].substations[0];
	ss1.fieldSlice.GetFieldValue(lpar[0].X, lpar[0].Y, v1);
	ss2.fieldSlice.GetFieldValue(lpar[0].X, lpar[0].Y, v2);
	field.Set(v1, ss1.fieldSlice.Z, v2, ss2.fieldSlice.Z);

#ifdef LIT_USE_TBB
	// Propagate tracks to each substation
	for (unsigned char i = 1; i < nofSubstations; i++) lpar[i] = lpar[0];
	tbb::parallel_for(tbb::blocked_range<unsigned int>(0, sta.GetNofSubstations(), 1),
				PropagateToSubstationClass(lpar, sta, field));
#else
	// Propagate to each of the substations
	for (unsigned char iSubstation = 0; iSubstation < nofSubstations; iSubstation++) { // loop over substations
		LitSubstation<fvec>& substation = sta.substations[iSubstation];
		// Propagation through station
		LitRK4Extrapolation(lpar[iSubstation], substation.Z, field);
//		LitLineExtrapolation(lpar[iSubstation], substation.Z);
		LitAddMaterial(lpar[iSubstation], substation.material);
		if (iSubstation < nofSubstations - 1) lpar[iSubstation + 1] = lpar[iSubstation];
	} // loop over substations
#endif

#ifdef LIT_USE_TBB
    tbb::parallel_for(tbb::blocked_range<unsigned int>(0, fvecLen, 1),
          CollectHitsClass(this, lpar, tracks, stationGroup, station, nofSubstations));
#else
    for (unsigned char iSubstation = 0; iSubstation < nofSubstations; iSubstation++)
		UnpackTrackParam(lpar[iSubstation], par[iSubstation]);
    for(unsigned int i = 0; i < fvecLen; i++) {
		LitTrackParam<fscal> spar[nofSubstations];
		for (unsigned char iSubstation = 0; iSubstation < nofSubstations; iSubstation++)
			spar[iSubstation] = par[iSubstation][i];

		CollectHits(spar, tracks[i], stationGroup, station, nofSubstations);
	}
#endif // LIT_USE_TBB
}


void LitTrackFinderNNParallel::CollectHits(
		LitTrackParam<fscal>* par,
		LitTrack* track,
		unsigned char stationGroup,
		unsigned char station,
		unsigned char nofSubstations)
{
	std::pair<unsigned int, unsigned int> hits[nofSubstations];
	unsigned int nofHits = 0;

	// TODO implement multithreading for this loop
	for (unsigned char iSubstation = 0; iSubstation < nofSubstations; iSubstation++) { // loop over substations
		track->paramLast = par[iSubstation];

		LitScalPixelHit** hitvec = fHitData.GetHits(stationGroup, station, iSubstation);
		unsigned int nh = fHitData.GetNofHits(stationGroup, station, iSubstation);
		fscal err = fHitData.GetMaxErr(stationGroup, station, iSubstation);

		MinMaxIndex(&par[iSubstation], hitvec, nh, err, hits[iSubstation].first, hits[iSubstation].second);
		nofHits += hits[iSubstation].second - hits[iSubstation].first;
	} // loop over substations


	bool hitAdded = false;
	LitScalPixelHit* ahits[nofHits];
	LitTrackParam<fscal>* apars[nofHits];
	unsigned int cnt = 0;
	for(unsigned char iss = 0; iss < nofSubstations; iss++) {
		LitScalPixelHit** hitvec = fHitData.GetHits(stationGroup, station, iss);
		for (unsigned int j = hits[iss].first; j < hits[iss].second; j++) {
			ahits[cnt] = hitvec[j];
			apars[cnt] = &par[iss];
			cnt++;
		}
	}

	if (AddNearestHit(track, ahits, apars, nofHits)) hitAdded = true;
	// Check if hit was added, if not than increase number of missing hits
	if (!hitAdded) track->nofMissingHits++;
}

bool LitTrackFinderNNParallel::AddNearestHit(
		LitTrack* track,
		LitScalPixelHit* hits[],
		LitTrackParam<fscal>* pars[],
		unsigned int nofHits)
{
	bool hitAdded = false;
	LitScalPixelHit* hita = NULL;
	LitTrackParam<fscal> param;
	fscal chiSq = 1e10;

	unsigned int nofHitsVec = nofHits / fvecLen; // number of hits grouped in vectors
	unsigned int dHits = nofHits - fvecLen * nofHitsVec; // number of hits remained after grouping in vectors
	for (unsigned int iHit = 0; iHit < nofHitsVec; iHit++) {
		unsigned int start = fvecLen * iHit;

		// Pack hit
		LitScalPixelHit hit[fvecLen];
		for(unsigned int i = 0; i < fvecLen; i++) hit[i] = *hits[start + i];
		LitPixelHit<fvec> lhit;
		PackPixelHit(hit, lhit);
		// Pack track parameters
		LitTrackParam<fscal> par[fvecLen];
		for(unsigned int i = 0; i < fvecLen; i++) par[i] = *pars[start + i];
		LitTrackParam<fvec> lpar;
		PackTrackParam(par, lpar);

		//First update track parameters with KF, than check whether the hit is in the validation gate.
		LitFiltration(lpar, lhit);
		cnst CHISQCUT = 15.;
		fvec chisq = ChiSq(lpar, lhit);

		// Unpack track parameters
		UnpackTrackParam(lpar, par);
		for (unsigned int i = 0; i < fvecLen; i++) {
			if (chisq[i] < CHISQCUT[i] && chisq[i] < chiSq) {
				chiSq = chisq[i];
				hita = hits[start + i];
				param = par[i];
			}
		}
	}
	if (dHits > 0){
		unsigned int start = fvecLen * nofHitsVec;
		LitScalPixelHit hit[fvecLen];
		LitPixelHit<fvec> lhit;
		LitTrackParam<fscal> par[fvecLen];
		LitTrackParam<fvec> lpar;
		for(unsigned int i = 0; i < dHits; i++) {
			hit[i] = *hits[start + i];
			par[i] = *pars[start + i];
		}
		// Check if the number of remaining tracks is less than fvecLen.
		if (dHits < fvecLen) {
			for(unsigned int i = 0; i < fvecLen - dHits; i++) {
				hit[dHits+i] = *hits[nofHits-1];
				par[dHits+i] = *pars[nofHits-1];
			}
		}
		PackPixelHit(hit, lhit);
		PackTrackParam(par, lpar);

		//First update track parameters with KF, than check whether the hit is in the validation gate.
		LitFiltration(lpar, lhit);
		cnst CHISQCUT = 15.;
		fvec chisq = ChiSq(lpar, lhit);

		// Unpack track parameters
		UnpackTrackParam(lpar, par);
		for (unsigned int i = 0; i < fvecLen; i++) {
			if (chisq[i] < CHISQCUT[i] && chisq[i] < chiSq) {
				chiSq = chisq[i];
				hita = hits[start + i];
				param = par[i];
			}
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

#undef cnst
