/** CbmMuchModuleGemRectangular.cxx
 *@author  M.Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@version 1.0
 *@since   11.02.08
 **
 ** This class holds the transport geometry parameters
 ** of one side of MuCh module.
 **/

#include "CbmMuchModuleGemRectangular.h"
#include "CbmMuchSectorRectangular.h"
#include "CbmMuchPad.h"
#include "CbmMuchGeoScheme.h"

#include <algorithm>
#include <vector>
using std::vector;

// -----   Default constructor   -------------------------------------------
CbmMuchModuleGemRectangular::CbmMuchModuleGemRectangular()
  : CbmMuchModuleGem(),
    fUseModuleDesign(kFALSE),
    fGridDx(0.),
    fGridDy(0.),
    fGridNx(0),
    fGridNy(0),
    fGrid()
{
    fDetectorType = 1;
}
// -------------------------------------------------------------------------


// -----   Standard constructor   ------------------------------------------
CbmMuchModuleGemRectangular::CbmMuchModuleGemRectangular(Int_t iStation, Int_t iLayer, Bool_t iSide,
    Int_t iModule, TVector3 position, TVector3 size, Double_t cutRadius)
  : CbmMuchModuleGem(iStation, iLayer, iSide, iModule, position, size, cutRadius),
    fUseModuleDesign(kFALSE),
    fGridDx(0.),
    fGridDy(0.),
    fGridNx(0),
    fGridNy(0),
    fGrid()
{
    fDetectorType = 1;
}
// -------------------------------------------------------------------------


// -----   Public method GetSector   ---------------------------------------
CbmMuchSectorRectangular* CbmMuchModuleGemRectangular::GetSector(Int_t ix, Int_t iy) {
  if(ix < 0 || ix >= fGridNx) return NULL;
  if(iy < 0 || iy >= fGridNy) return NULL;
  Long64_t iSector = fGrid[ix][iy];
  if (iSector==-1) return NULL;
  return (CbmMuchSectorRectangular*) fSectors[iSector];
}
// -------------------------------------------------------------------------


// -----   Public method GetSector   ---------------------------------------
CbmMuchSectorRectangular* CbmMuchModuleGemRectangular::GetSector(Double_t x, Double_t y){
  Int_t nX = fPosition[0] < 0 ? -1 : 1;
  Int_t nY = fPosition[1] < 0 ? -1 : 1;

  // Get module corner nearest to the layer side center for module design
  Double_t xCorner = fUseModuleDesign ? fPosition[0] - nX*fSize[0]/2. : -fGridDx*fGridNx/2.;
  Double_t yCorner = fUseModuleDesign ? fPosition[1] - nY*fSize[1]/2. : -fGridDy*fGridNy/2.;

  // Translate (x,y) to the module corner system
  Double_t xM = fUseModuleDesign ? nX*(x - xCorner) : x - xCorner;
  Double_t yM = fUseModuleDesign ? nY*(y - yCorner) : y - yCorner;

  Int_t ix = xM < 0 ? -1 : Int_t(xM/fGridDx);
  Int_t iy = yM < 0 ? -1 : Int_t(yM/fGridDy);
  return GetSector(ix,iy);
}
// -------------------------------------------------------------------------

// -----   Public method GetGridCellX  -------------------------------------
Double_t CbmMuchModuleGemRectangular::GetGridCellX(Int_t ix){
  Int_t nX;
  if(fUseModuleDesign){
    nX = fPosition[0] < 0 ? -1 : 1;
    return nX*((ix + 1e-3) * fGridDx - fSize[0] / 2.) + fPosition[0];
  }
  else{
    Double_t x = -fGridDx*fGridNx/2. + (ix + 0.5)*fGridDx;
    nX = x<0 ? -1 : 1;
    return x - nX*(fGridDx/2. - 1e-3);
  }
}
// -------------------------------------------------------------------------

