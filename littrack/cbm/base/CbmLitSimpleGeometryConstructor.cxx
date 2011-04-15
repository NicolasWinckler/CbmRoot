#include "base/CbmLitSimpleGeometryConstructor.h"

#include "base/CbmLitEnvironment.h"
#include "utils/CbmLitComparators.h"

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

CbmLitSimpleGeometryConstructor::CbmLitSimpleGeometryConstructor():
   fGeo(NULL),
   fSimpleGeo(NULL),
   fMedium(),
   fMyGeoNodes(),
   fMyMuchGeoNodes(),
   fMyTrdGeoNodes(),
   fIsTrd(false),
   fIsMuch(false),
   fIsTof(false)
{
   ConstructGeometry();
}

CbmLitSimpleGeometryConstructor::~CbmLitSimpleGeometryConstructor()
{
   if (fInstance != NULL) { delete fInstance; }
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
   fGeo->Print();

   CbmLitEnvironment* env = CbmLitEnvironment::Instance();
   fIsTrd = env->IsTrd();
   fIsMuch = env->IsMuch();
   fIsTof = env->IsTof();

   CreateMediumList();
   std::cout << "Medium list created" << std::endl;

   gGeoManager = 0;

   // Create new simplified geometry
   fSimpleGeo = new TGeoManager("FAIRSimpleGeom", "Simplified geometry");
   TGeoVolume* topVolume = fSimpleGeo->MakeBox("cave", fMedium["air"], 20000., 20000., 20000.);
   fSimpleGeo->SetTopVolume(topVolume);


   ConstructSts();
   if (fIsMuch) { ConstructMuch(); }
   if (fIsTrd) { ConstructTrd(); }
   if (fIsTof) { ConstructTof(); }

   fSimpleGeo->CloseGeometry();
   fSimpleGeo->Print();
   fSimpleGeo->CheckOverlaps(1e-7,"SAME");
   fSimpleGeo->PrintOverlaps();
   fSimpleGeo->Write();

   std::sort(fMyGeoNodes.begin(), fMyGeoNodes.end(), CompareMaterialInfoZLess());
   std::sort(fMyMuchGeoNodes.begin(), fMyMuchGeoNodes.end(), CompareMaterialInfoZLess());
   std::sort(fMyTrdGeoNodes.begin(), fMyTrdGeoNodes.end(), CompareMaterialInfoZLess());

   std::cout << "My Simple Geometry:" << std::endl;
   for (size_t i = 0; i < fMyGeoNodes.size(); ++i) {
      std::cout << i << " " << fMyGeoNodes[i].ToString();
   }

// std::cout << "My MUCH Simple Geometry:" << std::endl;
//    for (size_t i = 0; i < fMyMuchGeoNodes.size(); ++i)
//       std::cout << i << " " << fMyMuchGeoNodes[i].ToString();
//
// std::cout << "My TRD Simple Geometry:" << std::endl;
//    for (size_t i = 0; i < fMyTrdGeoNodes.size(); ++i)
//       std::cout << i << " " << fMyTrdGeoNodes[i].ToString();

   gGeoManager = fGeo;
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

void CbmLitSimpleGeometryConstructor::GeoMediumToMaterialInfo(
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
   CbmLitEnvironment* env = CbmLitEnvironment::Instance();

   fMedium["air"] = CreateMedium("air");
   fMedium["silicon"] = CreateMedium("silicon");

   if (fIsMuch) {
      fMedium["MUCHiron"] = CreateMedium("MUCHiron");
      fMedium["MUCHargon"] = CreateMedium("MUCHargon");
   }
   if (fIsTrd) {
      fMedium["polypropylene"] = CreateMedium("polypropylene");
      fMedium["TRDgas"] = CreateMedium("TRDgas");
      fMedium["goldcoatedcopper"] = CreateMedium("goldcoatedcopper");
      fMedium["mylar"] = CreateMedium("mylar");
      fMedium["carbon"] = CreateMedium("carbon");
   }
   if (fIsTof) {
      fMedium["aluminium"] = CreateMedium("aluminium");
      fMedium["RPCgas"] = CreateMedium("RPCgas");
      fMedium["RPCglass"] = CreateMedium("RPCglass");
   }

}

void CbmLitSimpleGeometryConstructor::ConstructSts()
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
      std::stringstream name;
      name << "sts" << i;

      TGeoShape* shape = new TGeoCone( station->GetD() / 2.,   station->GetRmin(),
                                       station->GetRmax(), station->GetRmin(),   station->GetRmax());

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
                                         sh->GetRmin2(),   sh->GetRmax2());

         TGeoMedium* medium = fMedium[med->GetName()];

         myf Z = muchPos[2] + muchNode->GetMatrix()->GetTranslation()[2];

         TGeoVolume* volume = new TGeoVolume(muchNode->GetName(), shape, medium);
         TGeoMatrix* matrix = new TGeoTranslation(0, 0, Z);
         fSimpleGeo->GetTopVolume()->AddNode(volume, 0, matrix);

         CbmLitMaterialInfo litMaterial;
         litMaterial.SetLength(2. * sh->GetDz());
         litMaterial.SetZpos(Z + sh->GetDz());
         GeoMediumToMaterialInfo(medium, litMaterial);
         fMyGeoNodes.push_back(litMaterial);
         fMyMuchGeoNodes.push_back(litMaterial);
      }

      if (TString(muchNode->GetName()).Contains("station")) {
         TObjArray* layerNodes = muchNode->GetNodes();
         for (int iLayerNode = 0; iLayerNode < layerNodes->GetEntriesFast(); iLayerNode++) {
            TGeoNode* sideNode = (TGeoNode*) layerNodes->At(iLayerNode);

            bool bactive = false, factive = false;
            for (int iSideNode = 0; iSideNode < sideNode->GetNodes()->GetEntriesFast(); iSideNode++) {
               TGeoNode* active = (TGeoNode*) sideNode->GetNodes()->At(iSideNode);
               if (TString(active->GetName()).Contains("active")) {
                  if (bactive && TString(active->GetName()).Contains("bactive")) { continue; }
                  if (factive && TString(active->GetName()).Contains("factive")) { continue; }

                  TGeoBBox* sh = (TGeoBBox*) active->GetVolume()->GetShape();
                  TGeoMedium* med = active->GetVolume()->GetMedium();

                  TGeoShape* shape = new TGeoCone(sh->GetDZ(), 0., 300.,   0.,   300.);

                  TGeoMedium* medium = fMedium[med->GetName()];

                  TGeoVolume* volume = new TGeoVolume(active->GetName(), shape, medium);
                  myf z = muchPos[2] + muchNode->GetMatrix()->GetTranslation()[2]
                          + sideNode->GetMatrix()->GetTranslation()[2] + active->GetMatrix()->GetTranslation()[2];
                  std::cout << std::cout.precision(5) << "z station MUCH: " << z << std::endl;
                  TGeoMatrix* matrix = new TGeoTranslation(0, 0, z);
                  fSimpleGeo->GetTopVolume()->AddNode(volume, 0, matrix);

                  CbmLitMaterialInfo litMaterial;
                  litMaterial.SetLength(2. * sh->GetDZ());
                  litMaterial.SetZpos(z);
                  GeoMediumToMaterialInfo(medium, litMaterial);
                  fMyGeoNodes.push_back(litMaterial);
                  fMyMuchGeoNodes.push_back(litMaterial);

                  bactive = TString(active->GetName()).Contains("bactive");
                  factive = TString(active->GetName()).Contains("factive");

                  if (factive && bactive) { break; }
               }
            }
         }
      }
   }
   std::cout << "-I- Construction of the MUCH geometry finished" << std::endl;
}

