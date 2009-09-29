/** CbmLitEnvironment.cxx
 *@author A.Lebedev <alebedev@jinr.ru>
 *@since 2007
 **/
#define NEWMUCH 1

#include "CbmLitEnvironment.h"
#include "CbmLitMemoryManagment.h"
#include "CbmLitComparators.h"
#include "CbmLitFieldFitter.h"
#include "CbmLitSimpleGeometryConstructor.h"

#include "FairField.h"
#include "FairRunAna.h"
#include "FairDetector.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"

#ifdef NEWMUCH
#include "CbmMuchGeoScheme.h"
#include "CbmGeoMuchPar.h"
#include "CbmMuchStation.h"
#include "CbmMuchLayer.h"
#include "CbmMuchLayerSide.h"
#include "CbmMuchModule.h"
#endif

#include "TGeoNode.h"
#include "TGeoMaterial.h"
#include "TGeoManager.h"
#include "TGeoPcon.h"
#include "TGeoPgon.h"
#include "TGeoArb8.h"
#include "TGeoSphere.h"
#include "TGeoBBox.h"
#include "TGeoMatrix.h"

#include <set>
#include <functional>
#include <algorithm>
#include <cmath>

CbmLitEnvironment* CbmLitEnvironment::fInstance = NULL;

CbmLitEnvironment::CbmLitEnvironment():
   fField(NULL)
{
}

CbmLitEnvironment::~CbmLitEnvironment()
{
   if (fInstance != NULL) delete fInstance;
}

CbmLitEnvironment* CbmLitEnvironment::Instance()
{
   if (fInstance == NULL) {
      fInstance = new CbmLitEnvironment();
   }
   return fInstance;
}

FairField* CbmLitEnvironment::GetField()
{
   if (fField == NULL) {
      FairRunAna *Run = FairRunAna::Instance();

      if(NULL == Run) {
         std::cout << "-E- CbmLitEnvironment::GetField : "
            << "Run Ana is not instantiated" << std::endl;
         return NULL;
      }

      std::cout << "-I- CbmLitEnvironment::GetField : "
         << "Reading Magnetic Field " << std::endl;

      fField = (FairField*) Run->GetField();
   }
   return fField;
}

CbmLitDetectorLayout CbmLitEnvironment::GetLayout()
{
	static Bool_t layoutCreated = false;

	if (!layoutCreated) {
		if (IsMuch() && !IsTrd()) {
			MuchLayout();
			fLayout = fMuchLayout;
		} else
		if (IsTrd() && !IsMuch()){
			TrdLayout();
			fLayout = fTrdLayout;
		} else
		if (IsMuch() && IsTrd()) {
			CombineMuchAndTrd();
			fLayout = fMuchTrdLayout;
		}
		std::cout << fLayout.ToString();
		layoutCreated = true;
	}
	return fLayout;
}

void CbmLitEnvironment::MuchLayout()
{
	if ((TGeoNode*) gGeoManager->GetTopNode()->GetNodes()->FindObject("much1_0") != NULL){
		OldMuchLayout();
	} else {
		NewMuchLayout();
	}
}

