// -------------------------------------------------------------------------
// -----                     CbmMuchPad source file                    -----
// -----                  Created 26/10/07  by E. Kryshen              -----
// -------------------------------------------------------------------------

#include "CbmMuchPad.h"
#include "CbmMuchGeoScheme.h"
#include "TColor.h"
#include <iostream>

using std::cout;
using std::endl;

// -----  Defaul constructor  ----------------------------------------------
CbmMuchPad::CbmMuchPad():TPolyLine(){}
// -------------------------------------------------------------------------

// -----  Standard constructor  --------------------------------------------
CbmMuchPad::CbmMuchPad (CbmMuchSector* sector, Int_t iChannel):TPolyLine(){
  // Generate detectorId
  Long64_t sectorId = sector->GetDetectorId();
  fDetectorId = CbmMuchGeoScheme::GetDetIdFromSector(sectorId, iChannel);

  fSector = *sector;
  TVector3 secPos = fSector.GetPosition();
  TVector3 secSize = fSector.GetSize();
  Double_t secX0 = secPos[0];
  Double_t secY0 = secPos[1];
  Double_t dx = fSector.GetDx();
  Double_t dy = fSector.GetDy();
  Double_t secLx = secSize[0];
  Double_t secLy = secSize[1];
  Int_t nCols = fSector.GetNCols();
  Int_t nRows = fSector.GetNRows();
  Double_t xVertices[5], yVertices[5];
  
  // Create TPolyLine for this pad
  Int_t iRow = iChannel/nCols;
  Int_t iCol = iChannel - iRow*nCols;
  for (Int_t iVertex = 0; iVertex < 4 ; iVertex++){
    Double_t xInt = (iVertex < 2 ) ? iCol*dx : (iCol+1)*dx;
    Double_t yInt = (iVertex == 0 || iVertex ==3) ? iRow*dy : (iRow+1)*dy;
    // Translate to centre of the sector
    Double_t xc = xInt - secLx/2.;
    Double_t yc = yInt - secLy/2.;
    // Translate to global system
    xVertices[iVertex] = xc + secX0;
    yVertices[iVertex] = yc + secY0;
  }
  xVertices[4] = xVertices[0];
  yVertices[4] = yVertices[0];
  
  fX0 = (xVertices[1] + xVertices[3])/2.;
  fY0 = (yVertices[0] + yVertices[2])/2.;
  SetPolyLine(5, xVertices, yVertices);
  SetFillColor(0);
  SetLineWidth(1);
  SetLineColor(34);
}
// -------------------------------------------------------------------------


// ------  Destructor  -----------------------------------------------------
CbmMuchPad::~CbmMuchPad(){
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
vector<CbmMuchPad*> CbmMuchPad::GetNeighbours(){
  vector<CbmMuchPad*> pads;
  CbmMuchGeoScheme* geoScheme = CbmMuchGeoScheme::Instance();
  for(Int_t i=0; i < fNeighbours.GetSize(); i++){
    Long64_t detectorId = fNeighbours.At(i);
    CbmMuchPad* pad = geoScheme->GetPadByDetId(detectorId);
    if(pad) pads.push_back(pad);
  }
  return pads;
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void CbmMuchPad::Reset(){
  fDigiIndex = -1;
  fCharge = 0;
  fFired = 0;
  SetFillColor(kYellow);
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmMuchPad::SetFired(Int_t digiId, Int_t charge, Int_t ADCcharge){
  fDigiIndex = digiId;
  fCharge = charge;
  if (fDigiIndex>=0) fFired = 1; else fFired = 0;
//  Int_t ch = fCharge/1000;
//  if (ch>255) ch = 255;
//  if (ch<10) ch = 10;
//  if (fFired) SetFillColor(TColor::GetColor(255-ch,255-ch,245));
  if (fFired) SetFillColor(TColor::GetColor(255-ADCcharge,255-ADCcharge,245));
  else SetFillColor(kYellow-7);
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmMuchPad::DrawPad(){
  Draw("f");
  Draw();
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
TString CbmMuchPad::GetInfo(){
    return Form("Channel:%i fired:%i charge:%i digiId:%i",
         CbmMuchGeoScheme::GetChannelIndex(fDetectorId),fFired,fCharge,fDigiIndex);
}
// -------------------------------------------------------------------------

ClassImp(CbmMuchPad)
