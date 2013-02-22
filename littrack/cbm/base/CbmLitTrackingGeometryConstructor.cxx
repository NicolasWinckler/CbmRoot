/**
 * \file CbmLitTrackingGeometryConstructor.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 **/

#include "base/CbmLitTrackingGeometryConstructor.h"

#include "base/CbmLitFieldGridCreator.h"
#include "utils/CbmLitComparators.h"
#include "utils/CbmLitUtils.h"

#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "CbmStsStation.h"
#include "CbmGeoStsPar.h"
#include "CbmStsDigiScheme.h"
#include "CbmMuchGeoScheme.h"
#include "CbmGeoMuchPar.h"
#include "CbmMuchStation.h"
#include "CbmMuchLayer.h"
#include "CbmMuchModule.h"

#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TGeoShape.h"
#include "TGeoMedium.h"
#include "TGeoMatrix.h"
#include "TGeoBBox.h"
#include "TGeoCone.h"
#include "TGeoSphere.h"
#include "TGeoArb8.h"
#include "TGeoPgon.h"

#include "TRandom.h"

#include <set>
#include <iostream>
#include <sstream>
#include <algorithm>

using std::set;
using lit::ToString;

CbmLitTrackingGeometryConstructor::CbmLitTrackingGeometryConstructor():
   fGeo(NULL),
   fNofTrdStations(-1),
   fNofMuchStations(-1),
   fNofMvdStations(-1)
{
	fGeo = gGeoManager;
}

CbmLitTrackingGeometryConstructor::~CbmLitTrackingGeometryConstructor()
{
}

CbmLitTrackingGeometryConstructor* CbmLitTrackingGeometryConstructor::Instance()
{
   static CbmLitTrackingGeometryConstructor instance;
   return &instance;
}

void CbmLitTrackingGeometryConstructor::GetMuchLayoutVec(
   lit::parallel::LitDetectorLayoutMuonVec& layout)
{
   GetMuchLayout(layout);
}

void CbmLitTrackingGeometryConstructor::GetMuchLayoutScal(
   lit::parallel::LitDetectorLayoutMuonScal& layout)
{
   GetMuchLayout(layout);
}

