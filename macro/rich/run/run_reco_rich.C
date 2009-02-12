void run_reco_rich(Int_t nEvents = 500)
{

  // ========================================================================
  //          Adjust this part according to your requirements

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;

  // Input file (MC events)
  TString inFile1 = "/d/cbm02/slebedev/rich/FEB09/real/auau.25gev.centr.0000.mc.root";
  TString inFile2 = "/d/cbm02/slebedev/rich/FEB09/real/auau.25gev.centr.0000.reco.root";
  // Parameter file
  TString parFile = "/d/cbm02/slebedev/rich/FEB09/real/auau.25gev.centr.0000.params.root";

  // STS digitisation file
  TString stsDigiFile = "sts_standard.digi.par";

  // Output file
  TString outFile = "/d/cbm02/slebedev/rich/FEB09/real/auau.25gev.centr.0000.recorich.root";

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
  // ------------------------------------------------------------------------

/*

  // ---  STS track finding   ------------------------------------------------
  CbmKF* kalman = new CbmKF();
  run->AddTask(kalman);
  CbmL1* l1 = new CbmL1();
  run->AddTask(l1);


  // =========================================================================
  // ===                        RICH reconstruction                        ===
  // =========================================================================
*/
  // ---------------------RICH Hit Producer ----------------------------------
  Double_t richPmtRad  = 0.4;     // PMT radius [cm]
  Double_t richPmtDist = 0.;      // Distance between PMTs [cm]
  Int_t    richDetType = 4;       // Detector type Hamamatsu H8500-03
  Int_t    richNoise   = 220;     // Number of noise points per event
  CbmRichHitProducer* richHitProd 
    = new CbmRichHitProducer(richPmtRad, richPmtDist, richDetType, 
			     richNoise, iVerbose);
  run->AddTask(richHitProd);
  //--------------------------------------------------------------------------
/*
       
  //----------------------RICH Track Extrapolation ---------------------------
  Int_t    richNSts = 4;     // minimum number of STS hits for extrapolation
  Double_t richZPos = 300.;  // z position for extrapolation [cm]
  CbmRichTrackExtrapolation* richExtra
    = new CbmRichTrackExtrapolationKF(richNSts, iVerbose);
  CbmRichExtrapolateTracks* richExtrapolate = new CbmRichExtrapolateTracks();
  richExtrapolate->UseExtrapolation(richExtra,richZPos);
  run->AddTask(richExtrapolate);
  //--------------------------------------------------------------------------


  //--------------------- Rich Track Projection to photodetector -------------
  Int_t richZFlag = 1;       // Projetion from IM plane (default)
  CbmRichProjectionProducer* richProj = 
  new CbmRichProjectionProducer(iVerbose, richZFlag);
  run->AddTask(richProj);
  //--------------------------------------------------------------------------
  
*/  
  //--------------------- RICH Ring Finding ----------------------------------
 // CbmL1RichENNRingFinder* richFinder = new CbmL1RichENNRingFinder(iVerbose);
  CbmRichRingFinderHough* richFinder = new CbmRichRingFinderHough(iVerbose);
  CbmRichFindRings* richFindRings = new CbmRichFindRings();
  richFindRings->UseFinder(richFinder);
  run->AddTask(richFindRings);
  //--------------------------------------------------------------------------

 
  //-------------------- RICH Ring Fitting -----------------------------------
  // B-field configuration 
  TString field ="muon";  // choose between "muon" or "active"  
  Double_t iRingCorr = 0.;      // correction done (default), choose 0 if not
  CbmRichRingFitter* richFitter = new CbmRichRingFitterEllipse(iVerbose, iRingCorr, field);
  CbmRichFitRings* fitRings = new CbmRichFitRings("","",richFitter);
  run->AddTask(fitRings);
  //--------------------------------------------------------------------------



  // ------------------- RICH Ring matching  ---------------------------------
  CbmRichMatchRings* matchRings = new CbmRichMatchRings(iVerbose);
  run->AddTask(matchRings);
  // -------------------------------------------------------------------------



  //--------------------- RICH ring-track assignment ------------------------
  Double_t richDistance = 10.; // Max. dist. ring centre to track [cm]
  Int_t    richNPoints  = 5;   // Minmum number of hits on ring
  CbmRichRingTrackAssign* richAssign   =
    new CbmRichRingTrackAssignClosestD(richDistance, richNPoints, iVerbose);
  CbmRichAssignTrack* assignTrack = new CbmRichAssignTrack();
  assignTrack->UseAssign(richAssign);
  run->AddTask(assignTrack);
  // ------------------------------------------------------------------------
  

  //--------------------- RICH ring selection -------------------------------

  TString richSelectNNFile = gSystem->Getenv("VMCWORKDIR");
  richSelectNNFile += "/parameters/rich/NeuralNet_RingSelection_Weights_Ellipse1.txt";
 // richSelectNNFile += "/parameters/rich/NeuralNet_RingSelection_Weights.txt";
  CbmRichRingSelectNeuralNet *ringSelectNN 
    = new CbmRichRingSelectNeuralNet(iVerbose, richSelectNNFile);
  CbmRichSelectRings* richSelectRingsNN = new CbmRichSelectRings();
  richSelectRingsNN->UseSelect(ringSelectNN);
  run->AddTask(richSelectRingsNN);

  CbmRichRingQa* richQa   =  new CbmRichRingQa("Qa","qa", 0);
  run->AddTask(richQa);    
  
  
  // ------------------------------------------------------------------------

  // ===                 End of RICH local reconstruction                  ===
  // =========================================================================



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
