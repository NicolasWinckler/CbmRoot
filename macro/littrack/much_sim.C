void much_sim(Int_t nEvents = 100)
{
	TString script = TString(gSystem->Getenv("SCRIPT"));

	// number of embedded muons or Jpsi
	Int_t NMUONS = 1;

	TString muchGeom, trdGeom, inFile, dir, mcFile, parFile, plutoFile[NMUONS], muons, urqmd, pluto;
	//if SCRIPT environment variable is set to "yes", i.e. macro is run via script
	if (script != "yes") {
		//input UrQMD file
		inFile  = "/d/cbm03/urqmd/auau/25gev/centr/urqmd.auau.25gev.centr.0000.ftn14";
		//if necessary specify input pluto file to embed signal particles
		TString plutoDir = "/u/andrey/cbm/much/pluto/25gev/";
		plutoFile[0] = plutoDir + "Jpsi.0010.root";
//		plutoFile[1] = plutoDir + "Jpsi.0011.root";
//		plutoFile[2] = plutoDir + "Jpsi.0012.root";
//		plutoFile[3] = plutoDir + "Jpsi.0013.root";
//		plutoFile[4] = plutoDir + "Jpsi.0014.root";
//		plutoFile[5] = plutoDir + "Jpsi.0015.root";
//		plutoFile[6] = plutoDir + "Jpsi.0016.root";
//		plutoFile[7] = plutoDir + "Jpsi.0017.root";
//		plutoFile[8] = plutoDir + "Jpsi.0018.root";
//		plutoFile[9] = plutoDir + "Jpsi.0019.root";
		//directory for output simulation files
		dir  = "/d/cbm02/andrey/test/";
		//MC file name
		mcFile = dir + "mc.0000.root";
		//Parameter file name
		parFile = dir + "param.0000.root";
		//If "yes" than 10 primary muons will be generated
		muons = "no";
		//If "yes" than UrQMD will be used as background
		urqmd = "no";
		//If "yes" PLUTO particles will be embedded
		pluto = "yes";
		//MUCH geometry file name
		muchGeom = "much_standard_2layers.geo";
		//TRD geometry file name
		trdGeom = "";//"trd_muon_setup_new.geo";
	} else {
		inFile  = TString(gSystem->Getenv("INFILE"));
		plutoFile  = TString(gSystem->Getenv("PLUTOFILE"));
		mcFile = TString(gSystem->Getenv("MCFILE"));
		parFile = TString(gSystem->Getenv("PARFILE"));
		muons = TString(gSystem->Getenv("MUONS"));
		urqmd = TString(gSystem->Getenv("URQMD"));
		pluto = TString(gSystem->Getenv("PLUTO"));
		muchGeom = TString(gSystem->Getenv("MUCHGEOM"));
		trdGeom = TString(gSystem->Getenv("TRDGEOM"));
	}

	TString pipeGeom   = "pipe_much.geo";
	TString shieldGeom = "shield_standard.geo";

	TString caveGeom   = "cave.geo";
	TString targetGeom = "target_au_250mu.geo";
	TString stsGeom    = "sts_standard.geo";
	TString tofGeom    = "tof_standard.geo";

	// -----   Magnetic field   -----------------------------------------------
	TString fieldMap = "field_muon_standard";   // name of field map
	TString magnetGeom = "magnet_muon_standard.geo";
	Double_t fieldZ = 50.;                 // field center z position
	Double_t fieldScale =  1.;                 // field scaling factor

	TStopwatch timer;
	timer.Start();

	gSystem->Load("/u/andrey/soft/tbb/Etch32/libtbb");

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
	cbmrootlibs();

	FairRunSim* fRun = new FairRunSim();
	fRun->SetName("TGeant3");              // Transport engine
	fRun->SetOutputFile(mcFile);          // Output file
	FairRuntimeDb* rtdb = fRun->GetRuntimeDb();

	fRun->SetMaterials("media.geo");       // Materials
	//fRun->SetStoreTraj(kTRUE);

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

	if ( stsGeom != "" ) {
		FairDetector* sts = new CbmSts("STS", kTRUE);
		sts->SetGeometryFileName(stsGeom);
		fRun->AddModule(sts);
		cout << "    --- " << stsGeom << endl;
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

	if (pluto == "yes") {
		for (Int_t i = 0; i < NMUONS; i++) {
			FairPlutoGenerator *plutoGen= new FairPlutoGenerator(plutoFile[i]);
			primGen->AddGenerator(plutoGen);
		}
	}

	if (urqmd == "yes") {
		FairUrqmdGenerator*  urqmdGen = new FairUrqmdGenerator(inFile);
		primGen->AddGenerator(urqmdGen);
	}

	if (muons == "yes") {
		Double_t minMom, maxMom;
		if (muchGeom.Contains("standard")) {
			minMom = 2.5;
			maxMom = 25.;
		} else if (muchGeom.Contains("compact")) {
			minMom = 1.5;
			maxMom = 10.;
		}

		FairBoxGenerator* boxGen1 = new FairBoxGenerator(13, NMUONS/2);
		boxGen1->SetPRange(minMom, maxMom);
		boxGen1->SetPhiRange(0.,360.);
		boxGen1->SetThetaRange(2.5, 25.);
		boxGen1->SetCosTheta();
		boxGen1->Init();
		primGen->AddGenerator(boxGen1);

		FairBoxGenerator* boxGen2 = new FairBoxGenerator(-13, NMUONS/2);
		boxGen2->SetPRange(minMom, maxMom);
		boxGen2->SetPhiRange(0.,360.);
		boxGen2->SetThetaRange(2.5, 25.);
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
