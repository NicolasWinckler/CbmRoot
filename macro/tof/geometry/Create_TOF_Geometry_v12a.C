// in root all sizes are given in cm

#include "TSystem.h"
#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TGeoMaterial.h"
#include "TGeoMedium.h"
#include "TGeoPgon.h"
#include "TGeoMatrix.h"
#include "TGeoCompositeShape.h"
#include "TFile.h"
#include "TString.h"
#include "TList.h"
#include "TROOT.h"

#include <iostream>

// Name of output file with geometry
const TString FileName = "tof_v12a.root";

// Names of the different used materials which are used to build the modules
// The materials are defined in the global media.geo file 
const TString KeepingVolumeMedium     = "air";
const TString BoxVolumeMedium         = "aluminium";

const TString NoActivGasMedium        = "Rpcgas_noact";
const TString ActivGasMedium          = "RPCgas";
const TString GlasMedium              = "RPCglass";
const TString ElectronicsMedium           = "carbon";

const Int_t NofModuleTypes = 2;


// All neded distances and sizes are defined here
Float_t B_factor=1.; // Relation between x and y
Float_t Xlim=25.; // Begining of X segmentation [cm]
Float_t Ylim=25.; // Begining of Y segmentation [cm]
Float_t Dplate=0.1; // Glass plate [cm]
Float_t Dwall = 600; // Distance to the center of the TOF wall [cm];
Float_t box_xdim = 1500.;  // x extension of wall [cm]
Float_t box_ydim = 1100.;  //y extension of wall [cm]
Float_t box_zdim = 200.;  //z extension of wall [cm]
Float_t width_box =  0.4;   // Width of the gas box [cm]

//Int_t Nchannel = 0; // don't know if this is needed
Int_t ngaps=8; // Number of gaps

// some global variables
TGeoManager* gGeoMan = NULL;  // Pointer to TGeoManager instance
TGeoVolume* gModules[NofModuleTypes]; // Global storage for module types
TGeoVolume* gCounter;

// Forward declarations
void create_materials_from_media_file();
TGeoVolume* create_small_tof_module();
//TGeoVolume* create_large_tof_module();
TGeoVolume* create_glass_stack();



void Create_TOF_Geometry_v12a() {
  // Load the necessary FairRoot libraries 
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");

  // Load needed material definition from media.geo file
  create_materials_from_media_file();

  // Get the GeoManager for later usage
  gGeoMan = (TGeoManager*) gROOT->FindObject("FAIRGeom");
  gGeoMan->SetVisLevel(10);  

  // Create the top volume 
  TGeoBBox* topbox= new TGeoBBox("", 1000., 1000., 1000.);
  TGeoVolume* top = new TGeoVolume("top", topbox, gGeoMan->GetMedium("air"));
  gGeoMan->SetTopVolume(top);

  TGeoVolume* tof = new TGeoVolumeAssembly("tof");
  top->AddNode(tof, 1);

  gCounter = create_glass_stack();

  /*
  TGeoTranslation* counter_trans 
    = new TGeoTranslation("", 0., 0., 100.);
  gGeoMan->GetVolume("tof")->AddNode(gCounter, 0, counter_trans);
  */

  gModules[0] = create_small_tof_module();
  //  gModules[1] = create_large_tof_module();

  
  TGeoTranslation* module_trans 
    = new TGeoTranslation("", 0., 0., 0.);
  gGeoMan->GetVolume("tof")->AddNode(gModules[0], 0, module_trans);
    


  gGeoMan->CloseGeometry();
//  gGeoMan->CheckOverlaps(0.001);
//  gGeoMan->PrintOverlaps();
//  gGeoMan->Test();

  TFile* outfile = new TFile(FileName,"RECREATE");
  //  top->Write();
  gGeoMan->Write();
  outfile->Close();
  //  top->Draw("ogl");
  //top->Raytrace();

}

