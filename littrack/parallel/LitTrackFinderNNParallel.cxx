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

//#include "tbb/task_scheduler_init.h"
//#include "tbb/parallel_for.h"
//#include "tbb/blocked_range.h"

#include <algorithm>
#include <iostream>
#include <algorithm>

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

LitTrackFinderNNParallel::LitTrackFinderNNParallel():
	fMaxNofMissingHits(2),
	fIsProcessSubstationsTogether(true),
	fSigmaCoef(3.5),
	fMaxCovSq(20.*20.)
{
//	tbb::task_scheduler_init init;
}

LitTrackFinderNNParallel::~LitTrackFinderNNParallel()
{
}

void LitTrackFinderNNParallel::DoFind(
		ScalPixelHitVector& hits,
		TrackVector& trackSeeds,
		TrackVector& tracks)
{
//	fTracks.clear();
//	fUsedSeedsSet.clear();
//	fUsedHitsSet.clear();
//	fHitData.SetDetectorLayout(fLayout);

//	std::cout << fLayout;

//	for (int iIter = 0; iIter < fNofIter; iIter++) {
//		SetIterationParameters(iIter);
		ArrangeHits(hits);
		InitTrackSeeds(trackSeeds);
		FollowTracks();
//		fFinalSelection->DoSelect(fTracks.begin(), fTracks.end());
//		RemoveHits(fTracks.begin(), fTracks.end());
//		CopyToOutput(fTracks.begin(), fTracks.end(), tracks);
//
		//Copy tracks to output
		for (TrackIterator it = fTracks.begin(); it != fTracks.end(); it++) {
			LitTrack* track = *it;
//			std::cout << track;
			if (track->hits.size() < 11) continue;
//			std::cout << "cut 11" << track;
			tracks.push_back(new LitTrack(*track));
		}

		for_each(fTracks.begin(), fTracks.end(), DeleteObject());
		fTracks.clear();
		fHitData.Clear();
//	}
//	std::cout << "-I- LitTrackFinderNNParallel: " << fEventNo++ << " events processed" << std::endl;
//
//	return kLITSUCCESS;
}

void LitTrackFinderNNParallel::ArrangeHits(
		ScalPixelHitVector& hits)
{
    for(ScalPixelHitIterator it = hits.begin(); it != hits.end(); it++) {
    	LitScalPixelHit* hit = *it;
//    	if (fUsedHitsSet.find(hit->GetRefId()) != fUsedHitsSet.end()) continue;
     	fHitData.AddHit(hit->planeId, hit);
    }

//    std::cout << fHitData;

    for (int i = 0; i < fLayout.GetNofStationGroups(); i++){
    	for (int j = 0; j < fLayout.GetNofStations(i); j++){
    		LitStation station = fLayout.GetStation(i, j);
//    		if (station.type == kLITPIXELHIT) {
    			for (int k = 0; k < fLayout.GetNofSubstations(i, j); k++){
    				ScalPixelHitVector& shits = fHitData.GetHits(i, j, k);
    				std::sort(shits.begin(), shits.end(), ComparePixelHitXLess());
//    				std::cout << "station group " << i << " station " << j
//    					<< " substation " << k << std::endl;
//    				for(ScalPixelHitIterator it = shits.begin(); it != shits.end(); it++)
//    					std::cout << *it;
//    			}
    		}
    	}
    }
}

void LitTrackFinderNNParallel::MinMaxIndex(
		const LitScalTrackParam* par,
		ScalPixelHitVector& hits,
		fscal maxErr,
		ScalPixelHitIterator& first,
		ScalPixelHitIterator& last) const
{
	LitScalPixelHit hit;
	if(par->C0 > fMaxCovSq || par->C0 < 0.) return;
	fscal devX = fSigmaCoef * (std::sqrt(par->C0) + maxErr);
//	fscal devX = fSigmaCoef * std::sqrt(par->C0 + maxErr * maxErr);
	hit.X = par->X - devX;
	first = std::lower_bound(hits.begin(), hits.end(), &hit, ComparePixelHitXLess());
	hit.X = par->X + devX;
	last = std::lower_bound(hits.begin(), hits.end(), &hit, ComparePixelHitXLess());
}

