// --------------------------------------------------------------------------
//
// Macro for standard transport simulation using UrQMD input and GEANT3
// CBM setup with STS only
//
// V. Friese   14/09/2006
//
// --------------------------------------------------------------------------

void sts_sim(Int_t nEvents = 1)
{
  // ========================================================================
  //          Adjust this part according to your requirements

  // Input file
  TString inPath = "/d/cbm03/urqmd/auau/25gev/centr/";
  TString inFile = inPath + "urqmd.auau.25gev.centr.0000.ftn14";
  
  // Output file
  TString outFile = "sts.mc.root";
  
  // Parameter file
  TString parFile = "params.root";

  // Cave geometry
  TString caveGeom = "cave.geo";
  
  // Target geometry
  TString targetGeom = "target.geo";

  // Beam pipe geometry
  TString pipeGeom = "pipe_standard.geo";

  // Magnet geometry and field map
  TString magnetGeom  = "magnet_active.geo";
  TString fieldMap    = "FieldActive";
  Double_t fieldZ     = 50.;     // z position of field centre
  Double_t fieldScale = 1.;      // field scaling factor

  // MVD geometry
  TString mvdGeom = "mvd_standard.geo";

  // STS geometry
  TString stsGeom = "sts_standard.geo";

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
  gSystem->Load("libField");
  gSystem->Load("libGen");
  gSystem->Load("libPassive");
  gSystem->Load("libMvd");
  gSystem->Load("libSts");
  // ------------------------------------------------------------------------

   
 
  // -----   Create simulation run   ----------------------------------------
  CbmRunSim* run = new CbmRunSim();
  run->SetName("TGeant3");              // Transport engine
  run->SetOutputFile(outFile);          // Output file
  CbmRuntimeDb* rtdb = run->GetRuntimeDb();
  // ------------------------------------------------------------------------
  
  

  // -----   Create media   -------------------------------------------------
  run->SetMaterials("media.geo");       // Materials
  // ------------------------------------------------------------------------

    
  
  // -----   Create geometry   ----------------------------------------------
  CbmModule* cave= new CbmCave("CAVE");
  cave->SetGeometryFileName(caveGeom);
  run->AddModule(cave);

  CbmModule* pipe= new CbmPipe("PIPE");
  pipe->SetGeometryFileName(pipeGeom);
  run->AddModule(pipe);
  
  CbmModule* target= new CbmTarget("Target");
  target->SetGeometryFileName(targetGeom);
  run->AddModule(target);

  CbmModule* magnet= new CbmMagnet("MAGNET");
  magnet->SetGeometryFileName(magnetGeom);
  run->AddModule(magnet);
  
  CbmDetector* mvd= new CbmMvd("MVD", kTRUE);
  mvd->SetGeometryFileName(mvdGeom); 
  run->AddModule(mvd);

  CbmDetector* sts= new CbmSts("STS", kTRUE);
  sts->SetGeometryFileName(stsGeom); 
  run->AddModule(sts);
  // ------------------------------------------------------------------------

  
  
  // -----   Create magnetic field   ----------------------------------------
  if ( fieldMap == "FieldActive" || fieldMap == "FieldIron")
    magField = new CbmFieldMapSym3(fieldMap);
  else if ( fieldMap == "FieldAlligator" )
    magField = new CbmFieldMapSym2(fieldMap);
  else {
    cout << "===> ERROR: Field map " << fieldMap << " unknown! " << endl;
    exit;
  }
  magField->SetPosition(0., 0., fieldZ);
  magField->SetScale(fieldScale);
  run->SetField(magField);
  // ------------------------------------------------------------------------

  
  
  // -----   Create PrimaryGenerator   --------------------------------------
  CbmPrimaryGenerator* primGen = new CbmPrimaryGenerator();
  CbmUrqmdGenerator*  urqmdGen = new CbmUrqmdGenerator(inFile);
  primGen->AddGenerator(urqmdGen);
  run->SetGenerator(primGen);
  // ------------------------------------------------------------------------


  
  // -----   Initialize simulation run   ------------------------------------
  run->Init();
  // ------------------------------------------------------------------------

  
  
  // -----   Runtime database   ---------------------------------------------
  CbmFieldPar* fieldPar = (CbmFieldPar*) rtdb->getContainer("CbmFieldPar");
  fieldPar->SetParameters(magField);
  fieldPar->setChanged();
  fieldPar->setInputVersion(run->GetRunId(),1);
  Bool_t kParameterMerged = kTRUE;
  CbmParRootFileIo* parOut = new CbmParRootFileIo(kParameterMerged);
  parOut->open(parFile.Data());
  rtdb->setOutput(parOut);
  rtdb->saveOutput();
  rtdb->print();
  // ------------------------------------------------------------------------

  
   
  // -----   Start run   ----------------------------------------------------
  run->Run(nEvents);
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

