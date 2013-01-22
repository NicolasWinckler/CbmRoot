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
   fMedium(),
   fDet(),
   fTrdTrackingGeo(NULL),
   fTofTrackingGeo(NULL)
{
	fGeo = gGeoManager;
	CreateMediumList();
}

CbmLitTrackingGeometryConstructor::~CbmLitTrackingGeometryConstructor()
{
}

CbmLitTrackingGeometryConstructor* CbmLitTrackingGeometryConstructor::Instance()
{
   static CbmLitTrackingGeometryConstructor instance;
   return &instance;
}

void CbmLitTrackingGeometryConstructor::Draw()
{
   //fTrdTrackingGeo->SetVisLevel(0);
   //TGeoVolume* trdGeoMaster = fTrdTrackingGeo->GetMasterVolume();
   //trdGeoMaster->Draw("ogl");
   GetTofStation();
   fTofTrackingGeo->SetVisLevel(0);
   TGeoVolume* tofGeoMaster = fTofTrackingGeo->GetMasterVolume();
   tofGeoMaster->Draw("ogl");
}

void CbmLitTrackingGeometryConstructor::CreateMediumList()
{
	TList* mediumList = fGeo->GetListOfMedia();
	Int_t nofMedium = mediumList->GetEntries();
	for (Int_t iMedium = 0; iMedium < nofMedium; iMedium++) {
		TGeoMedium* medium = static_cast<TGeoMedium*>(mediumList->At(iMedium));
		TGeoMaterial* material = medium->GetMaterial();
		TGeoMaterial* newMaterial = new TGeoMaterial(material->GetName(), material->GetA(), material->GetZ(), material->GetDensity(), material->GetRadLen(), material->GetIntLen());
		TGeoMedium* newMedium = new TGeoMedium(medium->GetName(), medium->GetId(), newMaterial);
		fMedium[newMedium->GetName()] = newMedium;
	}
}