void LitTrackFinderNNParallel::InitTrackSeeds(
		TrackVector& seeds)
{
	fscal QpCut = 1./1.5;
	for (TrackIterator it = seeds.begin(); it != seeds.end(); it++) {
		LitTrack* track = *it;
		if (fabs(track->paramLast.Qp) > QpCut) continue;
//		if (fUsedSeedsSet.find((*track)->GetPreviousTrackId()) != fUsedSeedsSet.end()) continue;
//		track->SetPDG(fPDG);
		track->previouseTrackId = std::distance(seeds.begin(), it);
//		std::cout << track->previouseTrackId << "   ";

		LitTrack* newTrack = new LitTrack(*track);
//		newTrack->paramFirst = newTrack->paramLast;
		newTrack->paramLast = newTrack->paramFirst;
		fTracks.push_back(newTrack);
	}
	std::cout << "TrackSeed.size=" << seeds.size() << ", created:" << fTracks.size() << std::endl;
}


void LitTrackFinderNNParallel::FollowTracks()
{
//	for (TrackIterator it = fTracks.begin(); it != fTracks.end(); it++) {
//		FollowTrack(*it);
//	}

//	unsigned int N = std::distance(itBegin, itEnd);
//	std::cout << "N=" << N << std::endl;
//	tbb::parallel_for(tbb::blocked_range<unsigned int>(0, N), Follow(this, fTracks), tbb::auto_partitioner());

	// temporary array to store track indexes from the fTracks array
	std::vector<unsigned int> tracksId1(fTracks.size());
	std::vector<unsigned int> tracksId2;

	//Initially use all tracks from fTracks array
	for (unsigned int i = 0; i < fTracks.size(); i++) tracksId1[i] = i;

	// Main loop over station groups for track following
	int nofStationGroups = fLayout.GetNofStationGroups();
	for(int iStationGroup = 0; iStationGroup < nofStationGroups; iStationGroup++) { // loop over station groups
		LitStationGroup stg = fLayout.stationGroups[iStationGroup];

		unsigned int nofTracks = tracksId1.size(); // number of tracks
		unsigned int nofTracksVec = nofTracks / fvecLen; // number of tracks grouped in vectors
		unsigned int dTracks = nofTracks - fvecLen * nofTracksVec; // number of tracks remained after grouping in vectors
//		std::cout << "nofTracks=" << nofTracks << " nofTracksVec=" << nofTracksVec
//			<< " dTracks=" << dTracks << std::endl;

		// loop over fTracks, pack data and propagate through the absorber
		for (unsigned int iTrack = 0; iTrack < nofTracksVec; iTrack++) {
			unsigned int start = fvecLen * iTrack;
			// Collect track group
			LitTrack* tracks[fvecLen];
			for(unsigned int i = 0; i < fvecLen; i++) tracks[i] = fTracks[tracksId1[start + i]];
			PropagateTroughAbsorber(tracks, stg.absorber);
		} // loop over tracks
		// Propagate remaining dTracks through the absorber
		if (dTracks > 0){
			LitTrack* tracks[fvecLen];
			unsigned int start = fvecLen * nofTracksVec;
			for(unsigned int i = 0; i < dTracks; i++) tracks[i] = fTracks[tracksId1[start + i]];
			// Check if the number of remaining tracks is less than fvecLen.
			if (dTracks < fvecLen) {
				for(unsigned int i = 0; i < fvecLen - dTracks; i++) tracks[dTracks+i] = tracks[dTracks-1];
			}
			PropagateTroughAbsorber(tracks, stg.absorber);
		}
		// end propagation through the absorber

		// Loop over stations, and propagate tracks from station to station
		unsigned char nofStations = stg.GetNofStations();
		for(unsigned char iStation = 0; iStation < nofStations; iStation++) { // loop over stations

//			std::cout << "tracksId1.size()=" << tracksId1.size() << std::endl;
			unsigned int nofTracks = tracksId1.size(); // number of tracks
			unsigned int nofTracksVec = nofTracks / fvecLen; // number of tracks grouped in vectors
			unsigned int dTracks = nofTracks - fvecLen * nofTracksVec; // number of tracks remained after grouping in vectors
//			std::cout << "iStation --> nofTracks=" << nofTracks << " nofTracksVec=" << nofTracksVec
//				<< " dTracks=" << dTracks << std::endl;

			for (unsigned int iTrack = 0; iTrack < nofTracksVec; iTrack++) { // loop over tracks
				unsigned int start = fvecLen * iTrack;
				// Collect track group
				LitTrack* tracks[fvecLen];
				for(unsigned int i = 0; i < fvecLen; i++) tracks[i] = fTracks[tracksId1[start + i]];
				ProcessStation(tracks, iStationGroup, iStation);
			} // loop over tracks
			// Propagate remaining dTracks
			if (dTracks > 0){
				LitTrack* tracks[fvecLen];
				unsigned int start = fvecLen * nofTracksVec;
				for(unsigned int i = 0; i < dTracks; i++) tracks[i] = fTracks[tracksId1[start + i]];
				// Check if the number of remaining tracks is less than fvecLen.
				if (dTracks < fvecLen) {
					for(unsigned int i = 0; i < fvecLen - dTracks; i++) tracks[dTracks+i] = tracks[dTracks-1];
				}
				ProcessStation(tracks, iStationGroup, iStation);
			}

			for (unsigned int iTrack = 0; iTrack < nofTracks; iTrack++) {
				unsigned int id = tracksId1[iTrack];
				if (fTracks[id]->nofMissingHits <= fMaxNofMissingHits) {
//					std::cout << "mh=" << (int)fTracks[id]->nofMissingHits << std::endl;
					tracksId2.push_back(id);
				}
			}
			tracksId1.assign(tracksId2.begin(), tracksId2.end());
			tracksId2.clear();

		} // loop over stations

	} // loop over station groups
}

