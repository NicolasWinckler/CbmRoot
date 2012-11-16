void run_sim(Int_t nEvents = 10)
{
   TTree::SetMaxTreeSize(90000000000);
	Int_t iVerbose = 0;

	TString script = TString(gSystem->Getenv("SCRIPT"));
	TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");

	//gRandom->SetSeed(10);

	TString inFile = "", parFile = "", outFile ="";
	TString caveGeom = "", targetGeom = "", pipeGeom   = "", magnetGeom = "", mvdGeom = "",
	      mvdGeom = "",stsGeom = "", richGeom= "", trdGeom = "", tofGeom = "", ecalGeom = "";
	TString fieldMap = "";

	TString electrons = ""; // If "yes" than primary electrons will be generated
	Int_t NELECTRONS = 0; // number of e- to be generated
	Int_t NPOSITRONS = 0; // number of e+ to be generated
	TString urqmd = ""; // If "yes" than UrQMD will be used as background
	TString pluto = ""; // If "yes" PLUTO particles will be embedded
	TString plutoFile = "";
	TString plutoParticle = "";

	// Magnetic field
	Double_t fieldZ = 50.; // field center z position
	Double_t fieldScale =  1.0; // field scaling factor

	if (script != "yes") {
		inFile = "/Users/slebedev/Development/cbm/data/urqmd/auau/25gev/centr/urqmd.auau.25gev.centr.0000.ftn14";
		parFile = "/Users/slebedev/Development/cbm/data/simulations/test.params.root";
		outFile = "/Users/slebedev/Development/cbm/data/simulations/test.mc.root";

	   caveGeom = "cave.geo";
	   targetGeom = "target_au_250mu.geo";
	   pipeGeom = "pipe_standard.geo";
	   mvdGeom = "";//"mvd_v08a.geo";
	   stsGeom = "sts/sts_v12b.geo.root";
	   richGeom = "rich/rich_v08a.geo";
	   trdGeom = "trd/trd_v10b.geo";
	   tofGeom = "tof/tof_v07a.geo";
	   ecalGeom = "";//"ecal_FastMC.geo";
	   fieldMap = "field_v12a";
	   magnetGeom = "passive/magnet_v12a.geo";

		electrons = "yes";
		NELECTRONS = 5;
		NPOSITRONS = 5;
		urqmd = "yes";
		pluto = "no";
		plutoFile = "";
	} else {
		inFile = TString(gSystem->Getenv("INFILE"));
		outFile = TString(gSystem->Getenv("MCFILE"));
		parFile = TString(gSystem->Getenv("PARFILE"));

		caveGeom = TString(gSystem->Getenv("CAVEGEOM"));
		targetGeom = TString(gSystem->Getenv("TARGETGEOM"));
		pipeGeom = TString(gSystem->Getenv("PIPEGEOM"));
		mvdGeom = TString(gSystem->Getenv("MVDGEOM"));
		stsGeom = TString(gSystem->Getenv("STSGEOM"));
		richGeom = TString(gSystem->Getenv("RICHGEOM"));
		trdGeom = TString(gSystem->Getenv("TRDGEOM"));
		tofGeom = TString(gSystem->Getenv("TOFGEOM"));
		ecalGeom = TString(gSystem->Getenv("ECALGEOM"));
		fieldMap = TString(gSystem->Getenv("FIELDMAP"));
		magnetGeom = TString(gSystem->Getenv("MAGNETGEOM"));

		Int_t NELECTRONS = TString(gSystem->Getenv("NELECTRONS")).Atoi();
		Int_t NPOSITRONS = TString(gSystem->Getenv("NPOSITRONS")).Atoi();
		electrons = TString(gSystem->Getenv("ELECTRONS"));
		urqmd = TString(gSystem->Getenv("URQMD"));
		pluto = TString(gSystem->Getenv("PLUTO"));
		plutoFile = TString(gSystem->Getenv("PLUTOFILE"));
		plutoParticle = TString(gSystem->Getenv("PLUTOPARTICLE"));
		fieldScale = TString(gSystem->Getenv("FIELDMAPSCALE")).Atof();
	}

	gDebug = 0;
	TStopwatch timer;
	timer.Start();

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();

	gROOT->LoadMacro("$VMCWORKDIR/macro/rich/cbmlibs.C");
	cbmlibs();

	FairRunSim* fRun = new FairRunSim();
	fRun->SetName("TGeant3"); // Transport engine
	fRun->SetOutputFile(outFile);
	FairRuntimeDb* rtdb = fRun->GetRuntimeDb();

	fRun->SetMaterials("media.geo"); // Materials

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

	// if ( ecalGeom != "" ) {
	//   FairDetector* ecal = new CbmEcal("ECAL", kTRUE, ecalGeom.Data());
	//   fRun->AddModule(ecal);
	// }

	// -----   Create magnetic field   ----------------------------------------
	CbmFieldMap* magField = NULL;
	//if (fieldMap == "new_field" || fieldMap == "field_v10e" || fieldMap == "FieldShieldBox" ){
	   magField = new CbmFieldMapSym2(fieldMap);
   //}else if (fieldMap == "field_muon_standard" )
	//	magField = new CbmFieldMapSym2(fieldMap);
	//else if (fieldMap == "FieldActive" )
	//	magField = new CbmFieldMapSym3(fieldMap);
	//else {
	//	cout << "===> ERROR: Unknown field map " << fieldMap << endl;
	//	exit;
	//}
	magField->SetPosition(0., 0., fieldZ);
	magField->SetScale(fieldScale);
	fRun->SetField(magField);

	// -----   Create PrimaryGenerator   --------------------------------------
	FairPrimaryGenerator* primGen = new FairPrimaryGenerator();

	if (urqmd == "yes"){
		FairUrqmdGenerator* urqmdGen = new FairUrqmdGenerator(inFile);
		primGen->AddGenerator(urqmdGen);
	}

	//add electrons
	if (electrons == "yes"){
		FairBoxGenerator* boxGen1 = new FairBoxGenerator(11, NPOSITRONS);
		boxGen1->SetPtRange(0.,3.);
		boxGen1->SetPhiRange(0.,360.);
		boxGen1->SetThetaRange(2.5,25.);
		boxGen1->SetCosTheta();
		boxGen1->Init();
		primGen->AddGenerator(boxGen1);

		FairBoxGenerator* boxGen2 = new FairBoxGenerator(-11, NELECTRONS);
		boxGen2->SetPtRange(0.,3.);
		boxGen2->SetPhiRange(0.,360.);
		boxGen2->SetThetaRange(2.5,25.);
		boxGen2->SetCosTheta();
		boxGen2->Init();
		primGen->AddGenerator(boxGen2);

//      CbmLitPolarizedGenerator *polGen;
//      polGen = new CbmLitPolarizedGenerator(443, NELECTRONS);
//      polGen->SetDistributionPt(0.176);        // 25 GeV
//      polGen->SetDistributionY(1.9875,0.228);  // 25 GeV
//      polGen->SetRangePt(0.,3.);
//      polGen->SetRangeY(1.,3.);
//      polGen->SetBox(0);
//      polGen->SetRefFrame(CbmLitPolarizedGenerator::kHelicity);
//      polGen->SetDecayMode(CbmLitPolarizedGenerator::kDiElectron);
//      polGen->SetAlpha(0);
//      polGen->Init();
//      primGen->AddGenerator(polGen);
   }

	if (pluto == "yes") {
		FairPlutoGenerator *plutoGen= new FairPlutoGenerator(plutoFile);
		primGen->AddGenerator(plutoGen);
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

	fRun->Run(nEvents);

	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	cout << endl << endl;
	cout << "Macro finished succesfully." << endl;
	cout << "Output file is "    << outFile << endl;
	cout << "Parameter file is " << parFile << endl;
	cout << "Real time " << rtime << " s, CPU time " << ctime
       << "s" << endl << endl;

	cout << " Test passed" << endl;
	cout << " All ok " << endl;
}

