// -------------------------------------------------------------------------
// -----                   CbmMuchGeoScheme header file                -----
// -----                  Created 18/02/08  by E. Kryshen              -----
// -------------------------------------------------------------------------
// TODO add comments

/** CbmMuchGeoScheme
 *@author Evgeny Kryshen <e.kryshen@gsi.de>
 *@since 18.02.08
 *@version 1.0
 **
 ** The detector ID consists of:
 **   system ID      (0-15, MUCH=4), bits 34-37
 **   station number (0-7), bits 31-33
 **   layer number   (0-7), bits 28-30
 **   layer side     (0-1), bits 27 [0 - Front, 1 - Back]
 **   module number  (0-511), bits 18-26
 **   sector number  (0-2047), bits 7-17 
 **   channel number (0-127), bits 0-6
 **
 **/
#ifndef CbmMuchGeoScheme_H
#define CbmMuchGeoScheme_H 1

#include "CbmDetectorList.h"
#include "TObject.h"
#include "TObjArray.h"
#include "TArrayD.h"
#include "TArrayI.h"
#include "TArrayC.h"
#include "TGeoCone.h"
#include <vector>
#include <map>

class CbmMuchStation;
class CbmMuchLayer;
class CbmMuchLayerSide;
class CbmMuchModule;
class CbmMuchSector;
class CbmMuchPad;

using std::vector;
using std::map;

class CbmMuchGeoScheme : public TObject{

public:
  ~CbmMuchGeoScheme();
  static CbmMuchGeoScheme* Instance();
  Bool_t IsInitialized() {return fInitialized;}

  // Static functions to access indices by detetctor ids
  static Int_t GetSystemIndex(Long64_t detId)    { return ( detId & ( (Long64_t)  15 << 34) ) >> 34; }
  static Int_t GetStationIndex(Long64_t detId)   { return ( detId & ( (Long64_t)   7 << 31) ) >> 31; }
  static Int_t GetLayerIndex(Long64_t detId)     { return ( detId & ( (Long64_t)   7 << 28) ) >> 28; }
  static Int_t GetLayerSideIndex(Long64_t detId) { return ( detId & ( (Long64_t)   1 << 27) ) >> 27; }
  static Int_t GetModuleIndex(Long64_t detId)    { return ( detId & ( (Long64_t) 511 << 18) ) >> 18; }
  static Int_t GetSectorIndex(Long64_t detId)    { return ( detId & ( (Long64_t)2047 <<  7) ) >>  7; }
  static Int_t GetChannelIndex(Long64_t detId)   { return   detId &   (Long64_t) 127;                }

  /* Static functions to access detector ids of mother volumes 
     by detector ids of the daughter volumes */
/*   static Int_t GetSystemId(Long64_t detId)    { return (detId >> 32) << 32; } */
/*   static Int_t GetStationId(Long64_t detId)   { return (detId >> 29) << 29; } */
/*   static Int_t GetLayerId(Long64_t detId)     { return (detId >> 26) << 26; } */
/*   static Int_t GetLayerSideId(Long64_t detId) { return (detId >> 25) << 25; } */
/*   static Int_t GetModuleId(Long64_t detId)    { return (detId >> 16) << 16; } */
/*   static Int_t GetSectorId(Long64_t detId)    { return (detId >>  7) <<  7; } */
/*   static Int_t GetChannelId(Long64_t detId)   { return  detId;              } */

  static Long64_t GetDetectorId(Long64_t iStation) 
    { return ((Long64_t)kMUCH << 34) | (iStation << 31); }
  static Long64_t GetDetectorId(Long64_t iStation, Long64_t iLayer) 
    { return ((Long64_t)kMUCH << 34) | (iStation << 31) | (iLayer << 28); }
  static Long64_t GetDetectorId(Long64_t iStation, Long64_t iLayer, Long64_t iSide) 
    { return ((Long64_t)kMUCH << 34) | (iStation << 31) | (iLayer << 28) | (iSide << 27); }
  static Long64_t GetDetectorId(Long64_t iStation, Long64_t iLayer, Long64_t iSide, Long64_t iModule) 
    { return ((Long64_t)kMUCH << 34) | (iStation << 31) | (iLayer << 28) | (iSide << 27) | (iModule << 18); }
  static Long64_t GetDetectorId(Long64_t iStation, Long64_t iLayer, Long64_t iSide, Long64_t iModule, Long64_t iSector) 
    { return ((Long64_t)kMUCH << 34) | (iStation << 31) | (iLayer << 28) | (iSide << 27) | (iModule << 18) | (iSector << 7); }

  static Long64_t GetDetIdFromSector(Long64_t sectorId, Long64_t iChannel) { return sectorId | iChannel;     }
  static Long64_t GetDetIdFromModule(Long64_t moduleId, Long64_t iSector)  { return moduleId | (iSector << 7); }


