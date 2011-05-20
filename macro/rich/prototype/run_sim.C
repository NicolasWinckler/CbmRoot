void run_sim(Int_t nEvents = 2)
{
  // ----- Paths and file names  --------------------------------------------
  TString outDir  = "/d/cbm02/kresan/rich_prot/may11";
  TString outFile = outDir + "/epi.standard.mc.root";
  TString parFile = outDir + "/params.epi.standard.root";
  //TString outFile = outDir + "/epi.standard.eve.mc.root";
  //TString parFile = outDir + "/params.epi.standard.eve.root";


  // -----  Geometries  -----------------------------------------------------
  TString caveGeom   = "cave.geo";
  TString richGeom   = "rich_prototype_standard.geo";
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
  // -----------------------------------------------------------------------


  // -----   Create simulation run   ----------------------------------------
  FairRunSim* fRun = new FairRunSim();
  fRun->SetName("TGeant3");              // Transport engine
  fRun->SetOutputFile(outFile);          // Output file
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  // ------------------------------------------------------------------------


  // -----   Create media   -------------------------------------------------
  fRun->SetMaterials("media.geo");       // Materials
  // ------------------------------------------------------------------------


  // -----   Create detectors and passive volumes   -------------------------
  if ( caveGeom != "" ) {
    FairModule* cave = new CbmCave("CAVE");
    cave->SetGeometryFileName(caveGeom);
    fRun->AddModule(cave);
  }
  if ( richGeom != "" ) {
    FairDetector* rich = new CbmRichProt("RICH", kTRUE);
    rich->SetGeometryFileName(richGeom);
    fRun->AddModule(rich);
  }
  // ------------------------------------------------------------------------


  // -----   Create PrimaryGenerator   --------------------------------------
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  FairAsciiGenerator* asciiGen =
      new FairAsciiGenerator("/d/cbm02/kresan/rich_prot/may11/epi.CERNPST9.dat");
  primGen->AddGenerator(asciiGen);
  fRun->SetGenerator(primGen);
  // ------------------------------------------------------------------------


  //fRun->SetStoreTraj(kTRUE);


  // -----   Run initialisation   -------------------------------------------
  fRun->Init();
  // ------------------------------------------------------------------------


  //((CbmStack*)FairMCApplication::Instance()->GetStack())->SetMinPoints(0);

  /*
  FairTrajFilter* trajFilter = FairTrajFilter::Instance();
  trajFilter->SetStepSizeCut(0.01); // 1 cm
  trajFilter->SetVertexCut(-2000., -2000., -2000., 2000., 2000., 2000.);
  trajFilter->SetMomentumCutP(0.); // p_lab > 0
  trajFilter->SetEnergyCut(0., 10.); // 0 < Etot < 10 GeV
  trajFilter->SetStorePrimaries(kTRUE);
  trajFilter->SetStoreSecondaries(kTRUE);
  */

  // -----   Runtime database   ---------------------------------------------
  Bool_t kParameterMerged = kTRUE;
  FairParRootFileIo* parOut = new FairParRootFileIo(kParameterMerged);
  parOut->open(parFile.Data());
  rtdb->setOutput(parOut);
  rtdb->saveOutput();
  rtdb->print();
  // ------------------------------------------------------------------------


  // -----   Start run   ----------------------------------------------------
  fRun->Run(nEvents);
  fRun->CreateGeometryFile("geofile.rich.prot.standard.root");
  // ------------------------------------------------------------------------


  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime 
       << "s" << endl << endl;
  // ------------------------------------------------------------------------


  cout << " Test passed" << endl;
  cout << " All ok " << endl;
}

