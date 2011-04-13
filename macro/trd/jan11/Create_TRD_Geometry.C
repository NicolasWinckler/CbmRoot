#include "TSystem.h"
#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TGeoMaterial.h"
#include "TGeoMedium.h"
#include "TGeoPgon.h"
#include "TGeoMatrix.h"
#include "TFile.h"
#include "TString.h"

#include <iostream>

void create_keeping_volumes(const char* name, Int_t station, Float_t* first,
			    Float_t* second, Float_t* radius1,
			    Float_t* radius2, Float_t* distance, 
			    Float_t thickness, 
			    TGeoVolume* vol, TGeoMedium* med);


void Create_TRD_Geometry() {

  // Create the GeoManager and the top volume 
  gSystem->Load("libGeom");
  TGeoManager* gGeoMan = new TGeoManager("TRDGeom", "TRD geometry"); 
  TGeoVolume* top = new TGeoVolumeAssembly("TRD");
  gGeoMan->SetTopVolume(top);
  

  // Define all media and materials needed for the TRD detector.
  // The code is taken from a dump of an already existing GeoManager
  // A material in ROOT is a real material together with some special
  // settings for simulation (e.g. Is the volume sensitive, maximum 
  // field etc. In principle the materials and mixtures are not so
  // important here, because the materials are taken from the CbmRoot
  // framework and not from the produced geometry file. 
  // Only the names must correspond to the names defined in CbmRoot    
  Int_t nel, numed;
  Double_t density, a, z, w;
  Double_t par[8];
  
  // Mixture: air
  nel     = 3;
  density = 0.001205;
  TGeoMixture* pMat1 = new TGeoMixture("air", nel,density);
  a = 14.010000;   z = 7.000000;   w = 0.780000;  // N
  pMat1->DefineElement(0,a,z,w);
  a = 16.000000;   z = 8.000000;   w = 0.210000;  // O
  pMat1->DefineElement(1,a,z,w);
  a = 39.950000;   z = 18.000000;   w = 0.010000;  // AR
  pMat1->DefineElement(2,a,z,w);
  pMat1->SetIndex(0);
  // Medium: air
  numed   = 1;  // medium number
  par[0]  = 0.000000; // isvol
  par[1]  = 1.000000; // ifield
  par[2]  = 20.000000; // fieldm
  par[3]  = -1.000000; // tmaxfd
  par[4]  = -1.000000; // stemax
  par[5]  = -1.000000; // deemax
  par[6]  = 0.001000; // epsil
  par[7]  = -1.000000; // stmin
  TGeoMedium* air = new TGeoMedium("air", numed,pMat1, par);
  
  // Mixture: polypropylene
  nel     = 2;
  density = 0.074000;
  TGeoMixture*  pMat16 = new TGeoMixture("polypropylene", nel,density);
  a = 12.010000;   z = 6.000000;   w = 0.856267;  // C
  pMat16->DefineElement(0,a,z,w);
  a = 1.008000;   z = 1.000000;   w = 0.143733;  // H
  pMat16->DefineElement(1,a,z,w);
  pMat16->SetIndex(1);
  // Medium: polypropylene
  numed   = 2;  // medium number
  par[0]  = 0.000000; // isvol
  par[1]  = 0.000000; // ifield
  par[2]  = 20.000000; // fieldm
  par[3]  = -1.000000; // tmaxfd
  par[4]  = -1.000000; // stemax
  par[5]  = -1.000000; // deemax
  par[6]  = 0.001000; // epsil
  par[7]  = -1.000000; // stmin
  TGeoMedium* polypropylene = new TGeoMedium("polypropylene", numed,pMat16, par);
  // Mixture: TRDgas
  nel     = 3;
  density = 0.004944;
  TGeoMixture*  pMat17 = new TGeoMixture("TRDgas", nel,density);
  a = 12.011000;   z = 6.000000;   w = 0.015243;  // C
  pMat17->DefineElement(0,a,z,w);
  a = 15.994000;   z = 8.000000;   w = 0.040595;  // O
  pMat17->DefineElement(1,a,z,w);
  a = 131.290000;   z = 54.000000;   w = 0.944162;  // XE
  pMat17->DefineElement(2,a,z,w);
  pMat17->SetIndex(2);
  // Medium: TRDgas
  numed   = 3;  // medium number
  par[0]  = 1.000000; // isvol
  par[1]  = 0.000000; // ifield
  par[2]  = 20.000000; // fieldm
  par[3]  = -1.000000; // tmaxfd
  par[4]  = -1.000000; // stemax
  par[5]  = -1.000000; // deemax
  par[6]  = 0.000100; // epsil
  par[7]  = -1.000000; // stmin
  TGeoMedium* TRDgas = new TGeoMedium("TRDgas", numed,pMat17, par);
  
  // Mixture: goldcoatedcopper
  nel     = 2;
  density = 9.063400;
  TGeoMixture* pMat18 = new TGeoMixture("goldcoatedcopper", nel,density);
  a = 63.540000;   z = 29.000000;   w = 0.990000;  // CU
  pMat18->DefineElement(0,a,z,w);
  a = 196.970000;   z = 79.000000;   w = 0.010000;  // AU
  pMat18->DefineElement(1,a,z,w);
  pMat18->SetIndex(3);
  // Medium: goldcoatedcopper
  numed   = 4;  // medium number
  par[0]  = 0.000000; // isvol
  par[1]  = 0.000000; // ifield
  par[2]  = 20.000000; // fieldm
  par[3]  = -1.000000; // tmaxfd
  par[4]  = -1.000000; // stemax
  par[5]  = -1.000000; // deemax
  par[6]  = 0.001000; // epsil
  par[7]  = -1.000000; // stmin
  TGeoMedium* goldcoatedcopper = new TGeoMedium("goldcoatedcopper", numed,pMat18, par);
  
  // Mixture: mylar
  nel     = 3;
  density = 1.390000;
  TGeoMixture*  pMat19 = new TGeoMixture("mylar", nel,density);
  a = 1.008000;   z = 1.000000;   w = 0.041964;  // H
  pMat19->DefineElement(0,a,z,w);
  a = 12.010000;   z = 6.000000;   w = 0.624987;  // C
  pMat19->DefineElement(1,a,z,w);
  a = 16.000000;   z = 8.000000;   w = 0.333049;  // O
  pMat19->DefineElement(2,a,z,w);
  pMat19->SetIndex(4);
  // Medium: mylar
  numed   = 5;  // medium number
  par[0]  = 0.000000; // isvol
  par[1]  = 0.000000; // ifield
  par[2]  = 20.000000; // fieldm
  par[3]  = -1.000000; // tmaxfd
  par[4]  = -1.000000; // stemax
  par[5]  = -1.000000; // deemax
  par[6]  = 0.001000; // epsil
  par[7]  = -1.000000; // stmin
  TGeoMedium* mylar = new TGeoMedium("mylar", numed,pMat19, par);
  
  // Mixture: G10
  nel     = 4;
  density = 1.700000;
  TGeoMixture* pMat20 = new TGeoMixture("G10", nel,density);
  a = 12.010000;   z = 6.000000;   w = 0.259000;  // C
  pMat20->DefineElement(0,a,z,w);
  a = 1.008000;   z = 1.000000;   w = 0.288000;  // H
  pMat20->DefineElement(1,a,z,w);
  a = 16.000000;   z = 8.000000;   w = 0.248000;  // O
  pMat20->DefineElement(2,a,z,w);
  a = 28.090000;   z = 14.000000;   w = 0.205000;  // SI
  pMat20->DefineElement(3,a,z,w);
  pMat20->SetIndex(5);
  // Medium: G10
  numed   = 6;  // medium number
  par[0]  = 0.000000; // isvol
  par[1]  = 0.000000; // ifield
  par[2]  = 20.000000; // fieldm
  par[3]  = -1.000000; // tmaxfd
  par[4]  = -1.000000; // stemax
  par[5]  = -1.000000; // deemax
  par[6]  = 0.001000; // epsil
  par[7]  = -1.000000; // stmin
  TGeoMedium* G10 = new TGeoMedium("G10", numed,pMat20, par);
  
  // Here comes now the creation of needed TRD modules.

  // Add the keeping volumes for each trd station to the top
  // volume

  //---------- Declaration of parameters ----------------------------------
  Float_t Distance[3];         // Distance between target an the front 
                               // of the TRD stations
  Float_t Station_thickness;   // Thickness of one TRD station
  Float_t Layer_thickness;     // Thickness of one layer of a TRD station
  Float_t Layer_pitch;         // Distance between 2 adjacent layers of a 
                               // TRD station
  int   Layer_number;          // Number of detector layers per station
  int   Station_number;        // Number of TRD stations in the setup
  Float_t Detector_size_x[3];  // length in mm of a detector module in 
                               // x-direction
  Float_t Detector_size_y[3];  // length in mm of a detector module in 
                               // y-direction
  Float_t Frame_width;         // Width of detector frames in mm
  Float_t Inner_radius[3];     // Inner acceptance in mm
  Float_t Outer_radius[3];     // Outer acceptance in mm

  //--------- Initialization of parameters -------------------------------------

  Distance[0]          = 450;
  Distance[1]          = 675;
  Distance[2]          = 900;

  Station_number       = 3;
  Layer_number         = 4;
  Layer_thickness      = 13.5;   
  Layer_pitch          = 15.0;   
  Station_thickness    = Layer_number * Layer_pitch;
  Frame_width          = 2;

// 29.06.2010 - DE - hardcoded keeping volumes
  Inner_radius[0] =  24.;  //  250 -  10
  Outer_radius[0] = 460.;  // 4500 + 100
  Inner_radius[1] =  24.;  //  250 -  10
  Outer_radius[1] = 660.;  // 6500 + 100
  Inner_radius[2] =  49.;  //  500 -  10
  Outer_radius[2] = 860.;  // 8500 + 100
 
  // fixed detector sizes, independent of z Position
  Detector_size_x[0] =  50;                 // x =  500 mm
  Detector_size_y[0] = Detector_size_x[0];   // y =  500 mm
  
  Detector_size_x[1] =  75;                 // x =  750 mm
  Detector_size_y[1] = Detector_size_x[1];   // y =  750 mm
  
  Detector_size_x[2] = 100;                 // x = 1000 mm
  Detector_size_y[2] = Detector_size_x[2];   // y = 1000 mm
  

  Float_t first_layer[3];
  Float_t second_layer[3];
  
  first_layer[0]=-130.;
  first_layer[1]=430.;
  first_layer[2]=460.;

  second_layer[0]=-30.;
  second_layer[1]=150.;
  second_layer[2]=460.;

  create_keeping_volumes("trd1", 1, first_layer, second_layer, 
			 Inner_radius, Outer_radius, Distance, 
			 Station_thickness, top, air);

  first_layer[0]=-180.;
  first_layer[1]=620.;
  first_layer[2]=660.;

  second_layer[0]=-30.;
  second_layer[1]=250.;
  second_layer[2]=660.;

  create_keeping_volumes("trd2", 2, first_layer, second_layer, 
			 Inner_radius, Outer_radius, Distance, 
			 Station_thickness, top, air);

  first_layer[0]=-230.;
  first_layer[1]=810.;
  first_layer[2]=860.;

  second_layer[0]=-30.;
  second_layer[1]=350.;
  second_layer[2]=860.;

  create_keeping_volumes("trd3", 3, first_layer, second_layer, 
			 Inner_radius, Outer_radius, Distance, 
			 Station_thickness, top, air);

   gGeoMan->CloseGeometry();
   TFile* outfile = new TFile("TRD_Geom.root","RECREATE");
   top->Write();
   outfile->Close();
   top->Draw("ogl");

}