  // Get geometry objects by indices
  CbmMuchStation*   GetStation(Int_t iStation);
  CbmMuchLayer*     GetLayer(Int_t iStation, Int_t iLayer);
  CbmMuchLayerSide* GetLayerSide(Int_t iStation, Int_t iLayer, Bool_t iSide);
  CbmMuchModule*    GetModule(Int_t iStation, Int_t iLayer, Bool_t iSide, Int_t iModule);
  CbmMuchSector*    GetSector(Int_t iStation, Int_t iLayer, Bool_t iSide, Int_t iModule, Int_t iSector);
  CbmMuchPad*       GetPad(Int_t iStation, Int_t iLayer, Bool_t iSide, Int_t iModule, Int_t iSector, Int_t iChannel);

  // Get geometry objects by detector id
  CbmMuchStation*   GetStationByDetId(Long64_t detId);
  CbmMuchLayer*     GetLayerByDetId(Long64_t detId);
  CbmMuchLayerSide* GetLayerSideByDetId(Long64_t detId);
  CbmMuchModule*    GetModuleByDetId(Long64_t detId);
  CbmMuchSector*    GetSectorByDetId(Long64_t detId);
  CbmMuchPad*       GetPadByDetId(Long64_t detId);

  Int_t GetNStations()            { return fStations->GetEntries(); }
  Int_t GetNAbsorbers()           { return fNabs; }
  TObjArray* GetStations()        { return fStations;   }
  TObjArray* GetAbsorbers()       { return fAbsorbers;  }
  TGeoCone*  GetMuchCave()        { return fMuchCave;   }
  Double_t GetMuchCaveZ0()        { return fMuchZ1+fMuchCave->GetDz(); }
  Double_t GetAbsorberZ0(Int_t i) { return fMuchZ1+fAbsorberZ1[i]+((TGeoCone*) fAbsorbers->At(i))->GetDz();}
  Char_t GetAbsorberMat(Int_t i)  { return fAbsorberMat[i]; }
  
  Double_t GetActiveLx() { return fActiveLx; }
  Double_t GetActiveLy() { return fActiveLy; }
  Double_t GetActiveLz() { return fActiveLz; }
  Double_t GetSpacerLx() { return fSpacerLx; }
  Double_t GetSpacerLy() { return fSpacerLy; }
  Double_t GetLayersDz(Int_t st) { return fLayersDz[st]; }
  
  void Init(TObjArray* stations);
  void CreatePointArrays(char* pointClassName);
  void CreateHitArrays(char* hitClassName);
  void CreateClusterArrays(char* clusterClassName);
  void ClearPointArrays();
  void ClearHitArrays();
  void ClearClusterArrays();
  void InitGrid();
  void ResetPads();
  vector<CbmMuchPad*> GetPads() { return fPads; }
  vector<CbmMuchLayerSide*> GetLayerSides(Int_t iStation);
  Int_t GetLayerSideNr(Long64_t detId);
  
  void ReadGeoFile(const char* geoName);
  void Print();
  void CreateMuchCave();
  Bool_t IsModuleDesign() { return fModuleDesign; }

private:
  CbmMuchGeoScheme();
  void CreateAbsorbers();
  void CreateStations();
  Int_t Intersect(Float_t x, Float_t y, Float_t dx, Float_t dy, Float_t r);
  static CbmMuchGeoScheme*  fInstance;
  static Bool_t             fInitialized;     // Defines whether the instance was initialized
  static Bool_t             fGridInitialized; // Defines whether grid of the instance was initialized
  vector<CbmMuchPad*>       fPads;            //!
  map<Long64_t,Int_t>       fMapSides;
  
  TObjArray* fStations; //!
  TObjArray* fAbsorbers;//!
  TGeoCone*  fMuchCave; //!
  
  // Input parameters
  Double_t fMuchZ1;           // MuchCave Zin position [cm] 
  Double_t fAcceptanceTanMin; // Acceptance tangent min
  Double_t fAcceptanceTanMax; // Acceptance tangent max
  Int_t    fNabs;             // Number of absorbers
  Int_t    fNst;              // Number of stations
  Bool_t   fModuleDesign;     /* 1 - detailed design (modules at two sides) 
                               * 0 - simple design (1 module per layer)     */
  
  Double_t fActiveLx;         // Active volume lx [cm]
  Double_t fActiveLy;         // Active volume ly [cm]
  Double_t fActiveLz;         // Active volume lz [cm]
  Double_t fSpacerLx;         // Spacer lx [cm]
  Double_t fSpacerLy;         // Spacer ly [cm]
  Double_t fOverlapY;         // Overlap along y axis [cm]

  TArrayD fAbsorberZ1;        // Absorber Zin position [cm]
  TArrayD fAbsorberLz;        // Absorber thickness [cm]
  TArrayC fAbsorberMat;       // Absorber material
  TArrayD fStationZ0;         // Station Zceneter [cm] 
  TArrayI fNlayers;           // Number of layers
  TArrayI fDetType;           // Detector type
  TArrayD fLayersDz;          // Distance between layers [cm]
  TArrayD fSupportLz;         // Support thickness [cm]
  TArrayD fSigmaXmin;         // Sigma X minimum [cm]
  TArrayD fSigmaYmin;         // Sigma Y maximum [cm]
  TArrayD fSigmaXmax;         // Sigma X minimum [cm]
  TArrayD fSigmaYmax;         // Sigma Y maximum [cm]
  TArrayD fOccupancyMax;      // Maximum occupancy

ClassDef(CbmMuchGeoScheme,1);
};

#endif
