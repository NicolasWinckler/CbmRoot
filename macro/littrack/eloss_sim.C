void eloss_sim(Int_t nEvents = 200000)
{
  TString engine = "geant4/";
  TString dir = "/d/cbm02/andrey/events/eloss/" + engine + "10gev1X0iron/";

  TString outFile = dir + "eloss.sim.root";
 // TString parFile = dir + "eloss.params.root";

  TString caveGeom   = "cave.geo";
  TString muchGeom   = "simple_geo.geo";

  gDebug = 0;

  TStopwatch timer;
  timer.Start();

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
  cbmrootlibs();

  FairRunSim* fRun = new FairRunSim();
  fRun->SetName("TGeant4");              // Transport engine
  fRun->SetOutputFile(outFile);          // Output file
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();

  fRun->SetMaterials("media.geo");       // Materials

  if ( caveGeom != "" ) {
    FairModule* cave = new CbmCave("CAVE");
    cave->SetGeometryFileName(caveGeom);
    fRun->AddModule(cave);
    cout << "    --- " << caveGeom << endl;
  }

  if ( muchGeom != "" ) {
    FairDetector* much = new CbmMuch("MUCH", kTRUE);
    much->SetGeometryFileName(muchGeom);
    fRun->AddModule(much);
    cout << "    --- " << muchGeom << endl;
  }

  CbmFieldMap* magField = NULL;
  fRun->SetField(magField);

  CbmPrimaryGenerator* primGen = new CbmPrimaryGenerator();

  Double_t minMom = 10.; //minimum momentum
  Double_t maxMom = 10.; //maximum momentum

  FairBoxGenerator* boxGen1 = new FairBoxGenerator(13, 1);
  boxGen1->SetPRange(minMom, maxMom);
  boxGen1->SetPhiRange(0.,360.);
  boxGen1->SetThetaRange(0.,0.);
  boxGen1->SetCosTheta();
  boxGen1->Init();
  primGen->AddGenerator(boxGen1);

  fRun->SetGenerator(primGen);

  fRun->Init();
  fRun->Run(nEvents);

  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished successfully." << endl;
  cout << "Output file is " << outFile << endl;
  cout << "Real time used: " << timer.RealTime() << "s " << endl;
  cout << "CPU time used : " << timer.CpuTime() << "s " << endl;
  cout << endl << endl;
    // ------------------------------------------------------------------------
}