// -----   Public method GetGridCellY  -------------------------------------
Double_t CbmMuchModuleGemRectangular::GetGridCellY(Int_t iy){
  Int_t nY;
  if(fUseModuleDesign){
    nY = fPosition[1] < 0 ? -1 : 1;
    return nY*((iy + 1e-3) * fGridDy - fSize[1] / 2.) + fPosition[1];
  }
  else{
    Double_t y = -fGridDy*fGridNy/2. + (iy + 0.5)*fGridDy;
    nY = y<0 ? -1 : 1;
    return y - nY*(fGridDy/2. - 1e-3);
  }
}
// -------------------------------------------------------------------------

// -----   Public method GetInitX  -----------------------------------------
Double_t CbmMuchModuleGemRectangular::GetInitX(CbmMuchSectorRectangular* sector){
  Int_t nX;
  Double_t secLx = sector->GetSize()[0];
  Double_t secX0 = sector->GetPosition()[0];

  if(fUseModuleDesign){
    nX = fPosition[0] < 0 ? -1 : 1;
    Double_t xCorner = fPosition[0] - nX*fSize[0]/2.;
    // Translate sector center to the module corner (nearest to the layer center) system
    Double_t x = nX*(secX0 - xCorner);
    return x - secLx/2. - fGridDx/2.;
  }
  else{
    nX = secX0 < 0 ? -1 : 1;
    // Translate to right bottom corner of the grid
    Double_t xCorner = -fGridDx*fGridNx/2.;
    return secX0 - nX*(fGridDx/2.) - xCorner;
  }
}
// -------------------------------------------------------------------------

// -----   Public method GetInitY  -----------------------------------------
Double_t CbmMuchModuleGemRectangular::GetInitY(CbmMuchSectorRectangular* sector){
  Int_t nY;
  Double_t secLy = sector->GetSize()[1];
  Double_t secY0 = sector->GetPosition()[1];

  if(fUseModuleDesign){
    nY = fPosition[1] < 0 ? -1 : 1;
    Double_t yCorner = fPosition[1] - nY*fSize[1]/2.;
    // Translate sector center to the module corner (nearest to the layer center) system
    Double_t y = nY*(secY0 - yCorner);
    return y - secLy/2. - fGridDy/2.;
  }
  else{
    nY = secY0 < 0 ? -1 : 1;
    Double_t yCorner = -fGridDy*fGridNy/2.;
    // Translate to right bottom corner of the grid
    return secY0 - nY*(fGridDy/2.) - yCorner;
  }
}
// -------------------------------------------------------------------------

// -----   Public method InitGrid  -----------------------------------------
Bool_t CbmMuchModuleGemRectangular::InitGrid(Bool_t useModuleDesign) {
  if (GetNSectors() == 0) return kFALSE;
  fUseModuleDesign = useModuleDesign;

  CbmMuchSectorRectangular* muchSector = (CbmMuchSectorRectangular*) fSectors[0];
  Int_t nChannels = muchSector->GetNChannels();
  TVector3 size = muchSector->GetSize();
  fGridDx = size[0];
  fGridDy = size[1];
  Int_t nSectors = GetNSectors();
  for (Int_t iSector = 0; iSector < nSectors; iSector++) {
    CbmMuchSectorRectangular* s = (CbmMuchSectorRectangular*) fSectors[iSector];
    if(s->GetNChannels() < nChannels) continue; // Process only complete sectors
    TVector3 secSize = s->GetSize();
    if (fGridDx > secSize[0]) fGridDx = secSize[0];
    if (fGridDy > secSize[1]) fGridDy = secSize[1];
  }

  Int_t nCell = fUseModuleDesign ? 1 : 2; // Number of additional columns/rows in the grid
  fGridNx = Int_t ((fSize[0] + 1e-5) / fGridDx) + nCell;
  fGridNy = Int_t ((fSize[1] + 1e-5) / fGridDy) + nCell;

  // Fill cells with sector numbers
  fGrid.resize(fGridNx);
  for (int ix = 0; ix < fGridNx; ix++) {
    fGrid[ix].resize(fGridNy);
    Double_t x = GetGridCellX(ix);
    for (int iy = 0; iy < fGridNy; iy++) {
      Double_t y = GetGridCellY(iy);
      bool result = kFALSE;
      for (int iSector = 0; iSector < nSectors; iSector++) {
        CbmMuchSectorRectangular* sec = (CbmMuchSectorRectangular*) fSectors[iSector];
        if(!sec) continue;
        result = sec->Inside(x, y);
        if (result) {
          fGrid[ix][iy] = sec->GetSectorId();
          break;
        }
      }
      if (!result) fGrid[ix][iy] = -1;
    }
  }
  return kTRUE;
}
// -------------------------------------------------------------------------

