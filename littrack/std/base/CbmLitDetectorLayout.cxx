/** CbmLitDetectorLayout.cxx
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 1.0
 **/

#include "base/CbmLitDetectorLayout.h"
#include "utils/CbmLitUtils.h"
#include <iostream>

CbmLitDetectorLayout::CbmLitDetectorLayout():
   fStationGroups()
{
}

CbmLitDetectorLayout::~CbmLitDetectorLayout()
{
}

std::string CbmLitDetectorLayout::ToString() const
{
   std::string str = std::string("Detector Layout:\n") +
		   std::string("-number of station groups: ") + lit::ToString<Int_t>(GetNofStationGroups()) + std::string("\n") +
		   std::string("-number of detector planes: ") + lit::ToString<Int_t>(GetNofPlanes()) + std::string("\n") + "-station groups: \n";
   Int_t nofStationGroups = GetNofStationGroups();
   for (Int_t iStationGroup = 0; iStationGroup < nofStationGroups; iStationGroup++) {
      str += " " + GetStationGroup(iStationGroup).ToString();
      Int_t nofStations = GetNofStations(iStationGroup);
      for (Int_t iStation = 0; iStation < nofStations; iStation++) {
         str += "  " + GetStation(iStationGroup, iStation).ToString();
         Int_t nofSubstations = GetNofSubstations(iStationGroup, iStation);
         for (Int_t iSubstation = 0; iSubstation < nofSubstations; iSubstation++) {
            str += "   " + GetSubstation(iStationGroup, iStation, iSubstation).ToString();
         }
      }
   }
   str += "\n";
   return str;
}
