// --------------------------------------------------------------------------
//
// Macro for reconstruction of simulated events
//
// STS, RICH and TRD Hitproducers
// STS track finding and fitting
// TRD track finding and fitting
// RICH ring finding (ideal) and fitting
// Global track finding (ideal), rich assignment
// Primary vertex finding (ideal)
// Matching of reconstructed and MC tracks in STS, RICH and TRD
//
// V. Friese   24/02/2006
//
// --------------------------------------------------------------------------


{

  // ========================================================================
  //          Adjust this part according to your requirements

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;


  //  Digitisation files
  TList *parFileList = new TList();

  TString paramDir = gSystem->Getenv("VMCWORKDIR");
  paramDir += "/parameters/sts/";

  TObjString stsDigiFile = paramDir + "sts_Standard_s3055AAFK5.SecD.digi.par";
  parFileList->Add(&stsDigiFile);

  paramDir = gSystem->Getenv("VMCWORKDIR");
  paramDir += "/parameters/much/";

  TObjString muchDigiFile = paramDir + "much_standard.digi.par";
  parFileList->Add(&muchDigiFile);

  TString digiFile = "data/much_digi.root";

  // Input file (MC events)
  TString inFile = "data/Jpsi.auau.25gev.centr.mc.root";

  // Number of events to process
  Int_t nEvents = 2;

  // Output file
  TString outFile = "data/Jpsi.auau.25gev.centr.muchreco.root";


  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR//cbmbase/CbmDetectorList.h");
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
  gSystem->Load("libSts");
  gSystem->Load("libRich");
  gSystem->Load("libTrd");
  gSystem->Load("libTof");
  gSystem->Load("libEcal");
  gSystem->Load("libMuch");
  gSystem->Load("libGlobal");
  gSystem->Load("libKF");
  gSystem->Load("libL1");
  gSystem->Load("libLittrack");
  // ------------------------------------------------------------------------

  // In general, the following parts need not be touched
  // ========================================================================



  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------



  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------

  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(gFile);
  FairParAsciiFileIo* parInput2 = new FairParAsciiFileIo();
  parInput2->open(parFileList,"in");
  rtdb->setFirstInput(parInput1);
  rtdb->setSecondInput(parInput2);
  rtdb->setOutput(parInput1);
  rtdb->saveOutput();

  fRun->LoadGeometry();
  // ------------------------------------------------------------------------


  // -----   STS digitizer   -------------------------------------------------
  FairTask* stsDigitize = new CbmStsDigitize(iVerbose);
  fRun->AddTask(stsDigitize);
  // -------------------------------------------------------------------------


  // -----  STS hit finding   ------------------------------------------------
  FairTask* stsFindHits = new CbmStsFindHits(iVerbose);
  fRun->AddTask(stsFindHits);
  // -------------------------------------------------------------------------


  // -----  STS hit matching   -----------------------------------------------
  CbmStsMatchHits* stsMatchHits = new CbmStsMatchHits(iVerbose);
  fRun->AddTask(stsMatchHits);
  // -------------------------------------------------------------------------


  // ------------------------------------------------------------------------

  // -----   STS track finding   --------------------------------------------
  // ---  STS track finding   ------------------------------------------------
  CbmKF* kalman = new CbmKF();
  fRun->AddTask(kalman);
  CbmL1* l1 = new CbmL1();
  fRun->AddTask(l1);
  CbmStsTrackFinder* stsTrackFinder    = new CbmL1StsTrackFinder();
  FairTask* stsFindTracks = new CbmStsFindTracks(iVerbose, stsTrackFinder);
  fRun->AddTask(stsFindTracks);
  // -------------------------------------------------------------------------


  // -----   STS track matching   -------------------------------------------
  CbmStsMatchTracks* stsMatchTracks = new CbmStsMatchTracks(iVerbose);
  fRun->AddTask(stsMatchTracks);
  // ------------------------------------------------------------------------


  // -----   STS track fitting   --------------------------------------------
  CbmStsTrackFitter* stsTrackFitter = new CbmStsKFTrackFitter();
  FairTask* stsFitTracks = new CbmStsFitTracks(stsTrackFitter, iVerbose);
  fRun->AddTask(stsFitTracks);
  // ------------------------------------------------------------------------


  // -----   Primary vertex finding   ---------------------------------------
  CbmPrimaryVertexFinder* pvFinder     = new CbmPVFinderKF();
  CbmFindPrimaryVertex* pvFindTask = new CbmFindPrimaryVertex(pvFinder);
  fRun->AddTask(pvFindTask);
  // ------------------------------------------------------------------------

  // ---  MuCh digitizer ----------------------------------------------------
  CbmMuchDigitizeAdvancedGem* muchDigitize = new CbmMuchDigitizeAdvancedGem("MuchDigitize", digiFile.Data(), iVerbose);
  fRun->AddTask(muchDigitize);
  // ------------------------------------------------------------------------

  // ---  MuCh hit finder ---------------------------------------------------
  CbmMuchFindHitsAdvancedGem* muchFindHits = new CbmMuchFindHitsAdvancedGem("MuchFindHits", digiFile.Data(), iVerbose);
  fRun->AddTask(muchFindHits);
  // ------------------------------------------------------------------------



  // ---  Much track finder ---------------------------------------------------
  CbmMuchTrackFinder* muchTrackFinder = new CbmLitMuchTrackFinderBranch();
  CbmMuchFindTracks* muchFindTracks = new CbmMuchFindTracks("Much Track Finder");
  muchFindTracks->UseFinder(muchTrackFinder);
  fRun->AddTask(muchFindTracks);

  // -----   Much track matching   -------------------------------------------
  CbmMuchMatchTracks* muchMatchTracks = new CbmMuchMatchTracks();
  fRun->AddTask(muchMatchTracks);

  // -----   Much track Qa   ------------------------------------------
  CbmLitRecQa* muchRecQa = new CbmLitRecQa(12, 0.7, kMUCH, 1);
  muchRecQa->SetNormType(2); // '2' to number of STS tracks
  fRun->AddTask(muchRecQa);

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0,nEvents);//0,nEvents);
  // ------------------------------------------------------------------------

  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully.2" << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Parameters are in MC file" << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------

  cout << " Test passed" << endl;
  cout << " All ok " << endl;
  exit(0);
}