void CbmLitTrackingGeometryConstructor::ConstructSts()
{
   std::cout << "-I- Construction of the STS geometry started" << std::endl;
   FairRunAna* run = FairRunAna::Instance();
   FairRuntimeDb* runDB = run->GetRuntimeDb();
   CbmGeoStsPar* stsPar = (CbmGeoStsPar*) (runDB->getContainer("CbmGeoStsPar"));
   CbmStsDigiPar* digiPar = (CbmStsDigiPar*)(runDB->getContainer("CbmStsDigiPar"));
   CbmStsDigiScheme stsDigiScheme;
   stsDigiScheme.Init(stsPar, digiPar);

   int nofStations = stsDigiScheme.GetNStations();
   for (int i = 0; i < nofStations; i++ ) {
      CbmStsStation* station = stsDigiScheme.GetStation(i);
      if (station == NULL) { continue; }
//      CbmLitMaterialInfo litMaterial;
//      litMaterial.SetLength(station->GetD());
//      litMaterial.SetZpos(station->GetZ());
//      GeoMediumToMaterialInfo(medium, litMaterial);
   }
   std::cout << "-I- Construction of the STS geometry finished" << std::endl;
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
   std::cout << "Getting detector layout for parallel MUCH tracking..." << std::endl;

   CbmLitFieldGridCreator gridCreator;

   const TGeoNode* much = static_cast<const TGeoNode*>(fGeo->GetTopNode()->GetNodes()->FindObject("much_0"));
   const double* muchPos  = much->GetMatrix()->GetTranslation();
   TObjArray* muchNodes = much->GetNodes();
   for (Int_t iMuchNode = 0; iMuchNode < muchNodes->GetEntriesFast(); iMuchNode++) {
      const TGeoNode* muchNode = static_cast<const TGeoNode*>(muchNodes->At(iMuchNode));

      // If MUCH node is an absorber
      if (TString(muchNode->GetName()).Contains("absorber")) {
         const TGeoCone* absorberShape = static_cast<const TGeoCone*>(muchNode->GetVolume()->GetShape());
         const TGeoMaterial* absorberMaterial = muchNode->GetVolume()->GetMedium()->GetMaterial();

         Double_t zPos = muchPos[2] + muchNode->GetMatrix()->GetTranslation()[2]; // Z center position
         Double_t thickness = 2. * absorberShape->GetDz();

         lit::parallel::LitAbsorber<T> absorber;
         absorber.SetZ(zPos);

         lit::parallel::LitMaterialInfo<T> material;
         material.Thickness = thickness;
         material.A = absorberMaterial->GetA();
         material.Z = absorberMaterial->GetZ();
         material.Rho = absorberMaterial->GetDensity();
         material.X0 = absorberMaterial->GetRadLen();
         material.I = (absorberMaterial->GetZ() > 16.) ? 10. * absorberMaterial->GetZ() * 1e-9 : 16. * std::pow(absorberMaterial->GetZ(), 0.9) * 1e-9;
         absorber.SetMaterial(material);

         Double_t gridZ[3] = { zPos - 0.5 * thickness, zPos, zPos + 0.5 * thickness};
         lit::parallel::LitFieldGrid frontGrid, middleGrid, backGrid;
         gridCreator.CreateGrid(gridZ[0], frontGrid);
         gridCreator.CreateGrid(gridZ[1], middleGrid);
         gridCreator.CreateGrid(gridZ[2], backGrid);
         absorber.SetFieldGridFront(frontGrid);
         absorber.SetFieldGridMiddle(middleGrid);
         absorber.SetFieldGridBack(backGrid);
      }

      // If MUCH node is a station
      if (TString(muchNode->GetName()).Contains("station")) {
    	 // Create station group
    	 lit::parallel::LitStationGroupMuon<T> sg;
         TObjArray* layerNodes = muchNode->GetNodes();
         // Calculate minimum and maximum Z of the substation which is needed for field approximation.
         Double_t minZ = std::numeric_limits<Double_t>::max(), maxZ = std::numeric_limits<Double_t>::min();
         for (Int_t iLayerNode = 0; iLayerNode < layerNodes->GetEntriesFast(); iLayerNode++) {
        	// Create station
        	lit::parallel::LitStationMuon<T> st;
        	st.SetType(lit::parallel::kLITPIXELHIT);
            const TGeoNode* sideNode = static_cast<const TGeoNode*>(layerNodes->At(iLayerNode));
            Bool_t bactive = false, factive = false;
            TObjArray* sideNodes = sideNode->GetNodes();
            for (Int_t iSideNode = 0; iSideNode < sideNodes->GetEntriesFast(); iSideNode++) {
               const TGeoNode* active = static_cast<const TGeoNode*>(sideNode->GetNodes()->At(iSideNode));
               if (TString(active->GetName()).Contains("active")) {
                  if (bactive && TString(active->GetName()).Contains("bactive")) { continue; }
                  if (factive && TString(active->GetName()).Contains("factive")) { continue; }

                  const TGeoBBox* activeShape = static_cast<const TGeoBBox*>(active->GetVolume()->GetShape());
                  const TGeoMaterial* activeMaterial = active->GetVolume()->GetMedium()->GetMaterial();

                  Double_t zPos = muchPos[2] + muchNode->GetMatrix()->GetTranslation()[2]
                          + sideNode->GetMatrix()->GetTranslation()[2] + active->GetMatrix()->GetTranslation()[2] + activeShape->GetDZ();

                  minZ = std::min(minZ, zPos);
                  maxZ = std::max(maxZ, zPos);

                  lit::parallel::LitSubstationMuon<T> ss;
                  ss.SetZ(zPos);

                  lit::parallel::LitMaterialInfo<T> material;
                  material.Thickness = 2. * activeShape->GetDZ();
                  material.A = activeMaterial->GetA();
                  material.Z = activeMaterial->GetZ();
                  material.Rho = activeMaterial->GetDensity();
                  material.X0 = activeMaterial->GetRadLen();
                  material.I = (activeMaterial->GetZ() > 16.) ? 10. * activeMaterial->GetZ() * 1e-9 : 16. * std::pow(activeMaterial->GetZ(), 0.9) * 1e-9;
                  ss.SetMaterial(material);

                  st.AddSubstation(ss);

                  bactive = TString(active->GetName()).Contains("bactive");
                  factive = TString(active->GetName()).Contains("factive");
                  if (factive && bactive) { break; }
               }
            }
            sg.AddStation(st);
         }
         // Magnetic field approximation for the station group
         lit::parallel::LitFieldGrid frontGrid, middleGrid, backGrid;
         gridCreator.CreateGrid(minZ, frontGrid);
         gridCreator.CreateGrid((minZ + 0.5 * (maxZ - minZ)), middleGrid);
         gridCreator.CreateGrid(maxZ, backGrid);
         sg.SetFieldGridFront(frontGrid);
         sg.SetFieldGridMiddle(middleGrid);
         sg.SetFieldGridBack(backGrid);
         layout.AddStationGroup(sg);
      }
   }

   std::cout << layout;
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
/*   cout << "Getting TRD layout for parallel version of tracking..." << endl;

   CbmLitFieldGridCreator gridCreator;
   CbmLitSimpleGeometryConstructor* geoConstructor = CbmLitSimpleGeometryConstructor::Instance();
   const std::vector<CbmLitMaterialInfo>& trdMaterial = geoConstructor->GetMyTrdGeoNodes();
   const std::vector<CbmLitMaterialInfo>& richMaterial = geoConstructor->GetMyRichGeoNodes();

   TrdLayout();
   const CbmLitDetectorLayout& trdLayout = GetTrdLayout();
   cout << trdLayout.ToString();

   // Add virtual planes
   int richMatCnt = 0;
   for (int nvp = 0; nvp < 33; nvp++) {
      lit::parallel::LitVirtualPlaneElectron<T> virtualPlane;
      float DZ = 10.;
      float Z = 100. + nvp * DZ;

      lit::parallel::LitFieldGrid fieldGrid, fieldGridMid;
      gridCreator.CreateGrid(Z, fieldGrid);
      gridCreator.CreateGrid(Z + DZ/2., fieldGridMid);

      CbmLitMaterialInfo mat;
      lit::parallel::LitMaterialInfo<T> m;
      if (nvp >= 10 && nvp <= 15) { // Add RICH material
         mat = richMaterial[richMatCnt++];
         m.Thickness = mat.GetLength();
      } else { // Add air
         mat = trdMaterial[5]; //air material
         m.Thickness = DZ;//mat.GetLength();
      }
      m.A = mat.GetA();
      m.Z = mat.GetZ();
      m.I = (mat.GetZ() > 16)? 10 * mat.GetZ() * 1e-9 :
            16 * std::pow(mat.GetZ(), 0.9) * 1e-9;
      m.Rho = mat.GetRho();
      m.X0 = mat.GetRL();
      m.Zpos = Z + DZ;//mat.GetZpos();
      m.CalculateValues();

      virtualPlane.SetZ(Z);
      virtualPlane.SetFieldGrid(fieldGrid);
      virtualPlane.SetFieldGridMid(fieldGridMid);
      virtualPlane.SetMaterial(m);

      layout.AddVirtualPlane(virtualPlane);
   }
   // end add virtual planes

   for (int isg = 0; isg < trdLayout.GetNofStationGroups(); isg++) {
      const CbmLitStationGroup& stationGroup = trdLayout.GetStationGroup(isg);
      lit::parallel::LitStationGroupElectron<T> sg;

      for (int ist = 0; ist < stationGroup.GetNofStations(); ist++) {
         const CbmLitStation& station = stationGroup.GetStation(ist);

         for(int iss = 0; iss < station.GetNofSubstations(); iss++) {
            const CbmLitSubstation& substation = station.GetSubstation(iss);
            lit::parallel::LitStationElectron<T> st;
            st.SetZ(substation.GetZ());

            int matId = TrdMaterialId(isg, ist, trdLayout);
            for (int im = 0; im < 6; im++) {
               CbmLitMaterialInfo mat = trdMaterial[matId + im];
               lit::parallel::LitMaterialInfo<T> m;
               m.A = mat.GetA();
               m.Z = mat.GetZ();
               m.I = (mat.GetZ() > 16)? 10 * mat.GetZ() * 1e-9 :
                     16 * std::pow(mat.GetZ(), 0.9) * 1e-9;
               m.Rho = mat.GetRho();
               m.Thickness = mat.GetLength();
               m.X0 = mat.GetRL();
               m.Zpos = mat.GetZpos();
               m.CalculateValues();

               if (im < 3) { st.AddMaterialBefore(m); }
               else { st.AddMaterialAfter(m); }
            }

            sg.AddStation(st);
         } // loop over substations
      } // loop over stations
      layout.AddStationGroup(sg);
   } // loop over station groups
   cout << layout;
   cout << "Finish getting TRD layout for parallel version of tracking..." << endl;
   */
}

