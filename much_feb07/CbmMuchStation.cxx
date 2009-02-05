/** CbmMuchStation.cxx
 *@author  M.Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@version 1.0
 *@since   15.03.07
 **
 ** This class holds the transport geometry parameters 
 ** of one MuCh tracking station necessary for digitization.
 **/

#include "CbmMuchStation.h"

#include "CbmMuchSector.h"
#include "CbmMuchSectorDigiPar.h"
#include "CbmDetectorList.h"
#include "CbmMuchPad.h"

#include "TMath.h"

#include <iostream>
#include <cassert>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
CbmMuchStation::CbmMuchStation() {
  fDetectorId = 0;
  fZ = fD = fRadLength = fRmin = fRmax = fRotation = 0.;
  fSectors = new TObjArray(100);
  fCosRot = TMath::Cos(fRotation);
  fSinRot = TMath::Sin(fRotation);
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmMuchStation::CbmMuchStation(const char* name, Int_t iStation, Double_t z, 
			     Double_t d, Double_t rl, Double_t rmin, 
			     Double_t rmax, Double_t rotation) 
  : TNamed(name, "MuCh station") {
  fDetectorId = kMUCH << 24 | iStation << 16;
  fZ          = z;
  fD          = d;
  fRadLength  = rl;
  fRmin       = rmin;
  fRmax       = rmax;
  fRotation   = rotation;
  fSectors = new TObjArray(100);
  fCosRot = TMath::Cos(fRotation);
  fSinRot = TMath::Sin(fRotation);
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmMuchStation::~CbmMuchStation() {
  if ( fSectors ) {
    fSectors->Delete();
    delete fSectors;
  }
}
// -------------------------------------------------------------------------



// -----   Public method GetNChannels   ------------------------------------
Int_t CbmMuchStation::GetNChannels() {
  Int_t nChan = 0;
  for (Int_t iSect=0; iSect<GetNSectors(); iSect++)
    nChan += GetSector(iSect)->GetNChannels();
  return nChan;
}
// -------------------------------------------------------------------------




// -----   Public method GetSectorByNr   -----------------------------------
CbmMuchSector* CbmMuchStation::GetSectorByNr(Int_t sectorNr) {
  if ( fSectorMap.find(sectorNr) != fSectorMap.end() ) {
    Int_t index = fSectorMap[sectorNr];
    CbmMuchSector* sec = (CbmMuchSector*) fSectors->At(index);
    if(!sec) {
      cout << "nSectors = " << GetNSectors()  << "\t"
	   << "iSector = " << index << endl;
      Fatal("No sector!!!", "No sector!!!");
    }
    return sec;
  }
  else {
    cout << "-W- CbmMuchStation::GetSectorByNr: sector " << sectorNr
	 << " not found (station " << GetStationNr() << ")." << endl;
    return NULL;
  }
}
// -------------------------------------------------------------------------



// -----   Public method AddSector   ---------------------------------------
void CbmMuchStation::AddSector(CbmMuchSectorDigiPar* sectorPar) {

  // Get digitisation parameters
  Int_t iSector   = sectorPar->GetSectorNr();
  Int_t iType     = sectorPar->GetType();
  Double_t x0     = sectorPar->GetX0();
  Double_t y0     = sectorPar->GetY0();
  Double_t z0     = sectorPar->GetZ0();
  Double_t lx     = sectorPar->GetLx();
  Double_t ly     = sectorPar->GetLy();
  Double_t d      = sectorPar->GetD();
  Int_t    nCols  = sectorPar->GetNCols();
  Int_t    nRows  = sectorPar->GetNRows();

  // Check for previous existence of sector
  if ( fSectorMap.find(iSector) != fSectorMap.end() ) {
    cout << "-W- " << fName << "::AddSector: Sector number " << iSector
	 << " already exists for station " << GetStationNr() << endl;
    return;
  }

  // Calculate detectorId and rotation in global c.s.
  Int_t    detId  = kMUCH << 24 | (GetStationNr() << 16) | (iSector << 1);

  // Calculate sector coordinates in global c.s.
  Double_t x0Glb = x0*fCosRot - y0*fSinRot;
  Double_t y0Glb = y0*fCosRot + x0*fSinRot;

  // Number of existing sectors
  Int_t nSectors = fSectors->GetEntries();

  // Create and add new sector
  CbmMuchSector* sector = new CbmMuchSector(Form("%ssector%i",GetName(),iSector),detId, iType, x0Glb, y0Glb, 
				  z0, fRotation, lx, ly, d, nCols, nRows);
  sector->AddPads();
  fSectors->Add(sector);

  fSectorMap[iSector] = nSectors;
}
// -------------------------------------------------------------------------


// -----   Public method AddSector   ---------------------------------------
void CbmMuchStation::AddSector(CbmMuchSector* sector) {

  Int_t iSector  = sector->GetSectorNr();
  Int_t nSectors = fSectors->GetEntries();

  fSectors->Add(sector);

  fSectorMap[iSector] = nSectors;

}
// -------------------------------------------------------------------------



// -----   Public method Reset   -------------------------------------------
void CbmMuchStation::Reset() {
  for (Int_t iSector=0; iSector<GetNSectors(); iSector++) 
    GetSector(iSector)->Reset();
}
// -------------------------------------------------------------------------



// -----   Public method Print   -------------------------------------------
void CbmMuchStation::Print() {
  cout << "Station Nr. ";
  cout.width(2);
  cout << GetStationNr() << ", z =  ";
  cout.width(3);
  cout << fZ << " cm, Rmin = ";
  cout.width(5);
  cout << fRmin << " cm, Rmax = " << fRmax << " cm, angle = ";
  cout.width(3);
  cout << fRotation * 180 / TMath::Pi() << " deg., sectors: ";
  cout.width(4);
  cout << GetNSectors() << ", channels: ";
  cout.width(8);
  cout << GetNChannels() << endl;
//   for (Int_t iSec=0; iSec<GetNSectors(); iSec++) 
//     GetSector(iSec)->Print();
}
// -------------------------------------------------------------------------


// -----   Public method InitGrid  -----------------------------------------
void CbmMuchStation::InitGrid() {
  // Width and length of the grid cell
  fGridDx = fGridDy = 1000.;
  for(int iSector = 0; iSector < GetNSectors(); iSector++){
    CbmMuchSector* sector = (CbmMuchSector*) fSectors->At(iSector);
    if(sector->GetLx() < fGridDx) fGridDx = sector->GetLx();
    if(sector->GetLy() < fGridDy) fGridDy = sector->GetLy();
  }
  if(fGridDx > fGridDy) fGridDx = fGridDy;
  else if(fGridDy > fGridDx) fGridDy = fGridDx;    	  

  // Width and length of the grid
  fGridLx = 0.;
  fGridLy = 0.;
  for(int iSector = 0; iSector<GetNSectors(); iSector++){
    CbmMuchSector* sector = (CbmMuchSector*)fSectors->At(iSector);
    Double_t xSec = sector->GetX0();
    Double_t ySec = sector->GetY0();
    // Transform to rotated system
    Double_t xSecRot = xSec*fCosRot + ySec*fSinRot;
    Double_t ySecRot = ySec*fCosRot - xSec*fSinRot;
    if(xSecRot < 0 || ySecRot < 0) continue;

    Double_t secLx = sector->GetLx();
    Double_t secLy = sector->GetLy();
    if(xSecRot > 0) {
      Double_t gridLx = xSecRot + 0.5*secLx;
      if(fGridLx < gridLx) fGridLx = gridLx;
    }
    if(ySecRot > 0) {
      Double_t gridLy = ySecRot + 0.5*secLy;
      if(fGridLy < gridLy) fGridLy = gridLy;
    }    
  }
  fGridLx += 2.*fGridDx; fGridLx *= 2.; // width of the grid
  fGridLy += 2.*fGridDy; fGridLy *= 2.; // length of the grid

  // Fill cells with sector numbers
  Int_t nCols = (Int_t)((fGridLx + fGridDx/2.)/fGridDx); // number of columns in the grid
  Int_t nRows = (Int_t)((fGridLy + fGridDx/2.)/fGridDy); // number of rows in the grid
  fGridCols = nCols;
  fGridRows = nRows;
  fGridIndices.resize(nRows);
  for(int iRow = 0; iRow < nRows; iRow++){
    fGridIndices[iRow].resize(nCols);
    Double_t y = (iRow + 0.5)*fGridDy - fGridLy/2.;
    for(int iCol = 0; iCol < nCols; iCol++){
      Double_t x = (iCol + 0.5)*fGridDx - fGridLx/2.;
      // Transform to global system
      Double_t xRot = x*fCosRot - y*fSinRot;
      Double_t yRot = y*fCosRot + x*fSinRot;
      bool result = kFALSE;
      for(int iSector = 0; iSector < GetNSectors(); iSector++){
         CbmMuchSector* sec = (CbmMuchSector*) fSectors->At(iSector);
         if(sec){
            if(result = sec->Inside(xRot, yRot)){
               fGridIndices[iRow][iCol] = sec->GetSectorNr();
			   //if(GetStationNr()==13 && sec->GetSectorNr() == 186){
				  // printf("iRow = %i, iCol = %i\n", iRow, iCol);
				  // printf("xRot = %f, yRot = %f\n", xRot, yRot);
				  // printf("lx = %f, ly = %f\n", sec->GetLx(), sec->GetLy());
				  // printf("x0 = %f, y0 = %f\n", sec->GetX0(), sec->GetY0());
				  // printf("gridDx = %f, gridDy = %f\n", fGridDx, fGridDy);
			   //}
               break;
			}
         }
      }
      if(!result) fGridIndices[iRow][iCol] = -1;
    }
  }
  cout << "-I- Station " << GetStationNr() << "::InitGrid: grid initialized.\n";
}
// -------------------------------------------------------------------------

// ------ Public method InitNeighbours  ------------------------------------
void CbmMuchStation::InitNeighbourSectors(){
  for(Int_t iSector = 0; iSector<GetNSectors(); iSector++){
    CbmMuchSector* sector = (CbmMuchSector*)fSectors->At(iSector);
    if(sector){
      Double_t secLx = sector->GetLx();
      Double_t secLy = sector->GetLy();
      Double_t secX0 = sector->GetX0();
      Double_t secY0 = sector->GetY0();
      Double_t secNr = sector->GetSectorNr();
      // Transform to rotated system
      Double_t xSecRot = secX0*fCosRot + secY0*fSinRot;
      Double_t ySecRot = secY0*fCosRot - secX0*fSinRot;

      Int_t nCol = (Int_t)((secLx + 1e-5)/fGridDx) + 2;
      Int_t nRow = (Int_t)((secLy + 1e-5)/fGridDy) + 2;
      Double_t x = xSecRot - secLx/2. - fGridDx/2;
      Double_t y = ySecRot - secLy/2. - fGridDy/2;

      Int_t gCol = (Int_t)((x+fGridLx/2.)/fGridDx);
      Int_t gRow = (Int_t)((y+fGridLy/2.)/fGridDy);

      for(Int_t iCol = gCol; iCol<gCol+nCol; iCol++){
         for(Int_t iRow = gRow; iRow<gRow+nRow; iRow++){
            CbmMuchSector* sec = (CbmMuchSector*)GetSector(iRow, iCol);
            if(sec){
               sector->AddNeighbour(sec);
            }
         }
      }
    }
  }
}
// -------------------------------------------------------------------------

// ------ Public method InitNeighbourPads ----------------------------------
void CbmMuchStation::InitNeighbourPads(){
   // Loop over all sectors within the station
   for(Int_t iSector = 0; iSector < GetNSectors(); iSector++){
      CbmMuchSector* sector = (CbmMuchSector*)fSectors->At(iSector);
      if(!sector) continue;
      Int_t sectorNr = sector->GetSectorNr();
      Int_t nCols = sector->GetNCols();
      Int_t nRows = sector->GetNRows();
      Double_t secDx = sector->GetDx();
      Double_t secDy = sector->GetDy();
      Double_t sinRot = sector->GetSinRot();
      Double_t cosRot = sector->GetCosRot();
      map<Int_t, CbmMuchSector*> neighbours = sector->GetNeighbours();

      // Define minimal pad size among neighbour sectors
      Double_t minDx = secDx;
      Double_t minDy = secDy;
      for(map<Int_t, CbmMuchSector*>::iterator it=neighbours.begin(); it!=neighbours.end(); it++){
         CbmMuchSector* s = (*it).second;
         Double_t dx = s->GetDx();
         Double_t dy = s->GetDy();
         if(dx < minDx) minDx = dx;
         if(dy < minDy) minDy = dy;
      }
      Int_t iWidth = (Int_t)((secDx+minDx/2.)/minDx);
      Int_t iLength = (Int_t)((secDy+minDy/2.)/minDy);

	  // Search for neighbour pads for each given pad
      for(Int_t iChannel = 0; iChannel < sector->GetNChannels(); iChannel++){
         CbmMuchPad* pad = sector->GetPad(iChannel);
         if(!pad) continue;         
         Int_t iRow = iChannel/nCols;
         Int_t iCol = iChannel - iRow*nCols;
         // If channel is within the sector
         if(iRow > 0 && iRow < nRows - 1 && iCol > 0 && iCol < nCols - 1){
            for(Int_t i=iCol-1; i<iCol+2; i++){ // columns
               for(Int_t j=iRow-1; j<iRow+2; j++){  // rows
                  Int_t chan = j*nCols + i;
                  assert(chan >=0 && chan <128);
				  CbmMuchPad* p = sector->GetPad(chan);
                  if(p) pad->AddNeighbour(p);
               }
            }
         }
         // If channel is on the sector's border
         else{
            // Pad center in global system
            Double_t padX0 = pad->GetX0(); 
            Double_t padY0 = pad->GetY0();
            // Go to rotated system
            Double_t xcRot = padX0*cosRot + padY0*sinRot;
            Double_t ycRot = padY0*cosRot - padX0*sinRot;
            // Initial coordinate for loop
            Double_t xInit = xcRot - secDx/2. - minDx/2.;
            Double_t yInit = ycRot - secDy/2. - minDy/2.;
   //         if(sectorNr == 178 && GetStationNr()==13 && iChannel == 112){
   //           printf("x0 = %f, y0 = %f\n", sector->GetX0(), sector->GetY0());
   //           printf("lx = %f, ly = %f\n", sector->GetLx(), sector->GetLy());
			//  printf("iChannel = %i\n", iChannel);
			//}
	        for(Int_t i=0; i<iWidth+2; i++){
               Double_t x = xInit + i*minDx;
               Int_t iGridColumn = (Int_t)((x + fGridLx/2.)/fGridDx);
               for(Int_t j=0; j<iLength+2; j++){                  
                  Double_t y = yInit + j*minDy;
                  Int_t iGridRow    = (Int_t)((y + fGridLy/2.)/fGridDy);
                  CbmMuchSector* sec = GetSector(iGridRow, iGridColumn);
                  if(!sec) continue;
				  //if(sectorNr == 178 && GetStationNr()==13 && iChannel == 112){
				  //  printf("x = %f, y = %f\n", x, y);					
				  //}
                  // Calculate channel number
                  Int_t n_cols = sec->GetNCols();
                  Double_t dx = sec->GetDx();
                  Double_t dy = sec->GetDy();
                  Double_t lx = sec->GetLx();
                  Double_t ly = sec->GetLy();
                  Double_t x0 = sec->GetX0();
                  Double_t y0 = sec->GetY0();
                  Double_t x0_rot = x0*cosRot + y0*sinRot;
                  Double_t y0_rot = y0*cosRot - x0*sinRot;
                  Double_t x_int = x - x0_rot + lx/2.;
                  Double_t y_int = y - y0_rot + ly/2.;
                  Int_t i_col = (Int_t)(x_int/dx);
                  Int_t i_row = (Int_t)(y_int/dy);
                  Int_t chan = i_row*n_cols + i_col;
				 // if(!(chan >=0 && chan < 128)){
				 //   printf("*************\ni_row = %i, i_col = %i, n_cols = %i\n", i_row, i_col, n_cols);
					//printf("lx = %f, ly = %f\n", lx, ly);
					//printf("x = %f, y = %f\n", x, y);
					//printf("x0 = %f, y0 = %f\n", x0, y0);
					//printf("x0_rot = %f, y0_rot = %f\n", x0_rot, y0_rot);
					//printf("secNr = %i\n", sec->GetSectorNr());
					//printf("iGridRow = %i, iGridColumn = %i\n", iGridRow, iGridColumn);
				 // }
				  assert(chan >=0 && chan < 128);
				  CbmMuchPad* p = sec->GetPad(chan);
                  if(p) {
                     pad->AddNeighbour(p);
                  }
               }
            }
         }
         // DEBUG info
//         if(GetStationNr() == 2 && sectorNr == 374 && iChannel == 15){
//            cout << pad->GetX0() << "\t" << pad->GetY0() << endl;
//            map<pair<Int_t, Int_t>, CbmMuchPad*> neighPads = pad->GetNeighbours();
//            for(map<pair<Int_t, Int_t>, CbmMuchPad*>::iterator it=neighPads.begin(); it!=neighPads.end(); it++){
//               CbmMuchPad* p = (*it).second;
//               cout << "\t" << p->GetX0() << "\t" << p->GetY0() << endl;
//            }
//         }
      }
   }
}
// -------------------------------------------------------------------------

// ------ Public method GetSector ------------------------------------------
CbmMuchSector* CbmMuchStation::GetSector(Int_t iRow, Int_t iCol){
  Int_t iSectorNr = GetSectorNr(iRow, iCol);
  if(iSectorNr != -1){
    CbmMuchSector* sector = GetSectorByNr(iSectorNr);
    return sector;
  }
  return NULL;
}
// -------------------------------------------------------------------------


ClassImp(CbmMuchStation)
