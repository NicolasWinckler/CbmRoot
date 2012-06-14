#include "TSystem.h"
#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TGeoMaterial.h"
#include "TGeoMedium.h"
#include "TGeoPgon.h"
#include "TGeoMatrix.h"
#include "TFile.h"
#include "TString.h"
#include "TList.h"

#include <iostream>

// some global variables
TGeoManager* gGeoMan = NULL;  // Pointer to TGeoManager instance

//---------- Declaration of parameters ----------------------------------
//const Float_t Distance[3]={450., 675., 900.}; // Distance between target and the front of the TRD stations

//const Int_t   Station_number=3; // Number of TRD stations in the setup
//const Int_t   Layer_number=4;   // Number of detector layers per station
const Float_t Layer_pitch=50.0; // Distance between 2 adjacent layers of a TRD station
//const Float_t Station_thickness=Layer_number * Layer_pitch; // Thickness of one TRD station

// Parameters defining the layout of the different detector modules

const Float_t Layer_thickness=49.5; // Thickness of one layer of a TRD station
const Float_t Frame_width[2]={ 1.5, 2. };         // Width of detector frames in cm
const Float_t Detector_size_x[2] = { 60., 100.};
const Float_t Detector_size_y[2] = { 60., 100.};

const Float_t radiator_thickness    =  2.9000 /2.;
const Float_t radiator_position     =  - Layer_thickness /2. + radiator_thickness;
const Float_t gas_thickness         =  1.2000 /2.;
const Float_t gas_position          =  radiator_position + radiator_thickness + gas_thickness;
const Float_t padplane_thickness    =   0.0030 /2.;
const Float_t padplane_position     =  gas_position + gas_thickness + padplane_thickness;
const Float_t mylar_thickness       =   0.1500 /2.;
const Float_t mylar_position        =  padplane_position + padplane_thickness + mylar_thickness;
const Float_t electronics_thickness =   0.0070 /2.;
const Float_t electronics_position  =  mylar_position + mylar_thickness + electronics_thickness;
const Float_t frame_thickness       =  radiator_thickness + gas_thickness + padplane_thickness + mylar_thickness + electronics_thickness;
const Float_t frame_position        =  frame_thickness - Layer_thickness /2.;

const TString keepingVolumeMedium ="air";
const TString radiatorVolumeMedium ="polypropylene";
const TString gasVolumeMedium ="TRDgas";
const TString padVolumeMedium ="goldcoatedcopper";
const TString mylarVolumeMedium ="mylar";
const TString electronicsVolumeMedium ="goldcoatedcopper";
const TString frameVolumeMedium ="G10";


const Int_t NrModuleTypes=8;
const Int_t TypeOfModule[NrModuleTypes]={0,0,0,0,1,1,1,1};
const Int_t febs_per_module[NrModuleTypes] ={ 19, 10, 5, 3, 12, 6, 4, 3 }; 

//Parameters defing the layout of the complete detector build out of different 
// detector layers.
const Int_t NrOfLayers = 12;
const Int_t layerType[NrOfLayers] = { 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 0, 0 };
const Float_t layerPosition[NrOfLayers] = { 4500, 5000, 5500, 6000, 6750, 7250, 7750, 8250, 9000, 9500, 6500, 8500 };


TGeoVolume* trd_mod[NrModuleTypes];

void create_trd_module(Int_t moduleType);

void create_materials_from_media_file();

//void create_trd_feb();
//void create_trd_module();

void Create_TRD_Geometry_v12xxx() {

  // Load the necessary FairRoot libraries 
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");

  // Load needed material definition from media.geo file
  create_materials_from_media_file();
  
  // Get the GeoManager for later usage
  //  TGeoManager* gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
  gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
  
  // Create the top volume 
  TGeoVolume* top = new TGeoVolumeAssembly("TRD");
  gGeoMan->SetTopVolume(top);
  TGeoVolume* trd_v_12a = new TGeoVolumeAssembly("trd_v_12a");

  TGeoTranslation *no_trans = new TGeoTranslation("", 0., 0., 0.);
  top->AddNode(trd_v_12a, 1);

  create_trd_module(1);
  //  TGeoVolume* trd_mod1 = gGeoMan->GetVolume("trd_mod1");
  trd_v_12a->AddNode(trd_mod[0], 1);
   

  gGeoMan->CloseGeometry();
  TFile* outfile = new TFile("TRD_geom_v12b.root","RECREATE");
  top->Write();
  //gGeoMan->Write();
  outfile->Close();
  top->Draw("ogl");
  //top->Raytrace();

}

