// -------------------------------------------------------------------------
// -----                      CbmRichHit source file                 -----
// -----               Created 28/04/04  by B. Polichtchouk            -----
// -------------------------------------------------------------------------

#include "CbmRichHit.h"

#include <iostream>

using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
CbmRichHit::CbmRichHit(): FairHit() {
  fPmtID=0;
  fNPhotons=0;
  fAmplitude=0.;
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
CbmRichHit::~CbmRichHit(){}
// -------------------------------------------------------------------------


// -----   Public method Print   -------------------------------------------
void CbmRichHit::Print(const Option_t* opt) const {
  printf("RICH hit: PMT=%d, XY=(%.2f,%.2f) cm, Nphot=%d, Amp=%f\n",
	 fPmtID,fX,fY, fNPhotons, fAmplitude);
}
// -------------------------------------------------------------------------

ClassImp(CbmRichHit)
