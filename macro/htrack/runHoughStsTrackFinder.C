// --------------------------------------------------------------------------
//
// Macro for reconstruction in STS from MC data
//
// Tasks:  CbmStsDigitise
//         CbmStsFindHits
//         CbmStsMatchHits
//         CbmStsFindTracks
//
// C. Steinle   23/01/2007
//
// --------------------------------------------------------------------------


{

  // ========================================================================
  //          Adjust this part according to your requirements

  // Input folder for the files
  TString inputFolder  = "files";

  // Input folder for the files
  TString outputFolder = "files";

  // Collision system
  TString system       = "auau";

  // Beam momentum
  TString beam         = "25gev";

  // Trigger (centrality)
  TString trigger      = "centr";

  // Input file (MC events)
  TString inFile       = inputFolder  + "/" + system + "." + beam + "." + trigger + ".mc.root";

  // Output file
  TString outFile      = outputFolder + "/" + system + "." + beam + "." + trigger + ".reco.root";

  // Parameter file
  TString parFile      = inputFolder  + "/" + system + "." + beam + "." + trigger + ".params.root";

  // Analysis file
  TString anaFile      = outputFolder + "/" + system + "." + beam + "." + trigger + ".ana.root";

  // Digitisation file
  TString digiFile     = "sts_standard.digi.par";

  // Number of events to process
  Int_t nEvents        = 1;

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose       = 1;


  // In general, the following parts need not be touched
  // ========================================================================

  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libCbmBase");
  gSystem->Load("libField");
  gSystem->Load("libPassive");
  gSystem->Load("libSts");
  gSystem->Load("libGeane");
  gSystem->Load("libHTrack");
  // ------------------------------------------------------------------------

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  // -----   Reconstruction run   -------------------------------------------
  CbmRunAna *fRun = new CbmRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------

  // -----  Parameter database   --------------------------------------------
  TString stsDigiFile        = gSystem->Getenv("VMCWORKDIR");
  stsDigiFile               += "/parameters/sts/";
  stsDigiFile               += digiFile;
  CbmRuntimeDb* rtdb         = fRun->GetRuntimeDb();
  CbmParRootFileIo*  parIo1  = new CbmParRootFileIo();
  CbmParAsciiFileIo* parIo2  = new CbmParAsciiFileIo();
  parIo1->open(parFile.Data());
  parIo2->open(stsDigiFile.Data(),"in");
  rtdb->setFirstInput(parIo1);
  rtdb->setSecondInput(parIo2);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();
  fRun->LoadGeometry();
  // ------------------------------------------------------------------------

  // -----   STS digitizer   ------------------------------------------------
  CbmStsDigitize* stsDigitize         = new CbmStsDigitize("STSDigitize", iVerbose);
  fRun->AddTask(stsDigitize);
  // ------------------------------------------------------------------------

  // ---  STS hit finding   -------------------------------------------------
  CbmStsFindHits* findHits            = new CbmStsFindHits("STS Hit Finder", iVerbose);
  fRun->AddTask(findHits);
  // ------------------------------------------------------------------------

  // ---  STS hit matching   ------------------------------------------------
  CbmStsMatchHits* matchHits          = new CbmStsMatchHits("STS Hit Matcher", iVerbose);
  fRun->AddTask(matchHits);
  // ------------------------------------------------------------------------

  // ---  STS Track finding   -------------------------------------------------
  CbmStsFindTracks* findTracks        = new CbmStsFindTracks(iVerbose, NULL, kFALSE, "STS Track Finder");
  fRun->AddTask(findTracks);
  // ------------------------------------------------------------------------

  // ---  STS TrackFinder     -------------------------------------------------
  CbmHoughStsTrackFinder* trackFinder = new CbmHoughStsTrackFinder();
  trackFinder->SetOutputFile(anaFile);
  findTracks->UseFinder(trackFinder);
  // ------------------------------------------------------------------------

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0,nEvents);
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

}