void CbmLitSimpleGeometryConstructor::ConstructTrd()
{
   // NEW TRD GEOMETRY (w/o layers)

   std::cout << "-I- Construction of the TRD geometry started" << std::endl;
   std::string trds[] = {"trd1_0", "trd2_0", "trd3_0"};
   std::string mods[][4] = {
      "trd1mod1_1001", "trd1mod1_2001", "trd1mod1_3001", "trd1mod1_4001",
      "trd2mod2_1001", "trd2mod2_2001", "trd2mod2_3001", "trd2mod2_4001",
      "trd3mod3_1001", "trd3mod3_2001", "trd3mod3_3001", "trd3mod3_4001"
   };

   for (int iTrds = 0; iTrds < 3; iTrds++) {
      TGeoNode* trd = (TGeoNode*) fGeo->GetTopNode()->GetNodes()->FindObject(trds[iTrds].c_str());

      for (int iTrdModule = 0; iTrdModule < 4; iTrdModule++) {
         TGeoNode* module = (TGeoNode*) trd->GetNodes()->FindObject(mods[iTrds][iTrdModule].c_str());
         TObjArray* parts = module->GetNodes();
         for (int iPart = 0; iPart < parts->GetEntriesFast(); iPart++) {
            TGeoNode* part = (TGeoNode*) parts->At(iPart);
            if (!TString(part->GetName()).Contains("frame")) {

               TGeoBBox* sh = (TGeoBBox*) part->GetVolume()->GetShape();
               TGeoMedium* med = part->GetVolume()->GetMedium();

               TGeoShape* shape = new TGeoCone(sh->GetDZ(), 0., 500.,   0.,   500.);
               TGeoMedium* medium = fMedium[med->GetName()];

               TGeoVolume* volume = new TGeoVolume(part->GetName(), shape, medium);
               myf z = trd->GetMatrix()->GetTranslation()[2] +
                       module->GetMatrix()->GetTranslation()[2] +
                       part->GetMatrix()->GetTranslation()[2];

               TGeoMatrix* matrix = new TGeoTranslation(0, 0, z);
               fSimpleGeo->GetTopVolume()->AddNode(volume, 0, matrix);

               CbmLitMaterialInfo litMaterial;
               litMaterial.SetLength(2. * sh->GetDZ());
               litMaterial.SetZpos(z);
               GeoMediumToMaterialInfo(medium, litMaterial);
//             std::cout << "mm: " << litMaterial.ToString();
//             std::cout << medium->GetName() << std::endl;
               fMyGeoNodes.push_back(litMaterial);
               fMyTrdGeoNodes.push_back(litMaterial);
            }
         }
         // add air
         CbmLitMaterialInfo litMaterial;
         float ml = (iTrdModule == 3)? 150. : 10.;
//       if (iTrdModule == 3) litMaterial.SetLength(200.);
//       else litMaterial.SetLength(10.);
         litMaterial.SetLength(ml);
         CbmLitMaterialInfo lastmat = fMyGeoNodes.back();
         litMaterial.SetZpos(lastmat.GetZpos() + lastmat.GetLength() + ml);
         GeoMediumToMaterialInfo(fMedium["air"], litMaterial);
         fMyGeoNodes.push_back(litMaterial);
         fMyTrdGeoNodes.push_back(litMaterial);
      }
   }
   std::cout << "-I- Construction of the TRD geometry finished" << std::endl;
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

   myf Z = tofPos[2] + gasPos[2] + modPos[2] + celPos[2];

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
}
