
void much_sim(Int_t nEvents = 1500)
{
  
  TString system  = "auau";  
  TString beam    = "25gev";  
  TString trigger = "centr";
  TString particle = "mu";  
   
  //TString directory = "/d/cbm02/andrey/events/much/compact/signal/";
  //TString dir = "/d/cbm02/andrey/events/much/10stations/new/";
  //TString dir = "/d/cbm02/andrey/events/much/10stations/new/signal/";
  //TString dir = "/d/cbm02/andrey/events/much/10stations/new/wofield/signal/";
  TString dir = "/d/cbm02/andrey/events/much/10stations/10mu/urqmd/";
  
  
  TString outFile = dir + beam + "/" + particle + "/mc." + system + "." + beam + "." 
                    + particle + "." + trigger + ".root";                      
                    
  TString parFile = dir + beam + "/" + particle + "/params." + system + "." + beam + "." 
                    + particle + "." + trigger + ".root";                     
                    
  TString inFile = "/d/cbm03/urqmd/" + system + "/" + beam + "/"
                   + trigger + "/urqmd." + system + "." + beam + "."
                   + trigger+ ".0001.ftn14";                   
                 
//  TString plutoFile = "/u/andrey/cbm/much/pluto/" 
//                      + particle + "/"  + beam + "/"
//                      + particle + "." + "0000" + ".root";  
   
  
  //TString muchGeom   = "much_standard.geo";
 // TString muchGeom   = "../much/geometry/much_compact.geo";
 TString muchGeom   = "../much/geometry/much_compact.2.geo";
  TString pipeGeom   = "pipe_much.geo";
  TString shieldGeom = "shield_standard.geo";
  
  // -----   Other geometries   ---------------------------------------------
  TString caveGeom   = "cave.geo";
  TString targetGeom = "target.geo";
  TString magnetGeom = "magnet_muon.geo";
  TString stsGeom    = "sts_allstrips.geo";
  TString trdGeom    = "trd_standard.geo";
  TString tofGeom    = "tof_standard.geo";
  
  // -----   Magnetic field   -----------------------------------------------
  TString  fieldMap   = "FieldMuonMagnet";   // name of field map
  Double_t fieldZ     = 50.;                 // field centre z position
  Double_t fieldScale =  1.;                 // field scaling factor

  gDebug = 0;

  TStopwatch timer;
  timer.Start();

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
  gSystem->Load("libTof");
  gSystem->Load("libEcal");
  gSystem->Load("libGlobal");
  gSystem->Load("libKF");
  gSystem->Load("libL1");
  gSystem->Load("libMuch");
  gSystem->Load("libLittrack"); 


  CbmRunSim* fRun = new CbmRunSim();
  fRun->SetName("TGeant3");              // Transport engine
  fRun->SetOutputFile(outFile);          // Output file
  CbmRuntimeDb* rtdb = fRun->GetRuntimeDb();

  fRun->SetMaterials("media.geo");       // Materials

  if ( caveGeom != "" ) {
    CbmModule* cave = new CbmCave("CAVE");
    cave->SetGeometryFileName(caveGeom);
    fRun->AddModule(cave);
    cout << "    --- " << caveGeom << endl;
  }

  if ( pipeGeom != "" ) {
    CbmModule* pipe = new CbmPipe("PIPE");
    pipe->SetGeometryFileName(pipeGeom);
    fRun->AddModule(pipe);
    cout << "    --- " << pipeGeom << endl;
  }
  
   if ( shieldGeom != "" ) {
    CbmModule* shield = new CbmShield("SHIELD");
    shield->SetGeometryFileName(shieldGeom);
    fRun->AddModule(shield);
    cout << "    --- " << shieldGeom << endl;
  }
      
  if ( targetGeom != "" ) {
    CbmModule* target = new CbmTarget("Target");
    target->SetGeometryFileName(targetGeom);
    fRun->AddModule(target);
    cout << "    --- " << targetGeom << endl;
  }

  if ( magnetGeom != "" ) {
    CbmModule* magnet = new CbmMagnet("MAGNET");
    magnet->SetGeometryFileName(magnetGeom);
    fRun->AddModule(magnet);
    cout << "    --- " << magnetGeom << endl;
  }
  
  if ( stsGeom != "" ) {
    CbmDetector* sts = new CbmSts("STS", kTRUE);
    sts->SetGeometryFileName(stsGeom);
    fRun->AddModule(sts);
    cout << "    --- " << stsGeom << endl;
  }

  if ( muchGeom != "" ) {
    CbmDetector* much = new CbmMuch("MUCH", kTRUE);
    much->SetGeometryFileName(muchGeom);
    fRun->AddModule(much);
    cout << "    --- " << muchGeom << endl;
  }

  if ( trdGeom != "" ) {
    CbmDetector* trd = new CbmTrd("TRD",kTRUE );
    trd->SetGeometryFileName(trdGeom);
    fRun->AddModule(trd);
    cout << "    --- " << trdGeom << endl;
  }

  if ( tofGeom != "" ) {
    CbmDetector* tof = new CbmTof("TOF", kTRUE);
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

  
  CbmPrimaryGenerator* primGen = new CbmPrimaryGenerator();
  
//  CbmPlutoGenerator *plutoGen= new CbmPlutoGenerator(plutoFile);
//  primGen->AddGenerator(plutoGen);
    
  CbmUrqmdGenerator*  urqmdGen = new CbmUrqmdGenerator(inFile);
  primGen->AddGenerator(urqmdGen);
  
//  Double_t minMom = 1.0; //minimum momentum
//  Double_t maxMom = 10.; //maximum momentum
//  
//  CbmBoxGenerator* boxGen1 = new CbmBoxGenerator(13, 5);
//  boxGen1->SetPRange(minMom, maxMom);
//  boxGen1->SetPhiRange(0.,360.);
//  boxGen1->SetThetaRange(2.5, 25.);
//  boxGen1->SetCosTheta();    
//  boxGen1->Init();
//  primGen->AddGenerator(boxGen1);
//  
//  CbmBoxGenerator* boxGen2 = new CbmBoxGenerator(-13, 5);
//  boxGen2->SetPRange(minMom, maxMom);
//  boxGen2->SetPhiRange(0.,360.);
//  boxGen2->SetThetaRange(2.5, 25.);
//  boxGen2->SetCosTheta();    
//  boxGen2->Init();
//  primGen->AddGenerator(boxGen2);
  
  fRun->SetGenerator(primGen);       
 
  fRun->Init();
 
   
  // -----   Runtime database   ---------------------------------------------
  cout << endl << "=== much_sim.C : Set up database ..." << endl;
  cout << "                Parameters will be saved to output file" << endl;
  CbmFieldPar* fieldPar = (CbmFieldPar*) rtdb->getContainer("CbmFieldPar");
  fieldPar->SetParameters(magField);
  fieldPar->setChanged();
  fieldPar->setInputVersion(fRun->GetRunId(),1);
  Bool_t kParameterMerged = kTRUE;
  CbmParRootFileIo* parOut = new CbmParRootFileIo(kParameterMerged);
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