template<class T> void CbmLitTrackingGeometryConstructor::GetMuchLayout(
   lit::parallel::LitDetectorLayoutMuon<T>& layout)
{
//   std::cout << "Getting detector layout for parallel MUCH tracking..." << std::endl;
//
//   CbmLitFieldGridCreator gridCreator;
//
//   const TGeoNode* much = static_cast<const TGeoNode*>(fGeo->GetTopNode()->GetNodes()->FindObject("much_0"));
//   const double* muchPos  = much->GetMatrix()->GetTranslation();
//   TObjArray* muchNodes = much->GetNodes();
//   for (Int_t iMuchNode = 0; iMuchNode < muchNodes->GetEntriesFast(); iMuchNode++) {
//      const TGeoNode* muchNode = static_cast<const TGeoNode*>(muchNodes->At(iMuchNode));
//
//      // If MUCH node is an absorber
//      if (TString(muchNode->GetName()).Contains("absorber")) {
//         const TGeoCone* absorberShape = static_cast<const TGeoCone*>(muchNode->GetVolume()->GetShape());
//         const TGeoMaterial* absorberMaterial = muchNode->GetVolume()->GetMedium()->GetMaterial();
//
//         Double_t zPos = muchPos[2] + muchNode->GetMatrix()->GetTranslation()[2]; // Z center position
//         Double_t thickness = 2. * absorberShape->GetDz();
//
//         lit::parallel::LitAbsorber<T> absorber;
//         absorber.SetZ(zPos);
//
//         lit::parallel::LitMaterialInfo<T> material;
//         material.Thickness = thickness;
//         material.A = absorberMaterial->GetA();
//         material.Z = absorberMaterial->GetZ();
//         material.Rho = absorberMaterial->GetDensity();
//         material.X0 = absorberMaterial->GetRadLen();
//         material.I = (absorberMaterial->GetZ() > 16.) ? 10. * absorberMaterial->GetZ() * 1e-9 : 16. * std::pow(absorberMaterial->GetZ(), 0.9) * 1e-9;
//         absorber.SetMaterial(material);
//
//         Double_t gridZ[3] = { zPos - 0.5 * thickness, zPos, zPos + 0.5 * thickness};
//         lit::parallel::LitFieldGrid frontGrid, middleGrid, backGrid;
//         gridCreator.CreateGrid(gridZ[0], frontGrid);
//         gridCreator.CreateGrid(gridZ[1], middleGrid);
//         gridCreator.CreateGrid(gridZ[2], backGrid);
//         absorber.SetFieldGridFront(frontGrid);
//         absorber.SetFieldGridMiddle(middleGrid);
//         absorber.SetFieldGridBack(backGrid);
//      }
//
//      // If MUCH node is a station
//      if (TString(muchNode->GetName()).Contains("station")) {
//    	 // Create station group
//    	 lit::parallel::LitStationGroupMuon<T> sg;
//         TObjArray* layerNodes = muchNode->GetNodes();
//         // Calculate minimum and maximum Z of the substation which is needed for field approximation.
//         Double_t minZ = std::numeric_limits<Double_t>::max(), maxZ = std::numeric_limits<Double_t>::min();
//         for (Int_t iLayerNode = 0; iLayerNode < layerNodes->GetEntriesFast(); iLayerNode++) {
//        	// Create station
//        	lit::parallel::LitStationMuon<T> st;
//        	st.SetType(lit::parallel::kLITPIXELHIT);
//            const TGeoNode* sideNode = static_cast<const TGeoNode*>(layerNodes->At(iLayerNode));
//            Bool_t bactive = false, factive = false;
//            TObjArray* sideNodes = sideNode->GetNodes();
//            for (Int_t iSideNode = 0; iSideNode < sideNodes->GetEntriesFast(); iSideNode++) {
//               const TGeoNode* active = static_cast<const TGeoNode*>(sideNode->GetNodes()->At(iSideNode));
//               if (TString(active->GetName()).Contains("active")) {
//                  if (bactive && TString(active->GetName()).Contains("bactive")) { continue; }
//                  if (factive && TString(active->GetName()).Contains("factive")) { continue; }
//
//                  const TGeoBBox* activeShape = static_cast<const TGeoBBox*>(active->GetVolume()->GetShape());
//                  const TGeoMaterial* activeMaterial = active->GetVolume()->GetMedium()->GetMaterial();
//
//                  Double_t zPos = muchPos[2] + muchNode->GetMatrix()->GetTranslation()[2]
//                          + sideNode->GetMatrix()->GetTranslation()[2] + active->GetMatrix()->GetTranslation()[2] + activeShape->GetDZ();
//
//                  minZ = std::min(minZ, zPos);
//                  maxZ = std::max(maxZ, zPos);
//
//                  lit::parallel::LitSubstationMuon<T> ss;
//                  ss.SetZ(zPos);
//
//                  lit::parallel::LitMaterialInfo<T> material;
//                  material.Thickness = 2. * activeShape->GetDZ();
//                  material.A = activeMaterial->GetA();
//                  material.Z = activeMaterial->GetZ();
//                  material.Rho = activeMaterial->GetDensity();
//                  material.X0 = activeMaterial->GetRadLen();
//                  material.I = (activeMaterial->GetZ() > 16.) ? 10. * activeMaterial->GetZ() * 1e-9 : 16. * std::pow(activeMaterial->GetZ(), 0.9) * 1e-9;
//                  ss.SetMaterial(material);
//
//                  st.AddSubstation(ss);
//
//                  bactive = TString(active->GetName()).Contains("bactive");
//                  factive = TString(active->GetName()).Contains("factive");
//                  if (factive && bactive) { break; }
//               }
//            }
//            sg.AddStation(st);
//         }
//         // Magnetic field approximation for the station group
//         lit::parallel::LitFieldGrid frontGrid, middleGrid, backGrid;
//         gridCreator.CreateGrid(minZ, frontGrid);
//         gridCreator.CreateGrid((minZ + 0.5 * (maxZ - minZ)), middleGrid);
//         gridCreator.CreateGrid(maxZ, backGrid);
//         sg.SetFieldGridFront(frontGrid);
//         sg.SetFieldGridMiddle(middleGrid);
//         sg.SetFieldGridBack(backGrid);
//         layout.AddStationGroup(sg);
//      }
//   }
//
//   std::cout << layout;
}

