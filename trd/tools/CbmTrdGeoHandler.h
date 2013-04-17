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

enum TrdGeometryVersions {kRootGeomWithLayers,
                          kRootGeom,
                          kOldMonolithic,
                          kNewMonolithic,
                          kQuasiMonolithic,
                          kSegmentedRectangular,
                          kSegmentedSquared,
                          kSegmentedSquaredOneKeepingVolume };

#include "CbmTrdDetectorId.h"

#include "TObject.h"

#include <map>

class FairLogger;
class TGeoBBox;
class TGeoVolume;
class TGeoHMatrix;

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

  Int_t GetUniqueDetectorId();
  Int_t GetUniqueDetectorId(TString volName);
  
  std::map<Int_t, Int_t> GetStationMap() {return fStationMap;}
  std::map<Int_t, Int_t> GetModuleMap() {return fModuleTypeMap;}
  Int_t GetGeoVersion() {return fGeoVersion;}  
          
  Int_t Init(Bool_t isSimulation=kFALSE);

  Int_t GetStation(Int_t uniqueId);
  Int_t GetLayer(Int_t uniqueId);
  Int_t GetModuleType(Int_t uniqueId);
  Int_t GetModuleCopyNr(Int_t uniqueId);
  Int_t GetPlane(Int_t uniqueId);
  Int_t GetModuleId(Int_t uniqueId);
  Int_t GetSector(Int_t uniqueId);

  Float_t GetSizeX(TString volName);        
  Float_t GetSizeY(TString volName);        
  Float_t GetSizeZ(TString volName);        
  Float_t GetX(TString volName);        
  Float_t GetY(TString volName);        
  Float_t GetZ(TString volName);        

  Int_t SetSector(Int_t moduleId, Int_t sectorId);
 private:

  Bool_t GetLayerInfoFromOldGeometry(std::vector<Int_t> &layersBeforStation);
  Bool_t GetLayerInfoFromStationKVolumeGeometry(std::vector<Int_t> &layersBeforStation);
  Bool_t GetLayerInfoFromSingleKVolumeGeometry(std::vector<Int_t> &layersBeforStation);
  Bool_t GetLayerInfoFromRootGeometry(std::vector<Int_t> &layersBeforStation);
  Bool_t GetLayerInfoFromRootGeometryWithLayers(std::vector<Int_t> &layersBeforStation);
  void FillDetectorInfoArray(Int_t uniqueId);
  void NavigateTo(TString volName);

  // Implement Interface functions to the TGeoManager to be 
  // the same as for the VMC
  Int_t CurrentVolOffID(Int_t off, Int_t& copy) const;
  Int_t CurrentVolID(Int_t& copy) const;
  Int_t VolId(const Text_t* name) const;
  Int_t VolIdGeo(const char *name) const;

  Int_t CheckGeometryVersion(); 
  void FillInternalStructures();

  Int_t            fGeoVersion; //!

  std::map<Int_t, Int_t> fStationMap;     //!
  std::map<Int_t, Int_t> fModuleTypeMap;  //!
  std::vector<Int_t> fLayersBeforeStation; //!

  FairLogger* fLogger;  //!                                                  
  Bool_t fIsSimulation; //!

  UInt_t fGeoPathHash;        //!
  TGeoVolume* fCurrentVolume; //! 
  TGeoBBox* fVolumeShape;     //!
  Double_t fGlobal[3];        //! Global centre of volume
  TGeoHMatrix* fGlobalMatrix; //!

 CbmTrdGeoHandler(const CbmTrdGeoHandler&);
 CbmTrdGeoHandler operator=(const CbmTrdGeoHandler&);

 ClassDef(CbmTrdGeoHandler,3) 

};


#endif //CBMTRDGEOHANDLER_H

