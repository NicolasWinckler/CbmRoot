/// 
/// \file CbmTrdCreateDigiPar.cxx
/// \brief Assign pad layout to TRD Modules.
/// 
/// Crate digitisation parameter map.
/// Read pad geometry from CbmTrdPads.h,
/// assign pad layout to sectors in TRD modules.
///  

#include "CbmTrdCreateDigiPar.h"

#include "CbmTrdPads.h"
#include "CbmTrdPadsMap.h"
#include "CbmTrdModule.h"
#include "CbmTrdGeoHandler.h"
#include "CbmTrdDigiPar.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"

#include <iostream>
using std::cout;
using std::endl;

// ---- Default constructor -------------------------------------------
CbmTrdCreateDigiPar::CbmTrdCreateDigiPar()
  :FairTask("TrdCreateDigiPar"),
   fStation(-1),
   fLayer(-1),
   fModuleType(-1),
   fModuleCopy(-1),
   fSector(-1),
   fSizex(-1.),
   fSizey(-1.),
   fSizez(-1.),
   fX(-1.),
   fY(-1.),   
   fZ(-1.),
   fpadsizex(0), 
   fpadsizey(0), 
   fsizex(-1.),    
   fsizey(-1.),    
   fSectorSizex(0),
   fSectorSizey(0),
   fCol(-1),     
   fRow(-1),     
   fModuleID(-1),
   fMaxSectors(0),
   fPosX(-1.),    
   fPosY(-1.),    
   fModuleMap(), 
   fModuleMapIt(),
   fDigiPar(NULL),
   fGeoHandler(new CbmTrdGeoHandler()),
   fModInfoMap(),
   fModTypeMap(),
   fModuleTypeVector()
{
  // Get the maximum number of sectors. All arrays will have
  // this number of entries.  
  fMaxSectors=TMath::Max(fst1_sect_count,fst2_sect_count); 
  fMaxSectors=TMath::Max(fMaxSectors,fst3_sect_count); 

  fSectorSizex.Set(fMaxSectors);
  fSectorSizey.Set(fMaxSectors);
  fpadsizex.Set(fMaxSectors);
  fpadsizey.Set(fMaxSectors);
}
// --------------------------------------------------------------------

// ---- Constructor ----------------------------------------------------
CbmTrdCreateDigiPar::CbmTrdCreateDigiPar(const char *name, const char *title)
  :FairTask(name),
   fStation(-1),
   fLayer(-1),
   fModuleType(-1),
   fModuleCopy(-1),
   fSector(-1),
   fSizex(-1.),
   fSizey(-1.),
   fSizez(-1.),
   fX(-1.),
   fY(-1.),   
   fZ(-1.),
   fpadsizex(0), 
   fpadsizey(0), 
   fsizex(-1.),    
   fsizey(-1.),    
   fSectorSizex(0),
   fSectorSizey(0),
   fCol(-1),     
   fRow(-1),     
   fModuleID(-1),
   fMaxSectors(0),
   fPosX(-1.),    
   fPosY(-1.),    
   fModuleMap(), 
   fModuleMapIt(),
   fDigiPar(NULL),
   fGeoHandler(new CbmTrdGeoHandler()),
   fModInfoMap(),
   fModTypeMap(),
   fModuleTypeVector()
{
  // Get the maximum number of sectors. All arrays will have
  // this number of entries.  
  fMaxSectors=TMath::Max(fst1_sect_count,fst2_sect_count); 
  fMaxSectors=TMath::Max(fMaxSectors,fst3_sect_count); 

  fSectorSizex.Set(fMaxSectors);
  fSectorSizey.Set(fMaxSectors);
  fpadsizex.Set(fMaxSectors);
  fpadsizey.Set(fMaxSectors);
}
// --------------------------------------------------------------------

// ---- Destructor ----------------------------------------------------
CbmTrdCreateDigiPar::~CbmTrdCreateDigiPar()
{

}
// --------------------------------------------------------------------

// ----  Initialisation  ----------------------------------------------
void CbmTrdCreateDigiPar::SetParContainers(){

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

  // Get Base Container
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();

  fDigiPar = (CbmTrdDigiPar*)
             (rtdb->getContainer("CbmTrdDigiPar"));
  
  return kSUCCESS;
}
// --------------------------------------------------------------------

