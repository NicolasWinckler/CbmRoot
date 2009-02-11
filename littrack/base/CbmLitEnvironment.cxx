// -------------------------------------------------------------------------
// -----                  CbmLitEnvironment source file               -----
// -----                  Created 16/07/07  by A. Lebedev               -----
// -------------------------------------------------------------------------

#define NEWMUCH 1

#include "CbmLitEnvironment.h"
#include "CbmLitMemoryManagment.h"
#include "CbmLitComparators.h"

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
#endif

#include "TGeoNode.h"
#include "TGeoMaterial.h"
#include "TGeoManager.h"
#include "TGeoPcon.h"
#include "TGeoPgon.h"
#include "TGeoArb8.h"
#include "TGeoSphere.h"
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

CbmLitDetectorLayout CbmLitEnvironment::GetMuchLayout()
{
	if ((TGeoNode*) gGeoManager->GetTopNode()->GetNodes()->FindObject("much1_0") != NULL){
		std::cout << "OLD MUCH GEOMETRY!!!" << std::endl;
		return GetOldMuchLayout();
	} else {
		std::cout << "NEW MUCH GEOMETRY!!!" << std::endl;
		return GetNewMuchLayout();
	}
}

CbmLitDetectorLayout CbmLitEnvironment::GetOldMuchLayout()
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
		std::cout << fMuchLayout.ToString();
		layoutCreated = true;
	}
	return fMuchLayout;
}


CbmLitDetectorLayout CbmLitEnvironment::GetNewMuchLayout()
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
				litStation.AddSubstation(litSubstationFront);
				litStation.AddSubstation(litSubstationBack);

				litStationGroup.AddStation(litStation);
			}
			fMuchLayout.AddStationGroup(litStationGroup);
		}
		std::cout << fMuchLayout.ToString();
		layoutCreated = true;
	}
	return fMuchLayout;
#endif
}


CbmLitDetectorLayout CbmLitEnvironment::GetTrdLayout()
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
							CbmLitStation station;
							CbmLitSubstation substation;
							substation.SetZ(stationPos[2] + layerPos[2] + pos[2] - shape->GetDZ());
							station.SetType(kLITPIXELHIT);
							station.AddSubstation(substation);
							stationSet.insert(station);
						}
					}
				}
			}
		}

		std::vector<CbmLitStation> stationVec(stationSet.begin(), stationSet.end());
//		for (int i = 0; i < stationVec.size(); i++ )
//			std::cout << stationVec[i].ToString();
		DetermineLayout(stationVec, fTrdLayout);
		std::cout << fTrdLayout.ToString();
		layoutCreated = true;
	}

	return fTrdLayout;
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

ClassImp(CbmLitEnvironment)
