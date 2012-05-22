// -------------------------------------------------------------------------
// -----                 CbmTrdGeoHandler source file                  -----
// -----                 Created 13/08/10  by F. Uhlig                 -----
// -------------------------------------------------------------------------


#include "CbmTrdGeoHandler.h"
#include "CbmDetectorList.h"

#include "FairLogger.h"

#include "TGeoVolume.h"
#include "TGeoBBox.h"
#include "TGeoNode.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"
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
    fStationMap(),
    fModuleTypeMap(),
    fLayersBeforeStation(),
    fLogger(FairLogger::GetLogger()),
    fIsSimulation(kFALSE),
    fLastUsedDetectorID(0),
    fDetectorInfoArray(),
    fGeoPathHash(0)
{
}

Int_t CbmTrdGeoHandler::Init(Bool_t isSimulation)
{
  Int_t geoVersion = CheckGeometryVersion();

  fIsSimulation=isSimulation;

  FillInternalStructures();

  GetLayerInfo(fLayersBeforeStation);
  
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
      fm = (TGeoVolume *)gGeoManager->GetListOfVolumes()->FindObject("trd2");
      if (fm) {
        // Only the normal old squared geometry has a separate keeping volume
        // for the second station    
	fLogger->Debug(MESSAGE_ORIGIN,"Found squared segmented TRD geometry.");
        fGeoVersion = kSegmentedSquared; 
        return fGeoVersion;
      } else {
        // The new squared geometry has only one keeping volume
        // for all layers    
        fLogger->Debug(MESSAGE_ORIGIN,"Found squared segmented TRD geometry with only one keeping volume.");
        fGeoVersion = kSegmentedSquaredOneKeepingVolume; 
        return fGeoVersion;
      }
    }
  }
  fLogger->Fatal(MESSAGE_ORIGIN,"Found an unknown TRD geometry.");
  fGeoVersion = -1; 
  return fGeoVersion;  
}

Int_t CbmTrdGeoHandler::GetUniqueDetectorId(TString volName)
{
  if (fGeoPathHash != volName.Hash()) {
    NavigateTo(volName);
  }
  return GetUniqueDetectorId();
}


