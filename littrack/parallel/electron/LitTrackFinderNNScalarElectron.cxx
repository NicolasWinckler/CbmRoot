#include "LitTrackFinderNNScalarElectron.h"

#include "../LitTypes.h"
#include "../LitHit.h"
#include "../LitAddMaterial.h"
#include "../LitExtrapolation.h"
#include "../LitFiltration.h"
#include "../LitField.h"
#include "../LitMath.h"

#include "CbmLitEnvironment.h"
//#include "CbmLitMapField.h"

#include <algorithm>
#include <iostream>
#include <limits>

LitTrackFinderNNScalarElectron::LitTrackFinderNNScalarElectron():
	fMaxNofMissingHits(2),
	fSigmaCoef(3.5),
	fMaxCovSq(20.*20.)
{
//	CbmLitEnvironment* env = CbmLitEnvironment::Instance();
//	fField = new CbmLitMapField(env->GetField());
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
//		std::cout << *track;
//		if (track->nofHits < 11) continue;
//		std::cout << *track;
		tracks[nofTracks++] = new LitScalTrack(*track);
	}

	for (unsigned int i = 0; i < nofTracks; i++)
		std::cout << *tracks[i];

	std::cout << "Number of found tracks: " << nofTracks << std::endl;

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
			unsigned int nh = fHitData.GetNofHits(i, j);
			LitScalPixelHit** shits = fHitData.GetHits(i, j);
			LitScalPixelHit** begin = &shits[0];
			LitScalPixelHit** end = &shits[0] + fHitData.GetNofHits(i, j);

			std::sort(begin, end, ComparePixelHitXLess());
    	}
    }

    std::cout << fHitData;
}

void LitTrackFinderNNScalarElectron::MinMaxIndex(
		const LitTrackParamScal* par,
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

void LitTrackFinderNNScalarElectron::InitTrackSeeds(
		LitScalTrack* trackSeeds[],
		unsigned int nofTrackSeeds)
{
//	tbb::parallel_for(tbb::blocked_range<unsigned int>(0, seeds.size()),
//	    				InitTrackSeedsClass(seeds, fTracks), tbb::auto_partitioner());

	fNofTracks = 0;
//	fscal QpCut = 1./1.5;
	//TODO : add threads here
	for (unsigned int i = 0; i < nofTrackSeeds; i++) {
		LitScalTrack* track = trackSeeds[i];
//		if (fabs(track->paramLast.Qp) > QpCut) continue;
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
	for (unsigned int iTrack = 0; iTrack < fNofTracks; iTrack++) {
		LitScalTrack* track = fTracks[iTrack];
		PropagateToFirstStation(track);
		FollowTrack(track);
	}
}

void LitTrackFinderNNScalarElectron::PropagateToFirstStation(
		LitScalTrack* track)
{
    for (unsigned char ivp = 0; ivp < fLayout.GetNofVirtualPlanes()-1; ivp++) {
    	LitVirtualPlaneElectron<fscal>& vp1 = fLayout.virtualPlanes[ivp];
    	LitVirtualPlaneElectron<fscal>& vp2 = fLayout.virtualPlanes[ivp+1];

    	LitRK4ExtrapolationElectron(track->paramLast, vp2.Z, vp1.fieldSlice, vp1.fieldSliceMid, vp2.fieldSlice);
//    	LitRK4ExtrapolationElectron(track->paramLast, vp2.Z, fField);
    	LitAddMaterial(track->paramLast, vp2.material);
    }
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
//	track->SetPDG(fPDG);
//	track->SetLastPlaneId(stationGroup);
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

	LitLineExtrapolation(par, st.Z);

	for (unsigned char im = 0; im < st.GetNofMaterialsBefore(); im++)
		LitAddMaterial(par, st.materialsBefore[im]);

	std::pair<unsigned int, unsigned int> hits;
	LitScalPixelHit** hitvec = fHitData.GetHits(stationGroup, station);
	unsigned int nh = fHitData.GetNofHits(stationGroup, station);
	fscal err = fHitData.GetMaxErr(stationGroup, station);

	MinMaxIndex(&par, hitvec, nh, err, hits.first, hits.second);

	unsigned int nofHits = hits.second - hits.first;
	hitAdded = AddNearestHit(track, hits, &par, nofHits, stationGroup, station);

	for (unsigned char im = 0; im < st.GetNofMaterialsAfter(); im++)
		LitAddMaterial(par, st.materialsAfter[im]);

	return hitAdded;
}

bool LitTrackFinderNNScalarElectron::AddNearestHit(
		LitScalTrack* track,
		const std::pair<unsigned int, unsigned int>& hits,
		LitTrackParamScal* par,
		unsigned int nofHits,
		int stationGroup,
		int station)
{
	bool hitAdded = false;
	LitScalPixelHit* hita = NULL;
	LitTrackParamScal param;
	fscal chiSq = std::numeric_limits<fscal>::max();

	LitScalPixelHit** hitvec = fHitData.GetHits(stationGroup, station);
	std::cout << "sg=" << stationGroup << " st=" << station
		<< " nofHits=" << nofHits << "" << std::endl;
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
		static const fscal CHISQCUT = 20.;
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
