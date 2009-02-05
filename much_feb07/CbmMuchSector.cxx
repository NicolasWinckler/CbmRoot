/** CbmMuchSector.cxx
 *@author  M.Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@since   15.03.07
 *@version 1.0
 **
 ** This class describes the digitization scheme for a sector of the MuCh.
 ** The sector is a rectangle of size fLx and fLy. The following
 ** types are implemented:
 ** 1 = GEMs
 ** 2 = CPCs
 **/

#include "CbmMuchSector.h"
#include "TPolyLine.h"
#include "CbmMuchPad.h"

#include "CbmMuchSectorDigiPar.h"

#include "TMath.h"

#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
CbmMuchSector::CbmMuchSector() {
  fDetectorId = 0;
  fX0 = fY0 = fRotation = fDx = fDy = fPadRadius = fLx = fLy = 0.;
  fNCols = fNRows = 0;
  fSinRot     = TMath::Sin(fRotation);
  fCosRot     = TMath::Cos(fRotation);
  fNChannels = 0;
  fSigmaX = fSigmaY = fSigmaXY = 0.;
  cout << "-W- CbmMuchSector: Do not use this constructor! " << endl;
}
// -------------------------------------------------------------------------



// -----   Enhanced constructor (by z0 and d)  ------------------------------------------
CbmMuchSector::CbmMuchSector(TString tempName, Int_t detId, Int_t iType, Double_t x0, Double_t y0, Double_t z0,
			   Double_t rotation, Double_t lx, Double_t ly, 
			   Double_t d, Int_t nCols, Int_t nRows, Int_t iChannels)
{
  fName = tempName.Data();
  fDetectorId = detId;
  fType       = iType;
  fX0         = x0;
  fY0         = y0;
  fZ0         = z0;  // z position of the station
  fRotation   = rotation;
  fSinRot     = TMath::Sin(fRotation);
  fCosRot     = TMath::Cos(fRotation);
  fLx         = lx;
  fLy         = ly;
  fD          = d;   // thickness of the station
  fNCols      = nCols;
  fNRows      = nRows;
  fDx         = fLx/fNCols;
  fDy         = fLy/fNRows;
  fPadRadius  = TMath::Sqrt(fDx*fDx + fDy*fDy)/2.;

  if ( fType == 1 || fType == 2) {       // GEMs or CPCs
    // Define number of channels
    fNChannels = iChannels;
    // calculate errors
    Double_t dx = fLx/fNCols;
    Double_t dy = fLy/fNRows;    
    fSigmaX  = dx / TMath::Sqrt(12);
    fSigmaY  = dy / TMath::Sqrt(12);
    fSigmaXY = 0.;
  }
  else {
    cout << "-E- CbmMuchSector: Illegal sector type " << fType << endl;
    Fatal("", "Illegal sector type");
  }


  // Transform errors to global c.s.
  Double_t vX  = fCosRot * fCosRot * fSigmaX * fSigmaX
               - 2. * fCosRot * fSinRot * fSigmaXY
               + fSinRot * fSinRot * fSigmaY * fSigmaY;
  Double_t vY  = fSinRot * fSinRot * fSigmaX * fSigmaX
               + 2. * fCosRot * fSinRot * fSigmaXY
               + fCosRot * fCosRot * fSigmaY * fSigmaY;
  Double_t vXY = fCosRot * fSinRot * fSigmaX * fSigmaX
               + ( fCosRot*fCosRot - fSinRot*fSinRot ) * fSigmaXY
               - fCosRot * fSinRot * fSigmaY * fSigmaY;
  fSigmaX  = TMath::Sqrt(vX);
  fSigmaY  = TMath::Sqrt(vY);
  fSigmaXY = vXY;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmMuchSector::CbmMuchSector(Int_t detId, Int_t iType, Double_t x0, 
			   Double_t y0, Double_t rotation, Double_t lx, 
			   Double_t ly, Int_t nCols, Int_t nRows, 
			   Int_t iChannels) {

  fDetectorId = detId;
  fType       = iType;
  fX0         = x0;
  fY0         = y0;
  fRotation   = rotation;
  fSinRot     = TMath::Sin(fRotation);
  fCosRot     = TMath::Cos(fRotation);
  fLx         = lx;
  fLy         = ly;
  fNCols      = nCols;
  fNRows      = nRows;
  fDx         = fLx/fNCols;
  fDy         = fLy/fNRows;
  fPadRadius  = TMath::Sqrt(fDx*fDx + fDy*fDy)/2.;

  if ( fType == 1 || fType == 2) {       // GEMs or CPCs
    // Define number of channels
    fNChannels = iChannels;
    // calculate errors
    Double_t dx = fLx/fNCols;
    Double_t dy = fLy/fNRows;    
    fSigmaX  = dx / TMath::Sqrt(12);
    fSigmaY  = dy / TMath::Sqrt(12);
    fSigmaXY = 0.;
  }
  else {
    cout << "-E- CbmMuchSector: Illegal sector type " << fType << endl;
    Fatal("", "Illegal sector type");
  }

  // Transform errors to global c.s.
  Double_t vX  = fCosRot * fCosRot * fSigmaX * fSigmaX
               - 2. * fCosRot * fSinRot * fSigmaXY
               + fSinRot * fSinRot * fSigmaY * fSigmaY;
  Double_t vY  = fSinRot * fSinRot * fSigmaX * fSigmaX
               + 2. * fCosRot * fSinRot * fSigmaXY
               + fCosRot * fCosRot * fSigmaY * fSigmaY;
  Double_t vXY = fCosRot * fSinRot * fSigmaX * fSigmaX
               + ( fCosRot*fCosRot - fSinRot*fSinRot ) * fSigmaXY
               - fCosRot * fSinRot * fSigmaY * fSigmaY;
  fSigmaX  = TMath::Sqrt(vX);
  fSigmaY  = TMath::Sqrt(vY);
  fSigmaXY = vXY;

}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmMuchSector::~CbmMuchSector() { };
// -------------------------------------------------------------------------



// -----   Public method GetChannel   --------------------------------------
Int_t CbmMuchSector::GetChannel(Double_t x, Double_t y) {
  // Calculate internal coordinates. Return -1 if outside the sector.
  Double_t xint  = 0;
  Double_t yint  = 0;
  if ( ! IntCoord(x, y, xint, yint) ) return -1;
  Int_t    iChan = -1;

  Double_t dx = fLx/fNCols;
  Double_t dy = fLy/fNRows;
  Int_t iCol = Int_t(xint / dx);
  Int_t iRow = Int_t(yint / dy);
  if(iCol == fNCols) iCol = fNCols - 1;  // stupid verification
  else if(iCol == -1) iCol = 0;
  if(iRow == fNRows) iRow = fNRows - 1;  // stupid verification
  else if (iRow == -1) iRow = 0;
  iChan = iRow * fNCols + iCol;

//   // DEBUG: verify correctness of fired pad
//   Double_t xPadInt = (Int_t)((2*iCol + 1)*fDx/2.);
//   Double_t yPadInt = (Int_t)((2*iRow + 1)*fDy/2.);
//   if(GetStationNr() == 2 && 
//      (TMath::Abs(xint-xPadInt) > fDx+1e-9 || TMath::Abs(yint-yPadInt) > fDy+1e-9))
//     cout << "Not proper pad fired" << endl
// 	 << "deltaX = " << TMath::Abs(xint-xPadInt) << "\t"
// 	 << "deltaY = " << TMath::Abs(yint-yPadInt) << "\n"
// 	 << "PadLx = " << fDx << "\t"
// 	 << "PadLy = " << fDy << "\n"
// 	 << "Channel Nr = " << iChan << endl;


  if (iChan < 0 || iChan > fNChannels-1) {
    cout << "-E- CbmMuchSector::GetChannel: "
	 << "Channel number " << iChan << " exceeds limit " 
	 << fNChannels << endl << endl;
    cout << "StationNr = " <<  GetStationNr() << "\t" 
	 << "SectorNr = " << GetSectorNr() << endl;
    cout << "x = " << x << "\t" << "y = " << y << endl;
    cout << "xint = " << xint << "\t" << "yint = " << yint << endl;
    cout << "Lx = " << fLx << "\t" << "Ly = " << fLy << endl;
    cout << "dx = " << dx << "\t" << "dy = " << dy << endl;
    cout << "fX0 = " << fX0 << "\t" << "fY0 = " << fY0 << endl;
    cout << "fNCol = " << fNCols << "\t" << "iCol = " << iCol << "iRow = " << iRow << endl << endl;
    Fatal("GetChannel", "illegal channel number");
  }

  return iChan;
}
// -------------------------------------------------------------------------

    



// -----   Public method Inside   ------------------------------------------
Bool_t CbmMuchSector::Inside(Double_t x, Double_t y) {
  Double_t xint, yint;
  return IntCoord(x, y, xint, yint);
}
// -------------------------------------------------------------------------



// -----   Public method ActivateChannels   --------------------------------
Bool_t CbmMuchSector::ActivateChannels(Int_t ipt, Double_t x, Double_t y) {

  Int_t iPad = GetChannel(x, y);
  if (iPad < 0) return kFALSE;
  
  fActive.insert(iPad);
  fTrueHits[iPad] = ipt;
  
  return kTRUE;
}
// -------------------------------------------------------------------------

  

// -----   Public method PointIndex   --------------------------------------
Int_t CbmMuchSector::GetPointIndex(Int_t iPad) {
  if (fTrueHits.find(iPad) == fTrueHits.end()) return -1;
  return fTrueHits[iPad];
}
// -------------------------------------------------------------------------



// -----   Public method Reset   -------------------------------------------
void CbmMuchSector::Reset() {
  fActive.clear();
  fTrueHits.clear();
}
// -------------------------------------------------------------------------



// -----   Public method Print   -------------------------------------------
void CbmMuchSector::Print() {
  cout << "   Sector Nr. ";
  cout.width(3);
  cout << GetSectorNr() << ", Type ";
  cout.width(1);
  cout << fType << ", centre (";
  cout.width(6);
  cout << fX0 << ", ";
  cout.width(6);
  cout << fY0 << ") cm, rotation " ;
  cout.width(6);
  cout << fRotation*180./TMath::Pi() << " deg., lx = ";
  cout.width(3);
  cout << fLx << " cm, ly = ";
  cout.width(3);
  cout << fLy << " cm, channels: " << GetNChannels() << endl;
}
// -------------------------------------------------------------------------



// -----   Private method IntCoord   ---------------------------------------
Bool_t CbmMuchSector::IntCoord(Double_t x, Double_t y,
			       Double_t& xint, Double_t& yint) const {

  // Translation into sector centre system
  x = x - fX0;
  y = y - fY0;

  // Rotation around the sector centre
  xint = x * fCosRot + y * fSinRot;
  yint = y * fCosRot - x * fSinRot;

  // Translation into sector corner system
  xint = xint + fLx/2.;
  yint = yint + fLy/2.;

  // Check whether point is inside the sector
  if ( ! IsInside(xint, yint) ) {
    //    xint = yint = 0.;
    return kFALSE;
  }

  return kTRUE;

}
// -------------------------------------------------------------------------



// -----   Private method IsInside   ---------------------------------------
Bool_t CbmMuchSector::IsInside(Double_t xint, Double_t yint) const {
  if ( xint < 0.  ) return kFALSE;
  if ( xint > fLx ) return kFALSE;
  if ( yint < 0.  ) return kFALSE;
  if ( yint > fLy ) return kFALSE;
  return kTRUE;
}
// -------------------------------------------------------------------------


// -----   Public method AddNeighbour  -------------------------------------
void CbmMuchSector::AddNeighbour(CbmMuchSector* sector){
  Int_t sectorNr = sector->GetSectorNr();
  if(sectorNr == GetSectorNr()) return;
  if(fNeighbours.find(sectorNr) == fNeighbours.end()){
    fNeighbours[sectorNr] = sector;
  }
}
// -------------------------------------------------------------------------

void CbmMuchSector::AddPads(){
  for(Int_t iChannel = 0; iChannel < fNChannels; iChannel++){
     CbmMuchPad* pad = new CbmMuchPad(this, iChannel);
     fPads.push_back(pad);
  }
}

void CbmMuchSector::GetPadVertices(Int_t iChannel, Double_t* xPad, Double_t* yPad){
	Int_t iRow = iChannel/fNCols;
	Int_t iCol = iChannel - iRow*fNCols;
	Double_t dx = fLx/fNCols;
	Double_t dy = fLy/fNRows;
	for (Int_t iVertex = 0; iVertex < 4 ; iVertex++){
		Double_t xInt = (iVertex < 2 ) ? iCol*dx : (iCol+1)*dx;
		Double_t yInt = (iVertex == 0 || iVertex ==3) ? iRow*dy : (iRow+1)*dy;
        // Translate to centre of the sector
		Double_t xc = xInt - fLx/2.;
		Double_t yc = yInt - fLy/2.;
		// Rotate
		Double_t xcGlob = xc*fCosRot - yc*fSinRot;
		Double_t ycGlob = yc*fCosRot + xc*fSinRot;
		// Translate to global system
		xPad[iVertex] = xcGlob + fX0;
		yPad[iVertex] = ycGlob + fY0;
	}
}
Double_t CbmMuchSector::GetPadX0(Int_t iChannel){
   CbmMuchPad* pad = fPads[iChannel];   
   return pad->GetX0();
}
Double_t CbmMuchSector::GetPadY0(Int_t iChannel){
   CbmMuchPad* pad = fPads[iChannel];
   return pad->GetY0();
}
TPolyLine CbmMuchSector::GetPadPolygon(Int_t iChannel){
   CbmMuchPad* pad = fPads[iChannel];   
   return pad->GetPadPolygon();
}


ClassImp(CbmMuchSector)
