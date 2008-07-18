//--------------------------------------------------------------------------
//----                     CbmTofHit                                    ----
//----                Created Everard Cordier 14/09/05                  ----
//----                Modified by D. Gonzalez-Diaz 06/09/06             ----
//--------------------------------------------------------------------------

#include "CbmTofHit.h"

#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------

CbmTofHit::CbmTofHit()
{
  fTime = 0.;
  fFlag = 1;
}


// -----   Standard constructor   ------------------------------------------

CbmTofHit::CbmTofHit(Int_t detID, TVector3 pos, TVector3 dpos, Int_t index, Double_t time, Int_t flag)
: CbmHit(detID, pos, dpos, index)
{
    fTime = time;
    fFlag = flag;
}

// -----   Constructor without flag  ------------------------------------------

CbmTofHit::CbmTofHit(Int_t detID, TVector3 pos, TVector3 dpos, Int_t index, Double_t time)
: CbmHit(detID, pos, dpos, index)
{
    fTime = time;
}

// -----   Destructor   ----------------------------------------------------
CbmTofHit::~CbmTofHit() { }


// -----  Print  -----------------------------------------------------------
void CbmTofHit::Print(const Option_t* opt) const
{
  cout << "-I- CbmTofHit" << endl;
  cout << "    DetectorID: " << fDetectorID << endl;
  cout << "    Position: (" << fX
       << ", " << fY
       << ", " << fZ << ") cm"
       << endl;
  cout << "    Position error: (" << fDx
       << ", " << fDy
       << ", " << fDz << ") cm"
       << endl;
  cout << "    Time: " << fTime << " ns" 
       << endl;
  cout << "    Flag: " << fFlag
       << endl;
}
// -------------------------------------------------------------------------


ClassImp(CbmTofHit)
