#include "CbmLitSimpleGeometryConstructor.h"

#include "CbmLitComparators.h"

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

#include <iostream>
#include <sstream>
#include <algorithm>

CbmLitSimpleGeometryConstructor* CbmLitSimpleGeometryConstructor::fInstance = NULL;

CbmLitSimpleGeometryConstructor::CbmLitSimpleGeometryConstructor()
{
	ConstructGeometry();
}

CbmLitSimpleGeometryConstructor::~CbmLitSimpleGeometryConstructor()
{
	if (fInstance != NULL) delete fInstance;
}

CbmLitSimpleGeometryConstructor* CbmLitSimpleGeometryConstructor::Instance()
{
   if (fInstance == NULL) {
      fInstance = new CbmLitSimpleGeometryConstructor();
   }
   return fInstance;
}

void CbmLitSimpleGeometryConstructor::Draw()
{
	fSimpleGeo->SetVisLevel(0);
	TGeoVolume* master = fSimpleGeo->GetMasterVolume();
	master->Draw("ogl");
}

void CbmLitSimpleGeometryConstructor::ConstructGeometry()
{
	std::cout << "-I- Simple geometry construction started" << std::endl;
	fGeo = gGeoManager;

	CreateMediumList();

	gGeoManager = 0;

	// Create new simplified geometry
	fSimpleGeo = new TGeoManager("FAIRSimpleGeom", "Simplified geometry");
	TGeoVolume* topVolume = fSimpleGeo->MakeBox("cave", fMedium["air"], 20000., 20000., 20000.);
	fSimpleGeo->SetTopVolume(topVolume);

	ConstructSts();
	ConstructMuch();
	ConstructTof();

	fSimpleGeo->CloseGeometry();
	fSimpleGeo->Print();
	fSimpleGeo->CheckOverlaps(1e-7,"SAME");
	fSimpleGeo->PrintOverlaps();
	fSimpleGeo->Write();

	std::sort(fMyGeoNodes.begin(), fMyGeoNodes.end(), CompareMaterialInfoZLess());

	std::cout << "My Simple Geometry:" << std::endl;
	for (size_t i = 0; i < fMyGeoNodes.size(); ++i)
		std::cout << i << " " << fMyGeoNodes[i].ToString();

	std::cout << "-I- Simple geometry construction finished" << std::endl;
}

TGeoMedium* CbmLitSimpleGeometryConstructor::CreateMedium(
		const std::string& name)
{
	TGeoMedium* med = fGeo->GetMedium(name.c_str());
	TGeoMaterial* mat = med->GetMaterial();
	TGeoMaterial* newmat = new TGeoMaterial(mat->GetName(), mat->GetA(), mat->GetZ(), mat->GetDensity(), mat->GetRadLen(), mat->GetIntLen());
	TGeoMedium* newmed = new TGeoMedium(med->GetName(), med->GetId(), newmat);
	return newmed;
}

void CbmLitSimpleGeometryConstructor:: GeoMediumToMaterialInfo(
		const TGeoMedium* med,
		CbmLitMaterialInfo& mat)
{
	TGeoMaterial* material = med->GetMaterial();
	mat.SetRL(material->GetRadLen());
	mat.SetRho(material->GetDensity());
	mat.SetZ(material->GetZ());
	mat.SetA(material->GetA());
}

void CbmLitSimpleGeometryConstructor::CreateMediumList()
{
	fMedium["air"] = CreateMedium("air");
	fMedium["silicon"] = CreateMedium("silicon");
	fMedium["MUCHiron"] = CreateMedium("MUCHiron");
	fMedium["MUCHargon"] = CreateMedium("MUCHargon");
	fMedium["aluminium"] = CreateMedium("aluminium");
	fMedium["RPCgas"] = CreateMedium("RPCgas");
	fMedium["RPCglass"] = CreateMedium("RPCglass");
}

