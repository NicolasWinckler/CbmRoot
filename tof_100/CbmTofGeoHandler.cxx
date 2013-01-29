// -------------------------------------------------------------------------
// -----                 CbmTrdGeoHandler source file                  -----
// -----                 Created 13/08/10  by F. Uhlig                 -----
// -------------------------------------------------------------------------


#include "CbmTofGeoHandler.h"
#include "CbmDetectorList.h"
#include "CbmTofDetectorId_v07a.h"
#include "CbmTofDetectorId_v12b.h"

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

CbmTofGeoHandler::CbmTofGeoHandler() 
  : TObject(),
    fTofId(NULL),
    fGeoVersion(-1),
    fIsSimulation(kFALSE),
    fLastUsedDetectorID(0),
    fDetectorInfoArray(),
    fGeoPathHash(0),
    fCurrentVolume(NULL),
    fVolumeShape(NULL),    
    fGlobal(),    
    fGlobalMatrix(NULL)
{
}

Int_t CbmTofGeoHandler::Init(Bool_t isSimulation)
{
  Int_t geoVersion = CheckGeometryVersion();

  fIsSimulation=isSimulation;

  return geoVersion;
}

Int_t CbmTofGeoHandler::CheckGeometryVersion() 
{
  // Check which geometry version is used. In the moment there are 3
  // possible geometry versions. 
  // v07a:
  //     Old TOF geometry at 10 m from target 
  // v12a:
  //     Old TOF geometry at 6 m from target. Beside the z position
  //     the geometry is the same as v07a, so it is handled in the same way 
  // v12b:
  //     New version of the tof geometrie developed by Norbert Herrmann
  //     including also a support structure

  TObjArray* nodes = gGeoManager->GetTopNode()->GetNodes();
  for (Int_t iNode = 0; iNode < nodes->GetEntriesFast(); iNode++) {
    TGeoNode* node = (TGeoNode*) nodes->At(iNode);
    if (TString(node->GetName()).Contains("tof")) {
      // Since there is only one tof top node we check for full node name
      // In the old geometries v07a and v12a the name of this node is tof1_0
      // In new geometries the node name is tof_v<year><version> eg. tof_v12b
      // With this naming scheme the geometry version is completely qualified
      if (TString(node->GetName()).EqualTo("tof1_0")) {
	gLogger->Info(MESSAGE_ORIGIN,"Found TOF geometry v07a or v12a.");
        fTofId = new CbmTofDetectorId_v07a();
	fGeoVersion = k07a;        
        return fGeoVersion;
      } else if (TString(node->GetName()).EqualTo("tof_v12b_0")) {
	LOG(INFO)<< "Found TOF geometry v12b." << FairLogger::endl;
        fTofId = new CbmTofDetectorId_v07a();
	//        fTofId = new CbmTofDetectorId_v12b();
	fGeoVersion = k12b;
        return fGeoVersion;
      } else {
	gLogger->Fatal(MESSAGE_ORIGIN,"Found an unknown TOF geometry.");
	fGeoVersion = -1; 
	return fGeoVersion;  
      }
    }
  }
}

Int_t CbmTofGeoHandler::GetUniqueDetectorId(TString volName)
{
  if (fGeoPathHash != volName.Hash()) {
    NavigateTo(volName);
  }
  return GetUniqueDetectorId();
}


Int_t CbmTofGeoHandler::GetUniqueDetectorId()
{

  Int_t smtype=-1; 
  Int_t smodule=-1;
  Int_t counter=-1;
  Int_t gap=-1; 
  Int_t cell=-1;
  Int_t region=-1;
  TString Volname;


  if (fGeoVersion == k07a) {
    Volname = gMC->CurrentVolName();
    smtype = Volname[5]-'0';
    gMC->CurrentVolOffID(2, smodule);
    gMC->CurrentVolOffID(1, cell);
    gMC->CurrentVolID(gap);
  } else if (fGeoVersion == k12b) {
    Volname = gMC->CurrentVolOffName(4);
    smtype = Volname[7]-'0';
    gMC->CurrentVolOffID(2, counter);
    gMC->CurrentVolOffID(1, gap);
    gMC->CurrentVolID(cell);
  }

  LOG(DEBUG2)<<"Volname: "<<Volname<<FairLogger::endl;
  LOG(DEBUG2)<<"SMtype: "<<smtype<<FairLogger::endl;
  LOG(DEBUG2)<<"SModule: "<<smodule<<FairLogger::endl;
  LOG(DEBUG2)<<"Counter: "<<counter<<FairLogger::endl;
  LOG(DEBUG2)<<"Gap: "<<gap<<FairLogger::endl;
  LOG(DEBUG2)<<"Cell: "<<cell<<FairLogger::endl;

  CbmTofDetectorInfo detInfo(kTOF, smtype, smodule, counter, 
			     gap, cell, region);         

  return fTofId->SetDetectorInfo(detInfo);

}

