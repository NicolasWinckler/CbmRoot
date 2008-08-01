
void eloss_sim(Int_t nEvents = 100000)
{
	
  TString engine = "geant3/";
  TString dir = "/d/cbm02/andrey/events/eloss/" + engine + "1-10gev_20cm/";

  TString outFile = dir + "eloss.sim.root";                      
 // TString parFile = dir + "eloss.params.root";  
  
  TString caveGeom   = "cave.geo";
  TString muchGeom   = "simple_geo.geo";

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

  if ( muchGeom != "" ) {
    CbmDetector* much = new CbmMuch("MUCH", kTRUE);
    much->SetGeometryFileName(muchGeom);
    fRun->AddModule(much);
    cout << "    --- " << muchGeom << endl;
  }

  CbmFieldMap* magField = NULL;
  fRun->SetField(magField);
  
  CbmPrimaryGenerator* primGen = new CbmPrimaryGenerator();
  
  Double_t minMom = 0.1.; //minimum momentum
  Double_t maxMom = 10.; //maximum momentum
  
  CbmBoxGenerator* boxGen1 = new CbmBoxGenerator(13, 1);
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
  cout << "=== much_sim.C : Macro finished successfully." << endl;
  cout << "=== much_sim.C : Output file is " << outFile << endl;
  cout << "=== much_sim.C : Real time used: " << rtime << "s " << endl;
  cout << "=== much_sim.C : CPU time used : " << ctime << "s " << endl;
  cout << endl << endl;
    // ------------------------------------------------------------------------

}

