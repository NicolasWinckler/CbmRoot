//Generator for CbmTrd Geometry
//Update 20.11.08
//Changes made that geometry is now independent of 
//Trd-positions.
//F. Uhlig

#include <cstring>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#define PI 3.14159265
FILE *geofile;
FILE *infofile;
FILE *parameterfile;
using namespace std;


int  TrdModules1(int, int, float, float, int, float, float*);


//--------------------------------------------------------------------
int TrdModules1(int Station_number, int Layer_number, float frame_width, float Layer_thickness, int Chamber_number, float Position_Station1[][4], float* Detector_size_x) {


  // create box of air with size of 'Detector_size_x[0] x Detector_size_y[0]' and fill
  // this box with the
  // radiator, mylar foil, gas, pad plane which have a size of 'Active_area_x[0] x Active_area_y[0]'  
  // the frames which have a width of 'frame_width' and are at the outer edges of the box


  bool first_time_Module1 = true;
  bool first_time_Module2 = true;
  bool first_time_Module3 = true;

  int Copy_number_Module1 = 1;
  int Copy_number_Module2 = 1;
  int Copy_number_Module3 = 1;
  int copy_number;

  float  Active_area_x[3];
  float  Active_area_y[3];
  
  Active_area_x[0] = Detector_size_x[0] - frame_width;
  Active_area_y[0] = Detector_size_x[0] / 2 - frame_width;
  Active_area_x[1] = Detector_size_x[1] - frame_width;
  Active_area_y[1] = Detector_size_x[1] / 2 - frame_width;
  Active_area_x[2] = Detector_size_x[2] - frame_width;
  Active_area_y[2] = Detector_size_x[2] / 2 - frame_width;
 

  float radiator_thickness = 14.5;
  float radiator_position = -15.5;
  float gas_thickness = 3.;
  float gas_position = 2.;
  float padplane_thickness = 0.015;
  float padplane_position = 5.015;
  float mylar_thickness = 0.75;
  float mylar_position = 5.78;
  float electronics_thickness = 0.035;
  float electronics_position = 6.565;
  float frame_thickness = 18.300;
  float frame_position = -11.7;



  for (int i=0; i< Chamber_number; i++){

    int module_number = (int)Position_Station1[i][2];
    int j = (int)Position_Station1[i][2]-1;

    if ( (first_time_Module1 && ( module_number == 1 )) ||
         (first_time_Module2 && ( module_number == 2 )) ||
         (first_time_Module3 && ( module_number == 3 ))) {

      if (first_time_Module1 && ( module_number == 1 ) ) {
        copy_number = Copy_number_Module1;
      }
      if (first_time_Module2 && ( module_number == 2 ) ) {
        copy_number = Copy_number_Module2;
      }
      if (first_time_Module3 && ( module_number == 3 ) ) {
        copy_number = Copy_number_Module3;
      }
  
      printf("Bin zum ersten mal hier. \n");

      fprintf(geofile,"//*********************************\n");
      fprintf(geofile,"trd%dmod%d#%d\n",Station_number, module_number ,copy_number);
      fprintf(geofile,"trd%dlayer#%d\n",Station_number, Layer_number+1);
      fprintf(geofile,"BOX\n");
      fprintf(geofile,"air\n");
      fprintf(geofile,"%f %f %f\n",Detector_size_x[j] ,-Detector_size_x[j]/2 , -Layer_thickness/2);
      fprintf(geofile,"%f %f %f\n",Detector_size_x[j] ,Detector_size_x[j]/2 , -Layer_thickness/2);
      fprintf(geofile,"%f %f %f\n",-Detector_size_x[j] ,Detector_size_x[j]/2 , -Layer_thickness/2);
      fprintf(geofile,"%f %f %f\n",-Detector_size_x[j] ,-Detector_size_x[j]/2 , -Layer_thickness/2);
      fprintf(geofile,"%f %f %f\n",Detector_size_x[j] ,-Detector_size_x[j]/2 , Layer_thickness/2);
      fprintf(geofile,"%f %f %f\n",Detector_size_x[j] ,Detector_size_x[j]/2 , Layer_thickness/2);
      fprintf(geofile,"%f %f %f\n",-Detector_size_x[j] ,Detector_size_x[j]/2 , Layer_thickness/2);
      fprintf(geofile,"%f %f %f\n",-Detector_size_x[j] ,-Detector_size_x[j]/2 , Layer_thickness/2);
      fprintf(geofile,"%f %f %f\n", Position_Station1[i][0], Position_Station1[i][1], 0.);
      if (Position_Station1[i][3] == 0) {
        //horizontal position
        fprintf(geofile,"1.  0.  0.  0.  1.  0.  0.  0.  1.\n");
      } 
      else {
        // vertical position
        fprintf(geofile,"0.  1.  0.  -1.  0.  0.  0.  0.  1.\n");
      }
      fprintf(geofile,"//*********************************\n");
  
  
      fprintf(geofile,"//*********************************\n");
      fprintf(geofile,"trd%dmod%dradiator\n",Station_number, module_number);
      fprintf(geofile,"trd%dmod%d#%d\n",Station_number, module_number,copy_number);
      fprintf(geofile,"BOX\n");
      fprintf(geofile,"polypropylene\n");
      fprintf(geofile,"%f %f %f\n",Active_area_x[j] ,-Active_area_y[j] , -radiator_thickness);
      fprintf(geofile,"%f %f %f\n",Active_area_x[j] ,Active_area_y[j] , -radiator_thickness);
      fprintf(geofile,"%f %f %f\n",-Active_area_x[j] ,Active_area_y[j] , -radiator_thickness);
      fprintf(geofile,"%f %f %f\n",-Active_area_x[j] ,-Active_area_y[j] , -radiator_thickness);
      fprintf(geofile,"%f %f %f\n",Active_area_x[j] ,-Active_area_y[j] , radiator_thickness);
      fprintf(geofile,"%f %f %f\n",Active_area_x[j] ,Active_area_y[j] , radiator_thickness);
      fprintf(geofile,"%f %f %f\n",-Active_area_x[j] ,Active_area_y[j] , radiator_thickness);
      fprintf(geofile,"%f %f %f\n",-Active_area_x[j] ,-Active_area_y[j] , radiator_thickness);
      fprintf(geofile,"%f %f %f\n",0. ,0. ,radiator_position);
      if (Position_Station1[i][3] == 0) {
        //horizontal position
        fprintf(geofile,"1.  0.  0.  0.  1.  0.  0.  0.  1.\n");
      } 
      else {
        // vertical position
        fprintf(geofile,"0.  1.  0.  -1.  0.  0.  0.  0.  1.\n");
      }
      fprintf(geofile,"//*********************************\n");
  
      fprintf(geofile,"//*********************************\n");
      fprintf(geofile,"trd%dmod%dgas\n",Station_number, module_number);
      fprintf(geofile,"trd%dmod%d#%d\n",Station_number, module_number,copy_number);
      fprintf(geofile,"BOX\n");
      fprintf(geofile,"TRDgas\n");
      fprintf(geofile,"%f %f %f\n",Active_area_x[j] ,-Active_area_y[j] , -gas_thickness);
      fprintf(geofile,"%f %f %f\n",Active_area_x[j] ,Active_area_y[j] , -gas_thickness);
      fprintf(geofile,"%f %f %f\n",-Active_area_x[j] ,Active_area_y[j] , -gas_thickness);
      fprintf(geofile,"%f %f %f\n",-Active_area_x[j] ,-Active_area_y[j] , -gas_thickness);
      fprintf(geofile,"%f %f %f\n",Active_area_x[j] ,-Active_area_y[j] , gas_thickness);
      fprintf(geofile,"%f %f %f\n",Active_area_x[j] ,Active_area_y[j] , gas_thickness);
      fprintf(geofile,"%f %f %f\n",-Active_area_x[j] ,Active_area_y[j] , gas_thickness);
      fprintf(geofile,"%f %f %f\n",-Active_area_x[j] ,-Active_area_y[j] , gas_thickness);
      fprintf(geofile,"%f %f %f\n", 0., 0., gas_position);
      if (Position_Station1[i][3] == 0) {
        //horizontal position
        fprintf(geofile,"1.  0.  0.  0.  1.  0.  0.  0.  1.\n");
      } 
      else {
        // vertical position
        fprintf(geofile,"0.  1.  0.  -1.  0.  0.  0.  0.  1.\n");
      }
      fprintf(geofile,"//*********************************\n");
  
      fprintf(geofile,"//*********************************\n");
      fprintf(geofile,"trd%dmod%dpadplane\n",Station_number, module_number);
      fprintf(geofile,"trd%dmod%d#%d\n",Station_number, module_number,copy_number);
      fprintf(geofile,"BOX\n");
      fprintf(geofile,"goldcoatedcopper\n");
      fprintf(geofile,"%f %f %f\n",Active_area_x[j] ,-Active_area_y[j] ,- padplane_thickness);
      fprintf(geofile,"%f %f %f\n",Active_area_x[j] ,Active_area_y[j] , -padplane_thickness);
      fprintf(geofile,"%f %f %f\n",-Active_area_x[j] ,Active_area_y[j] ,- padplane_thickness);
      fprintf(geofile,"%f %f %f\n",-Active_area_x[j] ,-Active_area_y[j] , -padplane_thickness);
      fprintf(geofile,"%f %f %f\n",Active_area_x[j] ,-Active_area_y[j] , padplane_thickness);
      fprintf(geofile,"%f %f %f\n",Active_area_x[j] ,Active_area_y[j] , padplane_thickness);
      fprintf(geofile,"%f %f %f\n",-Active_area_x[j] ,Active_area_y[j] , padplane_thickness);
      fprintf(geofile,"%f %f %f\n",-Active_area_x[j] ,-Active_area_y[j] , padplane_thickness);
      fprintf(geofile,"%f %f %f\n", 0., 0., padplane_position);
      if (Position_Station1[i][3] == 0) {
        //horizontal position
        fprintf(geofile,"1.  0.  0.  0.  1.  0.  0.  0.  1.\n");
      } 
      else {
        // vertical position
        fprintf(geofile,"0.  1.  0.  -1.  0.  0.  0.  0.  1.\n");
      }
      fprintf(geofile,"//*********************************\n");
  
      fprintf(geofile,"//*********************************\n");
      fprintf(geofile,"trd%dmod%dmylar\n",Station_number, module_number);
      fprintf(geofile,"trd%dmod%d#%d\n",Station_number, module_number,copy_number);
      fprintf(geofile,"BOX\n");
      fprintf(geofile,"mylar\n");
      fprintf(geofile,"%f %f %f\n",Active_area_x[j] ,-Active_area_y[j] , -mylar_thickness);
      fprintf(geofile,"%f %f %f\n",Active_area_x[j] ,Active_area_y[j] , -mylar_thickness);
      fprintf(geofile,"%f %f %f\n",-Active_area_x[j] ,Active_area_y[j] , -mylar_thickness);
      fprintf(geofile,"%f %f %f\n",-Active_area_x[j] ,-Active_area_y[j] , -mylar_thickness);
      fprintf(geofile,"%f %f %f\n",Active_area_x[j] ,-Active_area_y[j] , mylar_thickness);
      fprintf(geofile,"%f %f %f\n",Active_area_x[j] ,Active_area_y[j] , mylar_thickness);
      fprintf(geofile,"%f %f %f\n",-Active_area_x[j] ,Active_area_y[j] , mylar_thickness);
      fprintf(geofile,"%f %f %f\n",-Active_area_x[j] ,-Active_area_y[j] , mylar_thickness);
      fprintf(geofile,"%f %f %f\n", 0., 0., mylar_position);
      if (Position_Station1[i][3] == 0) {
        //horizontal position
        fprintf(geofile,"1.  0.  0.  0.  1.  0.  0.  0.  1.\n");
      } 
      else {
        // vertical position
        fprintf(geofile,"0.  1.  0.  -1.  0.  0.  0.  0.  1.\n");
      }
      fprintf(geofile,"//*********************************\n");
  
      fprintf(geofile,"//*********************************\n");
      fprintf(geofile,"trd%dmod%delectronics\n",Station_number, module_number);
      fprintf(geofile,"trd%dmod%d#%d\n",Station_number, module_number,copy_number);
      fprintf(geofile,"BOX\n");
      fprintf(geofile,"goldcoatedcopper\n");
      fprintf(geofile,"%f %f %f\n",Active_area_x[j] ,-Active_area_y[j] , -electronics_thickness);
      fprintf(geofile,"%f %f %f\n",Active_area_x[j] ,Active_area_y[j] , -electronics_thickness);
      fprintf(geofile,"%f %f %f\n",-Active_area_x[j] ,Active_area_y[j] , -electronics_thickness);
      fprintf(geofile,"%f %f %f\n",-Active_area_x[j] ,-Active_area_y[j] , -electronics_thickness);
      fprintf(geofile,"%f %f %f\n",Active_area_x[j] ,-Active_area_y[j] , electronics_thickness);
      fprintf(geofile,"%f %f %f\n",Active_area_x[j] ,Active_area_y[j] , electronics_thickness);
      fprintf(geofile,"%f %f %f\n",-Active_area_x[j] ,Active_area_y[j] , electronics_thickness);
      fprintf(geofile,"%f %f %f\n",-Active_area_x[j] ,-Active_area_y[j] , electronics_thickness);
      fprintf(geofile,"%f %f %f\n", 0., 0., electronics_position);
      if (Position_Station1[i][3] == 0) {
        //horizontal position
        fprintf(geofile,"1.  0.  0.  0.  1.  0.  0.  0.  1.\n");
      } 
      else {
        // vertical position
        fprintf(geofile,"0.  1.  0.  -1.  0.  0.  0.  0.  1.\n");
      }
      fprintf(geofile,"//*********************************\n");
  
      if ( frame_width > 0.) {
        fprintf(geofile,"//*********************************\n");
        fprintf(geofile,"trd%dmod%dcarbon1#1\n",Station_number, module_number);
        fprintf(geofile,"trd%dmod%d#%d\n",Station_number, module_number,copy_number);
        fprintf(geofile,"BOX\n");
        fprintf(geofile,"carbon\n");
        fprintf(geofile,"%f %f %f\n",Detector_size_x[j] ,-frame_width/2 , -frame_thickness);
        fprintf(geofile,"%f %f %f\n",Detector_size_x[j] ,frame_width/2 , -frame_thickness);
        fprintf(geofile,"%f %f %f\n",-Detector_size_x[j] ,frame_width/2 , -frame_thickness);
        fprintf(geofile,"%f %f %f\n",-Detector_size_x[j] ,-frame_width/2 , -frame_thickness);
        fprintf(geofile,"%f %f %f\n",Detector_size_x[j] ,-frame_width/2 , frame_thickness);
        fprintf(geofile,"%f %f %f\n",Detector_size_x[j] ,frame_width/2 , frame_thickness);
        fprintf(geofile,"%f %f %f\n",-Detector_size_x[j] ,frame_width/2 , frame_thickness);
        fprintf(geofile,"%f %f %f\n",-Detector_size_x[j] ,-frame_width/2 , frame_thickness);
        fprintf(geofile,"%f %f %f\n", 0., (Active_area_y[j] + frame_width / 2 ), frame_position);
        if (Position_Station1[i][3] == 0) {
          //horizontal position
          fprintf(geofile,"1.  0.  0.  0.  1.  0.  0.  0.  1.\n");
        } 
        else {
          // vertical position
          fprintf(geofile,"0.  1.  0.  -1.  0.  0.  0.  0.  1.\n");
        }

        fprintf(geofile,"//*********************************\n");
  
        fprintf(geofile,"//*********************************\n");
        fprintf(geofile,"trd%dmod%dcarbon1#2\n",Station_number, module_number);
        fprintf(geofile,"trd%dmod%d#%d\n",Station_number, module_number,copy_number);
        fprintf(geofile,"%f %f %f\n", 0., -(Active_area_y[j] + frame_width / 2 ), frame_position);
        if (Position_Station1[i][3] == 0) {
          //horizontal position
          fprintf(geofile,"1.  0.  0.  0.  1.  0.  0.  0.  1.\n");
        } 
        else {
          // vertical position
          fprintf(geofile,"0.  1.  0.  -1.  0.  0.  0.  0.  1.\n");
        }
        fprintf(geofile,"//*********************************\n");
  
  
        fprintf(geofile,"//*********************************\n");
        fprintf(geofile,"trd%dmod%dcarbon2#1\n",Station_number, module_number);
        fprintf(geofile,"trd%dmod%d#%d\n",Station_number, module_number,copy_number);
        fprintf(geofile,"BOX\n");
        fprintf(geofile,"carbon\n");
        fprintf(geofile,"%f %f %f\n",  frame_width/2, -Active_area_y[j], -frame_thickness);
        fprintf(geofile,"%f %f %f\n",  frame_width/2,  Active_area_y[j], -frame_thickness);
        fprintf(geofile,"%f %f %f\n", -frame_width/2,  Active_area_y[j], -frame_thickness);
        fprintf(geofile,"%f %f %f\n", -frame_width/2, -Active_area_y[j], -frame_thickness);
        fprintf(geofile,"%f %f %f\n",  frame_width/2, -Active_area_y[j],  frame_thickness);
        fprintf(geofile,"%f %f %f\n",  frame_width/2,  Active_area_y[j],  frame_thickness);
        fprintf(geofile,"%f %f %f\n", -frame_width/2,  Active_area_y[j],  frame_thickness);
        fprintf(geofile,"%f %f %f\n", -frame_width/2, -Active_area_y[j],  frame_thickness);
        fprintf(geofile,"%f %f %f\n", (Active_area_x[j] + frame_width / 2 ), 0., frame_position);
        if (Position_Station1[i][3] == 0) {
          //horizontal position
          fprintf(geofile,"1.  0.  0.  0.  1.  0.  0.  0.  1.\n");
        } 
        else {
          // vertical position
          fprintf(geofile,"0.  1.  0.  -1.  0.  0.  0.  0.  1.\n");
        }
        fprintf(geofile,"//*********************************\n");
  
        fprintf(geofile,"//*********************************\n");
        fprintf(geofile,"trd%dmod%dcarbon2#2\n",Station_number, module_number);
        fprintf(geofile,"trd%dmod%d#%d\n",Station_number, module_number,copy_number);
        fprintf(geofile,"%f %f %f\n", -(Active_area_x[j] + frame_width / 2 ), 0., frame_position);
        if (Position_Station1[i][3] == 0) {
          //horizontal position
          fprintf(geofile,"1.  0.  0.  0.  1.  0.  0.  0.  1.\n");
        } 
        else {
          // vertical position
          fprintf(geofile,"0.  1.  0.  -1.  0.  0.  0.  0.  1.\n");
        }

        fprintf(geofile,"//*********************************\n");
      }

      if (first_time_Module1 && ( module_number == 1 )) {
        first_time_Module1=false;
        Copy_number_Module1 = copy_number+1 ;
      }
      if (first_time_Module2 && ( module_number == 2 ) ) {
        first_time_Module2=false;
        Copy_number_Module2 = copy_number+1 ;
      }
      if (first_time_Module3 && ( module_number == 3 ) ) {
        first_time_Module3=false;
        Copy_number_Module3 = copy_number+1 ;
      }

    }
    else {

      if ( module_number == 1 ) {
        copy_number = Copy_number_Module1;
      } 
      else if ( module_number == 2 ) {
        copy_number = Copy_number_Module2;
      } 
      else if ( module_number == 3 ) {
        copy_number = Copy_number_Module3;
      } 
      else {
        printf("There is something wrong. We don't have more than three different detector sizes\n");
        printf("Break in TrdModules1.\n");
        return 1;
      }


      fprintf(geofile,"//*********************************\n");
      fprintf(geofile,"trd%dmod%d#%d\n",Station_number, module_number,copy_number);
      fprintf(geofile,"trd%dlayer#%d\n",Station_number, Layer_number+1);
      fprintf(geofile,"%f %f %f\n", Position_Station1[i][0], Position_Station1[i][1], 0.);
      if (Position_Station1[i][3] == 0) {
        //horizontal position
        fprintf(geofile,"1.  0.  0.  0.  1.  0.  0.  0.  1.\n");
      } 
      else {
        // vertical position
        fprintf(geofile,"0.  1.  0.  -1.  0.  0.  0.  0.  1.\n");
      }
      fprintf(geofile,"//*********************************\n");
  
      if ( module_number == 1 ) {
        Copy_number_Module1 = copy_number+1 ;
      }
      if ( module_number == 2 ) {
        Copy_number_Module2 = copy_number+1 ;
      }
      if ( module_number == 3 ) {
        Copy_number_Module3 = copy_number+1 ;
      }
  
    }

  }
  
  return 0;

}




