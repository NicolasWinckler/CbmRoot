/** LitHitDataElectron.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2010
 * @version 1.0
 **
 ** Class for accessing the hits in the track reconstruction.
 **/

#ifndef LITHITDATAELECTRON_H_
#define LITHITDATAELECTRON_H_

#include "../LitHit.h"
#include "LitDetectorGeometryElectron.h"

// Maximum number of hits for station
const unsigned int MAX_NOF_HITS_ELECTRON = 2000;

template<class T>
class LitHitDataElectron
{
public:
	/* Constructor */
	LitHitDataElectron() {
		for(int i = 0; i < MAX_NOF_STATION_GROUPS_ELECTRON; i++) {
			for(int j = 0; j < MAX_NOF_STATIONS_ELECTRON; j++) {
				fNofHits[i][j] = 0;
				fMaxErr[i][j] = 0.;
			}
		}
	};

	/* Destructor */
	virtual ~LitHitDataElectron(){};

	/* Sets the detector layout for which the hits are arranged
	 *@param layout Detector layout
	 */
	void SetDetectorLayout(
			const LitDetectorLayoutElectron<T>& layout){
		fLayout = layout;
	}

	/* Adds the hit using station group, station and substation indices
	 *@param stationGroup Index of the station group in the detector
	 *@param station Index of the station in the station group
	 *@param hit Pointer to the hit to be added
	 */
	void AddHit(
			int stationGroup,
			int station,
			LitScalPixelHit* hit) {
		unsigned int &nofHits = fNofHits[stationGroup][station];
		fHits[stationGroup][station][nofHits++] = hit;
		if (fMaxErr[stationGroup][station] < hit->Dx)
			fMaxErr[stationGroup][station] = hit->Dx;
	}

	/*Adds the hit using absolute detector plane (station) index in the detector
	 *@param planeId Index of the detector plane (station) in the detector
	 *@param hit Pointer to the hit to be added
	 */
	void AddHit(
			unsigned char planeId,
			LitScalPixelHit* hit) {
		unsigned char stationGroup;
		unsigned char station;
		StationByPlaneId(planeId, stationGroup, station);
		AddHit(stationGroup, station, hit);
	}

	/* Returns the hit using station group and station indices
	 *@param stationGroup Index of the station group in the detector
	 *@param station Index of the station in the station group
	 *@param hitId Hit index in the array of hits for the specified station
	 *@return Hit pointer
	 */
	const LitScalPixelHit* GetHit(
			int stationGroup,
			int station,
			int hitId) const {
		return fHits[stationGroup][station][hitId];
	}

	/* Returns hit iterators using station group and station indices
	 *@param stationGroup Index of the station group in the detector
	 *@param station Index of the station in the station group
	 *@return Hit iterators
	 */
	LitScalPixelHit** GetHits(
			int stationGroup,
			int station) {
		return fHits[stationGroup][station];
	}

	/* Returns number of hits for the specified station
	 *@param stationGroup Index of the station group in the detector
	 *@param station Index of the station in the station group
	 *@return Number of hits
	 */
	int GetNofHits(
			int stationGroup,
			int station) const {
		return fNofHits[stationGroup][station];
	}

	/* Returns maximum hit error in [cm] and the name of the coordinate
	 * ("X", "Y", "U")for the specified station.
	 *@param stationGroup Index of the station group in the detector
	 *@param station Index of the station in the station group
	 *@return Pair of hit error and and coordinate ID
	 */
	fscal GetMaxErr(
			int stationGroup,
			int station) const {
		return fMaxErr[stationGroup][station];
	}

	/* Clears the hit arrays */
	void Clear() {
		for(int i = 0; i < MAX_NOF_STATION_GROUPS_ELECTRON; i++) {
			for(int j = 0; j < MAX_NOF_STATIONS_ELECTRON; j++) {
				fNofHits[i][j] = 0;
				fMaxErr[i][j] = 0.;
			}
		}
	}

public:
	/* Calculates station group and station indices using the
	 * detector plane number.
	 *@param planeId [in] Detector plane index
	 *@param stationGroup [out] Index of the station group in the detector
	 *@param station [out] Index of the station in the station group
	 */
	void StationByPlaneId(
			unsigned char planeId,
			unsigned char& stationGroup,
			unsigned char& station) const {
		unsigned char counter = 0;
		for(unsigned char i = 0; i < fLayout.GetNofStationGroups(); i++) {
			counter += fLayout.GetNofStations(i);
			if (counter > planeId) {
				stationGroup = i;
				station = fLayout.GetNofStations(i) - (counter - planeId);
				return;
			}
		}
	}

	// Arrays with hits
	LitScalPixelHit* fHits[MAX_NOF_STATION_GROUPS_ELECTRON][MAX_NOF_STATIONS_ELECTRON][MAX_NOF_HITS_ELECTRON];
	// number of hits
	unsigned int fNofHits[MAX_NOF_STATION_GROUPS_ELECTRON][MAX_NOF_STATIONS_ELECTRON];
	// Arrays with maximum hit position errors for each substation
	fscal fMaxErr[MAX_NOF_STATION_GROUPS_ELECTRON][MAX_NOF_STATIONS_ELECTRON];

	LitDetectorLayoutElectron<T> fLayout;

	friend std::ostream & operator<<(std::ostream &strm, const LitHitDataElectron &hitData){
		strm << "HitDataElectron:" << std::endl;
		for(int i = 0; i < hitData.fLayout.GetNofStationGroups(); i++) {
			strm << " station group " << i << std::endl;
			for(int j = 0; j < hitData.fLayout.GetNofStations(i); j++) {
				strm << "   station " << j << ": " << hitData.GetNofHits(i, j) << " hits, "
					<< "max err=" << hitData.GetMaxErr(i, j) << std::endl;
			}
		}
		return strm;
	}
} _fvecalignment;

typedef LitHitDataElectron<fscal> LitHitDataElectronScal;
typedef LitHitDataElectron<fvec> LitHitDataElectronVec;

#endif /* LITHITDATAELECTRON_H_ */
