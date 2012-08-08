/** CbmMuchSectorRadial.cxx
 *@author  E.Kryshen <e.kryshen@gsi.de>
 *@since   6.02.12
 *@version 1.0
 **
 ** This class describes the digitization scheme for a ring of pads
 **
 **/


#include "CbmMuchSectorRadial.h"
#include "CbmMuchGeoScheme.h"
#include "CbmMuchPad.h"
#include "CbmMuchPadRadial.h"
#include "CbmMuchModuleGemRadial.h"
#include "TCrown.h"
#include "TMath.h"

// -----   Default constructor   -------------------------------------------
CbmMuchSectorRadial::CbmMuchSectorRadial() 
  : CbmMuchSector()
{
}
// -------------------------------------------------------------------------


// -----  Standard constructor  --------------------------------------------
CbmMuchSectorRadial::CbmMuchSectorRadial(Int_t detId, Int_t secId, 
    Double_t r1, Double_t r2, Double_t phi1, Double_t phi2)
  : CbmMuchSector(detId,secId,0),
    fR1(r1),
    fR2(r2),
    fPhi1(0),
    fPhi2(0),
    fPadDphi((r2-r1)/r1)
{
  fNChannels = 2*TMath::Ceil((phi2-phi1)/2/fPadDphi);
  fPhi1 = (phi1+phi2)/2-fNChannels/2*fPadDphi;
  fPhi2 = (phi1+phi2)/2+fNChannels/2*fPadDphi;
}
// -------------------------------------------------------------------------


// -----   Public method GetChannel   --------------------------------------
Int_t CbmMuchSectorRadial::GetPadIndexByPhi(Double_t phi) {
  if (fPhi2>TMath::TwoPi() && phi<fPhi1) phi+=TMath::TwoPi();
  if (phi<fPhi1 || phi>fPhi2) return -1;
  return TMath::Floor((phi-fPhi1)/fPadDphi);
}
// -------------------------------------------------------------------------


// -----   Public method GetChannel   --------------------------------------
CbmMuchPadRadial* CbmMuchSectorRadial::GetPadByPhi(Double_t phi) {
  Int_t i = GetPadIndexByPhi(phi);
//  printf("i=%i\n",i);
  if (i==-1) return NULL;
  
  return (CbmMuchPadRadial*) GetPad(i);
}
// -------------------------------------------------------------------------

//
//// -------------------------------------------------------------------------
//void CbmMuchSectorRadial::GetPadVertices(Int_t iPad, Double_t* x, Double_t* y){
//  Int_t i = iPad/2;
//  Int_t s = iPad%2==0 ? +1 : -1;
//  Double_t phi0 = (fPhi1+fPhi2)/2;
//  Double_t phi1 = phi0+s*i*fPadDphi;
//  Double_t phi2 = phi1+s*fPadDphi;
//  if (phi2>fPhi2) phi2 = fPhi2;
//  Double_t c1 = cos(phi1);
//  Double_t s1 = sin(phi1);
//  Double_t c2 = cos(phi2);
//  Double_t s2 = sin(phi2);
//  x[0] = fR1*c1;   y[0]=fR1*s1;
//  x[1] = fR2*c1;   y[1]=fR2*s1;
//  x[2] = fR2*c2;   y[2]=fR2*s2;
//  x[3] = fR1*c2;   y[3]=fR1*s2;
//  x[4] = x[0];     y[4]=y[0];
//}
//// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmMuchSectorRadial::AddPads(){
  for(Int_t iChannel = 0; iChannel < fNChannels; iChannel++){
     Long64_t channelId = CbmMuchModuleGem::GetChannelId(fSectorId,iChannel);
     Double_t r1,r2,phi1,phi2;
     r1 = fR1;
     r2 = fR2;
     phi1 = fPhi1+iChannel*fPadDphi;
     phi2 = phi1+fPadDphi;
     CbmMuchPad* pad = new CbmMuchPadRadial(fDetectorId,channelId,r1,r2,phi1,phi2);
     fPads.push_back(pad);
  }
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void CbmMuchSectorRadial::DrawPads(){
  for(Int_t iChannel = 0; iChannel < fNChannels; iChannel++){
     CbmMuchPadRadial* pad = (CbmMuchPadRadial*) fPads[iChannel];
     pad->DrawPad();
//     pad->Draw();
  }
}
// -------------------------------------------------------------------------


ClassImp(CbmMuchSectorRadial)
