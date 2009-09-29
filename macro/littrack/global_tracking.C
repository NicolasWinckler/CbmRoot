/** global_tracking.C
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 *
 * Macro runs Littrack global track reconstruction.
 **/

void global_tracking(Int_t nEvents = 100)
{
	TString script = TString(gSystem->Getenv("SCRIPT"));

	TString dir, mcFile, parFile, globalHitsFile, globalTracksFile;
	if (script != "yes") {
		// Output directory
		dir  = "/home/d/andrey/std_10mu_urqmd/";
		// MC transport file
		mcFile = dir + "mc.0000.root";
		// Parameter file
		parFile = dir + "param.0000.root";
		// File with reconstructed STS tracks, STS, MUCH, TRD and TOF hits and digis
		globalHitsFile = dir + "global.hits.0000.root";
		// Output file with global tracks
		globalTracksFile = dir + "global.tracks.0000.root";
	} else {
		mcFile = TString(gSystem->Getenv("MCFILE"));
		parFile = TString(gSystem->Getenv("PARFILE"));
		globalHitsFile = TString(gSystem->Getenv("GLOBALHITSFILE"));
		globalTracksFile = TString(gSystem->Getenv("GLOBALTRACKSFILE"));
	}

	Int_t iVerbose = 1;
	TStopwatch timer;
	timer.Start();

	gSystem->Load("/home/soft/tbb22_004oss/libtbb");

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
	finder->SetTrackingType("nn_parallel");

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
	reconstructionQa->SetMinNofPointsMuch(11);
	reconstructionQa->SetMinNofPointsTof(0);
	reconstructionQa->SetQuota(0.7);
	reconstructionQa->SetMinNofHitsTrd(8);
	reconstructionQa->SetMinNofHitsMuch(11);
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
	run->Run(0, nEvents);
//	run->Run(98, 100);
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