// ---- Init ----------------------------------------------------------
InitStatus CbmTrdCreateDigiPar::Init(){

  Int_t geoVersion = fGeoHandler->Init();

  fModInfoMap = GetModuleInfoMap();
  cout<<" * CbmTrdCreateDigiPar * :: Init() "<<endl;

  FairRootManager *ioman = FairRootManager::Instance();
  if ( ! ioman ) Fatal("Init", "No FairRootManager");
  

  if (kNewMonolithic == geoVersion) {
    fLogger->Fatal(MESSAGE_ORIGIN,"There is no support to create digitization parameters for this geometry.");
  }
  if (kQuasiMonolithic == geoVersion) {
    fLogger->Fatal(MESSAGE_ORIGIN,"There is no support to create digitization parameters for this geometry.");
  }
  if (kSegmentedRectangular == geoVersion) {
    fLogger->Fatal(MESSAGE_ORIGIN,"There is no support to create digitization parameters for this geometry.");
  }
  if (kRootGeom == geoVersion) {
    fLogger->Info(MESSAGE_ORIGIN,"Will now create digitization parameters for this geometry.");
    FillModuleMapRootGeometry();
  }
  if (kSegmentedSquaredOneKeepingVolume == geoVersion) {
    fLogger->Info(MESSAGE_ORIGIN,"Will now create digitization parameters for this geometry.");
    FillModuleMapSegmentedSquaredOneKeepingVolume();
  }
  if (kSegmentedSquared == geoVersion) {
    fLogger->Info(MESSAGE_ORIGIN,"Will now create digitization parameters for this geometry.");
    FillModuleMapSegmentedSquared();
  }

  return kSUCCESS;
}
// --------------------------------------------------------------------
void CbmTrdCreateDigiPar::FinishTask(){

  cout<<" * CbmTrdCreateDigiPar * :: FinishTask() "<<endl;

  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();

  fDigiPar = (CbmTrdDigiPar*)
             (rtdb->getContainer("CbmTrdDigiPar"));

  fDigiPar->print();
}

// ---- Exec ----------------------------------------------------------
void CbmTrdCreateDigiPar::Exec(Option_t * option)
{
}

// --------------------------------------------------------------------
void CbmTrdCreateDigiPar::FillModuleMapRootGeometry(){
 
  // The geometry structure is treelike with cave as
  // the top node. For the TRD there are keeping volumes
  // trd1-trd4 for each station which are only containers 
  // for the different layers of one station. The trdlayer
  // is again only a container for all volumes of this layer.   
  // Loop over all nodes below the top node (cave). If one of
  // the nodes containes a string trd it must be one of the
  // stations. Now loop over the layers of this station and 
  // then over all modules of the layer to extract in the end
  // all active regions (gas) of the complete TRD. For each
  // of the gas volumes get the information about size and
  // position from the geomanager and the sizes of the sectors
  // and pads from the definitions in CbmTrdPads. This info
  // is then stored in a TrdModule object for each of the
  // TRD modules.

  Int_t nmodules = 0;
  TString TopNode = gGeoManager->GetTopNode()->GetName();

  TObjArray* nodes = gGeoManager->GetTopNode()->GetNodes();
  for (Int_t iNode = 0; iNode < nodes->GetEntriesFast(); iNode++) {
    TGeoNode* node = (TGeoNode*) nodes->At(iNode);
    if (TString(node->GetName()).Contains("trd")) {   // trd_vXXy top node, e.g. trd_v13a, trd_v14b
      TString StationNode = node->GetName();
      TGeoNode* station = node;

	TObjArray* modules = station->GetNodes();
	for (Int_t iLayerPart = 0; iLayerPart < modules->GetEntriesFast(); iLayerPart++) {
          TGeoNode* module = (TGeoNode*) modules->At(iLayerPart);
          TString ModuleNode = module->GetName();

          TObjArray* parts = module->GetNodes();
	  for (Int_t iPart = 0; iPart < parts->GetEntriesFast(); iPart++) {
            TGeoNode* part = (TGeoNode*) parts->At(iPart);
            if (TString(part->GetName()).Contains("gas")) {
              TString PartNode = part->GetName();

              // Put together the full path to the interesting volume, which
	      // is needed to navigate with the geomanager to this volume.
              // Extract the geometry information (size, global position)
              // from this volume.;

              TString FullPath = "/" + TopNode + "/" + StationNode + "/" + 
                                 ModuleNode + "/" + PartNode;

              FillModuleInfoFromGeoHandler(FullPath);

              // Get Information about the padstructure for a
              // given trd module defined by the station and
              // layer numbers, the module type and the copy
              // number
              FillPadInfoSegmentedSquaredOneKeepingVolume();

              CorrectOrientationOfPadPlane();

              nmodules++;

              // Create new CbmTrdModule and add it to the map
	      fModuleMap[fModuleID] = 
                new CbmTrdModule(fModuleID, fX, fY, fZ, fSizex, fSizey, fSizez,
				 fMaxSectors, fSectorSizex, fSectorSizey, 
				 fpadsizex, fpadsizey);
	    }
	  }
      }
    }
  }

  FillDigiPar();
}

