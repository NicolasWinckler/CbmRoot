/** CbmMuchModuleGemRectangular.h
 *@author  E.Kryshen <e.kryshen@gsi.de>
 *@version 1.0
 *@since   03.08.12
 **/
#ifndef CBMMUCHMODULEGEMRECTANGULAR_H
#define CBMMUCHMODULEGEMRECTANGULAR_H 1

#include "CbmMuchModuleGem.h"
#include "vector"
using std::vector;

class CbmMuchSectorRectangular;
class CbmMuchPadRectangular;

class CbmMuchModuleGemRectangular : public CbmMuchModuleGem{
 public:
  CbmMuchModuleGemRectangular();
  CbmMuchModuleGemRectangular(Int_t iStation, Int_t iLayer, Bool_t iSide,
      Int_t iModule, TVector3 position, TVector3 size, Double_t cutRadius);
  virtual ~CbmMuchModuleGemRectangular() {}
  virtual Bool_t InitModule();
  CbmMuchSectorRectangular* GetSector(Double_t x, Double_t y);
  CbmMuchSectorRectangular* GetSector(Int_t ix, Int_t iy);
  CbmMuchPadRectangular* GetPad(Double_t x, Double_t y);
 protected:
  Bool_t fUseModuleDesign;
  Int_t fGridNx;    // Number of grid values in X direction
  Int_t fGridNy;    // Number of grid values in Y direction
  Double_t fGridDx; // X-width of the grid cell
  Double_t fGridDy; // Y-width of the grid cell
  vector<vector<Int_t> > fGrid; // 2D-vector of sector indices on the grid
  
  // Grid related private functions
  void InitNeighbourSectors();
  void InitNeighbourPads();
  Bool_t InitGrid(Bool_t useModuleDesign);
  Double_t GetInitX(CbmMuchSectorRectangular* sector);
  Double_t GetInitY(CbmMuchSectorRectangular* sector);
  Double_t GetGridCellX(Int_t ix);
  Double_t GetGridCellY(Int_t iy);


  ClassDef(CbmMuchModuleGemRectangular,1);
};
#endif
