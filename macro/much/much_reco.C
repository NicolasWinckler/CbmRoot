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

  // Input file (MC events)
  TString inFile = "Jpsi.auau.25gev.mbias.mc.root";

  // Number of events to process
  Int_t nEvents = 1;

  // Background file (for MAPS pileup)
  TString bgFile = "Jpsi.auau.25gev.mbias.mc.root";

  // Parameter file
  TString parFile = "Jpsi.auau.25gev.mbias.params.root";

  // Sts reco file
  TString stsRecoFile = "Jpsi.auau.25gev.mbias.stsreco.root";

  // Output file
  TString outFile = "Jpsi.auau.25gev.mbias.reco.root";


  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libField");
  gSystem->Load("libGen");
  gSystem->Load("libPassive");
  gSystem->Load("libSts");
  gSystem->Load("libRich");
  gSystem->Load("libTrd");
  gSystem->Load("libTof");
  //gSystem->Load("libEcal");
  gSystem->Load("libMuch");
  gSystem->Load("libGlobal");
  gSystem->Load("libKF");
  gSystem->Load("libL1");
  // ------------------------------------------------------------------------


  // ---  Now choose concrete engines for the different tasks   -------------
  CbmKF* kalman= new CbmKF();
  // ------------------------------------------------------------------------


  // In general, the following parts need not be touched
  // ========================================================================



  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------



  // -----   Reconstruction run   -------------------------------------------
  CbmRunAna *fRun= new CbmRunAna();
  fRun->SetInputFile(inFile);
  fRun->AddFriend(stsRecoFile);
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------



  // -----  Parameter database   --------------------------------------------
  CbmRuntimeDb* rtdb = fRun->GetRuntimeDb();
  CbmParRootFileIo* parInput1 = new CbmParRootFileIo();
  parInput1->open(parFile.Data());
  CbmParAsciiFileIo* parInput2 = new CbmParAsciiFileIo();
  TString stsDigiFile = gSystem->Getenv("VMCWORKDIR");
  stsDigiFile += "/parameters/sts/sts_digi.par";
  parInput2->open(stsDigiFile.Data(),"in");
  rtdb->setFirstInput(parInput1);
  rtdb->setSecondInput(parInput2);
  fRun->LoadGeometry();
  // ------------------------------------------------------------------------


  fRun->AddTask(kalman);
 
  CbmMuchHitProducer *MuchHitProducer = new CbmMuchHitProducer();
  fRun->AddTask(MuchHitProducer);

  CbmL1MuchFinder *MuchFinder = new CbmL1MuchFinder();
  fRun->AddTask(MuchFinder);
 
  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run();//0,nEvents);
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
 