void CbmLitEnvironment::OldMuchLayout()
{
	static Bool_t layoutCreated = false;

	if (!layoutCreated) {

		std::set<CbmLitStation, CompareStationZLess> stationSet;

		TGeoNode* much = (TGeoNode*) gGeoManager->GetTopNode()->GetNodes()->FindObject("much1_0");

		const Double_t* muchPos  = much->GetMatrix()->GetTranslation();
		TObjArray* muchNodes = much->GetNodes();
		for (Int_t iMuchNode = 0; iMuchNode < muchNodes->GetEntriesFast(); iMuchNode++) {
			TGeoNode* muchNode = (TGeoNode*) muchNodes->At(iMuchNode);

			if (IsStraw()) {// for straws
				Int_t nd = muchNode->CountDaughters(kTRUE);
				//cout << iMuchNode << " " << muchNode->GetName() << " " << nd << endl;
				TGeoNode *daught = muchNode;
				Double_t dz = daught->GetMatrix()->GetTranslation()[2];
				Double_t dz1 = 0.;
				for (Int_t i = 0; i < nd; ++i) {
					if (nd > 1) {
					    daught = muchNode->GetDaughter(i);
					    dz1 = daught->GetMatrix()->GetTranslation()[2];
					}
					//cout << i << " " << daught->GetName() << " " << dz << " " << dz1 << endl;
					if (TString(daught->GetName()).Contains("station")) {
					    TGeoPcon* shape = (TGeoPcon*) daught->GetVolume()->GetShape();
					    CbmLitStation station;
					    CbmLitSubstation substation;
					    substation.SetZ(muchPos[2] + shape->GetZ(0) + shape->GetDZ()
									   + dz + dz1);
					    // TODO temporary solution
					    if (substation.GetZ() > 220.)station.SetType(kLITSTRIPHIT);
					    else station.SetType(kLITPIXELHIT);
					    station.AddSubstation(substation);
					    stationSet.insert(station);
					}
				}// end for straws
			} else {
				if (TString(muchNode->GetName()).Contains("station")) {
					TGeoPcon* shape = (TGeoPcon*) muchNode->GetVolume()->GetShape();
					CbmLitStation station;
					CbmLitSubstation substation;
					substation.SetZ(muchPos[2] + shape->GetZ(0) + shape->GetDZ());
					station.SetType(kLITPIXELHIT);
					station.AddSubstation(substation);
					stationSet.insert(station);
				}
			}
		}

		std::vector<CbmLitStation> stationVec(stationSet.begin(), stationSet.end());
		DetermineLayout(stationVec, fMuchLayout);
//		std::cout << fMuchLayout.ToString();
		layoutCreated = true;
	}
}


void CbmLitEnvironment::NewMuchLayout()
{
#ifdef NEWMUCH
	static Bool_t layoutCreated = false;

	if (!layoutCreated) {

		CbmMuchGeoScheme* geoScheme = CbmMuchGeoScheme::Instance();
		FairRuntimeDb* db = FairRuntimeDb::instance();
		CbmGeoMuchPar* geoPar = (CbmGeoMuchPar*) db->getContainer("CbmGeoMuchPar");
		TObjArray* stations = (TObjArray*) geoPar->GetStations();
		geoScheme->Init(stations);
		Int_t nofStations = geoScheme->GetNStations();
		for (Int_t iStation = 0; iStation < nofStations; iStation++){
			CbmMuchStation* station = geoScheme->GetStation(iStation);

			Int_t nofLayers = station->GetNLayers();
			CbmLitStationGroup litStationGroup;
			for (Int_t iLayer = 0; iLayer < nofLayers; iLayer++) {
				CbmMuchLayer* layer = station->GetLayer(iLayer);
				Double_t zFront = layer->GetSideF()->GetZ();
				Double_t zBack = layer->GetSideB()->GetZ();

				CbmLitSubstation litSubstationFront, litSubstationBack;
				litSubstationFront.SetZ(zFront);
				litSubstationBack.SetZ(zBack);

				CbmLitStation litStation;

				Int_t type = layer->GetSideF()->GetModule(0)->GetDetectorType();

				litStation.AddSubstation(litSubstationFront);
				//if (station->IsModuleDesign()) litStation.AddSubstation(litSubstationBack);
				if (station->IsModuleDesign() || type == 2) litStation.AddSubstation(litSubstationBack);

				if (type == 2) litStation.SetType(kLITSTRIPHIT);
				else litStation.SetType(kLITPIXELHIT);

				litStationGroup.AddStation(litStation);
			}
			fMuchLayout.AddStationGroup(litStationGroup);
		}
//		std::cout << fMuchLayout.ToString();
		layoutCreated = true;
	}
#endif
}