const CbmLitDetectorLayout& CbmLitTrackingGeometryConstructor::GetLayout()
{
   static Bool_t layoutCreated = false;
   if (!layoutCreated) {
      CbmLitDetectorSetup det;
      det.DetermineSetup();
      if (det.GetDet(kMUCH) && !det.GetDet(kTRD)) {
         fLayout = GetMuchLayout();
      } else if (det.GetDet(kTRD) && !det.GetDet(kMUCH)) {
         fLayout = GetTrdLayout();
      } else if (det.GetDet(kMUCH) && det.GetDet(kTRD)) {
    	  // Combination of MUCH and TRD
          GetMuchLayout();
          GetTrdLayout();
          fLayout = fMuchLayout;
          for(Int_t i = 0; i < fTrdLayout.GetNofStationGroups(); i++) {
             fLayout.AddStationGroup(fTrdLayout.GetStationGroup(i));
          }
      }
      std::cout << fLayout.ToString();
      layoutCreated = true;
   }
   return fLayout;
}

const CbmLitDetectorLayout& CbmLitTrackingGeometryConstructor::GetMvdLayout()
{
   static Bool_t layoutCreated = false;
   if (!layoutCreated) {
      ConstructMvd();
      layoutCreated = true;
   }
   return fMvdLayout;
}

void CbmLitTrackingGeometryConstructor::ConstructMvd()
{
	std::set<CbmLitStation, CompareStationZLess> stationSet;
	TGeoRotation* matrix = new TGeoRotation();
	TGeoNode* node1 = gGeoManager->GetTopVolume()->FindNode("pipevac1_0");
	TObjArray* nodes = node1->GetNodes();
	for (Int_t iNode = 0; iNode < nodes->GetEntriesFast(); iNode++) {
		TGeoNode* node = (TGeoNode*) nodes->At(iNode);
		if (TString(node->GetName()).Contains("mvdstation")) {
			TGeoNode* station = node;
			const Double_t* stationPos = station->GetMatrix()->GetTranslation();
			CbmLitStation sta;
			CbmLitSubstation substation;
			substation.AddModule(0, matrix);
			substation.SetZ(stationPos[2]);
			sta.SetType(kLITPIXELHIT);
			sta.AddSubstation(substation);
			stationSet.insert(sta);
		}
	}
	CbmLitStationGroup sg;
	std::set<CbmLitStation, CompareStationZLess>::reverse_iterator it;
	for (it = stationSet.rbegin(); it != stationSet.rend(); it++) {
	 sg.AddStation(*it);
	}
	fMvdLayout.AddStationGroup(sg);
	std::cout << fMvdLayout.ToString();
}

