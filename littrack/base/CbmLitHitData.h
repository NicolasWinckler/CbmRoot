/** CbmLitHitData.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 1.0
 **
 ** Class for accessing the hits in the track reconstruction.
 **/

#ifndef CBMLITHITDATA_H_
#define CBMLITHITDATA_H_

#include "CbmLitTypes.h"

#include <vector>
#include <string>
#include <utility>

class CbmLitDetectorLayout;
class CbmLitHit;

class CbmLitHitData
{
public:
	/* Constructor */
	CbmLitHitData();

	/* Destructor */
	virtual ~CbmLitHitData();

	/* Sets the detector layout for which the hits are arranged
	 *@param layout Detector layout
	 */
	void SetDetectorLayout(
			const CbmLitDetectorLayout& layout);

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
			CbmLitHit* hit);

	/*Adds the hit using absolute detector plane (substation) index in the detector
	 *@param planeId Index of the detector plane (substation) in the detector
	 *@param hit Pointer to the hit to be added
	 */
	void AddHit(
			int planeId,
			CbmLitHit* hit);

	/* Returns the hit using station group, station and substation indices
	 *@param stationGroup Index of the station group in the detector
	 *@param station Index of the station in the station group
	 *@param substation Index of the substation in the station
	 *@param hitId Hit index in the array of hits for the specified substation
	 *@return Hit pointer
	 */
	const CbmLitHit* GetHit(
			int stationGroup,
			int station,
			int substation,
			int hitId) const;

	/* Returns the hit using absolute detector plane (substation) index in the detector
	 *@param planeId Index of the detector plane (substation) in the detector
	 *@param hitId Hit index in the array of hits for the specified substation
	 *@return Hit pointer
	 */
	const CbmLitHit* GetHit(
			int planeId,
			int hitId) const;

	/* Returns hit iterators using station group, station and substation indices
	 *@param stationGroup Index of the station group in the detector
	 *@param station Index of the station in the station group
	 *@param substation Index of the substation in the station
	 *@return Hit iterators
	 */
	HitPtrIteratorPair GetHits(
			int stationGroup,
			int station,
			int substation);

	/* Returns hits iterators using absolute detector plane (substation) index in the detector
	 *@param planeId Index of the detector plane (substation) in the detector
	 *@return Hit iterators
	 */
	HitPtrIteratorPair GetHits(
			int planeId);

	/* Returns number of hits for the specified substation
	 *@param stationGroup Index of the station group in the detector
	 *@param station Index of the station in the station group
	 *@param substation Index of the substation in the station
	 *@return Number of hits
	 */
	int GetNofHits(
			int stationGroup,
			int station,
			int substation) const;

	/* Returns maximum hit error in [cm] and the name of the coordinate
	 * ("X", "Y", "U")for the specified substation.
	 *@param stationGroup Index of the station group in the detector
	 *@param station Index of the station in the station group
	 *@param substation Index of the substation in the station
	 *@return Pair of hit error and and coordinate ID
	 */
	std::pair<myf, char> GetMaxErr(
			int stationGroup,
			int station,
			int substation) const;

	/* Returns maximum hit error in [cm] and the name of the coordinate
	 * ("X", "Y", "U")for the specified substation.
	 *@param planeId Index of the detector plane (substation) in the detector
	 *@return Pair of hit error and and coordinate ID
	 */
	std::pair<myf, char> GetMaxErr(
			int planeId) const;

	/* Clears the hit arrays */
	void Clear();

	/* Returns std::string representation of this class
	 * @return String representation of the class
	 */
	virtual std::string ToString() const;

private:
	/* Calculates station group, station and substation indices using the
	 * detector plane number.
	 *@param planeId [in] Detector plane index
	 *@param stationGroup [out] Index of the station group in the detector
	 *@param station [out] Index of the station in the station group
	 *@param subsattion [out] Index of the substation in the station
	 */
	void StationByPlaneId(
			int planeId,
			int& stationGroup,
			int& station,
			int& substation) const;

	// Arrays with hits
	std::vector<std::vector<std::vector<HitPtrVector> > > fHits;
	// Arrays with maximum hit position errors for each substation
	std::vector<std::vector<std::vector<std::pair<myf, char> > > > fMaxErr;
};

#endif /*CBMLITHITDATA_H_*/
