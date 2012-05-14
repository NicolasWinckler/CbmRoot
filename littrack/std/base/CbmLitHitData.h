/** CbmLitHitData.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 1.0
 **
 ** Class for accessing the hits in the track reconstruction.
 **/

#ifndef CBMLITHITDATA_H_
#define CBMLITHITDATA_H_

#include "base/CbmLitTypes.h"

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
    *@param
    *@param hit Pointer to the hit to be added
    */
   void AddHit(
      Int_t stationGroup,
      Int_t station,
      Int_t substation,
      Int_t moduleRotation,
      CbmLitHit* hit);

   /* Returns the hit using station group, station and substation indices
    *@param stationGroup Index of the station group in the detector
    *@param station Index of the station in the station group
    *@param substation Index of the substation in the station
    *@param moduleRotation
    *@param hitId Hit index in the array of hits for the specified substation
    *@return Hit pointer
    */
   const CbmLitHit* GetHit(
      Int_t stationGroup,
      Int_t station,
      Int_t substation,
      Int_t moduleRotation,
      Int_t hitId) const;

   /* Returns hit iterators using station group, station and substation indices
    *@param stationGroup Index of the station group in the detector
    *@param station Index of the station in the station group
    *@param substation Index of the substation in the station
    *@param moduleRotation
    *@return Hit iterators
    */
   HitPtrIteratorPair GetHits(
      Int_t stationGroup,
      Int_t station,
      Int_t substation,
      Int_t moduleRotation);

   /* Returns number of hits for the specified substation
    *@param stationGroup Index of the station group in the detector
    *@param station Index of the station in the station group
    *@param substation Index of the substation in the station
    *@param moduleRotation
    *@return Number of hits
    */
   Int_t GetNofHits(
      Int_t stationGroup,
      Int_t station,
      Int_t substation,
      Int_t moduleRotation) const;

   /* Returns maximum hit error in [cm] and the name of the coordinate
    * ("X", "Y", "U") for the specified substation.
    *@param stationGroup Index of the station group in the detector
    *@param station Index of the station in the station group
    *@param substation Index of the substation in the station
    *@param moduleRotation
    *@return Pair of hit error and and coordinate ID
    */
   std::pair<litfloat, char> GetMaxErr(
      Int_t stationGroup,
      Int_t station,
      Int_t substation,
      Int_t moduleRotation) const;

   /* Clears the hit arrays */
   void Clear();

   /* Returns std::string representation of this class
    * @return String representation of the class
    */
   virtual std::string ToString() const;

private:
   // Arrays with hits
   // [station group][station][substation][module rotation]
   std::vector<std::vector<std::vector<std::vector<HitPtrVector> > > > fHits;
   // Arrays with maximum hit position errors for each substation
   // [station group][station][substation][module rotation]
   std::vector<std::vector<std::vector<std::vector<std::pair<litfloat, char> > > > > fMaxErr;
};

#endif /*CBMLITHITDATA_H_*/
