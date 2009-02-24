void much_draw3D()
{
  TString dir = "/home/d/andrey/events/newmuch/standard/10mu/mu_urqmd/";
  TString mcFile = dir + "mc.root";

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
  cbmrootlibs();

  TFile* f = new TFile(mcFile);
  TGeoManager *geoMan = (TGeoManager*) f->Get("FAIRGeom");
  geoMan->SetVisLevel(0);

  // Check overlaps
  geoMan->CheckOverlaps(0.0000001);
  geoMan->PrintOverlaps();

  TGeoVolume* master = geoMan->GetMasterVolume();
  master->Draw("ogl");

//  TGeoVolume* much = master->FindNode("much_0")->GetVolume();
//  much->Draw("ogl");
}
