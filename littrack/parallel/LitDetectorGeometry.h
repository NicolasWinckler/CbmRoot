/** LitDetectorGeometry.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 *
 * Classe for geometry description for the Littrack parallel
 * version of the tracking.
 **/

#ifndef LITDETECTORGEOMETRY_H_
#define LITDETECTORGEOMETRY_H_

#include "LitTypes.h"
#include "LitMaterialInfo.h"
#include "LitField.h"
#include "CbmLitEnums.h"
#include <iostream>
#include <vector>

class LitSubstation
{
public:
	fvec Z;
	LitMaterialInfo material;
	LitFieldSlice fieldSlice;

	friend std::ostream & operator<<(std::ostream &strm, const LitSubstation &substation ){
		strm << "LitSubstation: " << "Z=" << substation.Z << ", material=" << substation.material;
//		strm << ", fieldSlice=" << substation.fieldSlice;
		return strm;
	}
} _fvecalignment;



class LitStation
{
public:

	void AddSubstation(const LitSubstation& substation) {
		substations.push_back(substation);
	}

	unsigned char GetNofSubstations() const {
		return substations.size();
	}

	// Type of hits on the station
	LitHitType type;
	// array with substations in the station
	std::vector<LitSubstation> substations;

	friend std::ostream & operator<<(std::ostream &strm, const LitStation &station){
		strm << "LitStation: type" << station.type << ", nofSubstations=" << station.GetNofSubstations() << std::endl;
		for (int i = 0; i < station.GetNofSubstations(); i++) {
			strm << "    " << i << station.substations[i];
		}
		return strm;
	}

} _fvecalignment;



class LitAbsorber
{
public:
	fvec Z;
	LitMaterialInfo material;
	LitFieldSlice fieldSliceFront;
	LitFieldSlice fieldSliceBack;

	friend std::ostream & operator<<(std::ostream &strm, const LitAbsorber &absorber){
		strm << "LitAbsorber: Z" << absorber.Z << ", material=" << absorber.material;
//		strm << "fieldSliceFront=" << absorber.fieldSliceFront
//			<< " fieladSliceBack=" << absorber.fieldSliceBack;
		return strm;
	}
} _fvecalignment;



class LitStationGroup
{
public:
	LitStationGroup() {}

	virtual ~LitStationGroup() {}

	void AddStation(const LitStation& station) {
		stations.push_back(station);
	}

	unsigned char GetNofStations() const {
		return stations.size();
	}

	std::vector<LitStation> stations;
	LitAbsorber absorber;

	friend std::ostream & operator<<(std::ostream &strm, const LitStationGroup &stationGroup){
		strm << "LitStationGroup: " << "nofStations=" << stationGroup.GetNofStations() << std::endl;
		for (unsigned char i = 0; i < stationGroup.GetNofStations(); i++) {
			strm << "  " << i << stationGroup.stations[i];
		}
		strm << "  " << stationGroup.absorber;
		return strm;
	}
} _fvecalignment;



class LitDetectorLayout
{
public:

	void AddStationGroup(const LitStationGroup& stationGroup) {
		stationGroups.push_back(stationGroup);
	}

	unsigned char GetNofStationGroups() const {
		return stationGroups.size();
	}

	unsigned char GetNofStations(unsigned char stationGroup) const {
		return stationGroups[stationGroup].GetNofStations();
	}

	unsigned char GetNofSubstations(unsigned char stationGroup, unsigned char station) const {
		return stationGroups[stationGroup].stations[station].GetNofSubstations();
	}

	const LitStationGroup& GetStationGroup(unsigned char stationGroup) {
		return stationGroups[stationGroup];
	}

	const LitStation& GetStation(unsigned char stationGroup, unsigned char station) {
		return stationGroups[stationGroup].stations[station];
	}

	const LitSubstation& GetSubstation(unsigned char stationGroup, unsigned char station, unsigned char substation){
		return stationGroups[stationGroup].stations[station].substations[substation];
	}

	// array with station groups
    std::vector<LitStationGroup> stationGroups;

	friend std::ostream & operator<<(std::ostream &strm, const LitDetectorLayout &layout){
		strm << "LitDetectorLayout: " << "nofStationGroups=" << layout.GetNofStationGroups() << std::endl;
		for (unsigned char i = 0; i < layout.GetNofStationGroups(); i++) {
			strm << i << layout.stationGroups[i];
		}
		return strm;
	}
} _fvecalignment;

#endif
