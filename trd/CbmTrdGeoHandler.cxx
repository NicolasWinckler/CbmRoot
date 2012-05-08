// -------------------------------------------------------------------------
// -----                 CbmTrdGeoHandler source file                  -----
// -----                 Created 13/08/10  by F. Uhlig                 -----
// -------------------------------------------------------------------------


#include "CbmTrdGeoHandler.h"
#include "CbmDetectorList.h"

#include "FairLogger.h"

#include "TGeoVolume.h"
#include "TGeoNode.h"
#include "TGeoManager.h"
#include "TVirtualMC.h"

#include <iostream>
#include <vector>
#include <map>
using std::map;
using std::pair;
using std::cout;
using std::endl;

CbmTrdGeoHandler::CbmTrdGeoHandler() 
  : TObject(),
    fTrdId(),
    fGeoVersion(-1),
    fStationId(),
    fModuleId(),
    fLogger(FairLogger::GetLogger())
{
}

Int_t CbmTrdGeoHandler::Init()
{
  Int_t geoVersion = CheckGeometryVersion();
  FillInternalStructures();
  return geoVersion;
}

Int_t CbmTrdGeoHandler::CheckGeometryVersion() 
{
  // Check which geometry version is used. In the moment there are 4
  // possible geometry versions. The details of the different geometries
  // are descibed in detail elswhere. TODO:
  // Old monolithic geometry:
  //     Big layers of material that cover the complete detector. The 
  //     dtector is squared with a squared hole in the middle for the
  //     beam pipe. Each
  //     detector station has a subvolume for each layer of this station.
  // New monolithic version:
  //     Same overall geometry than the old monolithic geometry, but with
  //     a differnt internal structure. 
  // Rectangular segmented geometry:
  //     The detector is build from rectangular TRD modules. 
  //     Each of these modules has an active area surrounded by a frame. 
  //     Each detector station has a subvolume for each layer of this station.
  // Squared segmented geometry
  //     The detector is build from squared TRD modules. 
  //     Each of these modules has an active area surrounded by a frame. 
  //     Each module is directly put in the station mother volume. There are
  //     now subvolumes for the different layers.
  //     TODO: Check if ther was never a squared geometry with layer
  //           subvolumes.
  // Quasi monolithic geometry:
  //     The detector is build from rectangular TRD modules. To mimic the
  //     monolithic detector layout to have a active area for the whole
  //     detector surface the single modules does not have any frames. The
  //     complete detector module is the active area.

  TGeoVolume *fm=NULL;

  // Only the old monolithic geometry version has a volume trd11
  fm = (TGeoVolume *)gGeoManager->GetListOfVolumes()->FindObject("trd11");
  if (fm) {
    fLogger->Error(MESSAGE_ORIGIN,"Old implementation of simple TRD geometry ('PGON') found");
    fLogger->Error(MESSAGE_ORIGIN,"This version does not work with newer ROOT versions and is obsolete.");
    fLogger->Error(MESSAGE_ORIGIN,"If you see this version you're using a rather old version of CbmRoot. Please update to a new version.");
    fLogger->Error(MESSAGE_ORIGIN,"Stop execution at this point.");
    fLogger->Fatal(MESSAGE_ORIGIN,"See error message above.");
    fGeoVersion = kOldMonolithic; 
    return fGeoVersion;
  }  

  // Only the new monolithic geometry has a volume trd1gas
  fm = (TGeoVolume *)gGeoManager->GetListOfVolumes()->FindObject("trd1gas");
  if (fm) {
    fLogger->Debug(MESSAGE_ORIGIN,"Found new monolithic TRD geometry.");
    fGeoVersion = kNewMonolithic; 
    return fGeoVersion;
  }

  // all modular (segmented) geometries have the volume trd1mod1gas
  fm = (TGeoVolume *)gGeoManager->GetListOfVolumes()->FindObject("trd1mod1gas");
  if (fm) {
    // the rectangular geometries have the volume trd1layer
    fm = (TGeoVolume *)gGeoManager->GetListOfVolumes()->FindObject("trd1layer");
    if (fm) {
      // Only the normal rectangular geometry has frames     
      fm = (TGeoVolume *)gGeoManager->GetListOfVolumes()->FindObject("trd1mod1carbon1");
      if (fm){
	fLogger->Debug(MESSAGE_ORIGIN,"Found rectangular segmented TRD geometry.");
	fGeoVersion = kSegmentedRectangular; 
	return fGeoVersion;
      } else {
	fLogger->Debug(MESSAGE_ORIGIN,"Found quasi monolithic TRD geometry.");
	fGeoVersion = kQuasiMonolithic; 
	return fGeoVersion;
      }
    } else {
      fLogger->Debug(MESSAGE_ORIGIN,"Found squared segmented TRD geometry.");
      fGeoVersion = kSegmentedSquared; 
      return fGeoVersion;
    }
  }
  fLogger->Fatal(MESSAGE_ORIGIN,"Found an unknown TRD geometry.");
  fGeoVersion = -1; 
  return fGeoVersion;  
}

