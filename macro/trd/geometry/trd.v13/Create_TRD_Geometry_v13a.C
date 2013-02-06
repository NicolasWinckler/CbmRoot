// 2013-01-21 - DE - introduce TRD media, use TRDG10 as material for pad plane and FEBs
// 2013-01-21 - DE - put backpanel into the geometry
// 2013-01-11 - DE - allow for misalignment of TRD modules
// 2012-11-04 - DE - add kapton foil, add FR4 padplane
// 2012-11-03 - DE - add lattice grid on entrance window as CompositeShape

// in root all sizes are diven in cm

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
#include "TRandom3.h"

#include <iostream>

// Name of output file with geometry
const TString geoVersion = "trd_v13x";
const TString FileNameSim = geoVersion + ".root";
const TString FileNameGeo = geoVersion + "_geo.root";
//const TString geoVersion = "trd1";
//const TString FileName = "trd_v13x.root";

// display switches
const Bool_t IncludeRadiator = true;  // false;  // true, if radiator is included in geometry
const Bool_t IncludeLattice  = true; // false;  // true, if lattice grid is included in geometry
const Bool_t IncludeGasHoles = false; // false;  // true, if gas holes to be pllotted in the lattice grid
const Bool_t IncludeFebs     = true;  // false;  // true, if FEBs are included in geometry

const Float_t feb_rotation_angle = 45; //0.1; // 65.; // 70.; // 0.;   // rotation around x-axis, should be < 90 degrees  

// positioning switches
const Bool_t DisplaceRandom  = false;  // true; // false;  // add random displacement of modules for alignment study
const Bool_t RotateRandom    = false;  // true; // false;  // add random rotation of modules for alignment study
const Bool_t DoExplode       = false;  // true, // false;  // add random displacement of modules for alignment study

// positioning parameters
const Float_t maxdx    = 0.2;   // max +- 0.1 cm shift in x
const Float_t maxdy    = 0.2;   // max +- 0.1 cm shift in y
const Float_t maxdz    = 1.0;   // max +- 1.0 cm shift in z

const Float_t maxdrotx = 2.0;   // 20.0; // max rotation around x
const Float_t maxdroty = 2.0;   // 20.0; // max rotation around y
const Float_t maxdrotz = 2.0;   // 20.0; // max rotation around z

Float_t ExplodeFactor = 1.02;   // 1.02; // Factor by which modules are exploded in the x/y plane

// initialise random numbers
TRandom3 r3(0);

// Parameters defining the layout of the complete detector build out of different detector layers.
const Int_t   NofLayers = 10;   // max layers

// select layers to display
//const Int_t   ShowLayer[NofLayers] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 };  // 1st layer only
//const Int_t   ShowLayer[NofLayers] = { 0, 0, 0, 0, 1, 1, 0, 0, 0, 0 };  // Station 2, layer 5, 6
//const Int_t   ShowLayer[NofLayers] = { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1 };  // Station 3, layer 9,10
//const Int_t   ShowLayer[NofLayers] = { 1, 1, 0, 0, 1, 1, 1, 0, 1, 1 };  // Station 3, layer 9,10
//const Int_t   ShowLayer[NofLayers] = { 1, 1, 1, 1, 0, 0, 0, 0, 0, 0 };  // SIS100-4l  // 1: plot, 0: hide
//const Int_t   ShowLayer[NofLayers] = { 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 };  // SIS100-2l  // 1: plot, 0: hide
const Int_t   ShowLayer[NofLayers] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };  // SIS300     // 1: plot, 0: hide

const Int_t   LayerType[NofLayers]        = { 10, 11, 10, 11, 20, 21, 20, 21, 30, 31 };  // ab: a [1-3] - layer type, b [0,1] - vertical/hoziontal pads  
// 3 station spacing
const Float_t LayerPosition[NofLayers]    = { 450., 500., 550., 600., 675., 725., 775., 825., 900., 950. };  // z position in cm of Layer front
// equal spacing
//const Float_t LayerPosition[NofLayers]    = { 500., 550., 600., 650., 700., 750., 800., 850., 900., 950. };  // z position in cm of Layer front
const Float_t LayerNrInStation[NofLayers] = { 1, 2, 3, 4, 1, 2, 3, 4, 1, 2 };
const Float_t LayerThickness = 49.5; // Thickness of one TRD layer in cm

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
const Int_t FebsPerModule[NofModuleTypes] = { 19, 10,  5,  5, 12,  6,  4,  3 }; // number of FEBs on backside (linked to pad layout) - mod4 = mod3, therefore same number of febs
//const Int_t FebsPerModule[NofModuleTypes] = { 10,  5,  5,  5, 12,  6,  4,  3 }; // number of FEBs on backside - reduced FEBs (64 ch ASICs)
const Float_t feb_z_offset = 0.1;  // 1 mm - offset in z of FEBs to backpanel

