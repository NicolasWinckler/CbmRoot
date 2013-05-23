///                                             
/// \file Create_TRD_Geometry_v13a.C
/// \brief Generates TRD geometry in Root format.
///                                             

// 2013-05-23 - DE - remove "trd_" prefix from node names (except top node)
// 2013-05-22 - DE - radiators G30 (z=240 mm) 
// 2013-05-22 - DE - radiators H (z=275 mm - 125 * 2.2mm), (H++ z=335 mm)
// 2013-05-22 - DE - radiators B++ (z=254 mm - 350 * 0.724 mm), K++ (z=254 mm - 350 * 0.724 mm)
// 2013-04-17 - DE - introduce volume assembly for layers, e.g. trd_layer03
// 2013-03-26 - DE - use Air as ASIC material
// 2013-03-26 - DE - put support structure into its own assembly
// 2013-03-26 - DE - move TRD upstream to z=400m
// 2013-03-26 - DE - RICH will probably end at z=380 cm, TRD can move to 400 cm
// 2013-03-25 - DE - shrink active area from 570 to 540 mm and 960 to 910 mm
// 2013-03-06 - DE - add ASICs on FEBs
// 2013-03-05 - DE - introduce supports for SIS100 and SIS300
// 2013-03-05 - DE - replace all Float_t by Double_t
// 2013-01-21 - DE - introduce TRD media, use TRDG10 as material for pad plane and FEBs
// 2013-01-21 - DE - put backpanel into the geometry
// 2013-01-11 - DE - allow for misalignment of TRD modules
// 2012-11-04 - DE - add kapton foil, add FR4 padplane
// 2012-11-03 - DE - add lattice grid on entrance window as CompositeShape

// TODO: 
// - use Silicon as ASIC material

// in root all sizes are diven in cm

#include "TSystem.h"
#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TGeoMaterial.h"
#include "TGeoMedium.h"
#include "TGeoPgon.h"
#include "TGeoMatrix.h"
#include "TGeoCompositeShape.h"
#include "TGeoXtru.h"
#include "TFile.h"
#include "TString.h"
#include "TList.h"
#include "TRandom3.h"

#include <iostream>

// Name of output file with geometry
const TString geoVersion = "trd_v13n";
const TString FileNameSim = geoVersion + ".root";
//const TString FileNameSim = geoVersion + ".geo.root";
const TString FileNameGeo = geoVersion + "_geo.root";

// display switches
const Bool_t IncludeRadiator = true;  // false;  // true, if radiator is included in geometry
const Bool_t IncludeLattice  = true;  // false;  // true, if lattice grid is included in geometry
const Bool_t IncludeGasHoles = false; // false;  // true, if gas holes to be pllotted in the lattice grid
const Bool_t IncludeFebs     = true;  // false;  // true, if FEBs are included in geometry
const Bool_t IncludeAsics    = true;  // false;  // true, if ASICs are included in geometry
const Bool_t IncludeSupports = true;  // false;  // true, if support structure is included in geometry

const Double_t feb_rotation_angle = 45; //0.1; // 65.; // 70.; // 0.;   // rotation around x-axis, should be < 90 degrees  

// positioning switches
const Bool_t DisplaceRandom  = false;  // true; // false;  // add random displacement of modules for alignment study
const Bool_t RotateRandom    = false;  // true; // false;  // add random rotation of modules for alignment study
const Bool_t DoExplode       = false;  // true, // false;  // add random displacement of modules for alignment study

// positioning parameters
const Double_t maxdx    = 0.2;   // max +- 0.1 cm shift in x
const Double_t maxdy    = 0.2;   // max +- 0.1 cm shift in y
const Double_t maxdz    = 1.0;   // max +- 1.0 cm shift in z

const Double_t maxdrotx = 2.0;   // 20.0; // max rotation around x
const Double_t maxdroty = 2.0;   // 20.0; // max rotation around y
const Double_t maxdrotz = 2.0;   // 20.0; // max rotation around z

const Double_t ExplodeFactor = 1.02;   // 1.02; // Factor by which modules are exploded in the x/y plane

// initialise random numbers
TRandom3 r3(0);

// Parameters defining the layout of the complete detector build out of different detector layers.
const Int_t   MaxLayers = 10;   // max layers

// select layers to display
//
//const Int_t    ShowLayer[MaxLayers] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 };  //  1st layer only
//const Int_t    ShowLayer[MaxLayers] = { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 };  //  2nd layer only
//const Int_t    ShowLayer[MaxLayers] = { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 };  //  5th layer only
//const Int_t    ShowLayer[MaxLayers] = { 0, 0, 0, 0, 0, 1, 0, 0, 0, 0 };  //  6th layer only
//const Int_t    ShowLayer[MaxLayers] = { 0, 0, 0, 0, 0, 0, 0, 0, 1, 0 };  //  9th layer only
//const Int_t    ShowLayer[MaxLayers] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 };  // 10th layer only
//
//const Int_t    ShowLayer[MaxLayers] = { 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 };  // Station 1, layer 1, 2
//const Int_t    ShowLayer[MaxLayers] = { 0, 0, 0, 0, 1, 1, 0, 0, 0, 0 };  // Station 2, layer 5, 6
//const Int_t    ShowLayer[MaxLayers] = { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1 };  // Station 3, layer 9,10
//const Int_t    ShowLayer[MaxLayers] = { 1, 1, 0, 0, 1, 1, 0, 0, 0, 0 };  // Station 1 and 2
//const Int_t    ShowLayer[MaxLayers] = { 1, 1, 0, 0, 1, 1, 1, 0, 1, 1 };  // Station 1, 2 and 3
//
//const Int_t    ShowLayer[MaxLayers] = { 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 };  // SIS100-2l  // 1: plot, 0: hide
//const Int_t    ShowLayer[MaxLayers] = { 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 };  // SIS100-2l  // 1: plot, 0: hide
//const Int_t    ShowLayer[MaxLayers] = { 1, 1, 1, 1, 0, 0, 0, 0, 0, 0 };  // SIS100-4l  // 1: plot, 0: hide
//const Int_t    ShowLayer[MaxLayers] = { 0, 0, 0, 0, 1, 1, 1, 1, 1, 1 };  // SIS300-mu  // 1: plot, 0: hide
//
const Int_t    ShowLayer[MaxLayers] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };  // SIS300-e   // 1: plot, 0: hide

Int_t    PlaneId[MaxLayers]; // automatiaclly filles with layer ID
Int_t    ModId = 0;

const Int_t    LayerType[MaxLayers] = { 10, 11, 10, 11, 20, 21, 20, 21, 30, 31 };  // ab: a [1-3] - layer type, b [0,1] - vertical/hoziontal pads
const Double_t LayerNrInStation[MaxLayers] = { 1, 2, 3, 4, 1, 2, 3, 4, 1, 2 };
const Double_t LayerThickness = 49.5; // Thickness of one TRD layer in cm
 
// just behind RICH v13a at z=400
const Double_t LayerPosition[MaxLayers] = { 400., 450., 500., 550., 600., 650., 700., 750., 800., 850. };   // z position in cm of Layer front
// 3 stations, no gap between TRD stations
//const Double_t LayerPosition[MaxLayers] = { 450., 500., 550., 600., 650., 700., 750., 800., 850., 900. };  // v13c // z position in cm of Layer front
// 3 stations, 25 cm gap
//const Double_t LayerPosition[MaxLayers] = { 450., 500., 550., 600., 675., 725., 775., 825., 900., 950. };  // z position in cm of Layer front
// equal spacing
//const Double_t LayerPosition[MaxLayers] = { 500., 550., 600., 650., 700., 750., 800., 850., 900., 950. };  // z position in cm of Layer front

const Int_t LayerArraySize[3][4] =  { { 5, 5, 9, 11 },    // for layer[1-3][i,o] below
                                      { 5, 5, 9, 11 },
                                      { 5, 5, 9, 11 } };
// ### Layer Type 1
// v12x - module types in the inner sector of layer1 - looking upstream
const Int_t layer1i[5][5] = { { 423,  323,  221,  321,  421 },    // abc: a module type - b orientation (x90 deg) in odd - c even layers
                              { 323,  123,  121,  121,  321 },  
                              { 203,  103,    0,  101,  201 },  
                              { 303,  103,  101,  101,  301 },  
                              { 403,  303,  201,  301,  401 } };
// number of modules 1x0, 8x1, 4x2, 8x3, 4x4

// v12x - module types in the outer sector of layer1 - looking upstream
const Int_t layer1o[9][11]= { {  0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0 },
                              {  0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0 },
                              {  0,    0,  823,  823,  723,  721,  721,  821,  821,    0,    0 },
                              {  0,    0,  823,  723,    0,    0,    0,  721,  821,    0,    0 },
                              {  0,    0,  803,  703,    0,    0,    0,  701,  801,    0,    0 },
                              {  0,    0,  803,  703,    0,    0,    0,  701,  801,    0,    0 },
                              {  0,    0,  803,  803,  703,  701,  701,  801,  801,    0,    0 },
                              {  0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0 },
                              {  0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0 } };
// number of modules 73x0, 0x5, 0x6, 12x7, 14x8
// Layer1 =  24 + 26;   // v12x

// ### Layer Type 11 is Layer Type 1 with detector modules rotated by 90°
// In the subroutine creating the layers this is recognized automatically 


// ### Layer Type 2
// v12x - module types in the inner sector of layer2 - looking upstream
const Int_t layer2i[5][5] = { { 423,  323,  321,  321,  421 },    // abc: a module type - b orientation (x90 deg) in odd - c even layers 
                              { 323,  223,  121,  221,  321 },
                              { 303,  103,    0,  101,  301 },
                              { 303,  203,  101,  201,  301 },
                              { 403,  303,  301,  301,  401 } };
// number of modules 1x0, 4x1, 4x2, 12x3, 4x4

// v12x - module types in the outer sector of layer2 - looking upstream
const Int_t layer2o[9][11]= { {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0 },
                              {   0,  823,  823,  823,  823,  821,  821,  821,  821,  821,    0 },
                              {   0,  823,  823,  823,  623,  621,  621,  821,  821,  821,    0 },
                              {   0,  823,  823,  623,    0,    0,    0,  621,  821,  821,    0 },
                              {   0,  803,  803,  603,    0,    0,    0,  601,  801,  801,    0 },
                              {   0,  803,  803,  603,    0,    0,    0,  601,  801,  801,    0 },
                              {   0,  803,  803,  803,  603,  601,  601,  801,  801,  801,    0 },
                              {   0,  803,  803,  803,  803,  801,  801,  801,  801,  801,    0 },
                              {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0 } };
// number of modules 45x0, 0x5, 12x6, 0x7, 42x8
// Layer2 =  78;   // v12x

// ### Layer Type 21 is Layer Type 2 with detector modules rotated by 90°
// In the subroutine creating the layers this is recognized automatically 


// ### Layer Type 3
// v12x - module types in the inner sector of layer2 - looking upstream
const Int_t layer3i[5][5] = { {  0,  0,  0,  0,  0 },     // abc: a module type - b orientation (x90 deg) in odd - c even layers
                              {  0,  0,  0,  0,  0 },
                              {  0,  0,  0,  0,  0 },
                              {  0,  0,  0,  0,  0 },
                              {  0,  0,  0,  0,  0 } };
// number of modules 25x0 
// Only for convinience in the function needed


// v12x - module types in the outer sector of layer3 - looking upstream
const Int_t layer3o[9][11] = { { 823,  823,  823,  823,  823,  821,  821,  821,  821,  821,  821 },
                               { 823,  823,  823,  723,  723,  721,  721,  721,  821,  821,  821 },
                               { 823,  823,  723,  723,  623,  621,  621,  721,  721,  821,  821 },
                               { 823,  823,  723,  623,  523,  521,  521,  621,  721,  821,  821 },
                               { 803,  803,  703,  603,  503,    0,  501,  601,  701,  801,  801 },
                               { 803,  803,  703,  603,  503,  501,  501,  601,  701,  801,  801 },
                               { 803,  803,  703,  703,  603,  601,  601,  701,  701,  801,  801 },
                               { 803,  803,  803,  703,  703,  701,  701,  701,  801,  801,  801 },
                               { 803,  803,  803,  803,  803,  801,  801,  801,  801,  801,  801 } };
