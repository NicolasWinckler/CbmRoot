/** LitHitData.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 **
 ** Class for accessing the hits in the track reconstruction.
 **/

#ifndef LITHITDATA_H_
#define LITHITDATA_H_

//#include "LitTypes.h"
#include "LitDetectorGeometry.h"
#include "LitHit.h"

//#include <vector>

// Maximum number of hits for station/substation
const unsigned int MAX_NOF_HITS = 2000;

class LitHitData
{
public:
	/* Constructor */
	LitHitData() {
		for(int i = 0; i < MAX_NOF_STATION_GROUPS; i++) {
			for(int j = 0; j < MAX_NOF_STATIONS; j++) {
				for(int k = 0; k < MAX_NOF_SUBSTATIONS; k++){
					fNofHits[i][j][k] = 0;
					fMaxErr[i][j][k] = 0.;
				}
			}
		}
	};

	/* Destructor */
	virtual ~LitHitData(){};

	/* Sets the detector layout for which the hits are arranged
	 *@param layout Detector layout
	 */
	void SetDetectorLayout(
			const LitDetectorLayout& layout){
		fLayout = layout;
//		unsigned char nofGroups = layout.GetNofStationGroups();
//		fHits.resize(nofGroups);
//		fMaxErr.resize(nofGroups);
//		for(unsigned char i = 0; i < nofGroups; i++) {
//			unsigned char nofStations = layout.stationGroups[i].GetNofStations();
//			fHits[i].resize(nofStations);
//			fMaxErr[i].resize(nofStations);
//			for(unsigned char j = 0; j < nofStations; j++) {
//				unsigned char nofSubstations = layout.stationGroups[i].stations[j].GetNofSubstations();
//				fHits[i][j].resize(nofSubstations);
//				fMaxErr[i][j].resize(nofSubstations);
//				for(unsigned char k = 0; k < nofSubstations; k++) {
//					fHits[i][j][k].reserve(1500);
//				}
//			}
//		}
	}

	/* Adds the hit using station group, station and substation indices
	 *@param stationGroup Index of the station group in the detector
	 *@param station Index of the station in the station group
	 *@param substation Index of the substation in the station
	 *@param hit Pointer to the hit to be added
	 */
	void AddHit(
			int stationGroup,
			int station,
			int substation,
			LitScalPixelHit* hit) {
		unsigned int &nofHits = fNofHits[stationGroup][station][substation];
		fHits[stationGroup][station][substation][nofHits++] = hit;
		if (fMaxErr[stationGroup][station][substation] < hit->Dx)
			fMaxErr[stationGroup][station][substation] = hit->Dx;
	}

	/*Adds the hit using absolute detector plane (substation) index in the detector
	 *@param planeId Index of the detector plane (substation) in the detector
	 *@param hit Pointer to the hit to be added
	 */
	void AddHit(
			unsigned char planeId,
			LitScalPixelHit* hit) {
		unsigned char stationGroup;
		unsigned char station;
		unsigned char substation;
		StationByPlaneId(planeId, stationGroup, station, substation);
		AddHit(stationGroup, station, substation, hit);
	}

	/* Returns the hit using station group, station and substation indices
	 *@param stationGroup Index of the station group in the detector
	 *@param station Index of the station in the station group
	 *@param substation Index of the substation in the station
	 *@param hitId Hit index in the array of hits for the specified substation
	 *@return Hit pointer
	 */
	const LitScalPixelHit* GetHit(
			int stationGroup,
			int station,
			int substation,
			int hitId) const {
		return fHits[stationGroup][station][substation][hitId];
	}

	/* Returns hit iterators using station group, station and substation indices
	 *@param stationGroup Index of the station group in the detector
	 *@param station Index of the station in the station group
	 *@param substation Index of the substation in the station
	 *@return Hit iterators
	 */
	LitScalPixelHit** GetHits(
			int stationGroup,
			int station,
			int substation) {
		return fHits[stationGroup][station][substation];
	}

	/* Returns number of hits for the specified substation
	 *@param stationGroup Index of the station group in the detector
	 *@param station Index of the station in the station group
	 *@param substation Index of the substation in the station
	 *@return Number of hits
	 */
	int GetNofHits(
			int stationGroup,
			int station,
			int substation) const {
		return fNofHits[stationGroup][station][substation];
	}

	/* Returns maximum hit error in [cm] and the name of the coordinate
	 * ("X", "Y", "U")for the specified substation.
	 *@param stationGroup Index of the station group in the detector
	 *@param station Index of the station in the station group
	 *@param substation Index of the substation in the station
	 *@return Pair of hit error and and coordinate ID
	 */
	fscal GetMaxErr(
			int stationGroup,
			int station,
			int substation) const {
		return fMaxErr[stationGroup][station][substation];
	}

	/* Clears the hit arrays */
	void Clear() {
		for(int i = 0; i < MAX_NOF_STATION_GROUPS; i++) {
			for(int j = 0; j < MAX_NOF_STATIONS; j++) {
				for(int k = 0; k < MAX_NOF_SUBSTATIONS; k++){
					fNofHits[i][j][k] = 0;
					fMaxErr[i][j][k] = 0.;
				}
			}
		}
	}

public:
	/* Calculates station group, station and substation indices using the
	 * detector plane number.
	 *@param planeId [in] Detector plane index
	 *@param stationGroup [out] Index of the station group in the detector
	 *@param station [out] Index of the station in the station group
	 *@param subsattion [out] Index of the substation in the station
	 */
	void StationByPlaneId(
			unsigned char planeId,
			unsigned char& stationGroup,
			unsigned char& station,
			unsigned char& substation) const {
		unsigned char counter = 0;
		for(unsigned char i = 0; i < fLayout.GetNofStationGroups(); i++) {
			for(unsigned char j = 0; j < fLayout.GetNofStations(i); j++) {
				counter += fLayout.GetNofSubstations(i, j);
				if (counter > planeId) {
					stationGroup = i;
					station = j;
					substation = fLayout.GetNofSubstations(i, j) - (counter - planeId);
					return;
				}
			}
		}
	}

	// Arrays with hits
	LitScalPixelHit* fHits[MAX_NOF_STATION_GROUPS][MAX_NOF_STATIONS][MAX_NOF_SUBSTATIONS][MAX_NOF_HITS];
	// number of hits
	unsigned int fNofHits[MAX_NOF_STATION_GROUPS][MAX_NOF_STATIONS][MAX_NOF_SUBSTATIONS];
	// Arrays with maximum hit position errors for each substation
	fscal fMaxErr[MAX_NOF_STATION_GROUPS][MAX_NOF_STATIONS][MAX_NOF_SUBSTATIONS];

	LitDetectorLayout fLayout;

	friend std::ostream & operator<<(std::ostream &strm, const LitHitData &hitData){
		strm << "HitData:" << std::endl;
		for(int i = 0; i < hitData.fLayout.GetNofStationGroups(); i++) {
			strm << " station group " << i << std::endl;
			for(int j = 0; j < hitData.fLayout.GetNofStations(i); j++) {
				strm << "  station " << j << std::endl;
				for(int k = 0; k < hitData.fLayout.GetNofSubstations(i, j); k++){
					strm << "   substation " << k << ": " << hitData.GetNofHits(i, j, k) << " hits, "
						<< "max err=" << hitData.GetMaxErr(i, j, k) << std::endl;
				}
			}
		}
		return strm;
	}

};// _fvecalignment;

#endif /* LITHITDATA_H_ */
