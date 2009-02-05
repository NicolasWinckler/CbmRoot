// -------------------------------------------------------------------------
// -----                     CbmMuchPad source file                    -----
// -----                  Created 26/10/07  by E. Kryshen              -----
// -----                  Modified by Mikhail Ryzhinskiy               -----
// -------------------------------------------------------------------------

#include "CbmMuchPad.h"
#include <cassert>

// -------------------------------------------------------------------------
CbmMuchPad::CbmMuchPad(){}

CbmMuchPad::CbmMuchPad (CbmMuchSector* sector, Int_t iChannel){
    fSector = sector;
    Double_t secX0 = sector->GetX0();
    Double_t secY0 = sector->GetY0();
    Double_t dx = sector->GetDx();
    Double_t dy = sector->GetDy();
    Double_t secLx = sector->GetLx();
    Double_t secLy = sector->GetLy();
    Double_t sinRot = sector->GetSinRot();
    Double_t cosRot = sector->GetCosRot();
    Int_t nCols = sector->GetNCols();
    Int_t nRows = sector->GetNRows();
    Double_t xVertices[5], yVertices[5];
    fChannel = iChannel;
    
    // Create TPolyLine for this pad
    Int_t iRow = iChannel/nCols;
	 Int_t iCol = iChannel - iRow*nCols;
	 for (Int_t iVertex = 0; iVertex < 4 ; iVertex++){
   	Double_t xInt = (iVertex < 2 ) ? iCol*dx : (iCol+1)*dx;
		Double_t yInt = (iVertex == 0 || iVertex ==3) ? iRow*dy : (iRow+1)*dy;
      // Translate to centre of the sector
		Double_t xc = xInt - secLx/2.;
		Double_t yc = yInt - secLy/2.;
		// Rotate
		Double_t xcGlob = xc*cosRot - yc*sinRot;
		Double_t ycGlob = yc*cosRot + xc*sinRot;
		// Translate to global system
		xVertices[iVertex] = xcGlob + secX0;
		yVertices[iVertex] = ycGlob + secY0;
	 }
    xVertices[4] = xVertices[0];
    yVertices[4] = yVertices[0];
    fPadPolygon.SetPolyLine(5, xVertices, yVertices);

    fX0 = (xVertices[1] + xVertices[3])/2.;
    fY0 = (yVertices[0] + yVertices[2])/2.;
}

// ------  Destructor  -----------------------------------------------------
CbmMuchPad::~CbmMuchPad(){
}
// -------------------------------------------------------------------------

// -----   Public method AddNeighbour  -------------------------------------
void CbmMuchPad::AddNeighbour(CbmMuchPad* pad){
  Int_t iChannel = pad->GetChannel();
  Int_t detectorId = pad->GetDetectorId();//GetSectorNr();
  if(detectorId == GetDetectorId() && iChannel == fChannel) return;
  pair<Int_t, Int_t> id(detectorId, iChannel);
  if(fNeighbours.find(id) == fNeighbours.end()){
    fNeighbours[id] = pad;
  }
}
// -------------------------------------------------------------------------