Int_t CbmTrdGeoHandler::GetUniqueDetectorId(Int_t geoVersion, 
					    std::vector<Int_t>& stationId,
					    std::vector< std::vector<Int_t> >&
					    moduleId)
{
  Int_t temp_station;
  Int_t temp_layer;
  Int_t temp_mod;

  Int_t modnumber;
  Int_t modtype;
  Int_t layer;	
  Int_t station;

  if (geoVersion != kNewMonolithic) {
    
    Int_t id2; 

    Int_t id1 = gMC->CurrentVolOffID(1, temp_mod);
        
    if (kSegmentedSquared == geoVersion) {
      id2 = gMC->CurrentVolOffID(2, temp_station);
      layer=temp_mod/1000;
      modnumber=temp_mod%1000;
    } else {
      gMC->CurrentVolOffID(2, layer);
      id2 = gMC->CurrentVolOffID(3, temp_station);
      modnumber=temp_mod;
    }

    station = fStationMap.find(id2)->second;
   
    modtype = fModuleTypeMap.find(id1)->second;

  } else {            
    
    // Get the VolumeId of the volume we are in
    Int_t volumeID = gMC->CurrentVolID(temp_mod);
    
    // Get the station number from map with help of the
    // VolumeID
    station = fStationMap.find(volumeID)->second;
   
    // get the copy number of the volume one level upward in the
    // geometrical tree. This is the layer number.
    gMC->CurrentVolOffID(1, layer);
    
    // There are no modules in the monolithic geometry
    modtype=0;
    modnumber=0;
  }
  
    Int_t sector=0;
    Int_t detInfo_array[6]={kTRD, station,layer,modtype,modnumber,sector};         
    return fTrdId.SetDetectorInfo(detInfo_array);
}

Int_t CbmTrdGeoHandler::GetUniqueDetectorId()
{
  return GetUniqueDetectorId(fGeoVersion, fStationId, fModuleId);
}


Bool_t CbmTrdGeoHandler::GetLayerInfo(std::vector<Int_t> &layersBeforeStation)
{

  // Check if old geometry version with kepping volumes for each layer
  // or new geometry version with modules put directly in the mother
  // volume for a TRD station 
  TGeoVolume *fm=NULL;
  fm = (TGeoVolume *)gGeoManager->GetListOfVolumes()->FindObject("trd1layer");
  if (fm) {
    cout<<"Found old TRD geometry version."<<endl;
    //    return kTRUE;     
    return GetLayerInfoFromOldGeometry(layersBeforeStation);
  } else {
    cout<<"Found new TRD geometry version."<<endl;
    //    return kTRUE;     
    return GetLayerInfoFromNewGeometry(layersBeforeStation);
  }
}