// --------------------------------------------------------------------
void CbmTrdCreateDigiPar::FillModuleMapSegmentedSquaredOneKeepingVolume(){
 
  // The geometry structure is treelike with cave as
  // the top node. For the TRD there are keeping volumes
  // trd1-trd4 for each station which are only containers 
  // for the different layers of one station. The trdlayer
  // is again only a container for all volumes of this layer.   
  // Loop over all nodes below the top node (cave). If one of
  // the nodes containes a string trd it must be one of the
  // stations. Now loop over the layers of this station and 
  // then over all modules of the layer to extract in the end
  // all active regions (gas) of the complete TRD. For each
  // of the gas volumes get the information about size and
  // position from the geomanager and the sizes of the sectors
  // and pads from the definitions in CbmTrdPads. This info
  // is then stored in a TrdModule object for each of the
  // TRD modules.

  Int_t nmodules = 0;
  TString TopNode = gGeoManager->GetTopNode()->GetName();

  TObjArray* nodes = gGeoManager->GetTopNode()->GetNodes();
  for (Int_t iNode = 0; iNode < nodes->GetEntriesFast(); iNode++) {
    TGeoNode* node = (TGeoNode*) nodes->At(iNode);
    if (TString(node->GetName()).Contains("trd")) {
      TString StationNode = node->GetName();
      TGeoNode* station = node;

	TObjArray* modules = station->GetNodes();
	for (Int_t iLayerPart = 0; iLayerPart < modules->GetEntriesFast(); iLayerPart++) {
          TGeoNode* module = (TGeoNode*) modules->At(iLayerPart);
          TString ModuleNode = module->GetName();

          TObjArray* parts = module->GetNodes();
	  for (Int_t iPart = 0; iPart < parts->GetEntriesFast(); iPart++) {
            TGeoNode* part = (TGeoNode*) parts->At(iPart);
            if (TString(part->GetName()).Contains("gas")) {
              TString PartNode = part->GetName();

              // Put together the full path to the interesting volume, which
	      // is needed to navigate with the geomanager to this volume.
              // Extract the geometry information (size, global position)
              // from this volume.;

              TString FullPath = "/" + TopNode + "/" + StationNode + "/" + 
                                 ModuleNode + "/" + PartNode;

              FillModuleInfoFromGeoHandler(FullPath);

              // Get Information about the padstructure for a
              // given trd module defined by the station and
              // layer numbers, the module type and the copy
              // number
              FillPadInfoSegmentedSquaredOneKeepingVolume();

              CorrectOrientationOfPadPlane();

              nmodules++;

              // Create new CbmTrdModule and add it to the map
	      fModuleMap[fModuleID] = 
                new CbmTrdModule(fModuleID, fX, fY, fZ, fSizex, fSizey, fSizez,
				 fMaxSectors, fSectorSizex, fSectorSizey, 
				 fpadsizex, fpadsizey);
	    }
	  }
      }
    }
  }

  FillDigiPar();
}

