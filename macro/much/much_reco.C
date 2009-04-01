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
  TString inFile = "data/Jpsi.auau.25gev.centr.mc.root";

  // Number of events to process
  Int_t nEvents = 2;

  // Sts reco file
  TString stsRecoFile = "data/Jpsi.auau.25gev.centr.stsreco.root";

  // Much hits file
  TString muchHitsFile = "data/Jpsi.auau.25gev.centr.muchhits.root";

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
  gSystem->Load("liblittrack");
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
  fRun->AddFriend(stsRecoFile);
  fRun->AddFriend(muchHitsFile);
  fRun->SetOutputFile(outFile);




  // ------------------------------------------------------------------------



  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(gFile);
  rtdb->setFirstInput(parInput1);
  fRun->LoadGeometry();
  // ------------------------------------------------------------------------

  CbmMuchTrackFinder* muchTrackFinder = new CbmLitMuchTrackFinderBranch();
  CbmMuchFindTracks* muchFindTracks = new CbmMuchFindTracks("Much Track Finder");
  muchFindTracks->UseFinder(muchTrackFinder);
  fRun->AddTask(muchFindTracks);

//  CbmKF* kalman= new CbmKF();
//  fRun->AddTask(kalman);
//  CbmL1MuchFinder *MuchFinder = new CbmL1MuchFinder();
//  fRun->AddTask(MuchFinder);
 
  CbmMuchMatchTracks* muchMatchTracks = new CbmMuchMatchTracks();
  fRun->AddTask(muchMatchTracks);

  CbmLitRecQa* muchRecQa = new CbmLitRecQa(12, 0.7, kMUCH, 1);
  muchRecQa->SetNormType(2); // '2' to number of STS tracks
  fRun->AddTask(muchRecQa);
  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
//  fRun->Run(0,1);
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

}
 