Bool_t CbmTrdGeoHandler::GetLayerInfoFromOldGeometry(std::vector<Int_t> &layersBeforeStation)
{
    // Extract information about the number of TRD stations and
    // the number of layers per TRD station from the geomanager.
    // Store the information about the number of layers at the entrance
    // of subsequent stations in a vector. 
    // This allows to calculate the layer number starting with 1 for the
    // first layer of the first station at a later stage by only adding 
    // the layer number in the station to the number of layers in 
    // previous stations 
    TGeoVolume *fm=NULL;
    Int_t stationNr = 1;
    Int_t totalNrOfLayers = 0;
    layersBeforeStation.push_back(totalNrOfLayers);
    char volumeName[10];
    sprintf(volumeName, "trd%d", stationNr);
    fm = (TGeoVolume *)gGeoManager->GetListOfVolumes()->FindObject(volumeName);  
    if (fm){
      Int_t nrOfLayers = fm->GetNdaughters();
      totalNrOfLayers += nrOfLayers;
      layersBeforeStation.push_back(totalNrOfLayers);
      do {
	stationNr++;
        sprintf(volumeName, "trd%d", stationNr);
	fm = (TGeoVolume *)gGeoManager->GetListOfVolumes()->FindObject(volumeName);  
        if (fm) {
          nrOfLayers = fm->GetNdaughters();
	  totalNrOfLayers += nrOfLayers;
	  layersBeforeStation.push_back(totalNrOfLayers);
	}
      } while (fm);
    } else {
      cout << "***************************************" <<endl;
      cout << "                                       " <<endl;
      cout << " - FATAL ERROR Unknown geometry version" <<endl;
      cout << "   in CbmTrdHitProducerSmearing        " <<endl;
      cout << " No TRD stations found in the geometry " <<endl;
      cout << "                                       " <<endl;
      cout << "***************************************" <<endl;
      return kFALSE;
    }

    return kTRUE;
}

Bool_t CbmTrdGeoHandler::GetLayerInfoFromNewGeometry(std::vector<Int_t> &layersBeforeStation)
{

    TGeoVolume *fm=NULL;
    TGeoNode *node = NULL;
    

    Int_t stationNr = 1;
    Int_t totalNrOfLayers = 0;
    Int_t totalNrOfStations = 0;
    layersBeforeStation.push_back(totalNrOfLayers);
    char volumeName[15];


    // Now extract first the number of stations
    // If a geometry without keeping volumes for the TRD station is
    // found the execution of the program is stopped
    sprintf(volumeName, "trd%d", stationNr);
    fm = (TGeoVolume *)gGeoManager->GetListOfVolumes()->FindObject(volumeName);
    if (fm) {
      do {
	sprintf(volumeName, "trd%d", stationNr);
	fm = (TGeoVolume *)gGeoManager->GetListOfVolumes()->FindObject(volumeName);  
	totalNrOfStations++;
        stationNr++;
      } while (fm);
      totalNrOfStations--;
    } else {
      cout << "***************************************" <<endl;
      cout << "                                       " <<endl;
      cout << " - FATAL ERROR Unknown geometry version" <<endl;
      cout << "   in CbmTrdHitProducerSmearing        " <<endl;
      cout << " No TRD stations found in the geometry " <<endl;
      cout << "                                       " <<endl;
      cout << "***************************************" <<endl;
      return kFALSE;
    }


    Int_t layersPerStation[totalNrOfStations];
    Int_t maxModuleType = 3;
    Int_t maxLayer = 6 ;


    // Now loop over the stations and extract the number of layers
    // for each station. Get a pointer to the station volume. With
    // this get all nodes which belong to this volume. Loop for all
    // possible differnt module types over all theoratically possible 
    // layers. The highest found layer number is the number of layers
    // for a station.     
    


    for ( Int_t iStation=1; iStation<=totalNrOfStations; iStation++) {

      sprintf(volumeName, "trd%d", iStation);
      fm = (TGeoVolume *)gGeoManager->GetListOfVolumes()->FindObject(volumeName);  

      // Check if a volume trdXmodY#1001 exists (first copy of trd 
      // module type Y in trad station X). Start with the highest
      // possible layer number. The layer of the first found module
      // is the number of layers in this station

      Bool_t foundModule=kFALSE;
      

      for ( Int_t iLayer=maxLayer; iLayer>=1; iLayer--) {
	for ( Int_t iModuleType=1; iModuleType<=maxModuleType; iModuleType++) {

	  sprintf(volumeName, "trd%dmod%d_%d001", iStation, iModuleType, iLayer);
          node = (TGeoNode *) fm->GetNodes()->FindObject(volumeName);
          if (node) {
	    foundModule=kTRUE;
	    break;
          }
	}

        if (foundModule){
          totalNrOfLayers += iLayer;
	  layersBeforeStation.push_back(totalNrOfLayers);
	  layersPerStation[iStation-1]=iLayer;
	  break;
	}
      }

    }

    
    for ( Int_t iStation=1; iStation<=totalNrOfStations; iStation++) {

      cout<<"TRD Station "<< iStation <<" has "<< layersPerStation[iStation-1] 
          <<" layers."<<endl;
    }

    return kTRUE;

}

