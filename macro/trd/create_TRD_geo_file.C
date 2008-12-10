void create_TRD_geo_file(char *geometry="trd_new")
{

  char infile[256];       // geometry data file - input
  char outfile[256];      // geometry data file for CBMROOT - output
  char logfile[256];      // log file for the conversion
  const Int_t max = 20;   // max. number of stations
  Float_t d[max];         // detector thickness [mm]
  Float_t z[max];         // z position from target [mm]
  Float_t ai[max];        // inner acceptance [deg]
  Float_t ao[max];        // outer acceptance [deg]
  Float_t ri[max];        // inner radius [mm]
  Float_t ro[max];        // outer radius [mm]
  Int_t num_station;      // number of stations
  Int_t num_layer;        // number of layers per station stations
  Int_t thick;            // number defining thickness of det 1=thick,0=thin
  Int_t nn;
  char dummy[120];         //dummy text

  // The following arrays always stores the following information
  // first element:  half thickness of volume detector
  // second element: position of volume inside mother volume

  Float_t radiator_thin[2]={14.5,-15.5};
  Float_t radiator_thick[2]={15.0,-15.0}; 
  Float_t* radiator;
  Float_t gas_thin[2]={3,2};
  Float_t gas_thick[2]={3,3};
  Float_t* gas;
  Float_t pad_thin[2]={0.015,5.015};
  Float_t pad_thick[2]={0.025,6.025};
  Float_t* pad;
  Float_t mylar_thin[2]={0.75,5.78};
  Float_t mylar_thick[2]={1.0,7.5};
  Float_t* mylar;
  Float_t electronics_thin[2]={0.035,6.565};
  Float_t electronics_thick[2]={0.05,8.55};
  Float_t* electronics;
 

  printf("geometry: %s\n",geometry);

  sprintf(infile,"%s.dat",geometry);
  sprintf(outfile,"%s.geo",geometry);
  sprintf(logfile,"%s.log",geometry);

  printf("input file: %s\n",infile);
  if(!fopen(infile,"r")){
    printf("... does not exist.\n");
    printf("... exit.\n\n");
  }
  else{

    printf("output file: %s\n",outfile);
    printf("log file:    %s\n",logfile);

    printf("converting ...\n");

    FILE *fin= fopen(infile,"r");
    fscanf (fin, "%s",dummy);
    // read number of stations
    fscanf (fin, "%d", &num_station);
    // read number of layers per stationstations
    fscanf (fin, "%d", &num_layer);   
    for (Int_t i=0; i<5;i++){
      fscanf (fin, "%s",dummy);
    }
    // read numbe defining thickness of detector
    fscanf (fin, "%d", &thick); 
    for (Int_t i=0; i<10;i++){
      fscanf (fin, "%s",dummy);
    }
  

    for(Int_t i=0; i<num_station; i++){
      fscanf (fin, "%d %f %f %f %f", &nn, &d[i],&z[i],
                                        &ai[i],&ao[i]);
      //  printf("%d %f %f %f %f\n", nn, d[i],z[i],
      //                                  ai[i],ao[i]);
    }
    fclose(fin);


    if (thick==1) then {
      radiator=radiator_thick;
      gas=gas_thick;
      pad=pad_thick;
      mylar=mylar_thick;
      electronics=&electronics_thick;
      printf("setup for thick detector is calculated.\n");
    }
    else {
      radiator=radiator_thin;
      gas=gas_thin;
      pad=pad_thin;
      mylar=mylar_thin;
      electronics=&electronics_thin;
      printf("setup for thin detector is calculated.\n");
    }

    // calculate inner and outer radius of the detector
    // shift z position by half thickness, because the origin
    // of the subvolume is in the middle of the volume
    for(Int_t i=0; i<num_station; i++) {    
      Float_t bla=ai[i]/1000.;
      Float_t bla1=ao[i]/1000.;
      ri[i] = z[i]*TMath::Tan(bla); 
      ro[i] = z[i]*TMath::Tan(bla1); 
      // up to here z[i] is the front of the trd, from here on z[i]
      // is the middle of the volume
      z[i]=z[i]+d[i]/2;

     
      /* changed 12.06.06 FU due to change of pipe geometry

      // beampipe radius increase from z=3700 mm to z=6000 mm from
      // 185.5 to 300.5 mm. This has to be taken into account that
      // the trd is not inside the beampie
      if ((z[i]+(d[i]/2))<6000) then {
        // calculate the outer radius of the beampipe at the end
        // of the trd station and add 5mm for safety reasons as
        // inner radius of the trd.
        Int_t radius_beampipe = ((z[i]+(d[i]/2)-3700)*TMath::Tan(0.05))+185.5;
	if (ri[i] <= radius_beampipe) then {
          printf("radius beampipe: %f\n",radius_beampipe);
          printf("inner radius trd: %f\n",ri[i]);
          ri[i]=radius_beampipe + 5;
	  printf("Inner radius of trd station %d was inside the beam pipe. Increased radius to %f\n", i+1, ri[i]);
	}
      } 
      else {
        if (ri[i] <= 300.5) then {
          printf("inner radius trd: %f\n",ri[i]);
          ri[i]=305.5;
	  printf("Inner radius of trd station %d was inside the beam pipe. Increased radius to %f\n", i+1, ri[i]);
        }
      }

      */

      Int_t radius_beampipe = 200.5; // constant radius after RICH
      if (ri[i] <= radius_beampipe) then {
        printf("radius beampipe: %f\n",radius_beampipe);
        printf("inner radius trd: %f\n",ri[i]);
        ri[i]=radius_beampipe + 5;
	printf("Inner radius of trd station %d was inside the beam pipe. Increased radius to %f\n", i+1, ri[i]);
      }
      //ri[i]=TMath::Sqrt(ri[i]*ri[i]+ri[i]*ri[i]);
      printf("inner radius trd: %f\n",ri[i]);
      //ro[i]=TMath::Sqrt(ro[i]*ro[i]+ro[i]*ro[i]);
      printf("outer radius trd: %f\n",ro[i]);
    }

    // print info to screen and log file
    FILE *flog= fopen(logfile,"w");

    printf("detector layout:\n");
    for(Int_t i=0; i<num_station; i++) {
      printf("station: %2d  z=%6.1f  ri=%5.1f  ro=%5.1f  d=%3.1f\n",
	     i+1,z[i],ri[i],ro[i],d[i]);  
    }

    fprintf(flog,"input file:  %s\n",infile);
    fprintf(flog,"output file: %s\n",outfile);
    fprintf(flog,"detector layout:\n");
    for(Int_t i=0; i<num_station; i++) {
      fprintf(flog,"station: %2d  z=%6.1f  ri=%5.1f  ro=%5.1f  d=%3.1f\n",
	     i+1,z[i],ri[i],ro[i],d[i]);  
    }
    fclose(flog);

    // Up to here we have the main volumes for the TRD stations. Now
    // calculate the real volumes inside and place them correctly
    // also do the output into the .geo file

    // output to geo file
    FILE *fout= fopen(outfile,"w");
    fprintf(fout,"//*********************************\n");
    fprintf(fout,"// This is the geometry file of the following configuration\n");
    fprintf(fout,"// Number of stations: %d\n",num_station);
    fprintf(fout,"// Number of layers per station: %d\n",num_layer);
    if (thick==1) {
      fprintf(fout,"// Thick detector layout\n");
    } else {
      fprintf(fout,"// Thin detector layout\n");
    }
    fprintf(fout,"//                               \n");
    for(Int_t i=0; i<num_station; i++) {
      fprintf(fout,"// Inner Radius of station %d: %f\n",i+1,ri[i]);
      fprintf(fout,"// Outer Radius of station %d: %f\n",i+1,ro[i]);
      fprintf(fout,"// z-position of station %d: %f\n",i+1,z[i]-d[i]/2);
      fprintf(fout,"//                               \n");
    }
    for(Int_t i=0; i<num_station; i++) {
      fprintf(fout,"//*********************************\n");
      fprintf(fout,"trd%d\n",i+1);      
      fprintf(fout,"cave\n");      
      fprintf(fout,"PGON\n");
      fprintf(fout,"air\n");
      fprintf(fout,"2\n");
      fprintf(fout,"45 360 4\n");
      fprintf(fout,"-%f %f %f \n",d[i]/2,(ri[i]-0.5),(ro[i]+0.5));
      fprintf(fout,"%f %f %f \n",d[i]/2,(ri[i]-0.5),(ro[i]+0.5));
      fprintf(fout,"0. 0. %4.1f\n",z[i]);
      fprintf(fout,"1.  0.  0.  0.  1.  0.  0.  0.  1.\n");
      fprintf(fout,"//*********************************\n");
      for(Int_t j=0; j<num_layer; j++) {        
        fprintf(fout,"trd%d%d\n", i+1, j+1);        
        fprintf(fout,"trd%d\n", i+1);        
        fprintf(fout,"PGON\n");
        fprintf(fout,"air\n");
        fprintf(fout,"2\n");
        fprintf(fout,"45 360 4\n");
        fprintf(fout,"-%f %f %f \n",(d[i]/(num_layer*2)),ri[i],ro[i]);
        fprintf(fout,"%f %f %f \n",(d[i]/(num_layer*2)),ri[i],ro[i]);
        fprintf(fout,"0. 0. %4.1f\n",(-d[i]/2)+30+(60*j));
        fprintf(fout,"1.  0.  0.  0.  1.  0.  0.  0.  1.\n");
        fprintf(fout,"//*********************************\n");
      }
      fprintf(fout,"trd%dradiator#1\n", i+1);        
      fprintf(fout,"trd%d1\n", i+1);        
      fprintf(fout,"PGON\n");
      fprintf(fout,"polypropylene\n");
      fprintf(fout,"2\n");
      fprintf(fout,"45 360 4\n");
      fprintf(fout,"-%f %f %f \n",radiator[0],ri[i],ro[i]);
      fprintf(fout,"%f %f %f \n",radiator[0],ri[i],ro[i]);
      fprintf(fout,"0. 0. %f\n",radiator[1]);
      fprintf(fout,"1.  0.  0.  0.  1.  0.  0.  0.  1.\n");
      fprintf(fout,"//*********************************\n");
      for(Int_t j=1; j<num_layer; j++) {
        fprintf(fout,"trd%dradiator#%d\n", i+1, j+1);        
        fprintf(fout,"trd%d%d\n", i+1,j+1);        
        fprintf(fout,"0. 0. %f\n",radiator[1]);
        fprintf(fout,"1.  0.  0.  0.  1.  0.  0.  0.  1.\n");
        fprintf(fout,"//*********************************\n");
      }
      fprintf(fout,"trd%dgas#1\n", i+1);        
      fprintf(fout,"trd%d1\n", i+1);        
      fprintf(fout,"PGON\n");
      fprintf(fout,"TRDgas\n");
      fprintf(fout,"2\n");
      fprintf(fout,"45 360 4\n");
      fprintf(fout,"-%f %f %f \n",gas[0],ri[i],ro[i]);
      fprintf(fout,"%f %f %f \n",gas[0],ri[i],ro[i]);
      fprintf(fout,"0. 0. %f\n",gas[1]);
      fprintf(fout,"1.  0.  0.  0.  1.  0.  0.  0.  1.\n");
      fprintf(fout,"//*********************************\n");
      for(Int_t j=1; j<num_layer; j++) {
        fprintf(fout,"trd%dgas#%d\n", i+1, j+1);        
        fprintf(fout,"trd%d%d\n", i+1,j+1);        
        fprintf(fout,"0. 0. %f\n",gas[1]);
        fprintf(fout,"1.  0.  0.  0.  1.  0.  0.  0.  1.\n");
        fprintf(fout,"//*********************************\n");
      }
      fprintf(fout,"trd%dpadplane#1\n", i+1);        
      fprintf(fout,"trd%d1\n", i+1);        
      fprintf(fout,"PGON\n");
      fprintf(fout,"goldcoatedcopper\n");
      fprintf(fout,"2\n");
      fprintf(fout,"45 360 4\n");
      fprintf(fout,"-%f %f %f \n",pad[0],ri[i],ro[i]);
      fprintf(fout,"%f %f %f \n",pad[0],ri[i],ro[i]);
      fprintf(fout,"0. 0. %f\n",pad[1]);
      fprintf(fout,"1.  0.  0.  0.  1.  0.  0.  0.  1.\n");
      fprintf(fout,"//*********************************\n");
      for(Int_t j=1; j<num_layer; j++) {
        fprintf(fout,"trd%dpadplane#%d\n", i+1, j+1);        
        fprintf(fout,"trd%d%d\n", i+1,j+1);        
        fprintf(fout,"0. 0. %f\n",pad[1]);
        fprintf(fout,"1.  0.  0.  0.  1.  0.  0.  0.  1.\n");
        fprintf(fout,"//*********************************\n");
      }
      fprintf(fout,"trd%dmylar#1\n", i+1);        
      fprintf(fout,"trd%d1\n", i+1);        
      fprintf(fout,"PGON\n");
      fprintf(fout,"mylar\n");
      fprintf(fout,"2\n");
      fprintf(fout,"45 360 4\n");
      fprintf(fout,"-%f %f %f \n",mylar[0],ri[i],ro[i]);
      fprintf(fout,"%f %f %f \n",mylar[0],ri[i],ro[i]);
      fprintf(fout,"0. 0. %f\n",mylar[1]);
      fprintf(fout,"1.  0.  0.  0.  1.  0.  0.  0.  1.\n");
      fprintf(fout,"//*********************************\n");
      for(Int_t j=1; j<num_layer; j++) {
        fprintf(fout,"trd%dmylar#%d\n", i+1, j+1);        
        fprintf(fout,"trd%d%d\n", i+1,j+1);        
        fprintf(fout,"0. 0. %f\n",mylar[1]);
        fprintf(fout,"1.  0.  0.  0.  1.  0.  0.  0.  1.\n");
        fprintf(fout,"//*********************************\n");
      }
      fprintf(fout,"trd%delectronics#1\n", i+1);        
      fprintf(fout,"trd%d1\n", i+1);        
      fprintf(fout,"PGON\n");
      fprintf(fout,"goldcoatedcopper\n");
      fprintf(fout,"2\n");
      fprintf(fout,"45 360 4\n");
      fprintf(fout,"-%f %f %f \n",electronics[0],ri[i],ro[i]);
      fprintf(fout,"%f %f %f \n",electronics[0],ri[i],ro[i]);
      fprintf(fout,"0. 0. %f\n",electronics[1]);
      fprintf(fout,"1.  0.  0.  0.  1.  0.  0.  0.  1.\n");
      fprintf(fout,"//*********************************\n");
      for(Int_t j=1; j<num_layer; j++) {
        fprintf(fout,"trd%delectronics#%d\n", i+1, j+1);        
        fprintf(fout,"trd%d%d\n", i+1,j+1);        
        fprintf(fout,"0. 0. %f\n",electronics[1]);
        fprintf(fout,"1.  0.  0.  0.  1.  0.  0.  0.  1.\n");
        fprintf(fout,"//*********************************\n");
       }
    }
    fprintf(fout,"//*********************************\n");
    
    fclose(fout);
    printf("... done.\n\n");
  }

}
