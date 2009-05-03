
void much_draw3D(const char* mcfile = ""){

   if (mcfile == "") {
      mcfile = "data/Jpsi.auau.25gev.centr.mc.root";
   }

   gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
   basiclibs();
   gSystem->Load("libGeoBase");
   gSystem->Load("libParBase");
   gSystem->Load("libBase");
   gSystem->Load("libCbmBase");
   gSystem->Load("libCbmData");
   gSystem->Load("libField");
   gSystem->Load("libGen");
   gSystem->Load("libPassive");
   gSystem->Load("libSts");
   gSystem->Load("libMuch");

   TFile* f = new TFile(mcfile);
   TGeoManager *geoMan = (TGeoManager*) f->Get("FAIRGeom");
   geoMan->SetVisLevel(0);

   // Check overlaps
   geoMan->CheckOverlaps(0.0000001);
   geoMan->PrintOverlaps();

   TGeoVolume* master = geoMan->GetMasterVolume();
   //master->Draw("ogl");

   TGeoVolume* much = master->FindNode("much_0")->GetVolume();
   much->Draw("ogl");
}
