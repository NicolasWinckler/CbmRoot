void Create_TRD_Geometry() {

   gSystem->Load("libGeom");
   TGeoManager* gGeoMan = new TGeoManager("TRDGeom", "TRD geometry");

   TGeoVolume* top = new TGeoVolumeAssembly("TRD");
   gGeoMan->SetTopVolume(top);

   // Define all media and materials needed for the TRD detector.
   // The code is taken from a dump of an already existing GeoManager

   // MATERIALS, MIXTURES AND TRACKING MEDIA
   // Mixture: air
   Int_t nel;
   Double_t density, a, z, w;
   Double_t par[8];

   nel     = 3;
   density = 0.001205;
   pMat1 = new TGeoMixture("air", nel,density);
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
   pMat16 = new TGeoMixture("polypropylene", nel,density);
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
   pMat17 = new TGeoMixture("TRDgas", nel,density);
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
   TRDgas = new TGeoMedium("TRDgas", numed,pMat17, par);

   // Mixture: goldcoatedcopper
   nel     = 2;
   density = 9.063400;
   pMat18 = new TGeoMixture("goldcoatedcopper", nel,density);
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
   goldcoatedcopper = new TGeoMedium("goldcoatedcopper", numed,pMat18, par);

   // Mixture: mylar
   nel     = 3;
   density = 1.390000;
   pMat19 = new TGeoMixture("mylar", nel,density);
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
   mylar = new TGeoMedium("mylar", numed,pMat19, par);

   // Mixture: G10
   nel     = 4;
   density = 1.700000;
   pMat20 = new TGeoMixture("G10", nel,density);
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
   G10 = new TGeoMedium("G10", numed,pMat20, par);

   // Here comes now the creation of needed TRD modules.
   // The modules are then positioned in the next step

   //TODO: Calculate all the required dimensions and positions

   // TGeoPgon(const char* name, Double_t phi, Double_t dphi, 
   //          Int_t nedges, Int_t nz)

   /*
   Double_t trd1_param[16] = {45., 360., 4., 4., 
			      -1300., 4300., 4600., 
			      -300., 1500., 4600., 
			      -300., 240., 4600., 
			      300., 240., 4600.}: 
   
   TGeoShape* trd1_shape = new TGeoPgon(trd1_param); 
   TGeoVolume* trd1_volume = new TGeoVolume("trd1", trd1_shape, air);
   TGeoTranslation* trd1_trans = new TGeoTranslation(0., 0., 4800.);
   TGeoRotation* trd1_rot = new TGeoRotation();
   TGeoCombiTrans* trd1 = new TGeoCombiTrans(*trd1_trans, *trd1_rot);
   trd1->SetName("trd1");
   trd1->RegisterYourself();
   */

   Double_t phi1, dphi, nedges, nz, z,rmin, rmax;
   // Shape: trd1 type: TGeoPgon
   phi1    = 45.000000;
   dphi    = 360.000000;
   nedges = 4;
   nz      = 4;
   TGeoPgon *trd1_pgon = new TGeoPgon("trd1",phi1,dphi,nedges,nz);
   z     = -130.000000;
   rmin  = 430.000000;
   rmax  = 460.000000;
   trd1_pgon->DefineSection(0, z,rmin,rmax);
   z     = -30.000000;
   rmin  = 150.000000;
   rmax  = 460.000000;
   trd1_pgon->DefineSection(1, z,rmin,rmax);
   z     = -30.000000;
   rmin  = 24.000000;
   rmax  = 460.000000;
   trd1_pgon->DefineSection(2, z,rmin,rmax);
   z     = 30.000000;
   rmin  = 24.000000;
   rmax  = 460.000000;
   trd1_pgon->DefineSection(3, z,rmin,rmax);

   TGeoShape *trd1_shape = trd1_pgon;
   // Volume: trd1
   TGeoVolume* trd1 = new TGeoVolume("trd1",trd1_shape, air);
   // Translation: 
   TGeoTranslation *trd1_trans = new TGeoTranslation("",0. ,0. , 480.);
   top->AddNode(trd1, 0, trd1_trans);
  
   gGeoMan->CloseGeometry();
   TFile* outfile = new TFile("TRD_Geom.root","RECREATE");
   top->Write();
   outfile->Close();
   top->Draw("ogl");

}
