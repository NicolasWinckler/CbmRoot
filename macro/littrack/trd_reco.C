



void trd_reco(Int_t nEvents = 100)
{

  // ========================================================================
  //          Adjust this part according to your requirements

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 1;
  
  TString dir = "/d/cbm02/andrey/events/trd/25gev";
  
  TString inFile = dir + "/trd.444.mc.root"; 
  TString inFile1 = dir + "/trd.444.sts.reco.root"; 
  TString parFile = dir + "/trd.444.params.root"; 
  

  // Output file
  TString outFile = dir + "/trd.444.reco.root";

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
  gSystem->Load("libLittrack");  
  // ------------------------------------------------------------------------



  // -----   Reconstruction run   -------------------------------------------
  CbmRunAna *run= new CbmRunAna();
  run->SetInputFile(inFile);
  run->AddFriend(inFile1);
  run->SetOutputFile(outFile);
  // ------------------------------------------------------------------------
  
 // CbmKF* kalman = new CbmKF();
//  run->AddTask(kalman);
 // CbmL1* l1 = new CbmL1();
//  run->AddTask(l1);

  // =========================================================================
  // ===                     TRD local reconstruction                      ===
  // =========================================================================

  // -----   TRD hit producer   ----------------------------------------------
  Double_t trdSigmaX[] = {300, 400, 500};             // Resolution in x [mum]
  // Resolutions in y - station and angle dependent [mum]
  Double_t trdSigmaY1[] = {2700,   3700, 15000, 27600, 33000, 33000, 33000 };
  Double_t trdSigmaY2[] = {6300,   8300, 33000, 33000, 33000, 33000, 33000 };
  Double_t trdSigmaY3[] = {10300, 15000, 33000, 33000, 33000, 33000, 33000 };  

  // Update of the values for the radiator F.U. 17.08.07
  Int_t trdNFoils    = 130;       // number of polyetylene foils
  Float_t trdDFoils = 0.0013;    // thickness of 1 foil [cm]
  Float_t trdDGap   = 0.02;      // thickness of gap between foils [cm]
  CbmTrdHitProducer* trdHitProd = new CbmTrdHitProducer("TRD Hitproducer",
                                                        "TRD task");
  trdHitProd->SetPar(trdNFoils, trdDFoils, trdDGap);
  trdHitProd->SetSigmaX(trdSigmaX);
  trdHitProd->SetSigmaY(trdSigmaY1, trdSigmaY2, trdSigmaY3);
  run->AddTask(trdHitProd);
  // -------------------------------------------------------------------------


  // -----   TRD track finding   ---------------------------------------------
  //CbmTrdTrackFinder* trdTrackFinder    = new CbmL1TrdTrackFinderSts();
  //((CbmL1TrdTrackFinderSts*)trdTrackFinder)->SetVerbose(iVerbose);
  CbmTrdTrackFinder* trdTrackFinder    = new CbmLitTrdTrackFinderSts();
  //((CbmLitTrdTrackFinderStsNew*)trdTrackFinder)->SetVerbose(3);
  CbmTrdFindTracks* trdFindTracks = new CbmTrdFindTracks("TRD Track Finder");
  trdFindTracks->UseFinder(trdTrackFinder);
  run->AddTask(trdFindTracks);
  // -------------------------------------------------------------------------

  // -----   TRD track matching   --------------------------------------------
  CbmTrdMatchTracks* trdMatchTracks = new CbmTrdMatchTracks(iVerbose);
  run->AddTask(trdMatchTracks);
  
    // -----   TRD track finding QA check   ------------------------------------
  CbmLitTrdRecQa* trdRecQa = new CbmLitTrdRecQa(12, 0.7);
  trdRecQa->SetNormType(2); // '2' to number of STS tracks
  run->AddTask(trdRecQa);
  


  // -----  Parameter database   --------------------------------------------
 // TString stsDigi = gSystem->Getenv("VMCWORKDIR");
//  stsDigi += "/parameters/sts/";
 // stsDigi += stsDigiFile;
  CbmRuntimeDb* rtdb = run->GetRuntimeDb();
  CbmParRootFileIo* parIo1 = new CbmParRootFileIo();
  CbmParAsciiFileIo* parIo2 = new CbmParAsciiFileIo();
  parIo1->open(parFile.Data());
//  parIo2->open(stsDigi.Data(),"in");
  rtdb->setFirstInput(parIo1);
//  rtdb->setSecondInput(parIo2);
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
