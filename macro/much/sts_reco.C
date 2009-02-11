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

  // Output file
  TString outFile = "Jpsi.auau.25gev.mbias.stsreco.root";

  // Version of STS digitisation scheme
  TString stsDigiVersion = "v05a";

  // Specify MAPS digitisation parameters
  Int_t    iMapsF        = 1;           // First MAPS station
  Int_t    iMapsL        = 2;           // Last MAPS station
  Double_t mapsSigmaX    = 0.0005;      // resolution in x [cm]
  Double_t mapsSigmaY    = 0.0005;      // resolution in y [cm]
  Double_t mapsEff       = 0.99;        // single hit efficiency
  Double_t mapsFakeRate  = 0.03;        // fake hit rate 
  Double_t mapsMergeDist = 0.00;        // distance for hit merging [cm]
  Int_t    mapsPileUp    = 1;           // number of pileup events
  
  // Specify Hybrid digitisation parameters
  Int_t    iHybF   = 3;           // First hybrid station
  Int_t    iHybL   = 5;           // Last hybrid station
  Double_t hybLx   = 0.0050;      // Pixel x size [cm]
  Double_t hybLy   = 0.0050;      // Pixel y size [cm]

  // Specify Strip digitisation parameters
  Int_t iStripF = 6;           // First strip station
  Int_t iStripL = 7;           // Last strip station

  // Specify TRD digitisation parameters
  Double_t trdSigmaX[] = {300, 400, 500}; // Resolution in x [mum]
  // Resolutions in y - station and angle dependent [mum]
  Double_t trdSigmaY1[] = {2700,   3700, 15000, 27600, 33000, 33000, 33000 };
  Double_t trdSigmaY2[] = {6300,   8300, 33000, 33000, 33000, 33000, 33000 };
  Double_t trdSigmaY3[] = {10300, 15000, 33000, 33000, 33000, 33000, 33000 };  
  Int_t trdNFoils    = 100;   // number of polyetylene foils,  default = 100
  Float_t trdDFoils = 0.001; // thickness of 1 foil [cm], default = 0.001
  Float_t trdDGap   = 0.008; // thickness of gap between foils [cm], default = 0.008


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
  CbmL1 *l1 = new CbmL1();
  CbmStsTrackFinder* stsTrackFinder    = new CbmStsTrackFinderIdeal();
  //CbmStsTrackFinder* stsTrackFinder    = new CbmL1StsTrackFinder();
  CbmStsTrackFitter* stsTrackFitter    = new CbmStsKFTrackFitter();
  CbmPrimaryVertexFinder* pvFinder     = new CbmPVFinderKF();
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
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------



  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());
  FairParAsciiFileIo* parInput2 = new FairParAsciiFileIo();
  TString stsDigiFile = gSystem->Getenv("VMCWORKDIR");
  stsDigiFile += "/parameters/sts/sts_digi.par";
  parInput2->open(stsDigiFile.Data(),"in");
  rtdb->setFirstInput(parInput1);
  rtdb->setSecondInput(parInput2);
  fRun->LoadGeometry();
  // ------------------------------------------------------------------------



  // -----   STS hit producers   --------------------------------------------
  CbmStsMapsHitProducer* mapsHitProd =
    new CbmStsMapsHitProducer(iMapsF, iMapsL, mapsSigmaX, mapsSigmaY,
			      mapsEff, mapsFakeRate, mapsMergeDist,
			      mapsPileUp, stsDigiVersion);
  mapsHitProd->SetPileupFileName(bgFile);

  CbmStsHybridHitProducer* hybHitprod =
    new CbmStsHybridHitProducer(iHybF, iHybL, hybLx, hybLy, stsDigiVersion);

  CbmStsStripHitProducer* stripHitProd =
    new CbmStsStripHitProducer(iStripF, iStripL, stsDigiVersion);

  fRun->AddTask(mapsHitProd);
  fRun->AddTask(hybHitprod);
  fRun->AddTask(stripHitProd);
  // ------------------------------------------------------------------------


  // -----   STS track finding   --------------------------------------------
  CbmStsFindTracks* stsFindTracks = new CbmStsFindTracks("Track Finder",
							 "FairTask",
							 stsTrackFinder);
  fRun->AddTask(kalman);
  //fRun->AddTask(l1);
  fRun->AddTask(stsFindTracks);
  // ------------------------------------------------------------------------


  // -----   STS track matching   -------------------------------------------
  CbmStsMatchTracks* stsMatchTracks = new CbmStsMatchTracks("Match tracks",
  							    "STS",
							    iVerbose);
  fRun->AddTask(stsMatchTracks);
  // ------------------------------------------------------------------------


  // -----   STS track fitting   --------------------------------------------
  CbmStsFitTracks* stsFitTracks = new CbmStsFitTracks("STS Track Fitter",
  						      "FairTask",
  						      stsTrackFitter);
  fRun->AddTask(stsFitTracks);
  // ------------------------------------------------------------------------


  // -----   Primary vertex finding   ---------------------------------------
  CbmFindPrimaryVertex* pvFindTask = new CbmFindPrimaryVertex(pvFinder);
  fRun->AddTask(pvFindTask);
  // ------------------------------------------------------------------------

  
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
