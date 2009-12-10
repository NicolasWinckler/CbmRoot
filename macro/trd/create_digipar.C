// --------------------------------------------------------------------------
//
// Macro for reconstruction of simulated events with standard settings
//
// HitProducers in MVD, RICH, TRD, TOF, ECAL
// Digitizer and HitFinder in STS
// FAST MC for ECAL
// STS track finding and fitting (L1 / KF)
// TRD track finding and fitting (L1 / KF)
// RICH ring finding (ideal) and fitting
// Global track finding (ideal), rich assignment
// Primary vertex finding (ideal)
// Matching of reconstructed and MC tracks in STS, RICH and TRD
//
// V. Friese   24/02/2006
// Version     24/04/2007 (V. Friese)
//
// --------------------------------------------------------------------------


void create_digipar(Int_t nEvents = 0)
{

  // ========================================================================
  //          Adjust this part according to your requirements

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;

  // Input file (MC events)
  TString inFile = "data/test.mc.root";

  // Parameter file
  //  TString parFile = "data/params.root";

  // STS digitisation file
//  TString stsDigiFile = "sts_standard.digi.par";
//  TString stsDigiFile = "sts_Standard_s3055AAFK5.SecD.digi.par";
  // Output file
  TString outFile = "data/test.eds.root";

  // Geometry File
  TString geoFile="geofile.root";

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
  run->SetInputFile(inFile);
  run->SetOutputFile(outFile);
  run->SetGeomFile(geoFile);
  // ------------------------------------------------------------------------

  FairRuntimeDb* rtdb = run->GetRuntimeDb();

  /*
  FairParRootFileIo* parIo1 = new FairParRootFileIo();
  parIo1->open(parFile.Data(),"UPDATE");
  rtdb->setFirstInput(parIo1);
  */

  FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
  parIo2->open("./trd.digi.par","out");
  rtdb->setOutput(parIo2);

  /*
  CbmFieldPar* fieldPar = (CbmFieldPar*) rtdb->getContainer("CbmFieldPar");
  fieldPar->SetParameters(magField);
  fieldPar->setChanged();
  */


  CbmTrdCreateDigiPar* trdDigitizer = new CbmTrdCreateDigiPar("TRD Digitizer",
                                                        "TRD task");
  run->AddTask(trdDigitizer);
  // -------------------------------------------------------------------------



  // -----  Parameter database   --------------------------------------------
  //  TString stsDigi = gSystem->Getenv("VMCWORKDIR");
  //  stsDigi += "/parameters/sts/";
  //  stsDigi += stsDigiFile;
 

  rtdb->saveOutput();
 
  // ------------------------------------------------------------------------


     
  // -----   Intialise and run   --------------------------------------------
  run->LoadGeometry();
  run->Init();
   
  rtdb->print();
  //  rtdb->getContainer("CbmTrdDigiPar")->print();

  

  CbmTrdDigiPar* DigiPar = (CbmTrdDigiPar*)
                           rtdb->getContainer("CbmTrdDigiPar");

  DigiPar->setChanged();
  DigiPar->setInputVersion(run->GetRunId(),1);
  rtdb->print();
  rtdb->saveOutput();
  //  parIo2->write(DigiPar);
  //parIo2->close();
			   

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
  exit(0);
}
