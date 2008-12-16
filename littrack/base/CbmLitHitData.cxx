#include "CbmLitHitData.h"

#include "CbmLitDetectorLayout.h"
#include "CbmLitHit.h"
#include "CbmLitStripHit.h"
#include "CbmLitPixelHit.h"

#include <sstream>
#include <iostream>

CbmLitHitData::CbmLitHitData()
{
}

CbmLitHitData::~CbmLitHitData()
{
}

void CbmLitHitData::SetDetectorLayout(
		const CbmLitDetectorLayout& layout)
{
	Int_t nofGroups = layout.GetNofStationGroups();
	fHits.resize(nofGroups);
	fMaxErr.resize(nofGroups);
	for(Int_t i = 0; i < nofGroups; i++) {
		Int_t nofStations = layout.GetNofStations(i);
		fHits[i].resize(nofStations);
		fMaxErr[i].resize(nofStations);
		for(Int_t j = 0; j < nofStations; j++) {
			Int_t nofSubstations = layout.GetNofSubstations(i, j);
			fHits[i][j].resize(nofSubstations);
			fMaxErr[i][j].resize(nofSubstations);
			for(Int_t k = 0; k < nofSubstations; k++) {
				fHits[i][j][k].reserve(1500);
			}			
		}
	}	
}

void CbmLitHitData::AddHit(
		Int_t stationGroup,
		Int_t station,
		Int_t substation,
		CbmLitHit* hit)
{
	fHits[stationGroup][station][substation].push_back(hit);
	
	if (hit->GetType() == kLITSTRIPHIT) {
		CbmLitStripHit* stripHit = static_cast<const CbmLitStripHit*>(hit);
		if (fMaxErr[stationGroup][station][substation].first < stripHit->GetDu())
			fMaxErr[stationGroup][station][substation] = std::pair<Double_t, Char_t>(stripHit->GetDu(), 'U');
	} else if (hit->GetType() == kLITPIXELHIT) {
		CbmLitPixelHit* pixelHit = static_cast<const CbmLitPixelHit*>(hit);
		if (fMaxErr[stationGroup][station][substation].first < pixelHit->GetDx())
			fMaxErr[stationGroup][station][substation] = std::pair<Double_t, Char_t>(pixelHit->GetDx(), 'X');
	}
}

void CbmLitHitData::AddHit(
		Int_t planeId,
		CbmLitHit* hit)
{
	Int_t stationGroup;
	Int_t station;
	Int_t substation;
	StationByPlaneId(planeId, stationGroup, station, substation);
	AddHit(stationGroup, station, substation, hit);
//	std::cout << "AddHit planeId=" << planeId << " stationGroup=" << stationGroup 
//		<< " station=" << station << " substation=" << substation << std::endl;
}

const CbmLitHit* CbmLitHitData::GetHit(
		Int_t stationGroup,
		Int_t station,
		Int_t substation,
		Int_t hitId) const
{
	return fHits[stationGroup][station][substation][hitId];
}

const CbmLitHit* CbmLitHitData::GetHit(
		Int_t planeId,
		Int_t hitId) const
{
	Int_t stationGroup;
	Int_t station;
	Int_t substation;
	StationByPlaneId(planeId, stationGroup, station, substation);
	return GetHit(stationGroup, station, substation, hitId);
}

HitPtrIteratorPair CbmLitHitData::GetHits(
		Int_t stationGroup,
		Int_t station,
		Int_t substation)
{
	return HitPtrIteratorPair(
			fHits[stationGroup][station][substation].begin(), 
			fHits[stationGroup][station][substation].end()); 
}

HitPtrIteratorPair CbmLitHitData::GetHits(
		Int_t planeId)
{
	Int_t stationGroup;
	Int_t station;
	Int_t substation;
	StationByPlaneId(planeId, stationGroup, station, substation);
	return GetHits(stationGroup, station, substation);
}

Int_t CbmLitHitData::GetNofHits(
		Int_t stationGroup,
		Int_t station,
		Int_t substation) const
{
	return fHits[stationGroup][station][substation].size();
}

void CbmLitHitData::Clear()
{
	for(Int_t i = 0; i < fHits.size(); i++) {
		for(Int_t j = 0; j < fHits[i].size(); j++) {
			for(Int_t k = 0; k < fHits[i][j].size(); k++){
				fHits[i][j][k].clear();
				fHits[i][j][k].reserve(1500);
				fMaxErr[i][j][k] = std::pair<Double_t, Char_t>(0., ' ');
			}			
		}
	}
}

std::string CbmLitHitData::ToString() const 
{
	std::stringstream ss;
	ss << "HitData:" << std::endl;
	for(Int_t i = 0; i < fHits.size(); i++) {
		ss << " station group " << i << std::endl;
		for(Int_t j = 0; j < fHits[i].size(); j++) {
			ss << "  station " << j << std::endl;
			for(Int_t k = 0; k < fHits[i][j].size(); k++){
				ss << "   substation " << k << ": " << GetNofHits(i, j, k) << " hits, " 
					<< "max err=(" << GetMaxErr(i, j, k).first << "," << GetMaxErr(i, j, k).second
					<< ")" << std::endl; 
			}
		}
	}
	return ss.str();
}

void CbmLitHitData::StationByPlaneId(
		Int_t planeId,
		Int_t& stationGroup,
		Int_t& station,
		Int_t& substation) const
{
	Int_t counter = 0;
	for(Int_t i = 0; i < fHits.size(); i++) {
		for(Int_t j = 0; j < fHits[i].size(); j++) {
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

std::pair<Double_t, Char_t> CbmLitHitData::GetMaxErr(
		Int_t stationGroup,
		Int_t station,
		Int_t substation) const
{
	return fMaxErr[stationGroup][station][substation];
}

std::pair<Double_t, Char_t> CbmLitHitData::GetMaxErr(
		Int_t planeId) const
{
	Int_t stationGroup;
	Int_t station;
	Int_t substation;
	StationByPlaneId(planeId, stationGroup, station, substation);
	return GetMaxErr(stationGroup, station, substation);
}

ClassImp(CbmLitHitData);
