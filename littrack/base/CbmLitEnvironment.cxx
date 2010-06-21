/** CbmLitEnvironment.cxx
 *@author A.Lebedev <alebedev@jinr.ru>
 *@since 2007
 **/

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
#include "CbmMuchGeoScheme.h"
#include "CbmGeoMuchPar.h"
#include "CbmMuchStation.h"
#include "CbmMuchLayer.h"
#include "CbmMuchLayerSide.h"
#include "CbmMuchModule.h"

#include "TGeoNode.h"
#include "TGeoMaterial.h"
#include "TGeoManager.h"
#include "TGeoPcon.h"
#include "TGeoPgon.h"
#include "TGeoArb8.h"
#include "TGeoSphere.h"
#include "TGeoBBox.h"
#include "TGeoCone.h"
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

const CbmLitDetectorLayout& CbmLitEnvironment::GetLayout()
{
	static Bool_t layoutCreated = false;

	if (!layoutCreated) {
//		if (IsTrd()) {
			if (IsTrdSimple()) {
				TrdLayoutSimple();
				fLayout = fTrdLayout;
//			}
		} else
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

					Double_t Zmodule = 0.;
					TObjArray* layerParts;
					if (IsTrdSegmented()) {
						TGeoNode* module = (TGeoNode*) layer->GetNodes()->At(0);
						layerParts = module->GetNodes();
						const Double_t* modPos = module->GetMatrix()->GetTranslation();
						Zmodule = modPos[2];
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
							substation.SetZ(stationPos[2] + layerPos[2] + pos[2] + shape->GetDZ() + Zmodule);
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

void CbmLitEnvironment::TrdLayoutSimple()
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

//				const Double_t* pos = layerPart->GetMatrix()->GetTranslation();
				TGeoCone* shape = (TGeoCone*) station->GetVolume()->GetShape();
				CbmLitStation sta;
				CbmLitSubstation substation;
				substation.SetZ(stationPos[2] - shape->GetDz());
				sta.SetType(kLITPIXELHIT);
				sta.AddSubstation(substation);
				stationSet.insert(sta);
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

const CbmLitDetectorLayout& CbmLitEnvironment::GetTofLayout()
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

const CbmLitStation& CbmLitEnvironment::GetTofStation()
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

void CbmLitEnvironment::GetMuchLayoutVec(
		LitDetectorLayoutVec& layout)
{
	GetMuchLayout(layout);
}

void CbmLitEnvironment::GetMuchLayoutScal(
		LitDetectorLayoutScal& layout)
{
	GetMuchLayout(layout);
}

template<class T>
void CbmLitEnvironment::GetMuchLayout(
		LitDetectorLayout<T>& layout)
{
	std::cout << "Getting MUCH layout for parallel version of tracking..." << std::endl;
#ifdef LIT_USE_THIRD_DEGREE
	CbmLitFieldFitter fieldFitter(3); // set polynom degree
	static const unsigned int N = 10; // set number of coefficients
#else
	CbmLitFieldFitter fieldFitter(5); // set polynom degree
	static const unsigned int N = 21; // set number of coefficients
#endif
	std::cout << "Field fitter initialized" << std::endl;
	CbmLitSimpleGeometryConstructor* geoConstructor = CbmLitSimpleGeometryConstructor::Instance();
	std::cout << "Simple geometry constructor initialized" << std::endl;
	std::vector<CbmLitMaterialInfo> muchMaterial = geoConstructor->GetMyMuchGeoNodes();

	MuchLayout();
	const CbmLitDetectorLayout& muchLayout = GetMuchLayout();
	std::cout << muchLayout.ToString();
	for (int isg = 0; isg < muchLayout.GetNofStationGroups(); isg++) {
		const CbmLitStationGroup& stationGroup = muchLayout.GetStationGroup(isg);
		LitStationGroup<T> sg;

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
		for (int i = 0; i < N; i++) {
			sg.absorber.fieldSliceFront.cx[i] = aparBx[0][i];
			sg.absorber.fieldSliceFront.cy[i] = aparBy[0][i];
			sg.absorber.fieldSliceFront.cz[i] = aparBz[0][i];
			sg.absorber.fieldSliceBack.cx[i] = aparBx[1][i];
			sg.absorber.fieldSliceBack.cy[i] = aparBy[1][i];
			sg.absorber.fieldSliceBack.cz[i] = aparBz[1][i];
		}

		LitMaterialInfo<T>& mat1 = sg.absorber.material;
		mat1.A = amat.GetA();
		mat1.Z = amat.GetZ();
		mat1.I = (amat.GetZ() > 16)? 10 * amat.GetZ() * 1e-9 :
			16 * std::pow(amat.GetZ(), 0.9) * 1e-9;
		mat1.Rho = amat.GetRho();
		mat1.Thickness = amat.GetLength();
		mat1.X0 = amat.GetRL();
		mat1.Zpos = amat.GetZpos();

		sg.absorber.Z = amat.GetZpos();

		mat1.RadThick = mat1.Thickness / mat1.X0; // Length/X0
		mat1.SqrtRadThick = sqrt(mat1.RadThick); // std::sqrt(Length/X0)
		mat1.LogRadThick = log(mat1.RadThick); // std::log(Length/X0)
		//end add absorber


		for (int ist = 0; ist < stationGroup.GetNofStations(); ist++) {
			CbmLitStation station = stationGroup.GetStation(ist);
			LitStation<T> st;
			st.type = station.GetType();
			for(int iss = 0; iss < station.GetNofSubstations(); iss++) {
				CbmLitSubstation substation = station.GetSubstation(iss);
				LitSubstation<T> ss;
				ss.Z = substation.GetZ();

				// Fit the field at Z position of the substation
				std::vector<double> parBx, parBy, parBz;
				ss.fieldSlice.Z = substation.GetZ();
				fieldFitter.FitSlice(substation.GetZ(), parBx, parBy, parBz);
				for (int i = 0; i < N; i++) {
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

				ss.material.RadThick = ss.material.Thickness / ss.material.X0; // Length/X0
				ss.material.SqrtRadThick = sqrt(ss.material.RadThick); // std::sqrt(Length/X0)
				ss.material.LogRadThick = log(ss.material.RadThick); // std::log(Length/X0)

				st.AddSubstation(ss);
			} // loop over substations
			sg.AddStation(st);
		} // loop over stations
		layout.AddStationGroup(sg);
	} // loop over station groups
}


//template<class T>
//void CbmLitEnvironment::TrdLayoutParallel(
//		LitDetectorLayout<T>& layout)
//{
//	std::cout << "Getting TRD layout for parallel version of tracking..." << std::endl;
//#ifdef LIT_USE_THIRD_DEGREE
//	CbmLitFieldFitter fieldFitter(3); // set polynom degree
//	static const unsigned int N = 10; // set number of coefficients
//#else
//	CbmLitFieldFitter fieldFitter(5); // set polynom degree
//	static const unsigned int N = 21; // set number of coefficients
//#endif
//	std::cout << "Field fitter initialized" << std::endl;
//	CbmLitSimpleGeometryConstructor* geoConstructor = CbmLitSimpleGeometryConstructor::Instance();
//	std::cout << "Simple geometry constructor initialized" << std::endl;
//	std::vector<CbmLitMaterialInfo> trdMaterial = geoConstructor->GetMyTrdGeoNodes();
//
//	TrdLayout();
//	const CbmLitDetectorLayout& trdLayout = GetTrdLayout();
//	std::cout << trdLayout.ToString();
//	for (int isg = 0; isg < trdLayout.GetNofStationGroups(); isg++) {
//		const CbmLitStationGroup& stationGroup = trdLayout.GetStationGroup(isg);
//		LitStationGroup<T> sg;
//
//		for (int ist = 0; ist < stationGroup.GetNofStations(); ist++) {
//			const CbmLitStation& station = stationGroup.GetStation(ist);
//			LitStation<T> st;
//			st.type = station.GetType();
//			for(int iss = 0; iss < station.GetNofSubstations(); iss++) {
//				const CbmLitSubstation& substation = station.GetSubstation(iss);
//				LitSubstation<T> ss;
//				ss.Z = substation.GetZ();
//
//				// Fit the field at Z position of the substation
//				std::vector<double> parBx, parBy, parBz;
//				ss.fieldSlice.Z = substation.GetZ();
//				fieldFitter.FitSlice(substation.GetZ(), parBx, parBy, parBz);
//				for (int i = 0; i < N; i++) {
//					ss.fieldSlice.cx[i] = parBx[i];
//					ss.fieldSlice.cy[i] = parBy[i];
//					ss.fieldSlice.cz[i] = parBz[i];
//				}
//
//				int matId = MaterialId(isg, ist, iss, muchLayout);
//				CbmLitMaterialInfo mat = muchMaterial[matId];
//				ss.material.A = mat.GetA();
//				ss.material.Z = mat.GetZ();
//				ss.material.I = (mat.GetZ() > 16)? 10 * mat.GetZ() * 1e-9 :
//					16 * std::pow(mat.GetZ(), 0.9) * 1e-9;
//				ss.material.Rho = mat.GetRho();
//				ss.material.Thickness = mat.GetLength();
//				ss.material.X0 = mat.GetRL();
//				ss.material.Zpos = mat.GetZpos();
//
//				ss.material.RadThick = ss.material.Thickness / ss.material.X0; // Length/X0
//				ss.material.SqrtRadThick = sqrt(ss.material.RadThick); // std::sqrt(Length/X0)
//				ss.material.LogRadThick = log(ss.material.RadThick); // std::log(Length/X0)
//
//				st.AddSubstation(ss);
//			} // loop over substations
//			sg.AddStation(st);
//		} // loop over stations
//		layout.AddStationGroup(sg);
//	} // loop over station groups
//}

int CbmLitEnvironment::MaterialId(
		int stationGroup,
		int station,
		int substation,
		const CbmLitDetectorLayout& layout) const
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
	FairDetector* trd = (FairDetector*) detList->FindObject("TRD");
	TString name = trd->GetGeometryFileName();
	if(name.Contains("monolithic") && !name.Contains("monolithic_new")) {
		return false;
	} else {
		return true;
	}
}

bool CbmLitEnvironment::IsTrdSimple() const
{
	FairRunAna* ana = FairRunAna::Instance();
	FairRuntimeDb* rtdb = ana->GetRuntimeDb();
	FairBaseParSet* baseParSet = (FairBaseParSet*) rtdb->getContainer("FairBaseParSet");
	TObjArray *detList = baseParSet->GetDetList();
	FairDetector* trd = (FairDetector*) detList->FindObject("TRD");
	if (!trd) return false;
	TString name = trd->GetGeometryFileName();
	if(name.Contains("simple")) {
		return true;
	} else {
		return false;
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
	return CheckDetectorPresence("rich");// || CheckDetectorPresence("trd");
}

bool CbmLitEnvironment::IsSts() const
{
	return CheckDetectorPresence("sts");
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
