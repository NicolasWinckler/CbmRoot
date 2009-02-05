/** CbmMuchSectorDigiPar.cxx
 *@author   M.Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@version  1.0
 *@since    14.03.07
 **
 ** Parameter container for the digitization of a sector of a MuCh
 ** station.
 ** Supports the following digitisation types:
 ** 1 = GEMs
 ** 2 = CPC
 **
 ** Coordinates and rot. angle are in the station coordinate system. 
 ** Angles are stored and accessed in radians, but must be given in
 ** degrees in the input file.
 **/


#include "CbmMuchSectorDigiPar.h"

#include "TMath.h"

#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
CbmMuchSectorDigiPar::CbmMuchSectorDigiPar() {
  fSectorNr = -1;
  fType     =  0;
  fX0 = fY0 = fLx = fLy = 0.;
  fNCols = fNRows = 0;
  cout << "-W- CbmMuchSectorDigiPar: Do not use this constructor! " 
       << endl;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmMuchSectorDigiPar::CbmMuchSectorDigiPar(Int_t iSector, Int_t iType,
					   Double_t x0, Double_t y0,
					   Double_t lx, Double_t ly, 
					   Int_t nCols, Int_t nRows){
  fSectorNr = iSector;
  fType     = iType;
  fX0       = x0;
  fY0       = y0;
  fLx       = lx;
  fLy       = ly;
  fNCols    = nCols;
  fNRows    = nRows;
}
// -------------------------------------------------------------------------

CbmMuchSectorDigiPar::CbmMuchSectorDigiPar(Int_t iSector, Int_t iType,
					   Double_t x0, Double_t y0, Double_t z0,
					   Double_t lx, Double_t ly, 
					   Double_t d, Int_t nCols, Int_t nRows){
  fSectorNr = iSector;
  fType     = iType;
  fX0       = x0;
  fY0       = y0;
  fZ0       = z0;
  fLx       = lx;
  fLy       = ly;
  fD        = d;
  fNCols    = nCols;
  fNRows    = nRows;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmMuchSectorDigiPar::~CbmMuchSectorDigiPar() { }
// -------------------------------------------------------------------------



ClassImp(CbmMuchSectorDigiPar)
