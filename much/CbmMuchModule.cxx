/** CbmMuchModule.cxx
 *@author  M.Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@version 1.0
 *@since   11.02.08
 **
 ** This class holds the transport geometry parameters
 ** of one side of MuCh module.
 **/

#include "CbmMuchModule.h"
#include "CbmMuchSector.h"
#include "CbmMuchPad.h"
#include "CbmMuchGeoScheme.h"

#include "TMath.h"
#include "TPolyLine.h"
#include "TPave.h"

#include <iostream>
#include <algorithm>
#include <vector>
#include <cassert>

using std::cout;
using std::endl;
using std::vector;

// -----   Default constructor   -------------------------------------------
CbmMuchModule::CbmMuchModule()
//:TPolyLine(){
:
    TPave() {
    fDetectorId = 0;
    fCutRadius = 0;
    fPosition = TVector3();
    fSize = TVector3();
    fGridDx = fGridDy = 0.0;
    fGridCols = fGridRows = 0;
    fPoints = NULL;
    fHits = NULL;
    fClusters = NULL;
    // cout << "-W- CbmMuchModule: Do not use this constructor! " << endl;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmMuchModule::CbmMuchModule(Int_t detId, TVector3 position, TVector3 size,
        Double_t cutRadius)
//:TPolyLine()
:
    TPave(position[0] - size[0] / 2, position[1] - size[1] / 2, position[0]
                                                                         + size[0] / 2, position[1] + size[1] / 2, 1) {
    fDetectorId = detId;
    fPosition = position;
    fSize = size;
    fCutRadius = cutRadius;
    fGridDx = fGridDy = 0.0;
    fGridCols = fGridRows = 0;
    fPoints = NULL;
    fHits = NULL;
    fClusters = NULL;
    Double_t x[5];
    Double_t y[5];
    x[0] = position[0] - size[0] / 2.;
    y[0] = position[1] - size[1] / 2.;
    x[1] = position[0] - size[0] / 2.;
    y[1] = position[1] + size[1] / 2.;
    x[2] = position[0] + size[0] / 2.;
    y[2] = position[1] + size[1] / 2.;
    x[3] = position[0] + size[0] / 2.;
    y[3] = position[1] - size[1] / 2.;
    x[4] = x[0];
    y[4] = y[0];

    //SetPolyLine(5,x,y);
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmMuchModule::CbmMuchModule(Int_t iStation, Int_t iLayer, Bool_t iSide,
        Int_t iModule, TVector3 position, TVector3 size, Double_t cutRadius)
//:TPolyLine()
:
    TPave(position[0] - size[0] / 2, position[1] - size[1] / 2, position[0]
                                                                         + size[0] / 2, position[1] + size[1] / 2, 1) {
    fDetectorId = CbmMuchGeoScheme::GetDetectorId(iStation, iLayer, iSide,
            iModule);
    fPosition = position;
    fSize = size;
    fCutRadius = cutRadius;
    fGridDx = fGridDy = 0.0;
    fGridCols = fGridRows = 0;
    fPoints = NULL;
    fHits = NULL;
    fClusters = NULL;
    Double_t x[5];
    Double_t y[5];
    x[0] = position[0] - size[0] / 2.;
    y[0] = position[1] - size[1] / 2.;
    x[1] = position[0] - size[0] / 2.;
    y[1] = position[1] + size[1] / 2.;
    x[2] = position[0] + size[0] / 2.;
    y[2] = position[1] + size[1] / 2.;
    x[3] = position[0] + size[0] / 2.;
    y[3] = position[1] - size[1] / 2.;
    x[4] = x[0];
    y[4] = y[0];

    //SetPolyLine(5,x,y);
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmMuchModule::~CbmMuchModule() {
}
// -------------------------------------------------------------------------

// -----   Public method AddSector   ---------------------------------------
void CbmMuchModule::AddSector(CbmMuchSector* sector) {
    fSectors.Add(sector);
}
// -------------------------------------------------------------------------

// -----   Public method GetSector   ---------------------------------------
CbmMuchSector* CbmMuchModule::GetSector(Int_t iGridColumn, Int_t iGridRow) {
    if(iGridColumn < 0 || iGridColumn > fGridCols - 1) return NULL;
    if(iGridRow < 0 || iGridRow > fGridRows - 1) return NULL;
    Int_t iSector = fGridIndices[iGridRow][iGridColumn];
    if (iSector != -1) {
        CbmMuchSector* sector = GetSector(iSector);
        return sector;
    }
    else
        return NULL;
}
// -------------------------------------------------------------------------

// -----   Public method GetSector   ---------------------------------------
CbmMuchSector* CbmMuchModule::GetSector(Double_t x, Double_t y){
    Int_t nX = fPosition[0] < 0 ? -1 : 1;
    Int_t nY = fPosition[1] < 0 ? -1 : 1;

    // Get module corner nearest to the layer side center
    Double_t xCorner = fUseModuleDesign ? fPosition[0] - nX*fSize[0]/2. : -fGridDx*fGridCols/2.;
    Double_t yCorner = fUseModuleDesign ? fPosition[1] - nY*fSize[1]/2. : -fGridDy*fGridRows/2.;

    // Translate (x,y) to the module corner system
    Double_t xM = fUseModuleDesign ? nX*(x - xCorner) : x - xCorner;
    Double_t yM = fUseModuleDesign ? nY*(y - yCorner) : y - yCorner;

    Int_t iCol = xM < 0 ? -1 : Int_t(xM/fGridDx);
    Int_t iRow = yM < 0 ? -1 : Int_t(yM/fGridDy);
    return GetSector(iCol, iRow);
}
// -------------------------------------------------------------------------

// -----   Public method GetGridCellX  -------------------------------------
Double_t CbmMuchModule::GetGridCellX(Int_t iCol){
    Int_t nX;
    if(fUseModuleDesign){
        nX = fPosition[0] < 0 ? -1 : 1;
        return nX*((iCol + 1e-3) * fGridDx - fSize[0] / 2.) + fPosition[0];
    }
    else{
        Double_t x = -fGridDx*fGridCols/2. + (iCol + 0.5)*fGridDx;
        nX = x<0 ? -1 : 1;
        return x - nX*(fGridDx/2. - 1e-3);
    }
}
// -------------------------------------------------------------------------

// -----   Public method GetGridCellY  -------------------------------------
Double_t CbmMuchModule::GetGridCellY(Int_t iRow){
    Int_t nY;
    if(fUseModuleDesign){
        nY = fPosition[1] < 0 ? -1 : 1;
        return nY*((iRow + 1e-3) * fGridDy - fSize[1] / 2.) + fPosition[1];
    }
    else{
        Double_t y = -fGridDy*fGridRows/2. + (iRow + 0.5)*fGridDy;
        nY = y<0 ? -1 : 1;
        return y - nY*(fGridDy/2. - 1e-3);
    }
}
// -------------------------------------------------------------------------

// -----   Public method GetInitX  -----------------------------------------
Double_t CbmMuchModule::GetInitX(CbmMuchSector* sector){
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
        Double_t xCorner = -fGridDx*fGridCols/2.;
        return secX0 - nX*(fGridDx/2.) - xCorner;
    }
}
// -------------------------------------------------------------------------

// -----   Public method GetInitY  -----------------------------------------
Double_t CbmMuchModule::GetInitY(CbmMuchSector* sector){
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
        Double_t yCorner = -fGridDy*fGridRows/2.;
        // Translate to right bottom corner of the grid
        return secY0 - nY*(fGridDy/2.) - yCorner;
    }
}
// -------------------------------------------------------------------------