int main(void)
{
  const char* c_str();

 //-----------Files------------------------------------------------
  string path = "./";
  string geoname  = path + "trd_segmented.geo" ;
  string infoname = path + "trd_segmented.info";
  string parametername  = path + "trd_segmented.txt";

  geofile = fopen(geoname.c_str(),  "w+");
  infofile = fopen(infoname.c_str(), "w+");
  parameterfile = fopen(parametername.c_str(),  "w+");


  //---------- Declaration of parameters ---------------------------------------
  float Distance[3];         // Distance between target an the front of the TRD stations
  float Station_thickness;   // Thickness of one TRD station
  float Layer_thickness;     // Thickness of one layer of a TRD station
  int   Layer_number;        // Number of detector layers per station
  int   Station_number;      // Number of TRD stations in the setup
  float Detector_size_x[3];  // length in mm of a detector module in x-direction       
  float Detector_size_y[3];  // length in mm of a detector module in y-direction       
  float Frame_width;         // Width of detector frames in mm
  float Inner_radius[3];     // Inner acceptance in mm
  float Outer_radius[3];     // Outer acceptance in mm

  //--------- Initialization of parameters -------------------------------------  
//  Distance[0]          = 4000;
//  Distance[1]          = 6750;
//  Distance[2]          = 9500;
   Distance[0]          = 5000;
   Distance[1]          = 7250;
   Distance[2]          = 9500;
  Station_number       = 3;
  Layer_number         = 4;
  Layer_thickness      = 60;
  Station_thickness    = Layer_number * Layer_thickness;
  float Inner_acceptance[3]  = {50, 50, 50};
  float Outer_acceptance[3]  = {500, 500, 500};
//  Frame_width          = 20;
  Frame_width          = 0;

  //--------- Basic calculations ----------------------------------------------
  // Here the size of the detector is calculated on the basis of the acceptance
  // which ranges from 50 to 500 mrad.
  // There is also a check if the detector station overlaps with the beampipe
  // Some basic information of the setup are written to the geofile 

  fprintf(geofile,"//*********************************\n");
  fprintf(geofile,"// This is the geometry file of the following configuration\n");
  fprintf(geofile,"// Number of stations: %d\n",Station_number);
  fprintf(geofile,"// Number of layers per station: %d\n",Layer_number);
  fprintf(geofile,"//*******************************************\n");
  

  for(int i = 0; i < Station_number; i++) {
    float bla=Inner_acceptance[i]/1000.;
    float bla1=Outer_acceptance[i]/1000.;

    // F.U. 20.02.07
    // Outer radius increased by factor 1.5 to keep all modules in th
    // keeping volume since the size in y-direction
    // was increased by a factor of 1.5
    // There is also a problem with the inner radius of station 2 which is
    // to large at the standard distance from the target. this has to be lower
    // by 12 cm
    Inner_radius[i] = Distance[i]*tan(bla);
    if ( i== 1 ) {
      Inner_radius[i] = 250.;
    }

   //Outer_radius[i] = Distance[i]*tan(bla1);
    Outer_radius[i] = Distance[i]*tan(bla1)*1.6;

    float radius_beampipe = 200.5; // constant radius after RICH
    if (Inner_radius[i] <= radius_beampipe) {
      printf("radius beampipe: %f\n",radius_beampipe);
      printf("inner radius trd: %f\n",Inner_radius[i]);
      Inner_radius[i]=radius_beampipe + 5;
      printf("Inner radius of trd station %d was inside the beam pipe. Increased radius to %f\n",
              i+1, Inner_radius[i]);
    }
    printf("inner radius trd: %f\n",Inner_radius[i]);
    printf("outer radius trd: %f\n",Outer_radius[i]);

    fprintf(geofile,"// Inner Radius of station %d: %f\n",i+1,Inner_radius[i]);
    fprintf(geofile,"// Outer Radius of station %d: %f\n",i+1,Outer_radius[i]);
    fprintf(geofile,"// z-position at the middle of the station %d: %f\n",i+1,
            Distance[i]+(Station_thickness/2));
    fprintf(geofile,"//*******************************************\n");
 
  }

  // Calculate the sizes of all three detector modules. All sizes depend on the radius
  // of the inner acceptance of the first station.
  // At least this is correct is the stations are at 5000, 7250 and 9500 mm. If this is
  // still okay when the stations are shifted has to be checked.

  Detector_size_x[0] = roundf(Inner_radius[0]);
  if ((int)Detector_size_x[0]%2) Detector_size_x[0]++;

  // Calculate size of the complete detector with frames in x-direction which is the double
  // size of the inner acceptance for geometrical reasons. The y-direction is twice as much
  // as the length in x-direction

  Detector_size_x[0] *= 2;
  Detector_size_y[0] = Detector_size_x[0] * 2;


  // Calculate the sizes of the lager detector modules according to the size of the smallest
  // modules. The calculations follow the geometrical layout of the stations.

  Detector_size_y[1] = Detector_size_y[0] * 3 / 2 ;
  Detector_size_x[1] = Detector_size_x[0] * 3 / 2 ;

  Detector_size_y[2] = Detector_size_y[0] * 2 ;
  Detector_size_x[2] = Detector_size_x[0] * 2 ;



  
  // calculate positions of small chambers in the first station
  // This is done according the geometrical layout of this station

  float x1 = Detector_size_x[0];
  float y1 = 2 * Detector_size_x[0] + Detector_size_x[1] + Detector_size_x[2]; 
  float x2 = Detector_size_x[0] / 2;
  float y2 = Detector_size_x[0];
  float x3 = Detector_size_x[1];
  float y3 = Detector_size_x[1] * 3 / 2;
  float x4 = Detector_size_x[2] / 2;
  float y4 = Detector_size_x[2] * 2;
  float x5 = Detector_size_x[2] * 3 / 2;
  float y5 = Detector_size_x[2] * 2;

  float x6a = Detector_size_x[2] * 7 /2;
  float y6a = Detector_size_x[2] * 2;

  float x7a = Detector_size_x[2] * 7 /2;
  float y7a = Detector_size_x[2];

  float x8a = Detector_size_x[2] * 7 /2;
  float y8a = 0;

  cout << x1 <<" , " << y1 <<" , " << x2 <<" , " << y2 <<" , "  << x3 <<" , " << y3 << endl;
  cout  << x4 <<" , " << y4 <<" , " << x5 <<" , " << y5 << endl;
  cout  << x6a <<" , " << y6a <<" , " << x7a <<" , " << y7a <<" , " << x8a <<" , " << y8a <<endl;


  // This array stores all information about the geometrical layout of the first TRD
  // station. First row is x-position, second row is y-position, third row is the
  // chamber type and the last row gives information about the orientation of the
  // chamber


  int Chamber_number_Station1 = 28;

  float Position_Station1[][4] = { {-x2,y2,1,0},{x2,-y2,1,0},{y2,x2,1,1}, {-y2,-x2,1,1},{-x3,y3,2,0},{x3,y3,2,0},
                                     {-x3,-y3,2,0},{x3,-y3,2,0},{-y3,0,2,1},{y3,0,2,1},{-x4,y4,3,0},{x5,y4,3,0},{-x5,-y5,3,0},
                                     {x4,-y5,3,0},{-y5,x5,3,1},{-y4,-x4,3,1},{y4,x4,3,1},{y5,-x5,3,1},{x6a,y6a,3,0},{x7a,y7a,3,0},{x8a,y8a,3,0},{x7a,-y7a,3,0},{x6a,-y6a,3,0},{-x6a,y6a,3,0},{-x7a,y7a,3,0},{-x8a,y8a,3,0},{-x7a,-y7a,3,0},{-x6a,-y6a,3,0}};


  //  float Offset_size=250;
  float Offset_size=Detector_size_x[0]/2;

  float x6 = Offset_size + Detector_size_x[0] +  Detector_size_x[1] + 1.5 * Detector_size_x[2];
  float y6 = Offset_size + Detector_size_x[0] / 2;
  float x7 = Offset_size + Detector_size_x[0] +  Detector_size_x[1] + 1.5 * Detector_size_x[2];
  float y7 = Offset_size + Detector_size_x[0] / 2 + Detector_size_x[2];
  float x8 = Offset_size + Detector_size_x[0] +  Detector_size_x[1] + 1.5 * Detector_size_x[2];
  float y8 = Offset_size + Detector_size_x[0] / 2 + 2 * Detector_size_x[2];
  float x9 = Detector_size_x[1];
  float y9 = Offset_size + Detector_size_x[0] +  1.5 * Detector_size_x[1] + 2 * Detector_size_x[2];
  float x10a =  Offset_size + Detector_size_x[0] + Detector_size_x[1] + 3 * Detector_size_x[2];
  float y10a = 0.;
  float y10b = Detector_size_x[2];
  float y10c = 2 * Detector_size_x[2];
  float y10d = 3 * Detector_size_x[2];
  float x11a = Offset_size + Detector_size_x[0] + Detector_size_x[1] + 4.5 * Detector_size_x[2];
  float y11a = 0;
  float y11b = 2 * Detector_size_x[2];
  

  int Chamber_number_Station2 = 50;

  float Position_Station2[][4] = { {-x2,y2,1,0},{x2,-y2,1,0},{y2,x2,1,1},{-y2,-x2,1,1},{-x3,y3,2,0},{x3,y3,2,0},{-x3,-y3,2,0},{x3,-y3,2,0},
      	      			     {-y3,0,2,1},{y3,0,2,1},{-x4,y4,3,0},{x5,y4,3,0},{-x5,-y5,3,0},{x4,-y5,3,0},{-y5,x5,3,1},{-y4,-x4,3,1},
				     {y4,x4,3,1},{y5,-x5,3,1},{-y8,-x8,3,0},{-y6,-x6,3,0},{y7,-x7,3,0},{-y7,x7,3,0},{y6,x6,3,0},{y8,x8,3,0},
				     {-x8,y8,3,1},{-x6,y6,3,1},{-x7,-y7,3,1},{x7,y7,3,1},{x6,-y6,3,1},{x8,-y8,3,1},
{-x10a,y10d,3,0},{-x10a,y10c,3,0},{-x10a,y10b,3,0},{-x10a,y10a,3,0},{-x10a,-y10b,3,0},{-x10a,-y10c,3,0},{-x10a,-y10d,3,0},{-x11a,y11b,3,1},{-x11a,y11a,3,1},{-x11a,-y11b,3,1},{x10a,y10d,3,0},{x10a,y10c,3,0},{x10a,y10b,3,0},{x10a,y10a,3,0},{x10a,-y10b,3,0},{x10a,-y10c,3,0},{x10a,-y10d,3,0},{x11a,y11b,3,1},{x11a,y11a,3,1},{x11a,-y11b,3,1} };

  float x10 = 1.5 * Detector_size_x[2]; //1500 
  float y10 = 2 *  Detector_size_x[2];  //2000
  float x11 = 1.5 * Detector_size_x[2]; //1500
  float y11 = Detector_size_x[2];       //1000
  float x12 = 1.5 * Detector_size_x[2]; //1500
  float y12 = 0;                        //0
  float x13 = 2.5 * Detector_size_x[2]; //2500
  float y13 = 4 * Detector_size_x[2];   //4000
  float x14 = 2.5 * Detector_size_x[2]; //2500
  float y14 = 3 * Detector_size_x[2];   //3000
  float x15 = 1.5 * Detector_size_x[2]; //1500
  float y15 = 3 * Detector_size_x[2];   //3000
  float x16 = 0.5 * Detector_size_x[2]; //500
  float y16 = 3 * Detector_size_x[2];   //3000
  float x17 = 4.5 * Detector_size_x[2]; //4500
  float y17 = Detector_size_x[2];       //1000
  float x18 = 4.5 * Detector_size_x[2]; //4500
  float y18a = 4. * Detector_size_x[2];
  float y18b = 3. * Detector_size_x[2];
  float y18c = 2. * Detector_size_x[2]; 
  float y18d = 1. * Detector_size_x[2];
  float y18e = 0.;
  float x19 = 0;                        //0
  float y19 = 1.5 * Detector_size_x[2]; //1500
  float x20 = 6.5 * Detector_size_x[2];
  float x21 = 8. * Detector_size_x[2];
  

  int Chamber_number_Station3 = 72;

   
  float Position_Station3[][4] = { {-x10,y10,3,0},{-x11,y11,3,0},{-x12,y12,3,0},{-x11,-y11,3,0},{-x10,-y10,3,0},{x10,y10,3,0},
                                   {x11,y11,3,0},{x12,y12,3,0},{x11,-y11,3,0},{x10,-y10,3,0},{-x13,y13,3,0},{x13,y13,3,0},
                                   {-x13,-y13,3,0},{x13,-y13,3,0},{-x14,-y14,3,0},{-x16,-y15,3,0},{x15,-y15,3,0},{-x15,y15,3,0},
                                   {x16,y16,3,0},{x14,y14,3,0},{y17,x17,3,1},{y18e,x17,3,1},{-y17,x17,3,1},{y17,-x17,3,1},
                                   {y18e,-x17,3,1},{-y17,-x17,3,1},{y15,x15,3,1},{y16,-x16,3,1},{y14,-x14,3,1},{-y14,x14,3,1},
                                   {-y16,x16,3,1},{-y15,-x15,3,1},
				   {x19,-y19,3,1},{x19,y19,3,1},{-x18,y18a,3,0},{-x18,y18b,3,0},{-x18,y18c,3,0},{-x18,y18d,3,0},{-x18,y18e,3,0},{-x18,-y18d,3,0},{-x18,-y18c,3,0},{-x18,-y18b,3,0},{-x18,-y18a,3,0},{x18,y18a,3,0},{x18,y18b,3,0},{x18,y18c,3,0},{x18,y18d,3,0},{x18,y18e,3,0},{x18,-y18d,3,0},{x18,-y18c,3,0},{x18,-y18b,3,0},{x18,-y18a,3,0},{-x20,y18b,3,0},{-x20,y18c,3,0},{-x20,y18d,3,0},{-x20,y18e,3,0},{-x20,-y18d,3,0},{-x20,-y18c,3,0},{-x20,-y18b,3,0},{x20,y18b,3,0},{x20,y18c,3,0},{x20,y18d,3,0},{x20,y18e,3,0},{x20,-y18d,3,0},{x20,-y18c,3,0},{x20,-y18b,3,0},{x21,y18c,3,1},{x21,y18e,3,1},{x21,-y18c,3,1},{-x21,y18c,3,1},{-x21,y18e,3,1},{-x21,-y18c,3,1} };
   

  for ( int counter = 0; counter < Station_number; counter++) {

    printf("Distance, Inner_radius, Outer_radius, Station_thickness: %f , %f, %f, %f \n",Distance[counter], Inner_radius[counter], Outer_radius[counter], Station_thickness);
    printf("Layer_number, Layer_thickness, frame_width %d , %f, %f \n",Layer_number, Layer_thickness, Frame_width);
    printf("Detector_size_x %d: %f\n",counter,Detector_size_x[counter]);
  
    // The distance is measured to the start of the station. In GEANT the
    // volumes are defined from the center of the volume with half thickness
    // to either side. This has to corrected here
  
    float Distance_center = Distance[counter] + Station_thickness/2;
  
    fprintf(geofile,"//*********************************\n");
    fprintf(geofile,"trd%d\n",counter+1);
    fprintf(geofile,"cave\n");
    fprintf(geofile,"PGON\n");
    fprintf(geofile,"air\n");
    fprintf(geofile,"2\n");
    fprintf(geofile,"45 360 4\n");
    fprintf(geofile,"-%f %f %f \n",Station_thickness/2,(Inner_radius[counter]-1),(Outer_radius[counter]+1010));
    fprintf(geofile,"%f %f %f \n",Station_thickness/2,(Inner_radius[counter]-1),(Outer_radius[counter]+1010));
    fprintf(geofile,"0. 0. %4.1f\n",Distance_center);
    fprintf(geofile,"1.  0.  0.  0.  1.  0.  0.  0.  1.\n");
    fprintf(geofile,"//*********************************\n");
      
    for (int i = 0; i < Layer_number; i++){
      if (i == 0){
        fprintf(geofile,"//*********************************\n");
        fprintf(geofile,"trd%dlayer#%d\n",counter+1, i+1);
        fprintf(geofile,"trd%d\n",counter+1);
        fprintf(geofile,"PGON\n");
        fprintf(geofile,"air\n");
        fprintf(geofile,"2\n");
        fprintf(geofile,"45 360 4\n");
        fprintf(geofile,"-%f %f %f \n",Layer_thickness/2,(Inner_radius[counter]-0.5),(Outer_radius[counter]+500));
        fprintf(geofile,"%f %f %f \n",Layer_thickness/2,(Inner_radius[counter]-0.5),(Outer_radius[counter]+500));
        fprintf(geofile,"0. 0. %4.1f\n",(-Station_thickness/2)+30+(60*i));
        fprintf(geofile,"1.  0.  0.  0.  1.  0.  0.  0.  1.\n");
        fprintf(geofile,"//*********************************\n");
  
        
        if ( counter == 0 ) {
	  if ((TrdModules1(counter+1, i , Frame_width, Layer_thickness, Chamber_number_Station1, Position_Station1, Detector_size_x ))) {
            printf("There was an error in TrdModules1\n");
            printf("Stop program\n");            
            return 1;
          }
        }
        else if ( counter == 1 ) {
	  if ((TrdModules1(counter+1, i , Frame_width, Layer_thickness, Chamber_number_Station2, Position_Station2, Detector_size_x ))) {
            printf("There was an error in TrdModules1\n");
            printf("Stop program\n");            
            return 1;
          }
	}
        else if ( counter == 2 ) {
	  if ((TrdModules1(counter+1, i , Frame_width, Layer_thickness, Chamber_number_Station3, Position_Station3, Detector_size_x ))) {
            printf("There was an error in TrdModules1\n");
            printf("Stop program\n");            
            return 1;
          }
	}
	  
	  
      }
      else {
        // copy only the first layer and shift to the right positions
        fprintf(geofile,"//*********************************\n");
        fprintf(geofile,"trd%dlayer#%d\n",counter+1, i+1);
        fprintf(geofile,"trd%d\n",counter+1);
        fprintf(geofile,"0. 0. %4.1f\n",(-Station_thickness/2)+30+(60*i));
        fprintf(geofile,"1.  0.  0.  0.  1.  0.  0.  0.  1.\n");
        fprintf(geofile,"//*********************************\n");
      }
  
    }
  }


  fclose(geofile);
  fclose(infofile);
  fclose(parameterfile);

}

