#include "CbmLitSimpleGeometryConstructor.h"

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

CbmLitSimpleGeometryConstructor::CbmLitSimpleGeometryConstructor()
{
}

CbmLitSimpleGeometryConstructor::~CbmLitSimpleGeometryConstructor()
{
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

void CbmLitSimpleGeometryConstructor::CreateMediumList()
{
	fMedium["air"] = CreateMedium("air");
	fMedium["silicon"] = CreateMedium("silicon");
	fMedium["MUCHiron"] = CreateMedium("MUCHiron");
	fMedium["MUCHargon"] = CreateMedium("MUCHargon");
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

	Int_t nofStations = stsDigiScheme.GetNStations();
	for (Int_t i = 0; i < nofStations; i++ ) {
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
	}
	std::cout << "-I- Construction of the STS geometry finished" << std::endl;
}

void CbmLitSimpleGeometryConstructor::ConstructMuch()
{
	TGeoNode* much = (TGeoNode*) fGeo->GetTopNode()->GetNodes()->FindObject("much_0");
	const Double_t* muchPos  = much->GetMatrix()->GetTranslation();
	TObjArray* muchNodes = much->GetNodes();
	for (Int_t iMuchNode = 0; iMuchNode < muchNodes->GetEntriesFast(); iMuchNode++) {
		TGeoNode* muchNode = (TGeoNode*) muchNodes->At(iMuchNode);
		if (TString(muchNode->GetName()).Contains("absorber")) {
			TGeoCone* sh = (TGeoCone*) muchNode->GetVolume()->GetShape();
			TGeoMedium* med = muchNode->GetVolume()->GetMedium();

			TGeoShape* shape = new TGeoCone(sh->GetDz(), sh->GetRmin1(), sh->GetRmax1(),
					sh->GetRmin2(),	sh->GetRmax2());

			TGeoMedium* medium = fMedium[med->GetName()];

			TGeoVolume* volume = new TGeoVolume(muchNode->GetName(), shape, medium);
			TGeoMatrix* matrix = new TGeoTranslation(0, 0, muchPos[2] + muchNode->GetMatrix()->GetTranslation()[2]);
			fSimpleGeo->GetTopVolume()->AddNode(volume, 0, matrix);
		}

		if (TString(muchNode->GetName()).Contains("station")) {
			TObjArray* layerNodes = muchNode->GetNodes();
			for (Int_t iLayerNode = 0; iLayerNode < layerNodes->GetEntriesFast(); iLayerNode++) {
				TGeoNode* sideNode = (TGeoNode*) layerNodes->At(iLayerNode);

				Bool_t bactive = false, factive = false;
				for (Int_t iSideNode = 0; iSideNode < sideNode->GetNodes()->GetEntriesFast(); iSideNode++){
					TGeoNode* active = (TGeoNode*) sideNode->GetNodes()->At(iSideNode);
					if (TString(active->GetName()).Contains("active")) {
						if (bactive && TString(active->GetName()).Contains("bactive")) continue;
						if (factive && TString(active->GetName()).Contains("factive")) continue;

						TGeoBBox* sh = (TGeoBBox*) active->GetVolume()->GetShape();
						TGeoMedium* med = active->GetVolume()->GetMedium();

						TGeoShape* shape = new TGeoCone(sh->GetDZ(), 0., 300., 	0.,	300.);

						TGeoMedium* medium = fMedium[med->GetName()];

						TGeoVolume* volume = new TGeoVolume(active->GetName(), shape, medium);
						Double_t z = muchPos[2] + muchNode->GetMatrix()->GetTranslation()[2]
						       + sideNode->GetMatrix()->GetTranslation()[2] + active->GetMatrix()->GetTranslation()[2];
						std::cout << std::cout.precision(5) << "z station MUCH: " << z << std::endl;
						TGeoMatrix* matrix = new TGeoTranslation(0, 0, z);
						fSimpleGeo->GetTopVolume()->AddNode(volume, 0, matrix);

						bactive = TString(active->GetName()).Contains("bactive");
						factive = TString(active->GetName()).Contains("factive");

						if (factive && bactive) break;
					}
				}
			}
		}
	}
}

void CbmLitSimpleGeometryConstructor::ConstructTof()
{
	TGeoNode* tof = (TGeoNode*) fGeo->GetTopNode()->GetNodes()->FindObject("tof1_0");
	const Double_t* tofPos  = tof->GetMatrix()->GetTranslation();
	TGeoNode* gas = (TGeoNode*) tof->GetNodes()->FindObject("tof1gas_0");
	const Double_t* gasPos  = gas->GetMatrix()->GetTranslation();
	TGeoNode* mod = (TGeoNode*) gas->GetNodes()->FindObject("t1reg1mod_1");
	const Double_t* modPos  = mod->GetMatrix()->GetTranslation();
	TGeoNode* cel = (TGeoNode*) mod->GetNodes()->FindObject("t1reg1cel_1");
	const Double_t* celPos  = cel->GetMatrix()->GetTranslation();
	TGeoNode* ele = (TGeoNode*) cel->GetNodes()->FindObject("t1reg1ele_1");
	const Double_t* elePos  = ele->GetMatrix()->GetTranslation();

	Double_t z = tofPos[2] + gasPos[2] + modPos[2] + celPos[2];// + elePos[2];
	std::cout << "z position TOF: " << std::cout.precision(7) << "z station MUCH: " << z << std::endl;


//	TObjArray* nodes = tofGas->GetNodes();
//	for (Int_t iNode = 0; iNode < nodes->GetEntriesFast(); iNode++) {
//		TGeoNode* node = (TGeoNode*) nodes->At(iNode);
//
//	}
}

ClassImp(CbmLitSimpleGeometryConstructor);
