/** CbmLitHitData.cxx
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 1.0
 **/

#include "base/CbmLitHitData.h"

#include "base/CbmLitDetectorLayout.h"
#include "data/CbmLitHit.h"
#include "data/CbmLitStripHit.h"
#include "data/CbmLitPixelHit.h"

#include <sstream>
#include <iostream>

CbmLitHitData::CbmLitHitData():
	fHits(),
	fMaxErr()
{
}

CbmLitHitData::~CbmLitHitData()
{
}

void CbmLitHitData::SetDetectorLayout(
		const CbmLitDetectorLayout& layout)
{
	int nofGroups = layout.GetNofStationGroups();
	fHits.resize(nofGroups);
	fMaxErr.resize(nofGroups);
	for(int i = 0; i < nofGroups; i++) {
		int nofStations = layout.GetNofStations(i);
		fHits[i].resize(nofStations);
		fMaxErr[i].resize(nofStations);
		for(int j = 0; j < nofStations; j++) {
			int nofSubstations = layout.GetNofSubstations(i, j);
			fHits[i][j].resize(nofSubstations);
			fMaxErr[i][j].resize(nofSubstations);
			for(int k = 0; k < nofSubstations; k++) {
				fHits[i][j][k].reserve(1500);
			}
		}
	}
}

void CbmLitHitData::AddHit(
		int stationGroup,
		int station,
		int substation,
		CbmLitHit* hit)
{
	fHits[stationGroup][station][substation].push_back(hit);

	if (hit->GetType() == kLITSTRIPHIT) {
		CbmLitStripHit* stripHit = static_cast<const CbmLitStripHit*>(hit);
		if (fMaxErr[stationGroup][station][substation].first < stripHit->GetDu())
			fMaxErr[stationGroup][station][substation] = std::pair<myf, char>(stripHit->GetDu(), 'U');
	} else if (hit->GetType() == kLITPIXELHIT) {
		CbmLitPixelHit* pixelHit = static_cast<const CbmLitPixelHit*>(hit);
		if (fMaxErr[stationGroup][station][substation].first < pixelHit->GetDx())
			fMaxErr[stationGroup][station][substation] = std::pair<myf, char>(pixelHit->GetDx(), 'X');
	}
}

void CbmLitHitData::AddHit(
		int planeId,
		CbmLitHit* hit)
{
	int stationGroup;
	int station;
	int substation;
	StationByPlaneId(planeId, stationGroup, station, substation);
	AddHit(stationGroup, station, substation, hit);
}

const CbmLitHit* CbmLitHitData::GetHit(
		int stationGroup,
		int station,
		int substation,
		int hitId) const
{
	return fHits[stationGroup][station][substation][hitId];
}

const CbmLitHit* CbmLitHitData::GetHit(
		int planeId,
		int hitId) const
{
	int stationGroup;
	int station;
	int substation;
	StationByPlaneId(planeId, stationGroup, station, substation);
	return GetHit(stationGroup, station, substation, hitId);
}

HitPtrIteratorPair CbmLitHitData::GetHits(
		int stationGroup,
		int station,
		int substation)
{
	return HitPtrIteratorPair(
			fHits[stationGroup][station][substation].begin(),
			fHits[stationGroup][station][substation].end());
}

HitPtrIteratorPair CbmLitHitData::GetHits(
		int planeId)
{
	int stationGroup;
	int station;
	int substation;
	StationByPlaneId(planeId, stationGroup, station, substation);
	return GetHits(stationGroup, station, substation);
}

int CbmLitHitData::GetNofHits(
		int stationGroup,
		int station,
		int substation) const
{
	return fHits[stationGroup][station][substation].size();
}

void CbmLitHitData::Clear()
{
	for(unsigned int i = 0; i < fHits.size(); i++) {
		for(unsigned int j = 0; j < fHits[i].size(); j++) {
			for(unsigned int k = 0; k < fHits[i][j].size(); k++){
				fHits[i][j][k].clear();
				fHits[i][j][k].reserve(1500);
				fMaxErr[i][j][k] = std::pair<myf, char>(0., ' ');
			}
		}
	}
}

std::string CbmLitHitData::ToString() const
{
	std::stringstream ss;
	ss << "HitData:" << std::endl;
	for(unsigned int i = 0; i < fHits.size(); i++) {
		ss << " station group " << i << std::endl;
		for(unsigned int j = 0; j < fHits[i].size(); j++) {
			ss << "  station " << j << std::endl;
			for(unsigned int k = 0; k < fHits[i][j].size(); k++){
				ss << "   substation " << k << ": " << GetNofHits(i, j, k) << " hits, "
					<< "max err=(" << GetMaxErr(i, j, k).first << "," << GetMaxErr(i, j, k).second
					<< ")" << std::endl;
			}
		}
	}
	return ss.str();
}

void CbmLitHitData::StationByPlaneId(
		int planeId,
		int& stationGroup,
		int& station,
		int& substation) const
{
	int counter = 0;
	for(unsigned int i = 0; i < fHits.size(); i++) {
		for(unsigned int j = 0; j < fHits[i].size(); j++) {
			counter += fHits[i][j].size();
			if (counter > planeId) {
				stationGroup = i;
				station = j;
				substation = fHits[i][j].size() - (counter - planeId);
				return;
			}
		}
	}
}

std::pair<myf, char> CbmLitHitData::GetMaxErr(
		int stationGroup,
		int station,
		int substation) const
{
	return fMaxErr[stationGroup][station][substation];
}

std::pair<myf, char> CbmLitHitData::GetMaxErr(
		int planeId) const
{
	int stationGroup;
	int station;
	int substation;
	StationByPlaneId(planeId, stationGroup, station, substation);
	return GetMaxErr(stationGroup, station, substation);
}
