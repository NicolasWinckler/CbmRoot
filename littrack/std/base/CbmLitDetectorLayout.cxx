/** CbmLitDetectorLayout.cxx
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 1.0
 **/

#include "base/CbmLitDetectorLayout.h"
#include "utils/CbmLitUtils.h"
#include "data/CbmLitTrackParam.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"
#include <iostream>

CbmLitDetectorLayout::CbmLitDetectorLayout():
   fStationGroups(),
   fGeo()
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

LitStatus CbmLitDetectorLayout::FindBoundary(
   const CbmLitTrackParam* par,
   Int_t stationGroup,
   Int_t station,
   Int_t substation,
   bool downstream,
   litfloat& zOut,
   Int_t& moduleId) const
{
   zOut = -1.;
   InitTrack(par, downstream);

   string name = lit::ToString<Int_t>(stationGroup) + "_" + lit::ToString<Int_t>(station) + "_" + lit::ToString<Int_t>(substation) + "_";

   if (CheckGeoFailure()) { return kLITERROR; }

	do {
		string volumeName = fGeo->GetCurrentVolume()->GetName();
		if (volumeName.find(name) != string::npos) { // We are in the volume, make step to the exit of the volume
			fGeo->FindNextBoundaryAndStep();
			if (CheckGeoFailure()) { return kLITERROR; }
			zOut = fGeo->GetCurrentPoint()[2];
			moduleId = atoi(lit::Split(volumeName, '_')[3].c_str());
			return kLITSUCCESS;
		} else { // We are not in the required volume, make a step to the next volume
			fGeo->FindNextBoundaryAndStep();
			if (CheckGeoFailure()) { return kLITERROR; }
		}
	} while (true);

   return kLITERROR;
}

void CbmLitDetectorLayout::InitTrack(
   const CbmLitTrackParam* par,
   Bool_t downstream) const
{
   litfloat nx, ny, nz;
   par->GetDirCos(nx, ny, nz);
   // Change track direction for upstream
   if (!downstream) {
      nx = -nx;
      ny = -ny;
      nz = -nz;
   }
   fGeo->InitTrack(par->GetX(), par->GetY(), par->GetZ(), nx, ny, nz);
}

Bool_t CbmLitDetectorLayout::CheckGeoFailure() const
{
	return fGeo->IsOutside() || std::isnan(fGeo->GetCurrentPoint()[0]) || std::isnan(fGeo->GetCurrentPoint()[1]) || std::isnan(fGeo->GetCurrentPoint()[2]);
}