void CbmLitTrackingGeometryConstructor::GetTrdLayoutVec(
   lit::parallel::LitDetectorLayoutElectronVec& layout)
{
   GetTrdLayout(layout);
}

void CbmLitTrackingGeometryConstructor::GetTrdLayoutScal(
   lit::parallel::LitDetectorLayoutElectronScal& layout)
{
   GetTrdLayout(layout);
}

template<class T>
void CbmLitTrackingGeometryConstructor::GetTrdLayout(
   lit::parallel::LitDetectorLayoutElectron<T>& layout)
{
//   cout << "Getting TRD layout for parallel version of tracking..." << endl;
//
//   CbmLitFieldGridCreator gridCreator;
//   CbmLitSimpleGeometryConstructor* geoConstructor = CbmLitSimpleGeometryConstructor::Instance();
//   const std::vector<CbmLitMaterialInfo>& trdMaterial = geoConstructor->GetMyTrdGeoNodes();
//   const std::vector<CbmLitMaterialInfo>& richMaterial = geoConstructor->GetMyRichGeoNodes();
//
//   TrdLayout();
//   const CbmLitDetectorLayout& trdLayout = GetTrdLayout();
//   cout << trdLayout.ToString();
//
//   // Add virtual planes
//   int richMatCnt = 0;
//   for (int nvp = 0; nvp < 33; nvp++) {
//      lit::parallel::LitVirtualPlaneElectron<T> virtualPlane;
//      float DZ = 10.;
//      float Z = 100. + nvp * DZ;
//
//      lit::parallel::LitFieldGrid fieldGrid, fieldGridMid;
//      gridCreator.CreateGrid(Z, fieldGrid);
//      gridCreator.CreateGrid(Z + DZ/2., fieldGridMid);
//
//      CbmLitMaterialInfo mat;
//      lit::parallel::LitMaterialInfo<T> m;
//      if (nvp >= 10 && nvp <= 15) { // Add RICH material
//         mat = richMaterial[richMatCnt++];
//         m.Thickness = mat.GetLength();
//      } else { // Add air
//         mat = trdMaterial[5]; //air material
//         m.Thickness = DZ;//mat.GetLength();
//      }
//      m.A = mat.GetA();
//      m.Z = mat.GetZ();
//      m.I = (mat.GetZ() > 16)? 10 * mat.GetZ() * 1e-9 :
//            16 * std::pow(mat.GetZ(), 0.9) * 1e-9;
//      m.Rho = mat.GetRho();
//      m.X0 = mat.GetRL();
//      m.Zpos = Z + DZ;//mat.GetZpos();
//      m.CalculateValues();
//
//      virtualPlane.SetZ(Z);
//      virtualPlane.SetFieldGrid(fieldGrid);
//      virtualPlane.SetFieldGridMid(fieldGridMid);
//      virtualPlane.SetMaterial(m);
//
//      layout.AddVirtualPlane(virtualPlane);
//   }
//   // end add virtual planes
//
//   for (int isg = 0; isg < trdLayout.GetNofStationGroups(); isg++) {
//      const CbmLitStationGroup& stationGroup = trdLayout.GetStationGroup(isg);
//      lit::parallel::LitStationGroupElectron<T> sg;
//
//      for (int ist = 0; ist < stationGroup.GetNofStations(); ist++) {
//         const CbmLitStation& station = stationGroup.GetStation(ist);
//
//         for(int iss = 0; iss < station.GetNofSubstations(); iss++) {
//            const CbmLitSubstation& substation = station.GetSubstation(iss);
//            lit::parallel::LitStationElectron<T> st;
//            st.SetZ(substation.GetZ());
//
//            int matId = TrdMaterialId(isg, ist, trdLayout);
//            for (int im = 0; im < 6; im++) {
//               CbmLitMaterialInfo mat = trdMaterial[matId + im];
//               lit::parallel::LitMaterialInfo<T> m;
//               m.A = mat.GetA();
//               m.Z = mat.GetZ();
//               m.I = (mat.GetZ() > 16)? 10 * mat.GetZ() * 1e-9 :
//                     16 * std::pow(mat.GetZ(), 0.9) * 1e-9;
//               m.Rho = mat.GetRho();
//               m.Thickness = mat.GetLength();
//               m.X0 = mat.GetRL();
//               m.Zpos = mat.GetZpos();
//               m.CalculateValues();
//
//               if (im < 3) { st.AddMaterialBefore(m); }
//               else { st.AddMaterialAfter(m); }
//            }
//
//            sg.AddStation(st);
//         } // loop over substations
//      } // loop over stations
//      layout.AddStationGroup(sg);
//   } // loop over station groups
//   cout << layout;
//   cout << "Finish getting TRD layout for parallel version of tracking..." << endl;
}


