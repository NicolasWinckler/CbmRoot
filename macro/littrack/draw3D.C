/** draw3D.C
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 1.0
 * Macro draws 3D view of the CBM detector using TGeo functionality.
 **/

void draw3D()
{
	//Input directory
	TString dir = "/home/d/andrey/std_1e-/";
	//Input MC file with TGeo
//	TString mcFile = dir + "mc.0000.root";
	TString parFile = dir + "param.0000.root";

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
	cbmrootlibs();

	TFile* f = new TFile(parFile);
	f->Get("FairBaseParSet");
//	TFile* f = new TFile(mcFile);
//	TGeoManager *geoMan = (TGeoManager*) f->Get("FAIRGeom");
	gGeoManager->SetVisLevel(4);

	// Check overlaps
//	gGeoManager->CheckOverlaps(0.0001);
//	gGeoManager->PrintOverlaps();

	TGeoVolume* master = gGeoManager->GetMasterVolume();
	master->Draw("ogl");
}
