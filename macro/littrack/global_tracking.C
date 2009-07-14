void global_tracking(Int_t nEvents = 1000)
{
	TString script = TString(gSystem->Getenv("SCRIPT"));

	TString dir, mcFile, parFile, globalHitsFile, globalTracksFile;
	if (script != "yes") {
		dir  = "/home/d/andrey/test/trunk/global_mon_mu/";
		mcFile = dir + "mc.0000.root";
		parFile = dir + "param.0000.root";
		globalHitsFile = dir + "global.hits.0000.root";
		globalTracksFile = dir + "global.tracks.test.0000.root";
	} else {
		mcFile = TString(gSystem->Getenv("MCFILE"));
		parFile = TString(gSystem->Getenv("PARFILE"));
		globalHitsFile = TString(gSystem->Getenv("GLOBALHITSFILE"));
		globalTracksFile = TString(gSystem->Getenv("GLOBALTRACKSFILE"));
	}

	Int_t iVerbose = 1;
	TStopwatch timer;
	timer.Start();

//	gSystem->Load("/home/soft/temp/tbb21_017oss/libtbb");

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
	cbmrootlibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/determine_setup.C");


	// -----   Reconstruction run   -------------------------------------------
	FairRunAna *run= new FairRunAna();
	run->SetInputFile(mcFile);
	run->AddFriend(globalHitsFile);
	run->SetOutputFile(globalTracksFile);
	// ------------------------------------------------------------------------

	CbmLitFindGlobalTracks* finder = new CbmLitFindGlobalTracks();
	// Tracking method to be used
	// "branch" - branching tracking
	// "nn" - nearest neighbor tracking
	// "weight" - weighting tracking
	finder->SetTrackingType("branch");

	// Hit-to-track merger method to be used
	// "nearest_hit" - assigns nearest hit to the track
	finder->SetMergerType("nearest_hit");

	run->AddTask(finder);

	if (IsTrd(mcFile)) {
		CbmTrdMatchTracks* trdMatchTracks = new CbmTrdMatchTracks(1);
		run->AddTask(trdMatchTracks);
	}

	if (IsMuch(mcFile)) {
		CbmMuchMatchTracks* muchMatchTracks = new CbmMuchMatchTracks();
		run->AddTask(muchMatchTracks);
	}

	// -----   Track finding QA check   ------------------------------------
	CbmLitReconstructionQa* reconstructionQa = new CbmLitReconstructionQa();
	reconstructionQa->SetMinNofPointsSts(4);
	reconstructionQa->SetMinNofPointsTrd(10);
	reconstructionQa->SetMinNofPointsMuch(9);
	reconstructionQa->SetMinNofPointsTof(1);
	reconstructionQa->SetQuota(0.7);
	reconstructionQa->SetMinNofHitsTrd(8);
	reconstructionQa->SetMinNofHitsMuch(8);
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
	cout << "Output file is " << globalTracksFile << endl;
	cout << "Parameter file is " << parFile << endl;
	cout << "Real time " << timer.RealTime() << " s, CPU time " << timer.CpuTime() << " s" << endl;
	cout << endl;
	// ------------------------------------------------------------------------
}
