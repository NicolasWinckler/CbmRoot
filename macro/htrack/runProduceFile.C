// --------------------------------------------------------------------------
//
// Macro for standard transport simulation using UrQMD input and GEANT3
// CBM setup with STS only
//
// C. Steinle   23/01/2007
//
// --------------------------------------------------------------------------


{

  // ========================================================================
  //          Adjust this part according to your requirements

  // Output folder for the files
  TString folder      = "files";

  // Collision system
  TString system      = "auau";

  // Beam momentum
  TString beam        = "25gev";

  // Trigger (centrality)
  TString trigger     = "centr";
  // Number of events
  Int_t   nEvents     = 1;

  // Output file name
  TString outFile     = folder + "/" + system + "." + beam + "." + trigger + ".mc.root";

  // Parameter file name
  TString parFile     = folder + "/" + system + "." + beam + "." + trigger + ".params.root";

  // Cave geometry
  TString caveGeom    = "cave.geo";

  // Target geometry
  TString targetGeom  = "target_au_250mu.geo";

  // Beam pipe geometry
  TString pipeGeom    = "pipe_standard.geo";

  // Magnet geometry and field map
  TString magnetGeom  = "magnet_standard.geo";
  TString fieldMap    = "FieldActive";
  Double_t fieldZ     = 50.;     // z position of field centre
  Double_t fieldScale = 1.;      // field scaling factor

  // STS geometry
  TString stsGeom     = "sts_standard.geo";


  // In general, the following parts need not be touched
  // ========================================================================

  // -----   Input file name   ----------------------------------------------
  TString inFile      = "/d/cbm03/urqmd/" + system + "/" + beam + "/"
                      + trigger + "/urqmd." + system + "." + beam + "."
                      + trigger + ".0000.ftn14";
  // ------------------------------------------------------------------------

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
  gSystem->Load("libSts");
  // ------------------------------------------------------------------------

  // -----   Create detectors and passive volumes   -------------------------
  FairModule* cave   = new CbmCave("CAVE");
  cave->SetGeometryFileName(caveGeom);

  FairModule* pipe   = new CbmPipe("PIPE");
  pipe->SetGeometryFileName(pipeGeom);

  FairModule* target = new CbmTarget("Target");
  target->SetGeometryFileName(targetGeom);

  FairModule* magnet = new CbmMagnet("MAGNET");
  magnet->SetGeometryFileName(magnetGeom);

  FairDetector* sts  = new CbmSts("STS", kTRUE);
  sts->SetGeometryFileName(stsGeom);
  // ------------------------------------------------------------------------

  // -----   Create magnetic field   ----------------------------------------
  if ( fieldMap == "FieldActive" || fieldMap == "FieldIron")
    CbmFieldMapSym3* magField = new CbmFieldMapSym3(fieldMap);
  else if ( fieldMap == "FieldAlligator" )
    CbmFieldMapSym2* magField = new CbmFieldMapSym2(fieldMap);
  else {
    CbmField*        magField = NULL;
    cout << "===> ERROR: Field map " << fieldMap << " unknown! " << endl;
    exit;
  }
  if (magField != NULL) {
    magField->SetPosition(0., 0., fieldZ);
    magField->SetScale(fieldScale);
  }
  // ------------------------------------------------------------------------

  // -----   Create PrimaryGenerator   --------------------------------------
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  FairUrqmdGenerator*  urqmdGen = new FairUrqmdGenerator(inFile);
  primGen->AddGenerator(urqmdGen);
  // ------------------------------------------------------------------------

  // -----   Create simulation run   ----------------------------------------
  FairRunSim* fRun = new FairRunSim();
  fRun->SetName("TGeant3");              // Transport engine
  fRun->SetOutputFile(outFile);          // Output file
  fRun->SetGenerator(primGen);           // PrimaryGenerator
  fRun->SetMaterials("media.geo");       // Materials
  fRun->AddModule(cave);
  fRun->AddModule(pipe);
  fRun->AddModule(target);
  fRun->AddModule(magnet);
  fRun->AddModule(sts);
  fRun->SetField(magField);
  fRun->Init();
  // ------------------------------------------------------------------------

  // -----   Fill parameter containers   ------------------------------------
  FairRuntimeDb* rtdb       = fRun->GetRuntimeDb();
  if (magField != NULL) {
    CbmFieldPar* fieldPar  = (CbmFieldPar*) rtdb->getContainer("CbmFieldPar");
    fieldPar->SetParameters(magField);
    fieldPar->setChanged();
  }
  Bool_t kParameterMerged  = kTRUE;
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

}