void create_materials_from_media_file()
{
  // Use the FairRoot geometry interface to load the media which are already defined
  FairGeoLoader* geoLoad = new FairGeoLoader("TGeo", "FairGeoLoader");
  FairGeoInterface* geoFace = geoLoad->getGeoInterface();
  TString geoPath = gSystem->Getenv("VMCWORKDIR");
  TString geoFile = geoPath + "/geometry/media.geo";
  geoFace->setMediaFile(geoFile);
  geoFace->readMedia();

  // Read the required media and create them in the GeoManager
  FairGeoMedia* geoMedia = geoFace->getMedia();
  FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();

  FairGeoMedium* air              = geoMedia->getMedium("air");
  FairGeoMedium* aluminium        = geoMedia->getMedium("aluminium");
  FairGeoMedium* RPCgas           = geoMedia->getMedium("RPCgas");
  FairGeoMedium* RPCgas_noact     = geoMedia->getMedium("RPCgas_noact");
  FairGeoMedium* RPCglass         = geoMedia->getMedium("RPCglass");
  FairGeoMedium* carbon           = geoMedia->getMedium("carbon");

  geoBuild->createMedium(air);
  geoBuild->createMedium(aluminium);
  geoBuild->createMedium(RPCgas);
  geoBuild->createMedium(RPCgas_noact);
  geoBuild->createMedium(RPCglass);
  geoBuild->createMedium(carbon);
}

TGeoVolume* create_glass_stack()
{

  Int_t   nstrips=32;

  Float_t gdx=32.; //glass
  Float_t gdy=26.9;
  Float_t gdz=.1;
  Float_t ggdx=gdx;  //gas gap
  Float_t ggdy=gdy;
  Float_t ggdz=0.025;
  Float_t gsdx=ggdx/float(nstrips);

  Float_t dzpos=gdz+ggdz;
  Float_t startzpos=-.6;
  Float_t startzposg=-.6+gdz/2.;

  // needed materials
  TGeoMedium* glassPlateVolMed   = gGeoMan->GetMedium(GlasMedium);
  TGeoMedium* noActiveGasVolMed  = gGeoMan->GetMedium(NoActivGasMedium);
  TGeoMedium* activeGasVolMed    = gGeoMan->GetMedium(ActivGasMedium);
  TGeoMedium* electronicsVolMed  = gGeoMan->GetMedium(ElectronicsMedium);

  // Single glass plate
  TGeoBBox* glass_plate = new TGeoBBox("", gdx/2., gdy/2., gdz/2.);
  TGeoVolume* glass_plate_vol = 
    new TGeoVolume("tof_glass", glass_plate, glassPlateVolMed);
  glass_plate_vol->SetLineColor(kBlue); // set line color for the glass plate
  glass_plate_vol->SetTransparency(70); // set transparency for the TOF
  TGeoTranslation* glass_plate_trans 
    = new TGeoTranslation("", 0., 0., 0.);

  // Single gas gap
  TGeoBBox* gas_gap = new TGeoBBox("", ggdx/2., ggdy/2., ggdz/2.);
  TGeoVolume* gas_gap_vol = 
    new TGeoVolume("tof_gas_gap", gas_gap, noActiveGasVolMed);
  gas_gap_vol->SetLineColor(kRed); // set line color for the gas gap
  gas_gap_vol->SetTransparency(70); // set transparency for the TOF
  TGeoTranslation* gas_gap_trans 
    = new TGeoTranslation("", 0., 0., (gdz+ggdz)/2.);

  // Single subdivided active gas gap 
  TGeoBBox* gas_active = new TGeoBBox("", gsdx/2., ggdy/2., ggdz/2.);
  TGeoVolume* gas_active_vol = 
    new TGeoVolume("tof_gas_active", gas_active, activeGasVolMed);
  gas_active_vol->SetLineColor(kBlack); // set line color for the gas gap
  gas_active_vol->SetTransparency(70); // set transparency for the TOF

  // Add glass plate, inactive gas gap and active gas gaps to a single stack
  TGeoVolume* single_stack = new TGeoVolumeAssembly("single_stack");
  single_stack->AddNode(glass_plate_vol, 0, glass_plate_trans);
  single_stack->AddNode(gas_gap_vol, 0, gas_gap_trans);
  for (Int_t l=0; l<nstrips; l++){
    TGeoTranslation* gas_active_trans 
      = new TGeoTranslation("", -ggdx/2+(l+0.5)*gsdx, 0., (gdz+ggdz)/2.);
    single_stack->AddNode(gas_active_vol, l, gas_active_trans);
  }

  // Add 8 single stacks + one glass plate at the end to a multi stack
  TGeoVolume* multi_stack = new TGeoVolumeAssembly("multi_stack");
  for (Int_t l=0; l<ngaps; l++){
    TGeoTranslation* single_stack_trans 
      = new TGeoTranslation("", 0., 0., startzpos + l*dzpos);
    multi_stack->AddNode(single_stack, l, single_stack_trans);
  }
  TGeoTranslation* single_glass_back_trans 
    = new TGeoTranslation("", 0., 0., startzpos + 8*dzpos);
  multi_stack->AddNode(glass_plate_vol, l, single_glass_back_trans);
  

// electronics
  float dxe=34.0; //pcb dimensions 
  float dye=5.0;
  float dze=0.3;
  float yele=(gdy+0.1)/2.+dye/2.;

  // Add electronics above and below the glass stack to build a complete counter
  TGeoVolume* counter = new TGeoVolumeAssembly("counter");
  TGeoTranslation* multi_stack_trans 
      = new TGeoTranslation("", 0., 0., 0.);
  counter->AddNode(multi_stack, l, multi_stack_trans);

  TGeoBBox* pcb = new TGeoBBox("", dxe/2., dye/2., dze/2.);
  TGeoVolume* pcb_vol = 
    new TGeoVolume("pcb", pcb, electronicsVolMed);
  pcb_vol->SetLineColor(kBlack); // set line color for the gas gap
  pcb_vol->SetTransparency(70); // set transparency for the TOF
  for (Int_t l=0; l<2; l++){
    yele *= -1.;
    TGeoTranslation* pcb_trans 
      = new TGeoTranslation("", 0., yele, 0.);
    counter->AddNode(pcb_vol, l, pcb_trans);
  }

  return counter;

}

