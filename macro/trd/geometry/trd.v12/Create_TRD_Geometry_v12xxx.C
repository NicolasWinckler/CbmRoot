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

//void create_parameters() 
//{

// Parameters defing the layout of the complete detector build out of different 
// detector layers.
const Int_t NrOfLayers = 10;
const Int_t NrOfDifferentLayerTypes = 3;
const Int_t layerType[NrOfLayers] = { 10, 11, 10, 11, 20, 21, 20, 21, 30, 31};//, 3, 13 };
const Float_t layerPosition[NrOfLayers] = { 450., 500., 550., 600., 675., 725., 775., 825., 900., 950.};//, 6500, 8500 };
const Float_t Layer_thickness=49.5; // Thickness of one layer of a TRD station
const Float_t Layer_pitch=50.0; // Distance between 2 adjacent layers of a TRD s

const Int_t layerNrOfSectors[NrOfDifferentLayerTypes] = { 2, 2, 1 };
const Int_t layerArraySize[3][4] =  { { 5, 5, 9, 11 },
				      { 5, 5, 9, 11 },
				      { 5, 5, 9, 11 } };
// ### Layer Type 1
// v12x - module types in the inner sector of layer1 - looking upstream
const Int_t layer1i[5][5] = { { 4,  3,  2,  3,  4 },
			      { 3,  1,  1,  1,  3 },
			      { 2,  1,  0,  1,  2 },
			      { 3,  1,  1,  1,  3 },
			      { 4,  3,  2,  3,  4 } };
// number of modules 1x0, 8x1, 4x2, 8x3, 4x4

// v12x - module types in the outer sector of layer1 - looking upstream
const Int_t layer1o[9][11]= { { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
			      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
			      { 0,  0,  8,  8,  7,  7,  7,  8,  8,  0,  0 },
			      { 0,  0,  8,  7,  0,  0,  0,  7,  8,  0,  0 },
			      { 0,  0,  8,  7,  0,  0,  0,  7,  8,  0,  0 },
			      { 0,  0,  8,  7,  0,  0,  0,  7,  8,  0,  0 },
			      { 0,  0,  8,  8,  7,  7,  7,  8,  8,  0,  0 },
			      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
			      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 } };
// number of modules 73x0, 0x5, 0x6, 12x7, 14x8
// Layer1 =  24 + 26;   // v12x

// ### Layer Type 11 is Layer Type 1 with detector modules rotated by 90°
// In the subroutine creating the layers this is recognized automatically 

// ### Layer Type 2

// v12x - module types in the inner sector of layer2 - looking upstream
Int_t layer2i[5][5] = { { 4,  3,  3,  3,  4 },     
			{ 3,  2,  1,  2,  3 },     
			{ 3,  1,  0,  1,  3 },     
			{ 3,  2,  1,  2,  3 },     
			{ 4,  3,  3,  3,  4 } };   
// number of modules 1x0, 4x1, 4x2, 12x3, 4x4

