/**
 * \file CbmLitTrackingGeometryConstructor.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 **/

#include "base/CbmLitTrackingGeometryConstructor.h"

#include "base/CbmLitEnvironment.h"
#include "base/CbmLitFieldGridCreator.h"
#include "utils/CbmLitComparators.h"
#include "utils/CbmLitUtils.h"

#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "CbmStsStation.h"
#include "CbmGeoStsPar.h"
#include "CbmStsDigiScheme.h"

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
   fTrdTrackingGeo(NULL)
{
   ConstructGeometry();
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
//   fSimpleGeo->SetVisLevel(0);
//   TGeoVolume* master = fSimpleGeo->GetMasterVolume();
//   master->Draw("ogl");

   fTrdTrackingGeo->SetVisLevel(0);
   TGeoVolume* trdGeoMaster = fTrdTrackingGeo->GetMasterVolume();
   trdGeoMaster->Draw("ogl");
}

void CbmLitTrackingGeometryConstructor::ConstructGeometry()
{
   std::cout << "-I- Simple geometry construction started" << std::endl;

   fGeo = gGeoManager;
   fDet.DetermineSetup();
   CreateMediumList();


//
//   gGeoManager = 0;
//
//   // Create new simplified geometry
//   fSimpleGeo = new TGeoManager("FAIRSimpleGeom", "Simplified geometry");
//   TGeoVolume* topVolume = fSimpleGeo->MakeBox("cave", fMedium["air"], 20000., 20000., 20000.);
//   fSimpleGeo->SetTopVolume(topVolume);


//   ConstructSts();
//   if (fDet.GetDet(kMUCH)) { ConstructMuch(); }
   if (fDet.GetDet(kTRD)) { ConstructTrd(); }
//   if (fDet.GetDet(kTOF)) { ConstructTof(); }
//   if (fDet.GetDet(kRICH)) { ConstructRich(); }

//   fSimpleGeo->CloseGeometry();
//   fSimpleGeo->Print();
//   fSimpleGeo->CheckOverlaps(1e-7,"SAME");
//   fSimpleGeo->PrintOverlaps();
//   fSimpleGeo->Write();


//   gGeoManager = fGeo;
   std::cout << "-I- Simple geometry construction finished" << std::endl;
}

void CbmLitTrackingGeometryConstructor::GeoMediumToMaterialInfo(
   const TGeoMedium* med,
   CbmLitMaterialInfo& mat)
{
   TGeoMaterial* material = med->GetMaterial();
   mat.SetRL(material->GetRadLen());
   mat.SetRho(material->GetDensity());
   mat.SetZ(material->GetZ());
   mat.SetA(material->GetA());
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

void CbmLitTrackingGeometryConstructor::ConstructTof()
{
	/*
   std::cout << "-I- Construction of the TOF geometry started" << std::endl;
   TGeoNode* tof = (TGeoNode*) fGeo->GetTopNode()->GetNodes()->FindObject("tof1_0");
   const double* tofPos  = tof->GetMatrix()->GetTranslation();
   TGeoNode* gas = (TGeoNode*) tof->GetNodes()->FindObject("tof1gas_0");
   const double* gasPos  = gas->GetMatrix()->GetTranslation();
   TGeoNode* mod = (TGeoNode*) gas->GetNodes()->FindObject("t1reg1mod_1");
   const double* modPos  = mod->GetMatrix()->GetTranslation();
   TGeoNode* cel = (TGeoNode*) mod->GetNodes()->FindObject("t1reg1cel_1");
   const double* celPos  = cel->GetMatrix()->GetTranslation();

   litfloat Z = tofPos[2] + gasPos[2] + modPos[2] + celPos[2];

   for (int i = 0; i < cel->GetNodes()->GetEntriesFast(); ++i) {
      TGeoNode* ele = (TGeoNode*) cel->GetNodes()->At(i);
      TString name = ele->GetName();
      if (name.Contains("gap")) { continue; }

      const double* elePos  = ele->GetMatrix()->GetTranslation();
      TGeoBBox* sh = (TGeoBBox*) ele->GetVolume()->GetShape();
      TGeoMedium* med = ele->GetVolume()->GetMedium();
      TGeoMaterial* mat = med->GetMaterial();

      TGeoShape* shape = new TGeoCone(sh->GetDZ(), 0., 500.,   0.,   500.);
      TGeoMedium* medium = fMedium[med->GetName()];
      TGeoVolume* volume = new TGeoVolume(ele->GetName(), shape, medium);

      Z += elePos[2];
      TGeoMatrix* matrix = new TGeoTranslation(0, 0, Z);
      fSimpleGeo->GetTopVolume()->AddNode(volume, 0, matrix);

      CbmLitMaterialInfo litMaterial;
      litMaterial.SetLength(2. * sh->GetDZ());
      litMaterial.SetZpos(Z + sh->GetDZ());
      GeoMediumToMaterialInfo(medium, litMaterial);
      fMyGeoNodes.push_back(litMaterial);
   }

   std::cout << "-I- Construction of the TOF geometry finished" << std::endl;
   */
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

