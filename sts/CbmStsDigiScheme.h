//* $Id: */

// -------------------------------------------------------------------------
// -----                   CbmStsDigiScheme header file                -----
// -----                  Created 31/08/06  by V. Friese               -----
// -------------------------------------------------------------------------


/** CbmStsDigiScheme
 **@author Volker Friese <v.friese@gsi.de>
 **@since 31.08.06
 **@version 1.0
 **
 ** Auxiliary class for STS digitisation and hit finding.
 ** Controls the digitisation scheme of the stations and sectors.
 **/


#ifndef CBMSTSDIGISCHEME_H
#define CBMSTSDIGISCHEME_H 1

#include "TObjArray.h"
#include "TObject.h"

#include <map>

class CbmGeoStsPar;
class CbmStsDigiPar;
class CbmStsSector;
class CbmStsStation;



class CbmStsDigiScheme : public TObject
{


 public:

  /** Constructor **/
  CbmStsDigiScheme();


  /** Destructor **/
  virtual ~CbmStsDigiScheme();


  /** Initialisation **/
  Bool_t Init(CbmGeoStsPar* geoPar, CbmStsDigiPar* digiPar);


  /** Clear digitisation scheme **/
  void Clear();


  /** Reset eventwise counters **/
  void Reset();


  /** Accessors **/
  Int_t GetNStations() { return fStations->GetEntries(); }
  Int_t GetNSectors()  { return fNSectors; }
  Int_t GetNChannels() { return fNChannels; }
  TObjArray* GetStations() { return fStations; }
  CbmStsStation* GetStation(Int_t iStation);       /** Station by index **/
  CbmStsStation* GetStationByNr(Int_t stationNr);  /** Station by number **/
  CbmStsStation* GetStationByMcId(Int_t iVol);     /** Station by MCid **/
  CbmStsSector* GetSector(Int_t stationNr, Int_t sectorNr);
  CbmStsSector* GetSectorByMcId(Int_t iVol);     /** Sector by MCid **/


  /** Screen output  **/
  void Print(Bool_t kLong = kFALSE);
  

 private:

  TObjArray* fStations;       /** Array of CbmStsStation **/
  TObjArray* fSectors;        /** Array of CbmStsSector **/
  Int_t fNSectors;            /** Total number of sectors **/
  Int_t fNChannels;           /** Total number of channels **/

  std::map<Int_t, CbmStsStation*> fStationMap; //! Map from number to station
  std::map<Int_t, CbmStsStation*> fVolumeMap;  //! Map from MCid to station

  std::map<Int_t, CbmStsSector*> fSectorMap;  //! Map from MCid to sector


  ClassDef(CbmStsDigiScheme,1);

};



#endif

