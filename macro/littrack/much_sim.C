void much_sim(Int_t nEvents = 5)
{
  TString dir = "/home/d/andrey/events/newmuch/standard/10mu/mu_urqmd/";

  TString outFile = dir + "mc.root";
  TString parFile = dir + "params.root";

  TString inFileUrqmd = "/home/d/urqmd/auau/25gev/centr/urqmd.auau.25gev.centr.0007.ftn14";
//  TString plutoFile = "/u/andrey/cbm/much/pluto/omega/25gev/omega.0000.root";

  TString muchGeom   = "much_standard.geo";
//  TString muchGeom   = "much_straw_new.geo";
//  TString pipeGeom   = "pipe_standard.geo";//
  TString pipeGeom   = "pipe_much.geo";
  TString shieldGeom = "shield_standard.geo";

  TString caveGeom   = "cave.geo";
  TString targetGeom = "target_au_250mu.geo";
  TString magnetGeom = "magnet_standard.geo";
  TString stsGeom    = "sts_Standard_s3055AAFK5.SecD.geo";
//  TString stsGeom    = "sts_allstrips.geo";
  TString trdGeom    = "trd_standard.geo";
  TString tofGeom    = "tof_standard.geo";

  // -----   Magnetic field   -----------------------------------------------
  TString  fieldMap   = "FieldMuonMagnet";   // name of field map
  Double_t fieldZ     = 50.;                 // field centre z position
  Double_t fieldScale =  1.;                 // field scaling factor

  TStopwatch timer;
  timer.Start();

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
  cbmrootlibs();

  FairRunSim* fRun = new FairRunSim();
  fRun->SetName("TGeant3");              // Transport engine
  fRun->SetOutputFile(outFile);          // Output file
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();

  fRun->SetMaterials("media.geo");       // Materials

  if ( caveGeom != "" ) {
    FairModule* cave = new CbmCave("CAVE");
    cave->SetGeometryFileName(caveGeom);
    fRun->AddModule(cave);
    cout << "    --- " << caveGeom << endl;
  }

  if ( pipeGeom != "" ) {
    FairModule* pipe = new CbmPipe("PIPE");
    pipe->SetGeometryFileName(pipeGeom);
    fRun->AddModule(pipe);
    cout << "    --- " << pipeGeom << endl;
  }

   if ( shieldGeom != "" ) {
    FairModule* shield = new CbmShield("SHIELD");
    shield->SetGeometryFileName(shieldGeom);
    fRun->AddModule(shield);
    cout << "    --- " << shieldGeom << endl;
  }

  if ( targetGeom != "" ) {
    FairModule* target = new CbmTarget("Target");
    target->SetGeometryFileName(targetGeom);
    fRun->AddModule(target);
    cout << "    --- " << targetGeom << endl;
  }

  if ( magnetGeom != "" ) {
    FairModule* magnet = new CbmMagnet("MAGNET");
    magnet->SetGeometryFileName(magnetGeom);
    fRun->AddModule(magnet);
    cout << "    --- " << magnetGeom << endl;
  }

  if ( stsGeom != "" ) {
    FairDetector* sts = new CbmSts("STS", kTRUE);
    sts->SetGeometryFileName(stsGeom);
    fRun->AddModule(sts);
    cout << "    --- " << stsGeom << endl;
  }

  if ( muchGeom != "" ) {
    FairDetector* much = new CbmMuch("MUCH", kTRUE);
    much->SetGeometryFileName(muchGeom);
    fRun->AddModule(much);
    cout << "    --- " << muchGeom << endl;
  }

  if ( trdGeom != "" ) {
    FairDetector* trd = new CbmTrd("TRD",kTRUE );
    trd->SetGeometryFileName(trdGeom);
    fRun->AddModule(trd);
    cout << "    --- " << trdGeom << endl;
  }

  if ( tofGeom != "" ) {
    FairDetector* tof = new CbmTof("TOF", kTRUE);
    tof->SetGeometryFileName(tofGeom);
    fRun->AddModule(tof);
    cout << "    --- " << tofGeom << endl;
  }

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


  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();

//  FairPlutoGenerator *plutoGen= new FairPlutoGenerator(plutoFile);
//  primGen->AddGenerator(plutoGen);

  FairUrqmdGenerator*  urqmdGen = new FairUrqmdGenerator(inFileUrqmd);
  primGen->AddGenerator(urqmdGen);

  Double_t minMom = 2.5; //minimum momentum
  Double_t maxMom = 25.; //maximum momentum

  FairBoxGenerator* boxGen1 = new FairBoxGenerator(13, 5);
  boxGen1->SetPRange(minMom, maxMom);
  boxGen1->SetPhiRange(0.,360.);
  boxGen1->SetThetaRange(2.5, 25.);
  boxGen1->SetCosTheta();
  boxGen1->Init();
  primGen->AddGenerator(boxGen1);

  FairBoxGenerator* boxGen2 = new FairBoxGenerator(-13, 5);
  boxGen2->SetPRange(minMom, maxMom);
  boxGen2->SetPhiRange(0.,360.);
  boxGen2->SetThetaRange(2.5, 25.);
  boxGen2->SetCosTheta();
  boxGen2->Init();
  primGen->AddGenerator(boxGen2);

  fRun->SetGenerator(primGen);

  fRun->Init();


  // -----   Runtime database   ---------------------------------------------
  CbmFieldPar* fieldPar = (CbmFieldPar*) rtdb->getContainer("CbmFieldPar");
  fieldPar->SetParameters(magField);
  fieldPar->setChanged();
  fieldPar->setInputVersion(fRun->GetRunId(),1);
  Bool_t kParameterMerged = kTRUE;
  FairParRootFileIo* parOut = new FairParRootFileIo(kParameterMerged);
  parOut->open(parFile.Data());//gFile);
  rtdb->setOutput(parOut);
  rtdb->saveOutput();
  rtdb->print();
  // ------------------------------------------------------------------------

  // -----   Start run   ----------------------------------------------------
  cout << endl << "=== much_sim.C : Start run ..." << endl;
  fRun->Run(nEvents);
  // ------------------------------------------------------------------------

  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "=== much_sim.C : Macro finished successfully." << endl;
  cout << "=== much_sim.C : Output file is " << outFile << endl;
  cout << "=== much_sim.C : Real time used: " << rtime << "s " << endl;
  cout << "=== much_sim.C : CPU time used : " << ctime << "s " << endl;
  cout << endl << endl;
    // ------------------------------------------------------------------------

}

