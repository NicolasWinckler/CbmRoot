/*  Description: This macro runs simulation of  
electrons and pions using BoxGenerator

 Author : Simeon Lebedev
 E-mail : S.Lebedev@gsi.de
 */

void trd_el_id_sim(Int_t nEvents = 1000)
{

  // ========================================================================
  //          Adjust this part according to your requirements
  
  // ----- Paths and file names  --------------------------------------------
  TString inDir   = "/d/cbm03/urqmd/auau/25gev/centr";
  TString inFile  = inDir + "/urqmd.auau.25gev.centr.0000.ftn14";
  TString outDir  = "/d/cbm02/slebedev/trd/MAY08/";
  TString outFile = outDir + "mom.piel.0000.mc.root";
  TString parFile = outDir + "mom.piel.0000.params.root";
  
  // -----  Geometries  -----------------------------------------------------
  TString caveGeom   = "cave.geo";
  TString targetGeom = "target.geo";
  TString pipeGeom   = "pipe_standard.geo";
  TString magnetGeom = "magnet_active.geo";
  TString mvdGeom    = "mvd_standard.geo";
  TString stsGeom    = "sts_standard.geo";
  TString richGeom   = "rich_standard.geo";
  TString trdGeom    = "trd_standard.geo";
  //TString tofGeom    = "tof_standard.geo";
  //TString ecalGeom   = "ecal_FastMC.geo";
  
  // -----   Magnetic field   -----------------------------------------------
  TString fieldMap    = "FieldMuonMagnet";   // name of field map
  Double_t fieldZ     = 50.;             // field centre z position
  Double_t fieldScale =  1.;             // field scaling factor
  
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
  gSystem->Load("libRich");
  gSystem->Load("libTrd");
 // gSystem->Load("libTof");
 // gSystem->Load("libEcal");
  // -----------------------------------------------------------------------

 
 
  // -----   Create simulation run   ----------------------------------------
  CbmRunSim* fRun = new CbmRunSim();
  fRun->SetName("TGeant3");              // Transport engine
  fRun->SetOutputFile(outFile);          // Output file
  CbmRuntimeDb* rtdb = fRun->GetRuntimeDb();
  // ------------------------------------------------------------------------


  // -----   Create media   -------------------------------------------------
  fRun->SetMaterials("media.geo");       // Materials
  // ------------------------------------------------------------------------


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
/*
  if ( tofGeom != "" ) {
    CbmDetector* tof = new CbmTof("TOF", kTRUE);
    tof->SetGeometryFileName(tofGeom);
    fRun->AddModule(tof);
  }
  
  if ( ecalGeom != "" ) {
    CbmDetector* ecal = new CbmEcal("ECAL", kTRUE, ecalGeom.Data()); 
    fRun->AddModule(ecal);
  }*/
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
  //CbmUrqmdGenerator*  urqmdGen = new CbmUrqmdGenerator(inFile);
 // primGen->AddGenerator(urqmdGen);
     
  // ------------------------------------------------------------------------
  Int_t kfCode1=11;   // electrons
  Int_t kfCode2=-11;   // positrons
  Double_t minMomentum = 1.; //minimum momentum
  Double_t maxMomentum = 10.; //maximum momentum
  
  CbmBoxGenerator* boxGen1 = new CbmBoxGenerator(11, 20);
  boxGen1->SetPRange(minMomentum, maxMomentum);
  boxGen1->SetPhiRange(0.,360.);
  boxGen1->SetThetaRange(2.5,25.);
  boxGen1->SetCosTheta();    
  boxGen1->Init();
  primGen->AddGenerator(boxGen1);
  
  CbmBoxGenerator* boxGen2 = new CbmBoxGenerator(-11, 20);
  boxGen2->SetPRange(minMomentum, maxMomentum);
  boxGen2->SetPhiRange(0.,360.);
  boxGen2->SetThetaRange(2.5,25.);
  boxGen2->SetCosTheta();    
  boxGen2->Init();
  primGen->AddGenerator(boxGen2);
    
  CbmBoxGenerator* boxGen3 = new CbmBoxGenerator(211, 20);
  boxGen3->SetPRange(minMomentum, maxMomentum);
  boxGen3->SetPhiRange(0.,360.);
  boxGen3->SetThetaRange(2.5,25.);
  boxGen3->SetCosTheta();    
  boxGen3->Init();
  primGen->AddGenerator(boxGen3);
  
  CbmBoxGenerator* boxGen4 = new CbmBoxGenerator(-211, 20);
  boxGen4->SetPRange(minMomentum, maxMomentum);
  boxGen4->SetPhiRange(0.,360.);
  boxGen4->SetThetaRange(2.5,25.);
  boxGen4->SetCosTheta();    
  boxGen4->Init();
  primGen->AddGenerator(boxGen4);

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

  exit(0);
}

