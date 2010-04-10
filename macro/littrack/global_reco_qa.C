/** global_reco_qa.C
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2007
 * @version 3.0
 * Macro draws histograms produced by CbmLitReconstructionQa class.
 **/

void global_reco_qa(Int_t nEvents = 10)
{
	TString script = TString(gSystem->Getenv("SCRIPT"));

    	TString dir, imageDir, mcFile, parFile, globalHitsFile, globalTracksFile, trackingType;
	if (script != "yes") {
		// Output directory
		dir  = "/home/d/andrey/trdsimple_10pi/";
		//Output directory for images
		imageDir = "./test/";
		// MC transport file
		mcFile = dir + "mc.0000.root";
		// Parameter file
		parFile = dir + "param.0000.root";
		// File with reconstructed STS tracks, STS, MUCH, TRD and TOF hits and digis
		globalHitsFile = dir + "global.hits.trd10.0000.root";
		// File with global tracks
		globalTracksFile = dir + "global.tracks.0000.root";
		// Output file with histograms
                recoQaFile = dir + "reco.qa.trd100.0000.root";
	} else {
		mcFile = TString(gSystem->Getenv("MCFILE"));
		parFile = TString(gSystem->Getenv("PARFILE"));
		globalHitsFile = TString(gSystem->Getenv("GLOBALHITSFILE"));
		globalTracksFile = TString(gSystem->Getenv("GLOBALTRACKSFILE"));
		imageDir = TString(gSystem->Getenv("IMAGEDIR"));
                recoQaFile = TString(gSystem->Getenv("RECOQAFILE"));
	}

	TStopwatch timer;
	timer.Start();

	gSystem->Load("/home/soft/tbb/libtbb");
	gSystem->Load("/u/andrey/soft/tbb/Lenny64/libtbb");
//	gSystem->Load("/u/andrey/soft/tbb/Etch32/libtbb");

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
	cbmrootlibs();


	// -----   Reconstruction run   -------------------------------------------
	FairRunAna *run= new FairRunAna();
	run->SetInputFile(mcFile);
	run->AddFriend(globalHitsFile);
	run->AddFriend(globalTracksFile);
	run->SetOutputFile(recoQaFile);
	// ------------------------------------------------------------------------

	// -----   Track finding QA check   ------------------------------------
	CbmLitReconstructionQa* reconstructionQa = new CbmLitReconstructionQa();
	reconstructionQa->SetMinNofPointsSts(4);
	reconstructionQa->SetMinNofPointsTrd(9);
	reconstructionQa->SetMinNofPointsMuch(0);
	reconstructionQa->SetMinNofPointsTof(0);
	reconstructionQa->SetQuota(0.7);
	reconstructionQa->SetMinNofHitsTrd(9);
	reconstructionQa->SetMinNofHitsMuch(0);
	reconstructionQa->SetVerbose(1);
	reconstructionQa->SetMomentumRange(0., 15);
	reconstructionQa->SetNofBinsMom(15);
	reconstructionQa->SetOutputDir(std::string(imageDir));
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
	// ------------------------------------------------------------------------

	// -----   Finish   -------------------------------------------------------
	timer.Stop();
	cout << endl << endl;
	cout << "Macro finished successfully." << endl;
	cout << "Output file is " << recoQaFile << endl;
	cout << "Parameter file is " << parFile << endl;
	cout << "Real time " << timer.RealTime() << " s, CPU time " << timer.CpuTime() << " s" << endl;
	cout << endl;
	// ------------------------------------------------------------------------

}
