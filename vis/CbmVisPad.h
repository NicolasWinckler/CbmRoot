// -------------------------------------------------------------------------
// -----                       CbmVisPad header file                   -----
// -----                  Created 28/03/07  by E. Kryshen              -----
// -------------------------------------------------------------------------


/** CbmVisPad
 *@author Evgeny Kryshen <e.kryshen@gsi.de>
 *@since 28.03.07
 *@version 1.0
 **
 **/
#ifndef CBMVISPAD_H
#define CBMVISPAD_H 1


#include "TPolyLine.h"
class CbmMuchSector;

class CbmVisPad: public TPolyLine{
  ClassDef(CbmVisPad,1);
public:
  CbmVisPad(Double_t* x, Double_t* y, Int_t iChannel, Int_t iCharge,CbmMuchSector* fSector);
  ~CbmVisPad(){}
  void    SetFired(Int_t digiId);
  Bool_t  IsFired() {return fFired;}
  void    DrawPad();
  TString GetInfo();
  void    SetCharge(Int_t charge) {fCharge = charge; }
  Int_t   GetCharge()  { return fCharge;  }
  Int_t   GetChannel() { return fChannel; }
  CbmMuchSector* GetSector() { return fSector; }
  Int_t   GetDigiId()  { return fDigiId; }
protected:
  Int_t  fDigiId;
  Int_t  fChannel;
  Bool_t fFired;
  Int_t  fCharge;
  CbmMuchSector* fSector;
};

#endif
