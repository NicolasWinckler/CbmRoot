/**
 * \file CbmTrdDetectorId.h
 * \author F. Uhlig <f.uhlig@gsi.de>
 * \date 25/05/09
 * \brief Helper class to convert detector ID back and forth.
 *
 * Redesigned 27/02/2013 by Andrey Lebedev
 *
 *                                 3         2         1          shift length
 * Current definition:            10987654321098765432109876543210
 * System ID (kTRD=5) on bits 0-4 00000000000000000000000000011111      31
 * Station NR on bits 5-8         00000000000000000000000111100000 <<5  15
 * Layer NR on bits 9-11          00000000000000000000111000000000 <<9  7
 * Module Type on bits 12-16      00000000000000011111000000000000 <<12 31
 * Copy NR on bits 17-24          00000001111111100000000000000000 <<17 255
 * Sector Nr on bits 25-31        11111110000000000000000000000000 <<25 127
 *
 **/

#ifndef CBMTRDDETECTORID_H
#define CBMTRDDETECTORID_H 1

#include "CbmDetectorList.h"

class CbmTrdDetectorId
{
public:

   /**
   * \brief Return Detector ID from system ID, station number, layer number, mudule type, copy number and sector number.
   * \param[in] stationNr Station number.
   * \param[in] layerNr Layer number.
   * \param[in] moduleType Module type.
   * \param[in] copyNr Copy number.
   * \param[in] sectorNr Sector number.
   * \return Detector ID from system ID, station number, layer number, mudule type, copy number and sector number.
   **/
   static Int_t GetDetectorId(Int_t stationNr,
                              Int_t layerNr,
                              Int_t moduleType,
                              Int_t copyNr,
                              Int_t sectorNr) {
      return (kTRD << SB_SYSTEM) | (stationNr << SB_STATION_NR) | (layerNr << SB_LAYER_NR) |
            (moduleType << SB_MODULE_TYPE) | (copyNr << SB_COPY_NR) | (sectorNr << SB_SECTOR_NR);
    }

   /**
   * \brief Return System identifier from detector ID.
   * \param[in] detectorId Detector ID.
   * \return System identifier from detector ID.
   **/
   static Int_t GetSystemId(Int_t detectorId){
      return (detectorId & (WL_SYSTEM << SB_SYSTEM)) >> SB_SYSTEM;
   }

   /**
   * \brief Return station number from detector ID.
   * \param[in] detectorId Detector ID.
   * \return Station number from detector ID.
   **/
   static Int_t GetStationNr(Int_t detectorId) {
      return (detectorId & (WL_STATION_NR << SB_STATION_NR)) >> SB_STATION_NR;
   }

   /**
   * \brief Return layer number from detector ID.
   * \param[in] detectorId Detector ID.
   * \return Layer number from detector ID.
   **/
   static Int_t GetLayerNr(Int_t detectorId) {
      return (detectorId & (WL_LAYER_NR << SB_LAYER_NR)) >> SB_LAYER_NR;
   }
   
   /**
   * \brief Return module type from detector ID.
   * \param[in] detectorId Detector ID.
   * \return module type from detector ID.
   **/
   static Int_t GetModuleType(Int_t detectorId) {
      return (detectorId & (WL_MODULE_TYPE << SB_MODULE_TYPE)) >> SB_MODULE_TYPE;
   }

   /**
   * \brief Return copy number from detector ID.
   * \param[in] detectorId Detector ID.
   * \return Copy number from detector ID.
   **/
   static Int_t GetCopyNr(Int_t detectorId) {
      return (detectorId & (WL_COPY_NR << SB_COPY_NR)) >> SB_COPY_NR;
   }

   /**
   * \brief Return sector number from detector ID.
   * \param[in] detectorId Detector ID.
   * \return Sector number from detector ID.
   **/
   static Int_t GetSectorNr(Int_t detectorId) {
      return (detectorId & (WL_SECTOR_NR << SB_SECTOR_NR)) >> SB_SECTOR_NR;
   }

   /**
   * \brief Return module ID from detector ID.
   * \param[in] detectorId Detector ID.
   * \return Module ID from detector ID.
   **/
   static Int_t GetModuleId(Int_t detectorId) {
      return (detectorId & (WL_MODULE_ID << SB_MODULE_ID)) >> SB_MODULE_ID;
   }

private:

   // Length of the index of the corresponding volume
   static const Int_t WL_SYSTEM = 31;
   static const Int_t WL_STATION_NR = 15;
   static const Int_t WL_LAYER_NR = 7;
   static const Int_t WL_MODULE_TYPE = 31;
   static const Int_t WL_COPY_NR = 255;
   static const Int_t WL_SECTOR_NR = 127;
   // Number of a start bit for each volume
   static const Int_t SB_SYSTEM = 0;
   static const Int_t SB_STATION_NR = 5;
   static const Int_t SB_LAYER_NR = 9;
   static const Int_t SB_MODULE_TYPE = 12;
   static const Int_t SB_COPY_NR = 17;
   static const Int_t SB_SECTOR_NR = 25;
   // Start bit and length for module ID.
   // For module ID use all bits up to sector number.
   static const Int_t SB_MODULE_ID = 0;
   static const Int_t WL_MODULE_ID = 33554431; // 2^25 - 1
};

#endif