void LitTrackFinderNNParallel::PropagateTroughAbsorber(
		LitTrack* tracks[],
		LitAbsorber& absorber)
{
	// Pack track parameters
	LitScalTrackParam par[fvecLen];
	for(unsigned int i = 0; i < fvecLen; i++) par[i] = tracks[i]->paramLast;
	LitTrackParam lpar;
	PackTrackParam(par, lpar);

	// Propagate through the absorber
	LitFieldRegion field;
	LitFieldValue v1, v2;
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
	LitStationGroup& stg = fLayout.stationGroups[stationGroup];
	LitStation& sta = stg.stations[station];
	unsigned char nofSubstations = sta.GetNofSubstations();

	// Pack track parameters
	LitScalTrackParam par[nofSubstations][fvecLen];
	for(unsigned int i = 0; i < fvecLen; i++) par[0][i] = tracks[i]->paramLast;
	LitTrackParam lpar[nofSubstations];
	PackTrackParam(par[0], lpar[0]);

	//Approximate the field between the absorbers
	LitFieldRegion field;
	LitFieldValue v1, v2;
	LitSubstation& ss1 = stg.stations[0].substations[0];
	LitSubstation& ss2 = stg.stations[1].substations[0];
	ss1.fieldSlice.GetFieldValue(lpar[0].X, lpar[0].Y, v1);
	ss2.fieldSlice.GetFieldValue(lpar[0].X, lpar[0].Y, v2);
	field.Set(v1, ss1.fieldSlice.Z, v2, ss2.fieldSlice.Z);

	// Propagate to each substation
	for (unsigned char iSubstation = 0; iSubstation < nofSubstations; iSubstation++) { // loop over substations
		LitSubstation substation = stg.stations[station].substations[iSubstation];
		// Propagation through station
		LitRK4Extrapolation(lpar[iSubstation], substation.Z, field);
		LitAddMaterial(lpar[iSubstation], substation.material);
		if (iSubstation < nofSubstations - 1) lpar[iSubstation + 1] = lpar[iSubstation];
	} // loop over substations

	// Collect hits
//	ScalPixelHitVector hits[fvecLen];
	std::pair<ScalPixelHitIterator, ScalPixelHitIterator> hits[nofSubstations][fvecLen];
	for (unsigned char iSubstation = 0; iSubstation < nofSubstations; iSubstation++) { // loop over substations
		//Unpack data
		UnpackTrackParam(lpar[iSubstation], par[iSubstation]);
		for(unsigned int i = 0; i < fvecLen; i++) tracks[i]->paramLast = par[iSubstation][i];

//		ScalPixelHitIterator first, last;
		ScalPixelHitVector& hitvec = fHitData.GetHits(stationGroup, station, iSubstation);
		fscal err = fHitData.GetMaxErr(stationGroup, station, iSubstation);
		for(unsigned int i = 0; i < fvecLen; i++) { // loop over fvecLen
			MinMaxIndex(&par[iSubstation][i], hitvec, err, hits[iSubstation][i].first, hits[iSubstation][i].second);

//			hits[i].insert(hits[i].end(), first, last);
//			hits[i].insert(hits[i].end(), hitvec.begin(), hitvec.end());
		} // loop over fvecLen
	} // loop over substations

	for(unsigned int i = 0; i < fvecLen; i++) { // loop over fvecLen
		bool hitAdded = false;

		std::pair<ScalPixelHitIterator, ScalPixelHitIterator> ahits[nofSubstations];
		LitScalTrackParam ainpar[nofSubstations];
		for(unsigned char iss = 0; iss < nofSubstations; iss++) {
			ahits[iss] = hits[iss][i];
			ainpar[iss] = par[iss][i];
		}

		if (AddNearestHit(tracks[i], ahits, ainpar, nofSubstations)) hitAdded = true;
		// Check if hit was added, if not than increase number of missing hits
		if (!hitAdded) tracks[i]->nofMissingHits++;
	} // loop over fvecLen
	// end hit collection
}