const CbmLitDetectorLayout& CbmLitTrackingGeometryConstructor::GetMuchLayout()
{
   static Bool_t layoutCreated = false;
   if (!layoutCreated) {
      ConstructMuch();
      layoutCreated = true;
   }
   return fMuchLayout;
}

void CbmLitTrackingGeometryConstructor::ConstructMuch()
{
	TGeoMatrix* matrix = new TGeoRotation(); // needed to define rotation matrix for module
	CbmMuchGeoScheme* geoScheme = CbmMuchGeoScheme::Instance();
	FairRuntimeDb* db = FairRuntimeDb::instance();
	CbmGeoMuchPar* geoPar = (CbmGeoMuchPar*) db->getContainer("CbmGeoMuchPar");
	TObjArray* stations = (TObjArray*) geoPar->GetStations();
	geoScheme->Init(stations);
	Int_t nofStations = geoScheme->GetNStations();
	for (Int_t iStation = 0; iStation < nofStations; iStation++) {
		CbmMuchStation* station = geoScheme->GetStation(iStation);
		Int_t nofLayers = station->GetNLayers();
		CbmLitStationGroup LitStationGroupMuon;
		for (Int_t iLayer = 0; iLayer < nofLayers; iLayer++) {
			CbmMuchLayer* layer = station->GetLayer(iLayer);
			Double_t zFront = layer->GetSideF()->GetZ();// - geoScheme->GetGetActiveLz();
			Double_t zBack = layer->GetSideB()->GetZ();// - layer->GetSupportDz();
			//            std::cout << ">> sideF=" << layer->GetSideF()->GetZ() << " sideB=" << layer->GetSideB()->GetZ()
			//            		<< " moduleF=" << layer->GetSide(false)->GetModule(0)->GetPosition()[2]
			//            		<< " moduleB=" << layer->GetSide(true)->GetModule(0)->GetPosition()[2] << "\n";

			CbmLitSubstation litSubstationFront, litSubstationBack;
			litSubstationFront.AddModule(0, matrix);
			litSubstationFront.SetZ(zFront);
			litSubstationBack.AddModule(0, matrix);
			litSubstationBack.SetZ(zBack);

			CbmLitStation LitStationMuon;

			Int_t type = layer->GetSideF()->GetModule(0)->GetDetectorType();

			LitStationMuon.AddSubstation(litSubstationFront);
			if (station->IsModuleDesign() || type == 2) { LitStationMuon.AddSubstation(litSubstationBack); }

			if (type == 2) { LitStationMuon.SetType(kLITSTRIPHIT); }
			else { LitStationMuon.SetType(kLITPIXELHIT); }

			LitStationGroupMuon.AddStation(LitStationMuon);
		}
		fMuchLayout.AddStationGroup(LitStationGroupMuon);
	}
}

const CbmLitDetectorLayout& CbmLitTrackingGeometryConstructor::GetTrdLayout()
{
   static Bool_t layoutCreated = false;
   if (!layoutCreated) {
      ConstructTrd();
      fTrdLayout.SetGeo(fTrdTrackingGeo);
      layoutCreated = true;
   }
   return fTrdLayout;
}

void CbmLitTrackingGeometryConstructor::ConstructTrd()
{
	TObject* trd1 = fGeo->GetTopNode()->GetNodes()->FindObject("trd1_0");
	TObject* trd2 = fGeo->GetTopNode()->GetNodes()->FindObject("trd2_0");
	if (trd1 != NULL && trd2 != NULL) {
		ConstructTrdWitDifferentKeepingVolumes();
	} else {
		ConstructTrdWithSameKeepingVolume();
	}
}