void CbmLitEnvironment::TrdLayout()
{
	static Bool_t layoutCreated = false;

	if (!layoutCreated) {
		std::set<CbmLitStation, CompareStationZLess> stationSet;
		TObjArray* nodes = gGeoManager->GetTopNode()->GetNodes();
		for (Int_t iNode = 0; iNode < nodes->GetEntriesFast(); iNode++) {

			TGeoNode* node = (TGeoNode*) nodes->At(iNode);
			if (TString(node->GetName()).Contains("trd")) {

				TGeoNode* station = node;
				const Double_t* stationPos = station->GetMatrix()->GetTranslation();
				TObjArray* layers = station->GetNodes();
				for (Int_t iLayer = 0; iLayer < layers->GetEntriesFast(); iLayer++) {

					TGeoNode* layer = (TGeoNode*) layers->At(iLayer);
					const Double_t* layerPos = layer->GetMatrix()->GetTranslation();

					TObjArray* layerParts;
					if (IsTrdSegmented()) {
						TGeoNode* module = (TGeoNode*) layer->GetNodes()->At(0);
						layerParts = module->GetNodes();
					} else {
						layerParts = layer->GetNodes();
					}
					for (Int_t iLayerPart = 0; iLayerPart < layerParts->GetEntriesFast(); iLayerPart++) {

						TGeoNode* layerPart = (TGeoNode*) layerParts->At(iLayerPart);

						if (TString(layerPart->GetName()).Contains("gas")) {
							const Double_t* pos = layerPart->GetMatrix()->GetTranslation();
							TGeoPgon* shape = (TGeoPgon*) layerPart->GetVolume()->GetShape();
							CbmLitStation sta;
							CbmLitSubstation substation;
							substation.SetZ(stationPos[2] + layerPos[2] + pos[2] - shape->GetDZ());
							sta.SetType(kLITPIXELHIT);
							sta.AddSubstation(substation);
							stationSet.insert(sta);
						}
					}
				}
			}
		}

		std::vector<CbmLitStation> stationVec(stationSet.begin(), stationSet.end());
		DetermineLayout(stationVec, fTrdLayout);
//		std::cout << fTrdLayout.ToString();
		layoutCreated = true;
	}
}

void CbmLitEnvironment::CombineMuchAndTrd()
{
	static Bool_t layoutCreated = false;

	if (!layoutCreated) {
		MuchLayout();
		TrdLayout();

		fMuchTrdLayout = fMuchLayout;

		for(Int_t i = 0; i < fTrdLayout.GetNofStationGroups(); i++){
			fMuchTrdLayout.AddStationGroup(fTrdLayout.GetStationGroup(i));
		}

//		std::cout << fLayout.ToString();
		layoutCreated = true;
	}
}

