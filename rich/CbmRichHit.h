// -------------------------------------------------------------------------
// -----                     CbmRichHit header file                    -----
// -----               Created 28/04/04  by B. Polichtchouk            -----
// -----                  modified 13/07/04  by C. Hoehne              -----
// -------------------------------------------------------------------------


/**  CbmRichHit.h
 *@author B. Polichtchouk
 **
 ** Hits of MC tracks in Rich Photodetector including
 ** detector geometry and efficiency
 **/


#ifndef CBMRICHHIT_H
#define CBMRICHHIT_H

#include "CbmHit.h"

class CbmRichHit : public CbmHit {

public:    
  
  /** Default constructor **/
  CbmRichHit();

  /** Destructor **/
  virtual ~CbmRichHit();    
  
  /** Output to screen **/
  virtual void Print(const Option_t* opt ="") const;
  
  /** Modifiers **/
  virtual void SetPmtID    (Int_t det)    {fPmtID    =det;  }
  virtual void SetNPhotons (Int_t n)      {fNPhotons = n;   }
  virtual void SetAmplitude(Double_t amp) {fAmplitude = amp;}
  
  /** Accessors **/
  virtual Int_t GetPmtID()   {return fPmtID;  }
  virtual Double_t X()  {return fX;}
  virtual Double_t Y()  {return fY;}
  virtual Double_t Z()  {return fZ;}
  virtual Double_t Dx() {return fDx;}
  virtual Double_t Dy() {return fDy;}
  virtual Double_t Dz() {return fDz;}
  virtual Int_t    GetNPhotons() {return fNPhotons; }
  virtual Double_t GetAmplitude(){return fAmplitude;}
  
protected:

  Int_t      fPmtID;     // photomultiplier number
  Int_t      fNPhotons;  // number of photons in this hit
  Double32_t fAmplitude; // hit amplitude

  ClassDef(CbmRichHit,1)
};

#endif //CBMRICHHIT_H