Int_t CbmLitTrackingGeometryConstructor::GetNofTrdStations()
{
   static Bool_t firstTime = true;
   if (firstTime) {
      set<Int_t> planeIds;
      TObjArray* topNodes = fGeo->GetTopNode()->GetNodes();
      Int_t nofTopNodes = topNodes->GetEntriesFast();
      for (Int_t iTopNode = 0; iTopNode < nofTopNodes; iTopNode++) {
         TGeoNode* topNode = static_cast<TGeoNode*>(topNodes->At(iTopNode));
         if (TString(topNode->GetName()).Contains("trd")) {
            TObjArray* modules = topNode->GetNodes();
            for (Int_t iModule = 0; iModule < modules->GetEntriesFast(); iModule++) {
               TGeoNode* module = static_cast<TGeoNode*>(modules->At(iModule));
               TString moduleName = module->GetName();
               Int_t stationId = 0;
               Int_t layerId = 0;
               if (moduleName.Contains(TRegexp("trd[1-3]mod[0-9]_[0-9][0-9][0-9][0-9]$"))) { // trd_v10b and trd_v11c
                  stationId = std::atoi(string(1, moduleName[3]).c_str()); // 3rd element is station number
                  layerId = std::atoi(string(1, moduleName[9]).c_str()); // 9th element is layer number
               } else if (moduleName.Contains(TRegexp("trd1mod[0-9]_[0-9][0-9][0-9][0-9][0-9]"))) { // trd_v12x and trd_v13x
                  stationId = std::atoi(string(1, moduleName[9]).c_str()); // 9th element is station number
                  layerId = std::atoi(string(1, moduleName[10]).c_str()); // 10th element is layer number
               }
               Int_t planeId = 10 * stationId + layerId;
               planeIds.insert(planeId);
            }
         }
      }
      fNofTrdStations = planeIds.size();
      firstTime = false;
   }
   return fNofTrdStations;
}

Int_t CbmLitTrackingGeometryConstructor::GetNofMuchStations()
{
   static Bool_t firstTime = true;
   if (firstTime) {
      fNofMuchStations = 0;
      const TGeoNode* much = static_cast<const TGeoNode*>(fGeo->GetTopNode()->GetNodes()->FindObject("much_0"));
      if (NULL == much) { // No MUCH detector return 0 stations
         firstTime = false;
         return fNofMuchStations;
      }
      TObjArray* muchNodes = much->GetNodes();
      for (Int_t iMuchNode = 0; iMuchNode < muchNodes->GetEntriesFast(); iMuchNode++) {
         const TGeoNode* muchNode = static_cast<const TGeoNode*>(muchNodes->At(iMuchNode));
         if (TString(muchNode->GetName()).Contains("station")) {
            TObjArray* layerNodes = muchNode->GetNodes();
            fNofMuchStations += layerNodes->GetEntriesFast();
         }
      }
      firstTime = false;
   }
   return fNofMuchStations;
}

Int_t CbmLitTrackingGeometryConstructor::GetNofMuchTrdStations()
{
   return GetNofMuchStations() + GetNofTrdStations();
}

Int_t CbmLitTrackingGeometryConstructor::GetNofMvdStations()
{
   static Bool_t firstTime = true;
   if (firstTime) {
      fNofMvdStations = 0;
      TObjArray* nodes = gGeoManager->GetTopVolume()->FindNode("pipevac1_0")->GetNodes();
      for (Int_t iNode = 0; iNode < nodes->GetEntriesFast(); iNode++) {
         TGeoNode* node = (TGeoNode*) nodes->At(iNode);
         if (TString(node->GetName()).Contains("mvdstation")) {
            fNofMvdStations++;
         }
      }
      firstTime = false;
   }
   return fNofMvdStations;
}
