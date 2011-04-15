/** CbmLitTrack.cxx
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 1.0
 **/

#include "data/CbmLitTrack.h"

#include "data/CbmLitHit.h"
#include "data/CbmLitPixelHit.h"
#include "data/CbmLitStripHit.h"
#include "utils/CbmLitComparators.h"
#include "utils/CbmLitMemoryManagment.h"

#include <algorithm>
#include <sstream>
#include <cmath>

CbmLitTrack::CbmLitTrack():
   fHits(),
   fParamFirst(),
   fParamLast(),
   fFitNodes(),
   fQuality(kLITGOOD),
   fChi2(0),
   fNDF(0),
   fPreviousTrackId(0),
   fLastPlaneId(0),
   fPDG(211),
   fNofMissingHits(0),
   fRefId(-1)
{
}

CbmLitTrack::~CbmLitTrack()
{
   ClearHits();
}

CbmLitTrack::CbmLitTrack(const CbmLitTrack& track):
   fHits(),
   fParamFirst(),
   fParamLast(),
   fFitNodes(),
   fQuality(kLITGOOD),
   fChi2(0),
   fNDF(0),
   fPreviousTrackId(0),
   fLastPlaneId(0),
   fPDG(211),
   fNofMissingHits(0),
   fRefId(-1)
{
   *this = track;
}

CbmLitTrack& CbmLitTrack::operator=(
   const CbmLitTrack& track)
{
   for_each(fHits.begin(), fHits.end(), DeleteObject());
   fHits.clear();
   for (int iHit = 0; iHit < track.GetNofHits(); iHit++) {
      AddHit(track.GetHit(iHit));
   }

   SetParamFirst(track.GetParamFirst());
   SetParamLast(track.GetParamLast());
   SetQuality(track.GetQuality());
   SetChi2(track.GetChi2());
   SetNDF(track.GetNDF());
   SetPreviousTrackId(track.GetPreviousTrackId());
   SetLastPlaneId(track.GetLastPlaneId());
   SetPDG(track.GetPDG());
   SetFitNodes(track.GetFitNodes());
   SetNofMissingHits(track.GetNofMissingHits());

   return *this;
}

void CbmLitTrack::AddHit(
   const CbmLitHit* hit)
{
   if (hit->GetType() == kLITSTRIPHIT) {
      // TODO do we need typecasting here?!?!?
      CbmLitStripHit* newHit = new CbmLitStripHit(*static_cast<const CbmLitStripHit*>(hit));
      fHits.push_back(newHit);
   } else if (hit->GetType() == kLITPIXELHIT) {
      CbmLitPixelHit* newHit = new CbmLitPixelHit(*static_cast<const CbmLitPixelHit*>(hit));
      fHits.push_back(newHit);
   }
}

void CbmLitTrack::RemoveHit(
   int index)
{
   delete fHits[index];
   fHits.erase(fHits.begin() + index);
   if (!fFitNodes.empty()) { fFitNodes.erase(fFitNodes.begin() + index); }
}

void CbmLitTrack::ClearHits()
{
   std::for_each(fHits.begin(), fHits.end(), DeleteObject());
   fHits.clear();
}

std::string CbmLitTrack::ToString() const
{
   std::stringstream ss;
   ss << "Track: quality=" <<  fQuality << ", chi2=" << fChi2
      << ", ndf=" << fNDF << ", previousTrackId=" << fPreviousTrackId
      << ", lastPlaneId=" << fLastPlaneId << ", pdg=" << fPDG
      << ", nofHits=" << fHits.size() << ", nofFitNodes=" << fFitNodes.size() << std::endl;
   return ss.str();
}

void CbmLitTrack::SortHits(
   bool downstream)
{
// if (downstream) std::sort(fHits.begin(), fHits.end(), CompareHitPtrPlaneIdLess());
// else std::sort(fHits.begin(), fHits.end(), CompareHitPtrPlaneIdMore());
   if (downstream) { std::sort(fHits.begin(), fHits.end(), CompareHitPtrZLess()); }
   else { std::sort(fHits.begin(), fHits.end(), CompareHitPtrZMore()); }
}

bool CbmLitTrack::CheckParams() const
{
   //TODO improve parameters check for the track

   std::vector<myf> covFirst(fParamFirst.GetCovMatrix());
   std::vector<myf> covLast(fParamLast.GetCovMatrix());
   for (int i = 0; i < 15; i++) {
      if (std::abs(covFirst[i]) > 10000. ||
            std::abs(covLast[i]) > 10000.) { return false; }
   }
   if (GetNofHits() < 1) { return false; }
   return true;
}

int CbmLitTrack::GetNofHits(
   int planeId)
{
   CbmLitHit value;
   value.SetPlaneId(planeId);
   HitPtrIteratorPair bounds =
      std::equal_range(fHits.begin(), fHits.end(), &value, CompareHitPtrPlaneIdLess());
   return bounds.second - bounds.first;
}

HitPtrIteratorPair CbmLitTrack::GetHits(
   int planeId)
{
   CbmLitHit value;
   value.SetPlaneId(planeId);
   HitPtrIteratorPair bounds =
      std::equal_range(fHits.begin(), fHits.end(), &value, CompareHitPtrPlaneIdLess());
   return bounds;
}

void CbmLitTrack::GetHitBounds(
   std::vector<HitPtrIteratorPair>& bounds)
{
   bounds.clear();
   int min = fHits.front()->GetPlaneId();
   int max = fHits.back()->GetPlaneId();
   for (int i = min; i <= max; i++) {
      HitPtrIteratorPair b = GetHits(i);
      if(b.first == b.second) { continue; }
      bounds.push_back(b);
   }
}
