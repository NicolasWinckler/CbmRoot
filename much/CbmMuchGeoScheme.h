// -------------------------------------------------------------------------
// -----                   CbmMuchGeoScheme header file                -----
// -----                  Created 18/02/08  by E. Kryshen              -----
// -------------------------------------------------------------------------
// TODO add comments

/** CbmMuchGeoScheme
 *@author Evgeny Kryshen <e.kryshen@gsi.de>
 *@since 18.02.08
 *@version 1.0
 *
 * The detector ID consists of:
 *   system ID      (0-15, MUCH=4), bits 0-3
 *   station number (0-7),          bits 4-6
 *   layer number   (0-7),          bits 7-9
 *   layer side     (0-1),          bits 10 [0 - Front, 1 - Back]
 *   module number  (0-511),        bits 11-19
 *
 * The channel ID consists of:
 *   sector number  (0-2047),       bits 0-10
 *   channel number (0-127),        bits 11-17
 */

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

// Length of the index of the corresponding volume
#define WL_SYSTEM 15
#define WL_STATION 7
#define WL_LAYER 7
#define WL_LAYERSIDE 1
#define WL_MODULE 511
#define WL_SECTOR 2047
#define WL_CHANNEL 127
// Number of a start bit for each volume
#define SB_SYSTEM 0
#define SB_STATION 4
#define SB_LAYER 7
#define SB_LAYERSIDE 10
#define SB_MODULE 11
#define SB_SECTOR 0
#define SB_CHANNEL 11

class CbmMuchGeoScheme: public TObject {

public:

	/** Destructor.	 */
	~CbmMuchGeoScheme();
	static CbmMuchGeoScheme* Instance();
	/** Gets whether the geometry scheme is initialized. */
	Bool_t IsInitialized() {
		return fInitialized;
	}

	/**
	 * Gets system index for the given detector Id.
	 * @param detId   Detector Id.
	 * @return        System index (MUCH).
	 */
	static Int_t GetSystemIndex(Int_t detId) {
		return detId & WL_SYSTEM;
	}
	/**
	 * Gets station index for the given detector Id.
	 * @param detId   Detector Id.
	 * @return        Station index within the MUCH system.
	 */
	static Int_t GetStationIndex(Int_t detId) {
		return (detId & (WL_STATION << SB_STATION)) >> SB_STATION;
	}
	/**
	 * Gets layer index for the given detector Id.
	 * @param detId   Detector Id.
	 * @return        Layer index within the station.
	 */
	static Int_t GetLayerIndex(Int_t detId) {
		return (detId & (WL_LAYER << SB_LAYER)) >> SB_LAYER;
	}
	/**
	 * Gets layer side index for the given detector Id.
	 * @param detId   Detector Id.
	 * @return        Layer side index within the layer.
	 */
	static Int_t GetLayerSideIndex(Int_t detId) {
		return (detId & (WL_LAYERSIDE << SB_LAYERSIDE)) >> SB_LAYERSIDE;
	}
	/**
	 * Gets module index for the given detector Id.
	 * @param detId   Detector Id.
	 * @return        Module index within the layer side.
	 */
	static Int_t GetModuleIndex(Int_t detId) {
		return (detId & (WL_MODULE << SB_MODULE)) >> SB_MODULE;
	}
	/**
	 * Gets sector index for the given channel Id.
	 * @param channelId   Channel Id.
	 * @return            Sector index within the module.
	 */
	static Int_t GetSectorIndex(Int_t channelId) {
		return channelId & WL_SECTOR;
	}
	/**
	 * Gets channel index for the given channel Id.
	 * @param channelId   Channel Id.
	 * @return            Channel index within the sector.
	 */
	static Int_t GetChannelIndex(Int_t channelId) {
		return (channelId & (WL_CHANNEL << SB_CHANNEL)) >> SB_CHANNEL;
	}

	/**
	 * Gets detector Id by station index.
	 * @param iStation   Station index within the MUCH system.
	 * @return           Detector Id.
	 */
	static Int_t GetDetectorId(Int_t iStation) {
		return (kMUCH << SB_SYSTEM) |
		(iStation << SB_STATION);
	}
	/**
	 * Gets detector Id by station index and layer index.
	 * @param iStation   Station index within the MUCH system.
	 * @param iLayer     Layer index within the station.
	 * @return           Detector Id.
	 */
	static Int_t GetDetectorId(Int_t iStation, Int_t iLayer) {
		return (kMUCH << SB_SYSTEM) |
		(iStation << SB_STATION) |
		(iLayer << SB_LAYER);
	}
	/**
	 * Gets detector Id by station index, layer index and layer side index.
	 * @param iStation   Station index within the MUCH system.
	 * @param iLayer     Layer index within the station.
	 * @param iSide      Layer side index within the layer.
	 * @return           Detector Id.
	 */
	static Int_t GetDetectorId(Int_t iStation, Int_t iLayer, Int_t iSide) {
		return (kMUCH << SB_SYSTEM) |
		(iStation << SB_STATION) |
		(iLayer << SB_LAYER) |
		(iSide << SB_LAYERSIDE);
	}
	/**
	 * Gets detector Id by station index, layer index, layer side index and module index.
	 * @param iStation   Station index within the MUCH system.
	 * @param iLayer     Layer index within the station.
	 * @param iSide      Layer side index within the layer.
	 * @param iModule    Module index within the layer side.
	 * @return           Detector Id.
	 */
	static Int_t GetDetectorId(Int_t iStation, Int_t iLayer, Int_t iSide, Int_t iModule) {
		return (kMUCH << SB_SYSTEM) |
		(iStation << SB_STATION) |
		(iLayer << SB_LAYER) |
		(iSide << SB_LAYERSIDE) |
		(iModule << SB_MODULE);
	}
	/**
	 * Gets channel Id by sector index and channel index.
	 * @param iSector    Sector index within the module.
	 * @param iChannel   Channel index within the sector.
	 * @return           Detector Id.
	 */
	static Int_t GetChannelId(Int_t iSector, Int_t iChannel) {
		return (iSector << SB_SECTOR) | (iChannel << SB_CHANNEL);
	}