const Float_t FrameWidth[2]    = { 1.5, 2.0 };   // Width of detector frames in cm
const Float_t DetectorSizeX[2] = { 60., 100.};   // => 57 x 57 cm2 & 96 x 96 cm2 active area
const Float_t DetectorSizeY[2] = { 60., 100.};   // quadratic modules

// Parameters tor the lattice grid reinforcing the entrance window
const Float_t lattice_o_width[2] = { 1.5, 2.0 };   // Width of outer lattice frame in cm
const Float_t lattice_i_width[2] = { 0.4, 0.4 };   // Width of inner lattice frame in cm
// Thickness (in z) of lattice frames in cm - see below

// z - geometry of TRD modules
const Float_t radiator_thickness     =  35.0;    // 35 cm thickness of radiator
const Float_t radiator_position      =  - LayerThickness/2. + radiator_thickness/2.;

const Float_t lattice_thickness      =   1.0;    // 0.9975;  // 1.0;  // 10 mm thick lattice frames
const Float_t lattice_position       =  radiator_position + radiator_thickness/2. + lattice_thickness/2.;

const Float_t kapton_thickness       =   0.0025; //  25 micron thickness of kapton
const Float_t kapton_position        =  lattice_position + lattice_thickness/2. + kapton_thickness/2.;

const Float_t gas_thickness          =   1.2;    //  12 mm thickness of gas
const Float_t gas_position           =  kapton_position + kapton_thickness/2. + gas_thickness/2.;

const Float_t padcopper_thickness    =   0.0025; //  25 micron thickness of copper pads
const Float_t padcopper_position     =  gas_position + gas_thickness/2. + padcopper_thickness/2.;

const Float_t padplane_thickness     =   0.0360; // 360 micron thickness of padplane
const Float_t padplane_position      =  padcopper_position + padcopper_thickness/2. + padplane_thickness/2.;

// backpanel components
const Float_t carbon_thickness       =   0.0190 * 2; // use 2 layers!!   // 190 micron thickness for 1 layer of carbon fibres
const Float_t honeycomb_thickness    =   2.3 - kapton_thickness - padcopper_thickness - padplane_thickness - carbon_thickness;    //  ~ 2.3 mm thickness of honeycomb
const Float_t honeycomb_position     =  padplane_position + padplane_thickness/2. + honeycomb_thickness/2.;
const Float_t carbon_position        =  honeycomb_position + honeycomb_thickness/2. + carbon_thickness/2.;

// readout boards
const  Float_t febbox_thickness      =  10.0;    // 10 cm length of FEBs
const  Float_t febbox_position       =  carbon_position + carbon_thickness/2. + febbox_thickness/2.;
const  Float_t feb_thickness         =   0.5; //2.0;   //  5 mm thickness of FEBs

const Float_t frame_thickness        =  gas_thickness;   // frame covers gas volume: from kapton foil to pad plane
const Float_t frame_position         =  - LayerThickness /2. + radiator_thickness + lattice_thickness + kapton_thickness + frame_thickness/2.;


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
const TString FrameVolumeMedium       = "TRDG10";
//const TString MylarVolumeMedium       = "mylar";
//const TString RadiatorVolumeMedium    = "polypropylene";
//const TString ElectronicsVolumeMedium = "goldcoatedcopper";


// some global variables
TGeoManager* gGeoMan = NULL;  // Pointer to TGeoManager instance
TGeoVolume* gModules[NofModuleTypes]; // Global storage for module types

// Forward declarations
void create_materials_from_media_file();
void create_trd_module(Int_t moduleType);
void create_detector_layers(Int_t layer);

void Create_TRD_Geometry_v13a() {
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

  for (Int_t iLayer = 0; iLayer < NofLayers; iLayer++) {
    //    if ((iLayer != 0) && (iLayer != 3))  continue;  // first layer only - comment later on
    //    if (iLayer != 0) continue;  // first layer only - comment later on
    if (ShowLayer[iLayer])
      create_detector_layers(iLayer);
  }
  
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

//  geoBuild->createMedium(goldCoatedCopper);
//  geoBuild->createMedium(polypropylene);
//  geoBuild->createMedium(mylar);
}

