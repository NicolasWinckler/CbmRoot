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

// Name of geometry version and output file
const TString geoVersion = "tof_v12b";
const TString FileName = geoVersion + ".root";

// Names of the different used materials which are used to build the modules
// The materials are defined in the global media.geo file 
const TString KeepingVolumeMedium     = "air";
const TString BoxVolumeMedium         = "aluminium";
const TString NoActivGasMedium        = "Rpcgas_noact";
const TString ActivGasMedium          = "RPCgas";
const TString GlasMedium              = "RPCglass";
const TString ElectronicsMedium       = "carbon";

// Distance to the center of the TOF wall [cm];
const Float_t Wall_Z_Position = 600; 

const Int_t NumberOfDifferentCounterTypes = 1;
const Float_t Glass_X[NumberOfDifferentCounterTypes] = {32.};
const Float_t Glass_Y[NumberOfDifferentCounterTypes] = {26.9};
const Float_t Glass_Z[NumberOfDifferentCounterTypes] = {0.1};

const Float_t GasGap_X[NumberOfDifferentCounterTypes] = {32.};
const Float_t GasGap_Y[NumberOfDifferentCounterTypes] = {26.9};
const Float_t GasGap_Z[NumberOfDifferentCounterTypes] = {0.025};

const Int_t NumberOfGaps[NumberOfDifferentCounterTypes] = {8};
const Int_t NumberOfReadoutStrips[NumberOfDifferentCounterTypes] = {32};

const Float_t SingleStackStartPosition_Z[NumberOfDifferentCounterTypes] = {-0.6};

const Float_t Electronics_X[NumberOfDifferentCounterTypes] = {34.0};
const Float_t Electronics_Y[NumberOfDifferentCounterTypes] = {5.0};
const Float_t Electronics_Z[NumberOfDifferentCounterTypes] = {0.3};


const Int_t NofModuleTypes = 2;

//Aluminum box 
const Float_t AluBox_X = 180.;
const Float_t AluBox_Y = 50.;
const Float_t AluBox_Z = 20.;
const Float_t AluBox_Thick_X_left = 5.;
const Float_t AluBox_Thick_X_right = 1.;
const Float_t AluBox_Thick_Y = 1.;
const Float_t AluBox_Thick_Z = 0.1;

const Float_t CounterXStartPosition[NofModuleTypes] = {-60.0, -66.0};
const Float_t CounterXDistance[NofModuleTypes] = {30.0, 28.0};
const Float_t CounterZDistance[NofModuleTypes] = {5.0, 8.0};
const Float_t CounterRotationAngle[NofModuleTypes] = {0., 20.};

//Pole (support structure)
const Float_t Pole_X = 20.;
const Float_t Pole_Y = 1000.;
const Float_t Pole_Z = 20.;
const Float_t Pole_Thick_X = 5.;
const Float_t Pole_Thick_Y = 5.;
const Float_t Pole_Thick_Z = 5.;
const Int_t NumberOfPoles=7;
const Float_t ChamberOverlap=40;
const Float_t DxColl=AluBox_X-ChamberOverlap;
const Float_t Pole_Offset=AluBox_X/2.+Pole_X/2.;

const Float_t Module_Y_First_Position=70.;
const Float_t Module_Y_Last_Position=480.;

// some global variables
TGeoManager* gGeoMan = NULL;  // Pointer to TGeoManager instance
TGeoVolume* gModules[NofModuleTypes]; // Global storage for module types
TGeoVolume* gCounter;
TGeoVolume* gPole;

// Forward declarations
void create_materials_from_media_file();
TGeoVolume* create_counter(Int_t);
TGeoVolume* create_tof_module(Int_t);
TGeoVolume* create_tof_pole();
void position_tof_poles();
void position_inner_tof_modules();
void position_outer_tof_modules();



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

  TGeoVolume* tof = new TGeoVolumeAssembly(geoVersion);
  top->AddNode(tof, 1);

  for(Int_t counterType = 0; counterType < NumberOfDifferentCounterTypes; counterType++) { 
    gCounter = create_counter(counterType);
  }

  for(Int_t moduleType = 0; moduleType < NofModuleTypes; moduleType++) { 
    gModules[moduleType] = create_tof_module(moduleType);
  }

  gPole = create_tof_pole();

  postion_tof_poles();
  position_inner_tof_modules();
  position_outer_tof_modules();

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

