#ifndef CBMLITDETECTORLAYOUT_H_
#define CBMLITDETECTORLAYOUT_H_

#include "CbmLitStationGroup.h"
#include "CbmLitStation.h"
#include "CbmLitSubstation.h"
#include "CbmLitEnums.h"

#include <vector>
#include <string>
#include <sstream>

class CbmLitDetectorLayout
{
public:
	CbmLitDetectorLayout();
	virtual ~CbmLitDetectorLayout();

	void SetStationGroups(const std::vector<CbmLitStationGroup>& stationGroups){
		fStationGroups = stationGroups;
	}
	void AddStationGroup(const CbmLitStationGroup& stationGroup){
		fStationGroups.push_back(stationGroup);
	}

	int GetNofStationGroups() const {return fStationGroups.size();}
	const CbmLitStationGroup& GetStationGroup(int i) const {return fStationGroups[i];}
	int GetNofStations(int stationGroup) const {
		return GetStationGroup(stationGroup).GetNofStations();
	}
	const CbmLitStation& GetStation(int stationGroup, int station) const {
		return GetStationGroup(stationGroup).GetStation(station);
	}
	int GetNofSubstations(int stationGroup, int station) const {
		return GetStationGroup(stationGroup).GetStation(station).GetNofSubstations();
	}
	const CbmLitSubstation& GetSubstation(int stationGroup, int station, int substation) const {
		return GetStationGroup(stationGroup).GetStation(station).GetSubstation(substation);
	}

	int GetNofPlanes() const {
		int nofPlanes = 0;
		for (int i = 0; i < GetNofStationGroups(); i++)
			nofPlanes += GetNofPlanes(i);
		return nofPlanes;
	}
	int GetNofPlanes(int stationGroup) const {
		int counter = 0;
		for (int i = 0; i < GetNofStations(stationGroup); i++)
			counter += GetNofSubstations(stationGroup, i);
		return counter;
	}

	std::string ToString() const;

private:
    std::vector<CbmLitStationGroup> fStationGroups;
};

#endif /*CBMLITDETECTORLAYOUT_H_*/
