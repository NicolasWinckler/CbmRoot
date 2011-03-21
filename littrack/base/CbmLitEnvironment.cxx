/** CbmLitEnvironment.cxx
 *@author A.Lebedev <alebedev@jinr.ru>
 *@since 2007
 **/

#include "base/CbmLitFieldFitter.h"
#include "base/CbmLitSimpleGeometryConstructor.h"
#include "base/CbmLitEnvironment.h"
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

CbmLitEnvironment* CbmLitEnvironment::fInstance = NULL;

CbmLitEnvironment::CbmLitEnvironment():
   fField(NULL)
{
}

CbmLitEnvironment::~CbmLitEnvironment()
{
   if (fInstance != NULL) { delete fInstance; }
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
      FairRunAna* Run = FairRunAna::Instance();

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
   std::cout << "-I- CbmLitEnvironment::GetLayout : "
             << "Getting layout... " << std::endl;

   static Bool_t layoutCreated = false;

   if (!layoutCreated) {
//    if (IsTrd()) {
      if (IsTrdSimple()) {
         TrdLayoutSimple();
         fLayout = fTrdLayout;
//       }
      } else if (IsMuch() && !IsTrd()) {
         MuchLayout();
         fLayout = fMuchLayout;
      } else if (IsTrd() && !IsMuch()) {
         TrdLayout();
         fLayout = fTrdLayout;
      } else if (IsMuch() && IsTrd()) {
         CombineMuchAndTrd();
         fLayout = fMuchTrdLayout;
      }
      std::cout << fLayout.ToString();
      layoutCreated = true;
   }

   std::cout << "-I- CbmLitEnvironment::GetLayout : "
             << "Finish getting layout" << std::endl;

   return fLayout;
}