void create_keeping_volumes(const char* name, Int_t station, Float_t* first,
			    Float_t* second, Float_t* radius1,
			    Float_t* radius2, Float_t* distance, 
			    Float_t thickness, TGeoVolume* vol, TGeoMedium* med)

{
   Double_t phi1, dphi, nedges, nz, z,rmin, rmax;
   // Shape: trd1 type: TGeoPgon
   phi1    = 45.;
   dphi    = 360.;
   nedges = 4;
   nz      = 4;

   TGeoPgon *trd_pgon = new TGeoPgon(name,phi1,dphi,nedges,nz);
   z     = first[0];
   rmin  = first[1];
   rmax  = first[2];
   trd_pgon->DefineSection(0, z,rmin,rmax);
   z     = second[0];
   rmin  = second[1];
   rmax  = second[2];
   trd_pgon->DefineSection(1, z,rmin,rmax);
   z     = -thickness/2;
   rmin  = radius1[station-1];
   rmax  = radius2[station-1];
   trd_pgon->DefineSection(2, z,rmin,rmax);
   z     = thickness/2;
   rmin  = radius1[station-1];
   rmax  = radius2[station-1];
   trd_pgon->DefineSection(3, z,rmin,rmax);

   TGeoShape *trd_shape = trd_pgon;
   // Volume: trd1
   TGeoVolume* trd = new TGeoVolume(name, trd_shape, med);
   // Translation: 
   Float_t position = distance[station-1] + thickness/2;
   TGeoTranslation *trd_trans = new TGeoTranslation("",0. ,0. , position);
   vol->AddNode(trd, 0, trd_trans);
}