TGeoVolume* create_trd_module(Int_t moduleType)
{
  Int_t type = ModuleType[moduleType - 1];
  Float_t sizeX = DetectorSizeX[type];
  Float_t sizeY = DetectorSizeY[type];
  Float_t frameWidth = FrameWidth[type];
  Float_t activeAreaX = sizeX - 2 * frameWidth;
  Float_t activeAreaY = sizeY - 2 * frameWidth;

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

  TString name = Form("trd1mod%d", moduleType);
  TGeoVolume* module = new TGeoVolumeAssembly(name);


  if(IncludeRadiator)
   {
     // Radiator
     //   TGeoBBox* trd_radiator = new TGeoBBox("", activeAreaX /2., activeAreaY /2., radiator_thickness /2.);
     TGeoBBox* trd_radiator = new TGeoBBox("", sizeX /2., sizeY /2., radiator_thickness /2.);
     TGeoVolume* trdmod1_radvol = new TGeoVolume(Form("trd1mod%dradiator", moduleType), trd_radiator, radVolMed);
     trdmod1_radvol->SetLineColor(kBlue);
     trdmod1_radvol->SetTransparency(70);  // (60);  // (70);  // set transparency for the TRD
     TGeoTranslation* trd_radiator_trans = new TGeoTranslation("", 0., 0., radiator_position);
     module->AddNode(trdmod1_radvol, 0, trd_radiator_trans);
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
    //   Float_t Lattice_1_width   = 0.1; // Width of inner lattice frame in cm
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
       TGeoVolume *trdmod0_lattice = new TGeoVolume(Form("trd1mod%dlatticegrid", moduleType), lattice_grid, latticeVolMed);
       trdmod0_lattice->SetLineColor(kYellow);
       TGeoTranslation *trd_lattice_trans = new TGeoTranslation("", 0., 0., lattice_position);
       module->AddNode(trdmod0_lattice, 0, trd_lattice_trans);
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
    //   Float_t Lattice_1_width   = 0.1; // Width of inner lattice frame in cm
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
       TGeoVolume *trdmod1_lattice = new TGeoVolume(Form("trd1mod%dlatticegrid", moduleType), lattice_grid, latticeVolMed);
       trdmod1_lattice->SetLineColor(kYellow);
       TGeoTranslation *trd_lattice_trans = new TGeoTranslation("", 0., 0., lattice_position);
       module->AddNode(trdmod1_lattice, 0, trd_lattice_trans);
     }

   }  // with lattice grid 

   // Kapton Foil
   TGeoBBox* trd_kapton = new TGeoBBox("", sizeX /2., sizeY /2., kapton_thickness /2.);
   TGeoVolume* trdmod1_kaptonvol = new TGeoVolume(Form("trd1mod%dkapton", moduleType), trd_kapton, kaptonVolMed);
   trdmod1_kaptonvol->SetLineColor(kGreen);
   TGeoTranslation* trd_kapton_trans = new TGeoTranslation("", 0., 0., kapton_position);
   module->AddNode(trdmod1_kaptonvol, 0, trd_kapton_trans);

   // start of Frame in z
   // Gas
   TGeoBBox* trd_gas = new TGeoBBox("", activeAreaX /2., activeAreaY /2., gas_thickness /2.);
   TGeoVolume* trdmod1_gasvol = new TGeoVolume(Form("trd1mod%dgas", moduleType), trd_gas, gasVolMed);
   //   trdmod1_gasvol->SetLineColor(kBlue);
   trdmod1_gasvol->SetLineColor(kGreen); // to avoid blue overlaps in the screenshots
   TGeoTranslation* trd_gas_trans = new TGeoTranslation("", 0., 0., gas_position);
   module->AddNode(trdmod1_gasvol, 0, trd_gas_trans);
   // end of Frame in z


   // frame1
   TGeoBBox* trd_frame1 = new TGeoBBox("", sizeX /2., frameWidth /2., frame_thickness/2.);
   TGeoVolume* trdmod1_frame1vol = new TGeoVolume(Form("trd1mod%dframe1", moduleType), trd_frame1, frameVolMed);
   trdmod1_frame1vol->SetLineColor(kRed);

   // translations 
   TGeoTranslation* trd_frame1_trans = new TGeoTranslation("", 0., activeAreaY /2. + frameWidth /2., frame_position);
   module->AddNode(trdmod1_frame1vol, 1, trd_frame1_trans);
   trd_frame1_trans = new TGeoTranslation("", 0., -(activeAreaY /2. + frameWidth /2.), frame_position);
   module->AddNode(trdmod1_frame1vol, 2, trd_frame1_trans);


   // frame2
   TGeoBBox* trd_frame2 = new TGeoBBox("", frameWidth /2., activeAreaY /2., frame_thickness /2.);
   TGeoVolume* trdmod1_frame2vol = new TGeoVolume(Form("trd1mod%dframe2", moduleType), trd_frame2, frameVolMed);
   trdmod1_frame2vol->SetLineColor(kRed);

   // translations 
   TGeoTranslation* trd_frame2_trans = new TGeoTranslation("", activeAreaX /2. + frameWidth /2., 0., frame_position);
   module->AddNode(trdmod1_frame2vol, 1, trd_frame2_trans);
   trd_frame2_trans = new TGeoTranslation("", -(activeAreaX /2. + frameWidth /2.), 0., frame_position);
   module->AddNode(trdmod1_frame2vol, 2, trd_frame2_trans);


   // Pad Copper
   TGeoBBox *trd_padcopper = new TGeoBBox("", sizeX /2., sizeY /2., padcopper_thickness /2.);
   TGeoVolume* trdmod1_padcoppervol = new TGeoVolume(Form("trd1mod%dpadcopper", moduleType), trd_padcopper, padcopperVolMed);
   trdmod1_padcoppervol->SetLineColor(kOrange);
   TGeoTranslation *trd_padcopper_trans = new TGeoTranslation("", 0., 0., padcopper_position);
   module->AddNode(trdmod1_padcoppervol, 0, trd_padcopper_trans);

   // Pad Plane
   TGeoBBox* trd_padpcb = new TGeoBBox("", sizeX /2., sizeY /2., padplane_thickness /2.);
   TGeoVolume* trdmod1_padpcbvol = new TGeoVolume(Form("trd1mod%dpadplane", moduleType), trd_padpcb, padpcbVolMed);
   trdmod1_padpcbvol->SetLineColor(kBlue);
   TGeoTranslation *trd_padpcb_trans = new TGeoTranslation("", 0., 0., padplane_position);
   module->AddNode(trdmod1_padpcbvol, 0, trd_padpcb_trans);

   // Honeycomb
   TGeoBBox* trd_honeycomb = new TGeoBBox("", sizeX /2., sizeY /2., honeycomb_thickness /2.);
   TGeoVolume* trdmod1_honeycombvol = new TGeoVolume(Form("trd1mod%dhoneycomb", moduleType), trd_honeycomb, honeycombVolMed);
   trdmod1_honeycombvol->SetLineColor(kOrange);
   TGeoTranslation* trd_honeycomb_trans = new TGeoTranslation("", 0., 0., honeycomb_position);
   module->AddNode(trdmod1_honeycombvol, 0, trd_honeycomb_trans);

   // Carbon fiber layers
   TGeoBBox* trd_carbon = new TGeoBBox("", sizeX /2., sizeY /2., carbon_thickness /2.);
   TGeoVolume* trdmod1_carbonvol = new TGeoVolume(Form("trd1mod%dcarbon", moduleType), trd_carbon, carbonVolMed);
   trdmod1_carbonvol->SetLineColor(kGreen);
   TGeoTranslation* trd_carbon_trans = new TGeoTranslation("", 0., 0., carbon_position);
   module->AddNode(trdmod1_carbonvol, 0, trd_carbon_trans);


   // FEBs
   if (IncludeFebs) {
      // Create all FEBs and place them in an assembly which will be added to the TRD module
      TGeoBBox* trd_feb = new TGeoBBox("", activeAreaX /2., feb_thickness/2., febbox_thickness/2.);     // the FEB itself - as a cuboid
      TGeoVolume* trdmod1_feb = new TGeoVolume(Form("trd1mod%dfeb", moduleType), trd_feb, febVolMed);   // the FEB made of a certain medium
      trdmod1_feb->SetLineColor(kYellow);    // set blue color
      //      trdmod1_feb->SetLineColor(kBlue);    // set blue color

      TGeoVolumeAssembly* trd_feb_inclined = new TGeoVolumeAssembly(Form("trd1mod%dfebincl", moduleType)); // volume for inclined FEBs, then shifted along y
      TGeoVolumeAssembly* trd_feb_box      = new TGeoVolumeAssembly(Form("trd1mod%dfebbox", moduleType));  // the mother volume of all FEBs

      // translations + rotations
      TGeoTranslation *trd_feb_trans1;     // center to corner
      TGeoRotation    *trd_feb_rotation;   // rotation around x axis
      TGeoTranslation *trd_feb_trans2;     // corner back
      TGeoTranslation *trd_feb_y_position; // shift to y position on TRD
//      TGeoTranslation *trd_feb_null;       // no displacement

      Float_t feb_pos;
      Float_t feb_pos_y;

// replaced by matrix operation (see below)
//  //      Float_t yback, zback;
//  //      TGeoCombiTrans  *trd_feb_placement;
//  //      // fix Z back offset 0.3 at some point
//  //      yback = -    sin(feb_rotation_angle/180*3.141)  * febbox_thickness /2.;
//  //      zback = - (1-cos(feb_rotation_angle/180*3.141)) * febbox_thickness /2. + 0.3;
//  //      trd_feb_placement = new TGeoCombiTrans(0, feb_pos_y + yback, zback, trd_feb_rotation);
//  //      trd_feb_inclined->AddNode(trdmod1_feb, iFeb+1, trd_feb_placement);

//      trd_feb_null       = new TGeoTranslation("", 0., 0., 0.);  // empty operation
      trd_feb_trans1     = new TGeoTranslation("", 0.,-feb_thickness/2.,-febbox_thickness/2.);  // move bottom right corner to center
      trd_feb_trans2     = new TGeoTranslation("", 0., feb_thickness/2., febbox_thickness/2.);  // move bottom right corner back
      trd_feb_rotation   = new TGeoRotation();
      trd_feb_rotation->RotateX(feb_rotation_angle);

      TGeoHMatrix *trd_feb_incline = new TGeoHMatrix("");

//        (*trd_feb_incline) = (*trd_feb_null);        // OK
//        (*trd_feb_incline) = (*trd_feb_y_position);  // OK
//        (*trd_feb_incline) = (*trd_feb_trans1);      // OK
//        (*trd_feb_incline) = (*trd_feb_trans1) * (*trd_feb_y_position);  // OK
//        (*trd_feb_incline) = (*trd_feb_trans1) * (*trd_feb_trans2);      // OK
//        (*trd_feb_incline) = (*trd_feb_trans1) * (*trd_feb_rotation);    // OK
//        (*trd_feb_incline) =  (*trd_feb_trans1) * (*trd_feb_rotation) * (*trd_feb_trans2) * (*trd_feb_y_position);  // not OK 
          // trd_feb_y_position is displaced in rotated coordinate system

      // matrix operation to rotate FEB PCB around its corner on the backanel 
      (*trd_feb_incline) = (*trd_feb_trans1) * (*trd_feb_rotation) * (*trd_feb_trans2);  // OK
      trd_feb_inclined->AddNode(trdmod1_feb, 1, trd_feb_incline);

      Int_t nofFebs = FebsPerModule[ moduleType - 1 ];
      for (Int_t iFeb = 0; iFeb < nofFebs; iFeb++) {
        feb_pos   = (iFeb + 0.5) / nofFebs - 0.5;   // equal spacing o f FEBs on the backpanel
        feb_pos_y = feb_pos * activeAreaY;

        // shift inclined FEB in y to its final position
        trd_feb_y_position = new TGeoTranslation("", 0., feb_pos_y, feb_z_offset);  // with additional fixed offset in z direction
	//        trd_feb_y_position = new TGeoTranslation("", 0., feb_pos_y, 0.0);  // touching the backpanel with the corner
        trd_feb_box->AddNode(trd_feb_inclined, iFeb+1, trd_feb_y_position);  // position FEB in y

      }
      TGeoTranslation* trd_febbox_trans = new TGeoTranslation("", 0., 0., febbox_position);
      gGeoMan->GetVolume(name)->AddNode(trd_feb_box, 1, trd_febbox_trans);  // put febbox at correct z position wrt to the module
   }

   return module;
}

