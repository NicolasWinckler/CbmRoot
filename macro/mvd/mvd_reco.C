// --------------------------------------------------------------------------
//
// Macro for local MVD reconstruction from MC data
//
// Tasks:  CbmMvdDigitiser
//         CbmMvdFindHits
// 
//
// V. Friese   23/04/2009
//
// --------------------------------------------------------------------------


{

  // ========================================================================
  //          Adjust this part according to your requirements

   
  // Input file (MC events)
  TString inFile = "data/mvd.mc.root";
  
  // Background file (MC events, for pile-up)
  TString bgFile = "data/mvd_bg.mc.root";

  // Output file
  TString outFile = "data/mvd.reco.root";

  // Parameter file
  TString parFile = "data/params.root";
 
  // Number of events to process
  Int_t nEvents = 3;

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;
 
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
  gSystem->Load("libCLHEP");
  // ------------------------------------------------------------------------



  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------
  
  
  // -----   MVD Digitiser   ------------------------------------------------
  FairTask* mvdDigitise = new CbmMvdDigitiser("MVD Digitiser", 0, iVerbose);
  fRun->AddTask(mvdDigitise);
  // ------------------------------------------------------------------------

  
  // -----   MVD Hit Finder   -----------------------------------------------
  FairTask* mvdFindHits = new CbmMvdFindHits("MVD Hit Finder", 0, iVerbose);
  fRun->AddTask(mvdFindHits);
  // ------------------------------------------------------------------------
  

  
  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo*  parIo = new FairParRootFileIo();
  parIo->open(parFile.Data());
  rtdb->setFirstInput(parIo);
  rtdb->setOutput(parIo);
  rtdb->saveOutput();
  rtdb->print();
  fRun->LoadGeometry();
  // ------------------------------------------------------------------------

 
  // -----   Run initialisation   -------------------------------------------
  fRun->LoadGeometry();
  fRun->Init();
  // ------------------------------------------------------------------------

  
     
  // -----   Start run   ----------------------------------------------------
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