// --------------------------------------------------------------------
void CbmTrdCreateDigiPar::FillModuleMapSegmentedSquared(){
 
  // The geometry structure is treelike with cave as
  // the top node. For the TRD there are keeping volumes
  // trd1-trd4 for each station which are only containers 
  // for the different layers of one station. The trdlayer
  // is again only a container for all volumes of this layer.   
  // Loop over all nodes below the top node (cave). If one of
  // the nodes containes a string trd it must be one of the
  // stations. Now loop over the layers of this station and 
  // then over all modules of the layer to extract in the end
  // all active regions (gas) of the complete TRD. For each
  // of the gas volumes get the information about size and
  // position from the geomanager and the sizes of the sectors
  // and pads from the definitions in CbmTrdPads. This info
  // is then stored in a TrdModule object for each of the
  // TRD modules.

  Int_t nmodules = 0;
  TString TopNode = gGeoManager->GetTopNode()->GetName();

  TObjArray* nodes = gGeoManager->GetTopNode()->GetNodes();
  for (Int_t iNode = 0; iNode < nodes->GetEntriesFast(); iNode++) {
    TGeoNode* node = (TGeoNode*) nodes->At(iNode);
    if (TString(node->GetName()).Contains("trd")) {
      TString StationNode = node->GetName();
      TGeoNode* station = node;

	TObjArray* modules = station->GetNodes();
	for (Int_t iLayerPart = 0; iLayerPart < modules->GetEntriesFast(); iLayerPart++) {
          TGeoNode* module = (TGeoNode*) modules->At(iLayerPart);
          TString ModuleNode = module->GetName();

          TObjArray* parts = module->GetNodes();
	  for (Int_t iPart = 0; iPart < parts->GetEntriesFast(); iPart++) {
            TGeoNode* part = (TGeoNode*) parts->At(iPart);
            if (TString(part->GetName()).Contains("gas")) {
              TString PartNode = part->GetName();

              // Put together the full path to the interesting volume, which
	      // is needed to navigate with the geomanager to this volume.
              // Extract the geometry information (size, global position)
              // from this volume.;

              TString FullPath = "/" + TopNode + "/" + StationNode + "/" + 
                                 ModuleNode + "/" + PartNode;

              FillModuleInfoFromGeoHandler(FullPath);

              // Get Information about the padstructure for a
              // given trd module defined by the station and
              // layer numbers, the module type and the copy
              // number
              FillPadInfoSegmentedSquared();
 
              CorrectOrientationOfPadPlane();

              nmodules++;

              // Create new CbmTrdModule and add it to the map
	      fModuleMap[fModuleID] = 
                new CbmTrdModule(fModuleID, fX, fY, fZ, fSizex, fSizey, fSizez,
				 fMaxSectors, fSectorSizex, fSectorSizey, 
				 fpadsizex, fpadsizey);
	    }
	  }
      }
    }
  }

  FillDigiPar();
}

void CbmTrdCreateDigiPar::FillModuleInfoFromGeoHandler(TString FullPath) 
{
  fModuleID = fGeoHandler->GetUniqueDetectorId(FullPath);	      
  
  fStation = fGeoHandler->GetStation(fModuleID);
  fLayer = fGeoHandler->GetLayer(fModuleID);
  fModuleType = fGeoHandler->GetModuleType(fModuleID);
  fModuleCopy = fGeoHandler->GetModuleCopyNr(fModuleID);
	    
  fSizex = fGeoHandler->GetSizeX(FullPath);
  fSizey = fGeoHandler->GetSizeY(FullPath);
  fSizez = fGeoHandler->GetSizeZ(FullPath);

  fX = fGeoHandler->GetX(FullPath);
  fY = fGeoHandler->GetY(FullPath);
  fZ = fGeoHandler->GetZ(FullPath);
}

void CbmTrdCreateDigiPar::CorrectOrientationOfPadPlane()
{
  //Orientation of the detector layers
  //Layer 1 and 3 have resolution in x-direction (Orientation=1)
  //Layer 2 and 4 have resolution in y-direction (Orientation=0)
  Int_t Orientation=fLayer%2;
  if( 0 == Orientation) {  // flip pads for even layers
    Double_t copybuf;
    for ( Int_t i=0; i<fMaxSectors; i++) {
      copybuf = fpadsizex.At(i);
      fpadsizex.AddAt(fpadsizey.At(i),i);
      fpadsizey.AddAt(copybuf,i);
      copybuf = fSectorSizex.At(i);
      fSectorSizex.AddAt(fSectorSizey.At(i),i);
      fSectorSizey.AddAt(copybuf,i);
    }
  }
}

