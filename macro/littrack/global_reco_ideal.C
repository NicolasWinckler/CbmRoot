void global_reco_ideal(Int_t nEvents = 1000)
{
	TString dir  = "/home/d/andrey/test/trunk/global_e/";
	TString mcFile = dir + "mc.0000.root";
	TString parFile = dir + "param.0000.root";
	TString globalTracksFile = dir + "global.tracks.ideal.0000.root";

	Bool_t isElectronSetup = true;

	Int_t iVerbose = 1;
	TStopwatch timer;
	timer.Start();

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
	cbmrootlibs();

	FairRunAna *run= new FairRunAna();
	run->SetInputFile(mcFile);
	run->SetOutputFile(globalTracksFile);

	TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");
	TString stsDigiFile = parDir+ "/sts/sts_Standard_s3055AAFK5.SecD.digi.par";
	TString muchDigiFile = parDir + "/much/much_standard.digi.root";

	// ----- STS reconstruction   ---------------------------------------------
	FairTask* stsDigitize = new CbmStsDigitize("STSDigitize", iVerbose);
	run->AddTask(stsDigitize);

	FairTask* stsFindHits = new CbmStsFindHits("STSFindHits", iVerbose);
	run->AddTask(stsFindHits);

	FairTask* stsMatchHits = new CbmStsMatchHits("STSMatchHits", iVerbose);
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

	if (!isElectronSetup) {
	// ----- MUCH reconstruction   --------------------------------------------
		CbmMuchDigitize* muchDigitize = new CbmMuchDigitize("MuchDigitize", muchDigiFile.Data(), iVerbose);
		muchDigitize->SetUseAvalanche(0); // 0 - Not account for avalanches; 1 - Account for avalanches
		//muchDigitize->SetMeanNoise(0);
		run->AddTask(muchDigitize);

		CbmMuchFindHits* muchFindHits = new CbmMuchFindHits("MuchFindHits", muchDigiFile.Data(), iVerbose);
		muchFindHits->SetUseClustering(0);
		run->AddTask(muchFindHits);

		CbmMuchTrackFinder* muchTrackFinder = new CbmMuchTrackFinderIdeal();
		CbmMuchFindTracks* muchFindTracks = new CbmMuchFindTracks("Much Track Finder");
		muchFindTracks->UseFinder(muchTrackFinder);
		run->AddTask(muchFindTracks);

		CbmMuchMatchTracks* muchMatchTracks = new CbmMuchMatchTracks();
		run->AddTask(muchMatchTracks);
	// ------------------------------------------------------------------------
	}

	if (isElectronSetup){
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

		CbmTrdHitProducerSmearing* trdHitProd = new
				 CbmTrdHitProducerSmearing("TRD Hitproducer", "TRD task", radiator);

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

	// ------ TOF reconstruction ----------------------------------------------
	CbmTofHitProducer* tofHitProd = new CbmTofHitProducer("TOF HitProducer", 1);
	run->AddTask(tofHitProd);
	// ------------------------------------------------------------------------

	// ------ Ideal global tracking -------------------------------------------
	CbmLitFindGlobalTracksIdeal* findGlobal = new CbmLitFindGlobalTracksIdeal();
	run->AddTask(findGlobal);
	// ------------------------------------------------------------------------

	// ------- Track finding QA check   ---------------------------------------
	CbmLitReconstructionQa* reconstructionQa = new CbmLitReconstructionQa();
	reconstructionQa->SetMinNofPointsSts(4);
	reconstructionQa->SetMinNofPointsTrd(8);
	reconstructionQa->SetMinNofPointsMuch(12);
	reconstructionQa->SetMinNofPointsTof(1);
	reconstructionQa->SetQuota(0.7);
	reconstructionQa->SetVerbose(1);
	run->AddTask(reconstructionQa);
	// ------------------------------------------------------------------------

	// -----  Parameter database   --------------------------------------------
	FairRuntimeDb* rtdb = run->GetRuntimeDb();
	FairParRootFileIo* parIo1 = new FairParRootFileIo();
	FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
	parIo1->open(parFile.Data());
	parIo2->open(stsDigiFile.Data(),"in");
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
