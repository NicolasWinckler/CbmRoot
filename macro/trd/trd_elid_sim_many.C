
void trd_elid_sim_many(Int_t fileNum, Int_t trdGeoType)
{
	 Int_t nEvents = 100;

    TString fileNumSt, trdGeomFile;

    Double_t momentum;
    if (fileNum == 0) fileNumSt = "0";
    if (fileNum == 1) fileNumSt = "1";
    if (fileNum == 2) fileNumSt = "2";
    if (fileNum == 3) fileNumSt = "3";
    if (fileNum == 4) fileNumSt = "4";
    if (fileNum == 5) fileNumSt = "5";
    if (fileNum == 6) fileNumSt = "6";
    if (fileNum == 7) fileNumSt = "7";
    if (fileNum == 8) fileNumSt = "8";
    if (fileNum == 9) fileNumSt = "9";

    if (fileNum == 0) momentum = 1.0;
    if (fileNum == 1) momentum = 1.5;
    if (fileNum == 2) momentum = 2.0;
    if (fileNum == 3) momentum = 3.0;
    if (fileNum == 4) momentum = 4.0;
    if (fileNum == 5) momentum = 5.0;
    if (fileNum == 6) momentum = 7.0;
    if (fileNum == 7) momentum = 9.0;
    if (fileNum == 8) momentum = 11.0;
    if (fileNum == 9) momentum = 13.0;

    if (trdGeoType == 1) trdGeomFile = "trd_monolithic.geo";
    if (trdGeoType == 2) trdGeomFile = "trd_monolithic_MB.geo";

    TString outDir;
	if (trdGeoType == 1) outDir = "/d/cbm02/slebedev/trd/JUL09/st/";
	if (trdGeoType == 2) outDir = "/d/cbm02/slebedev/trd/JUL09/mb/";

	TString outFile = outDir + "piel.000" + fileNumSt + ".mc.root";
	TString parFile = outDir + "piel.000" + fileNumSt + ".params.root";

	// -----  Geometries  -----------------------------------------------------
	TString caveGeom = "cave.geo";
	TString targetGeom = "target_au_250mu.geo";
	TString pipeGeom = "pipe_standard.geo";
	TString magnetGeom = "magnet_standard.geo";
	TString mvdGeom = "mvd_standard.geo";
	mvdGeom = "";
	TString stsGeom = "sts_standard.geo";
	TString richGeom = "rich_standard.geo";
	TString trdGeom = trdGeomFile;

// -----   Magnetic field   -----------------------------------------------
	TString fieldMap = "FieldMuonMagnet"; // name of field map
	Double_t fieldZ = 50.; // field center z position
	Double_t fieldScale = 1.; // field scaling factor

    gDebug = 0;

	TStopwatch timer;
	timer.Start();

// ----  Load libraries   -------------------------------------------------
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gSystem->Load("libGeoBase");
	gSystem->Load("libParBase");
	gSystem->Load("libBase");
	gSystem->Load("libCbmBase");
	gSystem->Load("libCbmData");
	gSystem->Load("libField");
	gSystem->Load("libGen");
	gSystem->Load("libPassive");
	gSystem->Load("libMvd");
	gSystem->Load("libSts");
	gSystem->Load("libRich");
	gSystem->Load("libTrd");
// -----------------------------------------------------------------------

// -----   Create simulation run   ----------------------------------------
	FairRunSim* fRun = new FairRunSim();
	fRun->SetName("TGeant3"); // Transport engine
	fRun->SetOutputFile(outFile); // Output file
	FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
// ------------------------------------------------------------------------


// -----   Create media   -------------------------------------------------
	fRun->SetMaterials("media.geo"); // Materials
// ------------------------------------------------------------------------

// -----   Create detectors and passive volumes   -------------------------
    if (caveGeom != "") {
		FairModule* cave = new CbmCave("CAVE");
		cave->SetGeometryFileName(caveGeom);
		fRun->AddModule(cave);
	}

	if (pipeGeom != "") {
		FairModule* pipe = new CbmPipe("PIPE");
		pipe->SetGeometryFileName(pipeGeom);
		fRun->AddModule(pipe);
	}

	if (targetGeom != "") {
		FairModule* target = new CbmTarget("Target");
		target->SetGeometryFileName(targetGeom);
		fRun->AddModule(target);
	}

	if (magnetGeom != "") {
		FairModule* magnet = new CbmMagnet("MAGNET");
		magnet->SetGeometryFileName(magnetGeom);
		fRun->AddModule(magnet);
	}

	if (mvdGeom != "") {
		FairDetector* mvd = new CbmMvd("MVD", kTRUE);
		mvd->SetGeometryFileName(mvdGeom);
		fRun->AddModule(mvd);
	}

	if (stsGeom != "") {
		FairDetector* sts = new CbmSts("STS", kTRUE);
		sts->SetGeometryFileName(stsGeom);
		fRun->AddModule(sts);
	}

	if (richGeom != "") {
		FairDetector* rich = new CbmRich("RICH", kTRUE);
		rich->SetGeometryFileName(richGeom);
		fRun->AddModule(rich);
	}

	if (trdGeom != "") {
		FairDetector* trd = new CbmTrd("TRD", kTRUE);
		trd->SetGeometryFileName(trdGeom);
		fRun->AddModule(trd);
	}
// ------------------------------------------------------------------------

// -----   Create magnetic field   ----------------------------------------
	CbmFieldMap* magField = NULL;
	if (fieldMap == "FieldActive" || fieldMap == "FieldIron")
		magField = new CbmFieldMapSym3(fieldMap);
	else if (fieldMap == "FieldAlligator")
		magField = new CbmFieldMapSym2(fieldMap);
	else if (fieldMap = "FieldMuonMagnet")
		magField = new CbmFieldMapSym3(fieldMap);
	else {
		cout << "===> ERROR: Field map " << fieldMap << " unknown! " << endl;
		exit;
	}
	magField->SetPosition(0., 0., fieldZ);
	magField->SetScale(fieldScale);
	fRun->SetField(magField);
// ------------------------------------------------------------------------

    CbmPrimaryGenerator* primGen = new CbmPrimaryGenerator();

	Int_t kfCode1 = 11; // electrons
	Int_t kfCode2 = -11; // positrons
	Double_t minMomentum = momentum; //minimum momentum
	Double_t maxMomentum = momentum; //maximum momentum

	FairBoxGenerator* boxGen1 = new FairBoxGenerator(11, 60);
	boxGen1->SetPRange(minMomentum, maxMomentum);
	boxGen1->SetPhiRange(0., 360.);
	boxGen1->SetThetaRange(2.5, 25.);
	boxGen1->SetCosTheta();
	boxGen1->Init();
	primGen->AddGenerator(boxGen1);

	FairBoxGenerator* boxGen2 = new FairBoxGenerator(-11, 60);
	boxGen2->SetPRange(minMomentum, maxMomentum);
	boxGen2->SetPhiRange(0., 360.);
	boxGen2->SetThetaRange(2.5, 25.);
	boxGen1->SetCosTheta();
	boxGen2->Init();
	primGen->AddGenerator(boxGen2);

	FairBoxGenerator* boxGen3 = new FairBoxGenerator(211, 60);
	boxGen3->SetPRange(minMomentum, maxMomentum);
	boxGen3->SetPhiRange(0., 360.);
	boxGen3->SetThetaRange(2.5, 25.);
	boxGen1->SetCosTheta();
	boxGen3->Init();
	primGen->AddGenerator(boxGen3);

	FairBoxGenerator* boxGen4 = new FairBoxGenerator(-211, 60);
	boxGen4->SetPRange(minMomentum, maxMomentum);
	boxGen4->SetPhiRange(0., 360.);
	boxGen4->SetThetaRange(2.5, 25.);
	boxGen1->SetCosTheta();
	boxGen4->Init();
	primGen->AddGenerator(boxGen4);

	fRun->SetGenerator(primGen);

  // -----   Run initialisation   -------------------------------------------
	fRun->Init();
	// ------------------------------------------------------------------------


	// -----   Runtime database   ---------------------------------------------
	CbmFieldPar* fieldPar = (CbmFieldPar*) rtdb->getContainer("CbmFieldPar");
	fieldPar->SetParameters(magField);
	fieldPar->setChanged();
	fieldPar->setInputVersion(fRun->GetRunId(), 1);
	Bool_t kParameterMerged = kTRUE;
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
	cout << "Output file is " << outFile << endl;
	cout << "Parameter file is " << parFile << endl;
	cout << "Real time " << rtime << " s, CPU time " << ctime << "s" << endl
			<< endl;
	// ------------------------------------------------------------------------

	exit(0);
}

