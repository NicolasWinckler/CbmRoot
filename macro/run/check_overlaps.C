check_overlaps() 
{
  TFile* f = new TFile("data/geofile_full.root");
  gGeoManager = (TGeoManager*) f->Get("FAIRGeom"); 
  gGeoManager->CheckOverlaps(0.001);
  TObjArray *overlapArray = gGeoManager->GetListOfOverlaps();
  Int_t numOverlaps = overlapArray->GetEntries();
  gGeoManager->PrintOverlaps();
  if ( numOverlaps > 0 ) {
    cout << " Test failed" << endl;
    cout << " We have in total "<<numOverlaps<<" overlaps."<<endl;
  } else {
    cout << " Test passed" << endl;
    cout << " All ok " << endl;
  }
}
