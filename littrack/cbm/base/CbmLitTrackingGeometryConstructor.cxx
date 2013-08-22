/**
 * \file CbmLitTrackingGeometryConstructor.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 **/

#include "base/CbmLitTrackingGeometryConstructor.h"
#include "base/CbmLitFieldGridCreator.h"
#include "std/utils/CbmLitUtils.h"

#include "CbmHistManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

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
#include "TProfile2D.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TRegexp.h"

#include <set>
#include <map>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <limits>

using std::set;
using std::cout;
using std::map;

CbmLitTrackingGeometryConstructor::CbmLitTrackingGeometryConstructor():
   fGeo(NULL),
   fNofTrdStations(-1),
   fNofMuchStations(-1),
   fNofMvdStations(-1),
   fNofStsStations(-1),
   fDet()
{
	fGeo = gGeoManager;
	fDet.DetermineSetup();
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
   lit::parallel::LitDetectorLayoutVec& layout)
{
   GetMuchLayout(layout);
}

void CbmLitTrackingGeometryConstructor::GetMuchLayoutScal(
   lit::parallel::LitDetectorLayoutScal& layout)
{
   GetMuchLayout(layout);
}

template<class T> void CbmLitTrackingGeometryConstructor::GetMuchLayout(
   lit::parallel::LitDetectorLayout<T>& layout)
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
   lit::parallel::LitDetectorLayoutVec& layout)
{
   GetTrdLayout(layout);
}

void CbmLitTrackingGeometryConstructor::GetTrdLayoutScal(
   lit::parallel::LitDetectorLayoutScal& layout)
{
   GetTrdLayout(layout);
}

template<class T>
void CbmLitTrackingGeometryConstructor::GetTrdLayout(
   lit::parallel::LitDetectorLayout<T>& layout)
{
   cout << "Getting TRD layout for parallel version of tracking...\n";

   // Read file with TProfile2D containing silicon equivalent of the material
   TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");
   TString matBudgetFile = parDir + "/littrack/trd_v13g.30M.silicon.root";
   TFile* oldFile = gFile;
   TDirectory* oldDirectory = gDirectory;
   TFile* file = new TFile(matBudgetFile, "READ");
   CbmHistManager hm;
   hm.ReadFromFile(file);

   CbmLitFieldGridCreator gridCreator;

   Double_t startZPosition = 100.;// Last STS station
//   Double_t detectorZPosition = 400.; // We want to extrapolate up to here using virtual stations
   Double_t dZ = 10.; // Distance between neighboring virtual stations

   lit::parallel::LitMaterialGrid richMaterial;
   GetRichMaterial(&richMaterial);

   // Virtual stations
   Int_t nofVirtualStations = 31;
   for (Int_t iStation = 0; iStation < nofVirtualStations; iStation++) {
      Double_t z = startZPosition + iStation * dZ;
      lit::parallel::LitFieldGrid fieldGrid;
      gridCreator.CreateGrid(z, fieldGrid);

      lit::parallel::LitStation<T> station;
      station.SetZ(z);
      station.SetField(fieldGrid);
      if (iStation == 10) station.SetMaterial(richMaterial);

      layout.AddVirtualStation(station);
   }

   // Stations
   Int_t nofStations = GetNofTrdStations();
   for (Int_t iStation = 0; iStation < nofStations; iStation++) {
      // Convert material for this station
      TProfile2D* profile = hm.P2("hrl_ThicknessSilicon_Trd_" + lit::ToString<Int_t>(iStation) + "_P2");
      //profile->Rebin2D(200, 200);
      lit::parallel::LitMaterialGrid material;
      ConvertTProfile2DToLitMaterialGrid(profile, &material);

      lit::parallel::LitStation<T> station;
      station.SetMaterial(material);

      layout.AddStation(station);
   }

   gFile = oldFile;
   gDirectory = oldDirectory;
   file->Close();
   delete file;

   cout << layout;
   cout << "Finish getting TRD layout for parallel version of tracking\n";
}

