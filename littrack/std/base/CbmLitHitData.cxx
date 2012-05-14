/** CbmLitHitData.cxx
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 1.0
 **/

#include "base/CbmLitHitData.h"

#include "base/CbmLitDetectorLayout.h"
#include "data/CbmLitHit.h"
#include "data/CbmLitStripHit.h"
#include "data/CbmLitPixelHit.h"

#include <sstream>
#include <iostream>

CbmLitHitData::CbmLitHitData():
   fHits(),
   fMaxErr()
{
}

CbmLitHitData::~CbmLitHitData()
{
}

void CbmLitHitData::SetDetectorLayout(
   const CbmLitDetectorLayout& layout)
{
   Int_t nofGroups = layout.GetNofStationGroups();
   fHits.resize(nofGroups);
   fMaxErr.resize(nofGroups);
   for(Int_t i = 0; i < nofGroups; i++) {
      Int_t nofStations = layout.GetNofStations(i);
      fHits[i].resize(nofStations);
      fMaxErr[i].resize(nofStations);
      for(Int_t j = 0; j < nofStations; j++) {
         Int_t nofSubstations = layout.GetNofSubstations(i, j);
         fHits[i][j].resize(nofSubstations);
         fMaxErr[i][j].resize(nofSubstations);
         for(Int_t k = 0; k < nofSubstations; k++) {
        	Int_t nofModuleRotations = layout.GetNofModuleRotations(i, j, k);
        	fHits[i][j][k].resize(nofModuleRotations);
        	fMaxErr[i][j][k].resize(nofModuleRotations);
            for(Int_t l = 0; l < nofModuleRotations; l++) {
        	 fHits[i][j][k][l].reserve(1500);
            }
         }
      }
   }
}

void CbmLitHitData::AddHit(
   Int_t stationGroup,
   Int_t station,
   Int_t substation,
   Int_t moduleRotation,
   CbmLitHit* hit)
{
   assert(stationGroup > -1 && station > -1 && substation > -1 && moduleRotation > -1 && hit != NULL);
   fHits[stationGroup][station][substation][moduleRotation].push_back(hit);

   if (hit->GetType() == kLITSTRIPHIT) {
      CbmLitStripHit* stripHit = static_cast<CbmLitStripHit*>(hit);
      if (fMaxErr[stationGroup][station][substation][moduleRotation].first < stripHit->GetDu()) {
         fMaxErr[stationGroup][station][substation][moduleRotation] = std::pair<litfloat, char>(stripHit->GetDu(), 'U');
      }
   } else if (hit->GetType() == kLITPIXELHIT) {
      CbmLitPixelHit* pixelHit = static_cast<CbmLitPixelHit*>(hit);
      if (fMaxErr[stationGroup][station][substation][moduleRotation].first < pixelHit->GetDx()) {
         fMaxErr[stationGroup][station][substation][moduleRotation] = std::pair<litfloat, char>(pixelHit->GetDx(), 'X');
      }
   }
}

const CbmLitHit* CbmLitHitData::GetHit(
   Int_t stationGroup,
   Int_t station,
   Int_t substation,
   Int_t moduleRotation,
   Int_t hitId) const
{
   return fHits[stationGroup][station][substation][moduleRotation][hitId];
}

HitPtrIteratorPair CbmLitHitData::GetHits(
   Int_t stationGroup,
   Int_t station,
   Int_t substation,
   Int_t moduleRotation)
{
   return HitPtrIteratorPair(
             fHits[stationGroup][station][substation][moduleRotation].begin(),
             fHits[stationGroup][station][substation][moduleRotation].end());
}

Int_t CbmLitHitData::GetNofHits(
   Int_t stationGroup,
   Int_t station,
   Int_t substation,
   Int_t moduleRotation) const
{
   return fHits[stationGroup][station][substation][moduleRotation].size();
}

void CbmLitHitData::Clear()
{
   for(UInt_t i = 0; i < fHits.size(); i++) {
      for(UInt_t j = 0; j < fHits[i].size(); j++) {
         for(UInt_t k = 0; k < fHits[i][j].size(); k++) {
        	 for(UInt_t l = 0; l < fHits[i][j][k].size(); l++) {
				fHits[i][j][k][l].clear();
				fHits[i][j][k][l].reserve(1500);
				fMaxErr[i][j][k][l] = std::pair<litfloat, char>(0., ' ');
        	 }
         }
      }
   }
}

std::string CbmLitHitData::ToString() const
{
   std::stringstream ss;
   ss << "HitData:" << std::endl;
   for(UInt_t i = 0; i < fHits.size(); i++) {
      ss << " station group " << i << std::endl;
      for(UInt_t j = 0; j < fHits[i].size(); j++) {
         ss << "  station " << j << std::endl;
         for(UInt_t k = 0; k < fHits[i][j].size(); k++) {
        	 ss << "  substation " << k << std::endl;
        	 for(UInt_t l = 0; l < fHits[i][j][k].size(); l++) {
				ss << "   moduleRotation " << l << ": " << GetNofHits(i, j, k, l) << " hits, "
				   << "max err=(" << GetMaxErr(i, j, k, l).first << "," << GetMaxErr(i, j, k, l).second
				   << ")" << std::endl;
        	 }
         }
      }
   }
   return ss.str();
}

std::pair<litfloat, char> CbmLitHitData::GetMaxErr(
   Int_t stationGroup,
   Int_t station,
   Int_t substation,
   Int_t moduleRotation) const
{
   return fMaxErr[stationGroup][station][substation][moduleRotation];
}