// number of modules 1x0, 8x5, 12x6, 24x7, 54x8
// Layer3 = 98;   // v12x

// ### Layer Type 31 is Layer Type 3 with detector modules rotated by 90
// In the subroutine creating the layers this is recognized automatically 


// Parameters defining the layout of the different detector modules
const Int_t NofModuleTypes = 8;
const Int_t ModuleType[NofModuleTypes]    = {  0,  0,  0,  0,  1,  1,  1,  1 }; // 0 = small module, 1 = large module

// ultimate density
//const Int_t FebsPerModule[NofModuleTypes] = {  6,  5,  3,  2,  5,  3,  2,  1 }; // min number of FEBs // number of FEBs on backside - reduced FEBs (64 ch ASICs)
//const Int_t AsicsPerFeb[NofModuleTypes]   = {216,210,210,210,216,216,216,216 }; //  %100 gives number of ASICs on FEB, /100 gives grouping
////
//const Int_t FebsPerModule[NofModuleTypes] = {  6,  5,  3,  3, 10,  5,  3,  3 }; // min (6) module types // number of FEBs on backside - reduced FEBs (64 ch ASICs)
//const Int_t AsicsPerFeb[NofModuleTypes]   = {216,210,210,210,108,108,108,108 }; //  %100 gives number of ASICs on FEB, /100 gives grouping
//// super density
const Int_t FebsPerModule[NofModuleTypes] = { 10,  5,  5,  5, 12,  6,  4,  3 }; // light // number of FEBs on backside - reduced FEBs (64 ch ASICs)
const Int_t AsicsPerFeb[NofModuleTypes]   = {210,210,105,105,108,108,108,108 }; // %100 gives number of ASICs on FEB, /100 gives grouping
//// normal density
//const Int_t FebsPerModule[NofModuleTypes] = { 19, 10,  5,  5, 12,  6,  4,  3 }; // number of FEBs on backside (linked to pad layout) - mod4 = mod3, therefore same
//const Int_t AsicsPerFeb[NofModuleTypes]   = {105,105,105,105,108,108,108,108 }; // %100 gives number of ASICs on FEB, /100 gives grouping
//
const Double_t feb_z_offset = 0.1;  // 1 mm - offset in z of FEBs to backpanel

// ASIC parameters
const Double_t asic_thickness = 0.25; // feb_thickness; // * 8.0; // 2.5 mm
const Double_t asic_width     = 3.0;  // 2.0;  1.0;   // 1 cm
//const Double_t asic_distance  = 0.4;  // 0.40; // a factor of width for ASIC pairs
Double_t asic_distance; //  = 0.40; // for 10 ASICs - a factor of width for ASIC pairs

const Double_t FrameWidth[2]    = { 1.5, 2.0 };   // Width of detector frames in cm
// mini - production
const Double_t DetectorSizeX[2] = { 57., 95.};   // => 54 x 54 cm2 & 91 x 91 cm2 active area
const Double_t DetectorSizeY[2] = { 57., 95.};   // quadratic modules
//// default
//const Double_t DetectorSizeX[2] = { 60., 100.};   // => 57 x 57 cm2 & 96 x 96 cm2 active area
//const Double_t DetectorSizeY[2] = { 60., 100.};   // quadratic modules

// Parameters tor the lattice grid reinforcing the entrance window
const Double_t lattice_o_width[2] = { 1.5, 2.0 };   // Width of outer lattice frame in cm
const Double_t lattice_i_width[2] = { 0.4, 0.4 };   // Width of inner lattice frame in cm
// Thickness (in z) of lattice frames in cm - see below

// z - geometry of TRD modules
const Double_t radiator_thickness     =  35.0;    // 35 cm thickness of radiator
const Double_t radiator_position      =  - LayerThickness/2. + radiator_thickness/2.;

const Double_t lattice_thickness      =   1.0;    // 0.9975;  // 1.0;  // 10 mm thick lattice frames
const Double_t lattice_position       =  radiator_position + radiator_thickness/2. + lattice_thickness/2.;

const Double_t kapton_thickness       =   0.0025; //  25 micron thickness of kapton
const Double_t kapton_position        =  lattice_position + lattice_thickness/2. + kapton_thickness/2.;

const Double_t gas_thickness          =   1.2;    //  12 mm thickness of gas
const Double_t gas_position           =  kapton_position + kapton_thickness/2. + gas_thickness/2.;

const Double_t padcopper_thickness    =   0.0025; //  25 micron thickness of copper pads
const Double_t padcopper_position     =  gas_position + gas_thickness/2. + padcopper_thickness/2.;

const Double_t padplane_thickness     =   0.0360; // 360 micron thickness of padplane
const Double_t padplane_position      =  padcopper_position + padcopper_thickness/2. + padplane_thickness/2.;

// backpanel components
const Double_t carbon_thickness       =   0.0190 * 2; // use 2 layers!!   // 190 micron thickness for 1 layer of carbon fibres
const Double_t honeycomb_thickness    =   2.3 - kapton_thickness - padcopper_thickness - padplane_thickness - carbon_thickness;    //  ~ 2.3 mm thickness of honeycomb
const Double_t honeycomb_position     =  padplane_position + padplane_thickness/2. + honeycomb_thickness/2.;
const Double_t carbon_position        =  honeycomb_position + honeycomb_thickness/2. + carbon_thickness/2.;

// readout boards
const  Double_t febvol_thickness      =  10.0;    // 10 cm length of FEBs
const  Double_t febvol_position       =  carbon_position + carbon_thickness/2. + febvol_thickness/2.;
//const  Double_t feb_thickness         =   0.50;  //  5.0 mm thickness of FEBs
const  Double_t feb_thickness         =   0.25;  // light //  2.5 mm thickness of FEBs

const Double_t frame_thickness        =  gas_thickness;   // frame covers gas volume: from kapton foil to pad plane
const Double_t frame_position         =  - LayerThickness /2. + radiator_thickness + lattice_thickness + kapton_thickness + frame_thickness/2.;


// Names of the different used materials which are used to build the modules
// The materials are defined in the global media.geo file 
const TString KeepingVolumeMedium     = "air";
const TString RadiatorVolumeMedium    = "TRDpefoam20";
const TString LatticeVolumeMedium     = "TRDG10";
const TString KaptonVolumeMedium      = "TRDkapton";
const TString GasVolumeMedium         = "TRDgas";
const TString PadCopperVolumeMedium   = "TRDcopper";
const TString PadPcbVolumeMedium      = "TRDG10";    // todo - put correct FEB material here
const TString HoneycombVolumeMedium   = "TRDaramide";
const TString CarbonVolumeMedium      = "TRDcarbon";
const TString FebVolumeMedium         = "TRDG10";    // todo - put correct FEB material here
const TString AsicVolumeMedium        = "air";       // todo - put correct ASIC material here
const TString FrameVolumeMedium       = "TRDG10";
const TString AluminiumVolumeMedium   = "aluminium";
//const TString MylarVolumeMedium       = "mylar";
//const TString RadiatorVolumeMedium    = "polypropylene";
//const TString ElectronicsVolumeMedium = "goldcoatedcopper";


// some global variables
TGeoManager* gGeoMan = NULL;  // Pointer to TGeoManager instance
TGeoVolume*  gModules[NofModuleTypes]; // Global storage for module types

// Forward declarations
void create_materials_from_media_file();
void create_trd_module(Int_t moduleType);
void create_detector_layers(Int_t layer);
void create_supports();


void Create_TRD_Geometry_v13b() {
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

  TGeoVolume* trd = new TGeoVolumeAssembly(geoVersion);
  top->AddNode(trd, 1);

  for (Int_t iModule = 0; iModule < NofModuleTypes; iModule++) {
     Int_t moduleType = iModule + 1;
     gModules[iModule] = create_trd_module(moduleType);
  }

  Int_t nLayer = 0;  // active layer counter
  for (Int_t iLayer = 0; iLayer < MaxLayers; iLayer++) {
    //    if ((iLayer != 0) && (iLayer != 3))  continue;  // first layer only - comment later on
    //    if (iLayer != 0) continue;  // first layer only - comment later on
    if (ShowLayer[iLayer])
    {
      PlaneId[iLayer]=++nLayer;
      create_detector_layers(iLayer);
      //      printf("calling layer %2d\n",iLayer);
    }
  }

  // TODO: remove or comment out
  // test PlaneId
  printf("generated TRD layers: ");
  for (Int_t iLayer = 0; iLayer < MaxLayers; iLayer++)
    if (ShowLayer[iLayer])
      printf(" %2d",PlaneId[iLayer]);
  printf("\n");

  if (IncludeSupports)
    create_supports();
  
  gGeoMan->CloseGeometry();
//  gGeoMan->CheckOverlaps(0.001);
//  gGeoMan->PrintOverlaps();
  gGeoMan->Test();

  TFile* outfile = new TFile(FileNameSim,"RECREATE");
  top->Write();      // use this as input to simulations (run_sim.C)
  outfile->Close();
  TFile* outfile = new TFile(FileNameGeo,"RECREATE");
  gGeoMan->Write();  // use this is you want GeoManager format in the output
  outfile->Close();
  top->Draw("ogl");
  //top->Raytrace();

//  cout << "Press Return to exit" << endl;
//  cin.get();
//  exit();
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

  FairGeoMedium* air              = geoMedia->getMedium(KeepingVolumeMedium);
  FairGeoMedium* pefoam20         = geoMedia->getMedium(RadiatorVolumeMedium);
  FairGeoMedium* G10              = geoMedia->getMedium(LatticeVolumeMedium);
  FairGeoMedium* kapton           = geoMedia->getMedium(KaptonVolumeMedium);
  FairGeoMedium* trdGas           = geoMedia->getMedium(GasVolumeMedium);
  FairGeoMedium* copper           = geoMedia->getMedium(PadCopperVolumeMedium);
  FairGeoMedium* carbon           = geoMedia->getMedium(CarbonVolumeMedium);
  FairGeoMedium* honeycomb        = geoMedia->getMedium(HoneycombVolumeMedium);
  FairGeoMedium* aluminium        = geoMedia->getMedium(AluminiumVolumeMedium);

//  FairGeoMedium* goldCoatedCopper = geoMedia->getMedium("goldcoatedcopper");
//  FairGeoMedium* polypropylene    = geoMedia->getMedium("polypropylene");
//  FairGeoMedium* mylar            = geoMedia->getMedium("mylar");

  geoBuild->createMedium(air);
  geoBuild->createMedium(pefoam20);
  geoBuild->createMedium(trdGas);
  geoBuild->createMedium(honeycomb);
  geoBuild->createMedium(carbon);
  geoBuild->createMedium(G10);
  geoBuild->createMedium(copper);
  geoBuild->createMedium(kapton);
  geoBuild->createMedium(aluminium);

//  geoBuild->createMedium(goldCoatedCopper);
//  geoBuild->createMedium(polypropylene);
//  geoBuild->createMedium(mylar);
}

