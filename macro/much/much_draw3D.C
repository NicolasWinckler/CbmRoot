void much_draw3D(char* mcfile = "data/Jpsi.auau.25gev.centr.mc.root"){

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
   f->Get("FairBaseParSet");
   gGeoManager->SetVisLevel(0);
//
   // Check overlaps
   gGeoManager->CheckOverlaps(0.0000001);
   gGeoManager->PrintOverlaps();

   TGeoVolume* master = gGeoManager->GetMasterVolume();

   // Draw all
   //master->Draw("ogl");

   // Draw much
   TGeoVolume* much = master->FindNode("much_0")->GetVolume();
   much->Draw("ogl");
}
