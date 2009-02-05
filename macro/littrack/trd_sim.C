void trd_sim(Int_t nEvents = 10000)
{
  TString inDir   = "/d/cbm03/urqmd/auau/25gev/centr/";
  TString inFile  = inDir + "urqmd.auau.25gev.centr.0000.ftn14";
  TString outDir  = "/d/cbm02/andrey/events/trd/segmented/e/";
  TString outFile = outDir + "mc.root";
  TString parFile = outDir + "params.root";

  // -----  Geometries  -----------------------------------------------------
  TString caveGeom   = "cave.geo";
  TString targetGeom = "target_au_250mu.geo";
  TString pipeGeom   = "pipe_standard.geo";
  TString magnetGeom = "magnet_standard.geo";
  TString mvdGeom    = "mvd_standard.geo";
  TString stsGeom    = "sts_standard.geo";
  TString richGeom   = "rich_standard.geo";
  TString trdGeom    = "trd_standard.geo";
  TString tofGeom    = "tof_standard.geo";
  TString ecalGeom   = "";//"ecal_FastMC.geo";

  // -----   Magnetic field   -----------------------------------------------
  TString fieldMap    = "FieldMuonMagnet";   // name of field map
  Double_t fieldZ     = 50.;             // field centre z position
  Double_t fieldScale =  1.;             // field scaling factor

  TStopwatch timer;
  timer.Start();

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
  cbmrootlibs();


  CbmRunSim* fRun = new CbmRunSim();
  fRun->SetName("TGeant3");              // Transport engine
  fRun->SetOutputFile(outFile);          // Output file
  CbmRuntimeDb* rtdb = fRun->GetRuntimeDb();

  fRun->SetMaterials("media.geo");       // Materials
 // fRun->SetStoreTraj(kTRUE);

  // -----   Create detectors and passive volumes   -------------------------
  if ( caveGeom != "" ) {
    CbmModule* cave = new CbmCave("CAVE");
    cave->SetGeometryFileName(caveGeom);
    fRun->AddModule(cave);
  }

  if ( pipeGeom != "" ) {
    CbmModule* pipe = new CbmPipe("PIPE");
    pipe->SetGeometryFileName(pipeGeom);
    fRun->AddModule(pipe);
  }

  if ( targetGeom != "" ) {
    CbmModule* target = new CbmTarget("Target");
    target->SetGeometryFileName(targetGeom);
    fRun->AddModule(target);
  }

  if ( magnetGeom != "" ) {
    CbmModule* magnet = new CbmMagnet("MAGNET");
    magnet->SetGeometryFileName(magnetGeom);
    fRun->AddModule(magnet);
  }

  if ( mvdGeom != "" ) {
    CbmDetector* mvd = new CbmMvd("MVD", kTRUE);
    mvd->SetGeometryFileName(mvdGeom);
    fRun->AddModule(mvd);
  }

  if ( stsGeom != "" ) {
    CbmDetector* sts = new CbmSts("STS", kTRUE);
    sts->SetGeometryFileName(stsGeom);
    fRun->AddModule(sts);
  }

  if ( richGeom != "" ) {
    CbmDetector* rich = new CbmRich("RICH", kTRUE);
    rich->SetGeometryFileName(richGeom);
    fRun->AddModule(rich);
  }

  if ( trdGeom != "" ) {
    CbmDetector* trd = new CbmTrd("TRD",kTRUE );
    trd->SetGeometryFileName(trdGeom);
    fRun->AddModule(trd);
  }

  if ( tofGeom != "" ) {
    CbmDetector* tof = new CbmTof("TOF", kTRUE);
    tof->SetGeometryFileName(tofGeom);
    fRun->AddModule(tof);
  }

  if ( ecalGeom != "" ) {
    CbmDetector* ecal = new CbmEcal("ECAL", kTRUE, ecalGeom.Data());
    fRun->AddModule(ecal);
  }
  // ------------------------------------------------------------------------



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
//  CbmUrqmdGenerator*  urqmdGen = new CbmUrqmdGenerator(inFile);
//  primGen->AddGenerator(urqmdGen);

 // ------------------------------------------------------------------------


// -----   Create BoxGenerator   --------------------------------------
  Int_t kfCode1=11;   // electrons
  Int_t kfCode2=-11;   // positrons

  CbmBoxGenerator* boxGen1 = new CbmBoxGenerator(kfCode1, 5);
  boxGen1->SetPtRange(0.,3.);
  boxGen1->SetPhiRange(0.,360.);
  boxGen1->SetThetaRange(2.5,25.);
  boxGen1->SetCosTheta();
  boxGen1->Init();
  primGen->AddGenerator(boxGen1);

  CbmBoxGenerator* boxGen2 = new CbmBoxGenerator(kfCode2, 5);
  boxGen2->SetPtRange(0.,3.);
  boxGen2->SetPhiRange(0.,360.);
  boxGen2->SetThetaRange(2.5,25.);
  boxGen2->SetCosTheta();
  boxGen2->Init();
  primGen->AddGenerator(boxGen2);

  // ------------------------------------------------------------------------
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
  CbmParRootFileIo* parOut = new CbmParRootFileIo(kParameterMerged);
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