void CbmLitTrackingGeometryConstructor::ConstructTrdWitDifferentKeepingVolumes()
{
	std::cout << "-I- Construction of the TRD geometry started" << std::endl;

	// Saved pointer to gGeoManager which will point to a new object after creation of new TGeoManager
	TGeoManager* savedGeo = gGeoManager;
	gGeoManager = 0;
	// Create new TGeomanager for TRD tracking geometry
	fTrdTrackingGeo = new TGeoManager("TrdTrackingGeo", "CBM TRD tracking geometry");
	TGeoVolume* topVolume = fTrdTrackingGeo->MakeBox("cave", fMedium["air"], 20000., 20000., 20000.);
	fTrdTrackingGeo->SetTopVolume(topVolume);

	TObjArray* topNodes = fGeo->GetTopNode()->GetNodes();
	Int_t nofTopNodes = topNodes->GetEntriesFast();
	for (Int_t iTopNode = 0; iTopNode < nofTopNodes; iTopNode++) {
		TGeoNode* topNode = static_cast<TGeoNode*>(topNodes->At(iTopNode));
		if (TString(topNode->GetName()).Contains("trd")) {
			map<Int_t, CbmLitSubstation> ss;
			TGeoNode* station = topNode;
			TObjArray* modules = station->GetNodes();
			for (Int_t iModule = 0; iModule < modules->GetEntriesFast(); iModule++) {
			   TGeoNode* module = static_cast<TGeoNode*>(modules->At(iModule));
			   Int_t stationId = std::atoi(string(1, *(module->GetName() + 3)).c_str()) - 1; // 4th element is station number
			   Int_t layerId = std::atoi(string(1, *(module->GetName() + 9)).c_str()) - 1; // 10th element is layer number
			   Int_t copyId = std::atoi(string(module->GetName()).erase(0, 10).c_str()) - 1; // copy id are the last 3 symbols
			   Int_t moduleType = std::atoi(string(1, *(module->GetName() + 7)).c_str()); // 8th symbol is module type
			   Int_t moduleId = moduleType * 1000 + copyId;

			   TObjArray* moduleParts = module->GetNodes();
			   for (Int_t iModulePart = 0; iModulePart < moduleParts->GetEntriesFast(); iModulePart++) {
				  TGeoNode* modulePart = static_cast<TGeoNode*>(moduleParts->At(iModulePart));
				  if (TString(modulePart->GetName()).Contains("gas")) {
					  TGeoBBox* moduleShape = static_cast<TGeoBBox*>(module->GetVolume()->GetShape());
					  TGeoBBox* modulePartShape = static_cast<TGeoBBox*>(modulePart->GetVolume()->GetShape());
				      TGeoShape* newShape = new TGeoBBox(moduleShape->GetDX(), moduleShape->GetDY(), modulePartShape->GetDZ());
				      TGeoMedium* newMedium = fMedium["air"];
				      string moduleName = ToString<Int_t>(stationId) + "_" + ToString<Int_t>(layerId) + "_" + ToString<Int_t>(0) + "_" + ToString<Int_t>(moduleId);
				      TGeoVolume* newVolume = new TGeoVolume(moduleName.c_str(), newShape, newMedium);
				      TGeoHMatrix* newMatrix = new TGeoHMatrix((*station->GetMatrix()) * (*module->GetMatrix()) * (*modulePart->GetMatrix()));
				      newVolume->SetLineColor(gRandom->Uniform(0, 100));
				      fTrdTrackingGeo->GetTopVolume()->AddNode(newVolume, 0, newMatrix);

				      ss[layerId].AddModule(moduleId, newMatrix);

				      // Get Z position for substation only works for perpendicular stations
				      // Does not work for tilted stations
				      Double_t stationZ = station->GetMatrix()->GetTranslation()[2];
				      Double_t moduleZ = module->GetMatrix()->GetTranslation()[2];
				      Double_t modulePartZ = modulePart->GetMatrix()->GetTranslation()[2];
				      Double_t additionalZ = modulePartShape->GetDZ();
				      ss[layerId].SetZ(stationZ + moduleZ + modulePartZ + additionalZ);
				  }
			   }
			}
			CbmLitStationGroup sg;
			map<Int_t, CbmLitSubstation>::iterator it;
			for (it = ss.begin(); it != ss.end(); it++) {
				CbmLitStation st;
				st.SetType(kLITPIXELHIT);
				st.AddSubstation(it->second);
				sg.AddStation(st);
			}
			fTrdLayout.AddStationGroup(sg);
		}
	}

//	fTrdTrackingGeo->CloseGeometry();
	fTrdTrackingGeo->Print();
	fTrdTrackingGeo->CheckOverlaps(1e-7,"SAME");
	fTrdTrackingGeo->PrintOverlaps();
//	fTrdTrackingGeo->Write();

	gGeoManager = savedGeo;

	std::cout << fTrdLayout.ToString();
    std::cout << "-I- Construction of the TRD geometry finished" << std::endl;
}


