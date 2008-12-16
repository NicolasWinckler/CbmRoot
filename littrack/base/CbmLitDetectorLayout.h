#ifndef CBMLITDETECTORLAYOUT_H_
#define CBMLITDETECTORLAYOUT_H_

#include "CbmLitStationGroup.h"
#include "CbmLitStation.h"
#include "CbmLitSubstation.h"
#include "CbmLitEnums.h"

#include "TObject.h"

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
	
	Int_t GetNofStationGroups() const {return fStationGroups.size();}
	const CbmLitStationGroup& GetStationGroup(Int_t i) const {return fStationGroups[i];}
	Int_t GetNofStations(Int_t stationGroup) const { 
		return GetStationGroup(stationGroup).GetNofStations(); 
	}
	const CbmLitStation& GetStation(Int_t stationGroup, Int_t station) const {
		return GetStationGroup(stationGroup).GetStation(station);
	}
	Int_t GetNofSubstations(Int_t stationGroup, Int_t station) const { 
		return GetStationGroup(stationGroup).GetStation(station).GetNofSubstations(); 
	}
	const CbmLitSubstation& GetSubstation(Int_t stationGroup, Int_t station, Int_t substation) const {
		return GetStationGroup(stationGroup).GetStation(station).GetSubstation(substation);
	}
	
	Int_t GetNofPlanes() const {
		Int_t nofPlanes = 0;
		for (Int_t i = 0; i < GetNofStationGroups(); i++) 
			nofPlanes += GetNofPlanes(i);
		return nofPlanes;   
	}
	Int_t GetNofPlanes(Int_t stationGroup) const {
		Int_t counter = 0;
		for (Int_t i = 0; i < GetNofStations(stationGroup); i++)
			counter += GetNofSubstations(stationGroup, i);
		return counter;
	}
	
	virtual std::string ToString() const;
	
private:
    std::vector<CbmLitStationGroup> fStationGroups;
	
	ClassDef(CbmLitDetectorLayout, 1);
};

#endif /*CBMLITDETECTORLAYOUT_H_*/