void create_trd_module(Int_t moduleType)
{
  Int_t typeOfModule = TypeOfModule[moduleType-1];
  
  Float_t ModuleSizeX = Detector_size_x[ typeOfModule ]; 
  Float_t ModuleSizeY = Detector_size_y[ typeOfModule ]; 

  Float_t frameWidth = Frame_width[ typeOfModule ];

  Float_t Active_area_x = ModuleSizeX - 2 * frameWidth;
  Float_t Active_area_y = ModuleSizeY - 2 * frameWidth;

  /*
  cout<<"Module size X: "<< ModuleSizeX<<endl;
  cout<<"Module size Y: "<< ModuleSizeY<<endl;
  cout<<"Active size X: "<<Active_area_x<<endl;
  cout<<"Active size Y: "<<Active_area_y<<endl;
  */

  TGeoMedium* keepVolMed = gGeoMan->GetMedium(keepingVolumeMedium);
  TGeoMedium* radVolMed = gGeoMan->GetMedium(radiatorVolumeMedium);
  TGeoMedium* gasVolMed = gGeoMan->GetMedium(gasVolumeMedium);
  TGeoMedium* padVolMed = gGeoMan->GetMedium(padVolumeMedium);
  TGeoMedium* mylarVolMed = gGeoMan->GetMedium(mylarVolumeMedium);
  TGeoMedium* electronicsVolMed = gGeoMan->GetMedium(electronicsVolumeMedium);
  TGeoMedium* frameVolMed = gGeoMan->GetMedium(frameVolumeMedium);

  TString name = Form("trd_mod%d", moduleType);
  TGeoBBox *trd_box = new TGeoBBox("", ModuleSizeX/2, 
				   ModuleSizeY/2, Layer_thickness/2);
  trd_mod[moduleType-1] = new TGeoVolume(name, trd_box, keepVolMed);
   
   // Radiator
   TGeoBBox *trd_radiator = new TGeoBBox("", Active_area_x/2, 
					 Active_area_y/2, radiator_thickness);
   TGeoVolume* trdmod1_radvol = new TGeoVolume(Form("trd_mod%d_radiator", 
					       moduleType), trd_radiator, 
					       radVolMed);
   trdmod1_radvol->SetLineColor(kBlue);
   TGeoTranslation *trd_radiator_trans = new TGeoTranslation("", 0., 0., 
							     radiator_position);
   gGeoMan->GetVolume(name)->AddNode(trdmod1_radvol, 0, trd_radiator_trans);


   // Gas
   TGeoBBox *trd_gas = new TGeoBBox("", Active_area_x/2, Active_area_y/2, 
				    gas_thickness);
   TGeoVolume* trdmod1_gasvol = new TGeoVolume(Form("trd_mod%d_gas", moduleType), 
					       trd_gas, gasVolMed);
   TGeoTranslation *trd_gas_trans = new TGeoTranslation("", 0., 0., gas_position);
   gGeoMan->GetVolume(name)->AddNode(trdmod1_gasvol, 0, trd_gas_trans);


   // Pad Plane
   TGeoBBox *trd_pad = new TGeoBBox("", Active_area_x/2, Active_area_y/2, 
				    padplane_thickness);
   TGeoVolume* trdmod1_padvol = new TGeoVolume(Form("trd_mod%d_padplane", moduleType), 
					       trd_pad, padVolMed);
   TGeoTranslation *trd_pad_trans = new TGeoTranslation("", 0., 0., padplane_position);
   gGeoMan->GetVolume(name)->AddNode(trdmod1_padvol, 0, trd_pad_trans);


   // mylar
   TGeoBBox *trd_mylar = new TGeoBBox("", Active_area_x/2, Active_area_y/2, 
				      mylar_thickness);
   TGeoVolume* trdmod1_mylarvol = new TGeoVolume(Form("trd_mod%d_mylar", moduleType), 
						 trd_mylar, mylarVolMed);
   TGeoTranslation *trd_mylar_trans = new TGeoTranslation("", 0., 0., mylar_position);
   gGeoMan->GetVolume(name)->AddNode(trdmod1_mylarvol, 0, trd_mylar_trans);


   // electronics
   TGeoBBox *trd_el = new TGeoBBox("", Active_area_x/2, Active_area_y/2, 
				   electronics_thickness);
   TGeoVolume* trdmod1_elvol = new TGeoVolume(Form("trd_mod%d_electronics", moduleType), 
					      trd_el, electronicsVolMed);
   trdmod1_elvol->SetLineColor(kGreen);
   TGeoTranslation *trd_el_trans = new TGeoTranslation("", 0., 0., electronics_position);
   gGeoMan->GetVolume(name)->AddNode(trdmod1_elvol, 0, trd_el_trans);


   // frame1
   TGeoBBox *trd_frame1 = new TGeoBBox("", ModuleSizeX/2, frameWidth/2, frame_thickness);
   TGeoVolume* trdmod1_frame1vol = new TGeoVolume(Form("trd_mod%d_frame1", moduleType), 
						  trd_frame1, frameVolMed);
   trdmod1_frame1vol->SetLineColor(kRed);

   // translations 
   TGeoTranslation *trd_frame1_trans = new TGeoTranslation("", 0., 
							   Active_area_y/2+frameWidth/2, 
							   frame_position);
   gGeoMan->GetVolume(name)->AddNode(trdmod1_frame1vol, 1, trd_frame1_trans);
   trd_frame1_trans = new TGeoTranslation("", 0., -(Active_area_y/2+frameWidth/2), 
					  frame_position);
   gGeoMan->GetVolume(name)->AddNode(trdmod1_frame1vol, 2, trd_frame1_trans);


   // frame2
   TGeoBBox *trd_frame2 = new TGeoBBox("", frameWidth/2, Active_area_y/2, 
				       frame_thickness);
   TGeoVolume* trdmod1_frame2vol = new TGeoVolume(Form("trd_mod%d_frame2", moduleType), 
						  trd_frame2, frameVolMed);
   trdmod1_frame2vol->SetLineColor(kRed);

   // translations 
   TGeoTranslation *trd_frame2_trans = new TGeoTranslation("", 
							   Active_area_x/2+frameWidth/2, 
							   0., frame_position);
   gGeoMan->GetVolume(name)->AddNode(trdmod1_frame2vol, 1, trd_frame2_trans);
   trd_frame2_trans = new TGeoTranslation("", -(Active_area_x/2+frameWidth/2), 0., 
					  frame_position);
   gGeoMan->GetVolume(name)->AddNode(trdmod1_frame2vol, 2, trd_frame2_trans);
}