void CbmLitTrackingGeometryConstructor::ConstructTrdWithSameKeepingVolume()
{
	std::cout << "-I- Construction of the TRD geometry with same keeping volume started" << std::endl;

	// Saved pointer to gGeoManager which will point to a new object after creation of new TGeoManager
	TGeoManager* savedGeo = gGeoManager;
	gGeoManager = 0;
	// Create new TGeomanager for TRD tracking geometry
	fTrdTrackingGeo = new TGeoManager("TrdTrackingGeo", "CBM TRD tracking geometry");
	TGeoVolume* topVolume = fTrdTrackingGeo->MakeBox("cave", fMedium["air"], 20000., 20000., 20000.);
	fTrdTrackingGeo->SetTopVolume(topVolume);

	TGeoNode* trd = static_cast<TGeoNode*>(fGeo->GetTopNode()->GetNodes()->FindObject("trd1_0"));
	map<Int_t, map<Int_t, CbmLitSubstation> > ss; // First index is station group ID, second is station(substation) ID
	TObjArray* modules = trd->GetNodes();
	for (Int_t iModule = 0; iModule < modules->GetEntriesFast(); iModule++) {
	   TGeoNode* module = static_cast<TGeoNode*>(modules->At(iModule));
	   Int_t stationId = std::atoi(string(1, *(module->GetName() + 9)).c_str()) - 1; // 10th element is station number
	   Int_t layerId = std::atoi(string(1, *(module->GetName() + 10)).c_str()) - 1; // 11th element is layer number
	   Int_t copyId = std::atoi(string(module->GetName()).erase(0, 11).c_str()) - 1; // copy id are the last 3 symbols
	   Int_t moduleType = std::atoi(string(1, *(module->GetName() + 7)).c_str()); // 8th symbol is module type
	   Int_t moduleId = moduleType * 1000 + copyId;

	   TObjArray* moduleParts = module->GetNodes();
	   for (Int_t iModulePart = 0; iModulePart < moduleParts->GetEntriesFast(); iModulePart++) {
		  TGeoNode* modulePart = static_cast<TGeoNode*>(moduleParts->At(iModulePart));
		  if (TString(modulePart->GetName()).Contains("gas")) {
			  TGeoBBox* moduleShape = static_cast<TGeoBBox*>(module->GetVolume()->GetShape());
			  TGeoBBox* modulePartShape = static_cast<TGeoBBox*>(modulePart->GetVolume()->GetShape());
			  TGeoShape* newShape = new TGeoBBox(moduleShape->GetDX(), moduleShape->GetDY(), modulePartShape->GetDZ());
			  TGeoMedium* newMedium = fMedium["air"];
			  string moduleName = ToString<Int_t>(stationId) + "_" + ToString<Int_t>(layerId) + "_" + ToString<Int_t>(0) + "_" + ToString<Int_t>(moduleId);
			  TGeoVolume* newVolume = new TGeoVolume(moduleName.c_str(), newShape, newMedium);
			  TGeoHMatrix* newMatrix = new TGeoHMatrix((*trd->GetMatrix()) * (*module->GetMatrix()) * (*modulePart->GetMatrix()));
			  newVolume->SetLineColor(gRandom->Uniform(0, 100));
			  fTrdTrackingGeo->GetTopVolume()->AddNode(newVolume, 0, newMatrix);

			  ss[stationId][layerId].AddModule(moduleId, newMatrix);

			  // Get Z position for substation only works for perpendicular stations
			  // Does not work for tilted stations
			  Double_t stationZ = trd->GetMatrix()->GetTranslation()[2];
			  Double_t moduleZ = module->GetMatrix()->GetTranslation()[2];
			  Double_t modulePartZ = modulePart->GetMatrix()->GetTranslation()[2];
			  Double_t additionalZ = modulePartShape->GetDZ();
			  ss[stationId][layerId].SetZ(stationZ + moduleZ + modulePartZ + additionalZ);
		  }
	   }
	}

	map<Int_t, map<Int_t, CbmLitSubstation> >::iterator itsg;
	map<Int_t, CbmLitSubstation>::iterator itss;
	for (itsg = ss.begin(); itsg != ss.end(); itsg++) {
		CbmLitStationGroup sg;
		for (itss = itsg->second.begin(); itss != itsg->second.end(); itss++) {
			CbmLitStation st;
			st.SetType(kLITPIXELHIT);
			st.AddSubstation(itss->second);
			sg.AddStation(st);
		}
		fTrdLayout.AddStationGroup(sg);
	}

//	fTrdTrackingGeo->CloseGeometry();
	fTrdTrackingGeo->Print();
	fTrdTrackingGeo->CheckOverlaps(1e-7,"SAME");
	fTrdTrackingGeo->PrintOverlaps();
//	fTrdTrackingGeo->Write();

	gGeoManager = savedGeo;

	std::cout << fTrdLayout.ToString();
    std::cout << "-I- Construction of the TRD geometry with same keeping volume finished" << std::endl;
}

const CbmLitDetectorLayout& CbmLitTrackingGeometryConstructor::GetTofLayout()
{
   static Bool_t layoutCreated = false;
   if (!layoutCreated) {
      ConstructTof();
      layoutCreated = true;
   }
   return fTofLayout;
}

void CbmLitTrackingGeometryConstructor::ConstructTof()
{
	CbmLitStation station;
	TObjArray* nodes = gGeoManager->GetTopNode()->GetNodes();
	for (Int_t iNode = 0; iNode < nodes->GetEntriesFast(); iNode++) {
		TGeoNode* tof = (TGeoNode*) nodes->At(iNode);
		if (TString(tof->GetName()).Contains("tof1_0")) {
			const Double_t* tofPos = tof->GetMatrix()->GetTranslation();
			TGeoNode* gas = (TGeoNode*)tof->GetNodes()->At(0);
			const Double_t* gasPos = gas->GetMatrix()->GetTranslation();
			TGeoNode* reg = (TGeoNode*)gas->GetNodes()->At(0);
			const Double_t* regPos = reg->GetMatrix()->GetTranslation();
			TGeoBBox* shape = (TGeoBBox*) reg->GetVolume()->GetShape();

			CbmLitSubstation substation;
			substation.SetZ(tofPos[2] + gasPos[2] + regPos[2] + shape->GetDZ());// - shape->GetDZ());
			station.SetType(kLITPIXELHIT);
			station.AddSubstation(substation);
		}
	}
	CbmLitStationGroup stationGroup;
	stationGroup.AddStation(station);
	fTofLayout.AddStationGroup(stationGroup);
	std::cout << fTofLayout.ToString();
}

const CbmLitStation& CbmLitTrackingGeometryConstructor::GetTofStation()
{
   static Bool_t layoutCreated = false;
   if (!layoutCreated) {
      ConstructTofStationV13a();
      layoutCreated = true;
   }
   return fTofStation;
}

