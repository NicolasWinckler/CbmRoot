/** LitDetectorGeometryMuon.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 *
 * Classes for geometry description of the 'muon'
 * setup of CBM.
 **/

#ifndef LITDETECTORGEOMETRYMUON_H_
#define LITDETECTORGEOMETRYMUON_H_

#include "../LitTypes.h"
#include "../LitMaterialInfo.h"
#include "../LitField.h"
#include "../../base/CbmLitEnums.h"
#include "../../utils/CbmLitUtils.h"

const unsigned char MAX_NOF_STATION_GROUPS = 6;
const unsigned char MAX_NOF_STATIONS = 4;
const unsigned char MAX_NOF_SUBSTATIONS = 2;

template<class T>
class LitSubstationMuon
{
public:
	T Z;
	LitMaterialInfo<T> material;
	LitFieldSlice<T> fieldSlice;

	friend std::ostream & operator<<(std::ostream &strm, const LitSubstationMuon &substation ){
		strm << "LitSubstationMuon: " << "Z=" << substation.Z << ", material=" << substation.material;
//		strm << ", fieldSlice=" << substation.fieldSlice;
		return strm;
	}

	std::string ToStringShort() {
		std::string str = ToString<T>(Z) + "\n" + material.ToStringShort();
		str += fieldSlice.ToStringShort();
		return str;
	}
} _fvecalignment;

typedef LitSubstationMuon<fvec> LitSubstationMuonVec;
typedef LitSubstationMuon<fscal> LitSubstationMuonScal;



template<class T>
class LitStationMuon
{
public:
	LitStationMuon():type(kLITPIXELHIT), nofSubstations(0) {}

	void AddSubstation(const LitSubstationMuon<T>& substation) {
		substations[nofSubstations++] = substation;
	}

	unsigned char GetNofSubstations() const {
		return nofSubstations;
	}

	// Type of hits on the station
	LitHitType type;
	// array with substations in the station
	LitSubstationMuon<T> substations[MAX_NOF_SUBSTATIONS];
	// number of substations
	unsigned char nofSubstations;

	friend std::ostream & operator<<(std::ostream &strm, const LitStationMuon &station){
		strm << "LitStationMuon: type=" << station.type << ", nofSubstations=" << (int) station.GetNofSubstations() << std::endl;
		for (unsigned char i = 0; i < station.GetNofSubstations(); i++) {
			strm << "    " << (int) i << " " << station.substations[i];
		}
		return strm;
	}

	std::string ToStringShort() {
		std::string str = type + " " + ToString<int>(GetNofSubstations()) + "\n";
		for (unsigned char i = 0; i < GetNofSubstations(); i++) {
//			str += "substation\n";
			str += ToString<int>(i) + "\n" + substations[i].ToStringShort();
		}
		return str;
	}

} _fvecalignment;

typedef LitStationMuon<fvec> LitStationMuonVec;
typedef LitStationMuon<fscal> LitStationMuonScal;



template<class T>
class LitAbsorber
{
public:
	T Z;
	LitMaterialInfo<T> material;
	LitFieldSlice<T> fieldSliceFront;
	LitFieldSlice<T> fieldSliceBack;

	friend std::ostream & operator<<(std::ostream &strm, const LitAbsorber &absorber){
		strm << "LitAbsorber: Z=" << absorber.Z << ", material=" << absorber.material;
//		strm << "fieldSliceFront=" << absorber.fieldSliceFront
//			<< " fieladSliceBack=" << absorber.fieldSliceBack;
		return strm;
	}

	std::string ToStringShort() {
		std::string str = ToString<T>(Z) + "\n" + material.ToStringShort();
		str += fieldSliceFront.ToStringShort();
		str += fieldSliceBack.ToStringShort();
		return str;
	}

} _fvecalignment;

typedef LitAbsorber<fvec> LitAbsorberVec;
typedef LitAbsorber<fscal> LitAbsorberScal;



