// -------------------------------------------------------------------------
// -----                     CbmVisPad source file                     -----
// -----                  Created 14/03/07  by E. Kryshen              -----
// -------------------------------------------------------------------------

#include "CbmVisPad.h"
#include "TPolyLine.h"
#include "TColor.h"
#include "CbmMuchSector.h"


// -------------------------------------------------------------------------
CbmVisPad::CbmVisPad(Double_t* x,Double_t* y, Int_t iChannel, Int_t charge,
                     CbmMuchSector* sector
  ):TPolyLine(5,x,y),fCharge(charge),fChannel(iChannel),fFired(0)
{
  fDigiId = -1;
  fSector=sector;
  SetFillColor(0);
  SetLineWidth(1);
  SetLineColor(34);
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmVisPad::SetFired(Int_t digiId){
  fDigiId = digiId;
  if (fDigiId>=0) fFired = 1; else fFired = 0;
  Int_t ch = fCharge/1000;
  if (ch>255) ch = 255;
  if (ch<10) ch = 10;

  if (fFired) SetFillColor(TColor::GetColor(255-ch,255-ch,245));
  else SetFillColor(kYellow);
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmVisPad::DrawPad(){
  Draw("f");
  Draw();
}
// -------------------------------------------------------------------------

TString CbmVisPad::GetInfo(){
    return Form("Channel:%i fired:%i charge:%i digiId:%i",
		 fChannel,fFired,fCharge,fDigiId);
}

ClassImp(CbmVisPad)