void CbmLitTrackingGeometryConstructor::ConstructTofStation()
{
	CbmLitStation station;
	TObjArray* nodes = gGeoManager->GetTopNode()->GetNodes();
	for (Int_t iNode = 0; iNode < nodes->GetEntriesFast(); iNode++) {
		TGeoNode* tof = (TGeoNode*) nodes->At(iNode);
		if (TString(tof->GetName()).Contains("tof1_0")) {
			const Double_t* tofPos = tof->GetMatrix()->GetTranslation();
			TGeoNode* gas = (TGeoNode*)tof->GetNodes()->At(0);
			const Double_t* gasPos = gas->GetMatrix()->GetTranslation();
			TGeoNode* reg = (TGeoNode*)gas->GetNodes()->At(0);
			const Double_t* regPos = reg->GetMatrix()->GetTranslation();
			TGeoBBox* shape = (TGeoBBox*) reg->GetVolume()->GetShape();

			CbmLitSubstation substation;
			substation.SetZ(tofPos[2] + gasPos[2] + regPos[2] + shape->GetDZ());
			station.SetType(kLITPIXELHIT);
			station.AddSubstation(substation);
		}
	}
	fTofStation = station;
	std::cout << fTofStation.ToString();
}

void CbmLitTrackingGeometryConstructor::ConstructTofStationV13a()
{
   // Saved pointer to gGeoManager which will point to a new object after creation of new TGeoManager
   TGeoManager* savedGeo = gGeoManager;
   gGeoManager = 0;
   // Create new TGeoManager for TOF tracking geometry
   fTofTrackingGeo = new TGeoManager("TofTrackingGeo", "CBM TOF tracking geometry");
   TGeoVolume* topVolume = fTofTrackingGeo->MakeBox("cave", fMedium["air"], 20000., 20000., 20000.);
   fTofTrackingGeo->SetTopVolume(topVolume);



   CbmLitStation station;
   TObjArray* nodes = fGeo->GetTopNode()->GetNodes();
   for (Int_t iNode = 0; iNode < nodes->GetEntriesFast(); iNode++) {
      TGeoNode* tof = (TGeoNode*) nodes->At(iNode);
      if (TString(tof->GetName()).Contains("tof")) { // Looking for nodes with "tof"
         const Double_t* tofPos = tof->GetMatrix()->GetTranslation();
         TGeoNode* gas = (TGeoNode*)tof->GetNodes()->At(0); // There is only one node so we take it safely
         const Double_t* gasPos = gas->GetMatrix()->GetTranslation();
         TObjArray* gasNodes = gas->GetNodes(); // Nodes inside gas volume

         // Loop over the nodes in gas volume and look for modules, i.e. nodes containing "mod" in the name.
         for (Int_t iGasNode = 0; iGasNode < gasNodes->GetEntriesFast(); iGasNode++) {
            TGeoNode* module = (TGeoNode*) gasNodes->At(iGasNode);
            if (TString(module->GetName()).Contains("mod")) { // Looking for nodes with "mod"
               const Double_t* modulePos = module->GetMatrix()->GetTranslation();

               TGeoBBox* moduleShape = static_cast<TGeoBBox*>(module->GetVolume()->GetShape());
              // TGeoBBox* modulePartShape = static_cast<TGeoBBox*>(modulePart->GetVolume()->GetShape());
               TGeoShape* newShape = new TGeoBBox(moduleShape->GetDX(), moduleShape->GetDY(), moduleShape->GetDZ());
               TGeoMedium* newMedium = fMedium["air"];
              // string moduleName = ToString<Int_t>(stationId) + "_" + ToString<Int_t>(layerId) + "_" + ToString<Int_t>(0) + "_" + ToString<Int_t>(moduleId);
               string moduleName = module->GetName();
               TGeoVolume* newVolume = new TGeoVolume(moduleName.c_str(), newShape, newMedium);
               TGeoHMatrix* newMatrix = new TGeoHMatrix((*tof->GetMatrix()) * (*gas->GetMatrix()) * (*module->GetMatrix()));
               newVolume->SetLineColor(gRandom->Uniform(0, 100));
               fTofTrackingGeo->GetTopVolume()->AddNode(newVolume, 0, newMatrix);
            }
         }

         CbmLitSubstation substation;
         //substation.SetZ(tofPos[2] + gasPos[2] + modulePos[2] + shape->GetDZ());
         station.SetType(kLITPIXELHIT);
         station.AddSubstation(substation);
      }
   }



// fTofTrackingGeo->CloseGeometry();
   fTofTrackingGeo->Print();
   fTofTrackingGeo->CheckOverlaps(1e-7,"SAME");
   fTofTrackingGeo->PrintOverlaps();
// fTofTrackingGeo->Write();

   gGeoManager = savedGeo;

   fTofStation = station;
   std::cout << fTofStation.ToString();
}

