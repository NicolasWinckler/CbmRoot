/** CbmMuchSectorRectangular.h
 *@author  E.Kryshen <e.kryshen@gsi.de>
 *@since   02.08.12
 *@version 2.0
 **
 ** This class describes the digitization scheme for a sector of the MuCh.
 ** The sector is a rectangle of size fLx and fLy.
 **
 **/
#ifndef CBMMUCHSECTORRECTANGULAR_H
#define CBMMUCHSECTORRECTANGULAR_H 1

#include "CbmMuchSector.h"
#include "TPave.h"
#include "TVector3.h"
#include "TArrayI.h"
#include "vector"
using std::vector;

class CbmMuchSectorRectangular : public CbmMuchSector{
public:
  CbmMuchSectorRectangular();
  CbmMuchSectorRectangular(Int_t detId, Int_t secId, TVector3 pos, TVector3 size, Int_t padNx, Int_t padNy);
  virtual ~CbmMuchSectorRectangular(){};
//  virtual void GetPadVertices(Int_t iChannel, Double_t* xPad, Double_t* yPad);
  TVector3 GetPosition() const { return fPosition; }
  TVector3 GetSize()  const { return fSize; }
  Int_t    GetPadNx() const { return fPadNx; } 
  Int_t    GetPadNy() const { return fPadNy; }
  Double_t GetPadDx() const { return fPadDx; }
  Double_t GetPadDy() const { return fPadDy; }
  Double_t GetSigmaX() const { return fPadDx/12; }
  Double_t GetSigmaY() const { return fPadDy/12; }
//  TArrayI  GetNeighbours() const { return fNeighbours; }
  void SetNeighbours(TArrayI &array) { fNeighbours = array; }
  Bool_t Inside(Double_t x, Double_t y) const {return 0; } // TODO
  vector<CbmMuchSectorRectangular*> GetNeighbours();
  void AddPads();
  void DrawPads();
  void Draw() { DrawPads(); }

protected:
  TVector3 fPosition;
  TVector3 fSize;
  Int_t fPadNx;
  Int_t fPadNy;
  Double_t fPadDx;
  Double_t fPadDy;
  TArrayI fNeighbours;

  ClassDef(CbmMuchSectorRectangular,1);

};

#endif