void CbmTrdCreateDigiPar::FillDigiPar()
{
  Int_t Nrmodules = (Int_t)fModuleMap.size();
  cout <<"Nr. of modules: "<<Nrmodules<<endl;
  
  fDigiPar->SetNrOfModules(Nrmodules);
  fDigiPar->SetMaxSectors(fMaxSectors);
  
  TArrayI *ModuleId  = new TArrayI(Nrmodules);
  
  Int_t iDigi=0; 
  for ( fModuleMapIt=fModuleMap.begin() ; fModuleMapIt != fModuleMap.end(); fModuleMapIt++ ){
    ModuleId->AddAt(fModuleMapIt->second->GetDetectorId(),iDigi);
    iDigi++;
  }  
  
  fDigiPar->SetModuleIdArray(*ModuleId);
  fDigiPar->SetModuleMap(fModuleMap);
}

// --------------------------------------------------------------------
void CbmTrdCreateDigiPar::FillPadInfoSegmentedSquaredOneKeepingVolume(){

  // Reset the Array in case we have different sector sizes for
  // different detector modules

  for (Int_t i=0; i < fMaxSectors; i++ ) {
    fSectorSizex.AddAt(0.,i);
    fSectorSizey.AddAt(0.,i);
    fpadsizex.AddAt(   0.,i);
    fpadsizey.AddAt(   0.,i);
  }

  Int_t moduleType;


  LOG(INFO)<< "Station: "<<fStation<<FairLogger::endl;
  LOG(INFO)<<  "Type: "<<fModuleType<<FairLogger::endl;
  LOG(INFO)<< "fModuleCopy: "<<fModuleCopy<<FairLogger::endl;

  fModTypeMap = fModInfoMap.find(fStation)->second;
  fModuleTypeVector = fModTypeMap.find(fModuleType)->second;
  moduleType=fModuleTypeVector[fModuleCopy-1];

  // copy pad plane layout
  for (Int_t i=0; i < fst1_sect_count; i++ ) {
    fSectorSizex.AddAt(fst1_pad_type[moduleType][i][0],i);
    fSectorSizey.AddAt(fst1_pad_type[moduleType][i][1],i);
    fpadsizex.AddAt(   fst1_pad_type[moduleType][i][2],i);
    fpadsizey.AddAt(   fst1_pad_type[moduleType][i][3],i);
  }
}

// --------------------------------------------------------------------
void CbmTrdCreateDigiPar::FillPadInfoSegmentedSquared(){
  
  // Reset the Array in case we have different sector sizes for
  // different detector modules

  for (Int_t i=0; i < fMaxSectors; i++ ) {
    fSectorSizex.AddAt(0.,i);
    fSectorSizey.AddAt(0.,i);
    fpadsizex.AddAt(   0.,i);
    fpadsizey.AddAt(   0.,i);
  }

  Int_t moduleType;

  fModTypeMap = fModInfoMap.find(fStation)->second;
  fModuleTypeVector = fModTypeMap.find(fModuleType)->second;
  moduleType=fModuleTypeVector[fModuleCopy-1];

  if (fStation==1) {
    for (Int_t i=0; i < fst1_sect_count; i++ ) {
      fSectorSizex.AddAt(fst1_pad_type[moduleType][i][0],i);
      fSectorSizey.AddAt(fst1_pad_type[moduleType][i][1],i);
      fpadsizex.AddAt(   fst1_pad_type[moduleType][i][2],i);
      fpadsizey.AddAt(   fst1_pad_type[moduleType][i][3],i);
    }
  }
  if (fStation==2) {
    for (Int_t i=0; i < fst2_sect_count; i++ ) {
      fSectorSizex.AddAt(fst2_pad_type[moduleType][i][0],i);
      fSectorSizey.AddAt(fst2_pad_type[moduleType][i][1],i);
      fpadsizex.AddAt(   fst2_pad_type[moduleType][i][2],i);
      fpadsizey.AddAt(   fst2_pad_type[moduleType][i][3],i);
    }
  }
  if (fStation==3) {
    for (Int_t i=0; i < fst3_sect_count; i++ ) {
      fSectorSizex.AddAt(fst3_pad_type[moduleType][i][0],i);
      fSectorSizey.AddAt(fst3_pad_type[moduleType][i][1],i);
      fpadsizex.AddAt(   fst3_pad_type[moduleType][i][2],i);
      fpadsizey.AddAt(   fst3_pad_type[moduleType][i][3],i);
    }
  }
}

