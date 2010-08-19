void global_reco_ideal(Int_t nEvents = 1000)
{
	TString script = TString(gSystem->Getenv("SCRIPT"));
	TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString(
			"/parameters");

	Double_t trdHitErr = 100; // if == 0 than standard errors are used
	TString dir, imageDir, mcFile, parFile, globalTracksFile;
	TList *parFileList = new TList();
	TObjString stsDigiFile, trdDigiFile;
	if (script != "yes") {
		dir  = "/d/cbm02/andrey/test_electrons_fit/";
		mcFile = dir + "mc.0000.root";
		parFile = dir + "param.0000.root";
		globalTracksFile = dir + "global.tracks.ideal.0000.root";
		muchDigiFile = parDir + "/much/much_standard_2layers.digi.root";
		TObjString stsDigiFile = parDir + "/sts/sts_standard.digi.par";
		parFileList->Add(&stsDigiFile);
		TObjString trdDigiFile = parDir + "/trd/trd_standard.digi.par";
		parFileList->Add(&trdDigiFile);
		imageDir = "./test_electrons_fit/";
	} else {
		mcFile = TString(gSystem->Getenv("MCFILE"));
		parFile = TString(gSystem->Getenv("PARFILE"));
		globalTracksFile = TString(gSystem->Getenv("GLOBALTRACKSIDEALFILE"));
		imageDir = TString(gSystem->Getenv("IMAGEDIR"));
		muchDigiFile = TString(gSystem->Getenv("MUCHDIGI"));
		//trdHitErr = TString(gSystem->Getenv("TRDHITERR"))->Atof();
		TObjString stsDigiFile = TString(gSystem->Getenv("STSDIGI"));
		parFileList->Add(&stsDigiFile);
		TObjString trdDigiFile = TString(gSystem->Getenv("TRDDIGI"));
		parFileList->Add(&trdDigiFile);
	}

	Int_t iVerbose = 1;
	TStopwatch timer;
	timer.Start();

	gSystem->Load("/home/soft/tbb/libtbb");
	gSystem->Load("/u/andrey/soft/tbb/Lenny64/libtbb");
	gSystem->Load("/u/andrey/soft/tbb/Etch32/libtbb");

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
	cbmrootlibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/determine_setup.C");

	FairRunAna *run= new FairRunAna();
	run->SetInputFile(mcFile);
	run->SetOutputFile(globalTracksFile);

	// ----- STS reconstruction   ---------------------------------------------
	FairTask* stsDigitize = new CbmStsIdealDigitize("STSDigitize", iVerbose);
	run->AddTask(stsDigitize);

//        FairTask* stsClusterFinder = new CbmStsClusterFinder("STS Cluster Finder", iVerbose);
//        run->AddTask(stsClusterFinder);

	FairTask* stsFindHits = new CbmStsIdealFindHits("STSFindHits", iVerbose);
	run->AddTask(stsFindHits);

	FairTask* stsMatchHits = new CbmStsIdealMatchHits("STSMatchHits", iVerbose);
	run->AddTask(stsMatchHits);

	FairTask* kalman= new CbmKF();
	run->AddTask(kalman);
	FairTask* l1 = new CbmL1();
	run->AddTask(l1);
	CbmStsTrackFinder* trackFinder    = new CbmL1StsTrackFinder();
//	CbmStsTrackFinder* trackFinder    = new CbmStsTrackFinderIdeal();
	FairTask* findTracks = new CbmStsFindTracks(iVerbose, trackFinder);
	run->AddTask(findTracks);

	FairTask* stsMatchTracks = new CbmStsMatchTracks("STSMatchTracks", iVerbose);
	run->AddTask(stsMatchTracks);

	CbmStsTrackFitter* trackFitter = new CbmStsKFTrackFitter();
	FairTask* fitTracks = new CbmStsFitTracks("STS Track Fitter", trackFitter, iVerbose);
	run->AddTask(fitTracks);
	// ------------------------------------------------------------------------

	if (IsMuch(parFile)) {
	// ----- MUCH reconstruction   --------------------------------------------
		CbmMuchDigitizeSimpleGem* muchDigitize = new CbmMuchDigitizeSimpleGem("MuchDigitize", muchDigiFile.Data(), iVerbose);
		run->AddTask(muchDigitize);
		CbmMuchDigitizeStraws* strawDigitize = new CbmMuchDigitizeStraws("MuchDigitizeStraws", muchDigiFile.Data(), iVerbose);
		run->AddTask(strawDigitize);

		CbmMuchFindHitsSimpleGem* muchFindHits = new CbmMuchFindHitsSimpleGem("MuchFindHits", muchDigiFile.Data(), iVerbose);
		run->AddTask(muchFindHits);
		CbmMuchFindHitsStraws* strawFindHits = new CbmMuchFindHitsStraws("MuchFindHitsStraws", muchDigiFile.Data(), iVerbose);
		run->AddTask(strawFindHits);

		CbmMuchTrackFinder* muchTrackFinder = new CbmMuchTrackFinderIdeal();
		CbmMuchFindTracks* muchFindTracks = new CbmMuchFindTracks("Much Track Finder");
		muchFindTracks->UseFinder(muchTrackFinder);
		run->AddTask(muchFindTracks);

		CbmMuchMatchTracks* muchMatchTracks = new CbmMuchMatchTracks();
		run->AddTask(muchMatchTracks);
	// ------------------------------------------------------------------------
	}

	if (IsTrd(parFile)){
	// ----- TRD reconstruction -- --------------------------------------------
		// Update of the values for the radiator F.U. 17.08.07
		Int_t trdNFoils    = 130;      // number of polyetylene foils
		Float_t trdDFoils = 0.0013;    // thickness of 1 foil [cm]
		Float_t trdDGap   = 0.02;      // thickness of gap between foils [cm]
		Bool_t simpleTR = kTRUE;       // use fast and simple version for TR
									   // production

		CbmTrdRadiator *radiator = new CbmTrdRadiator(simpleTR , trdNFoils,	 trdDFoils, trdDGap);

		Double_t trdSigmaX[] = {300, 400, 500};             // Resolution in x [mum]
		// Resolutions in y - station and angle dependent [mum]
		Double_t trdSigmaY1[] = {2700,   3700, 15000, 27600, 33000, 33000, 33000 };
		Double_t trdSigmaY2[] = {6300,   8300, 33000, 33000, 33000, 33000, 33000 };
		Double_t trdSigmaY3[] = {10300, 15000, 33000, 33000, 33000, 33000, 33000 };

//		Double_t trdSigmaX[] = {trdHitErr, trdHitErr, trdHitErr};             // Resolution in x [mum]
//		// Resolutions in y - station and angle dependent [mum]
//		Double_t trdSigmaY1[] = {trdHitErr, trdHitErr, trdHitErr, trdHitErr, trdHitErr, trdHitErr, trdHitErr};
//		Double_t trdSigmaY2[] = {trdHitErr, trdHitErr, trdHitErr, trdHitErr, trdHitErr, trdHitErr, trdHitErr};
//		Double_t trdSigmaY3[] = {trdHitErr, trdHitErr, trdHitErr, trdHitErr, trdHitErr, trdHitErr, trdHitErr};

		CbmTrdHitProducerSmearing* trdHitProd = new
				 CbmTrdHitProducerSmearing("TRD Hitproducer", "TRD task", radiator);
//		CbmTrdHitProducerSmearing* trdHitProd = new
//				 CbmTrdHitProducerSmearing("TRD Hitproducer", "TRD task", NULL);

		trdHitProd->SetSigmaX(trdSigmaX);
		trdHitProd->SetSigmaY(trdSigmaY1, trdSigmaY2, trdSigmaY3);
		run->AddTask(trdHitProd);

		CbmTrdTrackFinder* trdTrackFinder    = new CbmTrdTrackFinderIdeal();
		CbmTrdFindTracks* trdFindTracks = new CbmTrdFindTracks("TRDTrackFinder");
		trdFindTracks->UseFinder(trdTrackFinder);
		run->AddTask(trdFindTracks);

		CbmTrdMatchTracks* trdMatchTracks = new CbmTrdMatchTracks(1);
		run->AddTask(trdMatchTracks);
	// ------------------------------------------------------------------------
	}

	if (IsTof(parFile)) {
	// ------ TOF reconstruction ----------------------------------------------
		CbmTofHitProducer* tofHitProd = new CbmTofHitProducer("TOF HitProducer", 1);
		run->AddTask(tofHitProd);
	// ------------------------------------------------------------------------
	}

	// ------ Ideal global tracking -------------------------------------------
	CbmLitFindGlobalTracksIdeal* findGlobal = new CbmLitFindGlobalTracksIdeal();
	run->AddTask(findGlobal);
	// ------------------------------------------------------------------------

	// ------- Track finding QA check   ---------------------------------------
	CbmLitReconstructionQa* reconstructionQa = new CbmLitReconstructionQa();
	reconstructionQa->SetMinNofPointsSts(4);
	reconstructionQa->SetMinNofPointsTrd(9);
	reconstructionQa->SetMinNofPointsMuch(11);
	reconstructionQa->SetMinNofPointsTof(1);
	reconstructionQa->SetQuota(0.7);
	reconstructionQa->SetMinNofHitsTrd(9);
	reconstructionQa->SetMinNofHitsMuch(10);
	reconstructionQa->SetVerbose(1);
	reconstructionQa->SetMomentumRange(0., 25);
	reconstructionQa->SetNofBinsMom(25);
	reconstructionQa->SetOutputDir(std::string(imageDir));
	run->AddTask(reconstructionQa);
	// ------------------------------------------------------------------------

	// -----  Parameter database   --------------------------------------------
	FairRuntimeDb* rtdb = run->GetRuntimeDb();
	FairParRootFileIo* parIo1 = new FairParRootFileIo();
	FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
	parIo1->open(parFile.Data());
	parIo2->open(parFileList, "in");
	rtdb->setFirstInput(parIo1);
	rtdb->setSecondInput(parIo2);
	rtdb->setOutput(parIo1);
	rtdb->saveOutput();
	// ------------------------------------------------------------------------

	// -----   Intialise and run   --------------------------------------------
	run->LoadGeometry();
	run->Init();
	run->Run(0, nEvents);
	// ------------------------------------------------------------------------

	// -----   Finish   -------------------------------------------------------
	timer.Stop();
	cout << endl << endl;
	cout << "Macro finished succesfully." << endl;
	cout << "Output file is " << globalTracksFile << endl;
	cout << "Parameter file is " << parFile << endl;
	cout << "Real time " << timer.RealTime() << " s, CPU time " << timer.CpuTime() << " s" << endl;
	cout << endl;
	// ------------------------------------------------------------------------
}