// ------ Public method InitNeighbours  ------------------------------------
void CbmMuchModuleGemRectangular::InitNeighbourSectors() {
  for (Int_t iSector = 0; iSector < GetNSectors(); iSector++) {
    CbmMuchSectorRectangular* sector = (CbmMuchSectorRectangular*) fSectors[iSector];
    if (!sector) continue;
    vector<Int_t> neighbours;
    Double_t secLx = sector->GetSize()[0];
    Double_t secLy = sector->GetSize()[1];

    Int_t nx = Int_t ((secLx + 1e-3) / fGridDx) + 2;
    Int_t ny = Int_t ((secLy + 1e-3) / fGridDy) + 2;
    Double_t x = GetInitX(sector);
    Double_t y = GetInitY(sector);

    Int_t gx = x < 0 ? -1 : Int_t (x / fGridDx);
    Int_t gy = y < 0 ? -1 : Int_t (y / fGridDy);

    for (Int_t ix = gx; ix < gx + nx; ix++) {
      if (ix < 0 || ix>=fGridNx) continue;
      for (Int_t iy = gy; iy < gy + ny; iy++) {
        if (iy < 0 || iy >= fGridNy) continue;
        CbmMuchSectorRectangular* sec = GetSector(ix,iy);
        if (!sec) continue;
        Int_t iSec = sec->GetSectorId();
        if(iSec==iSector) continue;
        vector<Int_t>::iterator it = find(neighbours.begin(),neighbours.end(), iSec);
        if (it == neighbours.end()) neighbours.push_back(iSec);
      }
    }

    // Set TArrayI
    Int_t nSize = neighbours.size();
    if (nSize == 0) continue;
    TArrayI array(nSize, &neighbours[0]);
    sector->SetNeighbours(array);
  }
}
// -------------------------------------------------------------------------

