#ifndef CBMLITSTATIONGROUP_H_
#define CBMLITSTATIONGROUP_H_

#include "CbmLitStation.h"

#include <vector>
#include <string>
#include <sstream>

class CbmLitStationGroup
{
public:
	CbmLitStationGroup():fStations(){};
	virtual ~CbmLitStationGroup(){};

	void SetStations(const std::vector<CbmLitStation>& stations){
		fStations = stations;
	}
	void AddStation(const CbmLitStation& station){
		fStations.push_back(station);
	}

	const CbmLitStation& GetStation(int i) const {return fStations[i];}
	int GetNofStations() const {return fStations.size();}

	virtual std::string ToString() const {
		std::stringstream ss;
		ss << "Station group: nofStations=" << GetNofStations() << std::endl;
		return ss.str();
	}

private:
	std::vector<CbmLitStation> fStations;
};

#endif /*CBMLITSTATIONGROUP_H_*/
