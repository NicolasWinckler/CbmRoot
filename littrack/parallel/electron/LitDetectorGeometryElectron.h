/** LitDetectorGeometryElectron.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2010
 * @version 1.0
 *
 * Classes for geometry description for "electron" setup
 * of CBM in the Littrack parallel version of the tracking.
 **/

#ifndef LITDETECTORGEOMETRYELECTRON_H_
#define LITDETECTORGEOMETRYELECTRON_H_

#include "../LitTypes.h"
#include "../LitMaterialInfo.h"
#include "../LitField.h"
//#include "../base/CbmLitEnums.h"
#include "../../utils/CbmLitUtils.h"

const unsigned char MAX_NOF_STATION_GROUPS_ELECTRON = 6;
const unsigned char MAX_NOF_STATIONS_ELECTRON = 4;
const unsigned char MAX_NOF_VIRTUAL_PLANES_ELECTRON = 50;
const unsigned char MAX_NOF_MATERIAL_LAYERS_PER_STATION_BEFORE = 3;
const unsigned char MAX_NOF_MATERIAL_LAYERS_PER_STATION_AFTER = 3;

template<class T>
class LitStationElectron
{
public:
	LitStationElectron():
		nofMaterialsBefore(0),
		nofMaterialsAfter(0),
		Z(0.){}

	virtual ~LitStationElectron() {};

	T Z;
	LitMaterialInfo<T> materialsBefore[MAX_NOF_MATERIAL_LAYERS_PER_STATION_BEFORE];
	LitMaterialInfo<T> materialsAfter[MAX_NOF_MATERIAL_LAYERS_PER_STATION_AFTER];

	void AddMaterialBefore(const LitMaterialInfo<T>& material) {
		materialsBefore[nofMaterialsBefore++] = material;
	}

	void AddMaterialAfter(const LitMaterialInfo<T>& material) {
		materialsAfter[nofMaterialsAfter++] = material;
	}

	unsigned char GetNofMaterialsBefore() const {
		return nofMaterialsBefore;
	}

	unsigned char GetNofMaterialsAfter() const {
		return nofMaterialsAfter;
	}

	friend std::ostream & operator<<(std::ostream &strm, const LitStationElectron &station ){
		strm << "LitStationElectron: " << "Z=" << station.Z << std::endl;
		strm << "   materials before:" << std::endl;
		for (unsigned char i = 0; i < station.GetNofMaterialsBefore(); i++)
			strm << "      " << (int)i << " " << station.materialsBefore[i];
		strm << "   materials after:" << std::endl;
		for (unsigned char i = 0; i < station.GetNofMaterialsAfter(); i++)
			strm << "      " << (int)i << " " << station.materialsAfter[i];
		return strm;
	}

	std::string ToStringShort() {
		std::string str = ToString<T>(Z) + "\n";
		str += ToString<T>(GetNofMaterialsBefore) + "\n";
		for (unsigned char i = 0; i < GetNofMaterialsBefore(); i++)
			str += materialsBefore[i].ToStringShort() + "\n";
		str += ToString<T>(GetNofMaterialsAfter) + "\n";
		for (unsigned char i = 0; i < GetNofMaterialsAfter(); i++)
			str += materialsAfter[i].ToStringShort() + "\n";
		return str;
	}

private:
	unsigned char nofMaterialsBefore;
	unsigned char nofMaterialsAfter;

} _fvecalignment;

typedef LitStationElectron<fvec> LitStationElectronVec;
typedef LitStationElectron<fscal> LitStationElectronScal;



template<class T>
class LitVirtualPlaneElectron
{
public:
	LitVirtualPlaneElectron():
		Z(0),
		material(),
		fieldSlice(),
		fieldSliceMid(){}

	virtual ~LitVirtualPlaneElectron() {}

	T Z;
	LitMaterialInfo<T> material;
	LitFieldSlice<T> fieldSlice;
	LitFieldSlice<T> fieldSliceMid;

	friend std::ostream & operator<<(std::ostream &strm, const LitVirtualPlaneElectron &plane){
		strm << "LitVirtualPlaneElectron: Z=" << plane.Z << ", material=" << plane.material;
	//	strm << "fieldSlice=" << plane.fieldSlice;
	//	strm << "fieldSliceMid=" << plane.fieldSliceMid;
//		strm << std::endl;
		return strm;
	}

	std::string ToStringShort() {
		std::string str = ToString<T>(Z) + "\n" + material.ToStringShort();
		str += fieldSlice.ToStringShort();
		str += fieldSliceMid.ToStringShort();
		return str;
	}

} _fvecalignment;

typedef LitVirtualPlaneElectron<fvec> LitVirtualPlaneElectronVec;
typedef LitVirtualPlaneElectron<fscal> LitVirtualPlaneElectronScal;



