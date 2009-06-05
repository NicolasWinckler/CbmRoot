// -------------------------------------------------------------------------
// -----                    CbmGlobalTrack source file                 -----
// -----                  Created 01/12/05  by V. Friese               -----
// -----                  Modified 04/06/09  by A. Lebedev             -----
// -------------------------------------------------------------------------
#include "CbmGlobalTrack.h"

#include <iostream>

using std::cout;
using std::endl;



// -----   Default constructor   -------------------------------------------
CbmGlobalTrack::CbmGlobalTrack() {
  fStsTrack = fTrdTrack = fMuchTrack = fRichRing = fTofHit = -1;
  fPidHypo = fFlag = fNDF = 0;
  fChi2 = 0.;
  fLength = 0.;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmGlobalTrack::~CbmGlobalTrack() { }
// -------------------------------------------------------------------------



// -----   Public method Print   -------------------------------------------
void CbmGlobalTrack::Print() const {
  cout << endl << "StsTrack " << fStsTrack << ", TrdTrack " << fTrdTrack << ", MuchTrack " << fMuchTrack
       << ", RichRing " << fRichRing << ", TofHit " << fTofHit << endl;
//  cout << "Parameters at first plane: " << endl;
//  fParamFirst.Print();
//  cout << "Parameters at last plane: " << endl;
//  fParamLast.Print();
  cout << "chi2 = " << fChi2 << ", NDF = " << fNDF
       << ", Quality flag " << fFlag << endl;
  cout << "length = " << fLength << endl;
}
// -------------------------------------------------------------------------


ClassImp(CbmGlobalTrack)
