void trd_sim(Int_t nEvents = 1000)
{
	TString script = TString(gSystem->Getenv("SCRIPT"));

	TString inFile, plutoFile, dir, mcFile, parFile, electrons, urqmd, trdGeom, pluto;
	if (script != "yes") {
		//input UrQMD file
		inFile  = "/home/d/urqmd/auau/25gev/centr/urqmd.auau.25gev.centr.0000.ftn14";
		//if necessary specify input pluto file to embed signal particles
		plutoFile = "/u/andrey/cbm/much/pluto/omega/25gev/omega.0000.root";
		//directory for output simulation files
		dir  = "/home/d/andrey/std_10e_cut10gev/";
		//MC file name
		mcFile = dir + "mc.0000.root";
		//Parameter file name
		parFile = dir + "param.0000.root";
		//If "yes" than 10 primary muons will be generated
		electrons = "yes";
		//If "yes" than UrQMD will be used as background
		urqmd = "no";
		//If "yes" PLUTO particles will be embedded
		pluto = "no";
		//TRD geometry file name
		trdGeom = "trd_standard.geo";
	} else {
		inFile  = TString(gSystem->Getenv("INFILE"));
		plutoFile  = TString(gSystem->Getenv("PLUTOFILE"));
		mcFile = TString(gSystem->Getenv("MCFILE"));
		parFile = TString(gSystem->Getenv("PARFILE"));
		electrons = TString(gSystem->Getenv("ELECTRONS"));
		urqmd = TString(gSystem->Getenv("URQMD"));
		pluto = TString(gSystem->Getenv("PLUTO"));
		trdGeom = TString(gSystem->Getenv("TRDGEOM"));
	}

	TString caveGeom   = "cave.geo";
	TString targetGeom = "target_au_250mu.geo";
	TString pipeGeom   = "pipe_standard.geo";
	TString magnetGeom = "magnet_standard.geo";
	TString mvdGeom    = "";//"mvd_standard.geo";
	TString stsGeom    = "sts_standard.geo";
	TString richGeom   = "rich_standard.geo";
	TString tofGeom    = "tof_standard.geo";
	TString ecalGeom   = "";//"ecal_FastMC.geo";

	// -----   Magnetic field   -----------------------------------------------
	TString fieldMap    = "FieldMuonMagnet";   // name of field map
	Double_t fieldZ     = 50.;             // field centre z position
	Double_t fieldScale =  1.;             // field scaling factor

	TStopwatch timer;
	timer.Start();

	gSystem->Load("/home/soft/tbb22_004oss/libtbb");
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
	cbmrootlibs();

	FairRunSim* fRun = new FairRunSim();
	fRun->SetName("TGeant3");              // Transport engine
	fRun->SetOutputFile(mcFile);          // Output file
	FairRuntimeDb* rtdb = fRun->GetRuntimeDb();

	fRun->SetMaterials("media.geo");       // Materials
	// fRun->SetStoreTraj(kTRUE);

	// -----   Create detectors and passive volumes   -------------------------
	if ( caveGeom != "" ) {
		FairModule* cave = new CbmCave("CAVE");
		cave->SetGeometryFileName(caveGeom);
		fRun->AddModule(cave);
	}

	if ( pipeGeom != "" ) {
		FairModule* pipe = new CbmPipe("PIPE");
		pipe->SetGeometryFileName(pipeGeom);
		fRun->AddModule(pipe);
	}

	if ( targetGeom != "" ) {
		FairModule* target = new CbmTarget("Target");
		target->SetGeometryFileName(targetGeom);
		fRun->AddModule(target);
	}

	if ( magnetGeom != "" ) {
		FairModule* magnet = new CbmMagnet("MAGNET");
		magnet->SetGeometryFileName(magnetGeom);
		fRun->AddModule(magnet);
	}

	if ( mvdGeom != "" ) {
		FairDetector* mvd = new CbmMvd("MVD", kTRUE);
		mvd->SetGeometryFileName(mvdGeom);
		fRun->AddModule(mvd);
	}

	if ( stsGeom != "" ) {
		FairDetector* sts = new CbmSts("STS", kTRUE);
		sts->SetGeometryFileName(stsGeom);
		fRun->AddModule(sts);
	}

	if ( richGeom != "" ) {
		FairDetector* rich = new CbmRich("RICH", kTRUE);
		rich->SetGeometryFileName(richGeom);
		fRun->AddModule(rich);
	}

	if ( trdGeom != "" ) {
		FairDetector* trd = new CbmTrd("TRD",kTRUE );
		trd->SetGeometryFileName(trdGeom);
		fRun->AddModule(trd);
	}

	if ( tofGeom != "" ) {
		FairDetector* tof = new CbmTof("TOF", kTRUE);
		tof->SetGeometryFileName(tofGeom);
		fRun->AddModule(tof);
	}

	if ( ecalGeom != "" ) {
		FairDetector* ecal = new CbmEcal("ECAL", kTRUE, ecalGeom.Data());
		fRun->AddModule(ecal);
	}
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
		exit(1);
	}
	magField->SetPosition(0., 0., fieldZ);
	magField->SetScale(fieldScale);
	fRun->SetField(magField);
	// ------------------------------------------------------------------------

	// ------------------------------------------------------------------------
	CbmPrimaryGenerator* primGen = new CbmPrimaryGenerator();

	if (pluto == "yes") {
		FairPlutoGenerator *plutoGen= new FairPlutoGenerator(plutoFile);
		primGen->AddGenerator(plutoGen);
	}

	if (urqmd == "yes") {
		FairUrqmdGenerator*  urqmdGen = new FairUrqmdGenerator(inFile);
		primGen->AddGenerator(urqmdGen);
	}

	if (electrons == "yes") {
		FairBoxGenerator* boxGen1 = new FairBoxGenerator(11, 5);
		boxGen1->SetPtRange(0.,3.);
//		boxGen1->SetPRange(30, 30);
		boxGen1->SetPhiRange(0.,360.);
		boxGen1->SetThetaRange(2.5,25.);
		boxGen1->SetCosTheta();
		boxGen1->Init();
		primGen->AddGenerator(boxGen1);

		FairBoxGenerator* boxGen2 = new FairBoxGenerator(-11, 5);
		boxGen2->SetPtRange(0.,3.);
//		boxGen2->SetPRange(30, 30);
		boxGen2->SetPhiRange(0.,360.);
		boxGen2->SetThetaRange(2.5,25.);
		boxGen2->SetCosTheta();
		boxGen2->Init();
		primGen->AddGenerator(boxGen2);
	}

	fRun->SetGenerator(primGen);
	// ------------------------------------------------------------------------

	fRun->Init();

	// -----   Runtime database   ---------------------------------------------
	CbmFieldPar* fieldPar = (CbmFieldPar*) rtdb->getContainer("CbmFieldPar");
	fieldPar->SetParameters(magField);
	fieldPar->setChanged();
	fieldPar->setInputVersion(fRun->GetRunId(),1);
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
	cout << "Macro finished succesfully." << endl;
	cout << "Output file is "    << mcFile << endl;
	cout << "Parameter file is " << parFile << endl;
	cout << "Real time " << timer.RealTime() << " s, CPU time " << timer.CpuTime() << "s" << endl << endl;
	// ------------------------------------------------------------------------
}

