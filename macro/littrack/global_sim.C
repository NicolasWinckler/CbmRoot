/**
 * \file global_sim.C
 * \brief Macro runs simulation for "electron" or "muon" setups of CBM.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2010
 **/

#include <iostream>
using std::cout;
using std::endl;

void global_sim(Int_t nEvents = 1000)
{
	TString script = TString(gSystem->Getenv("SCRIPT"));

	// Specify "electron" or "muon" setup of CBM
//	TString setup = "muon";
	TString setup = "electron";

	// Event parameters
	Int_t nofMuonsPlus = 0; // number of embedded muons from FairBoxGenerator
	Int_t nofMuonsMinus = 0; // number of embedded muons from FairBoxGenerator
	Int_t nofElectrons = 0; // number of embedded electrons from FairBoxGenerator
	Int_t nofPositrons = 0; // number of embedded positrons from FairBoxGenerator
	Int_t nofPionsPlus = 0; // number of embedded pions from FairBoxGenerator
	Int_t nofPionsMinus = 0; // number of embedded pions from FairBoxGenerator
	Int_t nofJPsiToMuons = 0; // number of embedded J/Psi particles decaying to mu+ and mu-
	Int_t nofJPsiToElectrons = 5; // number of embedded J/Psi particles decaying to e+ and e-
	TString urqmd = "no"; // If "yes" than UrQMD will be used as background
   TString unigen = "no"; // If "yes" than CbmUnigenGenerator will be used instead of FairUrqmdGenerator

	// Files
	TString urqmdFile  = "/d/cbm03/urqmd/auau/25gev/centr/urqmd.auau.25gev.centr.0000.ftn14"; // input UrQMD file
	TString dir = "/data.local1/andrey/events/events_propagation_qa_electrons/"; //directory for output simulation files
	TString mcFile = dir + "mc.0000.root"; //MC file name
	TString parFile = dir + "param.0000.root"; //Parameter file name

	// Geometries
	TString caveGeom = "", targetGeom = "", pipeGeom = "", shieldGeom = "",
			mvdGeom = "", stsGeom = "", muchGeom = "", richGeom = "", trdGeom = "",
			tofGeom = "", ecalGeom = "", fieldMap = "", magnetGeom = "";
	if (setup == "muon") {
		caveGeom   = "cave.geo";
		targetGeom = "target_au_250mu.geo";
		pipeGeom   = "pipe_much.geo";
		shieldGeom = "shield_standard.geo";
		mvdGeom    = "mvd/mvd_v07a.geo";
		stsGeom    = "sts/sts_v11a.geo";
		muchGeom   = "much/much_v11a.geo";
		trdGeom    = "";//"trd_muon_setup_new.geo";
		tofGeom    = "tof/tof_v07a.geo";
		fieldMap   = "field_v10e";
		magnetGeom = "passive/magnet_v09m.geo";
	} else if (setup == "electron") {
		caveGeom   = "cave.geo";
		targetGeom = "target_au_250mu.geo";
		pipeGeom   = "pipe_standard.geo";
		mvdGeom    = "mvd/mvd_v07a.geo";
		stsGeom    = "sts/sts_v11a.geo";
		richGeom   = "rich/rich_v08a.geo";
		trdGeom    = "trd/trd_v10b.geo";
		tofGeom    = "tof/tof_v07a.geo";
		ecalGeom   = "";//"ecal_FastMC.geo";
		fieldMap   = "field_v10e";
		magnetGeom = "passive/magnet_v09e.geo";
	}

	// If SCRIPT environment variable is set to "yes", i.e. macro is run via script
	if (script == "yes") {
		urqmdFile  = TString(gSystem->Getenv("URQMDFILE"));
		mcFile = TString(gSystem->Getenv("MCFILE"));
		parFile = TString(gSystem->Getenv("PARFILE"));

		nofMuonsPlus = TString(gSystem->Getenv("NMUONSPLUS")).Atoi();
		nofMuonsMinus = TString(gSystem->Getenv("NMUONSMINUS")).Atoi();
		nofElectrons = TString(gSystem->Getenv("NELECTRONS")).Atoi();
		nofPositrons = TString(gSystem->Getenv("NPOSITRONS")).Atoi();
		nofPionsPlus = TString(gSystem->Getenv("NPIONSPLUS")).Atoi();
		nofPionsMinus = TString(gSystem->Getenv("NPIONSMINUS")).Atoi();
		nofJPsiToMuons = TString(gSystem->Getenv("NJPSITOMUONS")).Atoi();
		nofJPsiToElectrons = TString(gSystem->Getenv("NJPSITOELECTRONS")).Atoi();
		urqmd = TString(gSystem->Getenv("URQMD"));
      unigen = TString(gSystem->Getenv("UNIGEN"));

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
   CbmFieldMap* magField = new CbmFieldMapSym2(fieldMap);
   magField->SetPosition(0., 0., fieldZ);
   magField->SetScale(fieldScale);
   fRun->SetField(magField);
   // ------------------------------------------------------------------------

	// ------------------------------------------------------------------------
	FairPrimaryGenerator* primGen = new FairPrimaryGenerator();

	if (urqmd == "yes" && unigen == "yes") {
		CbmUnigenGenerator*  urqmdGen = new CbmUnigenGenerator(urqmdFile);
		primGen->AddGenerator(urqmdGen);
	}

	if (urqmd == "yes" && unigen != "yes") {
	    FairUrqmdGenerator* urqmdGen = new FairUrqmdGenerator(urqmdFile);
       primGen->AddGenerator(urqmdGen);
	}

	if (nofJPsiToMuons > 0) {
      CbmLitPolarizedGenerator *polGen;
      polGen = new CbmLitPolarizedGenerator(443, nofJPsiToMuons);
      polGen->SetDistributionPt(0.176);        // 25 GeV
      polGen->SetDistributionY(1.9875,0.228);  // 25 GeV
      polGen->SetRangePt(0.,3.);
      polGen->SetRangeY(1.,3.);
      polGen->SetBox(0);
      polGen->SetRefFrame(CbmLitPolarizedGenerator::kHelicity);
      polGen->SetDecayMode(CbmLitPolarizedGenerator::kDiMuon);
      polGen->SetAlpha(0);
      polGen->Init();
      primGen->AddGenerator(polGen);
	}

   if (nofJPsiToElectrons > 0) {
      CbmLitPolarizedGenerator *polGen;
      polGen = new CbmLitPolarizedGenerator(443, nofJPsiToElectrons);
      polGen->SetDistributionPt(0.176);        // 25 GeV
      polGen->SetDistributionY(1.9875,0.228);  // 25 GeV
      polGen->SetRangePt(0.,3.);
      polGen->SetRangeY(1.,3.);
      polGen->SetBox(0);
      polGen->SetRefFrame(CbmLitPolarizedGenerator::kHelicity);
      polGen->SetDecayMode(CbmLitPolarizedGenerator::kDiElectron);
      polGen->SetAlpha(0);
      polGen->Init();
      primGen->AddGenerator(polGen);
   }

   if (nofMuonsPlus > 0) {
		FairBoxGenerator* boxGen = new FairBoxGenerator(-13, nofMuonsPlus);
		boxGen->SetPRange(1.5, 16.);
		boxGen->SetPhiRange(0.,360.);
		boxGen->SetThetaRange(2.5, 25.);
		boxGen->SetCosTheta();
		boxGen->Init();
		primGen->AddGenerator(boxGen);
   }

   if (nofMuonsMinus > 0) {
		FairBoxGenerator* boxGen = new FairBoxGenerator(13, nofMuonsMinus);
		boxGen->SetPRange(1.5, 16.);
		boxGen->SetPhiRange(0.,360.);
		boxGen->SetThetaRange(2.5, 25.);
		boxGen->SetCosTheta();
		boxGen->Init();
		primGen->AddGenerator(boxGen);
	}

	if (nofElectrons > 0) {
		FairBoxGenerator* boxGen = new FairBoxGenerator(11, nofElectrons);
		boxGen->SetPtRange(0.,3.);
//		boxGen->SetPRange(30, 30);
		boxGen->SetPhiRange(0.,360.);
		boxGen->SetThetaRange(2.5,25.);
		boxGen->SetCosTheta();
		boxGen->Init();
		primGen->AddGenerator(boxGen);
	}

	if (nofPositrons > 0) {
		FairBoxGenerator* boxGen = new FairBoxGenerator(-11, nofPositrons);
		boxGen->SetPtRange(0.,3.);
//		boxGen->SetPRange(30, 30);
		boxGen->SetPhiRange(0.,360.);
		boxGen->SetThetaRange(2.5,25.);
		boxGen->SetCosTheta();
		boxGen->Init();
		primGen->AddGenerator(boxGen);
	}

	if (nofPionsPlus > 0) {
		FairBoxGenerator* boxGen = new FairBoxGenerator(211, nofPionsPlus);
//		boxGen1->SetPtRange(0.,3.);
		boxGen->SetPRange(0., 10.);
		boxGen->SetPhiRange(0.,360.);
		boxGen->SetThetaRange(2.5,25.);
		boxGen->SetCosTheta();
		boxGen->Init();
		primGen->AddGenerator(boxGen);
	}

	if (nofPionsMinus > 0) {
		FairBoxGenerator* boxGen = new FairBoxGenerator(-211, nofPionsMinus);
//		boxGen->SetPtRange(0.,3.);
		boxGen->SetPRange(0.3, 1.);
		boxGen->SetPhiRange(0.,360.);
		boxGen->SetThetaRange(2.5,25.);
		boxGen->SetCosTheta();
		boxGen->Init();
		primGen->AddGenerator(boxGen);
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
