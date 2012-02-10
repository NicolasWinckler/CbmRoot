/** CbmMuchRadialPad.h
 *@author Evgeny Kryshen <e.kryshen@gsi.de>
 *@since 6.02.12
 *@version 1.0
 **
 ** Class describing a single radial pad
 **/
#include "CbmMuchRadialPad.h"
#include "CbmMuchGeoScheme.h"
#include "TColor.h"
#include "CbmMuchModuleSector.h"
#include "CbmMuchRadialSector.h"
#include "TMath.h"

// -----  Default constructor  ----------------------------------------------
CbmMuchRadialPad::CbmMuchRadialPad():TCrown(){}
// -------------------------------------------------------------------------

// -----  Standard constructor  --------------------------------------------
CbmMuchRadialPad::CbmMuchRadialPad(CbmMuchRadialSector* sector, Int_t iChannel):TCrown(),fCharge(0),fTime(0){
  // Generate detectorId
  fDetectorId = sector->GetDetectorId();
  Int_t iSector = sector->GetSectorIndex();
  fChannelId = CbmMuchModuleSector::GetChannelId(iSector, iChannel);
  fR1 = sector->GetR1();
  fR2 = sector->GetR2();
  Int_t nPads = sector->GetNChannels();
  Double_t angle = sector->GetPadAngle()*180./TMath::Pi();
  Double_t phi0  = sector->GetPhi0()*180./TMath::Pi();
  if (fPhimax<fPhimin) printf("Warning!\n");
  fPhimin = (iChannel%2==0) ? phi0+(iChannel/2+0)*angle : phi0 - (iChannel/2+1)*angle;
  fPhimax = (iChannel%2==0) ? phi0+(iChannel/2+1)*angle : phi0 - (iChannel/2+0)*angle;
  fX1 = 0;
  fY1 = 0;
//  printf("Channel: %i %f R:Phi = %f-%f:%f-%f\n",iChannel,angle,fR1,fR2,fPhimin,fPhimax);
  SetFillColor(0);
  SetLineWidth(1);
  SetLineColor(34);
  fDigi = new CbmMuchDigi(fDetectorId,fChannelId,0,0,0);
  fMatch = new CbmMuchDigiMatch();
  
}
// -------------------------------------------------------------------------


// ------  Destructor  -----------------------------------------------------
CbmMuchRadialPad::~CbmMuchRadialPad(){
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
vector<CbmMuchRadialPad*> CbmMuchRadialPad::GetNeighbours(){// TODO
  vector<CbmMuchRadialPad*> pads;
//  CbmMuchGeoScheme* geoScheme = CbmMuchGeoScheme::Instance();
//  for(Int_t i=0; i < fNeighbours.GetSize(); i++){
//    Long64_t channelId = fNeighbours.At(i);
//    CbmMuchModuleGem* module = (CbmMuchModuleGem*) geoScheme->GetModuleByDetId(fDetectorId);
//    CbmMuchRadialPad* pad = module->GetPad(channelId);
//    if(pad) pads.push_back(pad);
//  }
  return pads;
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void CbmMuchRadialPad::Reset(){
  fDigiIndex = -1;
  SetFillColor(kYellow);
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void CbmMuchRadialPad::SetFired(Int_t iDigi, Int_t ADCcharge, Int_t nADCChannels){
  fDigiIndex = iDigi;
  if (fDigiIndex>=0) SetFillColor(TColor::GetColor(nADCChannels-1-ADCcharge,nADCChannels-1-ADCcharge,245));
  else SetFillColor(kYellow);
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void CbmMuchRadialPad::DrawPad(){
  Draw("f");
  Draw();
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
TString CbmMuchRadialPad::GetInfo(){ // TODO
  return "";
//  return Form("Channel:%i fired:%i digiId:%i",
//      CbmMuchModuleGem::GetChannelIndex(fChannelId),fDigiIndex>=0,fDigiIndex);
}
// -------------------------------------------------------------------------

ClassImp(CbmMuchRadialPad)
