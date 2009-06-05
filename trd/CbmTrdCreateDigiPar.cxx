#include "CbmTrdCreateDigiPar.h"

#include "CbmTrdModule.h"
#include "CbmTrdDigiPar.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"

#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TGeoMaterial.h"
#include "TGeoNode.h"
#include "TGeoMatrix.h"
#include "TGeoBBox.h"
#include "TPRegexp.h"
#if ROOT_VERSION_CODE < ROOT_VERSION(5,20,0)
#include "CbmTrdStringToken.h"
#endif

#include <iostream>
using std::cout;
using std::endl;

// ---- Default constructor -------------------------------------------
CbmTrdCreateDigiPar::CbmTrdCreateDigiPar()
    :FairTask("TrdDigitizer")
	//:fRef(0)
{

}
// --------------------------------------------------------------------

// ---- Constructor ----------------------------------------------------
CbmTrdCreateDigiPar::CbmTrdCreateDigiPar(const char *name, const char *title)
	:FairTask(name)
{

}
// --------------------------------------------------------------------

// ---- Destructor ----------------------------------------------------
CbmTrdCreateDigiPar::~CbmTrdCreateDigiPar()
{

}
// --------------------------------------------------------------------

// ----  Initialisation  ----------------------------------------------
void CbmTrdCreateDigiPar::SetParContainers()
{
    cout<<" * CbmTrdCreateDigiPar * :: SetParContainers() "<<endl;


    // Get Base Container
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb=ana->GetRuntimeDb();

    fDigiPar = (CbmTrdDigiPar*)
               (rtdb->getContainer("CbmTrdDigiPar"));

}
// --------------------------------------------------------------------

// ---- ReInit  -------------------------------------------------------
InitStatus CbmTrdCreateDigiPar::ReInit(){

  cout<<" * CbmTrdCreateDigiPar * :: ReInit() "<<endl;


  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();

  fDigiPar = (CbmTrdDigiPar*)
      (rtdb->getContainer("CbmTrdDigiPar"));
  
  return kSUCCESS;
}
// --------------------------------------------------------------------

// ---- Init ----------------------------------------------------------
InitStatus CbmTrdCreateDigiPar::Init()
{

    cout<<" * CbmTrdCreateDigiPar * :: Init() "<<endl;

    FairRootManager *ioman = FairRootManager::Instance();
    if ( ! ioman ) Fatal("Init", "No FairRootManager");
    
    FillModuleMap();

    return kSUCCESS;

}
// --------------------------------------------------------------------


// ---- Exec ----------------------------------------------------------
void CbmTrdCreateDigiPar::Exec(Option_t * option)
{

}
// --------------------------------------------------------------------


void CbmTrdCreateDigiPar::GetModuleInformation()
{

  // Extract the information about station, layer, module type
  // and cpoy number of the module from the full path to the
  // node.
  // The full path is tokenized at the "/" which diide the different
  // levels of the geometry.
  // Knowing the nameing scheme of the volumes one gets the required
  // information with simple string manipulation.
  // This is probably not the fastes way, but the speed has to be checked.
  // The methode works only for versions of Root > 5.20.0, before the
  // class TStringTocken is not implemented


  TString path = gGeoManager->GetPath();
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,20,0)
  TStringToken* bla = new TStringToken(path,"/");
#else
  CbmTrdStringToken* bla = new CbmTrdStringToken(path,"/");
#endif

  while (bla->NextToken()) {
    if (bla->Contains("layer")) {
      TString bla3 = (TString) *bla;
      Ssiz_t pos = bla3.Last('_');
      Ssiz_t substringLength=bla3.Length()-pos-1;
      TString bla2 = bla3((bla3.Last('_')+1),substringLength);
      TString bla1 = bla3(3,1);
      fStation=bla1.Atoi();
      fLayer=bla2.Atoi();
    }
    if (bla->Contains("mod")){
      TString bla3 = (TString) *bla;
      Ssiz_t pos = bla3.Last('_');
      Ssiz_t substringLength=bla3.Length()-pos-1;
      TString bla2 = bla3(pos+1,substringLength);
      substringLength=pos-7;
      TString bla1 = bla3(7,substringLength);     
      fModuleType = bla1.Atoi();
      fModuleCopy = bla2.Atoi();
      break;
    } 
  }

}

// --------------------------------------------------------------------