Int_t copy_nr(Int_t stationNr, Int_t layerNr, Int_t copyNr)
{
   return stationNr * 10000 + layerNr * 1000 + copyNr;
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

  if(!(DoExplode))  // if do not explode -> no explosion
   ExplodeFactor = 1.00;
  
  Int_t copyNrIn[4] = { 0, 0, 0, 0 }; // copy number for each module type
  for ( Int_t type = 1; type <= 4; type++) {
    for ( Int_t j = (innerarray_size1-1); j >= 0; j--)  { // start from the bottom 
      for ( Int_t i = 0; i < innerarray_size2; i++) {
        module_id = *(innerLayer + (j * innerarray_size2 + i));
        if ( module_id /100 == type) {
          Int_t y = -(j-2);
          Int_t x =   i-2;

          // displacement
          Float_t dx = 0;
          Float_t dy = 0;
          Float_t dz = 0;

          if(DisplaceRandom)
	  {
            dx = (r3.Rndm()-.5) * 2 * maxdx;  // max +- 0.1 cm shift
            dy = (r3.Rndm()-.5) * 2 * maxdy;  // max +- 0.1 cm shift
            dz = (r3.Rndm()-.5) * 2 * maxdz;  // max +- 1.0 cm shift
          }

          Float_t xPos = DetectorSizeX[0] * x * ExplodeFactor + dx;
          Float_t yPos = DetectorSizeY[0] * y * ExplodeFactor + dy;
          copyNrIn[type - 1]++;
          Int_t copy = copy_nr(stationNr, layerNrInStation, copyNrIn[type - 1]);

          // take care of FEB orientation away from beam
          module_rotation = new TGeoRotation();   // need to renew rotation to start from 0 degree angle
          if ( isRotated == 0 )  // layer 1,3 ...
	    //   	     module_rotation->RotateZ( (module_id /10 %10) * 90. );  // rotate   0 or 180 degrees, see layer[1-3][i,o]
   	     module_rotation->RotateZ( (module_id /10 %10) * 90. );  // rotate   0 or 180 degrees, see layer[1-3][i,o]
          else  // layer 2,4 ...
	    //   	     module_rotation->RotateZ( (module_id %10) * 90. );      // rotate  90 or 270 degrees, see layer[1-3][i,o]
   	     module_rotation->RotateZ( (module_id %10) * 90. );      // rotate  90 or 270 degrees, see layer[1-3][i,o]

          // rotation
          Float_t drotx = 0;
          Float_t droty = 0;
          Float_t drotz = 0;

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
          gGeoMan->GetVolume(geoVersion)->AddNode(gModules[type - 1], copy, module_placement);
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
          Float_t dx = 0;
          Float_t dy = 0;
          Float_t dz = 0;

          if(DisplaceRandom)
	  {
            dx = (r3.Rndm()-.5) * 2 * maxdx;  // max +- 0.1 cm shift
            dy = (r3.Rndm()-.5) * 2 * maxdy;  // max +- 0.1 cm shift
            dz = (r3.Rndm()-.5) * 2 * maxdz;  // max +- 1.0 cm shift
          }

          Float_t xPos = DetectorSizeX[1] * x * ExplodeFactor + dx;
          Float_t yPos = DetectorSizeY[1] * y * ExplodeFactor + dy;
          copyNrOut[type - 5]++;
          Int_t copy = copy_nr(stationNr, layerNrInStation, copyNrOut[type - 5]);

          // take care of FEB orientation - away from beam
          module_rotation = new TGeoRotation();   // need to renew rotation to start from 0 degree angle
          if ( isRotated == 0 )  // layer 1,3 ...
	    //          module_rotation->RotateZ( (module_id /10 %10) * 90. );  // rotate   0 or 180 degrees, see layer[1-3][i,o]
            module_rotation->RotateZ( (module_id /10 %10) * 90. );  // rotate   0 or 180 degrees, see layer[1-3][i,o]
          else  // layer 2,4 ...
	    //          module_rotation->RotateZ( (module_id %10) * 90. );      // rotate  90 or 270 degrees, see layer[1-3][i,o]
            module_rotation->RotateZ( (module_id %10) * 90. );      // rotate  90 or 270 degrees, see layer[1-3][i,o]
    
          // rotation
          Float_t drotx = 0;
          Float_t droty = 0;
          Float_t drotz = 0;

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
          gGeoMan->GetVolume(geoVersion)->AddNode(gModules[type - 1], copy, module_placement);
        }
      }
    }
  }  
}