	// Get geometry objects by indices
	CbmMuchStation* GetStation(Int_t iStation);
	CbmMuchLayer* GetLayer(Int_t iStation, Int_t iLayer);
	CbmMuchLayerSide* GetLayerSide(Int_t iStation, Int_t iLayer, Bool_t iSide);
	CbmMuchModule* GetModule(Int_t iStation, Int_t iLayer, Bool_t iSide, Int_t iModule);
	CbmMuchSector* GetSector(Int_t iStation, Int_t iLayer, Bool_t iSide, Int_t iModule, Int_t iSector);
	CbmMuchPad* GetPad(Int_t iStation, Int_t iLayer, Bool_t iSide, Int_t iModule, Int_t iSector, Int_t iChannel);

	// Get geometry objects by detector id
	CbmMuchStation* GetStationByDetId(Int_t detId);
	CbmMuchLayer* GetLayerByDetId(Int_t detId);
	CbmMuchLayerSide* GetLayerSideByDetId(Int_t detId);
	CbmMuchModule* GetModuleByDetId(Int_t detId);
	CbmMuchSector* GetSectorByDetId(Int_t detId, Int_t channelId);
	CbmMuchPad* GetPadByDetId(Int_t detId, Int_t channelId);

	Int_t GetNStations() {return fStations->GetEntries();}
	Int_t GetNAbsorbers() {return fNabs;}
	TObjArray* GetStations() {return fStations;}
	TObjArray* GetAbsorbers() {return fAbsorbers;}
	TGeoCone* GetMuchCave() {return fMuchCave;}
	Double_t GetMuchCaveZ0() {return fMuchZ1+fMuchCave->GetDz();}
	Double_t GetAbsorberZ0(Int_t i) {return fMuchZ1+fAbsorberZ1[i]+((TGeoCone*) fAbsorbers->At(i))->GetDz();}
	Char_t GetAbsorberMat(Int_t i) {return fAbsorberMat[i];}

	Double_t GetActiveLx() {return fActiveLx;}
	Double_t GetActiveLy() {return fActiveLy;}
	Double_t GetActiveLz() {return fActiveLz;}
	Double_t GetSpacerLx() {return fSpacerLx;}
	Double_t GetSpacerLy() {return fSpacerLy;}
	Double_t GetLayersDz(Int_t st) {return fLayersDz[st];}

	void Init(TObjArray* stations);
	void CreatePointArrays(char* pointClassName);
	void CreateHitArrays(char* hitClassName);
	void CreateClusterArrays(char* clusterClassName);
	void ClearPointArrays();
	void ClearHitArrays();
	void ClearClusterArrays();
	void InitGrid();
	void ResetPads();
	vector<CbmMuchPad*> GetPads() {return fPads;}
	vector<CbmMuchLayerSide*> GetLayerSides(Int_t iStation);
	Int_t GetLayerSideNr(Int_t detId);

	void ReadGeoFile(const char* geoName);
	void Print();
	void CreateMuchCave();
	Bool_t IsModuleDesign() {return fModuleDesign;}

private:
	CbmMuchGeoScheme();
	void CreateAbsorbers();
	void CreateStations();
	Int_t Intersect(Float_t x, Float_t y, Float_t dx, Float_t dy, Float_t r);
	static CbmMuchGeoScheme* fInstance;
	static Bool_t fInitialized; // Defines whether the instance was initialized
	static Bool_t fGridInitialized; // Defines whether grid of the instance was initialized
	vector<CbmMuchPad*> fPads; //!
	map<Int_t,Int_t> fMapSides;

	TObjArray* fStations; //!
	TObjArray* fAbsorbers;//!
	TGeoCone* fMuchCave; //!

	// Input parameters
	Double_t fMuchZ1; // MuchCave Zin position [cm]
	Double_t fAcceptanceTanMin; // Acceptance tangent min
	Double_t fAcceptanceTanMax; // Acceptance tangent max
	Int_t fNabs; // Number of absorbers
	Int_t fNst; // Number of stations
	Bool_t fModuleDesign; /* 1 - detailed design (modules at two sides)
	 * 0 - simple design (1 module per layer)     */

	Double_t fActiveLx; // Active volume lx [cm]
	Double_t fActiveLy; // Active volume ly [cm]
	Double_t fActiveLz; // Active volume lz [cm]
	Double_t fSpacerLx; // Spacer lx [cm]
	Double_t fSpacerLy; // Spacer ly [cm]
	Double_t fOverlapY; // Overlap along y axis [cm]

	TArrayD fAbsorberZ1; // Absorber Zin position [cm]
	TArrayD fAbsorberLz; // Absorber thickness [cm]
	TArrayC fAbsorberMat; // Absorber material
	TArrayD fStationZ0; // Station Zceneter [cm]
	TArrayI fNlayers; // Number of layers
	TArrayI fDetType; // Detector type
	TArrayD fLayersDz; // Distance between layers [cm]
	TArrayD fSupportLz; // Support thickness [cm]
	TArrayD fSigmaXmin; // Sigma X minimum [cm]
	TArrayD fSigmaYmin; // Sigma Y maximum [cm]
	TArrayD fSigmaXmax; // Sigma X minimum [cm]
	TArrayD fSigmaYmax; // Sigma Y maximum [cm]
	TArrayD fOccupancyMax; // Maximum occupancy


	ClassDef(CbmMuchGeoScheme,1);
};

#endif
