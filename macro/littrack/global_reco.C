void global_reco(Int_t nEvents = 100)
{
	TString script = TString(gSystem->Getenv("SCRIPT"));

	TString dir, mcFile, parFile, stsRecoFile, trdHitsFile, muchHitsFile;
	if (script != "yes") {
//		dir  = "/home/d/andrey/events/trd/segmented/10e/e_urqmd/";
		dir  = "/home/d/andrey/test/trunk/global_mu_urqmd/";
		mcFile = dir + "mc.0000.root";
		parFile = dir + "param.0000.root";
		stsRecoFile = dir + "sts.reco.0000.root";
		trdHitsFile = dir + "trd.hits.0000.root";
		muchHitsFile = dir + "much.hits.0000.root";
		tofHitsFile = dir + "tof.hits.0000.root";
		globalTracksFile = dir + "global.tracks.sg.0000.root";
	} else {
		mcFile = TString(gSystem->Getenv("MCFILE"));
		parFile = TString(gSystem->Getenv("PARFILE"));
		stsRecoFile = TString(gSystem->Getenv("STSRECOFILE"));
		trdHitsFile = TString(gSystem->Getenv("TRDHITSFILE"));
		trdTracksFile = TString(gSystem->Getenv("TRDTRACKSFILE"));
		tofHitsFile = TString(gSystem->Getenv("TOFHITSFILE"));
		globalTracksFile = TString(gSystem->Getenv("GLOBALTRACKSFILE"));
	}

	Int_t iVerbose = 1;
	TStopwatch timer;
	timer.Start();

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
	cbmrootlibs();

	// -----   Reconstruction run   -------------------------------------------
	FairRunAna *run= new FairRunAna();
	run->SetInputFile(mcFile);
	run->AddFriend(stsRecoFile);
//	run->AddFriend(trdHitsFile);
	run->AddFriend(muchHitsFile);
	run->AddFriend(tofHitsFile);
	run->SetOutputFile(globalTracksFile);
	// ------------------------------------------------------------------------

	CbmLitFindGlobalTracks* finder = new CbmLitFindGlobalTracks();
	finder->SetTrackingType("branch");
	finder->SetMergerType("nearest_hit");
	run->AddTask(finder);

	// -----   TRD track matching   --------------------------------------------
//	CbmTrdMatchTracks* trdMatchTracks = new CbmTrdMatchTracks(1);
//	run->AddTask(trdMatchTracks);

	// -----   MUCH track matching   --------------------------------------------
	CbmMuchMatchTracks* muchMatchTracks = new CbmMuchMatchTracks();
	run->AddTask(muchMatchTracks);

	// -----   Track finding QA check   ------------------------------------
	CbmLitReconstructionQa* reconstructionQa = new CbmLitReconstructionQa();
	reconstructionQa->SetMinNofPointsSts(4);
	reconstructionQa->SetMinNofPointsTrd(10);
	reconstructionQa->SetMinNofPointsMuch(12);
	reconstructionQa->SetMinNofPointsTof(1);
	reconstructionQa->SetQuota(0.7);
	reconstructionQa->SetVerbose(1);
	run->AddTask(reconstructionQa);
	// ------------------------------------------------------------------------

	// -----  Parameter database   --------------------------------------------
	FairRuntimeDb* rtdb = run->GetRuntimeDb();
	FairParRootFileIo* parIo1 = new FairParRootFileIo();
	parIo1->open(parFile.Data());
	rtdb->setFirstInput(parIo1);
	rtdb->setOutput(parIo1);
	rtdb->saveOutput();
	// ------------------------------------------------------------------------

	// -----   Initialize and run   --------------------------------------------
	run->LoadGeometry();
	run->Init();
	run->Run(0,nEvents);
	// ------------------------------------------------------------------------

	// -----   Finish   -------------------------------------------------------
	timer.Stop();
	cout << endl << endl;
	cout << "Macro finished successfully." << endl;
	cout << "Output file is "    << globalTracksFile << endl;
	cout << "Parameter file is " << parFile << endl;
	cout << "Real time " << timer.RealTime() << " s, CPU time " << timer.CpuTime() << " s" << endl;
	cout << endl;
	// ------------------------------------------------------------------------
}
