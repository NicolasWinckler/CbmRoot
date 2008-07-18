// -------------------------------------------------------------------------
// -----                 CbmStsSectorDigiPar source file               -----
// -----                  Created 27/06/05  by V. Friese               -----
// -------------------------------------------------------------------------


#include <iostream>

#include "TMath.h"

#include "CbmStsSectorDigiPar.h"

using std::cout;
using std::endl;



// -----   Default constructor   -------------------------------------------
CbmStsSectorDigiPar::CbmStsSectorDigiPar() {
  fSectorNr = -1;
  fType     =  0;
  fX0 = fY0 = fRotation = fLx = fLy = fDx = fDy = fStereo = 0.;
  cout << "-W- CbmStsSectorDigiPar: Do not use this constructor! " 
       << endl;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmStsSectorDigiPar::CbmStsSectorDigiPar(Int_t iSector, Int_t iType,
					 Double_t x0, Double_t y0,
					 Double_t rotation, 
					 Double_t lx, Double_t ly, 
					 Double_t dx, Double_t dy,
					 Double_t stereo) {

  fSectorNr = iSector;
  fType     = iType;
  fX0       = x0;
  fY0       = y0;
  fRotation = rotation;
  fLx       = lx;
  fLy       = ly;
  fDx       = dx;
  fDy       = dy;
  fStereo   = stereo;

  // Check for stereo angle
  // (Mathematics has not been checked for angles > 90 degrees)
  if (TMath::Abs(fStereo) > TMath::Pi()/2.) {
    cout << "-E- CbmStsSectorDigiPar: Stereo angle > 90 degrees !" 
	 << endl;
    cout << iSector << " " << iType << " " << x0 << " " << y0 << " "
	 << rotation << " " << lx << " " << ly << " " << dx << " "
	 << dy << " " << stereo << endl;
    cout << "                       Revise sector layout! " << endl;
    Fatal("CbmStsSectorDigiPar", "Stereo Angle");
  }

}
// -------------------------------------------------------------------------

CbmStsSectorDigiPar::CbmStsSectorDigiPar(Int_t iSector, Int_t iType,
					 Double_t x0, Double_t y0, Double_t z0,
					 Double_t rotation, 
					 Double_t lx, Double_t ly, 
					 Double_t d, Double_t dx, Double_t dy,
					 Double_t stereo) {

  fSectorNr = iSector;
  fType     = iType;
  fX0       = x0;
  fY0       = y0;
  fZ0       = z0;
  fRotation = rotation;
  fLx       = lx;
  fLy       = ly;
  fD        = d;
  fDx       = dx;
  fDy       = dy;
  fStereo   = stereo;

  // Check for stereo angle
  // (Mathematics has not been checked for angles > 90 degrees)
  if (TMath::Abs(fStereo) > TMath::Pi()/2.) {
    cout << "-E- CbmStsSectorDigiPar: Stereo angle > 90 degrees !" 
	 << endl;
    cout << iSector << " " << iType << " " << x0 << " " << y0 << " "
	 << rotation << " " << lx << " " << ly << " " << dx << " "
	 << dy << " " << stereo << endl;
    cout << "                       Revise sector layout! " << endl;
    Fatal("CbmStsSectorDigiPar", "Stereo Angle");
  }

}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmStsSectorDigiPar::~CbmStsSectorDigiPar() { }
// -------------------------------------------------------------------------



ClassImp(CbmStsSectorDigiPar)
