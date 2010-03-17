/** global_sim.C
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2010
 * @version 1.0
 *
 * Macro runs simulation for "electron" or "muon" setups
 * of the CBM.
 **/

void global_sim(Int_t nEvents = 1000)
{
	TString script = TString(gSystem->Getenv("SCRIPT"));

	// Specify "electron" or "muon" setup of CBM
	TString setup = "muon";
//	TString setup = "electron";

	// Event parameters
	Int_t NMUONSPLUS = 5; // number of embedded muons
	Int_t NMUONSMINUS = 5; // number of embedded muons
	Int_t NELECTRONS = 5; // number of embedded electrons
	Int_t NPOSITRONS = 5; // number of embedded positrons
	Int_t NPIONSPLUS = 5; // number of embedded pions
	Int_t NPIONSMINUS = 5; // number of embedded pions
	Int_t NPLUTO = 10; // number of embedded particles from pluto
	TString urqmd = "no"; // If "yes" than UrQMD will be used as background
	TString muons = "yes"; // If "yes" than primary muons will be generated
	TString electrons = "no"; // If "yes" than primary electrons will be generated
	TString pions = "no"; // If "yes" than primary pions will be generated
	TString pluto = "no"; // If "yes" PLUTO particles will be embedded

	// Files
	TString inFile  = "/d/cbm03/urqmd/auau/25gev/centr/urqmd.auau.25gev.centr.0000.ftn14"; // input UrQMD file
	TString dir  = "/d/cbm02/andrey/test/"; //directory for output simulation files
	TString mcFile = dir + "mc.0000.root"; //MC file name
	TString parFile = dir + "param.0000.root"; //Parameter file name

	// Pluto files
	TString plutoDir = "/d/cbm02/andrey/pluto/"; // if necessary specify input pluto file to embed signal particles
	std::vector<TString> plutoFile;
	if (script != "yes") {
		plutoFile.resize(NPLUTO);
		for (Int_t i = 0; i < NPLUTO; i++) {
			char name[256];
			if (i < 10) sprintf(name, "Jpsi.000%d", i);
			else if (i < 100) sprintf(name, "Jpsi.00%d", i);
			else sprintf(name, "Jpsi.0%d", i);
			plutoFile[0] = plutoDir + TString(name) + ".root";
		}
	}

	// Geometries
	TString caveGeom = "", targetGeom = "", pipeGeom = "", shieldGeom = "",
			mvdGeom = "", stsGeom = "", muchGeom = "", richGeom = "", trdGeom = "",
			tofGeom = "", ecalGeom = "", fieldMap = "", magnetGeom = "";
	if (setup == "muon") {
		caveGeom   = "cave.geo";
		targetGeom = "target_au_250mu.geo";
		pipeGeom   = "pipe_much.geo";
		shieldGeom = "shield_standard.geo";
		stsGeom    = "sts_standard.geo";
		muchGeom   = "much_standard_2layers.geo";
		trdGeom    = "";//"trd_muon_setup_new.geo";
		tofGeom    = "tof_standard.geo";
		fieldMap   = "field_muon_standard";
		magnetGeom = "magnet_muon_standard.geo";
	} else if (setup == "electron") {
		caveGeom   = "cave.geo";
		targetGeom = "target_au_250mu.geo";
		pipeGeom   = "pipe_standard.geo";
		mvdGeom    = "";//"mvd_standard.geo";
		stsGeom    = "sts_standard.geo";
		richGeom   = "rich_standard.geo";
		trdGeom    = "trd_standard.geo";
		tofGeom    = "tof_standard.geo";
		ecalGeom   = "";//"ecal_FastMC.geo";
		fieldMap   = "field_electron_standard";
		magnetGeom = "magnet_electron_standard.geo";
	}

	//if SCRIPT environment variable is set to "yes", i.e. macro is run via script
	if (script == "yes") {
		inFile  = TString(gSystem->Getenv("INFILE"));
		mcFile = TString(gSystem->Getenv("MCFILE"));
		parFile = TString(gSystem->Getenv("PARFILE"));

		NMUONSPLUS = TString(gSystem->Getenv("NMUONSPLUS")).Atoi();
		NMUONSMINUS = TString(gSystem->Getenv("NMUONSMINUS")).Atoi();
		NELECTRONS = TString(gSystem->Getenv("NELECTRONS")).Atoi();
		NPOSITRONS = TString(gSystem->Getenv("NPOSITRONS")).Atoi();
		NPIONSPLUS = TString(gSystem->Getenv("NPIONSPLUS")).Atoi();
		NPIONSMINUS = TString(gSystem->Getenv("NPIONSMINUS")).Atoi();
		NPLUTO = TString(gSystem->Getenv("NPLUTO")).Atoi();
		urqmd = TString(gSystem->Getenv("URQMD"));
		muons = TString(gSystem->Getenv("MUONS"));
		electrons = TString(gSystem->Getenv("ELECTRONS"));
		pions = TString(gSystem->Getenv("PIONS"));
		pluto = TString(gSystem->Getenv("PLUTO"));

		plutoFile.resize(NPLUTO);
		for (Int_t i = 0; i < NPLUTO; i++) {
			char name[256];
			if (i < 10) sprintf(name, "PLUTOFILE%d", i);
			plutoFile[i] = TString(gSystem->Getenv(name));
		}

		caveGeom = TString(gSystem->Getenv("CAVEGEOM"));
		targetGeom = TString(gSystem->Getenv("TARGETGEOM"));
		pipeGeom = TString(gSystem->Getenv("PIPEGEOM"));
		shieldGeom = TString(gSystem->Getenv("SHIELDGEOM"));
		mvdGeom = TString(gSystem->Getenv("MVDGEOM"));
		stsGeom = TString(gSystem->Getenv("STSGEOM"));
		muchGeom = TString(gSystem->Getenv("MUCHGEOM"));
		richGeom = TString(gSystem->Getenv("RICHGEOM"));
		trdGeom = TString(gSystem->Getenv("TRDGEOM"));
		tofGeom = TString(gSystem->Getenv("TOFGEOM"));
		ecalGeom = TString(gSystem->Getenv("ECALGEOM"));
		fieldMap = TString(gSystem->Getenv("FIELDMAP"));
		magnetGeom = TString(gSystem->Getenv("MAGNETGEOM"));
	}
	// -----   Magnetic field   -----------------------------------------------
	Double_t fieldZ = 50.; // field center z position
	Double_t fieldScale = 1.; // field scaling factor

	TStopwatch timer;
	timer.Start();

	gSystem->Load("/u/andrey/soft/tbb/Lenny64/libtbb");

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
	cbmrootlibs();

	FairRunSim* fRun = new FairRunSim();
	fRun->SetName("TGeant3"); // Transport engine
	fRun->SetOutputFile(mcFile); // Output file
	FairRuntimeDb* rtdb = fRun->GetRuntimeDb();

	fRun->SetMaterials("media.geo"); // Materials
//	fRun->SetStoreTraj(kTRUE);

	if ( caveGeom != "" ) {
		FairModule* cave = new CbmCave("CAVE");
		cave->SetGeometryFileName(caveGeom);
		fRun->AddModule(cave);
		cout << "    --- " << caveGeom << endl;
	}

	if ( pipeGeom != "" ) {
		FairModule* pipe = new CbmPipe("PIPE");
		pipe->SetGeometryFileName(pipeGeom);
		fRun->AddModule(pipe);
		cout << "    --- " << pipeGeom << endl;
	}

	if ( shieldGeom != "" ) {
		FairModule* shield = new CbmShield("SHIELD");
		shield->SetGeometryFileName(shieldGeom);
		fRun->AddModule(shield);
		cout << "    --- " << shieldGeom << endl;
	}

	if ( targetGeom != "" ) {
		FairModule* target = new CbmTarget("Target");
		target->SetGeometryFileName(targetGeom);
		fRun->AddModule(target);
		cout << "    --- " << targetGeom << endl;
	}

	if ( magnetGeom != "" ) {
		FairModule* magnet = new CbmMagnet("MAGNET");
		magnet->SetGeometryFileName(magnetGeom);
		fRun->AddModule(magnet);
		cout << "    --- " << magnetGeom << endl;
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
		cout << "    --- " << stsGeom << endl;
	}

	if ( richGeom != "" ) {
		FairDetector* rich = new CbmRich("RICH", kTRUE);
		rich->SetGeometryFileName(richGeom);
		fRun->AddModule(rich);
	}

	if ( muchGeom != "" ) {
		FairDetector* much = new CbmMuch("MUCH", kTRUE);
		much->SetGeometryFileName(muchGeom);
		fRun->AddModule(much);
		cout << "    --- " << muchGeom << endl;
	}

	if ( trdGeom != "" ) {
		FairDetector* trd = new CbmTrd("TRD",kTRUE );
		trd->SetGeometryFileName(trdGeom);
		fRun->AddModule(trd);
		cout << "    --- " << trdGeom << endl;
	}

	if ( tofGeom != "" ) {
		FairDetector* tof = new CbmTof("TOF", kTRUE);
		tof->SetGeometryFileName(tofGeom);
		fRun->AddModule(tof);
		cout << "    --- " << tofGeom << endl;
	}

	if ( ecalGeom != "" ) {
		FairDetector* ecal = new CbmEcal("ECAL", kTRUE, ecalGeom.Data());
		fRun->AddModule(ecal);
	}
	// ------------------------------------------------------------------------

	// -----   Create magnetic field   ----------------------------------------
	CbmFieldMap* magField = NULL;
	if (fieldMap == "field_electron_standard" )
		magField = new CbmFieldMapSym2(fieldMap);
	else if (fieldMap == "field_muon_standard" )
		magField = new CbmFieldMapSym2(fieldMap);
	else if (fieldMap == "FieldMuonMagnet" )
		magField = new CbmFieldMapSym3(fieldMap);
	else {
		cout << "===> ERROR: Unknown field map " << fieldMap << endl;
	exit;
	}
	magField->SetPosition(0., 0., fieldZ);
	magField->SetScale(fieldScale);
	fRun->SetField(magField);
	// ------------------------------------------------------------------------

	// ------------------------------------------------------------------------
	CbmPrimaryGenerator* primGen = new CbmPrimaryGenerator();

	if (urqmd == "yes") {
		FairUrqmdGenerator*  urqmdGen = new FairUrqmdGenerator(inFile);
		primGen->AddGenerator(urqmdGen);
	}

	if (pluto == "yes") {
		for (Int_t i = 0; i < NPLUTO; i++) {
			FairPlutoGenerator *plutoGen= new FairPlutoGenerator(plutoFile[i]);
			primGen->AddGenerator(plutoGen);
		}
	}

	if (muons == "yes") {
		Double_t minMom, maxMom;
		if (muchGeom.Contains("standard")) {
			minMom = 2.5;
			maxMom = 25.;
		} else if (muchGeom.Contains("compact")) {
			minMom = 1.0;
			maxMom = 10.;
		}

		FairBoxGenerator* boxGen1 = new FairBoxGenerator(13, NMUONSMINUS);
		boxGen1->SetPRange(minMom, maxMom);
		boxGen1->SetPhiRange(0.,360.);
		boxGen1->SetThetaRange(2.5, 25.);
		boxGen1->SetCosTheta();
		boxGen1->Init();
		primGen->AddGenerator(boxGen1);

		FairBoxGenerator* boxGen2 = new FairBoxGenerator(-13, NMUONSPLUS);
		boxGen2->SetPRange(minMom, maxMom);
		boxGen2->SetPhiRange(0.,360.);
		boxGen2->SetThetaRange(2.5, 25.);
		boxGen2->SetCosTheta();
		boxGen2->Init();
		primGen->AddGenerator(boxGen2);
	}

	if (electrons == "yes") {
		FairBoxGenerator* boxGen1 = new FairBoxGenerator(11, NELECTRONS);
		boxGen1->SetPtRange(0.,3.);
//		boxGen1->SetPRange(30, 30);
		boxGen1->SetPhiRange(0.,360.);
		boxGen1->SetThetaRange(2.5,25.);
		boxGen1->SetCosTheta();
		boxGen1->Init();
		primGen->AddGenerator(boxGen1);

		FairBoxGenerator* boxGen2 = new FairBoxGenerator(-11, NPOSITRONS);
		boxGen2->SetPtRange(0.,3.);
//		boxGen2->SetPRange(30, 30);
		boxGen2->SetPhiRange(0.,360.);
		boxGen2->SetThetaRange(2.5,25.);
		boxGen2->SetCosTheta();
		boxGen2->Init();
		primGen->AddGenerator(boxGen2);
	}

	if (pions == "yes") {
		FairBoxGenerator* boxGen1 = new FairBoxGenerator(211, NPIONSPLUS);
		boxGen1->SetPtRange(0.,3.);
//		boxGen1->SetPRange(30, 30);
		boxGen1->SetPhiRange(0.,360.);
		boxGen1->SetThetaRange(2.5,25.);
		boxGen1->SetCosTheta();
		boxGen1->Init();
		primGen->AddGenerator(boxGen1);

		FairBoxGenerator* boxGen2 = new FairBoxGenerator(-211, NPIONSMINUS);
		boxGen2->SetPtRange(0.,3.);
//		boxGen2->SetPRange(30, 30);
		boxGen2->SetPhiRange(0.,360.);
		boxGen2->SetThetaRange(2.5,25.);
		boxGen2->SetCosTheta();
		boxGen2->Init();
		primGen->AddGenerator(boxGen2);
	}

	fRun->SetGenerator(primGen);
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
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	cout << endl << endl;
	cout << "Macro finished successfully." << endl;
	cout << "Output file is " << mcFile << endl;
	cout << "Real time used: " << rtime << "s " << endl;
	cout << "CPU time used : " << ctime << "s " << endl << endl << endl;
	// ------------------------------------------------------------------------
}
