void draw3D()
{
	TString dir = "/home/d/andrey/test/trunk/global_mustraw/";
	TString mcFile = dir + "mc.0000.root";

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
	cbmrootlibs();

	TFile* f = new TFile(mcFile);
	TGeoManager *geoMan = (TGeoManager*) f->Get("FAIRGeom");
	geoMan->SetVisLevel(4);

	// Check overlaps
	geoMan->CheckOverlaps(0.0000001);
	geoMan->PrintOverlaps();

	TGeoVolume* master = geoMan->GetMasterVolume();
	master->Draw("ogl");
}