// v12x - module types in the outer sector of layer2 - looking upstream
Int_t layer2o[9][11]= { { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
                          { 0,  8,  8,  8,  8,  8,  8,  8,  8,  8,  0 },
                          { 0,  8,  8,  8,  6,  6,  6,  8,  8,  8,  0 },
                          { 0,  8,  8,  6,  0,  0,  0,  6,  8,  8,  0 },
                          { 0,  8,  8,  6,  0,  0,  0,  6,  8,  8,  0 },
                          { 0,  8,  8,  6,  0,  0,  0,  6,  8,  8,  0 },
                          { 0,  8,  8,  8,  6,  6,  6,  8,  8,  8,  0 },
                          { 0,  8,  8,  8,  8,  8,  8,  8,  8,  8,  0 },
                          { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 } };
// number of modules 45x0, 0x5, 12x6, 0x7, 42x8
// Layer2 =  78;   // v12x

// ### Layer Type 21 is Layer Type 2 with detector modules rotated by 90°
// In the subroutine creating the layers this is recognized automatically 

// ### Layer Type 3

// v12x - module types in the inner sector of layer2 - looking upstream
Int_t layer3i[5][5] = { { 0,  0,  0,  0, 0 },     
			{ 0,  0,  0,  0, 0 },     
			{ 0,  0,  0,  0, 0 },     
			{ 0,  0,  0,  0, 0 },     
			{ 0,  0,  0,  0, 0 } };   
// number of modules 25x0 
// Only for convinience in the function needed


// v12x - module types in the outer sector of layer3 - looking upstream
Int_t layer3o[9][11] = { { 8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8 },
			 { 8,  8,  8,  7,  7,  7,  7,  7,  8,  8,  8 },
			 { 8,  8,  7,  7,  6,  6,  6,  7,  7,  8,  8 },
			 { 8,  8,  7,  6,  5,  5,  5,  6,  7,  8,  8 },
			 { 8,  8,  7,  6,  5,  0,  5,  6,  7,  8,  8 },
			 { 8,  8,  7,  6,  5,  5,  5,  6,  7,  8,  8 },
			 { 8,  8,  7,  7,  6,  6,  6,  7,  7,  8,  8 },
			 { 8,  8,  8,  7,  7,  7,  7,  7,  8,  8,  8 },
			 { 8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8 } };
// number of modules 1x0, 8x5, 12x6, 24x7, 54x8
// Layer3 = 98;   // v12x

// ### Layer Type 31 is Layer Type 3 with detector modules rotated by 90°
// In the subroutine creating the layers this is recognized automatically 

// Parameters defining the layout of the different detector modules
const Int_t NrModuleTypes=8;
const Int_t TypeOfModule[NrModuleTypes]={0,0,0,0,1,1,1,1};
const Int_t febs_per_module[NrModuleTypes] ={ 19, 10, 5, 3, 12, 6, 4, 3 }; 

const Float_t Frame_width[2]={ 1.5, 2. };         // Width of detector frames in cm
const Float_t Detector_size_x[2] = { 60., 100.};
const Float_t Detector_size_y[2] = { 60., 100.};

const Float_t radiator_thickness    =  36.0 /2.;
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

const  Float_t febbox_thickness      =  10.000 /2.;
const  Float_t febbox_position       =  electronics_position + electronics_thickness + febbox_thickness/2;
const  Float_t feb_thickness         =  .5/2;

// Names of the different used materials which are used to build the modules
// The materials are defined in the global media.geo file 
const TString keepingVolumeMedium ="air";
const TString radiatorVolumeMedium ="polypropylene";
const TString gasVolumeMedium ="TRDgas";
const TString padVolumeMedium ="goldcoatedcopper";
const TString mylarVolumeMedium ="mylar";
const TString electronicsVolumeMedium ="goldcoatedcopper";
const TString frameVolumeMedium ="G10";
const TString febVolumeMedium ="pefoam20";


// some global variables
TGeoManager* gGeoMan = NULL;  // Pointer to TGeoManager instance
TGeoVolume* trd_mod[NrModuleTypes];
TGeoVolume* trd_layer[NrOfLayers];
//}

// Forward declarations
void create_materials_from_media_file();
void create_trd_module(Int_t moduleType);
void create_detector_layers(Int_t layer);

void Create_TRD_Geometry_v12xxx() {

  //  create_parameters();
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
  gGeoMan->SetVisLevel(10);  
  
  // Create the top volume 
  TGeoBBox *topbox= new TGeoBBox("", 10000., 
					 10000., 10000.);
  TGeoVolume* top = new TGeoVolume("top", 
				   topbox, gGeoMan->GetMedium("air"));

  //   TGeoVolume* top = new TGeoVolumeAssembly("TRD");
  gGeoMan->SetTopVolume(top);
  TGeoVolume* trd_v_12a = new TGeoVolumeAssembly("trd_v_12a");

  TGeoTranslation *no_trans = new TGeoTranslation("", 0., 0., 0.);
  top->AddNode(trd_v_12a, 1);

  TGeoTranslation *glob_trans;
  for (Int_t iModule = 0; iModule < NrModuleTypes; iModule++) {
    create_trd_module(iModule+1);
    //trd_v_12a->AddNode(trd_mod[iModule], 1, glob_trans);
  }
  
  
  for (Int_t iLayer = 0; iLayer < NrOfLayers; iLayer++) {
    create_detector_layers(iLayer+1);
    glob_trans = new TGeoTranslation("", 0., 0., layerPosition[iLayer]);
    trd_v_12a->AddNode(trd_layer[iLayer], 1, glob_trans);
  }
  

  gGeoMan->CloseGeometry();
  gGeoMan->CheckOverlaps(0.001);
  gGeoMan->PrintOverlaps();
  gGeoMan->Test();

  TFile* outfile = new TFile("TRD_geom_v12b.root","RECREATE");
  //top->Write();
  gGeoMan->Write();
  outfile->Close();
  //top->Draw("ogl");
  //top->Raytrace();

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

  FairGeoMedium* FairMediumAir = 
    geoMedia->getMedium("air");

  FairGeoMedium* FairMediumPolyPropylene  = 
    geoMedia->getMedium("polypropylene");

  FairGeoMedium* FairMediumTrdGas = 
    geoMedia->getMedium("TRDgas");

  FairGeoMedium* FairMediumGoldCoatedCopper  = 
    geoMedia->getMedium("goldcoatedcopper");

  FairGeoMedium* FairMediumMylar = 
    geoMedia->getMedium("mylar");

  FairGeoMedium* FairMediumG10  = 
    geoMedia->getMedium("G10");

  FairGeoMedium* FairMediumPeFoam20  = 
    geoMedia->getMedium("pefoam20");

  geoBuild->createMedium(FairMediumAir);
  geoBuild->createMedium(FairMediumPolyPropylene);
  geoBuild->createMedium(FairMediumTrdGas);
  geoBuild->createMedium(FairMediumGoldCoatedCopper);
  geoBuild->createMedium(FairMediumMylar);
  geoBuild->createMedium(FairMediumG10);
  geoBuild->createMedium(FairMediumPeFoam20);
}

void create_trd_module(Int_t moduleType)
{
  Int_t typeOfModule = TypeOfModule[moduleType-1];
  
  Float_t ModuleSizeX = Detector_size_x[ typeOfModule ]; 
  Float_t ModuleSizeY = Detector_size_y[ typeOfModule ]; 

  Float_t frameWidth = Frame_width[ typeOfModule ];

  Int_t nrOfFebs = febs_per_module[ moduleType - 1 ];

  cout<<"Nr. Of FEBs: "<< nrOfFebs<<endl;
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
  TGeoMedium* febVolMed = gGeoMan->GetMedium(febVolumeMedium);

  TString name = Form("trd_mod%d", moduleType);
  trd_mod[moduleType-1] = new TGeoVolumeAssembly(name);
   
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


   // Create all FEBs and place them in an assembly which will added 
   // to the trd module
   TGeoVolumeAssembly* trd_feb_box = 
     new TGeoVolumeAssembly(Form("trd_mod%d_febbox", moduleType));

   TGeoBBox *trd_feb = 
     new TGeoBBox("", Active_area_x/2, feb_thickness/2, 
		  febbox_thickness/2);
   TGeoVolume* trdmod1_feb = 
     new TGeoVolume(Form("trd_mod%d_feb", moduleType), 
		    trd_feb, febVolMed);

   trdmod1_feb->SetLineColor(kBlack);

   // translations 
   TGeoTranslation *trd_feb_trans;
   Float_t feb_pos;
   Float_t feb_pos_y;
   for (Int_t iFeb = 0; iFeb < nrOfFebs; iFeb++) {
     feb_pos   = (2. * iFeb + 1) / (2 * nrOfFebs) - 0.5;
     feb_pos_y = feb_pos * Active_area_y;

     trd_feb_trans = new TGeoTranslation("", 0., feb_pos_y, 0.);
     trdmod1_feb->SetLineColor(kBlack);
     trd_feb_box->AddNode(trdmod1_feb, iFeb+1, trd_feb_trans);
   }
   trd_febbox_trans = new TGeoTranslation("", 0., 0., febbox_position);
   gGeoMan->GetVolume(name)->AddNode(trd_feb_box, 1, trd_febbox_trans);
   
}

void create_detector_layers(Int_t layer)
{

  TString name = Form("trd_layer%d", layer);
  cout<<"Creating "<<name<<endl;
  trd_layer[layer-1] = new TGeoVolumeAssembly(name);

  Int_t typeOfLayer = layerType[ layer-1 ]/10;
  Int_t isRotated = layerType[ layer-1 ]%10;
  TGeoRotation* module_rotation;

  if ( 1 == isRotated) {
    module_rotation = new TGeoRotation();
    module_rotation->RotateZ(90.);
  } else {
    module_rotation = new TGeoRotation();
    module_rotation->RotateZ(0.);
  }
  
  TGeoCombiTrans* module_placement;
  
  Int_t innerarray_size1 = layerArraySize[typeOfLayer-1][0];
  Int_t innerarray_size2 = layerArraySize[typeOfLayer-1][1];
  Int_t* innerLayer;
  
  Int_t outerarray_size1 = layerArraySize[typeOfLayer-1][2];
  Int_t outerarray_size2 = layerArraySize[typeOfLayer-1][3];
  Int_t* outerLayer;
  
  if ( 1 == typeOfLayer ) {
    innerLayer = layer1i;      
    outerLayer = layer1o; 
  } elseif ( 2 == typeOfLayer ) {
    innerLayer = layer2i;      
    outerLayer = layer2o; 
  } elseif ( 3 == typeOfLayer ) {
    innerLayer = layer3i;      
    outerLayer = layer3o; 
  } else {
    cout<<"Type of layer not known"<<endl;
  } 
  
  Int_t a = 0;
  for ( Int_t type = 1; type <= 4; type++) {
    for ( Int_t j = (innerarray_size1-1); j >= 0; j--)  { // start from the bottom 
      for ( Int_t i = 0; i < innerarray_size2; i++) {
	if ( *(innerLayer + (j * innerarray_size2 + i)) == type) {          
	  Int_t y = -(j-2);
	  Int_t x =   i-2;
	  Int_t xPos = Detector_size_x[0] * x;
	  Int_t yPos = Detector_size_y[0] * y;	    
	  a++;
	  module_placement = new TGeoCombiTrans(xPos, yPos, 0.,  
						module_rotation);
	  gGeoMan->GetVolume(name)->
	    AddNode(trd_mod[type-1], a, module_placement);
	  
	}
      }
    }
  }
  
  
  for ( Int_t type = 5; type <= 8; type++) {
    for ( Int_t j = (outerarray_size1-1); j >= 0; j--)  { // start from the bottom 
      for ( Int_t i = 0; i < outerarray_size2; i++) {
	if ( *(outerLayer + (j * outerarray_size2 + i)) == type) {          
	  Int_t y = -(j-4);
	  Int_t x =   i-5;
	  Int_t xPos = Detector_size_x[1] * x;
	  Int_t yPos = Detector_size_y[1] * y;	    
	  a++;
	  module_placement = new TGeoCombiTrans(xPos, yPos, 0.,  
						module_rotation);
	  gGeoMan->GetVolume(name)->
	    AddNode(trd_mod[type-1], a, module_placement);
	  
	}
      }
    }
  }  
}
