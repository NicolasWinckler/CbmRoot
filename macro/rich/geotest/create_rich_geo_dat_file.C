/**
 * \file create_rich_geo_dat_file.C
 * \brief Macro .dat file generation with RICH geometry parameters.
 * It helps automatically create many RICH geometries for testing.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2012
 */
#include <fstream>
#include <sstream>
using namespace std;

void create_rich_geo_dat_file()
{
   int i = 0;
   for (double mirrorAngle = -1.; mirrorAngle <= 15; mirrorAngle += 3.){
      for (double pmtY = 1275.; pmtY <= 1500.; pmtY += 50.){
         for (double pmtAngleX = 0.; pmtAngleX <= 10.; pmtAngleX += 2.){
            for (double pmtAngleY = -10.; pmtAngleY <= 0.; pmtAngleY += 2.){
               i++;
               stringstream ss;
               ss << "geodat/rich_geometry";
               ss.fill('0');
               ss.width(4);
               ss << i << ".dat";
               cout << ss.str() << endl;
               ofstream fout(ss.str().c_str());
               //double mirrorAngle = -1.0;
               double pmtZ = 200.;
              // double pmtY = 1275.;
               double pmtDx = 1000.;
               double pmtDy = 300.;
               //double pmtAngleX = 5.;
               //double pmtAngleY = -5.;

               fout << "RICH barrel" << endl;
               fout << "z     length   dWindows   dVessel    lRadiator  radiator" << endl;
               fout <<"1600.     1900.       0.25        5.        1500.    RICHgas_CO2_dis" << endl;
               fout <<"RICH mirror" << endl;
               fout <<"radius  thickness   material   yBeamhole  angle" << endl;
               fout <<"3000.     6.      RICHglass     200.    " << mirrorAngle << endl;
               fout <<"RICH photodetector (PMT)" << endl;
               fout <<"z    y      dx      dy    angle_x  angle_y  dGlass  dCathode  lTube   dSupport" << endl;
               fout << pmtZ << " " << pmtY << " " << pmtDx << " " << pmtDy << " "
                     << pmtAngleX << " " << pmtAngleY << " 0. 0.5  0.  0." << endl;
               fout.close();
            }
         }
      }
   }
}
