// ------------------------------------------------------------------
// -----                      CbmHadron.h                       -----
// -----            Created 2008-01-18 by D.Kresan              -----
// ------------------------------------------------------------------

#include "CbmHadron.h"


// ------------------------------------------------------------------
CbmHadron::CbmHadron()
{
    fCharge = fNMvdHits = fNStsHits = fNTrdHits = fPdg = 0;
    fP = fPt = fPz = fB = fLength = fMass2 = 0.;
    fGhost = fTDH = kFALSE;
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
CbmHadron::CbmHadron(Int_t charge, Double_t p, Double_t pt, Double_t pz,
                     Int_t nMvdHits, Int_t nStsHits, Int_t nTrdHits,
		     Double_t b, Double_t length, Double_t mass2,
		     Int_t pdg, Bool_t ghost, Bool_t tdh)
{
    fCharge = charge;
    fP = p;
    fPt = pt;
    fPz = pz;
    fNMvdHits = nMvdHits;
    fNStsHits = nStsHits;
    fNTrdHits = nTrdHits;
    fB = b;
    fLength = length;
    fMass2 = mass2;
    fPdg = pdg;
    fGhost = ghost;
    fTDH = tdh;
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
CbmHadron::~CbmHadron()
{
}
// ------------------------------------------------------------------


ClassImp(CbmHadron);

