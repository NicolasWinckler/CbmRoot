#ifndef CBMLITSTATIONGROUP_H_
#define CBMLITSTATIONGROUP_H_

#include "CbmLitStation.h"

#include "TObject.h"

#include <vector>
#include <string>

class CbmLitStationGroup
{
public:
	CbmLitStationGroup();
	virtual ~CbmLitStationGroup();
	
	void SetStations(const std::vector<CbmLitStation>& stations){
		fStations = stations;
	}
	void AddStation(const CbmLitStation& station){
		fStations.push_back(station);
	}
	
	const CbmLitStation& GetStation(Int_t i) const {return fStations[i];}
	Int_t GetNofStations() const {return fStations.size();}
	
	virtual std::string ToString() const;
	
private:
	std::vector<CbmLitStation> fStations;
	
	ClassDef(CbmLitStationGroup, 1);
};

#endif /*CBMLITSTATIONGROUP_H_*/