TGeoVolume* create_trd_module(Int_t moduleType)
{
  Int_t type = ModuleType[moduleType - 1];
  Double_t sizeX = DetectorSizeX[type];
  Double_t sizeY = DetectorSizeY[type];
  Double_t frameWidth = FrameWidth[type];
  Double_t activeAreaX = sizeX - 2 * frameWidth;
  Double_t activeAreaY = sizeY - 2 * frameWidth;

  TGeoMedium* keepVolMed        = gGeoMan->GetMedium(KeepingVolumeMedium);
  TGeoMedium* radVolMed         = gGeoMan->GetMedium(RadiatorVolumeMedium);
  TGeoMedium* latticeVolMed     = gGeoMan->GetMedium(LatticeVolumeMedium);
  TGeoMedium* kaptonVolMed      = gGeoMan->GetMedium(KaptonVolumeMedium);
  TGeoMedium* gasVolMed         = gGeoMan->GetMedium(GasVolumeMedium);
  TGeoMedium* padcopperVolMed   = gGeoMan->GetMedium(PadCopperVolumeMedium);
  TGeoMedium* padpcbVolMed      = gGeoMan->GetMedium(PadPcbVolumeMedium);
  TGeoMedium* honeycombVolMed   = gGeoMan->GetMedium(HoneycombVolumeMedium);
  TGeoMedium* carbonVolMed      = gGeoMan->GetMedium(CarbonVolumeMedium);
//  TGeoMedium* mylarVolMed       = gGeoMan->GetMedium(MylarVolumeMedium);
//  TGeoMedium* electronicsVolMed = gGeoMan->GetMedium(ElectronicsVolumeMedium);
  TGeoMedium* frameVolMed       = gGeoMan->GetMedium(FrameVolumeMedium);
  TGeoMedium* febVolMed         = gGeoMan->GetMedium(FebVolumeMedium);
  TGeoMedium* asicVolMed        = gGeoMan->GetMedium(AsicVolumeMedium);
//  TGeoMedium* aluminiumVolMed   = gGeoMan->GetMedium(AluminiumVolumeMedium);

  TString name = Form("module%d", moduleType);
  TGeoVolume* module = new TGeoVolumeAssembly(name);


  if(IncludeRadiator)
   {
     // Radiator
     //   TGeoBBox* trd_radiator = new TGeoBBox("", activeAreaX /2., activeAreaY /2., radiator_thickness /2.);
     TGeoBBox* trd_radiator = new TGeoBBox("", sizeX /2., sizeY /2., radiator_thickness /2.);
     TGeoVolume* trdmod1_radvol = new TGeoVolume(Form("module%d_radiator", moduleType), trd_radiator, radVolMed);
     //     TGeoVolume* trdmod1_radvol = new TGeoVolume(Form("trd1mod%dradiator", moduleType), trd_radiator, radVolMed);
     trdmod1_radvol->SetLineColor(kBlue);
     trdmod1_radvol->SetTransparency(70);  // (60);  // (70);  // set transparency for the TRD
     TGeoTranslation* trd_radiator_trans = new TGeoTranslation("", 0., 0., radiator_position);
     module->AddNode(trdmod1_radvol, 1, trd_radiator_trans);
   }

   // Lattice grid
   if(IncludeLattice) 
   {

     if (type==0)  // inner modules
     {
       //     printf("lattice type %d\n", type);
       // drift window - lattice grid - sprossenfenster
       TGeoBBox *trd_lattice_mod0_ho = new TGeoBBox("S0ho", sizeX/2.,                       lattice_o_width[type]/2., lattice_thickness/2.);  // horizontal
       TGeoBBox *trd_lattice_mod0_hi = new TGeoBBox("S0hi", sizeX/2.-lattice_o_width[type], lattice_i_width[type]/2., lattice_thickness/2.);  // horizontal
       //       TGeoBBox *trd_lattice_mod0_hi = new TGeoBBox("S0hi", sizeX/2., lattice_i_width[type]/2., lattice_thickness/2.);  // horizontal
    
       TGeoBBox *trd_lattice_mod0_vo = new TGeoBBox("S0vo", lattice_o_width[type]/2., sizeX/2.,                       lattice_thickness/2.);  // vertical
       TGeoBBox *trd_lattice_mod0_vi = new TGeoBBox("S0vi", lattice_i_width[type]/2., sizeX/2.-lattice_o_width[type], lattice_thickness/2.);  // vertical
       //       TGeoBBox *trd_lattice_mod0_vi = new TGeoBBox("S0vi", lattice_i_width[type]/2., sizeX/2., lattice_thickness/2.);  // vertical
    
       TGeoTranslation *t010 = new TGeoTranslation("t010", 0.,  (1.00*activeAreaY/2.+lattice_o_width[type]/2.), 0);
       t010->RegisterYourself();
       TGeoTranslation *t011 = new TGeoTranslation("t011", 0.,  (0.60*activeAreaY/2.)                         , 0);
       t011->RegisterYourself();
       TGeoTranslation *t012 = new TGeoTranslation("t012", 0.,  (0.20*activeAreaY/2.)                         , 0);
       t012->RegisterYourself();
       TGeoTranslation *t013 = new TGeoTranslation("t013", 0., -(0.20*activeAreaY/2.)                         , 0);
       t013->RegisterYourself();
       TGeoTranslation *t014 = new TGeoTranslation("t014", 0., -(0.60*activeAreaY/2.)                         , 0);
       t014->RegisterYourself();
       TGeoTranslation *t015 = new TGeoTranslation("t015", 0., -(1.00*activeAreaY/2.+lattice_o_width[type]/2.), 0);
       t015->RegisterYourself();
    
       TGeoTranslation *t020 = new TGeoTranslation("t020",  (1.00*activeAreaX/2.+lattice_o_width[type]/2.), 0., 0);
       t020->RegisterYourself();
       TGeoTranslation *t021 = new TGeoTranslation("t021",  (0.60*activeAreaX/2.)                         , 0., 0);
       t021->RegisterYourself();
       TGeoTranslation *t022 = new TGeoTranslation("t022",  (0.20*activeAreaX/2.)                         , 0., 0);
       t022->RegisterYourself();
       TGeoTranslation *t023 = new TGeoTranslation("t023", -(0.20*activeAreaX/2.)                         , 0., 0);
       t023->RegisterYourself();
       TGeoTranslation *t024 = new TGeoTranslation("t024", -(0.60*activeAreaX/2.)                         , 0., 0);
       t024->RegisterYourself();
       TGeoTranslation *t025 = new TGeoTranslation("t025", -(1.00*activeAreaX/2.+lattice_o_width[type]/2.), 0., 0);
       t025->RegisterYourself();
    
    //   // with additional cross in the center - a la Roland
    //   Double_t Lattice_1_width   = 0.1; // Width of inner lattice frame in cm
    //   TGeoBBox *trd_lattice_h1 = new TGeoBBox("Sh1", activeAreaX/5/2., Lattice_1_width/2., lattice_thickness/2.);  // horizontal
    //   TGeoBBox *trd_lattice_v1 = new TGeoBBox("Sv1", Lattice_1_width/2., activeAreaX/5/2., lattice_thickness/2.);  // vertical
    //   TGeoCompositeShape *cs = new TGeoCompositeShape("cs", 
    //   "(Sho:t010 + Shi:t011 + Shi:t012 + Shi:t013 + Shi:t014 + Sho:t015 + Svo:t020 + Svi:t021 + Svi:t022 + Svi:t023 + Svi:t024 + Svo:t025 + Sh1 + Sv1)");
    
       if (IncludeGasHoles)
       {
  	 // start gas inlets                                                                                                                                                       
  	 // gas hole (additive)                                                                                                                                                    
  	 float hole_irad = 0.75;
  	 float hole_orad = hole_irad + 1.0;
  	 float hole_xoff = hole_irad;
  	 float hole_yoff = hole_irad;
  	 float hole_ang1 = 0.0;
  	 float hole_ang2 = 360.0;
  
  	 // frame around gas hole                                                                                                                                                  
  	 float tube_irad = sqrt(2)*hole_irad + hole_irad;  // sqrt(2)*r+1                                                                                                          
  	 float tube_orad = sqrt(2)*hole_irad + hole_orad;  // sqrt(2)*r+2                                                                                                          
  	 float tube_xoff = 0.0;
  	 float tube_yoff = 0.0;
  
  	 TGeoTubeSeg *trd_corner_001 = new TGeoTubeSeg("Co01", tube_irad, tube_orad, lattice_thickness/2, 0.0, 90.0);
  	 TGeoTranslation *t030       = new TGeoTranslation("t030", -(1.0*activeAreaX/2.-tube_xoff), -(1.0*activeAreaY/2.-tube_yoff), 0);
  	 t030->RegisterYourself();
  
  	 TGeoTubeSeg *trd_hole_001   = new TGeoTubeSeg("Ho01", hole_irad, hole_orad, lattice_thickness/2, hole_ang1, hole_ang2);
  	 TGeoTranslation *t031       = new TGeoTranslation("t031", -(1.0*activeAreaX/2.-hole_xoff), -(1.0*activeAreaY/2.-hole_yoff), 0);
  	 t031->RegisterYourself();
  
  	 TGeoTubeSeg *trd_corner_002 = new TGeoTubeSeg("Co02", tube_irad, tube_orad, lattice_thickness/2, 90.0, 180.0);
  	 TGeoTranslation *t032       = new TGeoTranslation("t032", +(1.0*activeAreaX/2.-tube_xoff), -(1.0*activeAreaY/2.-tube_yoff), 0);
  	 t032->RegisterYourself();
  
  	 TGeoTubeSeg *trd_hole_002   = new TGeoTubeSeg("Ho02", hole_irad, hole_orad, lattice_thickness/2, hole_ang1, hole_ang2);
  	 TGeoTranslation *t033       = new TGeoTranslation("t033", +(1.0*activeAreaX/2.-hole_xoff), -(1.0*activeAreaY/2.-hole_yoff), 0);
  	 t033->RegisterYourself();
  
  	 TGeoTubeSeg *trd_corner_003 = new TGeoTubeSeg("Co03", tube_irad, tube_orad, lattice_thickness/2, 180.0, 270.0);
  	 TGeoTranslation *t034       = new TGeoTranslation("t034", +(1.0*activeAreaX/2.-tube_xoff), +(1.0*activeAreaY/2.-tube_yoff), 0);
  	 t034->RegisterYourself();
  
  	 TGeoTubeSeg *trd_hole_003   = new TGeoTubeSeg("Ho03", hole_irad, hole_orad, lattice_thickness/2, hole_ang1, hole_ang2);
  	 TGeoTranslation *t035       = new TGeoTranslation("t035", +(1.0*activeAreaX/2.-hole_xoff), +(1.0*activeAreaY/2.-hole_yoff), 0);
  	 t035->RegisterYourself();
  
  	 TGeoTubeSeg *trd_corner_004 = new TGeoTubeSeg("Co04", tube_irad, tube_orad, lattice_thickness/2, 270.0, 360.0);
  	 TGeoTranslation *t036       = new TGeoTranslation("t036", -(1.0*activeAreaX/2.-tube_xoff), +(1.0*activeAreaY/2.-tube_yoff), 0);
  	 t036->RegisterYourself();
  
  	 TGeoTubeSeg *trd_hole_004   = new TGeoTubeSeg("Ho04", hole_irad, hole_orad, lattice_thickness/2, hole_ang1, hole_ang2);
  	 TGeoTranslation *t037       = new TGeoTranslation("t037", -(1.0*activeAreaX/2.-hole_xoff), +(1.0*activeAreaY/2.-hole_yoff), 0);
  	 t037->RegisterYourself();
  	 // end gas inlets                                                                                                                                                         
  
           TGeoCompositeShape *lattice_grid = new TGeoCompositeShape("lattice_grid", 
           "(S0ho:t010 + S0hi:t011 + S0hi:t012 + S0hi:t013 + S0hi:t014 + S0ho:t015 + \
             S0vo:t020 + S0vi:t021 + S0vi:t022 + S0vi:t023 + S0vi:t024 + S0vo:t025 + \
             Ho01:t031 + Ho02:t033 + Ho03:t035 + Ho04:t037 + \
             Co01:t030 + Co02:t032 + Co03:t034 + Co04:t036)");
       }
       else  // no gas holes in lattice grid
  
       TGeoCompositeShape *lattice_grid = new TGeoCompositeShape("lattice_grid", 
       "(S0ho:t010 + S0hi:t011 + S0hi:t012 + S0hi:t013 + S0hi:t014 + S0ho:t015 + \
         S0vo:t020 + S0vi:t021 + S0vi:t022 + S0vi:t023 + S0vi:t024 + S0vo:t025)");
       TGeoVolume *trdmod0_lattice = new TGeoVolume(Form("module%d_latticegrid", moduleType), lattice_grid, latticeVolMed);
       //       TGeoVolume *trdmod0_lattice = new TGeoVolume(Form("trd1mod%dlatticegrid", moduleType), lattice_grid, latticeVolMed);
       trdmod0_lattice->SetLineColor(kYellow);
       TGeoTranslation *trd_lattice_trans = new TGeoTranslation("", 0., 0., lattice_position);
       module->AddNode(trdmod0_lattice, 1, trd_lattice_trans);
     }
  
     else if (type==1)  // outer modules
  
     {
       //     printf("lattice type %d\n", type);
       // drift window - lattice grid - sprossenfenster
       TGeoBBox *trd_lattice_mod1_ho = new TGeoBBox("S1ho", sizeX/2.,                       lattice_o_width[type]/2., lattice_thickness/2.);  // horizontal
       TGeoBBox *trd_lattice_mod1_hi = new TGeoBBox("S1hi", sizeX/2.-lattice_o_width[type], lattice_i_width[type]/2., lattice_thickness/2.);  // horizontal
       //       TGeoBBox *trd_lattice_mod1_hi = new TGeoBBox("S1hi", sizeX/2., lattice_i_width[type]/2., lattice_thickness/2.);  // horizontal
    
       TGeoBBox *trd_lattice_mod1_vo = new TGeoBBox("S1vo", lattice_o_width[type]/2., sizeX/2.,                       lattice_thickness/2.);  // vertical
       TGeoBBox *trd_lattice_mod1_vi = new TGeoBBox("S1vi", lattice_i_width[type]/2., sizeX/2.-lattice_o_width[type], lattice_thickness/2.);  // vertical
       //       TGeoBBox *trd_lattice_mod1_vi = new TGeoBBox("S1vi", lattice_i_width[type]/2., sizeX/2., lattice_thickness/2.);  // vertical
    
       TGeoTranslation *t110 = new TGeoTranslation("t110", 0.,  (1.00*activeAreaY/2.+lattice_o_width[type]/2.), 0);
       t110->RegisterYourself();
       TGeoTranslation *t111 = new TGeoTranslation("t111", 0.,  (0.75*activeAreaY/2.)                         , 0);
       t111->RegisterYourself();
       TGeoTranslation *t112 = new TGeoTranslation("t112", 0.,  (0.50*activeAreaY/2.)                         , 0);
       t112->RegisterYourself();
       TGeoTranslation *t113 = new TGeoTranslation("t113", 0.,  (0.25*activeAreaY/2.)                         , 0);
       t113->RegisterYourself();
       TGeoTranslation *t114 = new TGeoTranslation("t114", 0.,  (0.00*activeAreaY/2.)                         , 0);
       t114->RegisterYourself();
       TGeoTranslation *t115 = new TGeoTranslation("t115", 0., -(0.25*activeAreaY/2.)                         , 0);
       t115->RegisterYourself();
       TGeoTranslation *t116 = new TGeoTranslation("t116", 0., -(0.50*activeAreaY/2.)                         , 0);
       t116->RegisterYourself();
       TGeoTranslation *t117 = new TGeoTranslation("t117", 0., -(0.75*activeAreaY/2.)                         , 0);
       t117->RegisterYourself();
       TGeoTranslation *t118 = new TGeoTranslation("t118", 0., -(1.00*activeAreaY/2.+lattice_o_width[type]/2.), 0);
       t118->RegisterYourself();
    
       TGeoTranslation *t120 = new TGeoTranslation("t120",  (1.00*activeAreaX/2.+lattice_o_width[type]/2.), 0., 0);
       t120->RegisterYourself();
       TGeoTranslation *t121 = new TGeoTranslation("t121",  (0.75*activeAreaX/2.)                         , 0., 0);
       t121->RegisterYourself();
       TGeoTranslation *t122 = new TGeoTranslation("t122",  (0.50*activeAreaX/2.)                         , 0., 0);
       t122->RegisterYourself();
       TGeoTranslation *t123 = new TGeoTranslation("t123",  (0.25*activeAreaX/2.)                         , 0., 0);
       t123->RegisterYourself();
       TGeoTranslation *t124 = new TGeoTranslation("t124",  (0.00*activeAreaX/2.)                         , 0., 0);
       t124->RegisterYourself();
       TGeoTranslation *t125 = new TGeoTranslation("t125", -(0.25*activeAreaX/2.)                         , 0., 0);
       t125->RegisterYourself();
       TGeoTranslation *t126 = new TGeoTranslation("t126", -(0.50*activeAreaX/2.)                         , 0., 0);
       t126->RegisterYourself();
       TGeoTranslation *t127 = new TGeoTranslation("t127", -(0.75*activeAreaX/2.)                         , 0., 0);
       t127->RegisterYourself();
       TGeoTranslation *t128 = new TGeoTranslation("t128", -(1.00*activeAreaX/2.+lattice_o_width[type]/2.), 0., 0);
       t128->RegisterYourself();
    
    //   // with additional cross in the center - a la Roland
    //   Double_t Lattice_1_width   = 0.1; // Width of inner lattice frame in cm
    //   TGeoBBox *trd_lattice_h1 = new TGeoBBox("Sh1", activeAreaX/5/2., Lattice_1_width/2., lattice_thickness/2.);  // horizontal
    //   TGeoBBox *trd_lattice_v1 = new TGeoBBox("Sv1", Lattice_1_width/2., activeAreaX/5/2., lattice_thickness/2.);  // vertical
    //   TGeoCompositeShape *cs = new TGeoCompositeShape("cs", 
    //   "(Sho:t110 + Shi:t111 + Shi:t112 + Shi:t113 + Shi:t114 + Sho:t115 + Svo:t120 + Svi:t121 + Svi:t122 + Svi:t123 + Svi:t124 + Svo:t125 + Sh1 + Sv1)");
  
       if (IncludeGasHoles)
       {
  	 // start gas inlets                                                                                                                                                       
  	 // gas hole (additive)                                                                                                                                                    
  	 float hole_irad = 0.75;
  	 float hole_orad = hole_irad + 1.0;
  	 float hole_xoff = hole_irad;
  	 float hole_yoff = hole_irad;
  	 float hole_ang1 = 0.0;
  	 float hole_ang2 = 360.0;
  
  	 // frame around gas hole                                                                                                                                                  
  	 float tube_irad = sqrt(2)*hole_irad + hole_irad;  // sqrt(2)*r+1                                                                                                          
  	 float tube_orad = sqrt(2)*hole_irad + hole_orad;  // sqrt(2)*r+2                                                                                                          
  	 float tube_xoff = 0.0;
  	 float tube_yoff = 0.0;
  
  	 TGeoTubeSeg *trd_corner_101 = new TGeoTubeSeg("Co11", tube_irad, tube_orad, lattice_thickness/2, 0.0, 90.0);
  	 TGeoTranslation *t130       = new TGeoTranslation("t130", -(1.0*activeAreaX/2.-tube_xoff), -(1.0*activeAreaY/2.-tube_yoff), 0);
  	 t130->RegisterYourself();
  
  	 TGeoTubeSeg *trd_hole_101   = new TGeoTubeSeg("Ho11", hole_irad, hole_orad, lattice_thickness/2, hole_ang1, hole_ang2);
  	 TGeoTranslation *t131       = new TGeoTranslation("t131", -(1.0*activeAreaX/2.-hole_xoff), -(1.0*activeAreaY/2.-hole_yoff), 0);
  	 t131->RegisterYourself();
  
  	 TGeoTubeSeg *trd_corner_102 = new TGeoTubeSeg("Co12", tube_irad, tube_orad, lattice_thickness/2, 90.0, 180.0);
  	 TGeoTranslation *t132       = new TGeoTranslation("t132", +(1.0*activeAreaX/2.-tube_xoff), -(1.0*activeAreaY/2.-tube_yoff), 0);
  	 t132->RegisterYourself();
  
  	 TGeoTubeSeg *trd_hole_102   = new TGeoTubeSeg("Ho12", hole_irad, hole_orad, lattice_thickness/2, hole_ang1, hole_ang2);
  	 TGeoTranslation *t133       = new TGeoTranslation("t133", +(1.0*activeAreaX/2.-hole_xoff), -(1.0*activeAreaY/2.-hole_yoff), 0);
  	 t133->RegisterYourself();
  
  	 TGeoTubeSeg *trd_corner_103 = new TGeoTubeSeg("Co13", tube_irad, tube_orad, lattice_thickness/2, 180.0, 270.0);
  	 TGeoTranslation *t134       = new TGeoTranslation("t134", +(1.0*activeAreaX/2.-tube_xoff), +(1.0*activeAreaY/2.-tube_yoff), 0);
  	 t134->RegisterYourself();
  
  	 TGeoTubeSeg *trd_hole_103   = new TGeoTubeSeg("Ho13", hole_irad, hole_orad, lattice_thickness/2, hole_ang1, hole_ang2);
  	 TGeoTranslation *t135       = new TGeoTranslation("t135", +(1.0*activeAreaX/2.-hole_xoff), +(1.0*activeAreaY/2.-hole_yoff), 0);
  	 t135->RegisterYourself();
  
  	 TGeoTubeSeg *trd_corner_104 = new TGeoTubeSeg("Co14", tube_irad, tube_orad, lattice_thickness/2, 270.0, 360.0);
  	 TGeoTranslation *t136       = new TGeoTranslation("t136", -(1.0*activeAreaX/2.-tube_xoff), +(1.0*activeAreaY/2.-tube_yoff), 0);
  	 t136->RegisterYourself();
  
  	 TGeoTubeSeg *trd_hole_104   = new TGeoTubeSeg("Ho14", hole_irad, hole_orad, lattice_thickness/2, hole_ang1, hole_ang2);
  	 TGeoTranslation *t137       = new TGeoTranslation("t137", -(1.0*activeAreaX/2.-hole_xoff), +(1.0*activeAreaY/2.-hole_yoff), 0);
  	 t137->RegisterYourself();
  	 // end gas inlets                                                                                                                                                         
  
           TGeoCompositeShape *lattice_grid = new TGeoCompositeShape("lattice_grid",
           "(S1ho:t110 + S1hi:t111 + S1hi:t112 + S1hi:t113 + S1hi:t114 + S1hi:t115 + S1hi:t116 + S1hi:t117 + S1ho:t118 + \
             S1vo:t120 + S1vi:t121 + S1vi:t122 + S1vi:t123 + S1vi:t124 + S1vi:t125 + S1vi:t126 + S1vi:t127 + S1vo:t128 + \
             Ho11:t131 + Ho12:t133 + Ho13:t135 + Ho14:t137 + \
             Co11:t130 + Co12:t132 + Co13:t134 + Co14:t136)");
       }
       else  // no gas holes in lattice grid
         TGeoCompositeShape *lattice_grid = new TGeoCompositeShape("lattice_grid",
         "(S1ho:t110 + S1hi:t111 + S1hi:t112 + S1hi:t113 + S1hi:t114 + S1hi:t115 + S1hi:t116 + S1hi:t117 + S1ho:t118 + \
           S1vo:t120 + S1vi:t121 + S1vi:t122 + S1vi:t123 + S1vi:t124 + S1vi:t125 + S1vi:t126 + S1vi:t127 + S1vo:t128)");
       TGeoVolume *trdmod1_lattice = new TGeoVolume(Form("module%d_latticegrid", moduleType), lattice_grid, latticeVolMed);
       //       TGeoVolume *trdmod1_lattice = new TGeoVolume(Form("trd1mod%dlatticegrid", moduleType), lattice_grid, latticeVolMed);
       trdmod1_lattice->SetLineColor(kYellow);
       TGeoTranslation *trd_lattice_trans = new TGeoTranslation("", 0., 0., lattice_position);
       module->AddNode(trdmod1_lattice, 1, trd_lattice_trans);
     }

   }  // with lattice grid 

   // Kapton Foil
   TGeoBBox* trd_kapton = new TGeoBBox("", sizeX /2., sizeY /2., kapton_thickness /2.);
   TGeoVolume* trdmod1_kaptonvol = new TGeoVolume(Form("module%d_kaptonfoil", moduleType), trd_kapton, kaptonVolMed);
   //   TGeoVolume* trdmod1_kaptonvol = new TGeoVolume(Form("trd1mod%dkapton", moduleType), trd_kapton, kaptonVolMed);
   trdmod1_kaptonvol->SetLineColor(kGreen);
   TGeoTranslation* trd_kapton_trans = new TGeoTranslation("", 0., 0., kapton_position);
   module->AddNode(trdmod1_kaptonvol, 1, trd_kapton_trans);

   // start of Frame in z
   // Gas
   TGeoBBox* trd_gas = new TGeoBBox("", activeAreaX /2., activeAreaY /2., gas_thickness /2.);
   TGeoVolume* trdmod1_gasvol = new TGeoVolume(Form("module%d_gas", moduleType), trd_gas, gasVolMed);
   //   TGeoVolume* trdmod1_gasvol = new TGeoVolume(Form("trd1mod%dgas", moduleType), trd_gas, gasVolMed);
   //   trdmod1_gasvol->SetLineColor(kBlue);
   trdmod1_gasvol->SetLineColor(kGreen); // to avoid blue overlaps in the screenshots
   TGeoTranslation* trd_gas_trans = new TGeoTranslation("", 0., 0., gas_position);
   module->AddNode(trdmod1_gasvol, 1, trd_gas_trans);
   // end of Frame in z


   // frame1
   TGeoBBox* trd_frame1 = new TGeoBBox("", sizeX /2., frameWidth /2., frame_thickness/2.);
   TGeoVolume* trdmod1_frame1vol = new TGeoVolume(Form("module%d_frame1", moduleType), trd_frame1, frameVolMed);
   //   TGeoVolume* trdmod1_frame1vol = new TGeoVolume(Form("trd1mod%dframe1", moduleType), trd_frame1, frameVolMed);
   trdmod1_frame1vol->SetLineColor(kRed);

   // translations 
   TGeoTranslation* trd_frame1_trans = new TGeoTranslation("", 0., activeAreaY /2. + frameWidth /2., frame_position);
   module->AddNode(trdmod1_frame1vol, 1, trd_frame1_trans);
   trd_frame1_trans = new TGeoTranslation("", 0., -(activeAreaY /2. + frameWidth /2.), frame_position);
   module->AddNode(trdmod1_frame1vol, 2, trd_frame1_trans);


   // frame2
   TGeoBBox* trd_frame2 = new TGeoBBox("", frameWidth /2., activeAreaY /2., frame_thickness /2.);
   TGeoVolume* trdmod1_frame2vol = new TGeoVolume(Form("module%d_frame2", moduleType), trd_frame2, frameVolMed);
   //   TGeoVolume* trdmod1_frame2vol = new TGeoVolume(Form("trd1mod%dframe2", moduleType), trd_frame2, frameVolMed);
   trdmod1_frame2vol->SetLineColor(kRed);

   // translations 
   TGeoTranslation* trd_frame2_trans = new TGeoTranslation("", activeAreaX /2. + frameWidth /2., 0., frame_position);
   module->AddNode(trdmod1_frame2vol, 1, trd_frame2_trans);
   trd_frame2_trans = new TGeoTranslation("", -(activeAreaX /2. + frameWidth /2.), 0., frame_position);
   module->AddNode(trdmod1_frame2vol, 2, trd_frame2_trans);


   // Pad Copper
   TGeoBBox *trd_padcopper = new TGeoBBox("", sizeX /2., sizeY /2., padcopper_thickness /2.);
   TGeoVolume* trdmod1_padcoppervol = new TGeoVolume(Form("module%d_padcopper", moduleType), trd_padcopper, padcopperVolMed);
   //   TGeoVolume* trdmod1_padcoppervol = new TGeoVolume(Form("trd1mod%dpadcopper", moduleType), trd_padcopper, padcopperVolMed);
   trdmod1_padcoppervol->SetLineColor(kOrange);
   TGeoTranslation *trd_padcopper_trans = new TGeoTranslation("", 0., 0., padcopper_position);
   module->AddNode(trdmod1_padcoppervol, 1, trd_padcopper_trans);

   // Pad Plane
   TGeoBBox* trd_padpcb = new TGeoBBox("", sizeX /2., sizeY /2., padplane_thickness /2.);
   TGeoVolume* trdmod1_padpcbvol = new TGeoVolume(Form("module%d_padplane", moduleType), trd_padpcb, padpcbVolMed);
   //   TGeoVolume* trdmod1_padpcbvol = new TGeoVolume(Form("trd1mod%dpadplane", moduleType), trd_padpcb, padpcbVolMed);
   trdmod1_padpcbvol->SetLineColor(kBlue);
   TGeoTranslation *trd_padpcb_trans = new TGeoTranslation("", 0., 0., padplane_position);
   module->AddNode(trdmod1_padpcbvol, 1, trd_padpcb_trans);

   // Honeycomb
   TGeoBBox* trd_honeycomb = new TGeoBBox("", sizeX /2., sizeY /2., honeycomb_thickness /2.);
   TGeoVolume* trdmod1_honeycombvol = new TGeoVolume(Form("module%d_honeycomb", moduleType), trd_honeycomb, honeycombVolMed);
   //   TGeoVolume* trdmod1_honeycombvol = new TGeoVolume(Form("trd1mod%dhoneycomb", moduleType), trd_honeycomb, honeycombVolMed);
   trdmod1_honeycombvol->SetLineColor(kOrange);
   TGeoTranslation* trd_honeycomb_trans = new TGeoTranslation("", 0., 0., honeycomb_position);
   module->AddNode(trdmod1_honeycombvol, 1, trd_honeycomb_trans);

   // Carbon fiber layers
   TGeoBBox* trd_carbon = new TGeoBBox("", sizeX /2., sizeY /2., carbon_thickness /2.);
   TGeoVolume* trdmod1_carbonvol = new TGeoVolume(Form("module%d_carbonsheet", moduleType), trd_carbon, carbonVolMed);
   //   TGeoVolume* trdmod1_carbonvol = new TGeoVolume(Form("trd1mod%dcarbon", moduleType), trd_carbon, carbonVolMed);
   trdmod1_carbonvol->SetLineColor(kGreen);
   TGeoTranslation* trd_carbon_trans = new TGeoTranslation("", 0., 0., carbon_position);
   module->AddNode(trdmod1_carbonvol, 1, trd_carbon_trans);


   // FEBs
   if (IncludeFebs) {

      // assemblies
      TGeoVolumeAssembly* trd_feb_vol = new TGeoVolumeAssembly(Form("module%d_febvol", moduleType));  // the mother volume of all FEBs
      TGeoVolumeAssembly* trd_feb_box = new TGeoVolumeAssembly(Form("module%d_febbox", moduleType)); // volume for inclined FEBs, then shifted along y
      //TGeoVolumeAssembly* trd_feb_vol = new TGeoVolumeAssembly(Form("trd1mod%dfebvol", moduleType));  // the mother volume of all FEBs
      //TGeoVolumeAssembly* trd_feb_box = new TGeoVolumeAssembly(Form("trd1mod%dfebbox", moduleType)); // volume for inclined FEBs, then shifted along y

      // translations + rotations
      TGeoTranslation *trd_feb_trans1;     // center to corner
      TGeoRotation    *trd_feb_rotation;   // rotation around x axis
      TGeoTranslation *trd_feb_trans2;     // corner back
      TGeoTranslation *trd_feb_y_position; // shift to y position on TRD
//      TGeoTranslation *trd_feb_null;       // no displacement

      Double_t feb_pos;
      Double_t feb_pos_y;

// replaced by matrix operation (see below)
//  //      Double_t yback, zback;
//  //      TGeoCombiTrans  *trd_feb_placement;
//  //      // fix Z back offset 0.3 at some point
//  //      yback = -    sin(feb_rotation_angle/180*3.141)  * febvol_thickness /2.;
//  //      zback = - (1-cos(feb_rotation_angle/180*3.141)) * febvol_thickness /2. + 0.3;
//  //      trd_feb_placement = new TGeoCombiTrans(0, feb_pos_y + yback, zback, trd_feb_rotation);
//  //      trd_feb_box->AddNode(trdmod1_feb, iFeb+1, trd_feb_placement);

//      trd_feb_null       = new TGeoTranslation("", 0., 0., 0.);  // empty operation
      trd_feb_trans1     = new TGeoTranslation("", 0.,-feb_thickness/2.,-febvol_thickness/2.);  // move bottom right corner to center
      trd_feb_trans2     = new TGeoTranslation("", 0., feb_thickness/2., febvol_thickness/2.);  // move bottom right corner back
      trd_feb_rotation   = new TGeoRotation(); trd_feb_rotation->RotateX(feb_rotation_angle);

      TGeoHMatrix *incline_feb = new TGeoHMatrix("");

//        (*incline_feb) = (*trd_feb_null);        // OK
//        (*incline_feb) = (*trd_feb_y_position);  // OK
//        (*incline_feb) = (*trd_feb_trans1);      // OK
//        (*incline_feb) = (*trd_feb_trans1) * (*trd_feb_y_position);  // OK
//        (*incline_feb) = (*trd_feb_trans1) * (*trd_feb_trans2);      // OK
//        (*incline_feb) = (*trd_feb_trans1) * (*trd_feb_rotation);    // OK
//        (*incline_feb) =  (*trd_feb_trans1) * (*trd_feb_rotation) * (*trd_feb_trans2) * (*trd_feb_y_position);  // not OK 
          // trd_feb_y_position is displaced in rotated coordinate system

      // matrix operation to rotate FEB PCB around its corner on the backanel 
      (*incline_feb) = (*trd_feb_trans1) * (*trd_feb_rotation) * (*trd_feb_trans2);  // OK

      // Create all FEBs and place them in an assembly which will be added to the TRD module
      TGeoBBox* trd_feb = new TGeoBBox("", activeAreaX/2., feb_thickness/2., febvol_thickness/2.);   // the FEB itself - as a cuboid
      TGeoVolume* trdmod1_feb = new TGeoVolume(Form("module%d_feb", moduleType), trd_feb, febVolMed);  // the FEB made of a certain medium
      //      TGeoVolume* trdmod1_feb = new TGeoVolume(Form("trd1mod%dfeb", moduleType), trd_feb, febVolMed);  // the FEB made of a certain medium
      trdmod1_feb->SetLineColor(kYellow);    // set yellow color
      trd_feb_box->AddNode(trdmod1_feb, 1, incline_feb);  
      // now we have an inclined FEB

      // ASICs
      Double_t asic_pos;
      Double_t asic_pos_x;

      if (IncludeAsics) {

        // put many ASICs on each inclined FEB
        TGeoBBox* trd_asic = new TGeoBBox("", asic_width/2., asic_thickness/2., asic_width/2.);              // ASIC dimensions
        // TODO: use Silicon as ASICs material
        TGeoVolume* trdmod1_asic = new TGeoVolume(Form("module%d_asic", moduleType), trd_asic, asicVolMed);   // the ASIC made of a certain medium
	//        TGeoVolume* trdmod1_asic = new TGeoVolume(Form("trd1mod%dasic", moduleType), trd_asic, asicVolMed);   // the ASIC made of a certain medium
        trdmod1_asic->SetLineColor(kBlue);                                                                   // set blue color for ASICs
  
        Int_t nofAsics   = AsicsPerFeb[ moduleType - 1 ] % 100;
        Int_t groupAsics = AsicsPerFeb[ moduleType - 1 ] / 100;   // either 1 or 2

        if ((nofAsics == 16) && (activeAreaX < 60))
          asic_distance = 0.0;  // for 57 cm  // 0.1;  // for 60 cm
        else
          asic_distance = 0.4;

        for (Int_t iAsic = 0; iAsic < (nofAsics / groupAsics); iAsic++) {

          if (groupAsics == 1)   // single ASICs 
	  {
            asic_pos   = (iAsic + 0.5) / nofAsics - 0.5;   // equal spacing of ASICs on the FEB, e.g. for no=3 : -1/3, 0, +1/3
            asic_pos_x = asic_pos * activeAreaX;
            trd_asic_trans1     = new TGeoTranslation("", asic_pos_x, feb_thickness/2.+asic_thickness/2., 0.);  // move asic on top of FEB
            TGeoHMatrix *incline_asic = new TGeoHMatrix("");
            (*incline_asic) = (*trd_asic_trans1) * (*incline_feb);
            trd_feb_box->AddNode(trdmod1_asic, iAsic+1, incline_asic);  // now we have ASICs on the inclined FEB
          }

          if (groupAsics == 2)   // pairs of ASICs
	  {
            asic_pos   = (iAsic + 0.5) / (nofAsics / groupAsics) - 0.5;   // equal spacing of ASICs on the FEB, e.g. for no=3 : -1/3, 0, +1/3

            asic_pos_x = asic_pos * activeAreaX + (0.5 + asic_distance/2.) * asic_width;
            trd_asic_trans1     = new TGeoTranslation("", asic_pos_x, feb_thickness/2.+asic_thickness/2., 0.);  // move asic on top of FEB
            TGeoHMatrix *incline_asic = new TGeoHMatrix("");
            (*incline_asic) = (*trd_asic_trans1) * (*incline_feb);
            trd_feb_box->AddNode(trdmod1_asic, 2*iAsic+1, incline_asic);  // now we have ASICs on the inclined FEB

            asic_pos_x = asic_pos * activeAreaX - (0.5 + asic_distance/2.) * asic_width;
            trd_asic_trans1     = new TGeoTranslation("", asic_pos_x, feb_thickness/2.+asic_thickness/2., 0.);  // move asic on top of FEB
            TGeoHMatrix *incline_asic = new TGeoHMatrix("");
            (*incline_asic) = (*trd_asic_trans1) * (*incline_feb);
            trd_feb_box->AddNode(trdmod1_asic, 2*iAsic+2, incline_asic);  // now we have ASICs on the inclined FEB
          }
 
        }
        // now we have an inclined FEB with ASICs
      }


      // now go on with FEB placement
      Int_t nofFebs = FebsPerModule[ moduleType - 1 ];
      for (Int_t iFeb = 0; iFeb < nofFebs; iFeb++) {
        feb_pos   = (iFeb + 0.5) / nofFebs - 0.5;   // equal spacing of FEBs on the backpanel
        feb_pos_y = feb_pos * activeAreaY;

        // shift inclined FEB in y to its final position
        trd_feb_y_position = new TGeoTranslation("", 0., feb_pos_y, feb_z_offset);  // with additional fixed offset in z direction
	//        trd_feb_y_position = new TGeoTranslation("", 0., feb_pos_y, 0.0);  // touching the backpanel with the corner
        trd_feb_vol->AddNode(trd_feb_box, iFeb+1, trd_feb_y_position);  // position FEB in y

      }
      TGeoTranslation* trd_febvol_trans = new TGeoTranslation("", 0., 0., febvol_position);
      gGeoMan->GetVolume(name)->AddNode(trd_feb_vol, 1, trd_febvol_trans);  // put febvol at correct z position wrt to the module
   }

   return module;
}

