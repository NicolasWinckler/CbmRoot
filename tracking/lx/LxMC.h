#ifndef LXMC_INCLUDED
#define LXMC_INCLUDED

#include <vector>
#include <list>
#include "Rtypes.h"
#include "LxSettings.h"
#include "CbmStsTrack.h"

struct LxPoint;
struct LxStsMCPoint;

struct LxMCPoint
{
  Double_t p, q, x, y, z, px, py, pz;
  Int_t stationNumber, layerNumber;
  std::list<LxPoint*> lxPoints;
};

struct LxMCTrack
{
  Double_t p, q, x, y, z, px, py, pz;
  Int_t mother_ID;
  Int_t pdg;
  std::vector<LxMCPoint*> Points;
#ifdef MAKE_STS_MC_RESEARCHES
  std::list<LxStsMCPoint*> stsPoints[8];
#endif//MAKE_STS_MC_RESEARCHES
  UInt_t fUniqueID;
  CbmStsTrack* externalTrack;
};

#endif//LXMC_INCLUDED
