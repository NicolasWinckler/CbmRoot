/** much_reco.C
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 *
 * Macro runs STS hit and track reconstruction and TRD, MUCH and TOF digitizers
 * and hit finders and finally Littrack global tracking.
 * Instead of this macro one may also run macro/littrack/global_hits.C
 * and macro/littrack/global_tracking.C consequently, to run Littrack
 * global tracking independently.
 **/

void much_reco(
		Int_t nEvents = 100, // number of events
		TString opt = "all") // if opt == "all" STS + hit producers + global tracking are executed
                             // if opt == "hits" STS + hit producers are executed
                             // if opt == "tracking" global tracking is executed
{
	TString script = TString(gSystem->Getenv("SCRIPT"));
	TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");

	TString dir, imageDir, mcFile, parFile, globalRecoFile, stsDigiFile, muchDigiFile, trackingType;
	//Double_t trdHitErr = 100; // if == 0 than standard errors are used
	if (script != "yes") {
		// Output directory
		dir  = "data/";
		// MC transport file
		mcFile = dir + "Jpsi.auau.25gev.centr.mc.root";
		// Parameters file
		parFile = dir + "params.root";
		// Output file with reconstructed tracks and hits
		globalRecoFile = dir + "global.reco.0000.root";
		// File with reconstructed STS tracks, STS, MUCH, TRD and TOF hits and digis
		globalHitsFile = dir + "global.hits.0000.root";
		// Output file with global tracks
		globalTracksFile = dir + "global.tracks.0000.root";
		// Digi scheme file for STS.
		// MUST be consistent with STS geometry used in MC transport.
		stsDigiFile = parDir+ "/sts/sts_standard.digi.par";
		// Digi scheme file for MUCH.
		// MUST be consistent with MUCH geometry used in MC transport.
		muchDigiFile = parDir + "/much/much_standard.digi.root";
		// Directory for output images
		TString imageDir = "./data/";
		// Tracking type
		trackingType = "branch";
	} else {
		mcFile = TString(gSystem->Getenv("MCFILE"));
		parFile = TString(gSystem->Getenv("PARFILE"));
		globalRecoFile = TString(gSystem->Getenv("GLOBALRECOFILE"));
		globalHitsFile = TString(gSystem->Getenv("GLOBALHITSFILE"));
		globalTracksFile = TString(gSystem->Getenv("GLOBALTRACKSFILE"));
		stsDigiFile = TString(gSystem->Getenv("STSDIGI"));
		muchDigiFile = TString(gSystem->Getenv("MUCHDIGI"));
		imageDir = TString(gSystem->Getenv("IMAGEDIR"));
		trackingType = TString(gSystem->Getenv("TRACKINGTYPE"));
		//trdHitErr = TString(gSystem->Getenv("TRDHITERR"))->Atof();
	}

	Int_t iVerbose = 1;
	TStopwatch timer;
	timer.Start();

//	gSystem->Load("/home/soft/tbb/libtbb");
//	gSystem->Load("/u/andrey/soft/tbb/Lenny64/libtbb");
//	gSystem->Load("/u/andrey/soft/tbb/Etch32/libtbb");

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
	cbmrootlibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/determine_setup.C");

	FairRunAna *run= new FairRunAna();
	if (opt == "all") {
		run->SetInputFile(mcFile);
		run->SetOutputFile(globalRecoFile);
	} else if (opt == "hits") {
		run->SetInputFile(mcFile);
		run->SetOutputFile(globalHitsFile);
	} else if (opt == "tracking") {
		run->SetInputFile(mcFile);
		run->AddFriend(globalHitsFile);
		run->SetOutputFile(globalTracksFile);
	} else {
		std::cout << "-E- Incorrect opt parameter" << std::endl;
		exit();
	}

	if (opt == "all" || opt == "hits") {
		// ----- STS reconstruction   ---------------------------------------------
		FairTask* stsDigitize = new CbmStsIdealDigitize("STSDigitize", iVerbose);
		run->AddTask(stsDigitize);

	//  FairTask* stsClusterFinder = new CbmStsClusterFinder("STS Cluster Finder", iVerbose);
	//  run->AddTask(stsClusterFinder);

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

		//	FairTask* stsFHQa = new CbmStsFindHitsQa("STSFindHitsQA",iVerbose);
		//	run->AddTask(stsFHQa);

		//  FairTask* stsRecoQa = new CbmStsReconstructionQa(kFALSE, 4, 0.7, 1);
		//	run->AddTask(stsRecoQa);
		// ------------------------------------------------------------------------

		if (IsMuch(parFile)) {
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

		if (IsTrd(parFile)){
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

	//		Double_t trdSigmaX[] = {trdHitErr, trdHitErr, trdHitErr};             // Resolution in x [mum]
	//		// Resolutions in y - station and angle dependent [mum]
	//		Double_t trdSigmaY1[] = {trdHitErr, trdHitErr, trdHitErr, trdHitErr, trdHitErr, trdHitErr, trdHitErr };
	//		Double_t trdSigmaY2[] = {trdHitErr, trdHitErr, trdHitErr, trdHitErr, trdHitErr, trdHitErr, trdHitErr };
	//		Double_t trdSigmaY3[] = {trdHitErr, trdHitErr, trdHitErr, trdHitErr, trdHitErr, trdHitErr, trdHitErr };

			CbmTrdHitProducerSmearing* trdHitProd = new
					 CbmTrdHitProducerSmearing("TRD Hitproducer", "TRD task", radiator);
	//		CbmTrdHitProducerSmearing* trdHitProd = new
	//						 CbmTrdHitProducerSmearing("TRD Hitproducer", "TRD task", NULL);

			trdHitProd->SetSigmaX(trdSigmaX);
			trdHitProd->SetSigmaY(trdSigmaY1, trdSigmaY2, trdSigmaY3);
			run->AddTask(trdHitProd);
		// ------------------------------------------------------------------------
		}

		if (IsTof(parFile)) {
		// ------ TOF hits --------------------------------------------------------
			CbmTofHitProducer* tofHitProd = new CbmTofHitProducer("TOF HitProducer", 1);
			run->AddTask(tofHitProd);
		// ------------------------------------------------------------------------
		}
	}

	if (opt == "all" || opt == "tracking") {
		// ------ Global track reconstruction -------------------------------------
		//	FairGeane* Geane = new FairGeane(parFile.Data());
		//	FairGeane *Geane = new FairGeane();
		//	run->AddTask(Geane);

		CbmLitFindGlobalTracks* finder = new CbmLitFindGlobalTracks();
		// Tracking method to be used
		// "branch" - branching tracking
		// "nn" - nearest neighbor tracking
		// "weight" - weighting tracking
		finder->SetTrackingType(std::string(trackingType));

		// Hit-to-track merger method to be used
		// "nearest_hit" - assigns nearest hit to the track
		finder->SetMergerType("nearest_hit");

		run->AddTask(finder);

		if (IsTrd(parFile)) {
			CbmTrdMatchTracks* trdMatchTracks = new CbmTrdMatchTracks(1);
			run->AddTask(trdMatchTracks);

//			CbmTrdSetTracksPidANN* trdSetTracksPidAnnTask = new CbmTrdSetTracksPidANN("Ann","Ann");
//			run->AddTask(trdSetTracksPidAnnTask);
		}

		if (IsMuch(parFile)) {
			CbmMuchMatchTracks* muchMatchTracks = new CbmMuchMatchTracks();
			run->AddTask(muchMatchTracks);
		}

		// -----   Track finding QA check   ------------------------------------
		CbmLitReconstructionQa* reconstructionQa = new CbmLitReconstructionQa();
		reconstructionQa->SetMinNofPointsSts(4);
		reconstructionQa->SetMinNofPointsTrd(10);
		reconstructionQa->SetMinNofPointsMuch(11);
		reconstructionQa->SetMinNofPointsTof(1);
		reconstructionQa->SetQuota(0.7);
		reconstructionQa->SetMinNofHitsTrd(9);
		reconstructionQa->SetMinNofHitsMuch(11);
		reconstructionQa->SetVerbose(1);
		reconstructionQa->SetMomentumRange(0., 25);
		reconstructionQa->SetNofBinsMom(25);
		reconstructionQa->SetOutputDir(std::string(imageDir));
		run->AddTask(reconstructionQa);
		// ------------------------------------------------------------------------
	}

	// -----  Parameter database   --------------------------------------------
	FairRuntimeDb* rtdb = run->GetRuntimeDb();
	FairParRootFileIo* parIo1 = new FairParRootFileIo();
	FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
	parIo1->open(parFile);
	parIo2->open(stsDigiFile.Data(),"in");
	rtdb->setFirstInput(parIo1);
	rtdb->setSecondInput(parIo2);
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
	cout << "Output file is "    << globalRecoFile << endl;
	cout << "Parameter file is " << parFile << endl;
	cout << "Real time " << timer.RealTime() << " s, CPU time " << timer.CpuTime() << " s" << endl;
	cout << endl;
	// ------------------------------------------------------------------------
}
