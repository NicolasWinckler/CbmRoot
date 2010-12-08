#ifndef CBMECALMAXIMUM_H
#define CBMECALMAXIMUM_H

#include "TObject.h"

class CbmEcalCell;
/** A calorimeter maximum. Used for unification of photon reconstruction and
 ** electron identification procedures **/
class CbmEcalMaximum : public TObject
{
public:
  /** An emtry constructor **/
  CbmEcalMaximum() {};
  /** Simplest constructor **/
  CbmEcalMaximum(CbmEcalCell* cell);
  /** Standard constructor **/
  CbmEcalMaximum(CbmEcalCell* cell, Double_t cx, Double_t cy, Double_t x, Double_t y, Int_t i);
  ~CbmEcalMaximum() {};

  CbmEcalCell* Cell() const {return fCell;}
  Double_t CX() const {return fCX;}
  Double_t CY() const {return fCY;}
  Double_t X() const {return fX;}
  Double_t Y() const {return fY;}
  Int_t I() const {return fI;}
  Int_t Mark() const {return fMark;}

  void SetMark(Int_t mark) {fMark=mark;}
private:
  CbmEcalCell* fCell;
  /** Coordinates of cell **/
  Double_t fCX;
  Double_t fCY;
  /** Coobdinates of center of mass of maximum subcluster **/
  Double_t fX;
  Double_t fY;
  /** Number maximum subcluster **/
  Int_t fI;
  /** A mark. Used for maximum exclusion. **/
  Int_t fMark;

  ClassDef(CbmEcalMaximum, 1)
};

inline CbmEcalMaximum::CbmEcalMaximum(CbmEcalCell* cell, Double_t cx, Double_t cy, Double_t x, Double_t y, Int_t i)
  : fCell(cell), fCX(cx), fCY(cy), fX(x), fY(y), fI(i), fMark(0)
{
  ;
}
#endif

