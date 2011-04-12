#ifndef CBMECALSHLIB_H
#define CBMECALSHLIB_H

#include "FairTask.h"
#include "CbmEcalCell.h"

class CbmEcalShLib : public FairTask
{
public:
  CbmEcalShLib() {};
  CbmEcalShLib(const char* nm, Int_t verb) : FairTask(nm, verb) {};
  // Response to photon with energy e, polar angle phi (rad), impact angle
  // theta (rad). X, Y are position vs. photons impact point
  // Cell is a size os cell in cm.
  virtual Float_t GetResponse(Float_t x, Float_t y, Float_t phi, Float_t theta, Float_t e, Int_t celltype=0)=0;
  virtual Float_t GetResponse(Float_t x, Float_t y, Float_t phi, Float_t theta, Float_t e, CbmEcalCell* cell)
  {
    return GetResponse(x, y, phi, theta, e, cell->GetType());
  }
  virtual ~CbmEcalShLib()=0;
private:
  ClassDef(CbmEcalShLib, 1);
};

#endif
