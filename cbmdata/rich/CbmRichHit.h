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

#include "CbmPixelHit.h"

class CbmRichHit : public CbmPixelHit {

public:

  /** Default constructor **/
  CbmRichHit();

  /** Destructor **/
  virtual ~CbmRichHit();

  virtual void Print() const;
  /** Modifiers **/
  virtual void SetPmtId    (Int_t det)    {fPmtId    =det;  }
  virtual void SetNPhotons (Int_t n)      {fNPhotons = n;   }
  virtual void SetAmplitude(Double_t amp) {fAmplitude = amp;}

  /** Accessors **/
  virtual Int_t GetPmtId()   {return fPmtId; }
  virtual Int_t    GetNPhotons() {return fNPhotons; }
  virtual Double_t GetAmplitude(){return fAmplitude;}
  virtual Int_t GetPlaneId() const {return 0;}

protected:

  Int_t      fPmtId;     // photomultiplier number
  Int_t      fNPhotons;  // number of photons in this hit
  Double32_t fAmplitude; // hit amplitude

  ClassDef(CbmRichHit,1)
};

#endif //CBMRICHHIT_H
