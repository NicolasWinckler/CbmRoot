/** global_reco_qa.C
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 * Macro runs CbmLitReconstructionQa class.
 **/

void global_reco_qa(Int_t nEvents = 500)
{
	TString script = TString(gSystem->Getenv("SCRIPT"));

    	TString dir, imageDir, mcFile, parFile, globalHitsFile, globalTracksFile, trackingType;
	if (script != "yes") {
		// Output directory
		dir  = "/data.local1/andrey/events/std_electron/";
		//Output directory for images
		imageDir = "./sts_qa/4conspoints/";
		// MC transport file
		mcFile = dir + "mc.0000.root";
		// Parameter file
		parFile = dir + "param.0000.root";
		// File with reconstructed STS tracks, STS, MUCH, TRD and TOF hits and digis
		globalHitsFile = dir + "global.hits.0000.root";
		// File with global tracks
		globalTracksFile = dir + "global.tracks.0000.root";
		// Output file with histograms
      recoQaFile = dir + "reco.qa.4conspoints.0000.root";
	} else {
//		mcFile = TString(gSystem->Getenv("MCFILE"));
//		parFile = TString(gSystem->Getenv("PARFILE"));
		//globalHitsFile = TString(gSystem->Getenv("GLOBALHITSFILE"));
		//globalTracksFile = TString(gSystem->Getenv("GLOBALTRACKSFILE"));
//      recoFile = TString(gSystem->Getenv("RECOFILE"));
//		imageDir = TString(gSystem->Getenv("IMAGEDIR"));
//      recoQaFile = TString(gSystem->Getenv("RECOQAFILE"));
	}

	TStopwatch timer;
	timer.Start();

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
	cbmrootlibs();


	// -----   Reconstruction run   -------------------------------------------
	FairRunAna *run= new FairRunAna();
	run->SetInputFile(mcFile);
	run->AddFriend(globalHitsFile);
	run->AddFriend(globalTracksFile);
//	run->AddFriend(recoFile);
	run->SetOutputFile(recoQaFile);
	// ------------------------------------------------------------------------

	CbmKF* kalman = new CbmKF();
	run->AddTask(kalman);
	CbmL1* l1 = new CbmL1();
	run->AddTask(l1);

	// -----   Track finding QA check   ------------------------------------
   CbmLitReconstructionQa* reconstructionQa = new CbmLitReconstructionQa();
   reconstructionQa->SetUseConsecutivePointsInSts(true);
   reconstructionQa->SetMinNofPointsSts(4);
   reconstructionQa->SetMinNofPointsTrd(8);
   reconstructionQa->SetMinNofPointsMuch(10);
   reconstructionQa->SetMinNofPointsTof(1);
   reconstructionQa->SetQuota(0.7);
   reconstructionQa->SetMinNofHitsTrd(8);
   reconstructionQa->SetMinNofHitsMuch(10);
   reconstructionQa->SetVerbose(0);
   reconstructionQa->SetMomentumRange(0, 12);
   reconstructionQa->SetNofBinsMom(12);
   reconstructionQa->SetMinNofHitsRich(7);
   reconstructionQa->SetQuotaRich(0.6);
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
