
void run_reco_geotest(Int_t nEvents = 200)
{

  // ========================================================================
  //          Adjust this part according to your requirements

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;

  // Input file (MC events)
  TString inFile = "/d/cbm06/user/slebedev/geotest/mc.00.root";

  // Parameter file
  TString parFile = "/d/cbm06/user/slebedev/geotest/params.00.root";

  // STS digitisation file
  TString stsDigiFile = "sts_standard.digi.par";

  // Output file
  TString outFile = "/d/cbm06/user/slebedev/geotest/reco.00.root";

  gDebug = 0;

  TStopwatch timer;
  timer.Start();

  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gROOT->LoadMacro("$VMCWORKDIR/macro/rich/cbmlibs.C");
  cbmlibs();

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *run= new FairRunAna();
  run->SetInputFile(inFile);
  run->SetOutputFile(outFile);


  // ---------------------RICH Hit Producer ----------------------------------
  Double_t richPmtRad  = 0.4;     // PMT radius [cm]
  Double_t richPmtDist = 0.;      // Distance between PMTs [cm]
  Int_t    richDetType = 5;       // Detector type Hamamatsu H8500-03
  Int_t    richNoise   = 220;     // Number of noise points per event
  CbmRichHitProducer* richHitProd
    = new CbmRichHitProducer(richPmtRad, richPmtDist, richDetType,
			     richNoise, iVerbose);
  run->AddTask(richHitProd);
  //--------------------------------------------------------------------------


  //--------------------- RICH Ring Finding ----------------------------------
  CbmRichRingFinderIdeal* richFinder = new CbmRichRingFinderIdeal(iVerbose);
  CbmRichFindRings* richFindRings = new CbmRichFindRings();
  richFindRings->UseFinder(richFinder);
  run->AddTask(richFindRings);
  //--------------------------------------------------------------------------


  //-------------------- RICH Ring Fitting -----------------------------------
  // B-field configuration
  TString field ="muon";  // choose between "muon" or "active"
  Double_t iRingCorr = 0.;      // correction done (default), choose 0 if not
  CbmRichRingFitter* richFitter = new CbmRichRingFitterEllipseTau(iVerbose, iRingCorr, field);
  CbmRichFitRings* fitRings = new CbmRichFitRings("","",richFitter);
  run->AddTask(fitRings);
  //--------------------------------------------------------------------------

  // ------------------- RICH Ring matching  ---------------------------------
  CbmRichMatchRings* matchRings = new CbmRichMatchRings(iVerbose);
  run->AddTask(matchRings);
  // -------------------------------------------------------------------------

   CbmRichGeoTest* geoTest = new CbmRichGeoTest();
  run->AddTask(geoTest);


  // -----  Parameter database   --------------------------------------------
  TString stsDigi = gSystem->Getenv("VMCWORKDIR");
  stsDigi += "/parameters/sts/";
  stsDigi += stsDigiFile;
  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  FairParRootFileIo* parIo1 = new FairParRootFileIo();
  FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
  parIo1->open(parFile.Data());
  parIo2->open(stsDigi.Data(),"in");
  rtdb->setFirstInput(parIo1);
  rtdb->setSecondInput(parIo2);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();
  // ------------------------------------------------------------------------

  // -----   Intialise and run   --------------------------------------------
  run->LoadGeometry();
  run->Init();
  cout << "Starting run" << endl;
  run->Run(0,nEvents);
  // ------------------------------------------------------------------------

  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------

  cout << " Test passed" << endl;
  cout << " All ok" << endl;
  exit(0);
}