// --------------------------------------------------------------------
void CbmTrdCreateDigiPar::FillModuleMapSegmentedRectangular(){
 
  // The geometry structure is treelike with cave as
  // the top node. For the TRD there are keeping volumes
  // trd1-trd4 for each station which are only containers 
  // for the different layers of one station. The trdlayer
  // is again only a container for all volumes of this layer.   
  // Loop over all nodes below the top node (cave). If one of
  // the nodes containes a string trd it must be one of the
  // stations. Now loop over the layers of this station and 
  // then over all modules of the layer to extract in the end
  // all active regions (gas) of the complete TRD. For each
  // of the gas volumes get the information about size and
  // position from the geomanager and the sizes of the sectors
  // and pads from the definitions in CbmTrdPads. This info
  // is then stored in a TrdModule object for each of the
  // TRD modules.

  Int_t nmodules = 0;
  TString TopNode = gGeoManager->GetTopNode()->GetName();
  TObjArray* nodes = gGeoManager->GetTopNode()->GetNodes();
  for (Int_t iNode = 0; iNode < nodes->GetEntriesFast(); iNode++) {
    TGeoNode* node = (TGeoNode*) nodes->At(iNode);
    if (TString(node->GetName()).Contains("trd")) {
      TString StationNode = node->GetName();
      TGeoNode* station = node;

//      TObjArray* layers = station->GetNodes();
//      for (Int_t iLayer = 0; iLayer < layers->GetEntriesFast(); iLayer++) {
//        TGeoNode* layer = (TGeoNode*) layers->At(iLayer);
//        TString LayerNode = layer->GetName();
//	TObjArray* modules = layer->GetNodes();

 	TObjArray* modules = station->GetNodes();
	for (Int_t iLayerPart = 0; iLayerPart < modules->GetEntriesFast(); iLayerPart++) {
          TGeoNode* module = (TGeoNode*) modules->At(iLayerPart);
          TString ModuleNode = module->GetName();
          TObjArray* parts = module->GetNodes();
	  for (Int_t iPart = 0; iPart < parts->GetEntriesFast(); iPart++) {
            TGeoNode* part = (TGeoNode*) parts->At(iPart);
            if (TString(part->GetName()).Contains("gas")) {
              TString PartNode = part->GetName();
              // Put together the full path to the interesting volume, which
	      // is needed to navigate with the geomanager to this volume.
              // Extract the geometry information (size, global position)
              // from this volume.

// no layers in Jul10  
//              TString FullPath = "/" + TopNode + "/" + StationNode + "/" + 
//                                 LayerNode + "/" + ModuleNode + "/" + PartNode;

              TString FullPath = "/" + TopNode + "/" + StationNode + "/" + 
                                 ModuleNode + "/" + PartNode;

	      FillModuleInfoFromGeoHandler(FullPath);

              // Get Information about the padstructure for a
              // given trd module defined by the station and
              // layer numbers, the module type and the copy
              // number
              FillPadInfoSegmentedRectangular();

	      CorrectOrientationOfPadPlane();

              nmodules++;

              // Create new CbmTrdModule and add it to the map
	      fModuleMap[fModuleID] = 
                new CbmTrdModule(fModuleID, fX, fY, fZ, fSizex, fSizey, fSizez,
				 fMaxSectors, fSectorSizex, fSectorSizey, 
				 fpadsizex, fpadsizey);
	    }
	  }

// no layers in Jul10  
//	}

      }
    }
  }

  FillDigiPar();

}
// --------------------------------------------------------------------
void CbmTrdCreateDigiPar::FillPadInfoSegmentedRectangular(){
  
  // Reset the Array in case we have different sector sizes for
  // different detector modules

  for (Int_t i=0; i < fMaxSectors; i++ ) {
    fSectorSizex.AddAt(0.,i);
    fSectorSizey.AddAt(0.,i);
    fpadsizex.AddAt(   0.,i);
    fpadsizey.AddAt(   0.,i);
  }

  Int_t moduleType;

  //-----------------------------------------------------------------------
  // station 1
  // v12x:  8 + X + X +16 + X + X +26 +  X +  X  -- number of detector sizes
  // v12x:  4 + 4 + 0 + 4 + 8 + 4 +12 + 14 +  0  -- number of pad plane types
  // v12x:  4   8   8  12  20  24  36   50   50  -- sum of modules for this station
  //
  // v11x:  8 + X +16 + X +26 +  X   -- number of detector sizes
  // v11x:  8 + 0 +12 + 4 + 6 + 20   -- number of pad plane types
  // v11x:  8   8  20  24  30   50   -- sum of modules for this station

  if (fStation==1) {

    if (fModuleType==1) 
    {
      if (fModuleCopy <= 4) 
      {
	moduleType=0;
      } else if (fModuleCopy <= 8) 
      {
	moduleType=1;
      } else
      {
	moduleType=2;
      }
    }
    
    if (fModuleType==2) 
    {
      if (fModuleCopy <= 4) 
      {
	moduleType=3;
      } else if (fModuleCopy <= 12) 
      {
	moduleType=4;
      } else
      {
	moduleType=5;
      }
    }
    
    if (fModuleType==3) 
    {
      if (fModuleCopy <= 12) 
      {
	moduleType=6;
      } else if (fModuleCopy <= 26) 
      {
	moduleType=7;
      } else
      {
	moduleType=8;
      }
    }

    for (Int_t i=0; i < fst1_sect_count; i++ ) {
      fSectorSizex.AddAt(fst1_pad_type[moduleType][i][0],i);
      fSectorSizey.AddAt(fst1_pad_type[moduleType][i][1],i);
      fpadsizex.AddAt(   fst1_pad_type[moduleType][i][2],i);
      fpadsizey.AddAt(   fst1_pad_type[moduleType][i][3],i);
    }
  }

  //-------------------------------------------------------------------------
  // station 2
  // v12x:  8 + X +16 + X +54 + X   -- number of detector sizes
  // v12x:  4 + 4 +12 + 4 +12 +42   -- number of pad plane types
  // v12x:  4   8  20  24  36  78   -- sum of modules for this station
  //
  // v11x:  8 + X +16 + X +54 + X   -- number of detector sizes
  // v11x:  4 + 4 +12 + 4 +54 + X   -- number of pad plane types
  // v11x:  4   8  20  24  78   X   -- sum of modules for this station
   
  if (fStation==2) {

    if (fModuleType==1) {
      if (fModuleCopy <= 4) {
	moduleType=0;
      } else {
	moduleType=1;
      }
    }

    if (fModuleType==2) {
      if (fModuleCopy <= 12) {
	moduleType=2;
      } else {
	moduleType=3;
      }
    }

    if (fModuleType==3) {
      if (fModuleCopy <= 12) {
 	moduleType=4;
      } else {
	moduleType=5;
      }
    }

    for (Int_t i=0; i < fst2_sect_count; i++ ) {
      fSectorSizex.AddAt(fst2_pad_type[moduleType][i][0],i);
      fSectorSizey.AddAt(fst2_pad_type[moduleType][i][1],i);
      fpadsizex.AddAt(   fst2_pad_type[moduleType][i][2],i);
      fpadsizey.AddAt(   fst2_pad_type[moduleType][i][3],i);
    }
  }
  //------------------------------------------------------------------------
  // station 3
  // v12x:  98 +  X +  X +  X   -- number of detector sizes
  // v12x:   8 + 12 + 24 + 54   -- number of pad plane types
  // v12x:   8   20   44   98   -- sum of modules for this station
  //
  // v11x:  98 +  X   -- number of detector sizes
  // v11x:   8 + 90   -- number of pad plane types
  // v11x:   8   98   -- sum of modules for this station
  
  if (fStation==3) {
    if (fModuleType==3)
    {
      if (fModuleCopy <= 8) 
      {
	moduleType=0;
      } else if (fModuleCopy <= 20) 
      {
	moduleType=1;
      } else if (fModuleCopy <= 44) 
      {
	moduleType=2;
      } else
      {
	moduleType=3;
      }
    }

    for (Int_t i=0; i < fst3_sect_count; i++ ) {
      fSectorSizex.AddAt(fst3_pad_type[moduleType][i][0],i);
      fSectorSizey.AddAt(fst3_pad_type[moduleType][i][1],i);
      fpadsizex.AddAt(   fst3_pad_type[moduleType][i][2],i);
      fpadsizey.AddAt(   fst3_pad_type[moduleType][i][3],i);
    }
  }
  //-------------------------------------------------------------------------
}

ClassImp(CbmTrdCreateDigiPar)