TGeoVolume* create_small_tof_module()
{
  //Aluminum box 
  //dy  = Thickness in Y
  //dx  = Thickness in X 
  //dz  = Thickness in Z
  //width_alu = Aluminum thickness
  //All dimensions are in cm

  Float_t dx=180.;
  Float_t dy=50.;
  Float_t dz=20.;
  Float_t width_aluxl=5.;
  Float_t width_aluxr=1.;
  Float_t width_aluy=1.;
  Float_t width_aluz=.1;

  TGeoMedium* boxVolMed          = gGeoMan->GetMedium(BoxVolumeMedium);
  TGeoMedium* noActiveGasVolMed  = gGeoMan->GetMedium(NoActivGasMedium);

  TGeoVolume* module = new TGeoVolumeAssembly("module");

  TGeoBBox* alu_box = new TGeoBBox("", dx/2., dy/2., dz/2.);
  TGeoVolume* alu_box_vol = 
    new TGeoVolume("alu_box", alu_box, boxVolMed);
  alu_box_vol->SetLineColor(kGreen); // set line color for the alu box
  alu_box_vol->SetTransparency(70); // set transparency for the TOF
  TGeoTranslation* alu_box_trans 
    = new TGeoTranslation("", 0., 0., 0.);
  module->AddNode(alu_box_vol, 0, alu_box_trans);

  TGeoBBox* gas_box = new TGeoBBox("", (dx-(width_aluxl+width_aluxr))/2., (dy-2*width_aluy)/2., (dz-2*width_aluz)/2.);
  TGeoVolume* gas_box_vol = 
    new TGeoVolume("gas_box", gas_box, noActiveGasVolMed);
  alu_box_vol->SetLineColor(kYellow); // set line color for the alu box
  alu_box_vol->SetTransparency(70); // set transparency for the TOF
  TGeoTranslation* gas_box_trans 
    = new TGeoTranslation("", -2., 0., 0.);
  alu_box_vol->AddNode(gas_box_vol, 0, gas_box_trans);

  Float_t dxpos=30.0;
  Float_t startxpos=-60.0;
  Float_t zoff=1.;
  Float_t dzoff=5.0;
  
  for (Int_t j=0; j<5; j++){ //loop over counters (modules)
    zoff=-zoff;
    TGeoTranslation* counter_trans 
      = new TGeoTranslation("", startxpos+ j*dxpos , 0.0 , zoff*dzoff);
    gas_box_vol->AddNode(gCounter, j, counter_trans);
  }

  return module;
}
