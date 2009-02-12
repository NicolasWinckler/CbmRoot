// --------------------------------------------------------------------------
//
// Macro for reconstruction of simulated events 
// in the CBM muon setup: 
//
// MUCH digitization
// MUCH hit production
//
// To be defined as arguments:
//    MC transport file
//    Input digitization file ("*.root")
//    Output file
//    Number of events to be processed (default = 1)
//    First event to be processed (default = 1)
// 
// M.Ryzhinskiy
//
// --------------------------------------------------------------------------

void much_hits(const char* inFile, const char* digiFile, const char* outFile,
	      Int_t nEvents = 10, Int_t iFirst = 1)
{
  // ========================================================================
  //          Adjust this part according to your requirements

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 1;

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
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------


  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo*  parIo1 = new FairParRootFileIo();
  parIo1->open(gFile);
  rtdb->setFirstInput(parIo1);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();
  fRun->LoadGeometry();
  // ------------------------------------------------------------------------

  // ---  MuCh digitizer ----------------------------------------------------
  CbmMuchDigitize* muchDigitize = new CbmMuchDigitize("MuchDigitize", digiFile, iVerbose);
  muchDigitize->SetUseAvalanche(1); // Account for avalanches
  fRun->AddTask(muchDigitize);
  // ------------------------------------------------------------------------

  // ---  MuCh hit finder ---------------------------------------------------
  CbmMuchFindHits* muchFindHits = new CbmMuchFindHits("MuchFindHits", digiFile, iVerbose);
  muchFindHits->SetUseClustering(1); // Use clustering algorithm
  fRun->AddTask(muchFindHits);
  // ------------------------------------------------------------------------ 
  
  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(iFirst,nEvents);
  // ------------------------------------------------------------------------
}
