void much_draw3D(char* geofile = "geometry.root"){
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gROOT->LoadMacro("$VMCWORKDIR/macro/much/muchlibs.C");
  muchlibs();
  
  TFile* f = new TFile(geofile);
  f->Get("FairBaseParSet");
  TGeoManager* gGeoManager = (TGeoManager*) f->Get("FAIRGeom");
  gGeoManager->SetVisLevel(0);

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
