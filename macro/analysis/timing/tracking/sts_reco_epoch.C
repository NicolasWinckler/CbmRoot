void sts_reco_epoch(TString dir = "data/", Int_t nEvents=10){
  TString mcFile      = dir + "epoch.root";
  TString parFile     = dir + "param.root";
  TString stsRecoFile = dir + "sts.reco.root";
  TString digiFile    = "sts_v11a.digi.par";

  Int_t iVerbose = 1;
  TStopwatch timer;
  timer.Start();

  // ----  Load libraries   -------------------------------------------------
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
  gSystem->Load("libGlobal");
  gSystem->Load("libKF");
  gSystem->Load("libL1");
  gSystem->Load("libMuch");
  // ------------------------------------------------------------------------

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna* run= new FairRunAna();
  run->SetInputFile(mcFile);
//  run->AddFile(dir+"mc_1.root");
  run->SetOutputFile(stsRecoFile);
  // ------------------------------------------------------------------------

  // -----   STS digitiser   ------------------------------------------------
  CbmStsIdealDigitizeEpoch* stsDigitize = new CbmStsIdealDigitizeEpoch("STSDigitize", iVerbose);
  stsDigitize->SetEpoch(1);
  stsDigitize->SetTimeSmearing(0.01);
  run->AddTask(stsDigitize);
  // ------------------------------------------------------------------------

  // ---  STS hit finding   -------------------------------------------------
  CbmStsIdealFindHitsEpoch* findHits = new CbmStsIdealFindHitsEpoch("STSFindHits", iVerbose);
  findHits->SetSliceSeparationTime(4);
  run->AddTask(findHits);
  // ------------------------------------------------------------------------
//
  // ---  STS hit matching   ------------------------------------------------
  FairTask* matchHits = new CbmStsIdealMatchHitsEpoch("STSMatchHits",iVerbose);
  run->AddTask(matchHits);
  // ------------------------------------------------------------------------
//
//
  // -----   STS track finding   --------------------------------------------
  FairTask* kalman= new CbmKF();
  run->AddTask(kalman);
  CbmL1* l1 = new CbmL1();
  l1->SetSliceSeparationTime(3);
  l1->SetVerbose(2);
  run->AddTask(l1);
  CbmStsTrackFinder* trackFinder    = new CbmL1StsTrackFinder();
  FairTask* findTracks = new CbmStsFindTracksEpoch(iVerbose, trackFinder);
  run->AddTask(findTracks);
  // ------------------------------------------------------------------------

  // -----   STS track matching   -------------------------------------------
  FairTask* matchTracks = new CbmStsMatchTracksEpoch("Match tracks", iVerbose);
  run->AddTask(matchTracks);
  // ------------------------------------------------------------------------

  // -----   STS track fitting   --------------------------------------------
  CbmStsTrackFitter* trackFitter = new CbmStsKFTrackFitter();
  FairTask* fitTracks = new CbmStsFitTracks("STS Track Fitter", trackFitter, iVerbose);
  run->AddTask(fitTracks);
  // ------------------------------------------------------------------------


  // -----  Parameter database   --------------------------------------------
  TString stsDigiFile = gSystem->Getenv("VMCWORKDIR");
  stsDigiFile += "/parameters/sts/";
  stsDigiFile += digiFile;
  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  FairParRootFileIo*  parIo1 = new FairParRootFileIo();
  FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
  parIo1->open(parFile.Data());
  parIo2->open(stsDigiFile.Data(),"in");
  rtdb->setFirstInput(parIo1);
  rtdb->setSecondInput(parIo2);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();
  // ------------------------------------------------------------------------

  // -----   Initialise and run   -------------------------------------------
  run->Init();
  run->Run(0, nEvents);
  // ------------------------------------------------------------------------

  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished successfully." << endl;
  cout << "Output file is "         << stsRecoFile << endl;
  cout << "Parameter file is "      << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------
}