void CbmLitSimpleGeometryConstructor::ConstructSts()
{
	std::cout << "-I- Construction of the STS geometry started" << std::endl;
	FairRunAna *run = FairRunAna::Instance();
	FairRuntimeDb *runDB = run->GetRuntimeDb();
	CbmGeoStsPar* stsPar = (CbmGeoStsPar*) (runDB->getContainer("CbmGeoStsPar"));
	CbmStsDigiPar* digiPar = (CbmStsDigiPar*)(runDB->getContainer("CbmStsDigiPar"));
	CbmStsDigiScheme stsDigiScheme;
	stsDigiScheme.Init(stsPar, digiPar);

	int nofStations = stsDigiScheme.GetNStations();
	for (int i = 0; i < nofStations; i++ ) {
		CbmStsStation *station = stsDigiScheme.GetStation(i);
		if (station == NULL) continue;
		std::stringstream name;
		name << "sts" << i;

		TGeoShape* shape = new TGeoCone( station->GetD() / 2., 	station->GetRmin(),
				station->GetRmax(), station->GetRmin(),	station->GetRmax());

		TGeoMedium* medium = fMedium["silicon"];

		TGeoVolume* volume = new TGeoVolume(name.str().c_str(), shape, medium);
		TGeoMatrix* matrix = new TGeoTranslation(0, 0, station->GetZ());
		fSimpleGeo->GetTopVolume()->AddNode(volume, 0, matrix);

		CbmLitMaterialInfo litMaterial;
		litMaterial.SetLength(station->GetD());
		litMaterial.SetZpos(station->GetZ());
		GeoMediumToMaterialInfo(medium, litMaterial);
		fMyGeoNodes.push_back(litMaterial);
	}
	std::cout << "-I- Construction of the STS geometry finished" << std::endl;
}

void CbmLitSimpleGeometryConstructor::ConstructMuch()
{
	std::cout << "-I- Construction of the MUCH geometry started" << std::endl;
	TGeoNode* much = (TGeoNode*) fGeo->GetTopNode()->GetNodes()->FindObject("much_0");
	const double* muchPos  = much->GetMatrix()->GetTranslation();
	TObjArray* muchNodes = much->GetNodes();
	for (int iMuchNode = 0; iMuchNode < muchNodes->GetEntriesFast(); iMuchNode++) {
		TGeoNode* muchNode = (TGeoNode*) muchNodes->At(iMuchNode);
		if (TString(muchNode->GetName()).Contains("absorber")) {
			TGeoCone* sh = (TGeoCone*) muchNode->GetVolume()->GetShape();
			TGeoMedium* med = muchNode->GetVolume()->GetMedium();

			TGeoShape* shape = new TGeoCone(sh->GetDz(), sh->GetRmin1(), sh->GetRmax1(),
					sh->GetRmin2(),	sh->GetRmax2());

			TGeoMedium* medium = fMedium[med->GetName()];

			double Z = muchPos[2] + muchNode->GetMatrix()->GetTranslation()[2];

			TGeoVolume* volume = new TGeoVolume(muchNode->GetName(), shape, medium);
			TGeoMatrix* matrix = new TGeoTranslation(0, 0, Z);
			fSimpleGeo->GetTopVolume()->AddNode(volume, 0, matrix);

			CbmLitMaterialInfo litMaterial;
			litMaterial.SetLength(2. * sh->GetDz());
			litMaterial.SetZpos(Z + sh->GetDz());
			GeoMediumToMaterialInfo(medium, litMaterial);
			fMyGeoNodes.push_back(litMaterial);
		}

		if (TString(muchNode->GetName()).Contains("station")) {
			TObjArray* layerNodes = muchNode->GetNodes();
			for (int iLayerNode = 0; iLayerNode < layerNodes->GetEntriesFast(); iLayerNode++) {
				TGeoNode* sideNode = (TGeoNode*) layerNodes->At(iLayerNode);

				bool bactive = false, factive = false;
				for (int iSideNode = 0; iSideNode < sideNode->GetNodes()->GetEntriesFast(); iSideNode++){
					TGeoNode* active = (TGeoNode*) sideNode->GetNodes()->At(iSideNode);
					if (TString(active->GetName()).Contains("active")) {
						if (bactive && TString(active->GetName()).Contains("bactive")) continue;
						if (factive && TString(active->GetName()).Contains("factive")) continue;

						TGeoBBox* sh = (TGeoBBox*) active->GetVolume()->GetShape();
						TGeoMedium* med = active->GetVolume()->GetMedium();

						TGeoShape* shape = new TGeoCone(sh->GetDZ(), 0., 300., 	0.,	300.);

						TGeoMedium* medium = fMedium[med->GetName()];

						TGeoVolume* volume = new TGeoVolume(active->GetName(), shape, medium);
						double z = muchPos[2] + muchNode->GetMatrix()->GetTranslation()[2]
						       + sideNode->GetMatrix()->GetTranslation()[2] + active->GetMatrix()->GetTranslation()[2];
						std::cout << std::cout.precision(5) << "z station MUCH: " << z << std::endl;
						TGeoMatrix* matrix = new TGeoTranslation(0, 0, z);
						fSimpleGeo->GetTopVolume()->AddNode(volume, 0, matrix);

						CbmLitMaterialInfo litMaterial;
						litMaterial.SetLength(2. * sh->GetDZ());
						litMaterial.SetZpos(z);
						GeoMediumToMaterialInfo(medium, litMaterial);
						fMyGeoNodes.push_back(litMaterial);

						bactive = TString(active->GetName()).Contains("bactive");
						factive = TString(active->GetName()).Contains("factive");

						if (factive && bactive) break;
					}
				}
			}
		}
	}
	std::cout << "-I- Construction of the MUCH geometry finished" << std::endl;
}