void create_materials_from_media_file()
{

  // Use the FairRoot geometry interface to load the media which are
  // already defined 
  FairGeoLoader*    geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
  FairGeoInterface* geoFace = geoLoad->getGeoInterface();
  TString geoPath = gSystem->Getenv("VMCWORKDIR");
  TString geoFile = geoPath + "/geometry/media.geo";
  geoFace->setMediaFile(geoFile);
  geoFace->readMedia();
  //  geoFace->print();

  // Read the required media and create them in the GeoManager
  FairGeoMedia*   geoMedia = geoFace->getMedia();
  FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();

  FairGeoMedium* FairMediumAir      = geoMedia->getMedium("air");
  FairGeoMedium* FairMediumPolyPropylene   = 
    geoMedia->getMedium("polypropylene");
  FairGeoMedium* FairMediumTrdGas = geoMedia->getMedium("TRDgas");
  FairGeoMedium* FairMediumGoldCoatedCopper  = 
    geoMedia->getMedium("goldcoatedcopper");
  FairGeoMedium* FairMediumMylar   = geoMedia->getMedium("mylar");
  FairGeoMedium* FairMediumG10   = geoMedia->getMedium("G10");

  geoBuild->createMedium(FairMediumAir);
  geoBuild->createMedium(FairMediumPolyPropylene);
  geoBuild->createMedium(FairMediumTrdGas);
  geoBuild->createMedium(FairMediumGoldCoatedCopper);
  geoBuild->createMedium(FairMediumMylar);
  geoBuild->createMedium(FairMediumG10);
}

//void create_trd_feb(){}
//void create_trd_module(){}
