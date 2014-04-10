#ifndef LXEFF_INCLUDED
#define LXEFF_INCLUDED

#include "Rtypes.h"
#include "LxSettings.h"
#include <map>

class LxFinder;

struct LxEff
{
  explicit LxEff(LxFinder& owner);
  Double_t CalcRecoEff(bool joinExt);
#ifdef CALC_LINK_WITH_STS_EFF
  Double_t CalcLinkEff();// Before calling of this function CalcRecoEff() must be called because mcTracks is filled there.
#endif//CALC_LINK_WITH_STS_EFF
  LxFinder& finder;
};

struct LxMCTrack;

struct LxStsMCPoint
{
  Double_t p, q, x, y, z, px, py, pz;
  Int_t stationNumber;
  LxMCTrack* mcTrack;
};

#endif//LXEFF_INCLUDED
