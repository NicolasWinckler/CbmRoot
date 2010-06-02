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

  TrdDigis->SetLayerStation1(kTRUE,kFALSE,kFALSE,kFALSE);
  TrdDigis->SetLayerStation2(kFALSE,kFALSE,kFALSE,kFALSE);
  TrdDigis->SetLayerStation3(kFALSE,kFALSE,kFALSE,kFALSE);
                                                               
  fMan->AddTask(Track);
  
  fMan->AddTask(TrdPoints);
  fMan->AddTask(TrdHits);
  fMan->AddTask(TrdDigis);
    
  fMan->Init();                    

  gGeoManager->SetVisLevel(3);
}
