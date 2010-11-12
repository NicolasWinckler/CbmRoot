check_overlaps() 
{
  // Number of known and unresolved overlaps. Most are in the Magnet.
  // Some are in the Sts.
  // Until they can be fixed we will not mark the test as error if the
  // number of overlaps is eqaul the known number of overlaps.
  // Today there are also some overlaps in the rich i don't remember.
  // FU 12.11.10

  Int_t benchmarkNumber = 94;

  TFile* f = new TFile("data/geofile_full.root");
  gGeoManager = (TGeoManager*) f->Get("FAIRGeom"); 
  gGeoManager->CheckOverlaps(0.001);
  TObjArray *overlapArray = gGeoManager->GetListOfOverlaps();
  Int_t numOverlaps = overlapArray->GetEntries();
  gGeoManager->PrintOverlaps();
  if ( numOverlaps != benchmarkNumber ) {
    cout << " Test failed" << endl;
    cout << " We have in total "<<numOverlaps<<" overlaps."<<endl;
  } else {
    cout << " Test passed" << endl;
    cout << " All ok " << endl;
  }
}