// -----   Public method InitGrid  -----------------------------------------
Bool_t CbmMuchModule::InitGrid(Bool_t useModuleDesign) {
    if (GetNSectors() == 0) return kFALSE;
    fUseModuleDesign = useModuleDesign;

    CbmMuchSector* muchSector = GetSector(0);
    Int_t nChannels = muchSector->GetNChannels();
    TVector3 size = muchSector->GetSize();
    fGridDx = size[0];
    fGridDy = size[1];
    Int_t nSectors = GetNSectors();
    for (Int_t iSector = 0; iSector < nSectors; iSector++) {
        CbmMuchSector* s = GetSector(iSector);
        if(s->GetNChannels() < nChannels) continue; // Process only complete sectors
        TVector3 secSize = s->GetSize();
        if (fGridDx > secSize[0]) fGridDx = secSize[0];
        if (fGridDy > secSize[1]) fGridDy = secSize[1];
    }

    Int_t nCell = fUseModuleDesign ? 1 : 2; // Number of additional columns/rows in the grid
    fGridCols = Int_t ((fSize[0] + 1e-3) / fGridDx) + nCell;
    fGridRows = Int_t ((fSize[1] + 1e-3) / fGridDy) + nCell;

    // Fill cells with sector numbers
    fGridIndices.resize(fGridRows);
    for (int iRow = 0; iRow < fGridRows; iRow++) {
        fGridIndices[iRow].resize(fGridCols);
        Double_t y = GetGridCellY(iRow);
        for (int iCol = 0; iCol < fGridCols; iCol++) {
            Double_t x = GetGridCellX(iCol);
            bool result = kFALSE;
            for (int iSector = 0; iSector < nSectors; iSector++) {
                CbmMuchSector* sec = (CbmMuchSector*) fSectors.At(iSector);
                if(!sec) continue;
                result = sec->Inside(x, y);
                if (result) {
                    fGridIndices[iRow][iCol] = sec->GetSectorIndex();
                    break;
                }
            }
            if (!result) fGridIndices[iRow][iCol] = -1;
        }
    }
    return kTRUE;
}
// -------------------------------------------------------------------------

