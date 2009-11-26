void run_sim_geotest(Int_t nEvents = 200)
{

  TString outDir  = "/d/cbm06/user/slebedev/geotest/";
  TString outFile = outDir + "mc.00.root";
  TString parFile = outDir + "params.00.root";

  // -----  Geometries  -----------------------------------------------------
  TString caveGeom   = "cave.geo";
  TString targetGeom = "target_au_250mu.geo";
  TString pipeGeom   = "pipe_standard.geo";
  TString magnetGeom = "magnet_standard.geo";
  TString stsGeom    = "sts_standard.geo";
  TString richGeom   = "rich_standard.geo";

  // -----   Magnetic field   -----------------------------------------------
  TString fieldMap    = "FieldMuonMagnet";   // name of field map
  Double_t fieldZ     = 50.;             // field centre z position
  Double_t fieldScale =  1.;             // field scaling factor


  gDebug = 0;

  TStopwatch timer;
  timer.Start();

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gROOT->LoadMacro("$VMCWORKDIR/macro/rich/cbmlibs.C");
  cbmlibs();



  // -----   Create simulation run   ----------------------------------------
  FairRunSim* fRun = new FairRunSim();
  fRun->SetName("TGeant3");              // Transport engine
  fRun->SetOutputFile(outFile);          // Output file
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  // ------------------------------------------------------------------------

  // -----   Create media   -------------------------------------------------
  fRun->SetMaterials("media.geo");       // Materials
  // ------------------------------------------------------------------------
  //fRun->SetStoreTraj(kTRUE);

  // -----   Create detectors and passive volumes   -------------------------
  if ( caveGeom != "" ) {
    FairModule* cave = new CbmCave("CAVE");
    cave->SetGeometryFileName(caveGeom);
    fRun->AddModule(cave);
  }

  if ( pipeGeom != "" ) {
    FairModule* pipe = new CbmPipe("PIPE");
    pipe->SetGeometryFileName(pipeGeom);
    fRun->AddModule(pipe);
  }

  if ( targetGeom != "" ) {
    FairModule* target = new CbmTarget("Target");
    target->SetGeometryFileName(targetGeom);
    fRun->AddModule(target);
  }

  if ( magnetGeom != "" ) {
    FairModule* magnet = new CbmMagnet("MAGNET");
    magnet->SetGeometryFileName(magnetGeom);
    fRun->AddModule(magnet);
  }

  if ( stsGeom != "" ) {
    FairDetector* sts = new CbmSts("STS", kTRUE);
    sts->SetGeometryFileName(stsGeom);
    fRun->AddModule(sts);
  }

  if ( richGeom != "" ) {
    FairDetector* rich = new CbmRich("RICH", kTRUE);
    rich->SetGeometryFileName(richGeom);
    fRun->AddModule(rich);
  }

  // -----   Create magnetic field   ----------------------------------------
  CbmFieldMap* magField = NULL;
  if ( fieldMap == "FieldActive" || fieldMap == "FieldIron")
    magField = new CbmFieldMapSym3(fieldMap);
  else if ( fieldMap == "FieldAlligator" )
    magField = new CbmFieldMapSym2(fieldMap);
  else if ( fieldMap = "FieldMuonMagnet" )
    magField = new CbmFieldMapSym3(fieldMap);
  else {
    cout << "===> ERROR: Field map " << fieldMap << " unknown! " << endl;
    exit;
  }
  magField->SetPosition(0., 0., fieldZ);
  magField->SetScale(fieldScale);
  fRun->SetField(magField);
  // ------------------------------------------------------------------------



  // -----   Create PrimaryGenerator   --------------------------------------
  CbmPrimaryGenerator* primGen = new CbmPrimaryGenerator();

  // -----   Create BoxGenerator   --------------------------------------
  Int_t kfCode1=11;   // electrons
  Int_t kfCode2=-11;   // positrons

  FairBoxGenerator* boxGen1 = new FairBoxGenerator(kfCode1, 25);
  boxGen1->SetPtRange(0.,3.);
  boxGen1->SetPhiRange(0.,360.);
  boxGen1->SetThetaRange(2.5,25.);
  boxGen1->SetCosTheta();
  boxGen1->Init();
  primGen->AddGenerator(boxGen1);

  FairBoxGenerator* boxGen2 = new FairBoxGenerator(kfCode2, 25);
  boxGen2->SetPtRange(0.,3.);
  boxGen2->SetPhiRange(0.,360.);
  boxGen2->SetThetaRange(2.5,25.);
  boxGen2->SetCosTheta();
  boxGen2->Init();
  primGen->AddGenerator(boxGen2);

  fRun->SetGenerator(primGen);

  // -----   Run initialisation   -------------------------------------------
  fRun->Init();
  // ------------------------------------------------------------------------


  // -----   Runtime database   ---------------------------------------------
  CbmFieldPar* fieldPar = (CbmFieldPar*) rtdb->getContainer("CbmFieldPar");
  fieldPar->SetParameters(magField);
  fieldPar->setChanged();
  fieldPar->setInputVersion(fRun->GetRunId(),1);
  Bool_t kParameterMerged = kTRUE;
  FairParRootFileIo* parOut = new FairParRootFileIo(kParameterMerged);
  parOut->open(parFile.Data());
  rtdb->setOutput(parOut);
  rtdb->saveOutput();
  rtdb->print();
  // ------------------------------------------------------------------------


  // -----   Start run   ----------------------------------------------------
  fRun->Run(nEvents);
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
  exit(0);
}

