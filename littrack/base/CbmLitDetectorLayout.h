/** CbmLitDetectorLayout.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 1.0
 **
 ** Class represents the detector layout used in the track reconstruction.
 ** The tracking detector consists of station groups.
 ** Each station group consists of stations.
 ** Each station consists of substations.
 **/

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
	/* Constructor */
	CbmLitDetectorLayout();

	/* Destructor */
	virtual ~CbmLitDetectorLayout();

	/* Set the vector with station groups
	 *@param stationGroups Vector with station groups
	 */
	void SetStationGroups(
			const std::vector<CbmLitStationGroup>& stationGroups){
		fStationGroups = stationGroups;
	}

	/* Adds a station group to the list
	 *@param stationGroup Station group to be added
	 */
	void AddStationGroup(
			const CbmLitStationGroup& stationGroup){
		fStationGroups.push_back(stationGroup);
	}

	/* Returns number of station groups in the detector */
	int GetNofStationGroups() const {
		return fStationGroups.size();
	}

	/* Returns i-th station group
	 *@param i Index of the station group in the detector
	 */
	const CbmLitStationGroup& GetStationGroup(int i) const {
		return fStationGroups[i];
	}

	/* Returns number of station in the concrete station group
	 *@param stationGroup Index of the station group
	 */
	int GetNofStations(int stationGroup) const {
		return GetStationGroup(stationGroup).GetNofStations();
	}

	/* Returns station
	 *@param stationGroup Index of the station group
	 *@param station Index of the station in the station group
	 */
	const CbmLitStation& GetStation(int stationGroup, int station) const {
		return GetStationGroup(stationGroup).GetStation(station);
	}

	/* Returns number of substations in the station
	 *@param stationGroup Index of the station group
	 *@param station Index of the station in the station group
	 */
	int GetNofSubstations(int stationGroup, int station) const {
		return GetStationGroup(stationGroup).GetStation(station).GetNofSubstations();
	}

	/* Returns the substations in the station
	 *@param stationGroup Index of the station group
	 *@param station Index of the station in the station group
	 *@param substation Index of the substation in the station
	 */
	const CbmLitSubstation& GetSubstation(int stationGroup, int station, int substation) const {
		return GetStationGroup(stationGroup).GetStation(station).GetSubstation(substation);
	}

	/* Returns total number of substations (detector planes) in the detector */
	int GetNofPlanes() const {
		int nofPlanes = 0;
		for (int i = 0; i < GetNofStationGroups(); i++)
			nofPlanes += GetNofPlanes(i);
		return nofPlanes;
	}

	/*Returns total number of substations (detector planes) in the station group
	 *@param stationGroup Index of the station group
	 */
	int GetNofPlanes(int stationGroup) const {
		int counter = 0;
		for (int i = 0; i < GetNofStations(stationGroup); i++)
			counter += GetNofSubstations(stationGroup, i);
		return counter;
	}

	/*
	 * Returns std::string representation of the class.
	 */
	std::string ToString() const;

private:
	// vector with station groups
    std::vector<CbmLitStationGroup> fStationGroups;
};

#endif /*CBMLITDETECTORLAYOUT_H_*/