void CbmTrdCreateDigiPar::CalculateModuleId() {

  Int_t sector = 0;
  Int_t detInfo_array[6]={kTRD, fStation,fLayer,fModuleType,fModuleCopy,0};      
  fModuleID = fTrdDetId.SetDetectorInfo(detInfo_array);

}

// --------------------------------------------------------------------

void CbmTrdCreateDigiPar::FillModuleMap(){
 
  Int_t nmodules = 0;
  TString TopNode = gGeoManager->GetTopNode()->GetName();
  TObjArray* nodes = gGeoManager->GetTopNode()->GetNodes();
  for (Int_t iNode = 0; iNode < nodes->GetEntriesFast(); iNode++) {
    TGeoNode* node = (TGeoNode*) nodes->At(iNode);
    if (TString(node->GetName()).Contains("trd")) {
      TString StationNode = node->GetName();
      TGeoNode* station = node;
      TObjArray* layers = station->GetNodes();
      for (Int_t iLayer = 0; iLayer < layers->GetEntriesFast(); iLayer++) {
        TGeoNode* layer = (TGeoNode*) layers->At(iLayer);
        TString LayerNode = layer->GetName();
	TObjArray* modules = layer->GetNodes();
	for (Int_t iLayerPart = 0; iLayerPart < modules->GetEntriesFast(); iLayerPart++) {
          TGeoNode* module = (TGeoNode*) modules->At(iLayerPart);
          TString ModuleNode = module->GetName();
          TObjArray* parts = module->GetNodes();
	  for (Int_t iPart = 0; iPart < parts->GetEntriesFast(); iPart++) {
            TGeoNode* part = (TGeoNode*) parts->At(iPart);
            if (TString(part->GetName()).Contains("gas")) {
              TString PartNode = part->GetName();
              TString FullPath = "/" + TopNode + "/" + StationNode + "/" + LayerNode + "/" + ModuleNode + "/" + PartNode;
              gGeoManager->cd(FullPath.Data());

              TGeoVolume *curvol = gGeoManager->GetCurrentVolume();
              TGeoBBox *shape = (TGeoBBox*)curvol->GetShape(); 
              Float_t sizex = shape->GetDX();
              Float_t sizey = shape->GetDY();
              Float_t sizez = shape->GetDZ();
	      GetModuleInformation();
              CalculateModuleId();
	      Double_t local[3] = {0., 0., 0.};  // Local centre of volume
              Double_t global[3];                // Global centre of volume
              gGeoManager->LocalToMaster(local, global);
              TGeoHMatrix *matrix = gGeoManager->GetCurrentMatrix(); 
              Double_t x = global[0];
              Double_t y = global[1];
              Double_t z = global[2] + sizez; //Padplane is not at local z=0 but at the end of the volume
              fIsRotated = matrix->IsRotation();
              
              //Orientation of the detector layers
              //Layer 1 and 3 have resolution in x-direction (Orientation=1)
              //Layer 2 and 4 have resolution in y-direction (Orientation=0)
              Int_t Orientation=fLayer%2;

              if( 1 == Orientation) {
                fpadsizex = 0.5;
                fpadsizey = 2;
              } else {
                fpadsizex = 2.;
                fpadsizey = 0.5;
              }
              nmodules++;

              // Create new CbmTrdModule and add it to the map
	      fModuleMap[fModuleID] = new CbmTrdModule(fModuleID, x, y, z, sizex, sizey, fpadsizex, fpadsizey, fIsRotated);
	    }
	  }

	}
      }
    }
  }


  Int_t Nrmodules = (Int_t)fModuleMap.size();
  cout <<"Nr. of modules: "<<Nrmodules<<endl;

  fDigiPar->SetNrOfModules(Nrmodules);

  TArrayI *ModuleId  = new TArrayI(Nrmodules);
  TArrayI *ModuleRot = new TArrayI(Nrmodules);
  
  Int_t iDigi=0; 
  for ( fModuleMapIt=fModuleMap.begin() ; fModuleMapIt != fModuleMap.end(); fModuleMapIt++ ){
    ModuleId->AddAt(fModuleMapIt->second->GetDetectorId(),iDigi);
    ModuleRot->AddAt(fModuleMapIt->second->IsRotated(),iDigi);
    iDigi++;
  }  

  fDigiPar->SetModuleIdArray(*ModuleId);
  fDigiPar->SetModuleRotArray(*ModuleRot);
  fDigiPar->SetModuleMap(fModuleMap);

}

ClassImp(CbmTrdCreateDigiPar)
