void run_electrons_qa(Int_t nEvents = 700)
{

  // ========================================================================
  //          Adjust this part according to your requirements

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;
//   TString inFile1 = "/d/cbm02/slebedev/rich/MAY08/electronId/urqmd.0000.mc.root";
//   TString inFile2 = "/d/cbm02/slebedev/rich/MAY08/electronId/urqmd.0000.reco.newstsrich.root";
//   TString parFile = "/d/cbm02/slebedev/rich/MAY08/electronId/urqmd.0000.params.root";

  TString inFile1 = "/d/cbm02/slebedev/ebelolap/compact/urqmd.mc.40.root";
  TString inFile2 = "/d/cbm02/slebedev/rich/MAY08/electronId/urqmd.0000.reco.compact.root";
  TString parFile = "/d/cbm02/slebedev/ebelolap/compact/urqmd.params.40.root";

   // STS digitisation file
   TString stsDigiFile = "sts_standard.digi.par";

   // Output file
   TString outFile = "/d/cbm02/slebedev/rich/MAY08/electronId/recoqa.compact.root";


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
  //run->AddFriend(inFile3);
  run->AddFriend(inFile2);
  run->SetOutputFile(outFile);
  // ------------------------------------------------------------------------

  CbmRichElectronsQa* richQa   =  new CbmRichElectronsQa("Qa","qa", 0);
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

