eventDisplay()
{

  // ========================================================================
  // geometry selection for sim + reco  by Cyrano                            
  // ========================================================================
  ifstream whichTrdGeo;
  whichTrdGeo.open("whichTrdGeo",ios::in);
  TString selectGeo;
  if (whichTrdGeo) whichTrdGeo >> selectGeo;
  TString digipar = selectGeo(0,8);
  cout << "selected geometry : >> " << selectGeo << " << (to select a different geometry, edit macro/trd/whichTrdGeo file)" << endl;
  cout << "selected digipar  : >> " << digipar << " << " << endl;
  whichTrdGeo.close();
  if (digipar.Length() == 0) digipar = "trd_standard";

  TString  InputFile     ="data/test.mc.root";
  TString  ParFile       ="data/params.root";

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();

  fRun->SetInputFile(InputFile.Data());
  fRun->SetOutputFile("data/test.root");

  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(ParFile.Data());
  rtdb->setFirstInput(parInput1);

  TString paramDir = gSystem->Getenv("VMCWORKDIR");
  paramDir += "/parameters";
  TString trdDigiFile = paramDir + "/trd/" + digipar + ".digi.par";

  FairParAsciiFileIo* parInput2 = new FairParAsciiFileIo();
  cout << "-I- CbmTrdDigiPar: Using file " << trdDigiFile << endl;
  parInput2->open(trdDigiFile, "in");
  rtdb->setSecondInput(parInput2);
  // ------------------------------------------------------------------------

  FairEventManager *fMan= new FairEventManager();
  FairMCTracks       *Track     = new FairMCTracks       ("Monte-Carlo Tracks");
  FairMCPointDraw    *TrdPoints = new FairMCPointDraw    ("TrdPoint",kBlue , kFullSquare);
  CbmPixelHitSetDraw *TrdHits   = new CbmPixelHitSetDraw ("TrdHit"  ,kRed  , kFullSquare);
  //  CbmTrdDigiDraw     *TrdDigis  = new CbmTrdDigiDraw     ("TrdDigi" ,kGreen, kFullSquare);

  //  TrdDigis->SetLayerStation1(kTRUE,kTRUE,kTRUE,kTRUE);
  //  TrdDigis->SetLayerStation2(kTRUE,kTRUE,kTRUE,kTRUE);
  //  TrdDigis->SetLayerStation3(kTRUE,kTRUE,kTRUE,kTRUE);

  fMan->AddTask(Track);
  fMan->AddTask(TrdPoints);
  fMan->AddTask(TrdHits);
  //  fMan->AddTask(TrdDigis);

  fMan->Init(1,4,10000);  
}