void CbmLitTrackingGeometryConstructor::GetRichMaterial(
      lit::parallel::LitMaterialGrid* material)
{
   if (!fDet.GetDet(kRICH)) return;
   // Read file with TProfile2D containing silicon equivalent of the material
   TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");
   TString matBudgetFile = parDir + "/littrack/rich_v08a.silicon.root";
   TFile* oldFile = gFile;
   TDirectory* oldDirectory = gDirectory;
   TFile* file = new TFile(matBudgetFile, "READ");
   CbmHistManager hm;
   hm.ReadFromFile(file);

   TProfile2D* profile = hm.P2("hrl_ThicknessSilicon_Rich_P2");
  // profile->Rebin2D(200, 200);
   ConvertTProfile2DToLitMaterialGrid(profile, material, 3.);

   gFile = oldFile;
   gDirectory = oldDirectory;
   file->Close();
   delete file;
}

void CbmLitTrackingGeometryConstructor::ConvertTProfile2DToLitMaterialGrid(
      const TProfile2D* profile,
      lit::parallel::LitMaterialGrid* grid,
      Double_t maximumValue)
{
//   Int_t nofBinsX = profile->GetNbinsX();
//   Int_t nofBinsY = profile->GetNbinsY();
//   vector<vector<fscal> >material(nofBinsX);
//   for (Int_t i = 0; i < nofBinsX; i++) material[i].resize(nofBinsY);
//   for (Int_t iX = 1; iX <= nofBinsX; iX++) {
//      for (Int_t iY = 1; iY <= nofBinsY; iY++) {
//         Double_t content = profile->GetBinContent(iX, iY);
//         if (maximumValue > 0 && content > maximumValue) content = maximumValue;
//         material[iX - 1][iY - 1] = content;
//      }
//   }
//   Double_t xmin = profile->GetXaxis()->GetXmin();
//   Double_t xmax = profile->GetXaxis()->GetXmax();
//   Double_t ymin = profile->GetYaxis()->GetXmin();
//   Double_t ymax = profile->GetYaxis()->GetXmax();
//   grid->SetMaterial(material, xmin, xmax, ymin, ymax, nofBinsX, nofBinsY);

   Int_t nofBinsX = profile->GetNbinsX();
   Int_t nofBinsY = profile->GetNbinsY();
   Int_t minShrinkBinX = std::numeric_limits<Double_t>::max();
   Int_t maxShrinkBinX = std::numeric_limits<Double_t>::min();
   Int_t minShrinkBinY = std::numeric_limits<Double_t>::max();
   Int_t maxShrinkBinY = std::numeric_limits<Double_t>::min();
   Bool_t isSet = false;
   for (Int_t iBinX = 1; iBinX <= nofBinsX; iBinX++) {
      for (Int_t iBinY = 1; iBinY <= nofBinsY; iBinY++) {
         Double_t content = profile->GetBinContent(iBinX, iBinY);
         if (content != 0.) {
            minShrinkBinX = std::min(iBinX, minShrinkBinX);
            maxShrinkBinX = std::max(iBinX, maxShrinkBinX);
            minShrinkBinY = std::min(iBinY, minShrinkBinY);
            maxShrinkBinY = std::max(iBinY, maxShrinkBinY);
            isSet = true;
         }
      }
   }

   Int_t nofShrinkBinsX = maxShrinkBinX - minShrinkBinX + 1;
   Int_t nofShrinkBinsY = maxShrinkBinY - minShrinkBinY + 1;
   vector<vector<fscal> >material(nofShrinkBinsX);
   for (Int_t i = 0; i < nofShrinkBinsX; i++) material[i].resize(nofShrinkBinsY);

   for (Int_t iX = minShrinkBinX; iX <= maxShrinkBinX; iX++) {
      for (Int_t iY = minShrinkBinY; iY <= maxShrinkBinY; iY++) {
         Double_t content = profile->GetBinContent(iX, iY);
         if (maximumValue > 0 && content > maximumValue) content = maximumValue;
         material[iX - minShrinkBinX][iY - minShrinkBinY] = content;
      }
   }
   Double_t xmin = profile->GetXaxis()->GetBinLowEdge(minShrinkBinX);
   Double_t xmax = profile->GetXaxis()->GetBinUpEdge(maxShrinkBinX);
   Double_t ymin = profile->GetYaxis()->GetBinLowEdge(minShrinkBinY);
   Double_t ymax = profile->GetYaxis()->GetBinUpEdge(maxShrinkBinY);
   grid->SetMaterial(material, xmin, xmax, ymin, ymax, nofShrinkBinsX, nofShrinkBinsY);
}