void CbmLitSimpleGeometryConstructor::ConstructTof()
{
	std::cout << "-I- Construction of the TOF geometry started" << std::endl;
	TGeoNode* tof = (TGeoNode*) fGeo->GetTopNode()->GetNodes()->FindObject("tof1_0");
	const double* tofPos  = tof->GetMatrix()->GetTranslation();
	TGeoNode* gas = (TGeoNode*) tof->GetNodes()->FindObject("tof1gas_0");
	const double* gasPos  = gas->GetMatrix()->GetTranslation();
	TGeoNode* mod = (TGeoNode*) gas->GetNodes()->FindObject("t1reg1mod_1");
	const double* modPos  = mod->GetMatrix()->GetTranslation();
	TGeoNode* cel = (TGeoNode*) mod->GetNodes()->FindObject("t1reg1cel_1");
	const double* celPos  = cel->GetMatrix()->GetTranslation();

	double Z = tofPos[2] + gasPos[2] + modPos[2] + celPos[2];

	for (int i = 0; i < cel->GetNodes()->GetEntriesFast(); ++i) {
		TGeoNode* ele = (TGeoNode*) cel->GetNodes()->At(i);
		TString name = ele->GetName();
		if (name.Contains("gap")) continue;

		const double* elePos  = ele->GetMatrix()->GetTranslation();
		TGeoBBox* sh = (TGeoBBox*) ele->GetVolume()->GetShape();
		TGeoMedium* med = ele->GetVolume()->GetMedium();
		TGeoMaterial* mat = med->GetMaterial();

		TGeoShape* shape = new TGeoCone(sh->GetDZ(), 0., 500., 	0.,	500.);
		TGeoMedium* medium = fMedium[med->GetName()];
		TGeoVolume* volume = new TGeoVolume(ele->GetName(), shape, medium);

		Z += elePos[2];
		TGeoMatrix* matrix = new TGeoTranslation(0, 0, Z);
		fSimpleGeo->GetTopVolume()->AddNode(volume, 0, matrix);

		CbmLitMaterialInfo litMaterial;
		litMaterial.SetLength(2. * sh->GetDZ());
		litMaterial.SetZpos(Z);
		GeoMediumToMaterialInfo(medium, litMaterial);
		fMyGeoNodes.push_back(litMaterial);
	}

	std::cout << "-I- Construction of the TOF geometry finished" << std::endl;
}
