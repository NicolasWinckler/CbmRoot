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

// -----  Default constructor  ----------------------------------------------
CbmMuchPad::CbmMuchPad():TPolyLine(){}
// -------------------------------------------------------------------------

// -----  Standard constructor  --------------------------------------------
CbmMuchPad::CbmMuchPad (CbmMuchSector* sector, Int_t iChannel):TPolyLine(){
  // Generate detectorId
  fDetectorId = sector->GetDetectorId();
  Int_t iSector = sector->GetSectorIndex();
  fChannelId = CbmMuchGeoScheme::GetChannelId(iSector, iChannel);

  TVector3 secPos = sector->GetPosition();
  TVector3 secSize = sector->GetSize();
  Double_t secX0 = secPos[0];
  Double_t secY0 = secPos[1];
  Double_t dx = sector->GetDx();
  Double_t dy = sector->GetDy();
  Double_t secLx = secSize[0];
  Double_t secLy = secSize[1];
  Int_t nCols = sector->GetNCols();
  Int_t nRows = sector->GetNRows();
  Double_t xVertices[5], yVertices[5];

  // Create TPolyLine for this pad
  Int_t iRow = iChannel/nCols;
  Int_t iCol = iChannel - iRow*nCols;
  for (Int_t iVertex = 0; iVertex < 4 ; iVertex++){
    Double_t xInt = (iVertex < 2 ) ? iCol*dx : (iCol+1)*dx;
    Double_t yInt = (iVertex == 0 || iVertex ==3) ? iRow*dy : (iRow+1)*dy;
    // Translate to center of the sector
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
Double_t CbmMuchPad::GetSectorX0() const{
	CbmMuchGeoScheme* geoScheme = CbmMuchGeoScheme::Instance();
	return geoScheme->GetSectorByDetId(fDetectorId, fChannelId)->GetPosition()[0];
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
Double_t CbmMuchPad::GetSectorY0() const{
	CbmMuchGeoScheme* geoScheme = CbmMuchGeoScheme::Instance();
	return geoScheme->GetSectorByDetId(fDetectorId, fChannelId)->GetPosition()[1];
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
Double_t CbmMuchPad::GetLx() const{
	CbmMuchGeoScheme* geoScheme = CbmMuchGeoScheme::Instance();
	return geoScheme->GetSectorByDetId(fDetectorId, fChannelId)->GetDx();
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
Double_t CbmMuchPad::GetLy() const{
	CbmMuchGeoScheme* geoScheme = CbmMuchGeoScheme::Instance();
	return geoScheme->GetSectorByDetId(fDetectorId, fChannelId)->GetDy();
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
vector<CbmMuchPad*> CbmMuchPad::GetNeighbours(){
  vector<CbmMuchPad*> pads;
  CbmMuchGeoScheme* geoScheme = CbmMuchGeoScheme::Instance();
  for(Int_t i=0; i < fNeighbours.GetSize(); i++){
    Int_t channelId = fNeighbours.At(i);
    CbmMuchPad* pad = geoScheme->GetPadByDetId(fDetectorId, channelId);
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
         CbmMuchGeoScheme::GetChannelIndex(fChannelId),fFired,fCharge,fDigiIndex);
}
// -------------------------------------------------------------------------

ClassImp(CbmMuchPad)
