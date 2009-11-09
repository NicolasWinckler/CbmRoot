void eloss_sim(Int_t nEvents = 200000)
{
	TString dir = "/home/d/andrey/ms_sim/1X01GeViron/";
	TString outFile = dir + "eloss.sim.root";
	// TString parFile = dir + "eloss.params.root";

	TString caveGeom   = "cave.geo";
	TString muchGeom   = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/littrack/qa/simple_geo.geo");

	TStopwatch timer;
	timer.Start();

	gSystem->Load("/home/soft/tbb22_004oss/libtbb");

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
	cbmrootlibs();

	FairRunSim* fRun = new FairRunSim();
	fRun->SetName("TGeant3");             // Transport engine
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

	Double_t minMom = 1.; //minimum momentum
	Double_t maxMom = 1.; //maximum momentum

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
	cout << endl << endl;
	cout << "Macro finished successfully." << endl;
	cout << "Output file is " << outFile << endl;
	cout << "Real time used: " << timer.RealTime() << "s " << endl;
	cout << "CPU time used : " << timer.CpuTime() << "s " << endl;
	cout << endl << endl;
	// ------------------------------------------------------------------------
}