Int_t copy_nr(Int_t stationNr, Int_t layerNr, Int_t copyNr)
{
   return stationNr * 10000 + layerNr * 1000 + copyNr;
}

Int_t copy_nr_modid(Int_t stationNr, Int_t layerNr, Int_t copyNr, Int_t planeNr, Int_t modinplaneNr)
{
  return (stationNr * 1000 + layerNr * 100 + copyNr) * 10000 + planeNr * 100 + modinplaneNr;
}

void create_detector_layers(Int_t layerId)
{
  Int_t module_id = 0;
  Int_t layerNrInStation = LayerNrInStation[layerId];
  Int_t layerType = LayerType[layerId] / 10;  // this is also a station number
  Int_t isRotated = LayerType[layerId] % 10;  // is 1 for layers 2,4, ...
  TGeoRotation* module_rotation = new TGeoRotation();

  Int_t stationNr = layerType;

// rotation is now done in the for loop for each module individually
//  if ( isRotated == 1 ) {
//    module_rotation = new TGeoRotation();
//    module_rotation->RotateZ(90.);
//  } else {
//    module_rotation = new TGeoRotation();
//    module_rotation->RotateZ( 0.);
//  }
  
  Int_t innerarray_size1 = LayerArraySize[layerType - 1][0];
  Int_t innerarray_size2 = LayerArraySize[layerType - 1][1];
  Int_t* innerLayer;
  
  Int_t outerarray_size1 = LayerArraySize[layerType - 1][2];
  Int_t outerarray_size2 = LayerArraySize[layerType - 1][3];
  Int_t* outerLayer;
  
  if ( 1 == layerType ) {
    innerLayer = layer1i;      
    outerLayer = layer1o; 
  } else if ( 2 == layerType ) {
    innerLayer = layer2i;      
    outerLayer = layer2o; 
  } else if ( 3 == layerType ) {
    innerLayer = layer3i;      
    outerLayer = layer3o; 
  } else {
    std::cout << "Type of layer not known" << std::endl;
  } 

// add layer keeping volume
  TString layername = Form("layer%02d", PlaneId[layerId]);
  TGeoVolume* layer = new TGeoVolumeAssembly(layername);
//  gGeoMan->GetVolume(geoVersion)->AddNode(layer, 1);
  Int_t i = 100 + PlaneId[layerId];
  gGeoMan->GetVolume(geoVersion)->AddNode(layer, i);
//  cout << layername << endl;

  Double_t ExplodeScale = 1.00;
  if(DoExplode)  // if explosion, set scale
    ExplodeScale = ExplodeFactor;

  Int_t modId = 0;  // module id, only within this layer
  
  Int_t copyNrIn[4] = { 0, 0, 0, 0 }; // copy number for each module type
  for ( Int_t type = 1; type <= 4; type++) {
    for ( Int_t j = (innerarray_size1-1); j >= 0; j--)  { // start from the bottom 
      for ( Int_t i = 0; i < innerarray_size2; i++) {
        module_id = *(innerLayer + (j * innerarray_size2 + i));
        if ( module_id /100 == type) {
          Int_t y = -(j-2);
          Int_t x =   i-2;

          // displacement
          Double_t dx = 0;
          Double_t dy = 0;
          Double_t dz = 0;

          if(DisplaceRandom)
	  {
            dx = (r3.Rndm()-.5) * 2 * maxdx;  // max +- 0.1 cm shift
            dy = (r3.Rndm()-.5) * 2 * maxdy;  // max +- 0.1 cm shift
            dz = (r3.Rndm()-.5) * 2 * maxdz;  // max +- 1.0 cm shift
          }

          Double_t xPos = DetectorSizeX[0] * x * ExplodeScale + dx;
          Double_t yPos = DetectorSizeY[0] * y * ExplodeScale + dy;
          copyNrIn[type - 1]++;
          modId++;
//          Int_t copy = copy_nr(stationNr, layerNrInStation, copyNrIn[type - 1]);  // orig
          Int_t copy = copy_nr_modid(stationNr, layerNrInStation, copyNrIn[type - 1], PlaneId[layerId], modId);  // with modID

          // take care of FEB orientation away from beam
          module_rotation = new TGeoRotation();   // need to renew rotation to start from 0 degree angle
          if ( isRotated == 0 )  // layer 1,3 ...
	    //   	     module_rotation->RotateZ( (module_id /10 %10) * 90. );  // rotate   0 or 180 degrees, see layer[1-3][i,o]
   	     module_rotation->RotateZ( (module_id /10 %10) * 90. );  // rotate   0 or 180 degrees, see layer[1-3][i,o]
          else  // layer 2,4 ...
	    //   	     module_rotation->RotateZ( (module_id %10) * 90. );      // rotate  90 or 270 degrees, see layer[1-3][i,o]
   	     module_rotation->RotateZ( (module_id %10) * 90. );      // rotate  90 or 270 degrees, see layer[1-3][i,o]

          // rotation
          Double_t drotx = 0;
          Double_t droty = 0;
          Double_t drotz = 0;

          if(RotateRandom)
	  {
            drotx = (r3.Rndm()-.5) * 2 * maxdrotx;
            droty = (r3.Rndm()-.5) * 2 * maxdroty;
            drotz = (r3.Rndm()-.5) * 2 * maxdrotz;

            module_rotation->RotateZ( drotz );
            module_rotation->RotateY( droty );
            module_rotation->RotateX( drotx );
          }

          TGeoCombiTrans* module_placement = new TGeoCombiTrans(xPos, yPos, LayerPosition[layerId] + LayerThickness/2 + dz, module_rotation);  // shift by half layer thickness
//          gGeoMan->GetVolume(geoVersion)->AddNode(gModules[type - 1], copy, module_placement);
// add module to layer
          gGeoMan->GetVolume(layername)->AddNode(gModules[type - 1], copy, module_placement);
//
        }
      }
    }
  }
  
  Int_t copyNrOut[4] = { 0, 0, 0, 0 }; // copy number for each module type
  for ( Int_t type = 5; type <= 8; type++) {
    for ( Int_t j = (outerarray_size1-1); j >= 0; j--)  { // start from the bottom 
      for ( Int_t i = 0; i < outerarray_size2; i++) {
	module_id = *(outerLayer + (j * outerarray_size2 + i));
        if ( module_id  /100 == type) {
          Int_t y = -(j-4);
          Int_t x =   i-5;

          // displacement
          Double_t dx = 0;
          Double_t dy = 0;
          Double_t dz = 0;

          if(DisplaceRandom)
	  {
            dx = (r3.Rndm()-.5) * 2 * maxdx;  // max +- 0.1 cm shift
            dy = (r3.Rndm()-.5) * 2 * maxdy;  // max +- 0.1 cm shift
            dz = (r3.Rndm()-.5) * 2 * maxdz;  // max +- 1.0 cm shift
          }

          Double_t xPos = DetectorSizeX[1] * x * ExplodeScale + dx;
          Double_t yPos = DetectorSizeY[1] * y * ExplodeScale + dy;
          copyNrOut[type - 5]++;
          modId++;
//          Int_t copy = copy_nr(stationNr, layerNrInStation, copyNrOut[type - 5]);  // orig
          Int_t copy = copy_nr_modid(stationNr, layerNrInStation, copyNrOut[type - 5],  PlaneId[layerId], modId);  // with modID

          // take care of FEB orientation - away from beam
          module_rotation = new TGeoRotation();   // need to renew rotation to start from 0 degree angle
          if ( isRotated == 0 )  // layer 1,3 ...
	    //          module_rotation->RotateZ( (module_id /10 %10) * 90. );  // rotate   0 or 180 degrees, see layer[1-3][i,o]
            module_rotation->RotateZ( (module_id /10 %10) * 90. );  // rotate   0 or 180 degrees, see layer[1-3][i,o]
          else  // layer 2,4 ...
	    //          module_rotation->RotateZ( (module_id %10) * 90. );      // rotate  90 or 270 degrees, see layer[1-3][i,o]
            module_rotation->RotateZ( (module_id %10) * 90. );      // rotate  90 or 270 degrees, see layer[1-3][i,o]
    
          // rotation
          Double_t drotx = 0;
          Double_t droty = 0;
          Double_t drotz = 0;

          if(RotateRandom)
	  {
            drotx = (r3.Rndm()-.5) * 2 * maxdrotx;
            droty = (r3.Rndm()-.5) * 2 * maxdroty;
            drotz = (r3.Rndm()-.5) * 2 * maxdrotz;

            module_rotation->RotateZ( drotz );
            module_rotation->RotateY( droty );
            module_rotation->RotateX( drotx );
          }

          TGeoCombiTrans* module_placement = new TGeoCombiTrans(xPos, yPos, LayerPosition[layerId] + LayerThickness/2 + dz, module_rotation);  // shift by half layer thickness
//          gGeoMan->GetVolume(geoVersion)->AddNode(gModules[type - 1], copy, module_placement);
// add module to layer
          gGeoMan->GetVolume(layername)->AddNode(gModules[type - 1], copy, module_placement);
//
        }
      }
    }
  }  
}


