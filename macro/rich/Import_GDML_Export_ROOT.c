void Import_GDML_Export_ROOT()
{
	TGeoManager *gdml = new TGeoManager("gdml", "FAIRGeom");

	TGDMLParse parser;
	// Define your input GDML file HERE
	TGeoVolume* gdmlTop = parser.GDMLReadFile ("../../geometry/rich/rich_v13b.gdml");

	TGeoVolume* rootTop = new TGeoVolumeAssembly("geometryFromGDML");
	gGeoManager->SetTopVolume(rootTop);

	// Define your position HERE
	TGeoRotation* rot = new TGeoRotation ("rot", 0., 0., 0.);
	TGeoCombiTrans* posrot = new TGeoCombiTrans (0., 0., 255., rot);

	rootTop->AddNode (gdmlTop, 1, posrot);

	gGeoManager->CloseGeometry();

	// Define you output ROOT file HERE
	TFile* outfile = new TFile("../../geometry/rich/rich_v13b.root", "RECREATE");
	rootTop->Write();
	outfile->Close();

}