// ------ Public method InitNeighbourPads ----------------------------------
void CbmMuchModuleGemRectangular::InitNeighbourPads() {
//  // Loop over all sectors within the module
//  for (Int_t iSector = 0; iSector < GetNSectors(); iSector++) {
//    CbmMuchSectorRectangular* sector =(CbmMuchSectorRectangular*) fSectors[iSector];
//    if (!sector) continue;
//    Int_t nx = sector->GetPadNx();
//    Int_t ny = sector->GetPadNy();
//    Double_t secDx = sector->GetPadDx();
//    Double_t secDy = sector->GetPadDy();
//    vector<CbmMuchSectorRectangular*> neighbourSectors = sector->GetNeighbours();
//
//    // Define minimal pad size among neighbour sectors
//    Double_t minDx = secDx;
//    Double_t minDy = secDy;
//    for (vector<CbmMuchSectorRectangular*>::iterator it = neighbourSectors.begin(); it != neighbourSectors.end(); it++) {
//      CbmMuchSectorRectangular* s = *it;
//      Double_t dx = s->GetPadDx();
//      Double_t dy = s->GetPadDy();
//      if (dx < minDx) minDx = dx;
//      if (dy < minDy) minDy = dy;
//    }
//    Int_t iWidth  = Int_t ((secDx + minDx / 2.) / minDx);
//    Int_t iLength = Int_t ((secDy + minDy / 2.) / minDy);
//
//    // Search for neighbour pads for each given pad
//    for (Int_t iChannel = 0; iChannel < sector->GetNChannels(); iChannel++) {
//      CbmMuchPad* pad = sector->GetPad(iChannel);
//      if (!pad) continue;
//      vector<Long64_t> neighbours;
//      Long64_t channelId = pad->GetChannelId();
//      Int_t ix = iChannel/nx;
//      Int_t iy = iChannel%nx;
//
//      // If channel is within the sector
//      if (ix > 0 && ix <nx-1 && iy > 0 && iy < ny-1) {
//        for (Int_t i=ix-1;i<ix+2;i++) { // columns
//          for (Int_t j=iy-1;j<iy+2;j++) { // rows
//            Int_t chan = j*ny + i;
//            CbmMuchPad* p = sector->GetPad(chan);
//            if(!p) continue;
//            Long64_t chanId = p->GetChannelId();
//            if (chanId == channelId) continue;
//            vector<Long64_t>::iterator it = find(neighbours.begin(), neighbours.end(),chanId);
//            if (it == neighbours.end()) neighbours.push_back(chanId);
//          }
//        }
//      }
//      // If channel is on the sector's border
//      else {
//        // Pad center in global system
//        Double_t padX0 = pad->GetX();
//        Double_t padY0 = pad->GetY();
//        // Initial coordinate for loop
//        Double_t xInit = padX0 - secDx / 2. - minDx / 2.;
//        Double_t yInit = padY0 - secDy / 2. - minDy / 2.;
//        for (Int_t i = 0; i < iWidth + 2; i++) {
//          Double_t x = xInit + i * minDx;
//          for (Int_t j = 0; j < iLength + 2; j++) {
//            Double_t y = yInit + j * minDy;
//            CbmMuchSectorRectangular* sec = GetSector(x, y);
//            if (!sec) continue;
//            // Calculate channel number
//            Int_t n_y = sec->GetPadNy();
//            Double_t dx = sec->GetPadDx();
//            Double_t dy = sec->GetPadDy();
//            Double_t lx = sec->GetSize()[0];
//            Double_t ly = sec->GetSize()[1];
//            Double_t x0 = sec->GetPosition()[0];
//            Double_t y0 = sec->GetPosition()[1];
//            Double_t x_int = x - x0 + lx / 2.;
//            Double_t y_int = y - y0 + ly / 2.;
//            Int_t i_x = (Int_t) (x_int / dx);
//            Int_t i_y = (Int_t) (y_int / dy);
//            if (i_x < 0) i_x = 0;
//            else if (i_x > sec->GetPadNx() - 1) i_x = sec->GetPadNx() - 1;
//            if (i_y < 0) i_y = 0;
//            else if (i_y > sec->GetPadNy() - 1) i_y = sec->GetPadNy() - 1;
//            Int_t chan = i_x * n_y + i_y;
//            CbmMuchPad* p = sec->GetPad(chan);
//            if (!p) continue;
//            Long64_t chanId = p->GetChannelId();
//            if (chanId == channelId) continue;
//            vector<Long64_t>::iterator it = find(neighbours.begin(), neighbours.end(),chanId);
//            if (it == neighbours.end()) neighbours.push_back(chanId);
//          }
//        }
//      }
//      // Set TArrayL64
//      Int_t nSize = neighbours.size();
//      if (nSize == 0) continue;
//      TArrayL64 array(nSize, &neighbours[0]);
//      pad->SetNeighbours(array);
//    }
//  }
}
// -------------------------------------------------------------------------

Bool_t CbmMuchModuleGemRectangular::InitModule(){
  Bool_t useModuleDesign = TMath::Abs(fPosition[0]) > 1e-5 || TMath::Abs(fPosition[1]) > 1e-5;
  if (!InitGrid(useModuleDesign)) return kFALSE;
  InitNeighbourSectors();
  for (Int_t iSector = 0; iSector < GetNSectors(); iSector++) {
    CbmMuchSectorRectangular* sector = (CbmMuchSectorRectangular*) fSectors[iSector];
    if (!sector) continue;
    sector->AddPads();
  }
  InitNeighbourPads();
  return kTRUE;
}

ClassImp(CbmMuchModuleGemRectangular)