const CbmLitDetectorLayout& CbmLitEnvironment::GetMvdLayout()
{
   std::cout << "-I- CbmLitEnvironment::GetMvdLayout : "
             << "Getting layout... " << std::endl;

   static Bool_t layoutCreated = false;
   if (!layoutCreated) {
      MvdLayout();
      std::cout << fMvdLayout.ToString();
      layoutCreated = true;
   }
   return fMvdLayout;
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
      for (Int_t iStation = 0; iStation < nofStations; iStation++) {
         CbmMuchStation* station = geoScheme->GetStation(iStation);

         Int_t nofLayers = station->GetNLayers();
         CbmLitStationGroup LitStationGroupMuon;
         for (Int_t iLayer = 0; iLayer < nofLayers; iLayer++) {
            CbmMuchLayer* layer = station->GetLayer(iLayer);
            Double_t zFront = layer->GetSideF()->GetZ();
            Double_t zBack = layer->GetSideB()->GetZ();

            CbmLitSubstation litSubstationFront, litSubstationBack;
            litSubstationFront.SetZ(zFront);
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
//    std::cout << fMuchLayout.ToString();
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
            TGeoNode* t = (TGeoNode*) layers->At(0);

            // OLD TRD GEOMETRY
            if (TString(t->GetName()).Contains("layer")) {
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
               // END: OLD TRD GEOMETRY
            } else {

               // NEW TRD GEOMETRY
               for (Int_t iModule = 0; iModule < layers->GetEntriesFast(); iModule++) {

                  TGeoNode* module = (TGeoNode*) layers->At(iModule);
                  const Double_t* modulePos = module->GetMatrix()->GetTranslation();

                  TObjArray* moduleParts = module->GetNodes();
                  for (Int_t iModulePart = 0; iModulePart < moduleParts->GetEntriesFast(); iModulePart++) {

                     TGeoNode* modulePart = (TGeoNode*) moduleParts->At(iModulePart);

                     if (TString(modulePart->GetName()).Contains("gas")) {
                        const Double_t* pos = modulePart->GetMatrix()->GetTranslation();
                        TGeoPgon* shape = (TGeoPgon*) modulePart->GetVolume()->GetShape();
                        CbmLitStation sta;
                        CbmLitSubstation substation;
                        substation.SetZ(stationPos[2] + modulePos[2] + pos[2] + shape->GetDZ());
                        sta.SetType(kLITPIXELHIT);
                        sta.AddSubstation(substation);
                        stationSet.insert(sta);
                     }
                  }
               }

               // END: NEW TRD GEOMETRY

            }
         }
      }

      std::vector<CbmLitStation> stationVec(stationSet.begin(), stationSet.end());
      DetermineLayout(stationVec, fTrdLayout);
//    std::cout << fTrdLayout.ToString();
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

//          const Double_t* pos = layerPart->GetMatrix()->GetTranslation();
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
//    std::cout << fTrdLayout.ToString();
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

      for(Int_t i = 0; i < fTrdLayout.GetNofStationGroups(); i++) {
         fMuchTrdLayout.AddStationGroup(fTrdLayout.GetStationGroup(i));
      }

//    std::cout << fLayout.ToString();
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

void CbmLitEnvironment::MvdLayout()
{
   static Bool_t layoutCreated = false;

   if (!layoutCreated) {

      std::set<CbmLitStation, CompareStationZLess> stationSet;
      TGeoNode* node1 = gGeoManager->GetTopVolume()->FindNode("pipevac1_0");
      TObjArray* nodes = node1->GetNodes();
      for (Int_t iNode = 0; iNode < nodes->GetEntriesFast(); iNode++) {
         TGeoNode* node = (TGeoNode*) nodes->At(iNode);
         if (TString(node->GetName()).Contains("mvdstation")) {
            TGeoNode* station = node;
            const Double_t* stationPos = station->GetMatrix()->GetTranslation();
            //TGeoTubeSeg* shape = (TGeoTubeSeg*) station->GetVolume()->GetShape();

            CbmLitStation sta;
            CbmLitSubstation substation;
            substation.SetZ(stationPos[2]);
            sta.SetType(kLITPIXELHIT);
            sta.AddSubstation(substation);
            stationSet.insert(sta);
         }
      }
      CbmLitStationGroup sg;
      for (std::set<CbmLitStation, CompareStationZLess>::reverse_iterator i = stationSet.rbegin(); i != stationSet.rend(); i++) {
         sg.AddStation(*i);
      }
      fMvdLayout.AddStationGroup(sg);
//    std::cout << fMvdLayout.ToString();
      layoutCreated = true;
   }
}

void CbmLitEnvironment::GetMuchLayoutVec(
   LitDetectorLayoutMuonVec& layout)
{
   GetMuchLayout(layout);
}

void CbmLitEnvironment::GetMuchLayoutScal(
   LitDetectorLayoutMuonScal& layout)
{
   GetMuchLayout(layout);
}

template<class T>
void CbmLitEnvironment::GetMuchLayout(
   LitDetectorLayoutMuon<T>& layout)
{
   std::cout << "Getting MUCH layout for parallel version of tracking..." << std::endl;
#if LIT_POLYNOM_DEGREE==3
   CbmLitFieldFitter fieldFitter(3); // set polynom degree
   static const unsigned int N = 10; // set number of coefficients
#else
#if LIT_POLYNOM_DEGREE==5
   CbmLitFieldFitter fieldFitter(5); // set polynom degree
   static const unsigned int N = 21; // set number of coefficients
#else
#if LIT_POLYNOM_DEGREE==7
   CbmLitFieldFitter fieldFitter(7); // set polynom degree
   static const unsigned int N = 36; // set number of coefficients
#else
#if LIT_POLYNOM_DEGREE==9
   CbmLitFieldFitter fieldFitter(9); // set polynom degree
   static const unsigned int N = 55; // set number of coefficients
#endif
#endif
#endif
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
      LitStationGroupMuon<T> sg;

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
         const CbmLitStation& station = stationGroup.GetStation(ist);
         LitStationMuon<T> st;
         st.type = station.GetType();
         for(int iss = 0; iss < station.GetNofSubstations(); iss++) {
            const CbmLitSubstation& substation = station.GetSubstation(iss);
            LitSubstationMuon<T> ss;
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

void CbmLitEnvironment::GetTrdLayoutVec(
   LitDetectorLayoutElectronVec& layout)
{
   GetTrdLayout(layout);
}

void CbmLitEnvironment::GetTrdLayoutScal(
   LitDetectorLayoutElectronScal& layout)
{
   GetTrdLayout(layout);
}

template<class T>
void CbmLitEnvironment::GetTrdLayout(
   LitDetectorLayoutElectron<T>& layout)
{
   std::cout << "Getting MUCH layout for parallel version of tracking..." << std::endl;
#if LIT_POLYNOM_DEGREE==3
   CbmLitFieldFitter fieldFitter(3); // set polynom degree
   static const unsigned int N = 10; // set number of coefficients
#else
#if LIT_POLYNOM_DEGREE==5
   CbmLitFieldFitter fieldFitter(5); // set polynom degree
   static const unsigned int N = 21; // set number of coefficients
#else
#if LIT_POLYNOM_DEGREE==7
   CbmLitFieldFitter fieldFitter(7); // set polynom degree
   static const unsigned int N = 36; // set number of coefficients
#else
#if LIT_POLYNOM_DEGREE==9
   CbmLitFieldFitter fieldFitter(9); // set polynom degree
   static const unsigned int N = 55; // set number of coefficients
#endif
#endif
#endif
#endif
   std::cout << "Field fitter initialized" << std::endl;
   CbmLitSimpleGeometryConstructor* geoConstructor = CbmLitSimpleGeometryConstructor::Instance();
   std::cout << "Simple geometry constructor initialized" << std::endl;
   std::vector<CbmLitMaterialInfo> trdMaterial = geoConstructor->GetMyTrdGeoNodes();

   TrdLayout();
   const CbmLitDetectorLayout& trdLayout = GetTrdLayout();
   std::cout << trdLayout.ToString();

   // add virtual planes
   for (int nvp = 0; nvp < 20; nvp++) {
      LitVirtualPlaneElectron<T> virtualPlane;
      float DZ = 10.;
      float Z = 105. + nvp * DZ;
      virtualPlane.Z = Z;
      // Fit the field at Z position of the substation
      std::vector<double> parBx, parBy, parBz;
      virtualPlane.fieldSlice.Z = virtualPlane.Z;
      fieldFitter.FitSlice(Z, parBx, parBy, parBz);
      for (int i = 0; i < N; i++) {
         virtualPlane.fieldSlice.cx[i] = parBx[i];
         virtualPlane.fieldSlice.cy[i] = parBy[i];
         virtualPlane.fieldSlice.cz[i] = parBz[i];
      }
      // Fit the field at Z between two virtual planes
      virtualPlane.fieldSlice.Z = virtualPlane.Z + DZ/2.;
      fieldFitter.FitSlice(Z+DZ/2., parBx, parBy, parBz);
      for (int i = 0; i < N; i++) {
         virtualPlane.fieldSliceMid.cx[i] = parBx[i];
         virtualPlane.fieldSliceMid.cy[i] = parBy[i];
         virtualPlane.fieldSliceMid.cz[i] = parBz[i];
      }

      CbmLitMaterialInfo mat = trdMaterial[5]; //air material
      LitMaterialInfo<T> m;
      m.A = mat.GetA();
      m.Z = mat.GetZ();
      m.I = (mat.GetZ() > 16)? 10 * mat.GetZ() * 1e-9 :
            16 * std::pow(mat.GetZ(), 0.9) * 1e-9;
      m.Rho = mat.GetRho();
      m.Thickness = DZ;//mat.GetLength();
      m.X0 = mat.GetRL();
      m.Zpos = Z + DZ;//mat.GetZpos();

      m.RadThick = m.Thickness / m.X0; // Length/X0
      m.SqrtRadThick = sqrt(m.RadThick); // std::sqrt(Length/X0)
      m.LogRadThick = log(m.RadThick); // std::log(Length/X0)
      m.ElLoss = exp(m.RadThick * log(3.) / log (2.)) - exp(-2. * m.RadThick);

      virtualPlane.material = m;

      layout.AddVirtualPlane(virtualPlane);
   }
   //

   for (int isg = 0; isg < trdLayout.GetNofStationGroups(); isg++) {
      const CbmLitStationGroup& stationGroup = trdLayout.GetStationGroup(isg);
      LitStationGroupElectron<T> sg;

      for (int ist = 0; ist < stationGroup.GetNofStations(); ist++) {
         const CbmLitStation& station = stationGroup.GetStation(ist);

         for(int iss = 0; iss < station.GetNofSubstations(); iss++) {
            const CbmLitSubstation& substation = station.GetSubstation(iss);
            LitStationElectron<T> st;
            st.Z = substation.GetZ();

            int matId = TrdMaterialId(isg, ist, trdLayout);
            for (int im = 0; im < 6; im++) {
               CbmLitMaterialInfo mat = trdMaterial[matId + im];
               LitMaterialInfo<T> m;
               m.A = mat.GetA();
               m.Z = mat.GetZ();
               m.I = (mat.GetZ() > 16)? 10 * mat.GetZ() * 1e-9 :
                     16 * std::pow(mat.GetZ(), 0.9) * 1e-9;
               m.Rho = mat.GetRho();
               m.Thickness = mat.GetLength();
               m.X0 = mat.GetRL();
               m.Zpos = mat.GetZpos();

               m.RadThick = m.Thickness / m.X0; // Length/X0
               m.SqrtRadThick = sqrt(m.RadThick); // std::sqrt(Length/X0)
               m.LogRadThick = log(m.RadThick); // std::log(Length/X0)
               m.ElLoss = exp(m.RadThick * log(3.) / log (2.)) - exp(-2. * m.RadThick);

               if (im < 3) { st.AddMaterialBefore(m); }
               else { st.AddMaterialAfter(m); }
            }

            sg.AddStation(st);
         } // loop over substations
      } // loop over stations
      layout.AddStationGroup(sg);
   } // loop over station groups
}

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

int CbmLitEnvironment::TrdMaterialId(
   int stationGroup,
   int station,
   const CbmLitDetectorLayout& layout) const
{
   const int nofMaterialsInLayer = 6;
   int counter = 0;
   for(int i = 0; i < stationGroup; i++) {
      for(int j = 0; j < layout.GetNofStations(i); j++) {
         counter += nofMaterialsInLayer;
      }
   }

   for(int j = 0; j < station; j++) {
      counter += nofMaterialsInLayer;
   }

   std::cout << "TrdMaterialId: " << stationGroup << " "
             << station << " " << counter << std::endl;
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
         for (Int_t j = groupBegin; j < i; j++) {
            subst.push_back(stations[j]);
         }

         stationGroup.SetStations(DivideToSubstations(subst));
         layout.AddStationGroup(stationGroup);
         groupBegin = i;
      }
      if (i < nofStations) { prev = stations[i].GetSubstation(0).GetZ(); }
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

         for (Int_t j = groupBegin; j < i; j++) {
            sst.SetZ(stations[j].GetSubstation(0).GetZ());
            st.AddSubstation(sst);
            st.SetType(stations[j].GetType());
         }
         subst.push_back(st);
         groupBegin = i;
      }
      if (i < nofStations) { prev = stations[i].GetSubstation(0).GetZ(); }
   }
   return subst;
}

bool CbmLitEnvironment::IsStraw() const
{
   FairRunAna* ana = FairRunAna::Instance();
   FairRuntimeDb* rtdb = ana->GetRuntimeDb();
   FairBaseParSet* baseParSet = (FairBaseParSet*) rtdb->getContainer("FairBaseParSet");
   TObjArray* detList = baseParSet->GetDetList();
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
   TObjArray* detList = baseParSet->GetDetList();
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
   TObjArray* detList = baseParSet->GetDetList();
   FairDetector* trd = (FairDetector*) detList->FindObject("TRD");
   if (!trd) { return false; }
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
      if (TString(node->GetName()).Contains(name.c_str())) { return true; }
   }

   if (name == "mvd") {
      TGeoNode* node1 = gGeoManager->GetTopVolume()->FindNode("pipevac1_0");
      if (node1 && node1->GetVolume()->FindNode("mvdstation01_0")) { return true; }
   }

   return false;
}

bool CbmLitEnvironment::IsElectronSetup() const
{
   return CheckDetectorPresence("rich");// || CheckDetectorPresence("trd");
}

bool CbmLitEnvironment::IsMvd() const
{
   return CheckDetectorPresence("mvd");
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