Int_t CbmLitTrackingGeometryConstructor::GetNofTrdStations()
{
   static Bool_t firstTime = true;
   if (firstTime) {
      Int_t layerCounter = 0;
      TObjArray* topNodes = fGeo->GetTopNode()->GetNodes();
      for (Int_t iTopNode = 0; iTopNode < topNodes->GetEntriesFast(); iTopNode++) {
         TGeoNode* topNode = static_cast<TGeoNode*>(topNodes->At(iTopNode));
         if (TString(topNode->GetName()).Contains("trd")) {
            TObjArray* layers = topNode->GetNodes();
            for (Int_t iLayer = 0; iLayer < layers->GetEntriesFast(); iLayer++) {
               TGeoNode* layer = static_cast<TGeoNode*>(layers->At(iLayer));
               if (TString(layer->GetName()).Contains("layer")) {
                  layerCounter++;
               }
            }
         }
      }
      fNofTrdStations = layerCounter;
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
      TGeoNode* topNode = gGeoManager->GetTopVolume()->FindNode("pipevac1_0");
      if (topNode) {
         TObjArray* nodes = topNode->GetNodes();
         for (Int_t iNode = 0; iNode < nodes->GetEntriesFast(); iNode++) {
            TGeoNode* node = (TGeoNode*) nodes->At(iNode);
            if (TString(node->GetName()).Contains("mvdstation")) {
               fNofMvdStations++;
            }
         }
      }
      firstTime = false;
   }
   return fNofMvdStations;
}

Int_t CbmLitTrackingGeometryConstructor::GetNofStsStations()
{
   static Bool_t firstTime = true;
   if (firstTime) {
      fNofStsStations = 0;
      TObjArray* nodes = gGeoManager->GetTopVolume()->GetNodes();
      for (Int_t iNode = 0; iNode < nodes->GetEntriesFast(); iNode++) {
         TGeoNode* node = (TGeoNode*) nodes->At(iNode);
         if (TString(node->GetName()).Contains("STS_")) { // Top STS node
            fNofStsStations = node->GetNodes()->GetEntriesFast();
            break;
         }
      }
      firstTime = false;
   }
   return fNofStsStations;
}

Int_t CbmLitTrackingGeometryConstructor::ConvertMuchToAbsoluteStationNr(
      Int_t station,
      Int_t layer)
{
   static Bool_t firstTime = true;
   static vector<Int_t> sumOfLayers;
   if (firstTime) {
      fNofMuchStations = 0;
      const TGeoNode* much = static_cast<const TGeoNode*>(fGeo->GetTopNode()->GetNodes()->FindObject("much_0"));
      if (NULL == much) { // No MUCH detector return 0
         firstTime = false;
         return 0;
      }
      map<Int_t, Int_t> nofLayersPerStation;
      TObjArray* muchNodes = much->GetNodes();
      for (Int_t iMuchNode = 0; iMuchNode < muchNodes->GetEntriesFast(); iMuchNode++) {
         const TGeoNode* muchNode = static_cast<const TGeoNode*>(muchNodes->At(iMuchNode));
         if (TString(muchNode->GetName()).Contains("station")) {
            Int_t station = std::atoi(string(muchNode->GetName() + 11, 2).c_str()) - 1;
            TObjArray* layerNodes = muchNode->GetNodes();
            nofLayersPerStation[station] = layerNodes->GetEntriesFast();
         }
      }
      map<Int_t, Int_t>::const_iterator it;
      Int_t sum = 0;
      sumOfLayers.push_back(0);
      for (it = nofLayersPerStation.begin(); it != nofLayersPerStation.end(); it++) {
         sum += (*it).second;
         sumOfLayers.push_back(sum);
      }
      firstTime = false;
   }
   return sumOfLayers[station] + layer;
}
