/** CbmLitEnvironment.cxx
 *@author A.Lebedev <alebedev@jinr.ru>
 *@since 2007
 **/

#include "base/CbmLitFieldFitter.h"
#include "base/CbmLitFieldGridCreator.h"
#include "base/CbmLitTrackingGeometryConstructor.h"
#include "base/CbmLitEnvironment.h"
#include "base/CbmLitDetectorSetup.h"
#include "utils/CbmLitMemoryManagment.h"
#include "utils/CbmLitComparators.h"

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
#include "TGeoTube.h"

#include <set>
#include <functional>
#include <algorithm>
#include <cmath>
#include <cassert>

using std::cout;
using std::endl;

CbmLitEnvironment::CbmLitEnvironment():
   fField(NULL)
{
}

CbmLitEnvironment::~CbmLitEnvironment()
{
}

CbmLitEnvironment* CbmLitEnvironment::Instance()
{
   static CbmLitEnvironment instance;
   return &instance;
}

FairField* CbmLitEnvironment::GetField()
{
   if (fField == NULL) {
      FairRunAna* run = FairRunAna::Instance();
      assert(run != NULL);
      fField = static_cast<FairField*>(run->GetField());
   }
   return fField;
}

//const CbmLitDetectorLayout& CbmLitEnvironment::GetLayout()
//{
//   cout << "-I- CbmLitEnvironment::GetLayout : " << "Getting layout... " << endl;
//   static Bool_t layoutCreated = false;
//   if (!layoutCreated) {
//      CbmLitDetectorSetup det;
//      det.DetermineSetup();
//
//      if (det.GetDet(kMUCH) && !det.GetDet(kTRD)) {
//         MuchLayout();
//         fLayout = fMuchLayout;
//      } else if (det.GetDet(kTRD) && !det.GetDet(kMUCH)) {
//         TrdLayout();
//         fLayout = fTrdLayout;
//      } else if (det.GetDet(kMUCH) && det.GetDet(kTRD)) {
//         CombineMuchAndTrd();
//         fLayout = fMuchTrdLayout;
//      }
//      cout << fLayout.ToString();
//      layoutCreated = true;
//   }
//   cout << "-I- CbmLitEnvironment::GetLayout : " << "Finish getting layout" << endl;
//   return fLayout;
//}

//const CbmLitDetectorLayout& CbmLitEnvironment::GetMvdLayout()
//{
//   cout << "-I- CbmLitEnvironment::GetMvdLayout : " << "Getting layout... " << endl;
//   static Bool_t layoutCreated = false;
//   if (!layoutCreated) {
//      MvdLayout();
//      cout << fMvdLayout.ToString();
//      layoutCreated = true;
//   }
//   return fMvdLayout;
//}
//
//void CbmLitEnvironment::MuchLayout()
//{
//   static Bool_t layoutCreated = false;
//   if (!layoutCreated) {
//	  TGeoMatrix* matrix = new TGeoRotation(); // needed to define rotation matrix for module
//      CbmMuchGeoScheme* geoScheme = CbmMuchGeoScheme::Instance();
//      FairRuntimeDb* db = FairRuntimeDb::instance();
//      CbmGeoMuchPar* geoPar = (CbmGeoMuchPar*) db->getContainer("CbmGeoMuchPar");
//      TObjArray* stations = (TObjArray*) geoPar->GetStations();
//      geoScheme->Init(stations);
//      Int_t nofStations = geoScheme->GetNStations();
//      for (Int_t iStation = 0; iStation < nofStations; iStation++) {
//         CbmMuchStation* station = geoScheme->GetStation(iStation);
//
//         Int_t nofLayers = station->GetNLayers();
//         CbmLitStationGroup LitStationGroupMuon;
//         for (Int_t iLayer = 0; iLayer < nofLayers; iLayer++) {
//            CbmMuchLayer* layer = station->GetLayer(iLayer);
//            Double_t zFront = layer->GetSideF()->GetZ();// - geoScheme->GetGetActiveLz();
//            Double_t zBack = layer->GetSideB()->GetZ();// - layer->GetSupportDz();
////            std::cout << ">> sideF=" << layer->GetSideF()->GetZ() << " sideB=" << layer->GetSideB()->GetZ()
////            		<< " moduleF=" << layer->GetSide(false)->GetModule(0)->GetPosition()[2]
////            		<< " moduleB=" << layer->GetSide(true)->GetModule(0)->GetPosition()[2] << "\n";
//
//            CbmLitSubstation litSubstationFront, litSubstationBack;
//            litSubstationFront.AddModule(0, matrix);
//            litSubstationFront.SetZ(zFront);
//            litSubstationBack.AddModule(0, matrix);
//            litSubstationBack.SetZ(zBack);
//
//            CbmLitStation LitStationMuon;
//
//            Int_t type = layer->GetSideF()->GetModule(0)->GetDetectorType();
//
//            LitStationMuon.AddSubstation(litSubstationFront);
//            if (station->IsModuleDesign() || type == 2) { LitStationMuon.AddSubstation(litSubstationBack); }
//
//            if (type == 2) { LitStationMuon.SetType(kLITSTRIPHIT); }
//            else { LitStationMuon.SetType(kLITPIXELHIT); }
//
//            LitStationGroupMuon.AddStation(LitStationMuon);
//         }
//         fMuchLayout.AddStationGroup(LitStationGroupMuon);
//      }
////    cout << fMuchLayout.ToString();
//      layoutCreated = true;
//   }
//}

