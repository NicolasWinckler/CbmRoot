/**
 * \file CbmLitStation.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 * \brief Detector station.
 **/

#ifndef CBMLITSTATION_H_
#define CBMLITSTATION_H_

#include "base/CbmLitEnums.h"
#include "base/CbmLitSubstation.h"

#include "TObject.h"

#include <string>
#include <vector>
#include <sstream>

/**
 * \class CbmLitStation
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 * \brief Detector station.
 **/
class CbmLitStation
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitStation():
      fType(kLITPIXELHIT),
      fSubstations() {}

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitStation() {}

   /**
    * \brief Set station type.
    * \param[in] type Station type.
    */
   void SetType(LitHitType type) { fType = type; }

   /**
    * \brief Set vector of substations.
    * \param[in] substations Vector of substations.
    */
   void SetSubstations(const std::vector<CbmLitSubstation>& substations) {
      fSubstations = substations;
   }

   /**
    * \brief Add substation to tation.
    * \param[in] substation Substation to be added.
    */
   void AddSubstation(const CbmLitSubstation& substation) {
      fSubstations.push_back(substation);
   }

   /**
    * \brief Return station type.
    * \return Station type.
    */
   LitHitType GetType() const { return fType; }

   /**
    * \brief Return i-th substation.
    * \param[in] i Substation index.
    * \return Substation.
    */
   const CbmLitSubstation& GetSubstation(Int_t i) const { return fSubstations[i]; }

   /**
    * \brief Return number of substations.
    * \return Number of substations.
    */
   Int_t GetNofSubstations() const { return fSubstations.size(); }

   /**
    * \brief Return string representation of class.
    * \return String representation of class.
    */
   virtual std::string ToString() const {
      std::stringstream ss;
      ss << "Station: type=" << GetType()
         << " nofSubstations=" << GetNofSubstations() << std::endl;
      return ss.str();
   }

private:
   LitHitType fType; // Type of the station
   std::vector<CbmLitSubstation> fSubstations; // Vector of substations
};

#endif /*CBMLITSTATION_H_*/