// ------ Public method InitNeighbours  ------------------------------------
void CbmMuchModule::InitNeighbourSectors() {
    for (Int_t iSector = 0; iSector < GetNSectors(); iSector++) {
        CbmMuchSector* sector = GetSector(iSector);
        if (!sector) continue;
        vector<Int_t> neighbours;
        Double_t secLx = sector->GetSize()[0];
        Double_t secLy = sector->GetSize()[1];

        Int_t nCol = Int_t ((secLx + 1e-3) / fGridDx) + 2;
        Int_t nRow = Int_t ((secLy + 1e-3) / fGridDy) + 2;
        Double_t x = GetInitX(sector);
        Double_t y = GetInitY(sector);

        Int_t gCol = x < 0 ? -1 : Int_t (x / fGridDx);
        Int_t gRow = y < 0 ? -1 : Int_t (y / fGridDy);

        for (Int_t iRow = gRow; iRow < gRow + nRow; iRow++) {
            if (iRow < 0 || iRow > fGridRows - 1) continue;
            for (Int_t iCol = gCol; iCol < gCol + nCol; iCol++) {
                if (iCol < 0 || iCol > fGridCols - 1) continue;
                CbmMuchSector* sec = GetSector(iCol, iRow);
                if (!sec) continue;
                Int_t iSec = sec->GetSectorIndex();
                if(iSec==iSector) continue;
                vector<Int_t>::iterator it = find(neighbours.begin(),
                        neighbours.end(), iSec);
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
void CbmMuchModule::InitNeighbourPads() {
    // Loop over all sectors within the module
    for (Int_t iSector = 0; iSector < GetNSectors(); iSector++) {
        CbmMuchSector* sector = GetSector(iSector);
        if (!sector) continue;
        Int_t nCols = sector->GetNCols();
        Int_t nRows = sector->GetNRows();
        Double_t secDx = sector->GetDx();
        Double_t secDy = sector->GetDy();
        vector<CbmMuchSector*> neighbours = sector->GetNeighbours();

        // Define minimal pad size among neighbour sectors
        Double_t minDx = secDx;
        Double_t minDy = secDy;
        for (vector<CbmMuchSector*>::iterator it = neighbours.begin(); it
        != neighbours.end(); it++) {
            CbmMuchSector* s = *it;
            Double_t dx = s->GetDx();
            Double_t dy = s->GetDy();
            if (dx < minDx) minDx = dx;
            if (dy < minDy) minDy = dy;
        }
        Int_t iWidth = Int_t ((secDx + minDx / 2.) / minDx);
        Int_t iLength = Int_t ((secDy + minDy / 2.) / minDy);

        // Search for neighbour pads for each given pad
        for (Int_t iChannel = 0; iChannel < sector->GetNChannels(); iChannel++) {
            CbmMuchPad* pad = sector->GetPad(iChannel);
            if (!pad) continue;
            vector<Int_t> neighbours;
            Int_t channelId = pad->GetChannelId();
            Int_t iRow = iChannel / nCols;
            Int_t iCol = iChannel - iRow * nCols;

            // If channel is within the sector
            if (iRow > 0 && iRow < nRows - 1 && iCol > 0 && iCol < nCols - 1) {
                for (Int_t i = iCol - 1; i < iCol + 2; i++) { // columns
                    for (Int_t j = iRow - 1; j < iRow + 2; j++) { // rows
                        Int_t chan = j * nCols + i;
                        CbmMuchPad* p = sector->GetPad(chan);
                        if(!p) continue;
                        Int_t chanId = p->GetChannelId();
                        if (chanId == channelId) continue;
                        vector<Int_t>::iterator it =
                            find(neighbours.begin(), neighbours.end(),
                                    chanId);
                        if (it == neighbours.end()) neighbours.push_back(chanId);

                    }
                }
            }
            // If channel is on the sector's border
            else {
                // Pad center in global system
                Double_t padX0 = pad->GetX0();
                Double_t padY0 = pad->GetY0();
                // Initial coordinate for loop
                Double_t xInit = padX0 - secDx / 2. - minDx / 2.;
                Double_t yInit = padY0 - secDy / 2. - minDy / 2.;
                for (Int_t i = 0; i < iWidth + 2; i++) {
                    Double_t x = xInit + i * minDx;
                    for (Int_t j = 0; j < iLength + 2; j++) {
                        Double_t y = yInit + j * minDy;
                        CbmMuchSector* sec = GetSector(x, y);
                        if (!sec) continue;
                        // Calculate channel number
                        Int_t n_cols = sec->GetNCols();
                        Double_t dx = sec->GetDx();
                        Double_t dy = sec->GetDy();
                        Double_t lx = sec->GetSize()[0];
                        Double_t ly = sec->GetSize()[1];
                        Double_t x0 = sec->GetPosition()[0];
                        Double_t y0 = sec->GetPosition()[1];
                        Double_t x_int = x - x0 + lx / 2.;
                        Double_t y_int = y - y0 + ly / 2.;
                        Int_t i_col = (Int_t) (x_int / dx);
                        Int_t i_row = (Int_t) (y_int / dy);
                        if (i_row < 0) i_row = 0;
                        else if (i_row > sec->GetNRows() - 1)
                            i_row = sec->GetNRows() - 1;
                        if (i_col < 0) i_col = 0;
                        else if (i_col > sec->GetNCols() - 1)
                            i_col = sec->GetNCols() - 1;
                        Int_t chan = i_row * n_cols + i_col;
                        CbmMuchPad* p = sec->GetPad(chan);
                        if (!p) continue;
                        Int_t chanId = p->GetChannelId();
                        if (chanId == channelId) continue;
                        vector<Int_t>::iterator it =
                            find(neighbours.begin(), neighbours.end(),
                                    chanId);
                        if (it == neighbours.end())
                            neighbours.push_back(chanId);

                    }
                }
            }
            // Set TArrayI
            Int_t nSize = neighbours.size();
            if (nSize == 0) continue;
            TArrayI array(nSize, &neighbours[0]);
            pad->SetNeighbours(array);
        }
    }
}
// -------------------------------------------------------------------------

void CbmMuchModule::DrawModule() {
    /*
	 this->SetFillStyle()
	 this->SetFillColor(kOrange);
	 this->Draw("f");
	 this->Draw();
	 for (Int_t i=0;i<GetNSectors();i++){
	 CbmMuchSector* sector = GetSector(i);
	 sector->SetFillColor(kYellow);
	 sector->Draw("f");
	 sector->Draw();
	 }
     */
}

ClassImp(CbmMuchModule)