template<class T>
class LitStationGroupElectron
{
public:
	LitStationGroupElectron():
		nofStations(0) {}

	virtual ~LitStationGroupElectron() {}

	void AddStation(const LitStationElectron<T>& station) {
		stations[nofStations++] = station;
	}

	unsigned char GetNofStations() const {
		return nofStations;
	}

	// array with stations in the station group
	LitStationElectron<T> stations[MAX_NOF_STATIONS_ELECTRON];

	friend std::ostream & operator<<(std::ostream &strm, const LitStationGroupElectron &stationGroup){
		strm << "LitStationGroupElectron: " << "nofStations=" << (int) stationGroup.GetNofStations() << std::endl;
		for (unsigned char i = 0; i < stationGroup.GetNofStations(); i++) {
			strm << "  " << (int) i << " " << stationGroup.stations[i];
		}
		return strm;
	}

	std::string ToStringShort() {
		std::string str = ToString<int>(GetNofStations()) + "\n";
		for (unsigned char i = 0; i < GetNofStations(); i++) {
//			str += "station\n";
			str += ToString<int>(i) + "\n" + stations[i].ToStringShort();
		}
		return str;
	}

private:
	// number of stations in the station group
	unsigned char nofStations;

} _fvecalignment;

typedef LitStationGroupElectron<fvec> LitStationGroupElectronVec;
typedef LitStationGroupElectron<fscal> LitStationGroupElectronScal;



template<class T>
class LitDetectorLayoutElectron
{
public:
	LitDetectorLayoutElectron():nofStationGroups(0), nofVirtualPlanes(0){};

	virtual ~LitDetectorLayoutElectron() {}


	void AddStationGroup(const LitStationGroupElectron<T>& stationGroup) {
		stationGroups[nofStationGroups++] = stationGroup;
	}

	void AddVirtualPlane(const LitVirtualPlaneElectron<T>& plane) {
		virtualPlanes[nofVirtualPlanes++] = plane;
	}

	unsigned char GetNofStationGroups() const {
		return nofStationGroups;
	}

	unsigned char GetNofStations(unsigned char stationGroup) const {
		return stationGroups[stationGroup].GetNofStations();
	}

	unsigned char GetNofVirtualPlanes() const {
		return nofVirtualPlanes;
	}

	const LitStationGroupElectron<T>& GetStationGroup(unsigned char stationGroup) {
		return stationGroups[stationGroup];
	}

	const LitStationElectron<T>& GetStation(unsigned char stationGroup, unsigned char station) {
		return stationGroups[stationGroup].stations[station];
	}

	const LitVirtualPlaneElectron<T>& GetVirtualPlane(unsigned char planeId) {
		return stationGroups[planeId];
	}

	// array with station groups
    LitStationGroupElectron<T> stationGroups[MAX_NOF_STATION_GROUPS_ELECTRON];

	// array with virtual planes
	LitVirtualPlaneElectron<T> virtualPlanes[MAX_NOF_VIRTUAL_PLANES_ELECTRON];

	friend std::ostream & operator<<(std::ostream &strm, const LitDetectorLayoutElectron &layout){
		strm << "LitDetectorLayoutElectron: " << std::endl;
		strm << "   virtual planes: nofVirtualPlanes=" << (int)layout.GetNofVirtualPlanes() << std::endl;
		for (unsigned char i = 0; i < layout.GetNofVirtualPlanes(); i++) {
			strm << (int) i << " " << layout.virtualPlanes[i];
		}
		strm << "   station groups: nofStationGroups=" << (int)layout.GetNofStationGroups() << std::endl;
		for (unsigned char i = 0; i < layout.GetNofStationGroups(); i++) {
			strm << "      " << (int) i << " " << layout.stationGroups[i];
		}
		return strm;
	}

	std::string ToStringShort() {
		std::string str = ToString<int>(GetNofVirtualPlanes()) + "\n";
		for (unsigned char i = 0; i < GetNofVirtualPlanes(); i++) {
		//	str += "virtual planes\n";
			str += ToString<int>(i) + "\n" + virtualPlanes[i].ToStringShort();
		}
		str = ToString<int>(GetNofStationGroups()) + "\n";
		for (unsigned char i = 0; i < GetNofStationGroups(); i++) {
//			str += "station group\n";
			str += ToString<int>(i) + "\n" + stationGroups[i].ToStringShort();
		}
		return str;
	}

private:

    //number of station groups
    unsigned char nofStationGroups;

	// number of virtual planes
	unsigned char nofVirtualPlanes;

} _fvecalignment;

typedef LitDetectorLayoutElectron<fvec> LitDetectorLayoutElectronVec;
typedef LitDetectorLayoutElectron<fscal> LitDetectorLayoutElectronScal;

#endif
