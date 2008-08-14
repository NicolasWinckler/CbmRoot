void runTest( Int_t nfile=1)
// --------------------------------------------------------------------------
//
// Macro for standard transport simulation using UrQMD input and GEANT3
// Standard CBM setup with STS, RICH, TRD and TOF
// Does not (yet) include ECAL
//
// V. Friese   06/03/2006
//
// --------------------------------------------------------------------------

{
  // ========================================================================
  //          Adjust this part according to your requirements

  // Collision system
  TString system  = "auau";

  // Beam momentum
  TString beam    = "25gev";

  // Trigger (centrality)
  TString trigger = "mbias";

  // Number of events
  Int_t   nEvents = 5;

  TString numFile = "";
  if(nfile<10) numFile="000";
  numFile +=nfile;

  // Output file name

  //   TString outFile = "/d/cbm04/alla/zdcJun08/g3/Sts/" + system + "." + beam + "." + trigger + "." + numFile + ".root";
  TString outFile =  "zdcSts.mc.root";
  //  outFile = "fluka.root";
  //   TString outFile = "/d/cbm05/alla/zdc/g3/hole/" + system + "." + beam + "." + trigger + "." + numFile + ".root";

  // Parameter file name
  //  TString parFile = "/d/cbm04/alla/zdcJun08/g3/Sts/" + system + "." + beam + "." + trigger + "." + numFile + ".params.root";

  TString parFile = "zdcSts.params.root";

  // Cave geometry
  TString caveGeom = "cave.geo";

  // Target geometry
  TString targetGeom = "target.geo";

  // Beam pipe geometry
  TString pipeGeom = "pipe_standard.geo";

  // Magnet geometry and field map
  TString magnetGeom = "magnet_muon.geo";
 //  TString magnetGeom  = "magnet_active.geo";
  //  TString fieldMap    = "FieldActive";
 TString fieldMap    = "FieldMuonMagnet";   // name of field map
  Double_t fieldZ     = 50.;     // z position of field centre
  Double_t fieldScale = 1.;      // field scaling factor 1.

  // STS geometry
   TString stsGeom    = "sts_allstrips.geo";
   //  TString stsGeom = "sts_standard.geo";

  // RICH geometry
  TString richGeom = "rich_standard.geo";
  
  // TRD geometry
  TString trdGeom = "trd_standard.geo";

  // TOF geometry
  TString tofGeom = "tof_standard.geo";
  
  // ECAL geometry. 
  // Put here ecal_FullMC.geo for full Monte-Carlo (takes much more time!)
  TString ecalGeom = "ecal_FastMC.geo";

  // TOF geometry
  TString zdcGeom = "zdc.geo";



  // In general, the following parts need not be touched
  // ========================================================================




  // -----   Input file name   ----------------------------------------------
  //TString inFile = "/d/cbm01/cbmsim/urqmd/" + system + "/" + beam + "/"
  //+ trigger + "/urqmd." + system + "." + beam + "."
  //+ trigger + ".0000.ftn14";
   TString inFile ="/d/cbm03/urqmd/auau/"+beam+"/"+trigger+"/urqmd.auau." + beam + "."+ trigger + "."  + numFile + ".ftn14";
 
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
 gSystem->Load("libMvd");
  gSystem->Load("libSts");
  gSystem->Load("libZdc");


  // -----   Create detectors and passive volumes   -------------------------
  CbmModule* cave= new CbmCave("CAVE");
  cave->SetGeometryFileName(caveGeom);

  CbmModule* pipe= new CbmPipe("PIPE");
  pipe->SetGeometryFileName(pipeGeom);
  
  CbmModule* target= new CbmTarget("Target");
  target->SetGeometryFileName(targetGeom);

  CbmModule* magnet= new CbmMagnet("MAGNET");
  magnet->SetGeometryFileName(magnetGeom);
  
  CbmDetector* sts= new CbmSts("STS", kTRUE);
  sts->SetGeometryFileName(stsGeom);
  /*
  CbmDetector* rich= new CbmRich("RICH", kTRUE);
  rich->SetGeometryFileName(richGeom);

  CbmDetector* trd= new CbmTrd("TRD",kTRUE );
  trd->SetGeometryFileName(trdGeom);

  CbmDetector* tof= new CbmTof("TOF", kTRUE);
  tof->SetGeometryFileName(tofGeom);
  
  CbmDetector* ecal = new CbmEcal("ECAL", kTRUE, ecalGeom.Data());
*/
  CbmDetector* zdc= new CbmZdcv1("ZDC", kTRUE);
  zdc->SetGeometryFileName(zdcGeom);

  
  // ------------------------------------------------------------------------

  CbmRunSim* fRun = new CbmRunSim();


  // -----   Create magnetic field   ----------------------------------------

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
 
  // ------------------------------------------------------------------------



  // -----   Create PrimaryGenerator   --------------------------------------
  CbmPrimaryGenerator* primGen = new CbmPrimaryGenerator();
   CbmUrqmdGenerator*  urqmdGen = new CbmUrqmdGenerator(inFile);
    primGen->AddGenerator(urqmdGen);
//  CbmAsciiGenerator* asciiGen = new CbmAsciiGenerator("test_shield.f14");
//  primGen->AddGenerator(asciiGen);

  // CbmIonGenerator *fIongen= new CbmIonGenerator(79, 197,79,
    // 					1, 0.,0., 25, 0.,0.,-1.);
//  primGen->AddGenerator(fIongen);
 //
//   CbmShieldGenerator* shieldGen = new CbmShieldGenerator("/d/cbm01/alla/shield/shield_au25au_500ev_1.evt");
//    CbmShieldGenerator* shieldGen = new CbmShieldGenerator("test.evt");
  // primGen->AddGenerator(shieldGen);
    //  Particle Generator
  // CbmParticleGenerator* partGen = new CbmParticleGenerator(2212, 1, 0., 0.,  25);
  // primGen->AddGenerator(partGen);
 // ------------------------------------------------------------------------


  // -----   Create simulation run   ----------------------------------------
  //  fRun->SetName("TGeant4");              // Transport engine
  fRun->SetName("TGeant3");              // Transport engine
  //  fRun->SetName("TFluka");              // Transport engine
  fRun->SetOutputFile(outFile);          // Output file
  fRun->SetGenerator(primGen);           // PrimaryGenerator
  fRun->SetMaterials("media.geo");       // Materials
  fRun->AddModule(cave);
  fRun->AddModule(pipe);
  fRun->AddModule(target);		
  fRun->AddModule(magnet);
  fRun->AddModule(sts);
//  fRun->AddModule(rich);
//  fRun->AddModule(trd);
//  fRun->AddModule(tof);
//  fRun->AddModule(ecal);
  fRun->AddModule(zdc);
  fRun->SetField(magField);
  fRun->Init();
  // ------------------------------------------------------------------------


  
  // -----   Fill parameter containers   ------------------------------------
  CbmRuntimeDb* rtdb = fRun->GetRuntimeDb();
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

}