bool LitTrackFinderNNParallel::AddNearestHit(
		LitTrack* track,
		std::pair<ScalPixelHitIterator, ScalPixelHitIterator> hits[],
		LitScalTrackParam inpar[],
		unsigned char nofSubstations)
{
	bool hitAdded = false;
	ScalPixelHitIterator hita(hits[0].second);
	LitTrackParam param;
	fscal chiSq = 1e10;
	for (unsigned char iSubstation = 0; iSubstation < nofSubstations; iSubstation++) {
		for (ScalPixelHitIterator it = hits[iSubstation].first; it != hits[iSubstation].second; it++) {
			// Pack hit
			LitScalPixelHit hit[] = {*(*it), *(*it), *(*it), *(*it)};
			LitPixelHit lhit;
			PackPixelHit(hit, lhit);
			// Pack track parameters
			LitScalTrackParam par[fvecLen] = {inpar[iSubstation], inpar[iSubstation], inpar[iSubstation], inpar[iSubstation]};
			LitTrackParam lpar;
			PackTrackParam(par, lpar);

			//First update track parameters with KF, than check whether the hit is in the validation gate.
			LitFiltration(lpar, lhit);
			cnst CHISQCUT = 15.;
			fvec chisq = ChiSq(lpar, lhit);
			if (chisq[0] < CHISQCUT[0]) {
				if (chisq[0] < chiSq) {
					chiSq = chisq[0];
					hita = it;
					param = lpar;
				}
			}
		}
	}
	// if hit was attached than change track information
	if (hita != hits[0].second) {
		track->hits.push_back(*hita);
		LitScalTrackParam prm[fvecLen];
		UnpackTrackParam(param, prm);
		track->paramLast = prm[0];
		track->chiSq += chiSq;
		track->NDF = NDF(*track);
		hitAdded = true;
	}
	return hitAdded;
}

#undef cnst