void create_supports()
{
  const TString trdSupport = "supportframe";
  TGeoVolume* trdsupport = new TGeoVolumeAssembly(trdSupport);

  TGeoMedium* aluminiumVolMed   = gGeoMan->GetMedium(AluminiumVolumeMedium);  // define Volume Medium

  const Double_t x[12] = { -15,-15, -1, -1,-15,-15, 15, 15,  1,  1, 15, 15 };  // IPB 400
  const Double_t y[12] = { -20,-18,-18, 18, 18, 20, 20, 18, 18,-18,-18,-20 };  // 30 x 40 cm in size, 2 cm wall thickness
  const Double_t Hwid = -2*x[0];  // 30  
  const Double_t Hhei = -2*y[0];  // 40

  Double_t AperX[3] = { 450., 550., 600.};  // inner aperture in X of support structure for stations 1,2,3
  Double_t AperY[3] = { 350., 450., 500.};  // inner aperture in Y of support structure for stations 1,2,3
  Double_t PilPosX;
  Double_t BarPosY;

  const Double_t BeamHeight = 570;  // beamline is at 5.7m above floor

  Double_t PilPosZ[6];  // PilPosZ
  PilPosZ[0] = LayerPosition[0] + LayerThickness/2.;
  PilPosZ[1] = LayerPosition[3] + LayerThickness/2.;
  PilPosZ[2] = LayerPosition[4] + LayerThickness/2.;
  PilPosZ[3] = LayerPosition[7] + LayerThickness/2.;
  PilPosZ[4] = LayerPosition[8] + LayerThickness/2.;
  PilPosZ[5] = LayerPosition[9] + LayerThickness/2.;

//  cout << "PilPosZ[0]: " << PilPosZ[0] << endl;
//  cout << "PilPosZ[1]: " << PilPosZ[1] << endl;

  TGeoRotation  *rotx090 = new TGeoRotation("rotx090"); rotx090->RotateX( 90.); // rotate  90 deg around x-axis                     
  TGeoRotation  *roty090 = new TGeoRotation("roty090"); roty090->RotateY( 90.); // rotate  90 deg around y-axis                     
  TGeoRotation  *rotz090 = new TGeoRotation("rotz090"); rotz090->RotateZ( 90.); // rotate  90 deg around y-axis                     

  TGeoRotation  *rotzx01 = new TGeoRotation("rotzx01"); 
  rotzx01->RotateZ(  90.); // rotate  90 deg around z-axis
  rotzx01->RotateX(  90.); // rotate  90 deg around x-axis

//  TGeoRotation  *rotxz01 = new TGeoRotation("rotxz01"); 
//  rotxz01->RotateX(  90.); // rotate  90 deg around x-axis
//  rotxz01->RotateZ(  90.); // rotate  90 deg around z-axis

  Double_t ang1 = atan(3./4.) * 180. / acos(-1.);
  //  cout << "DEDE " << ang1 << endl;
  //  Double_t sin1 = acos(-1.);
  //  cout << "DEDE " << sin1 << endl;
  TGeoRotation  *rotx080 = new TGeoRotation("rotx080"); rotx080->RotateX( 90.-ang1); // rotate  80 deg around x-axis                     
  TGeoRotation  *rotx100 = new TGeoRotation("rotx100"); rotx100->RotateX( 90.+ang1); // rotate 100 deg around x-axis                     

  TGeoRotation  *rotxy01 = new TGeoRotation("rotxy01"); 
  rotxy01->RotateX(  90.); // rotate  90 deg around x-axis                     
  rotxy01->RotateZ(-ang1); // rotate  ang1   around rotated y-axis                     

  TGeoRotation  *rotxy02 = new TGeoRotation("rotxy02"); 
  rotxy02->RotateX(  90.); // rotate  90 deg around x-axis                     
  rotxy02->RotateZ( ang1); // rotate  ang1   around rotated y-axis                     


//-------------------
// vertical pillars (Y)
//-------------------

  // station 1
  if (ShowLayer[0])  // if geometry contains layer 1 (1st layer of station 1)
    {
      TGeoXtru* trd1_H_vert1 = new TGeoXtru(2);  // define Xtrusion of 2 planes
      trd1_H_vert1->DefinePolygon(12,x,y);
      trd1_H_vert1->DefineSection( 0,-(AperY[0]+Hhei), 0, 0, 1.0);
      trd1_H_vert1->DefineSection( 1, BeamHeight,      0, 0, 1.0);  
      TGeoVolume* trd1_H_vert_vol1 = new TGeoVolume("trd1_H_y_01", trd1_H_vert1, aluminiumVolMed);
      trd1_H_vert_vol1->SetLineColor(kYellow);
      PilPosX = AperX[0];
    
      TGeoCombiTrans* trd1_H_vert_combi01 = new TGeoCombiTrans( (PilPosX+Hhei/2.), 0., PilPosZ[0], rotzx01);
      trdsupport->AddNode(trd1_H_vert_vol1, 11, trd1_H_vert_combi01);
      TGeoCombiTrans* trd1_H_vert_combi02 = new TGeoCombiTrans(-(PilPosX+Hhei/2.), 0., PilPosZ[0], rotzx01);
      trdsupport->AddNode(trd1_H_vert_vol1, 12, trd1_H_vert_combi02);
      TGeoCombiTrans* trd1_H_vert_combi03 = new TGeoCombiTrans( (PilPosX+Hhei/2.), 0., PilPosZ[1], rotzx01);
      trdsupport->AddNode(trd1_H_vert_vol1, 13, trd1_H_vert_combi03);
      TGeoCombiTrans* trd1_H_vert_combi04 = new TGeoCombiTrans(-(PilPosX+Hhei/2.), 0., PilPosZ[1], rotzx01);
      trdsupport->AddNode(trd1_H_vert_vol1, 14, trd1_H_vert_combi04);
    }

  // station 2
  if (ShowLayer[4])  // if geometry contains layer 5 (1st layer of station 2)
    {
      TGeoXtru* trd1_H_vert1 = new TGeoXtru(2);  // define Xtrusion of 2 planes
      trd1_H_vert1->DefinePolygon(12,x,y);
      trd1_H_vert1->DefineSection( 0,-(AperY[1]+Hhei), 0, 0, 1.0);
      trd1_H_vert1->DefineSection( 1, BeamHeight,      0, 0, 1.0);
      TGeoVolume* trd1_H_vert_vol1 = new TGeoVolume("trd1_H_y_02", trd1_H_vert1, aluminiumVolMed);
      trd1_H_vert_vol1->SetLineColor(kYellow);
      PilPosX = AperX[1];
    
      TGeoCombiTrans* trd1_H_vert_combi01 = new TGeoCombiTrans( (PilPosX+Hhei/2.), 0., PilPosZ[2], rotzx01);
      trdsupport->AddNode(trd1_H_vert_vol1, 21, trd1_H_vert_combi01);
      TGeoCombiTrans* trd1_H_vert_combi02 = new TGeoCombiTrans(-(PilPosX+Hhei/2.), 0., PilPosZ[2], rotzx01);
      trdsupport->AddNode(trd1_H_vert_vol1, 22, trd1_H_vert_combi02);
      TGeoCombiTrans* trd1_H_vert_combi03 = new TGeoCombiTrans( (PilPosX+Hhei/2.), 0., PilPosZ[3], rotzx01);
      trdsupport->AddNode(trd1_H_vert_vol1, 23, trd1_H_vert_combi03);
      TGeoCombiTrans* trd1_H_vert_combi04 = new TGeoCombiTrans(-(PilPosX+Hhei/2.), 0., PilPosZ[3], rotzx01);
      trdsupport->AddNode(trd1_H_vert_vol1, 24, trd1_H_vert_combi04);
    }

  // station 3
  if (ShowLayer[8])  // if geometry contains layer 9 (1st layer of station 3)
    {
      TGeoXtru* trd1_H_vert1 = new TGeoXtru(2);  // define Xtrusion of 2 planes
      trd1_H_vert1->DefinePolygon(12,x,y);
      trd1_H_vert1->DefineSection( 0,-(AperY[2]+Hhei), 0, 0, 1.0);
      trd1_H_vert1->DefineSection( 1, BeamHeight,      0, 0, 1.0);
      TGeoVolume* trd1_H_vert_vol1 = new TGeoVolume("trd1_H_y_03", trd1_H_vert1, aluminiumVolMed);
      trd1_H_vert_vol1->SetLineColor(kYellow);
      PilPosX = AperX[2];
      
      TGeoCombiTrans* trd1_H_vert_combi01 = new TGeoCombiTrans( (PilPosX+Hhei/2.), 0., PilPosZ[4], rotzx01);
      trdsupport->AddNode(trd1_H_vert_vol1, 31, trd1_H_vert_combi01);
      TGeoCombiTrans* trd1_H_vert_combi02 = new TGeoCombiTrans(-(PilPosX+Hhei/2.), 0., PilPosZ[4], rotzx01);
      trdsupport->AddNode(trd1_H_vert_vol1, 32, trd1_H_vert_combi02);
      TGeoCombiTrans* trd1_H_vert_combi03 = new TGeoCombiTrans( (PilPosX+Hhei/2.), 0., PilPosZ[5], rotzx01);
      trdsupport->AddNode(trd1_H_vert_vol1, 33, trd1_H_vert_combi03);
      TGeoCombiTrans* trd1_H_vert_combi04 = new TGeoCombiTrans(-(PilPosX+Hhei/2.), 0., PilPosZ[5], rotzx01);
      trdsupport->AddNode(trd1_H_vert_vol1, 34, trd1_H_vert_combi04);
    }


//-------------------
// horizontal supports (X)
//-------------------

  // station 1
  if (ShowLayer[0])  // if geometry contains layer 1 (1st layer of station 1)
    {
      TGeoXtru* trd1_H_hori1 = new TGeoXtru(2);  // define Xtrusion of 2 planes
      trd1_H_hori1->DefinePolygon(12,x,y);
      trd1_H_hori1->DefineSection( 0,-AperX[0], 0, 0, 1.0);
      trd1_H_hori1->DefineSection( 1, AperX[0], 0, 0, 1.0);
      TGeoVolume* trd1_H_hori_vol1 = new TGeoVolume("trd1_H_x_01", trd1_H_hori1, aluminiumVolMed);
      trd1_H_hori_vol1->SetLineColor(kRed);
      BarPosY = AperY[0];
    
      TGeoCombiTrans* trd1_H_hori_combi01 = new TGeoCombiTrans(0., (BarPosY+Hhei/2.), PilPosZ[0], roty090);
      trdsupport->AddNode(trd1_H_hori_vol1, 11, trd1_H_hori_combi01);
      TGeoCombiTrans* trd1_H_hori_combi02 = new TGeoCombiTrans(0.,-(BarPosY+Hhei/2.), PilPosZ[0], roty090);
      trdsupport->AddNode(trd1_H_hori_vol1, 12, trd1_H_hori_combi02);
      TGeoCombiTrans* trd1_H_hori_combi03 = new TGeoCombiTrans(0., (BarPosY+Hhei/2.), PilPosZ[1], roty090);
      trdsupport->AddNode(trd1_H_hori_vol1, 13, trd1_H_hori_combi03);
      TGeoCombiTrans* trd1_H_hori_combi04 = new TGeoCombiTrans(0.,-(BarPosY+Hhei/2.), PilPosZ[1], roty090);
      trdsupport->AddNode(trd1_H_hori_vol1, 14, trd1_H_hori_combi04);
    }

  // station 2
  if (ShowLayer[4])  // if geometry contains layer 5 (1st layer of station 2)
    {
      TGeoXtru* trd1_H_hori1 = new TGeoXtru(2);  // define Xtrusion of 2 planes
      trd1_H_hori1->DefinePolygon(12,x,y);
      trd1_H_hori1->DefineSection( 0,-AperX[1], 0, 0, 1.0);
      trd1_H_hori1->DefineSection( 1, AperX[1], 0, 0, 1.0);
      TGeoVolume* trd1_H_hori_vol1 = new TGeoVolume("trd1_H_x_02", trd1_H_hori1, aluminiumVolMed);
      trd1_H_hori_vol1->SetLineColor(kRed);
      BarPosY = AperY[1];
    
      TGeoCombiTrans* trd1_H_hori_combi01 = new TGeoCombiTrans(0., (BarPosY+Hhei/2.), PilPosZ[2], roty090);
      trdsupport->AddNode(trd1_H_hori_vol1, 21, trd1_H_hori_combi01);
      TGeoCombiTrans* trd1_H_hori_combi02 = new TGeoCombiTrans(0.,-(BarPosY+Hhei/2.), PilPosZ[2], roty090);
      trdsupport->AddNode(trd1_H_hori_vol1, 22, trd1_H_hori_combi02);
      TGeoCombiTrans* trd1_H_hori_combi03 = new TGeoCombiTrans(0., (BarPosY+Hhei/2.), PilPosZ[3], roty090);
      trdsupport->AddNode(trd1_H_hori_vol1, 23, trd1_H_hori_combi03);
      TGeoCombiTrans* trd1_H_hori_combi04 = new TGeoCombiTrans(0.,-(BarPosY+Hhei/2.), PilPosZ[3], roty090);
      trdsupport->AddNode(trd1_H_hori_vol1, 24, trd1_H_hori_combi04);
    }

  // station 3
  if (ShowLayer[8])  // if geometry contains layer 9 (1st layer of station 3)
    {
      TGeoXtru* trd1_H_hori1 = new TGeoXtru(2);  // define Xtrusion of 2 planes
      trd1_H_hori1->DefinePolygon(12,x,y);
      trd1_H_hori1->DefineSection( 0,-AperX[2], 0, 0, 1.0);
      trd1_H_hori1->DefineSection( 1, AperX[2], 0, 0, 1.0);
      TGeoVolume* trd1_H_hori_vol1 = new TGeoVolume("trd1_H_x_03", trd1_H_hori1, aluminiumVolMed);
      trd1_H_hori_vol1->SetLineColor(kRed);
      BarPosY = AperY[2];
    
      TGeoCombiTrans* trd1_H_hori_combi01 = new TGeoCombiTrans(0., (BarPosY+Hhei/2.), PilPosZ[4], roty090);
      trdsupport->AddNode(trd1_H_hori_vol1, 31, trd1_H_hori_combi01);
      TGeoCombiTrans* trd1_H_hori_combi02 = new TGeoCombiTrans(0.,-(BarPosY+Hhei/2.), PilPosZ[4], roty090);
      trdsupport->AddNode(trd1_H_hori_vol1, 32, trd1_H_hori_combi02);
      TGeoCombiTrans* trd1_H_hori_combi03 = new TGeoCombiTrans(0., (BarPosY+Hhei/2.), PilPosZ[5], roty090);
      trdsupport->AddNode(trd1_H_hori_vol1, 33, trd1_H_hori_combi03);
      TGeoCombiTrans* trd1_H_hori_combi04 = new TGeoCombiTrans(0.,-(BarPosY+Hhei/2.), PilPosZ[5], roty090);
      trdsupport->AddNode(trd1_H_hori_vol1, 34, trd1_H_hori_combi04);
    }


//-------------------
// horizontal supports (Z)
//-------------------

  // station 1
  if (ShowLayer[0])  // if geometry contains layer 1 (1st layer of station 1)
    {
      TGeoXtru* trd1_H_slope1 = new TGeoXtru(2);  // define Xtrusion of 2 planes
      trd1_H_slope1->DefinePolygon(12,x,y);
      trd1_H_slope1->DefineSection( 0,-(PilPosZ[1]-PilPosZ[0]-Hwid)/2., 0, 0, 1.0);
      trd1_H_slope1->DefineSection( 1,+(PilPosZ[1]-PilPosZ[0]-Hwid)/2., 0, 0, 1.0);
      TGeoVolume* trd1_H_slope_vol1 = new TGeoVolume("trd1_H_z_01", trd1_H_slope1, aluminiumVolMed);
      trd1_H_slope_vol1->SetLineColor(kGreen);
      PilPosX = AperX[0];
      BarPosY = AperY[0];
        
      TGeoCombiTrans* trd1_H_slope_combi01 = new TGeoCombiTrans( (PilPosX+Hhei/2.), (BarPosY+Hhei-Hwid/2.), (PilPosZ[0]+PilPosZ[1])/2., rotz090);
      trdsupport->AddNode(trd1_H_slope_vol1, 11, trd1_H_slope_combi01);
      TGeoCombiTrans* trd1_H_slope_combi02 = new TGeoCombiTrans(-(PilPosX+Hhei/2.), (BarPosY+Hhei-Hwid/2.), (PilPosZ[0]+PilPosZ[1])/2., rotz090);
      trdsupport->AddNode(trd1_H_slope_vol1, 12, trd1_H_slope_combi02);
      TGeoCombiTrans* trd1_H_slope_combi03 = new TGeoCombiTrans( (PilPosX+Hhei/2.),-(BarPosY+Hhei-Hwid/2.), (PilPosZ[0]+PilPosZ[1])/2., rotz090);
      trdsupport->AddNode(trd1_H_slope_vol1, 13, trd1_H_slope_combi03);
      TGeoCombiTrans* trd1_H_slope_combi04 = new TGeoCombiTrans(-(PilPosX+Hhei/2.),-(BarPosY+Hhei-Hwid/2.), (PilPosZ[0]+PilPosZ[1])/2., rotz090);
      trdsupport->AddNode(trd1_H_slope_vol1, 14, trd1_H_slope_combi04);
    }

  // station 2
  if (ShowLayer[4])  // if geometry contains layer 5 (1st layer of station 2)
    {
      TGeoXtru* trd1_H_slope1 = new TGeoXtru(2);  // define Xtrusion of 2 planes
      trd1_H_slope1->DefinePolygon(12,x,y);
      trd1_H_slope1->DefineSection( 0,-(PilPosZ[3]-PilPosZ[2]-Hwid)/2., 0, 0, 1.0);
      trd1_H_slope1->DefineSection( 1,+(PilPosZ[3]-PilPosZ[2]-Hwid)/2., 0, 0, 1.0);
      TGeoVolume* trd1_H_slope_vol1 = new TGeoVolume("trd1_H_z_02", trd1_H_slope1, aluminiumVolMed);
      trd1_H_slope_vol1->SetLineColor(kGreen);
      PilPosX = AperX[1];
      BarPosY = AperY[1];
        
      TGeoCombiTrans* trd1_H_slope_combi01 = new TGeoCombiTrans( (PilPosX+Hhei/2.), (BarPosY+Hhei-Hwid/2.), (PilPosZ[2]+PilPosZ[3])/2., rotz090);
      trdsupport->AddNode(trd1_H_slope_vol1, 21, trd1_H_slope_combi01);
      TGeoCombiTrans* trd1_H_slope_combi02 = new TGeoCombiTrans(-(PilPosX+Hhei/2.), (BarPosY+Hhei-Hwid/2.), (PilPosZ[2]+PilPosZ[3])/2., rotz090);
      trdsupport->AddNode(trd1_H_slope_vol1, 22, trd1_H_slope_combi02);
      TGeoCombiTrans* trd1_H_slope_combi03 = new TGeoCombiTrans( (PilPosX+Hhei/2.),-(BarPosY+Hhei-Hwid/2.), (PilPosZ[2]+PilPosZ[3])/2., rotz090);
      trdsupport->AddNode(trd1_H_slope_vol1, 23, trd1_H_slope_combi03);
      TGeoCombiTrans* trd1_H_slope_combi04 = new TGeoCombiTrans(-(PilPosX+Hhei/2.),-(BarPosY+Hhei-Hwid/2.), (PilPosZ[2]+PilPosZ[3])/2., rotz090);
      trdsupport->AddNode(trd1_H_slope_vol1, 24, trd1_H_slope_combi04);
    }

  // station 3
  if (ShowLayer[8])  // if geometry contains layer 9 (1st layer of station 3)
    {
      TGeoXtru* trd1_H_slope1 = new TGeoXtru(2);  // define Xtrusion of 2 planes
      trd1_H_slope1->DefinePolygon(12,x,y);
      trd1_H_slope1->DefineSection( 0,-(PilPosZ[5]-PilPosZ[4]-Hwid)/2., 0, 0, 1.0);
      trd1_H_slope1->DefineSection( 1,+(PilPosZ[5]-PilPosZ[4]-Hwid)/2., 0, 0, 1.0);
      TGeoVolume* trd1_H_slope_vol1 = new TGeoVolume("trd1_H_z_03", trd1_H_slope1, aluminiumVolMed);
      trd1_H_slope_vol1->SetLineColor(kGreen);
      PilPosX = AperX[2];
      BarPosY = AperY[2];
        
      TGeoCombiTrans* trd1_H_slope_combi01 = new TGeoCombiTrans( (PilPosX+Hhei/2.), (BarPosY+Hhei-Hwid/2.), (PilPosZ[4]+PilPosZ[5])/2., rotz090);
      trdsupport->AddNode(trd1_H_slope_vol1, 31, trd1_H_slope_combi01);
      TGeoCombiTrans* trd1_H_slope_combi02 = new TGeoCombiTrans(-(PilPosX+Hhei/2.), (BarPosY+Hhei-Hwid/2.), (PilPosZ[4]+PilPosZ[5])/2., rotz090);
      trdsupport->AddNode(trd1_H_slope_vol1, 32, trd1_H_slope_combi02);
      TGeoCombiTrans* trd1_H_slope_combi03 = new TGeoCombiTrans( (PilPosX+Hhei/2.),-(BarPosY+Hhei-Hwid/2.), (PilPosZ[4]+PilPosZ[5])/2., rotz090);
      trdsupport->AddNode(trd1_H_slope_vol1, 33, trd1_H_slope_combi03);
      TGeoCombiTrans* trd1_H_slope_combi04 = new TGeoCombiTrans(-(PilPosX+Hhei/2.),-(BarPosY+Hhei-Hwid/2.), (PilPosZ[4]+PilPosZ[5])/2., rotz090);
      trdsupport->AddNode(trd1_H_slope_vol1, 34, trd1_H_slope_combi04);
    }

  gGeoMan->GetVolume(geoVersion)->AddNode(trdsupport,1);

}
