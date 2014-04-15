/**
 * \file CbmStsHit.cxx
 **/
#include "CbmStsHit.h"

CbmStsHit::CbmStsHit()
    : CbmPixelHit()
    , fFrontDigiId(-1)
    , fBackDigiId(-1)
    , fSectorNr(-1)
    , fFrontClusterId(-1)
    , fBackClusterId(-1)
{
}

CbmStsHit::CbmStsHit(Int_t address, const TVector3& pos, const TVector3& dpos, Double_t dxy, Int_t frontClusterId, Int_t backClusterId, Int_t frontDigiId, Int_t backDigiId, Int_t sectorNr)
    : CbmPixelHit(address, pos, dpos, dxy, -1)
    , fFrontDigiId(frontDigiId)
    , fBackDigiId(backDigiId)
    , fSectorNr(sectorNr)
    , fFrontClusterId(frontClusterId)
    , fBackClusterId(backClusterId)
{
}

CbmStsHit::~CbmStsHit()
{
}

ClassImp(CbmStsHit)
