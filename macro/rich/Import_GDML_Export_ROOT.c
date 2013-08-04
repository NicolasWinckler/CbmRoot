void Import_GDML_Export_ROOT()
{
	TGeoManager *gdml = new TGeoManager("gdml","FAIRGeom");

	TGDMLParse parser;
	// Define your input GDML file HERE
	TGeoVolume* gdmlTop = parser.GDMLReadFile ("rich_v13a.gdml");

	gGeoManager->SetTopVolume(gdmlTop);
	gGeoManager->CloseGeometry();

	// Define you output ROOT file HERE
	TFile* outfile = new TFile("rich_v13a.root", "RECREATE");
	gdmlTop->Write();
	outfile->Close();
}
