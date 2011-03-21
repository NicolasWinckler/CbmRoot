/** CbmLitStation.h
 *@author A.Lebedev <andrey.lebedev@gsi.de>
 *@since 2008
 *
 * Class represents detector station.
 **/

#ifndef CBMLITSTATION_H_
#define CBMLITSTATION_H_

#include "base/CbmLitEnums.h"
#include "base/CbmLitSubstation.h"

#include <string>
#include <vector>
#include <sstream>

class CbmLitStation
{
public:
   /* Constructor */
   CbmLitStation():
      fType(kLITPIXELHIT),
      fSubstations() {};

   /* Destructor */
   virtual ~CbmLitStation() {};

   /* Sets type of the station. */
   void SetType(LitHitType type) {fType = type;}

   /* Sets vector of substations. */
   void SetSubstations(const std::vector<CbmLitSubstation>& substations) {
      fSubstations = substations;
   }

   /* Adds substation to the station. */
   void AddSubstation(const CbmLitSubstation& substation) {
      fSubstations.push_back(substation);
   }

   /* Returns station type. */
   LitHitType GetType() const {return fType;}

   /* Returns i-th substation. */
   const CbmLitSubstation& GetSubstation(int i) const {return fSubstations[i];}

   /* Returns number of substations. */
   int GetNofSubstations() const {return fSubstations.size();}

   /* Returns std::string representation of the class. */
   virtual std::string ToString() const {
      std::stringstream ss;
      ss << "Station: type=" << GetType()
         << " nofSubstations=" << GetNofSubstations() << std::endl;
      return ss.str();;
   }

private:
   LitHitType fType; // Type of the station
   std::vector<CbmLitSubstation> fSubstations; // Vector of substations
};

#endif /*CBMLITSTATION_H_*/