CbmLitDetectorLayout CbmLitEnvironment::GetTofLayout()
{
	static Bool_t layoutCreated = false;

	if (!layoutCreated) {
		//std::set<CbmLitStation, CompareStationZLess> stationSet;
		CbmLitStation station;
		TObjArray* nodes = gGeoManager->GetTopNode()->GetNodes();
		for (Int_t iNode = 0; iNode < nodes->GetEntriesFast(); iNode++) {

			TGeoNode* tof = (TGeoNode*) nodes->At(iNode);
			if (TString(tof->GetName()).Contains("tof")) {
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
		layoutCreated = true;
	}

	return fTofLayout;
}

CbmLitStation CbmLitEnvironment::GetTofStation()
{
	static Bool_t layoutCreated = false;
	if (!layoutCreated) {
		CbmLitStation station;
		TObjArray* nodes = gGeoManager->GetTopNode()->GetNodes();
		for (Int_t iNode = 0; iNode < nodes->GetEntriesFast(); iNode++) {

			TGeoNode* tof = (TGeoNode*) nodes->At(iNode);
			if (TString(tof->GetName()).Contains("tof")) {
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
		fTofStation = station;
		std::cout << fTofStation.ToString();
		layoutCreated = true;
	}
	return fTofStation;
}

void CbmLitEnvironment::GetMuchLayout(
		LitDetectorLayout& layout)
{
	std::cout << "Getting layout for parallel version of tracking..." << std::endl;
	CbmLitFieldFitter fieldFitter;
	std::cout << "Field fitter initialized" << std::endl;
	CbmLitSimpleGeometryConstructor* geoConstructor = CbmLitSimpleGeometryConstructor::Instance();
	std::cout << "Simple geometry constructor initialized" << std::endl;
	std::vector<CbmLitMaterialInfo> muchMaterial = geoConstructor->GetMyMuchGeoNodes();

	MuchLayout();
	CbmLitDetectorLayout muchLayout = GetMuchLayout();
	std::cout << muchLayout.ToString();
	for (int isg = 0; isg < muchLayout.GetNofStationGroups(); isg++) {
		CbmLitStationGroup stationGroup = muchLayout.GetStationGroup(isg);
		LitStationGroup sg;

		// Add absorber
		// Fit the field at Z front and Z back of the absorber
		int absorberMatId = MaterialId(isg, 0, 0, muchLayout) - 1;
		CbmLitMaterialInfo amat = muchMaterial[absorberMatId];
		double aZ[2] = {amat.GetZpos() - amat.GetLength(), amat.GetZpos()};
		std::vector<double> aparBx[2], aparBy[2], aparBz[2];

		fieldFitter.FitSlice(aZ[0], aparBx[0], aparBy[0], aparBz[0]);
		fieldFitter.FitSlice(aZ[1], aparBx[1], aparBy[1], aparBz[1]);
		sg.absorber.fieldSliceFront.Z = aZ[0];
		sg.absorber.fieldSliceBack.Z = aZ[1];
		for (int i = 0; i < 10; i++) {
			sg.absorber.fieldSliceFront.cx[i] = aparBx[0][i];
			sg.absorber.fieldSliceFront.cy[i] = aparBy[0][i];
			sg.absorber.fieldSliceFront.cz[i] = aparBz[0][i];
			sg.absorber.fieldSliceBack.cx[i] = aparBx[1][i];
			sg.absorber.fieldSliceBack.cy[i] = aparBy[1][i];
			sg.absorber.fieldSliceBack.cz[i] = aparBz[1][i];
		}

		sg.absorber.material.A = amat.GetA();
		sg.absorber.material.Z = amat.GetZ();
		sg.absorber.material.I = (amat.GetZ() > 16)? 10 * amat.GetZ() * 1e-9 :
			16 * std::pow(amat.GetZ(), 0.9) * 1e-9;
		sg.absorber.material.Rho = amat.GetRho();
		sg.absorber.material.Thickness = amat.GetLength();
		sg.absorber.material.X0 = amat.GetRL();
		sg.absorber.material.Zpos = amat.GetZpos();

		sg.absorber.Z = amat.GetZpos();
		//end add absorber


		for (int ist = 0; ist < stationGroup.GetNofStations(); ist++) {
			CbmLitStation station = stationGroup.GetStation(ist);
			LitStation st;
			st.type = station.GetType();
			for(int iss = 0; iss < station.GetNofSubstations(); iss++) {
				CbmLitSubstation substation = station.GetSubstation(iss);
				LitSubstation ss;
				ss.Z = substation.GetZ();

				// Fit the field at Z position of the substation
				std::vector<double> parBx, parBy, parBz;
				ss.fieldSlice.Z = substation.GetZ();
				fieldFitter.FitSlice(substation.GetZ(), parBx, parBy, parBz);
				for (int i = 0; i < 10; i++) {
					ss.fieldSlice.cx[i] = parBx[i];
					ss.fieldSlice.cy[i] = parBy[i];
					ss.fieldSlice.cz[i] = parBz[i];
				}

				int matId = MaterialId(isg, ist, iss, muchLayout);
				CbmLitMaterialInfo mat = muchMaterial[matId];
				ss.material.A = mat.GetA();
				ss.material.Z = mat.GetZ();
				ss.material.I = (mat.GetZ() > 16)? 10 * mat.GetZ() * 1e-9 :
					16 * std::pow(mat.GetZ(), 0.9) * 1e-9;
				ss.material.Rho = mat.GetRho();
				ss.material.Thickness = mat.GetLength();
				ss.material.X0 = mat.GetRL();
				ss.material.Zpos = mat.GetZpos();

				st.AddSubstation(ss);
			} // loop over substations
			sg.AddStation(st);
		} // loop over stations
		layout.AddStationGroup(sg);
	} // loop over station groups
}

int CbmLitEnvironment::MaterialId(
		int stationGroup,
		int station,
		int substation,
		CbmLitDetectorLayout& layout) const
{
	int counter = 0;
	for(int i = 0; i < stationGroup; i++) {
		for(int j = 0; j < layout.GetNofStations(i); j++) {
			counter += layout.GetNofSubstations(i, j);
		}
		counter++; // count for absorber
	}
	counter++;//again count for absorber
	for(int j = 0; j < station; j++) {
		counter += layout.GetNofSubstations(stationGroup, j);
	}
	counter += substation;

	std::cout << "MaterialId: " << stationGroup << " " << station << " " << substation
		<< " " << counter << std::endl;
	return counter;
}

void CbmLitEnvironment::DetermineLayout(
		const std::vector<CbmLitStation>& stations,
		CbmLitDetectorLayout& layout)
{
	Int_t nofStations = stations.size();
	Double_t prev = stations[0].GetSubstation(0).GetZ();
	Int_t groupBegin = 0;
	for (Int_t i = 0; i <= nofStations; i++) {
		if ( i == nofStations ||
				std::abs(stations[i].GetSubstation(0).GetZ() - prev) > 20.) { // new station group
            CbmLitStationGroup stationGroup;

            std::vector<CbmLitStation> subst;
            for (Int_t j = groupBegin; j < i; j++)
            	subst.push_back(stations[j]);

			stationGroup.SetStations(DivideToSubstations(subst));
			layout.AddStationGroup(stationGroup);
			groupBegin = i;
	    }
	    if (i < nofStations) prev = stations[i].GetSubstation(0).GetZ();
	}
}

std::vector<CbmLitStation> CbmLitEnvironment::DivideToSubstations(
		const std::vector<CbmLitStation>& stations)
{
	std::vector<CbmLitStation> subst;
	Int_t nofStations = stations.size();
	Double_t prev = stations[0].GetSubstation(0).GetZ();
	Int_t groupBegin = 0;
	for (Int_t i = 0; i <= nofStations; i++) {
		if (i == nofStations ||
				std::abs(stations[i].GetSubstation(0).GetZ() - prev) > 1.0) { // new substations
			CbmLitStation st;
	        CbmLitSubstation sst;

	       	for (Int_t j = groupBegin; j < i; j++){
	       		sst.SetZ(stations[j].GetSubstation(0).GetZ());
	       		st.AddSubstation(sst);
	       		st.SetType(stations[j].GetType());
	       	}
			subst.push_back(st);
			groupBegin = i;
	    }
		if (i < nofStations) prev = stations[i].GetSubstation(0).GetZ();
	}
	return subst;
}

bool CbmLitEnvironment::IsStraw() const
{
	FairRunAna* ana = FairRunAna::Instance();
	FairRuntimeDb* rtdb = ana->GetRuntimeDb();
	FairBaseParSet* baseParSet = (FairBaseParSet*) rtdb->getContainer("FairBaseParSet");
	TObjArray *detList = baseParSet->GetDetList();
	FairDetector* much = (FairDetector*) detList->FindObject("MUCH");
	TString name = much->GetGeometryFileName();
	if(name.Contains("straw")) {
		//std::cout << "-I- TrdLayout :" << "STRAWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" << std::endl;
		return true;
	} else {
		return false;
	}
}

bool CbmLitEnvironment::IsTrdSegmented() const
{
	FairRunAna* ana = FairRunAna::Instance();
	FairRuntimeDb* rtdb = ana->GetRuntimeDb();
	FairBaseParSet* baseParSet = (FairBaseParSet*) rtdb->getContainer("FairBaseParSet");
	TObjArray *detList = baseParSet->GetDetList();
	FairDetector* much = (FairDetector*) detList->FindObject("TRD");
	TString name = much->GetGeometryFileName();
	if(name.Contains("monolithic")) {
		return false;
	} else {
		return true;
	}
}

bool CbmLitEnvironment::CheckDetectorPresence(
		const std::string& name) const
{
	TObjArray* nodes = gGeoManager->GetTopNode()->GetNodes();
	for (Int_t iNode = 0; iNode < nodes->GetEntriesFast(); iNode++) {
		TGeoNode* node = (TGeoNode*) nodes->At(iNode);
		if (TString(node->GetName()).Contains(name.c_str())) return true;
	}
	return false;
}

bool CbmLitEnvironment::IsElectronSetup() const
{
	return CheckDetectorPresence("rich");
}

bool CbmLitEnvironment::IsTrd() const
{
	return CheckDetectorPresence("trd");
}

bool CbmLitEnvironment::IsMuch() const
{
	return CheckDetectorPresence("much");
}

bool CbmLitEnvironment::IsTof() const
{
	return CheckDetectorPresence("tof");
}
