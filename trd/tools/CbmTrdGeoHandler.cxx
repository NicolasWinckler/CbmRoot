/**
 * \file CbmTrdGeoHandler.cxx
 * \author Florian Uhlig <f.uhlig@gsi.de>
 */

#include "CbmTrdGeoHandler.h"
#include "CbmTrdAddress.h"

#include "FairLogger.h"

#include "TGeoVolume.h"
#include "TGeoBBox.h"
#include "TGeoNode.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"

#include <string>
#include <cstdlib>
#include <iostream>
using std::cout;
using std::endl;
using std::string;
using std::atoi;

CbmTrdGeoHandler::CbmTrdGeoHandler() 
  : TObject(),
    fIsSimulation(kFALSE),
    fGeoPathHash(0),
    fCurrentVolume(NULL),
    fVolumeShape(NULL),    
    fGlobal(),    
    fGlobalMatrix(NULL),
    fModuleType(0)
{
}

CbmTrdGeoHandler::~CbmTrdGeoHandler()
{

}

void CbmTrdGeoHandler::Init(Bool_t isSimulation)
{
   fIsSimulation = isSimulation;
}

Int_t CbmTrdGeoHandler::GetModuleAddress()
{
   // We take the mother node (module) of the current node we are in (gas).
   TGeoNode* node = gGeoManager->GetMother();
   // Get the module copy number to get the information about layerId and moduleId.
   Int_t copyNr = node->GetNumber();
   // In TGeoManager numbering starts with 1, so we have to subtract 1.
   Int_t layerId = ((copyNr / 100) % 100) - 1;
   Int_t moduleId = (copyNr % 100) - 1;
   // Form the module address.
   return CbmTrdAddress::GetAddress(layerId, moduleId, 0, 0, 0);
}

Int_t CbmTrdGeoHandler::GetModuleAddress(
      const TString& path)
{
  if (fGeoPathHash != path.Hash()) {
    NavigateTo(path);
  }
  return GetModuleAddress();
}

//Int_t CbmTrdGeoHandler::GetPadOrientation()
//{
//   // We take the mother node (module) of the current node we are in (gas).
//   TGeoNode* node = gGeoManager->GetMother(2);   // get layer
//   //   TGeoNode* node = gGeoManager->GetMother();   // get module
//   // Get the module copy number to get the information about layerId and moduleId.
//   Int_t copyNr = node->GetNumber();
//   //   std::cout << copyNr << std::endl;
//   // isRotated is the 4th digit from the back
//   Int_t isRotated = ((copyNr / 1000) % 10);  // from layer copy number
//   // isRotated is the 5th digit from the back
//   //   Int_t isRotated = ((copyNr / 10000) % 10);   // from module copy number
//   return isRotated;
//}

Int_t CbmTrdGeoHandler::GetPadOrientation(
      const TString& path)
{
  if (fGeoPathHash != path.Hash()) {
    NavigateTo(path);
  }
  return fIsRotated;
}

Double_t CbmTrdGeoHandler::GetSizeX(
      const TString& path)
{
   if (fGeoPathHash != path.Hash()) {
      NavigateTo(path);
   }
   return fVolumeShape->GetDX();
}

Double_t CbmTrdGeoHandler::GetSizeY(
      const TString& path)
{
   if (fGeoPathHash != path.Hash()) {
      NavigateTo(path);
   }
   return fVolumeShape->GetDY();
}

Double_t CbmTrdGeoHandler::GetSizeZ(
      const TString& path)
{
   if (fGeoPathHash != path.Hash()) {
      NavigateTo(path);
   }
   return fVolumeShape->GetDZ();
}

Double_t CbmTrdGeoHandler::GetZ(
      const TString& path)
{
   if (fGeoPathHash != path.Hash()) {
      NavigateTo(path);
   }
   return fGlobal[2];
}

Double_t CbmTrdGeoHandler::GetY(
      const TString& path)
{
   if (fGeoPathHash != path.Hash()) {
      NavigateTo(path);
   }
   return fGlobal[1];
}

Double_t CbmTrdGeoHandler::GetX(
      const TString& path)
{
   if (fGeoPathHash != path.Hash()) {
      NavigateTo(path);
   }
   return fGlobal[0];
}

Int_t CbmTrdGeoHandler::GetModuleType(
      const TString& path)
{
   if (fGeoPathHash != path.Hash()) {
      NavigateTo(path);
   }
   return fModuleType;
}

Int_t CbmTrdGeoHandler::GetStation(
      const TString& path)
{
   if (fGeoPathHash != path.Hash()) {
      NavigateTo(path);
   }
   return fStation;
}

Int_t CbmTrdGeoHandler::GetLayer(
      const TString& path)
{
   if (fGeoPathHash != path.Hash()) {
      NavigateTo(path);
   }
   return fLayer;
}

Int_t CbmTrdGeoHandler::GetModuleCopyNr(
      const TString& path)
{
   if (fGeoPathHash != path.Hash()) {
      NavigateTo(path);
   }
   return fModuleCopy;
}

void CbmTrdGeoHandler::NavigateTo(
      const TString& path)
{
   if (fIsSimulation) {
      LOG(FATAL) << "This method is not supported in simulation mode" << FairLogger::endl;
   } else {
      gGeoManager->cd(path.Data());
      fGeoPathHash = path.Hash();
      fCurrentVolume = gGeoManager->GetCurrentVolume();
      fVolumeShape = (TGeoBBox*)fCurrentVolume->GetShape();
      Double_t local[3] = {0., 0., 0.};  // Local center of volume
      gGeoManager->LocalToMaster(local, fGlobal);
      fGlobalMatrix = gGeoManager->GetCurrentMatrix();
      // Get module type information which is decoded in copy number.
      const char* moduleName = gGeoManager->GetMother()->GetName();
      fModuleType = std::atoi(string(1, *(moduleName + 6)).c_str()); // 6th element module type

      // We take the mother of the mother node (layer) of the current node we are in (gas).
      TGeoNode* layernode = gGeoManager->GetMother(2);   // get layer
      Int_t layercopyNr = layernode->GetNumber();
      // fIsRotated is the 4th digit from the back
      fStation   = ((layercopyNr / 10000) % 10);  // from layer copy number
      fIsRotated = ((layercopyNr /  1000) % 10);  // from layer copy number
      fLayer     = ((layercopyNr /   100) % 10);  // from layer copy number

      // We take the mother node (module) of the current node we are in (gas).
      TGeoNode* modulenode = gGeoManager->GetMother();
      // Get the module copy number to get the information about layerId and moduleId.
      Int_t modulecopyNr = modulenode->GetNumber();
      // In TGeoManager numbering starts with 1, so we have to subtract 1.
      fModuleCopy= ((modulecopyNr / 100000) % 100);
      //      fLayerId   = ((modulecopyNr /    100) % 100) - 1;
      //      fModuleId  = ((modulecopyNr /      1) % 100) - 1;
   }
}
 
ClassImp(CbmTrdGeoHandler)
