// -------------------------------------------------------------------------
// -----                 CbmTrdGeoHandler header file                  -----
// -----                 Created 13/08/10  by F. Uhlig                 -----
// -------------------------------------------------------------------------


/** CbmTrdGeoHandler.h
 ** Helper class to extract information from the GeoManager which is
 ** needed in many other TRD classes. This helper class should be a
 ** single place to hold all these functions.
 ** @author F. Uhlig <f.uhlig@gsi.de>
 **/

#ifndef CBMTRDGEOHANDLER_H
#define CBMTRDGEOHANDLER_H 1

enum TrdGeometryVersions {kOldMonolithic,
                          kNewMonolithic,
                          kQuasiMonolithic,
                          kSegmentedRectangular,
                          kSegmentedSquared };

#include "CbmTrdDetectorId.h"

#include "TObject.h"

#include <map>

class FairLogger;

class CbmTrdGeoHandler : public TObject {
 public:

  /** Constructor **/
  CbmTrdGeoHandler();


  /** Destructor **/
  ~CbmTrdGeoHandler() {};

  /** Get Layer Information from gGeoManager **/
  Bool_t GetLayerInfo(std::vector<Int_t> &layersBeforStation);

  /** Check which version of the geometry is used. 
   ** In the moment there are four different version which are defined
   ** in the enumarator TrdGeometryVersions.
   **/   
  Int_t CheckGeometryVersion(); 

  Bool_t GetMCId(const char* volumeName, std::vector<Int_t> &Id);

  Int_t GetUniqueDetectorId(Int_t geoVersion, 
                            std::vector<Int_t> &stationId,
			    std::vector< std::vector<Int_t> > &moduleId);

  Int_t GetUniqueDetectorId();
  
  void FillInternalStructures();
    
  std::vector<Int_t> GetStationId() {return fStationId;}
  std::vector< std::vector<Int_t> > GetModuleId() {return fModuleId;}
  std::map<Int_t, Int_t> GetStationMap() {return fStationMap;}
  std::map<Int_t, Int_t> GetModuleMap() {return fModuleTypeMap;}
  Int_t GetGeoVersion() {return fGeoVersion;}  
          
  Int_t Init();
            
 private:

  Bool_t GetLayerInfoFromOldGeometry(std::vector<Int_t> &layersBeforStation);
  Bool_t GetLayerInfoFromNewGeometry(std::vector<Int_t> &layersBeforStation);

  CbmTrdDetectorId fTrdId; //!
  Int_t            fGeoVersion; //!

  std::map<Int_t, Int_t> fStationMap; //!
  std::map<Int_t, Int_t> fModuleTypeMap; //1
  std::vector<Int_t> fStationId;     //! MC IDs of TRD stations
  std::vector< std::vector<Int_t> > fModuleId;//! MC IDs of gas volumes
                                                  //! in all stations
  FairLogger* fLogger; //!                                                  


 ClassDef(CbmTrdGeoHandler,2) 

};


#endif //CBMTRDGEOHANDLER_H