void CbmLitTrackingGeometryConstructor::ConstructRich()
{
	/*
   std::cout << "-I- Construction of the RICH geometry started" << std::endl;

   TGeoNode* rich = (TGeoNode*) fGeo->GetTopNode()->GetNodes()->FindObject("rich1_0");
   const double* richPos  = rich->GetMatrix()->GetTranslation();

   TGeoNode* gas1 = (TGeoNode*) rich->GetNodes()->FindObject("rich1gas1_0");
   const double* gas1Pos  = gas1->GetMatrix()->GetTranslation();
   ReadRichTRAP(gas1, richPos[2]);

   TGeoNode* gas3 = (TGeoNode*) rich->GetNodes()->FindObject("rich1gas3_0");
   const double* gas3Pos  = gas3->GetMatrix()->GetTranslation();
   ReadRichTRAP(gas3, richPos[2]);

   // Photodetector
   TGeoNode* photo = (TGeoNode*) gas1->GetNodes()->FindObject("rich1d_1");
   const double* photoPos  = photo->GetMatrix()->GetTranslation();
   photo->Print();
   TGeoBBox* photoShape = (TGeoBBox*) photo->GetVolume()->GetShape();
   TGeoMedium* photoMed = photo->GetVolume()->GetMedium();
   TGeoShape* photoShape1 = new TGeoCone(photoShape->GetDZ(), 0., 300.,   0.,   300.);
   TGeoMedium* photoMed1 = fMedium[photoMed->GetName()];
   TGeoVolume* photoVolume1 = new TGeoVolume(photo->GetName(), photoShape1, photoMed1);
   litfloat photoZ1 = richPos[2] + gas1Pos[2] + photoPos[2];
   TGeoMatrix* photoMatrix1 = new TGeoTranslation(0, 0, photoZ1);
   fSimpleGeo->GetTopVolume()->AddNode(photoVolume1, 0, photoMatrix1);

   CbmLitMaterialInfo photoLitMaterial;
   photoLitMaterial.SetLength(2. * photoShape->GetDZ());
   photoLitMaterial.SetZpos(photoZ1);
   GeoMediumToMaterialInfo(photoMed1, photoLitMaterial);
   fMyGeoNodes.push_back(photoLitMaterial);
   fMyRichGeoNodes.push_back(photoLitMaterial);
   // end photodetector

   // Mirror
   TGeoNode* mgl = (TGeoNode*) gas3->GetNodes()->FindObject("rich1mgl_1");
   const double* mglPos  = mgl->GetMatrix()->GetTranslation();
   TGeoSphere* mglShape = (TGeoSphere*) mgl->GetVolume()->GetShape();
   TGeoMedium* mglMed = mgl->GetVolume()->GetMedium();
   double mglDZ = mglShape->GetRmax() - mglShape->GetRmin();
   TGeoShape* mglShape1 = new TGeoCone(mglDZ, 0., 300.,   0.,   300.);
   TGeoMedium* mglMed1 = fMedium[mglMed->GetName()];
   TGeoVolume* mglVolume1 = new TGeoVolume(mgl->GetName(), mglShape1, mglMed1);
   litfloat mglZ1 = richPos[2] + gas3Pos[2];// + mglPos[2];
   TGeoMatrix* mglMatrix1 = new TGeoTranslation(0, 0, mglZ1);
   fSimpleGeo->GetTopVolume()->AddNode(mglVolume1, 0, mglMatrix1);

   CbmLitMaterialInfo mglLitMaterial;
   mglLitMaterial.SetLength(mglDZ);
   mglLitMaterial.SetZpos(mglZ1);
   GeoMediumToMaterialInfo(mglMed1, mglLitMaterial);
   fMyGeoNodes.push_back(mglLitMaterial);
   fMyRichGeoNodes.push_back(mglLitMaterial);
   // end mirror

   // Entrance
   TGeoNode* entrance = (TGeoNode*) rich->GetNodes()->FindObject("rich1entrance_0");
   ReadRichTRAP(entrance, richPos[2]);
   // end entrance

   // Exit
   TGeoNode* exit = (TGeoNode*) rich->GetNodes()->FindObject("rich1exit_0");
   ReadRichTRAP(exit, richPos[2]);
   // end exit

   std::cout << "-I- Construction of the RICH geometry finished" << std::endl;
   */
}

void CbmLitTrackingGeometryConstructor::ReadRichTRAP(
      TGeoNode* node,
      double startZ)
{
/*   TGeoTrap* shape = (TGeoTrap*) node->GetVolume()->GetShape();
   TGeoMedium* med = node->GetVolume()->GetMedium();
   double DZ = 2 * shape->GetDz();
   TGeoShape* shape1 = new TGeoCone(DZ, 0., 300.,   0.,   300.);
   TGeoMedium* med1 = fMedium[med->GetName()];
   TGeoVolume* volume1 = new TGeoVolume(node->GetName(), shape1, med1);
   litfloat Z1 = startZ +  node->GetMatrix()->GetTranslation()[2];
   TGeoMatrix* matrix1 = new TGeoTranslation(0, 0, Z1);
   fSimpleGeo->GetTopVolume()->AddNode(volume1, 0, matrix1);

   CbmLitMaterialInfo litMaterial;
   litMaterial.SetLength(DZ);
   litMaterial.SetZpos(Z1);
   GeoMediumToMaterialInfo(med1, litMaterial);
   fMyGeoNodes.push_back(litMaterial);
   fMyRichGeoNodes.push_back(litMaterial);
   */
}