Int_t CbmTrdGeoHandler::GetUniqueDetectorId()
{
  Int_t temp_station;
  Int_t temp_layer;
  Int_t temp_mod;

  Int_t modnumber;
  Int_t modtype;
  Int_t layer;	
  Int_t station;

  if (fGeoVersion != kNewMonolithic) {
    
    Int_t id2; 

    Int_t id1 = CurrentVolOffID(1, temp_mod);
        
    if (kSegmentedSquared == fGeoVersion) {
      id2 = CurrentVolOffID(2, temp_station);
      layer=temp_mod/1000;
      modnumber=temp_mod%1000;
      station = fStationMap.find(id2)->second;
    } else if (kSegmentedSquaredOneKeepingVolume == fGeoVersion) {
      station=temp_mod/10000;
      layer=(temp_mod%10000)/1000;
      modnumber=temp_mod%1000;
    } else {
      CurrentVolOffID(2, layer);
      id2 = CurrentVolOffID(3, temp_station);
      modnumber=temp_mod;
      station = fStationMap.find(id2)->second;
    }
   
    modtype = fModuleTypeMap.find(id1)->second;
  } else {            
    
    // Get the VolumeId of the volume we are in
    Int_t volumeID = CurrentVolID(temp_mod);
    
    // Get the station number from map with help of the
    // VolumeID
    station = fStationMap.find(volumeID)->second;
   
    // get the copy number of the volume one level upward in the
    // geometrical tree. This is the layer number.
    CurrentVolOffID(1, layer);
    
    // There are no modules in the monolithic geometry
    modtype=0;
    modnumber=0;
  }
  
    Int_t sector=0;
    Int_t detInfo_array[6]={kTRD, station,layer,modtype,modnumber,sector};         
    return fTrdId.SetDetectorInfo(detInfo_array);
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
    return GetLayerInfoFromOldGeometry(layersBeforeStation);
  } else {
    cout<<"Found new TRD geometry version."<<endl;
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

void CbmTrdGeoHandler::FillInternalStructures()
{
  // Extract geometry information from Virtual MC.
  // All geometry handling should be done now in the
  // separate utility class CbmTrdGeoHandler

  Int_t stationNr = 1;
  char volumeName[10];
  Bool_t result;
  Int_t MCid;
  
  fStationMap.clear();
  fModuleTypeMap.clear();

  if (fGeoVersion == kNewMonolithic) {
    
    do {
      sprintf(volumeName, "trd%dgas", stationNr);
      MCid = VolId(volumeName);
      if ( 0 != MCid) {
	fStationMap.insert(pair<Int_t,Int_t>(MCid,stationNr));
      }
      stationNr++;
    } while ( 0 != MCid); 

  } else {

    do {
      sprintf(volumeName, "trd%d", stationNr);
      MCid = VolId(volumeName);
      if ( 0 != MCid) {
	fStationMap.insert(pair<Int_t,Int_t>(MCid,stationNr));
      }
      stationNr++;
    }
    while ( 0 != MCid); 

    Int_t maxStationNr = --stationNr; 

    Int_t maxModuleTypes;
    if (fGeoVersion == kSegmentedSquaredOneKeepingVolume) { 
      maxModuleTypes = 8;
    } else {
      maxModuleTypes = 3;
    }

    for (Int_t iStation = 1; iStation < maxStationNr; iStation++) {
      for (Int_t iModule = 1; iModule <= maxModuleTypes; iModule++) {
	sprintf(volumeName, "trd%dmod%d", iStation, iModule);
	MCid = VolId(volumeName);
        if ( 0 != MCid ) { 
	  fModuleTypeMap.insert(pair<Int_t,Int_t>(MCid,iModule));
	}
      }
    }
  } 
}

Int_t CbmTrdGeoHandler::VolIdGeo(const char *name) const
{
  //
  // Return the unique numeric identifier for volume name
  //

   Int_t uid = gGeoManager->GetUID(name);
   if (uid<0) {
      printf("VolId: Volume %s not found\n",name);
      return 0;
   }
   return uid;
}

Int_t CbmTrdGeoHandler::VolId(const Text_t* name) const
{
  if (fIsSimulation) {
    return gMC->VolId(name);
  } else {
    //
    // Return the unique numeric identifier for volume name
    //
    char sname[20];
    Int_t len = strlen(name)-1;
    if (name[len] != ' ') { return VolIdGeo(name); }
    strncpy(sname, name, len);
    sname[len] = 0;
    return VolIdGeo(sname);
  }
}

Int_t CbmTrdGeoHandler::CurrentVolID(Int_t& copy) const
{
  if (fIsSimulation) {
    return gMC->CurrentVolID(copy);
  } else {
    //
    // Returns the current volume ID and copy number
    //
    if (gGeoManager->IsOutside()) { return 0; }
    TGeoNode* node = gGeoManager->GetCurrentNode();
    copy = node->GetNumber();
    Int_t id = node->GetVolume()->GetNumber();
    return id;
  }
}

//_____________________________________________________________________________
Int_t CbmTrdGeoHandler::CurrentVolOffID(Int_t off, Int_t& copy) const
{
  if (fIsSimulation) {
    return gMC->CurrentVolOffID(off, copy);
  } else {
    //
    // Return the current volume "off" upward in the geometrical tree
    // ID and copy number
    //
    if (off<0 || off>gGeoManager->GetLevel()) { return 0; }
    if (off==0) { return CurrentVolID(copy); }
    TGeoNode* node = gGeoManager->GetMother(off);
    if (!node) { return 0; }
    copy = node->GetNumber();
    return node->GetVolume()->GetNumber();
  }
}

void CbmTrdGeoHandler::FillDetectorInfoArray(Int_t uniqueId) 
{
  fDetectorInfoArray = fTrdId.GetDetectorInfo(uniqueId);
}
Int_t CbmTrdGeoHandler::GetStation(Int_t uniqueId)
{
  if (fLastUsedDetectorID != uniqueId) {
    FillDetectorInfoArray(uniqueId);
  }
  return fDetectorInfoArray[1];
}
Int_t CbmTrdGeoHandler::GetLayer(Int_t uniqueId)
{
  if (fLastUsedDetectorID != uniqueId) {
    FillDetectorInfoArray(uniqueId);
  }
  return fDetectorInfoArray[2];
}
Int_t CbmTrdGeoHandler::GetModuleType(Int_t uniqueId)
{
  if (fLastUsedDetectorID != uniqueId) {
    FillDetectorInfoArray(uniqueId);
  }
  return fDetectorInfoArray[3];
}
Int_t CbmTrdGeoHandler::GetModuleCopyNr(Int_t uniqueId)
{
  if (fLastUsedDetectorID != uniqueId) {
    FillDetectorInfoArray(uniqueId);
  }
  return fDetectorInfoArray[4];
}
Int_t CbmTrdGeoHandler::GetSector(Int_t uniqueId)
{
  if (fLastUsedDetectorID != uniqueId) {
    FillDetectorInfoArray(uniqueId);
  }
  return fDetectorInfoArray[5];
}
Int_t CbmTrdGeoHandler::GetPlane(Int_t uniqueId)
{
  if (fLastUsedDetectorID != uniqueId) {
    FillDetectorInfoArray(uniqueId);
  }
  return fLayersBeforeStation[(fDetectorInfoArray[1]-1)]+fDetectorInfoArray[2];
}
Int_t CbmTrdGeoHandler::GetModuleId(Int_t uniqueId)
{
  return  fTrdId.GetModuleId(uniqueId);
}

Float_t CbmTrdGeoHandler::GetSizeX(TString volName) 
{
  if (fGeoPathHash != volName.Hash()) {
    NavigateTo(volName);
  }
  Float_t sizex = fVolumeShape->GetDX();
  return sizex;
}

Float_t CbmTrdGeoHandler::GetSizeY(TString volName) 
{
  if (fGeoPathHash != volName.Hash()) {
    NavigateTo(volName);
  }
  Float_t sizey = fVolumeShape->GetDY();
  return sizey;
}

Float_t CbmTrdGeoHandler::GetSizeZ(TString volName) 
{
  if (fGeoPathHash != volName.Hash()) {
    NavigateTo(volName);
  }
  Float_t sizez = fVolumeShape->GetDZ();
  return sizez;
}

Float_t CbmTrdGeoHandler::GetZ(TString volName) 
{
  if (fGeoPathHash != volName.Hash()) {
    NavigateTo(volName);
  }
  return fGlobal[2];
}

Float_t CbmTrdGeoHandler::GetY(TString volName) 
{
  if (fGeoPathHash != volName.Hash()) {
    NavigateTo(volName);
  }
  return fGlobal[1];
}

Float_t CbmTrdGeoHandler::GetX(TString volName) 
{
  if (fGeoPathHash != volName.Hash()) {
    NavigateTo(volName);
  }
  return fGlobal[0];
}

void CbmTrdGeoHandler::NavigateTo(TString volName) 
{
  if (fIsSimulation) {
    fLogger->Fatal(MESSAGE_ORIGIN,"This methode is not supported in simulation mode");
  } else {
    gGeoManager->cd(volName.Data());
    fGeoPathHash = volName.Hash();
    fCurrentVolume = gGeoManager->GetCurrentVolume();
    fVolumeShape = (TGeoBBox*)fCurrentVolume->GetShape(); 
    Double_t local[3] = {0., 0., 0.};  // Local centre of volume
    gGeoManager->LocalToMaster(local, fGlobal);
    cout<<"Pos: "<<fGlobal[0]<<" , "<<fGlobal[1]<<" , "<<fGlobal[2]<<" , "<<endl;
    fGlobalMatrix = gGeoManager->GetCurrentMatrix(); 
  }	      
}
 
 ClassImp(CbmTrdGeoHandler)
