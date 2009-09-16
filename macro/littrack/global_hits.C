void global_hits(Int_t nEvents = 1000)
{
	TString script = TString(gSystem->Getenv("SCRIPT"));
	TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");

	TString dir, mcFile, parFile, globalHitsFile, muchDigiFile;
	if (script != "yes") {
		dir  = "/home/d/andrey/test/trunk/global_mu_low/";
		mcFile = dir + "mc.0000.root";
		parFile = dir + "param.0000.root";
		globalHitsFile = dir + "global.hits.0000.root";
		muchDigiFile = parDir + "/much/much_standard.digi.root";
//		muchDigiFile = parDir + "/much/much_standard_trd.digi.root";
//		muchDigiFile = parDir + "/much/much_standard_straw.digi.root";
	} else {
		mcFile = TString(gSystem->Getenv("MCFILE"));
		parFile = TString(gSystem->Getenv("PARFILE"));
		globalHitsFile = TString(gSystem->Getenv("GLOBALHITSFILE"));
		muchDigiFile = TString(gSystem->Getenv("MUCHDIGI"));
	}

	Int_t iVerbose = 1;
	TStopwatch timer;
	timer.Start();

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
	cbmrootlibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/determine_setup.C");

	FairRunAna *run= new FairRunAna();
	run->SetInputFile(mcFile);
	run->SetOutputFile(globalHitsFile);

	TString stsDigiFile = parDir+ "/sts/sts_standard.digi.par";

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

	if (IsMuch(mcFile)) {
	// ----- MUCH hits----------   --------------------------------------------
		CbmMuchDigitizeSimpleGem* muchDigitize = new CbmMuchDigitizeSimpleGem("MuchDigitize", muchDigiFile.Data(), iVerbose);
		run->AddTask(muchDigitize);
		CbmMuchDigitizeStraws* strawDigitize = new CbmMuchDigitizeStraws("MuchDigitizeStraws", muchDigiFile.Data(), iVerbose);
		run->AddTask(strawDigitize);

		CbmMuchFindHitsSimpleGem* muchFindHits = new CbmMuchFindHitsSimpleGem("MuchFindHits", muchDigiFile.Data(), iVerbose);
		run->AddTask(muchFindHits);
		CbmMuchFindHitsStraws* strawFindHits = new CbmMuchFindHitsStraws("MuchFindHitsStraws", muchDigiFile.Data(), iVerbose);
		run->AddTask(strawFindHits);
	// ------------------------------------------------------------------------
	}

	if (IsTrd(mcFile)){
	// ----- TRD hits ---------------------------------------------------------
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
	// ------------------------------------------------------------------------
	}

	if (IsTof(mcFile)) {
	// ------ TOF hits --------------------------------------------------------
		CbmTofHitProducer* tofHitProd = new CbmTofHitProducer("TOF HitProducer", 1);
		run->AddTask(tofHitProd);
	// ------------------------------------------------------------------------
	}

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
	cout << "Output file is " << globalHitsFile << endl;
	cout << "Parameter file is " << parFile << endl;
	cout << "Real time " << timer.RealTime() << " s, CPU time " << timer.CpuTime() << " s" << endl;
	cout << endl;
	// ------------------------------------------------------------------------
}