//void CbmLitEnvironment::TrdLayout()
//{
//   static Bool_t layoutCreated = false;
//   if (!layoutCreated) {
//	  fTrdLayout = CbmLitTrackingGeometryConstructor::Instance()->GetTrdLayout();
//      fTrdLayout.SetGeo(CbmLitTrackingGeometryConstructor::Instance()->GetTrdTrackingGeo());
//      //cout << fTrdLayout.ToString();
//      layoutCreated = true;
//   }
//}

//void CbmLitEnvironment::CombineMuchAndTrd()
//{
//   static Bool_t layoutCreated = false;
//   if (!layoutCreated) {
//      MuchLayout();
//      TrdLayout();
//      fMuchTrdLayout = fMuchLayout;
//      for(Int_t i = 0; i < fTrdLayout.GetNofStationGroups(); i++) {
//         fMuchTrdLayout.AddStationGroup(fTrdLayout.GetStationGroup(i));
//      }
////    cout << fLayout.ToString();
//      layoutCreated = true;
//   }
//}

//const CbmLitDetectorLayout& CbmLitEnvironment::GetTofLayout()
//{
//   static Bool_t layoutCreated = false;
//
//   if (!layoutCreated) {
//      //std::set<CbmLitStation, CompareStationZLess> stationSet;
//      CbmLitStation station;
//      TObjArray* nodes = gGeoManager->GetTopNode()->GetNodes();
//      for (Int_t iNode = 0; iNode < nodes->GetEntriesFast(); iNode++) {
//
//         TGeoNode* tof = (TGeoNode*) nodes->At(iNode);
//         if (TString(tof->GetName()).Contains("tof")) {
//            const Double_t* tofPos = tof->GetMatrix()->GetTranslation();
//
//            TGeoNode* gas = (TGeoNode*)tof->GetNodes()->At(0);
//            const Double_t* gasPos = gas->GetMatrix()->GetTranslation();
//
//            TGeoNode* reg = (TGeoNode*)gas->GetNodes()->At(0);
//            const Double_t* regPos = reg->GetMatrix()->GetTranslation();
//
//            TGeoBBox* shape = (TGeoBBox*) reg->GetVolume()->GetShape();
//
//            CbmLitSubstation substation;
//            substation.SetZ(tofPos[2] + gasPos[2] + regPos[2] + shape->GetDZ());// - shape->GetDZ());
//            station.SetType(kLITPIXELHIT);
//            station.AddSubstation(substation);
//         }
//      }
//      CbmLitStationGroup stationGroup;
//      stationGroup.AddStation(station);
//      fTofLayout.AddStationGroup(stationGroup);
//      cout << fTofLayout.ToString();
//      layoutCreated = true;
//   }
//
//   return fTofLayout;
//}
//
//const CbmLitStation& CbmLitEnvironment::GetTofStation()
//{
//   static Bool_t layoutCreated = false;
//   if (!layoutCreated) {
//      CbmLitStation station;
//      TObjArray* nodes = gGeoManager->GetTopNode()->GetNodes();
//      for (Int_t iNode = 0; iNode < nodes->GetEntriesFast(); iNode++) {
//
//         TGeoNode* tof = (TGeoNode*) nodes->At(iNode);
//         if (TString(tof->GetName()).Contains("tof")) {
//            const Double_t* tofPos = tof->GetMatrix()->GetTranslation();
//
//            TGeoNode* gas = (TGeoNode*)tof->GetNodes()->At(0);
//            const Double_t* gasPos = gas->GetMatrix()->GetTranslation();
//
//            TGeoNode* reg = (TGeoNode*)gas->GetNodes()->At(0);
//            const Double_t* regPos = reg->GetMatrix()->GetTranslation();
//
//            TGeoBBox* shape = (TGeoBBox*) reg->GetVolume()->GetShape();
//
//            CbmLitSubstation substation;
//            substation.SetZ(tofPos[2] + gasPos[2] + regPos[2] + shape->GetDZ());// - shape->GetDZ());
//            station.SetType(kLITPIXELHIT);
//            station.AddSubstation(substation);
//         }
//      }
//      fTofStation = station;
//      cout << fTofStation.ToString();
//      layoutCreated = true;
//   }
//   return fTofStation;
//}

//void CbmLitEnvironment::MvdLayout()
//{
//   static Bool_t layoutCreated = false;
//
//   if (!layoutCreated) {
//
//      std::set<CbmLitStation, CompareStationZLess> stationSet;
//      TGeoRotation* matrix = new TGeoRotation();
//      TGeoNode* node1 = gGeoManager->GetTopVolume()->FindNode("pipevac1_0");
//      TObjArray* nodes = node1->GetNodes();
//      for (Int_t iNode = 0; iNode < nodes->GetEntriesFast(); iNode++) {
//         TGeoNode* node = (TGeoNode*) nodes->At(iNode);
//         if (TString(node->GetName()).Contains("mvdstation")) {
//            TGeoNode* station = node;
//            const Double_t* stationPos = station->GetMatrix()->GetTranslation();
//            //TGeoTubeSeg* shape = (TGeoTubeSeg*) station->GetVolume()->GetShape();
//
//            CbmLitStation sta;
//            CbmLitSubstation substation;
//            substation.AddModule(0, matrix);
//            substation.SetZ(stationPos[2]);
//            sta.SetType(kLITPIXELHIT);
//            sta.AddSubstation(substation);
//            stationSet.insert(sta);
//         }
//      }
//      CbmLitStationGroup sg;
//      std::set<CbmLitStation, CompareStationZLess>::reverse_iterator it;
//      for (it = stationSet.rbegin(); it != stationSet.rend(); it++) {
//         sg.AddStation(*it);
//      }
//      fMvdLayout.AddStationGroup(sg);
//      cout << fMvdLayout.ToString();
//      layoutCreated = true;
//   }
//}
