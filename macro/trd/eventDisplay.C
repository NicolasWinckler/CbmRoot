eventDisplay()
{
   // Load basic libraries
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libCbmBase");
  gSystem->Load("libCbmData");
  gSystem->Load("libField");
  gSystem->Load("libGen");
  gSystem->Load("libPassive");
  gSystem->Load("libMvd");
  gSystem->Load("libSts");
  gSystem->Load("libRich");
  gSystem->Load("libTrd");
  gSystem->Load("libTof");
  gSystem->Load("libEcal");
  gSystem->Load("libEve");
  gSystem->Load("libEventDisplay");
  gSystem->Load("libCbmDisplay");

                                     
  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile("data/test.mc.root");
  fRun->AddFriend("data/test.eds.root");
  
  fRun->SetOutputFile("data/test.root");

  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();

  FairParRootFileIo* parIo1 = new FairParRootFileIo();
  parIo1->open("data/params.root");
  rtdb->setFirstInput(parIo1);

  FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
  parIo2->open("./trd.digi.par","in");
  rtdb->setSecondInput(parIo2);
  // ------------------------------------------------------------------------

  FairEventManager *fMan= new FairEventManager();
  FairMCTracks *Track =  new FairMCTracks ("Monte-Carlo Tracks");
  FairMCPointDraw *TrdPoints =   new FairMCPointDraw ("TrdPoint",kBlue,  kFullSquare);
  CbmPixelHitSetDraw *TrdHits =   new CbmPixelHitSetDraw ("TrdHit",kRed,  kFullSquare);
  CbmTrdDigiDraw *TrdDigis =   new CbmTrdDigiDraw("TrdDigi",kGreen, kFullSquare);

  // ugly version to switch of the drawing of digis in layers which are not visible
  // in the eventdisplay.
  // This works only for the digis. For the points a class from FairRoot is used which
  // is generic and does not know about the specific detector.
  // TODO: check for each digi if the volume it is inside is visible or not. If not
  // don't add the digi to the list of digis to be drawn. Check if it is possible to
  // get the required information from the gGeomanager and the eventdisplay. With the
  // geomanager it is possible to get the volume the digi/hit/point is in. From the
  // eventdisplay one can get hopefully the information if a volume is visible.

  TrdDigis->SetLayerStation1(kTRUE,kFALSE,kFALSE,kFALSE);
  TrdDigis->SetLayerStation2(kTRUE,kFALSE,kFALSE,kFALSE);
  TrdDigis->SetLayerStation3(kTRUE,kFALSE,kFALSE,kFALSE);
                                                               
  fMan->AddTask(Track);
  
  fMan->AddTask(TrdPoints);
  fMan->AddTask(TrdHits);
  fMan->AddTask(TrdDigis);
    
  fMan->Init();                    

  gGeoManager->SetVisLevel(3);
}
