{

  TStopwatch timer;
  timer.Start();

  gROOT->LoadMacro("/$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libBase");
  gSystem->Load("libSts");
  gSystem->Load("libPassive");
  gSystem->Load("libGen");
  gSystem->Load("libTof");
  gSystem->Load("libRich");
  gSystem->Load("libTrd");
  gSystem->Load("libEcal");
  gSystem->Load("libGlobal");
  gSystem->Load("libKF");
  gSystem->Load("libL1");

  char *inFile  = "test.mc.root";
  char *bgFile  = "test.mc.root";
  char *parFile = "test.mc.param.root";

  char *outFile = "test.stsTracks.root";
  char *version = "v05a";

  // -----   Run   ----------------------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);

  // Init Simulation Parameters from Root File
  FairRuntimeDb* rtdb=fRun->GetRuntimeDb();
  FairParRootFileIo* input=new FairParRootFileIo();
  input->open(parFile);

  // Init Digitization Parameters from Ascii File
  FairParAsciiFileIo* input2 = new FairParAsciiFileIo();
  TString StsDigiFile(gSystem->Getenv("VMCWORKDIR"));
  StsDigiFile = StsDigiFile + "/parameters/sts/sts_digi.par";
  input2->open(StsDigiFile.Data(),"in");

  rtdb->setFirstInput(input);
  rtdb->setSecondInput(input2);

  cout<<" LoadGeometry starting..."<<endl;
  fRun->LoadGeometry();
  cout<<"LoadGeometry ok"<<endl;
  cout << " - I TGeoManager " << gGeoManager << endl;

  // -----   MAPS hit producer   --------------------------------------------
  Double_t sigmaX  = 0.0005;      // resolution in x (5mum)
  Double_t sigmaY  = 0.0005;      // resolution in y (5mum)
  Double_t eff     = 0.99;        // single hit efficiency 0.99
  Double_t fakes   = 0.03;        // fake hit rate 0.03
  Double_t dist    = 0.00;        // distance for hit merging 0.00
//  Int_t    nPileUp = 30;           // number of pileup events
  Int_t    nPileUp = 1;           // number of pileup events

  CbmStsMapsHitProducer* mapsHitProd
    = new CbmStsMapsHitProducer(1, 2, sigmaX, sigmaY, eff, fakes, dist,
				nPileUp, version);
  mapsHitProd->SetPileupFileName(bgFile);

  // -----   Hybrid hit producer   ------------------------------------------
   Double_t lx = 0.0050;    // Pixel x size
   Double_t ly = 0.0050;    // Pixel y size
   CbmStsHybridHitProducer* hybHitProd     
//   = new CbmStsHybridHitProducer(3, 7, lx, ly, version);
     = new CbmStsHybridHitProducer(3, 5, lx, ly, version);

  // Since these parameters are the default ones, this is equivalent to
  // CbmHybridHitProducer(3, 3)
  // ------------------------------------------------------------------------


  // -----   Strip hit producer   -------------------------------------------
   CbmStsStripHitProducer* stripHitProd
     = new CbmStsStripHitProducer(6, 7, version);
  // ------------------------------------------------------------------------

  fRun->AddTask(mapsHitProd);
  fRun->AddTask(hybHitProd);
  fRun->AddTask(stripHitProd);

  // Kalman Filter

  CbmKF *KF = new CbmKF();
  fRun->AddTask(KF);

  // L1

  CbmL1 *L1 = new CbmL1();
  fRun->AddTask(L1);

  //******************* track finder ***************
 
  //CbmStsTrackFinderIdeal* trackFinder = new CbmStsTrackFinderIdeal();
  CbmL1StsTrackFinder* trackFinder = new CbmL1StsTrackFinder();
  
  CbmStsFindTracks* findTask = new CbmStsFindTracks("finderTask","FairTask",trackFinder);
  fRun->AddTask(findTask);


  // ------------ STS track matching -----------------
  CbmStsMatchTracks* matchStsTracks =
      new CbmStsMatchTracks("StsMatchTracks",
                            "StsMatchTracks",
                            1);
  fRun->AddTask(matchStsTracks);
  // -------------------------------------------------


  //****************** track fitter *************

  CbmStsKFTrackFitter* kalman = new CbmStsKFTrackFitter;
  CbmStsFitTracks *fitTask = new CbmStsFitTracks("Kalman fitter","FairTask",kalman);
  fRun->AddTask(fitTask);

  //****************** vertex finder *************

  CbmKFPrimaryVertexFinder *vfinder = new CbmKFPrimaryVertexFinder;
  CbmFindPrimaryVertex *vTask = new CbmFindPrimaryVertex("Kalman PV finder", "FairTask", vfinder);
  fRun->AddTask(vTask);

  //***************** performance finder *********

  //CbmStsFinderPerformanceTask *findperf = new CbmStsFinderPerformanceTask("FinderPerformance");  //findperf->SetNEventsCalculateRmin(1000);
  //fRun->AddTask(findperf);

  //***************** performance fitter ********
  
  CbmStsFitPerformanceTask *fitperf = new CbmStsFitPerformanceTask("FitPerformance");
  fitperf->DoVertexAnalysis(1);
  fRun->AddTask(fitperf);
  
  //******************

  cout << " INITIALISATION " << endl;

  fRun->Init();
  fRun->Run();

  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);

  //findperf->Write();
  fitperf->Write();
}
