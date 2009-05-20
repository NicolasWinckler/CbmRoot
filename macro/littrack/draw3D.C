void draw3D()
{
	TString dir = "/home/d/andrey/test/trunk/global_mu/";
	TString mcFile = dir + "mc.0000.root";

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
	cbmrootlibs();

	TFile* f = new TFile(mcFile);
	TGeoManager *geoMan = (TGeoManager*) f->Get("FAIRGeom");
	geoMan->SetVisLevel(2);

	// Check overlaps
	geoMan->CheckOverlaps(0.0000001);
	geoMan->PrintOverlaps();

	TGeoVolume* master = geoMan->GetMasterVolume();
	//  master->Draw("ogl");

	//  TGeoVolume* much = master->FindNode("much_0")->GetVolume();
	//  much->Draw("ogl");

	TGeoNode* tof = master->FindNode("tof1_0");
	TGeoNode* gas = (TGeoNode*) tof->GetNodes()->FindObject("tof1gas_0");

	TGeoVolume* volume = gas->GetVolume();
	volume->Draw("ogl");
}
