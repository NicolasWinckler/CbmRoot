// -------------------------------------------------------------------------
// -----                    CbmMuchGeoScheme source file               -----
// -----                  Created 18/02/08  by E. Kryshen              -----
// -------------------------------------------------------------------------
#include "CbmMuchGeoScheme.h"
#include "CbmMuchStation.h"
#include "CbmMuchLayer.h"
#include "CbmMuchLayerSide.h"
#include "CbmMuchModuleGem.h"
#include "CbmMuchModuleStraws.h"
#include "CbmMuchSector.h"
#include "CbmMuchPad.h"
#include "TObjArray.h"
#include "TFile.h"
#include "TMath.h"
#include "TGeoCone.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <stdexcept>

using std::cout;
using std::endl;
using std::vector;

CbmMuchGeoScheme* CbmMuchGeoScheme::fInstance = NULL;
Bool_t CbmMuchGeoScheme::fInitialized = kFALSE;
Bool_t CbmMuchGeoScheme::fModulesInitialized = kFALSE;

// -------------------------------------------------------------------------
CbmMuchGeoScheme::CbmMuchGeoScheme() {
  fMuchZ1 = 0.; // MuchCave Zin position [cm]
  fAcceptanceTanMin = 0.; // Acceptance tangent min
  fAcceptanceTanMax = 0.; // Acceptance tangent max
  fNabs = 0; // Number of absorbers
  fNst = 0; // Number of stations

  fActiveLx = 0.; // Active volume lx [cm]
  fActiveLy = 0.; // Active volume ly [cm]
  fActiveLz = 0.; // Active volume lz [cm]
  fSpacerLx = 0.; // Spacer lx [cm]
  fSpacerLy = 0.; // Spacer ly [cm]
  fOverlapY = 0.; // Overlap along y axis [cm]

  // Create arrays of absorber parameters
  fAbsorberZ1.Set(0); // Absorber Zin position [cm]
  fAbsorberLz.Set(0); // Absorber thickness [cm]
  fAbsorberMat.Set(0); // Absorber material
  // Create arrays of station parameters
  fStationZ0.Set(0); // Station Zceneter [cm]
  fNlayers.Set(0); // Number of layers
  fDetType.Set(0); // Detector type
  fLayersDz.Set(0); // Distance between layers [cm]
  fSupportLz.Set(0); // Support thickness [cm]
  fModuleDesign.Set(0); // Support thickness [cm]
  fAbsorbers = new TObjArray();
  Info("CbmMuchGeoScheme", "CbmMuchGeoScheme created");
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
CbmMuchGeoScheme::~CbmMuchGeoScheme() {
  if (fInstance != NULL)
    delete fInstance;
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
CbmMuchGeoScheme* CbmMuchGeoScheme::Instance() {
  if (!fInstance)
    fInstance = new CbmMuchGeoScheme();
  return fInstance;
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void CbmMuchGeoScheme::Init(TObjArray* stations) {
  if (!fInitialized) {
    fStations = stations;
    fInitialized = kTRUE;
  }

  InitModules();

  Int_t iSide = 0;
  for (Int_t iStation = 0; iStation < GetNStations(); iStation++) {
    vector<CbmMuchLayerSide*> sides = GetLayerSides(iStation);
  }
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void CbmMuchGeoScheme::InitModules() {
  if (!fModulesInitialized) {
    if (!fStations) Fatal("InitModules", "No input array of stations.");
    Int_t incSides = 0;
    fMapSides.clear();
    fSectors.clear();
    fSides.clear();
    fModules.clear();
    for (Int_t iStation = 0; iStation < GetNStations(); iStation++) {
      CbmMuchStation* station = GetStation(iStation);
      if (!station)  continue;
      assert(iStation == GetStationIndex(station->GetDetectorId()));
      vector<CbmMuchSector*> sectors;
      vector<CbmMuchLayerSide*> sides;
      vector<CbmMuchModule*> modules;
      for (Int_t iLayer = 0; iLayer < station->GetNLayers(); iLayer++) {
        CbmMuchLayer* layer = station->GetLayer(iLayer);
        if (!layer)  continue;
        assert(iLayer == GetLayerIndex(layer->GetDetectorId()));
        for (Int_t iSide = 0; iSide < 2; iSide++) {
          CbmMuchLayerSide* side = (CbmMuchLayerSide*) layer->GetSide(iSide);
          if (!side) continue;
          assert(iSide == GetLayerSideIndex(side->GetDetectorId()));
          if(side->GetNModules()!=0) fMapSides[side->GetDetectorId()] = incSides++;
          sides.push_back(side);
          for (Int_t iModule = 0; iModule < side->GetNModules(); iModule++) {
            CbmMuchModule* mod = side->GetModule(iModule);
            if (!mod) continue;
            assert(iModule == GetModuleIndex(mod->GetDetectorId()));
            if(!mod->InitModule()) continue;
            modules.push_back(mod);
            if(mod->GetDetectorType() != 1) continue;
            CbmMuchModuleGem* module = (CbmMuchModuleGem*) mod;
            for (Int_t iSector = 0; iSector < module->GetNSectors(); iSector++) {
              CbmMuchSector* sector = module->GetSector(iSector);
              if (!sector) continue;
              sectors.push_back(sector);
              for (Int_t iPad = 0; iPad < sector->GetNChannels(); iPad++) {
                CbmMuchPad* pad = sector->GetPad(iPad);
                if (pad) fPads.push_back(pad);
              } // Pads
            } // Sectors
          } // Modules
        } // Sides
      } // Layers
      fSectors.push_back(sectors);
      fSides.push_back(sides);
      fModules.push_back(modules);
    } // Stations

    fModulesInitialized = kTRUE;
  }
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
CbmMuchStation* CbmMuchGeoScheme::GetStation(Int_t iStation) {
  if (!fStations)
    return NULL;
  Bool_t result = (iStation >= 0) || (iStation < fStations->GetEntries());
  return result ? (CbmMuchStation*) fStations->At(iStation) : NULL;
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
CbmMuchLayer* CbmMuchGeoScheme::GetLayer(Int_t iStation, Int_t iLayer) {
  CbmMuchStation* station = GetStation(iStation);
  return station ? station->GetLayer(iLayer) : NULL;
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
CbmMuchLayerSide* CbmMuchGeoScheme::GetLayerSide(Int_t iStation, Int_t iLayer,
    Bool_t iSide) {
  CbmMuchLayer* layer = GetLayer(iStation, iLayer);
  return layer ? layer->GetSide(iSide) : NULL;;
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
CbmMuchModule* CbmMuchGeoScheme::GetModule(Int_t iStation, Int_t iLayer,
    Bool_t iSide, Int_t iModule) {
  CbmMuchLayerSide* side = GetLayerSide(iStation, iLayer, iSide);
  return side ? side->GetModule(iModule) : NULL;
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
CbmMuchStation* CbmMuchGeoScheme::GetStationByDetId(Int_t detId) {
  Int_t iStation = GetStationIndex(detId);
  return GetStation(iStation);
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
CbmMuchLayer* CbmMuchGeoScheme::GetLayerByDetId(Int_t detId) {
  CbmMuchStation* station = GetStationByDetId(detId);
  Int_t iLayer = GetLayerIndex(detId);
  return station ? station->GetLayer(iLayer) : NULL;
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
CbmMuchLayerSide* CbmMuchGeoScheme::GetLayerSideByDetId(Int_t detId) {
  CbmMuchLayer* layer = GetLayerByDetId(detId);
  Int_t iSide = GetLayerSideIndex(detId);
  return layer ? layer->GetSide(iSide) : NULL;
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
CbmMuchModule* CbmMuchGeoScheme::GetModuleByDetId(Int_t detId) {
  CbmMuchLayerSide* side = GetLayerSideByDetId(detId);
  Int_t iModule = GetModuleIndex(detId);
  return side ? side->GetModule(iModule) : NULL;
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void CbmMuchGeoScheme::CreatePointArrays(char* pointClassName) {
  for(vector<CbmMuchModule*>::iterator it=GetModules().begin(); it!=GetModules().end(); it++){
    CbmMuchModule* module = (*it);
    if(module->GetDetectorType()!=1) continue;
    CbmMuchModuleGem* mod = (CbmMuchModuleGem*)module;
    mod->SetPoints(new TClonesArray(pointClassName, 10));
  }
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void CbmMuchGeoScheme::CreateHitArrays(char* hitClassName) {
  for(vector<CbmMuchModule*>::iterator it=GetModules().begin(); it!=GetModules().end(); it++){
    CbmMuchModule* module = (*it);
    if(module->GetDetectorType()!=1) continue;
    CbmMuchModuleGem* mod = (CbmMuchModuleGem*)module;
    mod->SetHits(new TClonesArray(hitClassName, 10));
  }
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void CbmMuchGeoScheme::CreateClusterArrays(char* clusterClassName) {
  for(vector<CbmMuchModule*>::iterator it=GetModules().begin(); it!=GetModules().end(); it++){
    CbmMuchModule* module = (*it);
    if(module->GetDetectorType()!=1) continue;
    CbmMuchModuleGem* mod = (CbmMuchModuleGem*)module;
    mod->SetClusters(new TClonesArray(clusterClassName, 10));
  }
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void CbmMuchGeoScheme::ClearPointArrays() {
  for(vector<CbmMuchModule*>::iterator it=GetModules().begin(); it!=GetModules().end(); it++){
    CbmMuchModule* module = (*it);
    if(module->GetDetectorType()!=1) continue;
    CbmMuchModuleGem* mod = (CbmMuchModuleGem*)module;
    TClonesArray* points = mod->GetPoints();
    points->Clear();
  }
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void CbmMuchGeoScheme::ClearHitArrays() {
  for(vector<CbmMuchModule*>::iterator it=GetModules().begin(); it!=GetModules().end(); it++){
    CbmMuchModule* module = (*it);
    if(module->GetDetectorType()!=1) continue;
    CbmMuchModuleGem* mod = (CbmMuchModuleGem*)module;
    TClonesArray* hits = mod->GetPoints();
    hits->Clear();
  }
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void CbmMuchGeoScheme::ClearClusterArrays() {
  for(vector<CbmMuchModule*>::iterator it=GetModules().begin(); it!=GetModules().end(); it++){
    CbmMuchModule* module = (*it);
    if(module->GetDetectorType()!=1) continue;
    CbmMuchModuleGem* mod = (CbmMuchModuleGem*)module;
    TClonesArray* clusters = mod->GetClusters();
    clusters->Clear();
  }
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void CbmMuchGeoScheme::ResetPads() {
  for (Int_t i = 0; i < fPads.size(); i++)
    fPads[i]->Reset();
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
vector<CbmMuchLayerSide*> CbmMuchGeoScheme::GetLayerSides(Int_t iStation) {
  try
  {
    return fSides.at(iStation);
  }
  catch ( std::out_of_range exc)
  {
    Fatal("GetLayerSides","No input array of stations.");
  }
}

//Int_t CbmMuchGeoScheme::GetLayerSideNr(Int_t detId){
//  int i = 0;
//  Int_t iStation = GetStationIndex(detId);
//  for(Int_t iSt=0; iSt<=iStation; iSt++){
//    i++;
//    vector<CbmMuchLayerSide*> sides = GetLayerSides(iSt);
//    for(Int_t iSide=0; iSide<sides.size(); iSide++){
//      i++;
//    }
//  }
// // assert(fMapSides[sideId] + 1 == sideNumber);
//  return i;
//}

Int_t CbmMuchGeoScheme::GetLayerSideNr(Int_t detId) {
  Int_t sideId = GetLayerSideByDetId(detId)->GetDetectorId();
  if (fMapSides.find(sideId) == fMapSides.end())
    Fatal("GetLayerSideNr", "Wrong side id or no modules in the layer side");
  return fMapSides[sideId] + 1;
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void CbmMuchGeoScheme::ReadGeoFile(const char* geoName) {
  // Read geometry parameters from file
  fstream geo(geoName, fstream::in);
  Char_t b[200]; // read buffer
  Int_t i;
  // Skip header
  while (!geo.eof()) {
    geo.getline(b, 200);
    if (b[0] != '#')
      break;
  }

  geo.getline(b, 200); // # General Information
  geo.get(b, 30);
  geo >> fMuchZ1;
  geo.getline(b, 200);
  geo.get(b, 30);
  geo >> fAcceptanceTanMin;
  geo.getline(b, 200);
  geo.get(b, 30);
  geo >> fAcceptanceTanMax;
  geo.getline(b, 200);
  geo.get(b, 30);
  geo >> fNabs;
  geo.getline(b, 200);
  geo.get(b, 30);
  geo >> fNst;
  geo.getline(b, 200);

  // Create arrays of absorber parameters
  fAbsorberZ1.Set(fNabs); // Absorber Zin position [cm]
  fAbsorberLz.Set(fNabs); // Absorber thickness [cm]
  fAbsorberMat.Set(fNabs); // Absorber material
  // Create arrays of station parameters
  fStationZ0.Set(fNst); // Station Zceneter [cm]
  fNlayers.Set(fNst); // Number of layers
  fDetType.Set(fNst); // Detector type
  fLayersDz.Set(fNst); // Distance between layers [cm]
  fSupportLz.Set(fNst); // Support thickness [cm]
  fModuleDesign.Set(fNst); // Module design (0/1)

  geo.getline(b, 200);

  // # Absorber specification
  geo.getline(b, 200);
  geo.get(b, 30);
  for (i = 0; i < fNabs; i++)
    geo >> fAbsorberZ1[i];
  geo.getline(b, 200);
  geo.get(b, 30);
  for (i = 0; i < fNabs; i++)
    geo >> fAbsorberLz[i];
  geo.getline(b, 200);
  geo.get(b, 30);
  for (i = 0; i < fNabs; i++)
    geo >> fAbsorberMat[i];
  geo.getline(b, 200);

  geo.getline(b, 200);

  // # Station specification
  geo.getline(b, 200);
  geo.get(b, 30);
  for (i = 0; i < fNst; i++)
    geo >> fStationZ0[i];
  geo.getline(b, 200);
  geo.get(b, 30);
  for (i = 0; i < fNst; i++)
    geo >> fNlayers[i];
  geo.getline(b, 200);
  geo.get(b, 30);
  for (i = 0; i < fNst; i++)
    geo >> fDetType[i];
  geo.getline(b, 200);
  geo.get(b, 30);
  for (i = 0; i < fNst; i++)
    geo >> fLayersDz[i];
  geo.getline(b, 200);
  geo.get(b, 30);
  for (i = 0; i < fNst; i++)
    geo >> fSupportLz[i];
  geo.getline(b, 200);
  geo.get(b, 30);
  for (i = 0; i < fNst; i++)
    geo >> fModuleDesign[i];
  geo.getline(b, 200);

  geo.getline(b, 200);

  // # GEM module specification
  geo.getline(b, 200);
  geo.get(b, 30);
  geo >> fActiveLx;
  geo.getline(b, 200);
  geo.get(b, 30);
  geo >> fActiveLy;
  geo.getline(b, 200);
  geo.get(b, 30);
  geo >> fActiveLz;
  geo.getline(b, 200);
  geo.get(b, 30);
  geo >> fSpacerLx;
  geo.getline(b, 200);
  geo.get(b, 30);
  geo >> fSpacerLy;
  geo.getline(b, 200);
  geo.get(b, 30);
  geo >> fOverlapY;
  geo.getline(b, 200);

  geo.getline(b, 200);

  // # Straw module specification
  geo.getline(b, 200);
  geo.get(b, 30);
  geo >> fStrawLz;
  geo.getline(b, 200);
  geo.close();

}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmMuchGeoScheme::Print() {
  Int_t i;
  // Print geometry parameters
  printf("\n");
  printf("General information:\n");
  printf("  MuchCave Zin position [cm]  :%7.1f\n", fMuchZ1);
  printf("  Acceptance tangent min      :%7.1f\n", fAcceptanceTanMin);
  printf("  Acceptance tangent max      :%7.1f\n", fAcceptanceTanMax);
  printf("  Number of absorbers         :%7i\n", fNabs);
  printf("  Number of stations          :%7i\n", fNst);
  printf("\n");
  printf("Absorber specification:\n");
  printf("  Absorber Zin position [cm]  :");
  for (i = 0; i < fNabs; i++)
    printf("%7.1f", fAbsorberZ1[i]);
  printf("\n");
  printf("  Absorber thickness [cm]     :");
  for (i = 0; i < fNabs; i++)
    printf("%7.1f", fAbsorberLz[i]);
  printf("\n");
  printf("  Absorber material           :");
  for (i = 0; i < fNabs; i++)
    printf("      %c", fAbsorberMat[i]);
  printf("\n");
  printf("\n");
  printf("Station specification:\n");
  printf("  Station Zceneter [cm]       :");
  for (i = 0; i < fNst; i++)
    printf("%7.1f", fStationZ0[i]);
  printf("\n");
  printf("  Number of layers            :");
  for (i = 0; i < fNst; i++)
    printf("%7i", fNlayers[i]);
  printf("\n");
  printf("  Detector type               :");
  for (i = 0; i < fNst; i++)
    printf("%7i", fDetType[i]);
  printf("\n");
  printf("  Distance between layers [cm]:");
  for (i = 0; i < fNst; i++)
    printf("%7.1f", fLayersDz[i]);
  printf("\n");
  printf("  Support thickness [cm]      :");
  for (i = 0; i < fNst; i++)
    printf("%7.1f", fSupportLz[i]);
  printf("\n");
  printf("  Module design (0/1)         :");
  for (i = 0; i < fNst; i++)
    printf("%7i", fModuleDesign[i]);
  printf("\n");

  printf("\n");
  printf("GEM module specification:\n");
  printf("  Active volume lx [cm]       :%7.1f\n", fActiveLx);
  printf("  Active volume ly [cm]       :%7.1f\n", fActiveLy);
  printf("  Active volume lz [cm]       :%7.1f\n", fActiveLz);
  printf("  Spacer lx [cm]              :%7.1f\n", fSpacerLx);
  printf("  Spacer ly [cm]              :%7.1f\n", fSpacerLy);
  printf("  Overlap along y axis [cm]   :%7.1f\n", fOverlapY);
  printf("\n");

  printf("\n");
  printf("Straw module specification:\n");
  printf("  Straw thickness [cm]        :%7.1f\n", fStrawLz);
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmMuchGeoScheme::CreateMuchCave() {
  // Calculate Z2 position of much cave according to position of the last
  // station layer
  Double_t muchZ2 = fMuchZ1 + fStationZ0[fNst - 1] + fLayersDz[fNst - 1]
                                                               * (fNlayers[fNst - 1] - 1) / 2. + (fSupportLz[fNst - 1] + fActiveLz) / 2.
                                                               + 3.;
  // Calculate cave radia
  Double_t muchRmin1 = fMuchZ1 * fAcceptanceTanMin - 0.001;
  Double_t muchRmin2 = muchZ2 * fAcceptanceTanMin - 0.001;
  Double_t muchRmax2 = muchZ2 * fAcceptanceTanMax + 200; // 100 cm added for safety
  Double_t muchRmax1 = muchRmax2;
  // Calculate cone thickness
  Double_t muchDz = (muchZ2 - fMuchZ1) / 2.;
  // Calculate cone position
  Double_t fMuchZ0 = fMuchZ1 + muchDz;
  fMuchCave = new TGeoCone(muchDz, muchRmin1, muchRmax1, muchRmin2, muchRmax2);
  CreateAbsorbers();
  CreateStations();
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmMuchGeoScheme::CreateAbsorbers() {
  for (Int_t i = 0; i < fNabs; i++) {
    Double_t dz = fAbsorberLz[i] / 2.;
    Double_t globalZ1 = fAbsorberZ1[i] + fMuchZ1;
    Double_t globalZ2 = fAbsorberZ1[i] + 2 * dz + fMuchZ1;
    Double_t rmin1 = globalZ1 * fAcceptanceTanMin;
    Double_t rmax1 = globalZ1 * fAcceptanceTanMax + 20;
    Double_t rmin2 = globalZ2 * fAcceptanceTanMin;
    Double_t rmax2 = globalZ2 * fAcceptanceTanMax + 20;
    fAbsorbers->Add(new TGeoCone(dz, rmin1, rmax1, rmin2, rmax2));
  }
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmMuchGeoScheme::CreateStations() {
  Info("CreateStations", "...");
  // Create stations
  for (Int_t st = 0; st < fNst; st++) {
    CbmMuchStation* station;
    switch (fDetType[st]){
      case 1:
        station = CreateStationGem(st);
        break;
      case 2:
        station = CreateStationStraw(st);
        break;
      default:
        Fatal("CbmMuchGeoScheme", "Detector type %i not defined",fDetType[st]);
    }
    CbmMuchLayer* layer0 = station->GetLayer(0);
    Double_t supDx  = layer0->GetSupportDx();
    Double_t supDy  = layer0->GetSupportDy();
    Double_t supDz  = layer0->GetSupportDz();
    station->SetTubeRmin(station->GetRmin());
    station->SetTubeRmax(TMath::Sqrt(supDx*supDx+supDy*supDy)+10);
    fStations->Add(station);
  } // stations
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
CbmMuchStation* CbmMuchGeoScheme::CreateStationGem(Int_t st){
  Double_t stGlobalZ0 = fStationZ0[st] + fMuchZ1;
  TVector3 size = TVector3(fActiveLx, fActiveLy, fActiveLz);
  Double_t stDz = ((fNlayers[st] - 1) * fLayersDz[st] + fSupportLz[st]
                                                                   + fActiveLz) / 2. + 1;
  Double_t stGlobalZ2 = stGlobalZ0 + stDz;
  Double_t rmin = stGlobalZ2 * fAcceptanceTanMin;
  Double_t rmax = stGlobalZ2 * fAcceptanceTanMax;

  CbmMuchStation* station = new CbmMuchStation(st, stGlobalZ0);
  station->SetRmin(rmin);
  station->SetRmax(rmax);
  station->SetModuleDesign(fModuleDesign[st]);

  // Create layers
  for (Int_t l = 0; l < fNlayers[st]; l++) {
    Double_t layerZ0 = (l + 1 / 2. - fNlayers[st] / 2.) * fLayersDz[st];
    Double_t layerGlobalZ0 = layerZ0 + stGlobalZ0;
    Double_t sideDz = fSupportLz[st] + fActiveLz / 2. + 0.01;
    Double_t layerDz = fSupportLz[st]/2. + fActiveLz+1;
    CbmMuchLayer* layer = new CbmMuchLayer(st, l, layerGlobalZ0,layerZ0,layerDz);
    layer->GetSideB()->SetZ(layerGlobalZ0 + sideDz);
    layer->GetSideF()->SetZ(layerGlobalZ0 - sideDz);

    if (fModuleDesign[st]) {
      // Create modules
      Int_t mF = 0, mB = 0;
      Double_t mDx = fSpacerLx + fActiveLx / 2.;
      Double_t mDy = fSpacerLy + fActiveLy / 2.;
      Int_t nx = Int_t(TMath::Ceil(rmax / 2. / mDx));
      Int_t ny = Int_t(TMath::Ceil(rmax / (fActiveLy - fOverlapY)));
      for (Int_t ix = 1; ix <= nx; ix++) {
        Double_t mX = mDx * (2 * ix - 1);
        Double_t mZ = sideDz;
        for (Int_t iy = -ny; iy < ny; iy += 1) {
          mZ *= -1;
          Bool_t isBack = (mZ > 0);
          CbmMuchLayerSide* side = layer->GetSide(isBack);
          Int_t &m = isBack ? mB : mF;
          Double_t mY = (fActiveLy - fOverlapY) * (iy+1./2.);

          Int_t intersect = Intersect(mX, mY, mDx, mDy, rmin);
          if (intersect == 2)
            continue;
          Double_t rHole = (intersect == 1) ? rmin : -1;
          // Skip module if not in the acceptance
          if (pow(mX - fActiveLx / 2., 2) + pow(mY - fActiveLy / 2., 2)
              > rmax * rmax && pow(mX - fActiveLx / 2., 2) + pow(mY
                  + fActiveLy / 2., 2) > rmax * rmax)
            continue;
          // Create module with positive x
          TVector3 pos = TVector3(mX, mY, mZ + layerGlobalZ0);
          side->AddModule(
              new CbmMuchModuleGem(st, l, isBack, m, pos, size, rHole));
          m++;
          // Create module with negative x
          pos = TVector3(-mX, mY, mZ + layerGlobalZ0);
          side->AddModule(
              new CbmMuchModuleGem(st, l, isBack, m, pos, size, rHole));
          m++;
          //printf("mx=%6.2f my=%6.2f\n",mX,mY);
        } // mY
      } // mX
      //printf("\n");
      // Set support shape
      layer->SetSupportDx(nx * (fActiveLx + 2. * fSpacerLx));
      layer->SetSupportDy((2*ny + 1) * (fActiveLy - fOverlapY) / 2. + fOverlapY
          / 2. + fSpacerLy);
      layer->SetSupportDz(fSupportLz[st] / 2.);
    } else {
      TVector3 pos = TVector3(0, 0, -sideDz + layerGlobalZ0);
      size = TVector3(2 * rmax, 2 * rmax, fActiveLz);
      CbmMuchLayerSide* side = layer->GetSideF();
      side->AddModule(new CbmMuchModuleGem(st, l, 0, 0, pos, size, rmin));
      layer->SetSupportDx(rmax);
      layer->SetSupportDy(rmax);
      layer->SetSupportDz(fSupportLz[st] / 2.);
    }
    station->AddLayer(layer);
  } // layers
  return station;
}


// -------------------------------------------------------------------------
CbmMuchStation* CbmMuchGeoScheme::CreateStationStraw(Int_t st){
  Double_t stGlobalZ0 = fStationZ0[st] + fMuchZ1;
  Double_t stDz = ((fNlayers[st] - 1) * fLayersDz[st] + fStrawLz) / 2. + 1;
  Double_t stGlobalZ2 = stGlobalZ0 + stDz;
  Double_t rmin = stGlobalZ2 * fAcceptanceTanMin;
  Double_t rmax = stGlobalZ2 * fAcceptanceTanMax;

  CbmMuchStation* station = new CbmMuchStation(st, stGlobalZ0);
  station->SetRmin(rmin);
  station->SetRmax(rmax);
  station->SetModuleDesign(fModuleDesign[st]);


  // Create layers
  for (Int_t l = 0; l < fNlayers[st]; l++) {
    Double_t layerZ0 = (l + 1 / 2. - fNlayers[st] / 2.) * fLayersDz[st];
    Double_t layerGlobalZ0 = layerZ0 + stGlobalZ0;
    Double_t sideDz = fStrawLz / 2. + 0.001; // +0.001 to prevent overlaps
    Double_t layerDz = fStrawLz+1; // +1 for safety

    CbmMuchLayer* layer = new CbmMuchLayer(st, l, layerGlobalZ0,layerZ0,layerDz);
    layer->GetSideF()->SetZ(layerGlobalZ0 - sideDz);
    layer->GetSideB()->SetZ(layerGlobalZ0 + sideDz);

    if (fModuleDesign[st]){
      Fatal("CbmMuchGeoScheme","Station %i - detailed module design not implemented for straws",st);
    }

    // Create two modules (one per each layer side)
    TVector3 posF = TVector3(0, 0, -sideDz + layerGlobalZ0);
    TVector3 posB = TVector3(0, 0, +sideDz + layerGlobalZ0);
    TVector3 size = TVector3(2 * rmax, 2 * rmax, fStrawLz);
    layer->GetSideF()->AddModule(new CbmMuchModuleStraws(st, l, 0, 0, posF, size, rmin));
    layer->GetSideB()->AddModule(new CbmMuchModuleStraws(st, l, 0, 0, posB, size, rmin));

    layer->SetSupportDx(rmax);
    layer->SetSupportDy(rmax);
    layer->SetSupportDz(fSupportLz[st] / 2.);
    station->AddLayer(layer);
  } // layers
  return station;
}

// -------------------------------------------------------------------------
Int_t CbmMuchGeoScheme::Intersect(Float_t x, Float_t y, Float_t dx, Float_t dy,
    Float_t r) {
  Float_t x1 = x - dx;
  Float_t x2 = x + dx;
  Float_t y1 = y - dy;
  Float_t y2 = y + dy;
  Int_t nCornersInside = 0;
  if (x1 * x1 + y1 * y1 < r * r)
    nCornersInside++;
  if (x2 * x2 + y1 * y1 < r * r)
    nCornersInside++;
  if (x1 * x1 + y2 * y2 < r * r)
    nCornersInside++;
  if (x2 * x2 + y2 * y2 < r * r)
    nCornersInside++;
  if (nCornersInside == 4)
    return 2;
  if (nCornersInside)
    return 1;
  if (!nCornersInside && x1 < r && y1 < 0 && y2 > 0)
    return 1;
  return 0;
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
TVector2 CbmMuchGeoScheme::GetMinPadSize(Int_t iStation){
  Double_t padMinLx = std::numeric_limits<Double_t>::max();
  Double_t padMinLy = std::numeric_limits<Double_t>::max();
  vector<CbmMuchSector*> sectors = fSectors.at(iStation);
  for(vector<CbmMuchSector*>::iterator it = sectors.begin(); it!=sectors.end(); it++){
    CbmMuchSector* sector = (*it);
    if(!sector) continue;
    Double_t padLx = sector->GetDx();
    Double_t padLy = sector->GetDy();
    if(padLx < padMinLx) padMinLx = padLx;
    if(padLy < padMinLy) padMinLy = padLy;
  }
  return TVector2(padMinLx, padMinLy);
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
TVector2 CbmMuchGeoScheme::GetMaxPadSize(Int_t iStation){
  Double_t padMaxLx = std::numeric_limits<Double_t>::min();
  Double_t padMaxLy = std::numeric_limits<Double_t>::min();
  vector<CbmMuchSector*> sectors = fSectors.at(iStation);
  for(vector<CbmMuchSector*>::iterator it = sectors.begin(); it!=sectors.end(); it++){
    CbmMuchSector* sector = (*it);
    if(!sector) continue;
    Double_t padLy = sector->GetDy();
    Double_t padLx = sector->GetDx();
    if(padLx > padMaxLx) padMaxLx = padLx;
    if(padLy > padMaxLy) padMaxLy = padLy;
  }
  return TVector2(padMaxLx, padMaxLy);
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
vector<CbmMuchModule*> CbmMuchGeoScheme::GetModules(){
  vector<CbmMuchModule*> modules;
  for(Int_t iStation =0; iStation < GetNStations(); ++iStation){
    for(vector<CbmMuchModule*>::iterator it=GetModules(iStation).begin(); it!=GetModules(iStation).end(); it++){
      modules.push_back(*it);
    }
  }
  return modules;
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
vector<CbmMuchModule*> CbmMuchGeoScheme::GetModules(Int_t iStation){
  try
  {
    return fModules.at(iStation);
  }
  catch ( std::out_of_range exc)
  {
    Fatal("GetModules","No input array of stations.");
  }
}
// -------------------------------------------------------------------------

ClassImp(CbmMuchGeoScheme)
