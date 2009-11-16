void run_reco_param_opt_ht(Float_t p1, Float_t p2)
{
	Int_t nEvents = 700;

  // ========================================================================
  //          Adjust this part according to your requirements

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;

  // Input file (MC events)
  TString inFile1 = "/d/cbm02/slebedev/rich/MAR09/auau.25gev.centr.0000.mc.root";
  TString inFile2 = "/d/cbm02/slebedev/rich/MAR09/auau.25gev.centr.0000.reco.root";
  // Parameter file
  TString parFile = "/d/cbm02/slebedev/rich/MAR09/auau.25gev.centr.0000.params.root";

  // STS digitisation file
  TString stsDigiFile = "sts_standard.digi.par";

  // Output file
  TString outFile = "/d/cbm02/slebedev/rich/MAR09/auau.25gev.centr.0000.recorich.root";

  // In general, the following parts need not be touched
  // ========================================================================



  // ----    Debug option   -------------------------------------------------
  gDebug = 0;
  // ------------------------------------------------------------------------



  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------



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
  gSystem->Load("libMinuit2"); // Nedded for rich ellipse fitter
  // ------------------------------------------------------------------------



  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *run= new FairRunAna();
  run->SetInputFile(inFile1);
  run->AddFriend(inFile2);
  run->SetOutputFile(outFile);


 // CbmL1RichENNRingFinder* richFinder = new CbmL1RichENNRingFinder(iVerbose);
  TString richGeoType = "compact";
  //CbmL1RichENNRingFinder* richFinder = new CbmL1RichENNRingFinder();
  CbmRichRingFinderHough* richFinder = new CbmRichRingFinderHough(iVerbose, richGeoType);
  richFinder->SetFindOptPar(true);
  richFinder->SetParameters(
		  2, //Nof parts
		  11.5, 2.5, //Max-min dist
		  3.3, 5.7, //Max-min radius
		  5, 2, 5, 2, //HTCut
		  25, 25, 32, //NofBins
		  -0.5, 0.35, 0.4,
		  2, 0.8, 3., 1.2);

  CbmRichFindRings* richFindRings = new CbmRichFindRings();
  richFindRings->UseFinder(richFinder);
  run->AddTask(richFindRings);

  CbmRichMatchRings* matchRings = new CbmRichMatchRings(iVerbose);
  run->AddTask(matchRings);

  CbmRichRingQa* richQa   =  new CbmRichRingQa("Qa","qa", 0);
  run->AddTask(richQa);

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
  cout << " All ok " << endl;
  exit(0);
}