template<class T>
class LitStationGroupMuon
{
public:
	LitStationGroupMuon():nofStations(0) {}

	virtual ~LitStationGroupMuon() {}

	void AddStation(const LitStationMuon<T>& station) {
		stations[nofStations++] = station;
	}

	unsigned char GetNofStations() const {
		return nofStations;
	}

	// array with stations in the station group
	LitStationMuon<T> stations[MAX_NOF_STATIONS];
	// number of stations in the station group
	unsigned char nofStations;
	// absorber
	LitAbsorber<T> absorber;

	friend std::ostream & operator<<(std::ostream &strm, const LitStationGroupMuon &stationGroup){
		strm << "LitStationGroupMuon: " << "nofStations=" << (int) stationGroup.GetNofStations() << std::endl;
		for (unsigned char i = 0; i < stationGroup.GetNofStations(); i++) {
			strm << "  " << (int) i << " " << stationGroup.stations[i];
		}
		strm << "  " << stationGroup.absorber;
		return strm;
	}

	std::string ToStringShort() {
		std::string str = ToString<int>(GetNofStations()) + "\n";
		for (unsigned char i = 0; i < GetNofStations(); i++) {
//			str += "station\n";
			str += ToString<int>(i) + "\n" + stations[i].ToStringShort();
		}
//		str += "absorber\n";
		str += absorber.ToStringShort();
		return str;
	}

} _fvecalignment;

typedef LitStationGroupMuon<fvec> LitStationGroupMuonVec;
typedef LitStationGroupMuon<fscal> LitStationGroupMuonScal;



template<class T>
class LitDetectorLayoutMuon
{
public:
	LitDetectorLayoutMuon():nofStationGroups(0){};

	void AddStationGroup(const LitStationGroupMuon<T>& stationGroup) {
		stationGroups[nofStationGroups++] = stationGroup;
	}

	unsigned char GetNofStationGroups() const {
		return nofStationGroups;
	}

	unsigned char GetNofStations(unsigned char stationGroup) const {
		return stationGroups[stationGroup].GetNofStations();
	}

	unsigned char GetNofSubstations(unsigned char stationGroup, unsigned char station) const {
		return stationGroups[stationGroup].stations[station].GetNofSubstations();
	}

	const LitStationGroupMuon<T>& GetStationGroup(unsigned char stationGroup) {
		return stationGroups[stationGroup];
	}

	const LitStationMuon<T>& GetStation(unsigned char stationGroup, unsigned char station) {
		return stationGroups[stationGroup].stations[station];
	}

	const LitSubstationMuon<T>& GetSubstation(unsigned char stationGroup, unsigned char station, unsigned char substation){
		return stationGroups[stationGroup].stations[station].substations[substation];
	}

	// array with station groups
    LitStationGroupMuon<T> stationGroups[MAX_NOF_STATION_GROUPS];
    //number of station groups
    unsigned char nofStationGroups;

	friend std::ostream & operator<<(std::ostream &strm, const LitDetectorLayoutMuon &layout){
		strm << "LitDetectorLayoutMuon: " << "nofStationGroups=" << (int)layout.GetNofStationGroups() << std::endl;
		for (unsigned char i = 0; i < layout.GetNofStationGroups(); i++) {
			strm << (int) i << " " << layout.stationGroups[i];
		}
		return strm;
	}

	std::string ToStringShort() {
		std::string str = ToString<int>(GetNofStationGroups()) + "\n";
		for (unsigned char i = 0; i < GetNofStationGroups(); i++) {
//			str += "station group\n";
			str += ToString<int>(i) + "\n" + stationGroups[i].ToStringShort();
		}
		return str;
	}
} _fvecalignment;

typedef LitDetectorLayoutMuon<fvec> LitDetectorLayoutMuonVec;
typedef LitDetectorLayoutMuon<fscal> LitDetectorLayoutMuonScal;

#endif