/*
void CbmTofGeoHandler::FillInternalStructures()
{
  // Extract geometry information from Virtual MC.
  // All geometry handling should be done now in the
  // separate utility class CbmTofGeoHandler

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
*/

Int_t CbmTofGeoHandler::VolIdGeo(const char *name) const
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

Int_t CbmTofGeoHandler::VolId(const Text_t* name) const
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

Int_t CbmTofGeoHandler::CurrentVolID(Int_t& copy) const
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
Int_t CbmTofGeoHandler::CurrentVolOffID(Int_t off, Int_t& copy) const
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

void CbmTofGeoHandler::FillDetectorInfoArray(Int_t uniqueId) 
{
  fDetectorInfoArray = fTofId->GetDetectorInfo(uniqueId);
}

Int_t CbmTofGeoHandler::GetDetSystemId(Int_t uniqueId)
{
  if (fLastUsedDetectorID != uniqueId) {
    FillDetectorInfoArray(uniqueId);
  }
  return fDetectorInfoArray.fDetectorSystem;
}

Int_t CbmTofGeoHandler::GetSMType(Int_t uniqueId)
{
  if (fLastUsedDetectorID != uniqueId) {
    FillDetectorInfoArray(uniqueId);
  }
  return fDetectorInfoArray.fSMtype;
}

Int_t CbmTofGeoHandler::GetSModule(Int_t uniqueId)
{
  if (fLastUsedDetectorID != uniqueId) {
    FillDetectorInfoArray(uniqueId);
  }
  return fDetectorInfoArray.fSModule;
}

Int_t CbmTofGeoHandler::GetCounter(Int_t uniqueId)
{
  if (fLastUsedDetectorID != uniqueId) {
    FillDetectorInfoArray(uniqueId);
  }
  return fDetectorInfoArray.fCounter;
}

Int_t CbmTofGeoHandler::GetGap(Int_t uniqueId)
{
  if (fLastUsedDetectorID != uniqueId) {
    FillDetectorInfoArray(uniqueId);
  }
  return fDetectorInfoArray.fGap;
}

Int_t CbmTofGeoHandler::GetCell(Int_t uniqueId)
{
  if (fLastUsedDetectorID != uniqueId) {
    FillDetectorInfoArray(uniqueId);
  }
  return fDetectorInfoArray.fCell;
}

Int_t CbmTofGeoHandler::GetRegion(Int_t uniqueId)
{
  if (fLastUsedDetectorID != uniqueId) {
    FillDetectorInfoArray(uniqueId);
  }
  return fDetectorInfoArray.fRegion;
}
/*
Float_t CbmTofGeoHandler::GetSizeX(TString volName) 
{
  if (fGeoPathHash != volName.Hash()) {
    NavigateTo(volName);
  }
  Float_t sizex = fVolumeShape->GetDX();
  return sizex;
}

Float_t CbmTofGeoHandler::GetSizeY(TString volName) 
{
  if (fGeoPathHash != volName.Hash()) {
    NavigateTo(volName);
  }
  Float_t sizey = fVolumeShape->GetDY();
  return sizey;
}

Float_t CbmTofGeoHandler::GetSizeZ(TString volName) 
{
  if (fGeoPathHash != volName.Hash()) {
    NavigateTo(volName);
  }
  Float_t sizez = fVolumeShape->GetDZ();
  return sizez;
}

Float_t CbmTofGeoHandler::GetZ(TString volName) 
{
  if (fGeoPathHash != volName.Hash()) {
    NavigateTo(volName);
  }
  return fGlobal[2];
}

Float_t CbmTofGeoHandler::GetY(TString volName) 
{
  if (fGeoPathHash != volName.Hash()) {
    NavigateTo(volName);
  }
  return fGlobal[1];
}

Float_t CbmTofGeoHandler::GetX(TString volName) 
{
  if (fGeoPathHash != volName.Hash()) {
    NavigateTo(volName);
  }
  return fGlobal[0];
}

void CbmTofGeoHandler::NavigateTo(TString volName) 
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
*/
 
ClassImp(CbmTofGeoHandler)
