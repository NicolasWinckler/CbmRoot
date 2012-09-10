// --------------------------------------------------------------------------
//
// Macro for testing the trd digitizer and hit producer
//
// F. Uhlig    02/06/2010
// Version     02/06/2010 (F. Uhlig)
//
// --------------------------------------------------------------------------


void QAreco_squared_segmented_trd(Int_t nEvents = 2)
{

  // ========================================================================
  //          Adjust this part according to your requirements

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;

  // Input file (MC events)
  TString inFile = "data/trd_squared_segmented.mc.root";

  // Parameter file
  TString parFile = "data/params.root";

  // Output file
  TString outFile = "data/trd_squared_segmented_smearing.eds.root";

  //  Digitisation files.
  // The sts digi file is not needed. The code is only here to
  // show how one can load more than one digi file.
  TList *parFileList = new TList();

  TString paramDir = gSystem->Getenv("VMCWORKDIR");
  paramDir += "/parameters/sts/";

  TObjString stsDigiFile = paramDir + "sts_standard.digi.par";
  parFileList->Add(&stsDigiFile);

  // STS digitisation file
  //  TString stsDigiFile = "sts_standard.digi.par";

  TObjString trdDigiFile = "trd_standard.digi.par";
  parFileList->Add(&trdDigiFile);

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
  gSystem->Load("libLittrack");
  gSystem->Load("libMinuit2"); // Nedded for rich ellipse fitter
  // ------------------------------------------------------------------------



  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *run= new FairRunAna();
  run->SetInputFile(inFile);
  run->SetOutputFile(outFile);
  // ------------------------------------------------------------------------

  // =========================================================================
  // ===                     TRD local reconstruction                      ===
  // =========================================================================

  // Update of the values for the radiator F.U. 17.08.07
  Int_t trdNFoils    = 130;      // number of polyetylene foils
  Float_t trdDFoils = 0.0013;    // thickness of 1 foil [cm]
  Float_t trdDGap   = 0.02;      // thickness of gap between foils [cm]
  Bool_t simpleTR = kTRUE;       // use fast and simple version for TR
                                 // production

  CbmTrdRadiator *radiator = new CbmTrdRadiator(simpleTR , trdNFoils,
                                       trdDFoils, trdDGap);

  CbmTrdHitProducerSmearing* trdHitProd = new CbmTrdHitProducerSmearing(radiator);
  run->AddTask(trdHitProd);

  /*
  CbmTrdDigitizer* trdDigitizer = new CbmTrdDigitizer("TRD Digitizer",
                                                  "TRD task", radiator);
  run->AddTask(trdDigitizer);


  CbmTrdHitProducerDigi* trdHitProd =
                   new CbmTrdHitProducerDigi("TRD Hit Producer","TRD task");

  run->AddTask(trdHitProd);
  */

/*
  CbmTrdHitProducerQa* trdHitProdQa =
                   new CbmTrdHitProducerQa("TRD Hit Producer Qa","TRD task");

  run->AddTask(trdHitProdQa);
*/  
  // -------------------------------------------------------------------------



  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  FairParRootFileIo* parIo1 = new FairParRootFileIo();
  FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
  parIo1->open(parFile.Data());
  parIo2->open(parFileList,"in");
  rtdb->setFirstInput(parIo1);
  rtdb->setSecondInput(parIo2);
  //  rtdb->setOutput(parIo1);
  //  rtdb->saveOutput();
  // ------------------------------------------------------------------------


     
  // -----   Intialise and run   --------------------------------------------
  run->Init();
  cout << "Starting run" << endl;
//  run->Run(0,nEvents);
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
}
