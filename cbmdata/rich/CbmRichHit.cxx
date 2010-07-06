// -------------------------------------------------------------------------
// -----                      CbmRichHit source file                 -----
// -----               Created 28/04/04  by B. Polichtchouk            -----
// -------------------------------------------------------------------------

#include "CbmRichHit.h"

// -----   Default constructor   -------------------------------------------
CbmRichHit::CbmRichHit(): CbmPixelHit() {
  fPmtId=0;
  fNPhotons=0;
  fAmplitude=0.;
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
CbmRichHit::~CbmRichHit(){}
// -------------------------------------------------------------------------


// -----   Public method Print   -------------------------------------------
void CbmRichHit::Print() const {
  printf("RICH hit: PMT=%d, XY=(%.2f,%.2f) cm, Nphot=%d, Amp=%f\n",
	 fPmtId, GetX(), GetY(), fNPhotons, fAmplitude);
}
// -------------------------------------------------------------------------

ClassImp(CbmRichHit)
