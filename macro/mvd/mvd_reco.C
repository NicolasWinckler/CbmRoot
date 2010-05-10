// --------------------------------------------------------------------------
//
// Macro for local MVD reconstruction from MC data
//
// Tasks:  CbmMvdDigitiser
//         CbmMvdFindHits
// 
// Alternative: CbmHitProducer (direct production of MvdHits from MvdPoints).
//              Is obsolete but kept for reference.
// 
//
// V. Friese   23/04/2009
//
// --------------------------------------------------------------------------


void mvd_reco(Int_t  nEvents = 3,
              Int_t  iVerbose = 0, 
              Bool_t useDigitisation = kTRUE)
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
  FairRunAna *run= new FairRunAna();
  run->SetInputFile(inFile);
  run->SetOutputFile(outFile);
  // ------------------------------------------------------------------------
  
  
  // ***** Case: Digitisation and hit finding *******************************
  if ( useDigitisation ) {
  
    // -----   MVD Digitiser   ----------------------------------------------
    FairTask* mvdDigitise = new CbmMvdDigitizeL("MVD Digitiser", 0, iVerbose);
    run->AddTask(mvdDigitise);
    // ----------------------------------------------------------------------

    // -----   MVD Hit Finder   ---------------------------------------------
    FairTask* mvdFindHits = new CbmMvdFindHits("MVD Hit Finder", 0, iVerbose);
    run->AddTask(mvdFindHits);
    // ----------------------------------------------------------------------
    
  }
  
  // ***** Case: HitProducer ************************************************
  else {
  
    // -----   MVD HitProducer   --------------------------------------------
    FairTask* mvdHitProd = new CbmMvdHitProducer("MVD HitProducer", 0,
                                                 iVerbose);
    run->AddTask(mvdHitProd);
    // ----------------------------------------------------------------------
    
  }
  
  

  
  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  FairParRootFileIo*  parIo = new FairParRootFileIo();
  parIo->open(parFile.Data());
  rtdb->setFirstInput(parIo);
  rtdb->setOutput(parIo);
  rtdb->saveOutput();
  rtdb->print();
  // ------------------------------------------------------------------------

 
  // -----   Run initialisation   -------------------------------------------
  run->LoadGeometry();
  run->Init();
  // ------------------------------------------------------------------------

  
     
  // -----   Start run   ----------------------------------------------------
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


}
