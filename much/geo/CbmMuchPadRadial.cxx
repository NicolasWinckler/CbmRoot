/** CbmMuchPadRadial.h
 *@author Evgeny Kryshen <e.kryshen@gsi.de>
 *@since 6.02.12
 *@version 1.0
 **
 ** Class describing a single radial pad
 **/
#include "CbmMuchPadRadial.h"
#include "CbmMuchGeoScheme.h"
#include "TColor.h"
#include "TMath.h"

// -----  Default constructor  ----------------------------------------------
CbmMuchPadRadial::CbmMuchPadRadial()
  : CbmMuchPad(),TCrown()
{
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
CbmMuchPadRadial::CbmMuchPadRadial(Int_t detId, Long64_t chanId, Double_t r1, Double_t r2, Double_t phi1, Double_t phi2)
  : CbmMuchPad(detId,chanId,(r1+r2)/2.*cos((phi1+phi2)/2.),(r1+r2)/2.*sin((phi1+phi2)/2.),r2-r1,r2-r1),
    TCrown(0,0,r1,r2,phi1*TMath::RadToDeg(),phi2*TMath::RadToDeg()),
    fPhi1(phi1),
    fPhi2(phi2)
{
  SetFillColor(0);
  SetLineWidth(1);
  SetLineColor(34);
//  Double_t dr = (r2-r1)/12.;
//  Double_t dp = (phi2-phi1)*r1/12.;
//  Double_t phi0 = (phi2+phi1)/2.;
//  Double_t c = cos(phi0);
//  Double_t s = sin(phi0);
//  Double_t wx  = dr*dr*c*c+dp*dp*s*s;
//  Double_t wy  = dr*dr*s*s+dp*dp*c*c;
//  Double_t wxy = (dr*dr-dp*dp)*c*s;
}
// -------------------------------------------------------------------------

//
//// -------------------------------------------------------------------------
//void CbmMuchPadRadial::Reset(){
//  fDigiIndex = -1;
//  SetFillColor(kYellow);
//}
//// -------------------------------------------------------------------------
//
// -------------------------------------------------------------------------
void CbmMuchPadRadial::SetFired(Int_t iDigi, Int_t ADCcharge, Int_t nADCChannels){
  fDigiIndex = iDigi;
  if (fDigiIndex>=0) SetFillColor(TColor::GetColor(nADCChannels-1-ADCcharge,nADCChannels-1-ADCcharge,245));
  else SetFillColor(kYellow);
}
// -------------------------------------------------------------------------
//
// -------------------------------------------------------------------------
void CbmMuchPadRadial::DrawPad(){
  Draw("f");
  Draw();
}
// -------------------------------------------------------------------------

ClassImp(CbmMuchPadRadial)
