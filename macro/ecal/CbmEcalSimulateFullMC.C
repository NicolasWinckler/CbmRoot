/* $Id: CbmEcalSimulateFullMC.C,v 1.6 2006/09/15 12:50:52 turany Exp $ */

/* History of cvs commits:
 *
 * $Log: CbmEcalSimulateFullMC.C,v $
 * Revision 1.6  2006/09/15 12:50:52  turany
 * load the base before the field
 *
 * Revision 1.5  2006/07/29 00:31:50  prokudin
 * New way of parameter files processing
 *
 * Revision 1.4  2006/06/30 13:05:24  kharlov
 * Remove tracking cut setting because it is set in CbmEcal now
 *
 * Revision 1.3  2006/06/28 14:23:53  kharlov
 * Tracking cut for ECAL media are set
 *
 * Revision 1.2  2006/06/22 07:04:52  kharlov
 * Separate scripts for fast and full MC are provided
 *
 * Revision 1.1  2006/06/12 12:03:08  kharlov
 * Adding file for ECAL Full MC simulation
 *
 * Revision 1.1  2006/04/25 08:20:51  kharlov
 * Full chain of ECAL simulation from MCpoints to Hits and analysis
 *
 */

// --------------------------------------------------------------------------
//
// Macro for standard transport simulation using UrQMD input and GEANT3
// Standard CBM setup with STS, RICH, TRD, TOF and ECAL
// ECAL is simulated with Full MC (MC points are collected at each
// energy deposition of the shower development in ECAL)
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
  TString trigger = "centr";

  // Number of events
  Int_t   nEvents = 10;

  // Output file name
  TString outFile = system + "." + beam + "." + trigger + ".mc.root";
  outFile = "test.mc.root";

  // Parameter file name
  TString parFile = "params.root";

  // Cave geometry
  TString caveGeom = "cave.geo";

  // Target geometry
  TString targetGeom = "target.geo";

  // Beam pipe geometry
  TString pipeGeom = "pipe_newvac.geo";

  // Magnet geometry and field map
  TString magnetGeom  = "magnet_active.geo";
  TString fieldMap    = "FieldActive";
  Double_t fieldZ     = 50.;     // z position of field centre
  Double_t fieldScale = 1.;      // field scaling factor

  // STS geometry
  TString stsGeom = "sts-3maps-4strips.geo";

  // RICH geometry
  TString richGeom = "rich.geo";

  // TRD geometry
  TString trdGeom = "trd_9.geo";

  // TOF geometry
  TString tofGeom = "tof.geo";

  // In general, the following parts need not be touched
  // ========================================================================




  // -----   Input file name   ----------------------------------------------
  TString inFile = "/d/cbm01/cbmsim/urqmd/" + system + "/" + beam + "/"
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
  gSystem->Load("libField");
  gSystem->Load("libGen");
  gSystem->Load("libPassive");
  gSystem->Load("libSts");
  gSystem->Load("libRich");
  gSystem->Load("libTrd");
  gSystem->Load("libTof");
  gSystem->Load("libEcal");
  // ------------------------------------------------------------------------



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

  CbmDetector* rich= new CbmRich("RICH", kTRUE);
  rich->SetGeometryFileName(richGeom);

  CbmDetector* trd= new CbmTrd("TRD",kTRUE );
  trd->SetGeometryFileName(trdGeom);

  CbmDetector* tof= new CbmTof("TOF", kTRUE );
  tof->SetGeometryFileName(tofGeom);
  
  CbmDetector* ecal= new CbmEcal("ECAL", kTRUE,"ecal_FullMC.geo");
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
  // ------------------------------------------------------------------------



  // -----   Create PrimaryGenerator   --------------------------------------
  CbmPrimaryGenerator* primGen = new CbmPrimaryGenerator();
  CbmBoxGenerator* boxGen = new CbmBoxGenerator(22, 1);
  boxGen->SetPtRange(1.,1.01);
  boxGen->SetThetaRange(4.,4.01);
  boxGen->Init();
  primGen->AddGenerator(boxGen);
  // ------------------------------------------------------------------------


  // -----   Create simulation run   ----------------------------------------
  CbmRunSim* fRun = new CbmRunSim();
  fRun->SetName("TGeant3");              // Transport engine
  fRun->SetOutputFile(outFile);          // Output file
  fRun->SetGenerator(primGen);           // PrimaryGenerator
  fRun->SetMaterials("media.geo");       // Materials
  fRun->AddModule(cave);
  fRun->AddModule(pipe);
  fRun->AddModule(target);		
  fRun->AddModule(magnet);
  fRun->AddModule(sts);
  fRun->AddModule(rich);
  fRun->AddModule(trd);
  fRun->AddModule(tof);
  fRun->AddModule(ecal);
  fRun->SetField(magField);
  fRun->Init();

  // ------------------------------------------------------------------------



  // -----   Fill parameter containers   ------------------------------------
  CbmRuntimeDb* rtdb = fRun->GetRuntimeDb();
  Bool_t kParameterMerged = kTRUE;
  CbmParRootFileIo* parOut = new CbmParRootFileIo(kParameterMerged);
  parOut->open(gFile);
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

