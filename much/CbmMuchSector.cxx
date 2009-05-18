/** CbmMuchSector.cxx
 *@author  M.Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@since   15.03.07
 *@version 1.0
 **
 ** This class describes the digitization scheme for a sector of the MuCh.
 ** The sector is a rectangle of size fLx and fLy.
 **/

#include "CbmMuchSector.h"
#include "CbmMuchGeoScheme.h"
#include "CbmMuchPad.h"
#include "CbmMuchModuleGem.h"

#include "TPolyLine.h"
#include "TPave.h"
#include "TMath.h"

#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
CbmMuchSector::CbmMuchSector():TPolyLine(){
  fDetectorId = 0;
  fSectorIndex = 0;
  fDx = fDy = fPadRadius = 0.;
  fNCols = fNRows = 0;
  fNChannels = 0;
  fSigmaX = fSigmaY = 0.;
  fNeighbours.Set(0);
}
// -------------------------------------------------------------------------


// -----  Standard constructor  --------------------------------------------
CbmMuchSector::CbmMuchSector(Int_t detId, Int_t iSector, TVector3 position,  TVector3 size,
                             Int_t nCols, Int_t nRows):TPolyLine()
{
  fDetectorId  = detId;
  fSectorIndex = iSector;
  fPosition    = position;
  fSize        = size;
  fNCols       = nCols;
  fNRows       = nRows;

  fDx          = size.X()/fNCols;
  fDy          = size.Y()/fNRows;
  fPadRadius   = TMath::Sqrt(fDx*fDx + fDy*fDy)/2.;

  // Define number of channels
  fNChannels = nCols*nRows;
  // calculate errors
  fSigmaX  = fDx / TMath::Sqrt(12);
  fSigmaY  = fDy / TMath::Sqrt(12);

  fNeighbours.Set(0);

  Double_t x[5];
  Double_t y[5];
  x[0]= position[0]-size[0]/2;    y[0]=position[1]-size[1]/2.;
  x[1]= position[0]-size[0]/2;    y[1]=position[1]+size[1]/2.;
  x[2]= position[0]+size[0]/2;    y[2]=position[1]+size[1]/2.;
  x[3]= position[0]+size[0]/2;    y[3]=position[1]-size[1]/2.;
  x[4]=x[0];                      y[4]=y[0];

  SetPolyLine(5,x,y);
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

  Int_t iCol = Int_t(xint / fDx);
  Int_t iRow = Int_t(yint / fDy);
  if(iCol == fNCols) iCol = fNCols - 1;  // stupid verification
  else if(iCol == -1) iCol = 0;
  if(iRow == fNRows) iRow = fNRows - 1;  // stupid verification
  else if (iRow == -1) iRow = 0;
  iChan = iRow * fNCols + iCol;


  if (iChan < 0 || iChan > fNChannels-1) {
//     cout << "-E- CbmMuchSector::GetChannel: "
//          << "Channel number " << iChan << " exceeds limit "
//          << fNChannels << endl << endl;
//     cout << "StationNr = " <<  GetStationId() << "\t"
//          << "SectorNr = " << GetSectorId() << endl;
//     cout << "x = " << x << "\t" << "y = " << y << endl;
//     cout << "xint = " << xint << "\t" << "yint = " << yint << endl;
//     cout << "Lx = " << fSize[0] << "\t" << "Ly = " << fSize[1] << endl;
//     cout << "dx = " << fDx << "\t" << "dy = " << fDy << endl;
//     cout << "x = " << fPosition[0] << "\t" << "y = " << fPosition[1] << endl;
//     cout << "fNCol = " << fNCols << "\t" << "iCol = " << iCol << "iRow = " << iRow << endl << endl;
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


// -----   Public method Print   -------------------------------------------
void CbmMuchSector::PrintInfo() {
  cout << "   Sector Nr. ";
  cout.width(3);
  cout << fSectorIndex << ", centre (";
  cout.width(6);
  cout << fPosition[0] << ", ";
  cout.width(6);
  cout << fPosition[1] << ") cm, lx = ";
  cout.width(3);
  cout << fSize[0] << " cm, ly = ";
  cout.width(3);
  cout << fSize[1] << " cm, channels: " << fNChannels << endl;
}
// -------------------------------------------------------------------------


// -----   Private method IntCoord   ---------------------------------------
Bool_t CbmMuchSector::IntCoord(Double_t x, Double_t y,
			       Double_t& xint, Double_t& yint){

  // Translation into sector center system
  x = x - fPosition[0];
  y = y - fPosition[1];

  // Translation into sector corner system
  xint = x + fSize[0]/2.;
  yint = y + fSize[1]/2.;

  // Check whether point is inside the sector
  if ( ! IsInside(xint, yint) ) {
    //    xint = yint = 0.;
    return kFALSE;
  }

  return kTRUE;
}
// -------------------------------------------------------------------------


// -----   Private method IsInside   ---------------------------------------
Bool_t CbmMuchSector::IsInside(Double_t xint, Double_t yint){
  if ( xint < 0.  ) return kFALSE;
  if ( xint > fSize[0] ) return kFALSE;
  if ( yint < 0.  ) return kFALSE;
  if ( yint > fSize[1] ) return kFALSE;
  return kTRUE;
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
vector<CbmMuchSector*> CbmMuchSector::GetNeighbours(){
  vector<CbmMuchSector*> sectors;
  CbmMuchGeoScheme* geoScheme = CbmMuchGeoScheme::Instance();
  CbmMuchModuleGem* module = (CbmMuchModuleGem*) geoScheme->GetModuleByDetId(fDetectorId);
  for(Int_t i=0; i < fNeighbours.GetSize(); i++){
    Int_t iSector = fNeighbours.At(i);
    CbmMuchSector* sector = module->GetSector(iSector);
    if(sector) sectors.push_back(sector);
  }
  return sectors;
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void CbmMuchSector::AddPads(){
  for(Int_t iChannel = 0; iChannel < fNChannels; iChannel++){
     CbmMuchPad* pad = new CbmMuchPad(this, iChannel);
     fPads.Add(pad);
  }
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmMuchSector::GetPadVertices(Int_t iChannel, Double_t* xPad, Double_t* yPad){
  Int_t iRow = iChannel/fNCols;
  Int_t iCol = iChannel - iRow*fNCols;
  Double_t dx = fSize[0]/fNCols;
  Double_t dy = fSize[1]/fNRows;
  for (Int_t iVertex = 0; iVertex < 4 ; iVertex++){
    Double_t xInt = (iVertex < 2 ) ? iCol*dx : (iCol+1)*dx;
    Double_t yInt = (iVertex == 0 || iVertex ==3) ? iRow*dy : (iRow+1)*dy;
    // Translate to center of the sector
    Double_t xc = xInt - fSize[0]/2.;
    Double_t yc = yInt - fSize[1]/2.;
    // Translate to global system
    xPad[iVertex] = xc + fPosition[0];
    yPad[iVertex] = yc + fPosition[1];
  }
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
Double_t CbmMuchSector::GetPadX0(Int_t iChannel){
  CbmMuchPad* pad = (CbmMuchPad*)fPads.At(iChannel);
  return pad->GetX0();
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
Double_t CbmMuchSector::GetPadY0(Int_t iChannel){
  CbmMuchPad* pad = (CbmMuchPad*)fPads.At(iChannel);
  return pad->GetY0();
}
// -------------------------------------------------------------------------


ClassImp(CbmMuchSector)
