//--------------------------------------------------------------------------
//----                     CbmTofHit                                    ----
//----                Created Everard Cordier 14/09/05                  ----
//----                Modified by D. Gonzalez-Diaz 06/09/06             ----
//----                Modified by A.Lebedev 26/05/09                    ----
//--------------------------------------------------------------------------

#include "CbmTofHit.h"

#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------

CbmTofHit::CbmTofHit()
  : CbmPixelHit(),
    fTime(0.),
    fFlag(1),
    fChannel(0)
{
  SetType(kTOFHIT);
}


// -----   Standard constructor   ------------------------------------------

CbmTofHit::CbmTofHit(Int_t detID, TVector3 pos, TVector3 dpos, Int_t index, Double_t time, Int_t flag, Int_t channel)
  : CbmPixelHit(detID, pos, dpos, 0., index),
    fTime(time),
    fFlag(flag),
    fChannel(channel)
{
  SetType(kTOFHIT);
}

CbmTofHit::CbmTofHit(Int_t detID, TVector3 pos, TVector3 dpos, Int_t index, Double_t time, Int_t flag)
  : CbmPixelHit(detID, pos, dpos, 0., index),
    fTime(time),
    fFlag(flag),
    fChannel(0)
{
  SetType(kTOFHIT);
}

// -----   Constructor without flag  ------------------------------------------

CbmTofHit::CbmTofHit(Int_t detID, TVector3 pos, TVector3 dpos, Int_t index, Double_t time)
  : CbmPixelHit(detID, pos, dpos, 0., index),
    fTime(time),
    fFlag(1),
    fChannel(0)    
{
    SetType(kTOFHIT);
}

// -----   Destructor   ----------------------------------------------------
CbmTofHit::~CbmTofHit() { }


// -----  Print  -----------------------------------------------------------
void CbmTofHit::Print() const
{
  cout << "-I- CbmTofHit" << endl;
  cout << "    DetectorID: " << GetDetectorId() << endl;
  cout << "    Position: (" << GetX()
       << ", " << GetY()
       << ", " << GetZ() << ") cm"
       << endl;
  cout << "    Position error: (" << GetDx()
       << ", " << GetDy()
       << ", " << GetDz() << ") cm"
       << endl;
  cout << "    Time: " << GetTime() << " ns"
       << endl;
  cout << "    Flag: " << GetFlag()
       << endl;  
  cout << "    Channel: " << GetCh()
       << endl;
}
// -------------------------------------------------------------------------


ClassImp(CbmTofHit)