Bool_t CbmTrdGeoHandler::GetMCId(const char* volumeName, 
				 std::vector<Int_t> &Id) 
{

  // Use information from the Virtual Monte Carlo, which is used
  // in the simulation. This should make this function independent 
  // from the actaul MC engine.
  Int_t fMCid = gMC->VolId(volumeName);

  if ( 0 != fMCid) {
    Id.push_back(fMCid);
    return kTRUE;
  }

  return kFALSE;
}


void CbmTrdGeoHandler::FillInternalStructures()
{
  // Extract geometry information from Virtual MC.
  // All geometry handling should be done now in the
  // separate utility class CbmTrdGeoHandler

  Int_t stationNr = 1;
  char volumeName[10];
  Bool_t result;
  
  if (fGeoVersion == kNewMonolithic) {
    
    fStationId.clear();
    do {
      sprintf(volumeName, "trd%dgas", stationNr);
      result = GetMCId(volumeName, fStationId);
      stationNr++;
    }
    while (result);
    
  } else {

    Int_t MCid;
    do {
      sprintf(volumeName, "trd%d", stationNr);
      MCid = gMC->VolId(volumeName);
      fLogger->Info(MESSAGE_ORIGIN,"MCID: %i",MCid);
      if ( 0 != MCid) {
	fStationMap.insert(pair<Int_t,Int_t>(MCid,stationNr));
      }
      stationNr++;
    }
    while ( 0 != MCid); 

    stationNr=1;
    fStationId.clear();
    do {
      sprintf(volumeName, "trd%d", stationNr);
      result = GetMCId(volumeName, fStationId);
      stationNr++;
    }
    while (result); 
    Int_t maxStationNr = --stationNr; 
    cout<<"Max Station: "<<maxStationNr<<endl;
    
    Int_t layerNr = 1;
    
    fModuleId.clear();
    std::vector<Int_t> temp;
    Int_t maxModuleTypes = 3;
    for (Int_t iStation = 1; iStation < maxStationNr; iStation++) {
      temp.clear();
      for (Int_t iModule = 1; iModule <= maxModuleTypes; iModule++) {
	sprintf(volumeName, "trd%dmod%d", iStation, iModule);
	Int_t fMCid = gMC->VolId(volumeName);
	temp.push_back(fMCid);      
      }
      fModuleId.push_back(temp);
    }

    for (Int_t iStation = 1; iStation < maxStationNr; iStation++) {
      for (Int_t iModule = 1; iModule <= maxModuleTypes; iModule++) {
	sprintf(volumeName, "trd%dmod%d", iStation, iModule);
	Int_t fMCid = gMC->VolId(volumeName);
        if ( 0 != fMCid ) { 
	  fModuleTypeMap.insert(pair<Int_t,Int_t>(fMCid,iModule));
	}
      }
    }


  }



  
}




ClassImp(CbmTrdGeoHandler)
