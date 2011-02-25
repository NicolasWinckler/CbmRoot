/** CbmLitStationGroup.h
 *@author A.Lebedev <andrey.lebedev@gsi.de>
 *@since 2008
 *
 * Class represents detector station group.
 **/

#ifndef CBMLITSTATIONGROUP_H_
#define CBMLITSTATIONGROUP_H_

#include "base/CbmLitStation.h"

#include <vector>
#include <string>
#include <sstream>

class CbmLitStationGroup
{
public:
	/* Constructor */
	CbmLitStationGroup():fStations(){};

	/* Destructor */
	virtual ~CbmLitStationGroup(){};

	/* Sets vector of stations. */
	void SetStations(const std::vector<CbmLitStation>& stations){
		fStations = stations;
	}

	/* Adds station to station group. */
	void AddStation(const CbmLitStation& station){
		fStations.push_back(station);
	}

	/* Returns i-th station. */
	const CbmLitStation& GetStation(int i) const {return fStations[i];}

	/* Returns number of stations. */
	int GetNofStations() const {return fStations.size();}

	/* Returns std::string representation of the class. */
	virtual std::string ToString() const {
		std::stringstream ss;
		ss << "Station group: nofStations=" << GetNofStations() << std::endl;
		return ss.str();
	}

private:
	std::vector<CbmLitStation> fStations; // Vector of stations
};

#endif /*CBMLITSTATIONGROUP_H_*/
