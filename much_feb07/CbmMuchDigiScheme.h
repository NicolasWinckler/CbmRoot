/** CbmMuchDigiScheme.h
 **@author  Mikhail Ryzhinskiy <m.ryzhinskiy@gsi.de>
 **@since   15.03.07
 **@version 1.0
 **
 ** Auxiliary class for MuCh digitization and hit finding.
 ** Controls the digitisation scheme of the stations and sectors.
 **/


#ifndef CBMMUCHDIGISCHEME_H
#define CBMMUCHDIGISCHEME_H 1


#include "TObjArray.h"
#include "TClonesArray.h"
#include "TObject.h"

#include <map>

class CbmGeoMuchPar;
class CbmMuchDigiPar;
class CbmMuchSector;
class CbmMuchPad;
class CbmMuchDigi;
class CbmMuchStation;

class CbmMuchDigiScheme : public TObject
{
 protected:
  static CbmMuchDigiScheme* fInstance; // Instance of this singleton object
  static Int_t              fRefcount; // Counter of references on this object
  static Bool_t             fInitialized; // Defines whether the instance was initialized
  static Bool_t             fGridInitialized; // Defines whether grid of the instance was initialized
  /** Constructor **/
  CbmMuchDigiScheme();
  /** Destructor **/
  virtual ~CbmMuchDigiScheme();

 public:

  /** Initialisation **/
  Bool_t Init(CbmGeoMuchPar* geoPar, CbmMuchDigiPar* digiPar);

  /** Clear digitisation scheme **/
  void Clear();

  /** Reset eventwise counters **/
  void Reset();

  /** Initializes a grid for fast search for sectors within a station **/
  void InitGrid();

  /** Screen output  **/
  void Print();

  /** Returns instance of this singleton class. **/
  static CbmMuchDigiScheme* Instance();

  /** Frees memory from this singleton class. **/
  void FreeInstance();

  /** Accessors **/
  Int_t GetNStations() { return fStations->GetEntries(); }    // Number of stations
  Int_t GetNSectors()  { return fNSectors; }                  // Total number of sectors 
  Int_t GetNChannels() { return fNChannels; }                 // Total number of channels 
  TObjArray* GetStations() { return fStations; }              // Array of CbmMuchStation objects 
  CbmMuchStation* GetStation(Int_t iStation);                 // Station by index 
  CbmMuchStation* GetStationByNr(Int_t stationNr);            // Station by number 
  CbmMuchStation* GetStationByMcId(Int_t iVol);               // Station by MCid 
  CbmMuchSector*  GetSector(Int_t stationNr, Int_t sectorNr); // Sector by sector number and station number 
  CbmMuchPad*     GetPad(CbmMuchDigi* digi);                  // Accessor to CbmMuchPad by CbmMuchDigi
  
 private:
  TObjArray* fStations;       // Array of CbmMuchStation
  Int_t fNSectors;            // Total number of sectors 
  Int_t fNChannels;           // Total number of channels 
  std::map<Int_t, CbmMuchStation*> fStationMap; // Map from number to station
  std::map<Int_t, CbmMuchStation*> fVolumeMap;  // Map from MCid to station

  ClassDef(CbmMuchDigiScheme,1);
};
#endif