TGeoVolume* create_counter(Int_t modType)
{

  //glass
  Float_t gdx=Glass_X[modType]; 
  Float_t gdy=Glass_Y[modType];
  Float_t gdz=Glass_Z[modType];

  //gas gap
  Int_t  nstrips=NumberOfReadoutStrips[modType];
  Int_t  ngaps=NumberOfGaps[modType];


  Float_t ggdx=GasGap_X[modType];  
  Float_t ggdy=GasGap_Y[modType];
  Float_t ggdz=GasGap_Z[modType];
  Float_t gsdx=ggdx/float(nstrips);

  //single stack
  Float_t dzpos=gdz+ggdz;
  Float_t startzpos=SingleStackStartPosition_Z[modType];

  // electronics
  //pcb dimensions 
  Float_t dxe=Electronics_X[modType]; 
  Float_t dye=Electronics_Y[modType];
  Float_t dze=Electronics_Z[modType];
  Float_t yele=(gdy+0.1)/2.+dye/2.;
 
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
    = new TGeoTranslation("", 0., 0., startzpos + ngaps*dzpos);
  multi_stack->AddNode(glass_plate_vol, l, single_glass_back_trans);
  
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

TGeoVolume* create_tof_module(Int_t modType)
{
  Float_t dx=AluBox_X;
  Float_t dy=AluBox_Y;
  Float_t dz=AluBox_Z;
  Float_t width_aluxl=AluBox_Thick_X_left;
  Float_t width_aluxr=AluBox_Thick_X_right;
  Float_t width_aluy=AluBox_Thick_Y;
  Float_t width_aluz=AluBox_Thick_Z;

  Float_t shift_gas_box = (AluBox_Thick_X_right - AluBox_Thick_X_left)/2;

  Float_t dxpos=CounterXDistance[modType];
  Float_t startxpos=CounterXStartPosition[modType];
  Float_t dzoff=CounterZDistance[modType];
  Float_t rotangle=CounterRotationAngle[modType];

  TGeoMedium* boxVolMed          = gGeoMan->GetMedium(BoxVolumeMedium);
  TGeoMedium* noActiveGasVolMed  = gGeoMan->GetMedium(NoActivGasMedium);

  TString moduleName = Form("module_%d", modType);
  TGeoVolume* module = new TGeoVolumeAssembly(moduleName);

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
    = new TGeoTranslation("", shift_gas_box, 0., 0.);
  alu_box_vol->AddNode(gas_box_vol, 0, gas_box_trans);
  
  for (Int_t j=0; j<5; j++){ //loop over counters (modules)
    Float_t zpos;
    if (0 == modType) {
      zpos = dzoff *=-1;
    } else {
      zpos = 0.;
    }
    TGeoTranslation* counter_trans 
      = new TGeoTranslation("", startxpos+ j*dxpos , 0.0 , zpos);

    TGeoRotation* counter_rot = new TGeoRotation();
    counter_rot->RotateY(rotangle);
    TGeoCombiTrans* counter_combi_trans = new TGeoCombiTrans(*counter_trans, *counter_rot);
    gas_box_vol->AddNode(gCounter, j, counter_combi_trans);
  }

  return module;
}


TGeoVolume* create_tof_pole()
{
  // needed materials
  TGeoMedium* boxVolMed          = gGeoMan->GetMedium(BoxVolumeMedium);
  TGeoMedium* airVolMed   = gGeoMan->GetMedium(KeepingVolumeMedium);


  //i   = index of the first box/tower
  //Z   = distance to the target
  //phi = angle in along the Z axis
  //dy  = Thickness in Y
  //dx  = Thickness in X 
  //dz  = Thickness in Z
  //width_alu = Aluminum thickness

  // Float_t dxb=AluBox_X;
 Float_t dx=Pole_X;
 Float_t dy=Pole_Y;
 Float_t dz=Pole_Z;
 Float_t width_alux=Pole_Thick_X;
 Float_t width_aluy=Pole_Thick_Y;
 Float_t width_aluz=Pole_Thick_Z;

  TGeoVolume* pole = new TGeoVolumeAssembly("Pole");

  TGeoBBox* pole_alu_box = new TGeoBBox("", dx/2., dy/2., dz/2.);
  TGeoVolume* pole_alu_vol = 
    new TGeoVolume("pole_alu", pole_alu_box, boxVolMed);
  pole_alu_vol->SetLineColor(kGreen); // set line color for the alu box
  pole_alu_vol->SetTransparency(70); // set transparency for the TOF
  TGeoTranslation* pole_alu_trans 
    = new TGeoTranslation("", 0., 0., 0.);
  pole->AddNode(pole_alu_vol, 0, pole_alu_trans);

  TGeoBBox* pole_air_box = new TGeoBBox("", dx/2.-width_alux, dy/2.-width_aluy, dz/2.-width_aluz);
  TGeoVolume* pole_air_vol = 
    new TGeoVolume("pole_air", pole_air_box, airVolMed);
  pole_air_vol->SetLineColor(kYellow); // set line color for the alu box
  pole_air_vol->SetTransparency(70); // set transparency for the TOF
  TGeoTranslation* pole_air_trans 
    = new TGeoTranslation("", 0., 0., 0.);
  pole_alu_vol->AddNode(pole_air_vol, 0, pole_air_trans);

  return pole;
}

void postion_tof_poles()
{

  TGeoTranslation* pole_trans=NULL;

  Int_t numPoles=0;
  pole_trans 
    = new TGeoTranslation("", -Pole_Offset, 0., Wall_Z_Position);
  gGeoMan->GetVolume(geoVersion)->AddNode(gPole, numPoles, pole_trans);
  numPoles++;
  pole_trans 
    = new TGeoTranslation("", -Pole_Offset, 0., Wall_Z_Position+Pole_Z);
  gGeoMan->GetVolume(geoVersion)->AddNode(gPole, numPoles, pole_trans);
    numPoles++;

  Int_t maxDiffPoles=NumberOfPoles/2+1;
  for(Int_t i=1; i<maxDiffPoles; i++) {  
    Float_t xPos=Pole_Offset+Pole_X/2.+i*DxColl;
    Float_t zPos=Wall_Z_Position-i*2*AluBox_Z;

    pole_trans 
      = new TGeoTranslation("", xPos, 0., zPos);
    gGeoMan->GetVolume(geoVersion)->AddNode(gPole, numPoles, pole_trans);
    numPoles++;
    pole_trans 
      = new TGeoTranslation("", xPos, 0., zPos+Pole_Z);
    gGeoMan->GetVolume(geoVersion)->AddNode(gPole, numPoles, pole_trans);
    numPoles++;
    pole_trans 
      = new TGeoTranslation("", -xPos, 0., zPos);
    gGeoMan->GetVolume(geoVersion)->AddNode(gPole, numPoles, pole_trans);
    numPoles++;
    pole_trans 
      = new TGeoTranslation("", -xPos, 0., zPos+Pole_Z);
    gGeoMan->GetVolume(geoVersion)->AddNode(gPole, numPoles, pole_trans);
    numPoles++;
  }
}

void position_inner_tof_modules()
{
  TGeoTranslation* module_trans=NULL;

  Int_t numModules=(Int_t)( (Module_Y_Last_Position-Module_Y_First_Position)/AluBox_Y)+1;

  Int_t modNum = 0;
  for(Int_t i=0; i<numModules; i++) {  

    Float_t yPos = Module_Y_First_Position + i*AluBox_Y; 
    Float_t xPos = 0;
    Float_t zPos = Wall_Z_Position;

    module_trans 
      = new TGeoTranslation("", xPos, yPos, zPos);
    gGeoMan->GetVolume(geoVersion)->AddNode(gModules[0], modNum, module_trans);
    modNum++;
    module_trans 
      = new TGeoTranslation("", xPos, -yPos, zPos);
    gGeoMan->GetVolume(geoVersion)->AddNode(gModules[0], modNum, module_trans);
    modNum++;
    if (i>0) {
      module_trans 
	= new TGeoTranslation("", xPos, yPos-AluBox_Y/2, zPos+AluBox_Z);
      gGeoMan->GetVolume(geoVersion)->AddNode(gModules[0], modNum, module_trans);
    modNum++;
      module_trans 
	= new TGeoTranslation("", xPos, -(yPos-AluBox_Y/2), zPos+AluBox_Z);
      gGeoMan->GetVolume(geoVersion)->AddNode(gModules[0], modNum, module_trans);
    modNum++;
    }
  }

}

void position_outer_tof_modules()
{
  TGeoTranslation* module_trans=NULL;
  TGeoRotation* module_rot = new TGeoRotation();
  module_rot->RotateZ(180.);
  TGeoCombiTrans* module_combi_trans = NULL;

  Int_t numModules=(Int_t)( (Module_Y_Last_Position-0.)/AluBox_Y)+1;
  
  Int_t modNum = 0;
  for(Int_t j=0; j<3;j++){ 
      Float_t xPos = 10. + ((j+1)*DxColl);
      Float_t zPos = Wall_Z_Position - ((j+1)*2*AluBox_Z);

    for(Int_t i=0; i<numModules; i++) {  
      
      Float_t yPos = 0. + i*AluBox_Y; 

      module_trans 
	= new TGeoTranslation("", xPos, yPos, zPos);
      gGeoMan->GetVolume(geoVersion)->AddNode(gModules[1], modNum, module_trans);
      modNum++;
      module_trans 
	= new TGeoTranslation("", -xPos, yPos, zPos);
      module_combi_trans = new TGeoCombiTrans(*module_trans, *module_rot);
      gGeoMan->GetVolume(geoVersion)->AddNode(gModules[1], modNum, module_combi_trans);
      modNum++;

      if (i>0) {
	module_trans 
	  = new TGeoTranslation("", xPos, -yPos, zPos);
	gGeoMan->GetVolume(geoVersion)->AddNode(gModules[1], modNum, module_trans);
	modNum++;
	module_trans 
	  = new TGeoTranslation("", -xPos, -yPos, zPos);
	module_combi_trans = new TGeoCombiTrans(*module_trans, *module_rot);
	gGeoMan->GetVolume(geoVersion)->AddNode(gModules[1], modNum, module_combi_trans);
	modNum++;

	// second layer
	module_trans 
	  = new TGeoTranslation("", xPos, yPos-AluBox_Y/2., zPos+AluBox_Z);
	gGeoMan->GetVolume(geoVersion)->AddNode(gModules[1], modNum, module_trans);
	modNum++;
	module_trans 
	  = new TGeoTranslation("", -xPos, yPos-AluBox_Y/2., zPos+AluBox_Z);
	module_combi_trans = new TGeoCombiTrans(*module_trans, *module_rot);
	gGeoMan->GetVolume(geoVersion)->AddNode(gModules[1], modNum, module_combi_trans);
	modNum++;

	module_trans 
	  = new TGeoTranslation("", xPos, -(yPos-AluBox_Y/2.), zPos+AluBox_Z);
	gGeoMan->GetVolume(geoVersion)->AddNode(gModules[1], modNum, module_trans);
	modNum++;
	module_trans 
	  = new TGeoTranslation("", -xPos, -(yPos-AluBox_Y/2.), zPos+AluBox_Z);
	module_combi_trans = new TGeoCombiTrans(*module_trans, *module_rot);
	gGeoMan->GetVolume(geoVersion)->AddNode(gModules[1], modNum, module_combi_trans);
	modNum++;

      }

    }
  }
  
}
