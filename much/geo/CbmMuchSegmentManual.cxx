/** CbmMuchSegmentManual.cxx
 *@author Mikhail Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@since 02.04.09
 *@version 1.0
 *
 * class for making parameter file for MUCH digitizer
 *
 */

#include "CbmMuchSegmentManual.h"
#include "CbmGeoMuchPar.h"
#include "CbmMuchStation.h"
#include "CbmMuchLayer.h"
#include "CbmMuchLayerSide.h"
#include "CbmMuchModuleGem.h"
#include "CbmMuchSector.h"

#include "FairRuntimeDb.h"

#include "TFile.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TArc.h"
#include "TColor.h"

#include <stdio.h>
#include <string>
#include <cassert>

using std::string;

// -----   Default constructor   -------------------------------------------
CbmMuchSegmentManual::CbmMuchSegmentManual(){
  fDigiFileName = (char*)"much.digi.root";
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmMuchSegmentManual::CbmMuchSegmentManual(char* digiFileName){
  fDigiFileName = digiFileName;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmMuchSegmentManual::~CbmMuchSegmentManual() {
}
// -------------------------------------------------------------------------

// -----   Public method SetNRegions  --------------------------------------
void CbmMuchSegmentManual::SetNRegions(Int_t iStation, Int_t nRegions){
  if(iStation < 0 || iStation >= fNStations)
    Fatal("SetNRegions", "iStation is out of range.");
  fNRegions[iStation] = nRegions;
  fRadii[iStation].resize(nRegions);
}
// -------------------------------------------------------------------------

// -----   Public method SetNRegions  --------------------------------------
void CbmMuchSegmentManual::SetNRegions(Int_t nRegions[]){
  for(Int_t iStation=0; iStation < fNStations; ++iStation){
    SetNRegions(iStation, nRegions[iStation]);
  }
}
// -------------------------------------------------------------------------

// -----   Public method SetNChannels --------------------------------------
void CbmMuchSegmentManual::SetNChannels(Int_t iStation, Int_t nChannels){
  if(iStation < 0 || iStation >= fNStations)
    Fatal("SetNChannels", "iStation is out of range.");
  fNChannels[iStation] = nChannels;
  // Deal with channels more universally
  Int_t n = (Int_t)(TMath::Log2(fNChannels[iStation]) + 1e-2);
  Int_t nChans = (Int_t)(TMath::Power(2, n) + 1e-2);
  if(nChans != fNChannels[iStation]) Fatal("Init", "Number of channels should be equal to two with integer power.");
  Int_t nPower = n/2;
  fNCols[iStation] = (Int_t)TMath::Power(2, nPower);
  fNRows[iStation] = n%2 != 0 ? (Int_t)TMath::Power(2, nPower+1) : fNCols[iStation];
}
// -------------------------------------------------------------------------

// -----   Public method SetNChannels --------------------------------------
void CbmMuchSegmentManual::SetNChannels(Int_t nChannels[]){
  for(Int_t iStation=0; iStation < fNStations; ++iStation){
    SetNChannels(iStation, nChannels[iStation]);
  }
}
// -------------------------------------------------------------------------

// -----   Public method SetRegionRadius  ----------------------------------
void CbmMuchSegmentManual::SetRegionRadius(Int_t iStation, Int_t iRegion, Double_t radius){
  if(iStation < 0 || iStation >= fNStations) Fatal("SetRegionRadius", "iStation is out of range.");
  if(iRegion < 0 || iRegion >= fNRegions[iStation]) Fatal("SetRegionRadius", "iRegion is out of range.");
  fRadii[iStation].at(iRegion) = radius;
}
// -------------------------------------------------------------------------

// -----   Public method SetRegionRadii  -----------------------------------
void CbmMuchSegmentManual::SetRegionRadii(Int_t iStation, Double_t radii[]){
  for(Int_t iRegion=0;iRegion<fNRegions[iStation]; ++iRegion){
    SetRegionRadius(iStation, iRegion, radii[iRegion]);
  }
}
// -------------------------------------------------------------------------

// -----   Public method SetMinSigma   -------------------------------------
void CbmMuchSegmentManual::SetMinSigma(Int_t iStation, Double_t sigmaX, Double_t sigmaY){
  if(iStation < 0 || iStation >= fNStations) Fatal("SetMinSigma", "iStation is out of range.");
  fSecMinLx[iStation] = fNCols[iStation]*TMath::Sqrt(12.)*sigmaX;
  fSecMinLy[iStation] = fNRows[iStation]*TMath::Sqrt(12.)*sigmaY;
  if(TMath::Abs(fSecMinLy[iStation]-fSecMinLx[iStation]) < 1e-5) return;
  if(TMath::Abs(fSecMinLy[iStation]/fSecMinLx[iStation]-2.) > 1e-5){
    fNCols[iStation] *=2;
    fNRows[iStation] /=2;
    SetMinSigma(iStation, sigmaX, sigmaY);
  }
}
// -------------------------------------------------------------------------

// -----   Public method SetMinSigma   -------------------------------------
void CbmMuchSegmentManual::SetMinSigma(Double_t sigmaX[], Double_t sigmaY[]){
  for(Int_t iStation=0;iStation<fNStations; ++iStation){
    SetMinSigma(iStation, sigmaX[iStation], sigmaY[iStation]);
  }
}
// -------------------------------------------------------------------------

// -----   Public method SetMinPadSize -------------------------------------
void CbmMuchSegmentManual::SetMinPadSize(Int_t iStation, Double_t padLx, Double_t padLy){
  if(iStation < 0 || iStation >= fNStations) Fatal("SetMinPadSize", "iStation is out of range.");
  fSecMinLx[iStation] = fNCols[iStation]*padLx;
  fSecMinLy[iStation] = fNRows[iStation]*padLy;
  if(TMath::Abs(fSecMinLy[iStation]-fSecMinLx[iStation]) < 1e-5) return;
  if(TMath::Abs(fSecMinLy[iStation]/fSecMinLx[iStation]-2.) > 1e-5){
    fNCols[iStation] *=2;
    fNRows[iStation] /=2;
    SetMinPadSize(iStation, padLx, padLy);
  }
}
// -------------------------------------------------------------------------

// -----   Public method SetMinPadSize  ------------------------------------
void CbmMuchSegmentManual::SetMinPadSize(Double_t padLx[], Double_t padLy[]){
  for(Int_t iStation=0;iStation<fNStations; ++iStation){
    SetMinPadSize(iStation, padLx[iStation], padLy[iStation]);
  }
}
// -------------------------------------------------------------------------

// -----   Private method SetParContainers  --------------------------------
void CbmMuchSegmentManual::SetParContainers() {
  // Get runtime database
  FairRuntimeDb* db = FairRuntimeDb::instance();
  if ( ! db ) Fatal("Init", "No runtime database");
  fGeoPar = (CbmGeoMuchPar*) db->getContainer("CbmGeoMuchPar");
}
// -------------------------------------------------------------------------

// -----   Private method Init ---------------------------------------------
InitStatus CbmMuchSegmentManual::Init(){
  // Get MUCH geometry parameter container
  fStations = fGeoPar->GetStations();
  if(!fStations) Fatal("Init", "No input array of MUCH stations.");
  if(fStations->GetEntries() != fNStations) Fatal("Init", "Incorrect number of stations.");

  printf("Number of stations: %i\n", fStations->GetEntries());

  // Define sector sizes for each region
  for(Int_t iStation=0; iStation<fStations->GetEntries(); ++iStation){
    CbmMuchStation* station = (CbmMuchStation*)fStations->At(iStation);

    // Default initialization if required
    if(fNChannels.find(iStation)==fNChannels.end()) SetNChannels(iStation, 128);
    if(fNRegions.find(iStation)==fNRegions.end()) {
      if(iStation == 0) SetNRegions(iStation, 5);
      else if(iStation == 1) SetNRegions(iStation, 3);
      else SetNRegions(iStation, 1);
    }
    if(fRadii[iStation].at(0)==0) {
      if(iStation == 0){
        Double_t radii[] = {13.99, 19.39, 24.41, 31.51, 64.76};
        SetRegionRadii(iStation, radii);
      }
      else if(iStation==1){
        Double_t radii[] = {22, 24, 90.95};
        SetRegionRadii(iStation, radii);
      }
      else {
        Double_t radii[] = {station->GetRmax()};
        SetRegionRadii(iStation, radii);
      }
    }
    if(fSecMinLx.find(iStation)==fSecMinLx.end()) {
      if(iStation == 0) SetMinPadSize(iStation, 0.1386, 0.1386);
      else if(iStation == 1) SetMinPadSize(iStation, 0.4, 0.4);
      else SetMinPadSize(iStation, 0.8, 0.8);
    }

    fSecLx[iStation].push_back(fSecMinLx[iStation]);
    fSecLy[iStation].push_back(fSecMinLy[iStation]);
    for(Int_t iRegion=1; iRegion<fNRegions[iStation]; ++iRegion){
      Bool_t result = TMath::Abs(fSecLx[iStation].at(iRegion-1) - fSecLy[iStation].at(iRegion-1)) < 1e-5;
      Double_t secLx = result ? fSecLx[iStation].at(iRegion-1)   : 2*fSecLx[iStation].at(iRegion-1);
      Double_t secLy = result ? 2*fSecLy[iStation].at(iRegion-1) : fSecLy[iStation].at(iRegion-1);
      fSecLx[iStation].push_back(secLx);
      fSecLy[iStation].push_back(secLy);
    }
  }

  // Segment MuCh
  SegmentMuch();
  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Public method SegmentMuch  --------------------------------------
void CbmMuchSegmentManual::SegmentMuch(){
  for(Int_t iStation=0; iStation<fStations->GetEntries(); ++iStation){
    CbmMuchStation* station = (CbmMuchStation*)fStations->At(iStation);

    Int_t nLayers = station->GetNLayers();
    for(Int_t iLayer = 0; iLayer < nLayers; ++iLayer){
      CbmMuchLayer* layer = station->GetLayer(iLayer);
      if(!layer) Fatal("SegmentMuch", "Incomplete layers array.");
      // Segment layer sides
      SegmentLayerSide(layer->GetSideF());
      SegmentLayerSide(layer->GetSideB());
    }
    printf("Station %i segmented\n",iStation+1);
  }

  // Save parameters
  TFile* f = new TFile(fDigiFileName, "RECREATE");
  fStations->Write("stations",1);

  f->Close();

  Print();

  DrawSegmentation();
}
// -------------------------------------------------------------------------

// -----   Private method SegmentLayerSide  --------------------------------
void CbmMuchSegmentManual::SegmentLayerSide(CbmMuchLayerSide* layerSide){
  if(!layerSide) Fatal("SegmentLayerSide", "Incomplete layer sides array.");
  Int_t nModules = layerSide->GetNModules();
  for(Int_t iModule = 0; iModule < nModules; iModule++){
    CbmMuchModule* module = layerSide->GetModule(iModule);
    if(module->GetDetectorType()!=1) continue;
    CbmMuchModuleGem* mod = (CbmMuchModuleGem*)module;
    if(nModules > 1) SegmentModule(mod, true);
    else SegmentModule(mod, false);
  }
}
// -------------------------------------------------------------------------

// -----   Private method SegmentSector  -----------------------------------
void CbmMuchSegmentManual::SegmentModule(CbmMuchModuleGem* module, Bool_t useModuleDesign){
  Int_t detectorId = module->GetDetectorId();
  Int_t iStation = CbmMuchGeoScheme::GetStationIndex(detectorId);
  CbmMuchStation* station = (CbmMuchStation*)fStations->At(iStation);
  module->SetNSectorChannels(fNChannels[iStation]);
  Double_t secMaxLx = GetSectorMaxSize(module, "Width");
  Double_t secMaxLy = GetSectorMaxSize(module, "Length");
  Double_t padMaxLx = GetPadMaxSize(module, "Width");
  Double_t padMaxLy = GetPadMaxSize(module, "Length");
  TVector3 size = module->GetSize();
  Double_t modLx = size.X();
  Double_t modLy = size.Y();
  Double_t modLz = size.Z();
  TVector3 position = module->GetPosition();
  Double_t modX = position.X();
  Double_t modY = position.Y();
  Double_t modZ = position.Z();

  Int_t nCols = Int_t(modLx/secMaxLx);
  Int_t nRows = Int_t(modLy/secMaxLy);
  Int_t nX = modX < 0 ? -1 : 1;
  Int_t nY = modY < 0 ? -1 : 1;

  Double_t secX, secY, secLx, secLy;
  TVector3 secSize, secPosition;
  Int_t iSector;
  for(Int_t iCol=0; iCol<nCols; ++iCol){
      secX = useModuleDesign ? modX + nX*((iCol + 0.5)*secMaxLx - modLx/2.)
          : secMaxLx*nCols/2. - (0.5 + iCol)*secMaxLx;
    for(Int_t iRow=0; iRow<nRows; ++iRow){
      secY = useModuleDesign ? modY + nY*((iRow + 0.5)*secMaxLy - modLy/2.)
          : secMaxLy*nRows/2. - (0.5 + iRow)*secMaxLy;
      iSector = module->GetNSectors();
      secSize.SetXYZ(secMaxLx, secMaxLy, modLz);
      secPosition.SetXYZ(secX, secY, modZ);
      SegmentSector(module, new CbmMuchSector(detectorId, iSector, secPosition, secSize, fNCols[iStation], fNRows[iStation]));
    }
  }

  // Process incomplete sectors
  Int_t nPadRows, nPadCols;
  Int_t n = useModuleDesign ? 1 : 2;
  Double_t ly = useModuleDesign ? modLy - secMaxLy*nRows : (modLy - secMaxLy*nRows)/2.;
  if(ly > padMaxLy){
    nPadRows = Int_t(ly/padMaxLy);
    for(Int_t i=0;i<n; ++i){
      secY = useModuleDesign ? modY + nY*(modLy/2. - ly/2.)
          : TMath::Power(-1, i)*(modLy/2. - ly/2.);
      for(Int_t iCol=0; iCol<nCols; ++iCol){
        secX = useModuleDesign ? modX + nX*((iCol + 0.5)*secMaxLx - modLx/2.)
            : secMaxLx*nCols/2. - (0.5 + iCol)*secMaxLx;
        iSector = module->GetNSectors();
        secSize.SetXYZ(secMaxLx, ly, modLz);
        secPosition.SetXYZ(secX, secY, modZ);
        SegmentSector(module, new CbmMuchSector(detectorId, iSector, secPosition, secSize, fNCols[iStation], nPadRows));
      }
    }
  }

  Double_t lx = useModuleDesign ? modLx - secMaxLx*nCols : (modLx - secMaxLx*nCols)/2.;
  if(lx > padMaxLx){
    nPadCols = (Int_t)(lx/padMaxLx);
    for(Int_t i=0; i<n; ++i){
      secX = useModuleDesign ? modX + nX*(modLx/2. - lx/2.)
          : TMath::Power(-1, i)*(modLx/2. - lx/2.);
      for(Int_t iRow=0; iRow<nRows; ++iRow){
        secY = useModuleDesign ? modY + nY*((iRow + 0.5)*secMaxLy - modLy/2.)
            : secMaxLy*nRows/2. - (0.5 + iRow)*secMaxLy;
        iSector = module->GetNSectors();
        secSize.SetXYZ(lx, secMaxLy, modLz);
        secPosition.SetXYZ(secX, secY, modZ);
        SegmentSector(module, new CbmMuchSector(detectorId, iSector, secPosition, secSize, nPadCols, fNRows[iStation]));
      }
    }
  }

  if(lx > padMaxLx && ly > padMaxLy){
    nPadCols = (Int_t)(lx/padMaxLx);
    nPadRows = (Int_t)(ly/padMaxLy);
    secX = modX + nX*(modLx/2. - lx/2.);
    secY = modY + nY*(modLy/2. - ly/2.);
    iSector = module->GetNSectors();
    secSize.SetXYZ(lx, ly, modLz);
    secPosition.SetXYZ(secX, secY, modZ);
    SegmentSector(module, new CbmMuchSector(detectorId, iSector, secPosition, secSize, nPadCols, nPadRows));
  }
}
// -------------------------------------------------------------------------

// -----   Private method SegmentSector  -----------------------------------
void CbmMuchSegmentManual::SegmentSector(CbmMuchModuleGem* module, CbmMuchSector* sector){
  TVector3 secSize = sector->GetSize();
  TVector3 secPosition = sector->GetPosition();
  Int_t detectorId = module->GetDetectorId();
  Int_t iStation = CbmMuchGeoScheme::GetStationIndex(detectorId);
  Double_t secLx = secSize.X();
  Double_t secLy = secSize.Y();
  Double_t secLz = secSize.Z();
  Double_t padLx = sector->GetDx();
  Double_t padLy = sector->GetDy();
  Double_t secX  = secPosition.X();
  Double_t secY  = secPosition.Y();
  Double_t secZ  = secPosition.Z();
  Int_t nX = secX < 0 ? -1 : 1;
  Int_t nY = secY < 0 ? -1 : 1;

  Bool_t resultX = ShouldSegment(sector, "X");
  Bool_t resultY = ShouldSegment(sector, "Y");

  if(!resultX && !resultY){
    CbmMuchStation* station = (CbmMuchStation*)fStations->At(iStation);
    Double_t rMax = station->GetRmax();
    if(IntersectsRad(sector, module->GetCutRadius())==2 || !IntersectsRad(sector, rMax)){
      delete sector;
      return;
    }
    module->AddSector(sector);
    return;
  }

  // Needed for the case of incomplete sectors
  Int_t nC = Int_t(padLx/GetPadMaxSize(module, "Width"));
  Double_t pLx = nC == 0 ? padLx : nC*GetPadMaxSize(module, "Width");
  Int_t nR = Int_t(padLy/GetPadMaxSize(module, "Length"));
  Double_t pLy = nR == 0 ? padLy : nR*GetPadMaxSize(module, "Length");

  assert(resultX || resultY);
  delete sector;
  Int_t iSector;
  TVector3 position, size;
  Double_t newSecLx, newSecLy, newSecX, newSecY;
  for(Int_t i=0; i<2; ++i){
    newSecLx = resultX ? i*(secLx - newSecLx) - (i-1)*pLx/2.*fNCols[iStation] : secLx;
    newSecLy = resultY ? i*(secLy - newSecLy) - (i-1)*pLy/2.*fNRows[iStation] : secLy;
    newSecX  = resultX ? secX - TMath::Power(-1, i)*nX*(secLx/2. - newSecLx/2.) : secX;
    newSecY  = resultY ? secY - TMath::Power(-1, i)*nY*(secLy/2. - newSecLy/2.) : secY;
    Double_t newPadLx = resultX ? pLx/2. : padLx;
    Double_t newPadLy = resultY ? pLy/2. : padLy;
    Int_t nCols = Int_t(newSecLx/newPadLx);
    Int_t nRows = Int_t(newSecLy/newPadLy);
    position.SetXYZ(newSecX, newSecY, secZ);
    size.SetXYZ(newSecLx, newSecLy, secLz);
    iSector = module->GetNSectors();
    SegmentSector(module, new CbmMuchSector(detectorId, iSector, position, size, nCols, nRows));
  }
}
// -------------------------------------------------------------------------

// -----   Private method IntersectsRad  -----------------------------------
Int_t CbmMuchSegmentManual::IntersectsRad(CbmMuchModuleGem* module, Double_t radius){
  if(radius < 0) return 0;

  Int_t intersects = 0;

  Double_t modLx = module->GetSize()[0];
  Double_t modLy = module->GetSize()[1];
  Double_t modX  = module->GetPosition()[0];
  Double_t modY  = module->GetPosition()[1];

  Double_t ulR = TMath::Sqrt((modX - modLx/2.)*(modX - modLx/2.) + (modY + modLy/2.)*(modY + modLy/2.));
  Double_t urR = TMath::Sqrt((modX + modLx/2.)*(modX + modLx/2.) + (modY + modLy/2.)*(modY + modLy/2.));
  Double_t blR = TMath::Sqrt((modX - modLx/2.)*(modX - modLx/2.) + (modY - modLy/2.)*(modY - modLy/2.));
  Double_t brR = TMath::Sqrt((modX + modLx/2.)*(modX + modLx/2.) + (modY - modLy/2.)*(modY - modLy/2.));

  if(ulR < radius) intersects++;
  if(urR < radius) intersects++;
  if(blR < radius) intersects++;
  if(brR < radius) intersects++;

  if(intersects == 4) return 2;
  if(intersects) return 1;
  else return 0;
}
// -------------------------------------------------------------------------

// -----   Private method IntersectsRad  -----------------------------------
Int_t CbmMuchSegmentManual::IntersectsRad(CbmMuchSector* sector, Double_t radius){
  if(radius < 0) return 0;

  Int_t intersects = 0;

  Double_t secLx = sector->GetSize()[0];
  Double_t secLy = sector->GetSize()[1];
  Double_t secX  = sector->GetPosition()[0];
  Double_t secY  = sector->GetPosition()[1];

  Double_t ulR = TMath::Sqrt((secX - secLx/2.)*(secX - secLx/2.) + (secY + secLy/2.)*(secY + secLy/2.));
  Double_t urR = TMath::Sqrt((secX + secLx/2.)*(secX + secLx/2.) + (secY + secLy/2.)*(secY + secLy/2.));
  Double_t blR = TMath::Sqrt((secX - secLx/2.)*(secX - secLx/2.) + (secY - secLy/2.)*(secY - secLy/2.));
  Double_t brR = TMath::Sqrt((secX + secLx/2.)*(secX + secLx/2.) + (secY - secLy/2.)*(secY - secLy/2.));

  if(ulR < radius) intersects++;
  if(urR < radius) intersects++;
  if(blR < radius) intersects++;
  if(brR < radius) intersects++;

  if(intersects == 4) return 2;
  if(intersects) return 1;
  else return 0;
}
// -------------------------------------------------------------------------

// -----   Private method ShouldSegment  -----------------------------------
Bool_t CbmMuchSegmentManual::ShouldSegment(CbmMuchSector* sector, const TString direction){
  Double_t secLx = sector->GetSize()[0];
  Double_t secLy = sector->GetSize()[1];
  Double_t secX  = sector->GetPosition()[0];
  Double_t secY  = sector->GetPosition()[1];

  if(TMath::Abs(secLx - secLy) < 1e-5 && direction=="Y") return false;
  //if(!ShouldSegment(sector, "Y") && secLy > secLx && TMath::Abs(secLx - secLy) > 1e-5 && direction=="X")
  if(secLy > secLx && direction=="X" && ShouldSegment(sector, "Y") && TMath::Abs(secLx - secLy) > 1e-5) return false;

  Double_t ulR = TMath::Sqrt((secX - secLx/2.)*(secX - secLx/2.) + (secY + secLy/2.)*(secY + secLy/2.));
  Double_t urR = TMath::Sqrt((secX + secLx/2.)*(secX + secLx/2.) + (secY + secLy/2.)*(secY + secLy/2.));
  Double_t blR = TMath::Sqrt((secX - secLx/2.)*(secX - secLx/2.) + (secY - secLy/2.)*(secY - secLy/2.));
  Double_t brR = TMath::Sqrt((secX + secLx/2.)*(secX + secLx/2.) + (secY - secLy/2.)*(secY - secLy/2.));

  Double_t uR = TMath::Min(ulR, urR);
  Double_t bR = TMath::Min(blR, brR);
  Double_t R  = TMath::Min(uR, bR);

  Int_t iStation = CbmMuchGeoScheme::GetStationIndex(sector->GetDetectorId());
  Double_t secMinL = direction=="X" ? fSecMinLx[iStation] : fSecMinLy[iStation];
  Double_t secL    = direction=="X" ? secLx : secLy;
  if(secL > secMinL && secL/2. < secMinL) return false;

  // Check sector size in the corresponding region
  for(Int_t iRegion=0; iRegion<fNRegions[iStation]; ++iRegion){
    Double_t regionRad = fRadii[iStation].at(iRegion);
    if(R > regionRad) continue;
    if(direction=="X" && fSecLx[iStation].at(iRegion) < secLx && TMath::Abs(fSecLx[iStation].at(iRegion) - secLx) > 1e-5) return true;
    if(direction=="Y" && fSecLy[iStation].at(iRegion) < secLy && TMath::Abs(fSecLy[iStation].at(iRegion) - secLy) > 1e-5) return true;
    break;
  }
  return false;
}
// -------------------------------------------------------------------------

// -----   Private method GetSectorMaxSize  --------------------------------
Double_t CbmMuchSegmentManual::GetSectorMaxSize(CbmMuchModuleGem* module, const TString side){
  Int_t iStation = CbmMuchGeoScheme::GetStationIndex(module->GetDetectorId());
  Int_t nRegions = fNRegions[iStation];
  for(Int_t iRegion=0; iRegion<nRegions; ++iRegion){
    Double_t rad = fRadii[iStation].at(iRegion);
    Int_t result = IntersectsRad(module, rad);
    if(result == 2) return side == "Width" ? fSecLx[iStation].at(iRegion) :fSecLy[iStation].at(iRegion);
  }
  return side == "Width" ? fSecLx[iStation].at(nRegions-1) :fSecLy[iStation].at(nRegions-1);
}
// -------------------------------------------------------------------------

// -----   Private method GetPadMaxSize  -----------------------------------
Double_t CbmMuchSegmentManual::GetPadMaxSize(CbmMuchModuleGem* module, const TString side){
  Int_t iStation = CbmMuchGeoScheme::GetStationIndex(module->GetDetectorId());
  Double_t sectorSize = GetSectorMaxSize(module, side);
  return side=="Width" ? sectorSize/fNCols[iStation] : sectorSize/fNRows[iStation];
}
// -------------------------------------------------------------------------

// -----   Private method Print  -------------------------------------------
void CbmMuchSegmentManual::Print(){
  printf("Segmentation written to file %s\n", fDigiFileName);
  Int_t nTotSectors = 0;
  Int_t nTotChannels = 0;
  Int_t nTotGems = 0;
  Int_t nTotStraws = 0;
  printf("=================================================================================================\n");
  for(Int_t iStation=0; iStation < fStations->GetEntries(); ++iStation){
    CbmMuchStation* station = (CbmMuchStation*) fStations->At(iStation);
    Int_t nGems = 0;
    Int_t nStraws = 0;
    Int_t nSectors = 0;
    Int_t nChannels = 0;
    Double_t padMaxLx = std::numeric_limits<Double_t>::min();
    Double_t padMinLx = std::numeric_limits<Double_t>::max();
    Double_t padMaxLy = std::numeric_limits<Double_t>::min();
    Double_t padMinLy = std::numeric_limits<Double_t>::max();
    Double_t secMaxLx = std::numeric_limits<Double_t>::min();
    Double_t secMinLx = std::numeric_limits<Double_t>::max();
    Double_t secMaxLy = std::numeric_limits<Double_t>::min();
    Double_t secMinLy = std::numeric_limits<Double_t>::max();
    if(!station) continue;
    for(Int_t iLayer = 0; iLayer < station->GetNLayers(); ++iLayer){
      CbmMuchLayer* layer = station->GetLayer(iLayer);
      if(!layer) continue;
      for(Int_t iSide = 0; iSide < 2; ++iSide){
        CbmMuchLayerSide* side = layer->GetSide(iSide);
        if(!side) continue;
        for(Int_t iModule=0; iModule < side->GetNModules(); ++iModule){
          CbmMuchModule* mod = side->GetModule(iModule);
          if(!mod) continue;
          switch(mod->GetDetectorType()){
            case 1:{ // GEMs
              CbmMuchModuleGem* module = (CbmMuchModuleGem*)mod;
              if(!module) continue;
              nGems++;
              nSectors += module->GetNSectors();
              for(Int_t iSector=0; iSector<module->GetNSectors(); ++iSector){
                CbmMuchSector* sector = module->GetSector(iSector);
                if(!sector) continue;
                Double_t padLx = sector->GetDx();
                Double_t padLy = sector->GetDy();
                if(padLx > padMaxLx) { padMaxLx = padLx; secMaxLx = sector->GetNCols()*padLx; }
                if(padLx < padMinLx) { padMinLx = padLx; secMinLx = sector->GetNCols()*padLx; }
                if(padLy > padMaxLy) { padMaxLy = padLy; secMaxLy = sector->GetNRows()*padLy; }
                if(padLy < padMinLy) { padMinLy = padLy; secMinLy = sector->GetNRows()*padLy; }
                nChannels += sector->GetNChannels();
              }
              break;
            }
            case 2: // Straw tubes
              nStraws++;
              break;
          }
        }
      }
    }
    printf("Station %i:\n", iStation+1);
    printf("   GEM modules: %i\n", nGems);
    if(nGems) {
      printf("      Sectors: %i, Min.Sector size:%3.2fx%3.2f, Max.Sector size:%3.2fx%3.2f\n",nSectors, secMinLx, secMinLy, secMaxLx, secMaxLy);
      printf("      Pads: %i, Min.Pad size:%3.2fx%3.2f, Max.Pad size:%3.2fx%3.2f\n", nChannels, padMinLx, padMinLy, padMaxLx, padMaxLy);
    }
    printf("   Straw modules: %i\n", nStraws);
    nTotSectors += nSectors;
    nTotChannels += nChannels;
    nTotGems += nGems;
    nTotStraws += nStraws;
  }
  printf("-------------------------------------------------------------------------------------------------\n");
  printf(" Summary: \n   GEM modules: %i\n      Sectors: %i, Pads: %i\n   Straw modules: %i\n",
      nTotGems, nTotSectors, nTotChannels, nTotStraws);
  printf("=================================================================================================\n");
}


void CbmMuchSegmentManual::DrawSegmentation(){
  string digifile(fDigiFileName);
  Int_t startIndex = digifile.size() - 4;
  string txtfile  = digifile.erase(startIndex, 4);
  txtfile.append("txt");

  FILE* outfile;
  outfile = fopen(txtfile.c_str(), "w");
  Int_t colors[] = {kGreen, kMagenta, kCyan, kRed, kBlue, kYellow, kTeal, kPink, kAzure, kOrange, kViolet, kSpring,
      kGreen+10, kMagenta+10, kCyan+10, kRed+10, kBlue+10, kYellow+10, kTeal+10, kPink+10, kAzure+10, kOrange+10, kViolet+10, kSpring+10};
  for (Int_t iStation=0;iStation<fStations->GetEntriesFast();++iStation){
    fprintf(outfile, "===========================================================================\n");
    fprintf(outfile, "Station %i\n", iStation+1);
    fprintf(outfile, "Sector size, cm   Sector position, cm   Number of pads   Side   Pad size, cm\n");
    fprintf(outfile, "----------------------------------------------------------------------------\n");
    TCanvas* c1 = new TCanvas(Form("station%i",iStation+1),Form("station%i",iStation+1),800,800);
    c1->SetFillColor(0);
    c1->Range(-250,-250,250,250);
    CbmMuchStation* station = (CbmMuchStation*) fStations->At(iStation);
    CbmMuchLayer* layer = station->GetLayer(0);
    for (Int_t iSide=1;iSide>=0;iSide--){
      CbmMuchLayerSide* side = layer->GetSide(iSide);
      for (Int_t iModule=0;iModule<side->GetNModules();++iModule) {
        CbmMuchModule* mod = side->GetModule(iModule);
        mod->SetFillStyle(0);
        mod->Draw();
        CbmMuchModuleGem* module = (CbmMuchModuleGem*)mod;
        for (Int_t iSector=0;iSector<module->GetNSectors();++iSector){
          CbmMuchSector* sector = module->GetSector(iSector);
          // Reject incomplete sectors by size
          Double_t secLx = sector->GetSize()[0];
          Double_t secLy = sector->GetSize()[1];
          Double_t less = TMath::Min(secLx, secLy);
          Double_t more = TMath::Max(secLx, secLy);
          Int_t nReal = Int_t((more+1e-5)/less);
          Int_t nPow = Int_t(TMath::Log2(nReal) + 1e-2);
          Double_t more1 = less*TMath::Power(2,nPow);
          if(TMath::Abs(more1-more ) > 1e-5 || sector->GetNChannels() < module->GetNSectorChannels())
            iSide ? sector->SetFillColor(TColor::GetColorDark(kGray+1)) : sector->SetFillColor(kGray + 1) ;
          else{
            Int_t i = Int_t((secLx+1e-5)/fSecMinLx[0]) - 1;
            Int_t j = Int_t((secLy+1e-5)/fSecMinLy[0]) - 1;
            sector->SetFillColor(iSide ? TColor::GetColorDark(colors[i+j]) : colors[i+j]);
          }
          sector->Draw("f");
          sector->Draw();
          const char* side = iSide ? "Back" : "Front";
          fprintf(outfile, "%-4.2fx%-10.2f   %-6.2fx%-12.2f   %-14i   %-5s   ", secLx, secLy,
              sector->GetPosition()[0], sector->GetPosition()[1], sector->GetNChannels(), side);
          fprintf(outfile, "%-4.2fx%-4.2f\n", secLx, secLy, sector->GetNChannels(), sector->GetDx(), sector->GetDy());
        } // sectors
      } // modules
    } // sides

    for(Int_t iRegion=0; iRegion < fNRegions[iStation]; ++iRegion){
      TArc* arc = new TArc(0.,0.,fRadii[iStation].at(iRegion));
      arc->SetLineColor(kBlue);
      arc->SetLineWidth(2);
      arc->SetFillStyle(0);
      arc->Draw();
    }
    c1->Print(Form("station%i.eps",iStation+1));
    c1->Print(Form("station%i.png",iStation+1));
  }//stations
  fclose(outfile);
}
ClassImp(CbmMuchSegmentManual)

