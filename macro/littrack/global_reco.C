/** global_reco.C
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 *
 * Macro runs STS hit and track reconstruction and TRD, MUCH and TOF digitizers
 * and hit finders and finally Littrack global tracking.
 * Macro has 3 options "all", "hits" and "tracking".
 **/

void global_reco(Int_t nEvents = 1000, // number of events
		TString opt = "hits")
// if opt == "all" STS + hit producers + global tracking are executed
// if opt == "hits" STS + hit producers are executed
// if opt == "tracking" global tracking is executed
{
	TString script = TString(gSystem->Getenv("SCRIPT"));
	TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");

	TString dir, imageDir, mcFile, parFile, globalRecoFile, muchDigiFile, trackingType;
	TList *parFileList = new TList();
	TObjString stsDigiFile, trdDigiFile;
	Int_t normStsPoints, normTrdPoints, normMuchPoints, normTofPoints;
	Int_t normTrdHits, normMuchHits, normTofHits;
	Float_t momMin, momMax;
	Int_t momBins;
	//Double_t trdHitErr = 100; // if == 0 than standard errors are used
	if (script != "yes") {
		// Output directory
//		dir = "/d/cbm02/andrey/std13_10mu_urqmd/";
		dir = "/d/cbm02/andrey/muon/std_10mu_urqmd/";
		// MC transport file
		mcFile = dir + "mc.0000.root";
		// Parameters file
		parFile = dir + "param.0000.root";
		// Output file with reconstructed tracks and hits
		globalRecoFile = dir + "global.reco.0000.root";
		// File with reconstructed STS tracks, STS, MUCH, TRD and TOF hits and digis
		globalHitsFile = dir + "global.hits.0000.root";
		// Output file with global tracks
		globalTracksFile = dir + "global.tracks.0000.root";
		// Digi scheme file for MUCH.
		// MUST be consistent with MUCH geometry used in MC transport.
		muchDigiFile = parDir + "/much/much_standard.digi.root";
		// Digi scheme for STS
		TObjString stsDigiFile = parDir + "/sts/sts_standard.digi.par";
		parFileList->Add(&stsDigiFile);
		// Digi scheme for TRD
		TObjString trdDigiFile = parDir + "/trd/trd_standard.digi.par";
		parFileList->Add(&trdDigiFile);
		// Directory for output images
		TString imageDir = "./test/";
		// Tracking type
		trackingType = "branch";
		// Normalization for efficiency
		normStsPoints = 4;
		normTrdPoints = 10;
		normMuchPoints = 12;
		normTofPoints = 1;
		normTrdHits = 9;
		normMuchHits = 11;
		normTofHits = 1;
		//
		momMin = 0.;
		momMax = 12.;
		momBins = 12.;
	} else {
		mcFile = TString(gSystem->Getenv("MCFILE"));
		parFile = TString(gSystem->Getenv("PARFILE"));
		globalRecoFile = TString(gSystem->Getenv("GLOBALRECOFILE"));
		globalHitsFile = TString(gSystem->Getenv("GLOBALHITSFILE"));
		globalTracksFile = TString(gSystem->Getenv("GLOBALTRACKSFILE"));
		muchDigiFile = TString(gSystem->Getenv("MUCHDIGI"));
		imageDir = TString(gSystem->Getenv("IMAGEDIR"));
		trackingType = TString(gSystem->Getenv("TRACKINGTYPE"));
		//trdHitErr = TString(gSystem->Getenv("TRDHITERR"))->Atof();
		TObjString stsDigiFile = TString(gSystem->Getenv("STSDIGI"));
		parFileList->Add(&stsDigiFile);
		TObjString trdDigiFile = TString(gSystem->Getenv("TRDDIGI"));
		parFileList->Add(&trdDigiFile);
		normStsPoints = TString(gSystem->Getenv("NORMSTSPOINTS"))->Atoi();
		normTrdPoints = TString(gSystem->Getenv("NORMTRDPOINTS"))->Atoi();
		normMuchPoints = TString(gSystem->Getenv("NORMMUCHPOINTS"))->Atoi();
		normTofPoints = TString(gSystem->Getenv("NORMTOFPOINTS"))->Atoi();
		normTrdHits = TString(gSystem->Getenv("NORMTRDHITS"))->Atoi();
		normMuchHits = TString(gSystem->Getenv("NORMMUCHHITS"))->Atoi();
		normTofHits = TString(gSystem->Getenv("NORMTOFHITS"))->Atoi();
		momMin = TString(gSystem->Getenv("MOMMIN"))->Atof();
		momMax = TString(gSystem->Getenv("MOMMAX"))->Atof();
		momBins = TString(gSystem->Getenv("MOMBINS"))->Atoi();
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

	FairRunAna *run = new FairRunAna();
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

		if (IsMvd(parFile)) {
			// ----- MVD reconstruction    --------------------------------------------
			CbmMvdDigitizeL* mvdDigi = new CbmMvdDigitizeL("MVD Digitiser", 0, iVerbose);
			run->AddTask(mvdDigi);

			CbmMvdFindHits* mvdHitFinder = new CbmMvdFindHits("MVD Hit Finder", 0, iVerbose);
			run->AddTask(mvdHitFinder);
			// -------------------------------------------------------------------------
		}

		// ----- STS reconstruction   ---------------------------------------------
		FairTask* stsDigitize =	new CbmStsIdealDigitize("STSDigitize", iVerbose);
		run->AddTask(stsDigitize);

		FairTask* stsClusterFinder = new CbmStsClusterFinder("STS Cluster Finder", iVerbose);
		run->AddTask(stsClusterFinder);

		FairTask* stsFindHits =	new CbmStsIdealFindHits("STSFindHits", iVerbose);
		run->AddTask(stsFindHits);

		FairTask* stsMatchHits = new CbmStsIdealMatchHits("STSMatchHits", iVerbose);
		run->AddTask(stsMatchHits);

		FairTask* kalman = new CbmKF();
		run->AddTask(kalman);
		FairTask* l1 = new CbmL1();
		run->AddTask(l1);
		CbmStsTrackFinder* trackFinder = new CbmL1StsTrackFinder();
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
			// ----- MUCH reconstruction ---------------------------------------
			CbmMuchDigitizeSimpleGem* muchDigitize =
					new CbmMuchDigitizeSimpleGem("MuchDigitize",
							muchDigiFile.Data(), iVerbose);
			run->AddTask(muchDigitize);
			CbmMuchDigitizeStraws* strawDigitize = new CbmMuchDigitizeStraws(
					"MuchDigitizeStraws", muchDigiFile.Data(), iVerbose);
			run->AddTask(strawDigitize);

			CbmMuchFindHitsSimpleGem* muchFindHits =
					new CbmMuchFindHitsSimpleGem("MuchFindHits",
							muchDigiFile.Data(), iVerbose);
			run->AddTask(muchFindHits);
			CbmMuchFindHitsStraws* strawFindHits = new CbmMuchFindHitsStraws(
					"MuchFindHitsStraws", muchDigiFile.Data(), iVerbose);
			run->AddTask(strawFindHits);
			// -----------------------------------------------------------------
		}

		if (IsTrd(parFile)) {
			// ----- TRD reconstruction-----------------------------------------
			// Update of the values for the radiator F.U. 17.08.07
			Int_t trdNFoils = 130; // number of polyetylene foils
			Float_t trdDFoils = 0.0013; // thickness of 1 foil [cm]
			Float_t trdDGap = 0.02; // thickness of gap between foils [cm]
			Bool_t simpleTR = kTRUE; // use fast and simple version for TR
			// production

			CbmTrdRadiator *radiator = new CbmTrdRadiator(simpleTR, trdNFoils, trdDFoils, trdDGap);

			Double_t trdSigmaX[] = { 300, 400, 500 }; // Resolution in x [mum]
			// Resolutions in y - station and angle dependent [mum]
			Double_t trdSigmaY1[] = { 2700, 3700, 15000, 27600, 33000, 33000, 33000 };
			Double_t trdSigmaY2[] = { 6300, 8300, 33000, 33000, 33000, 33000, 33000 };
			Double_t trdSigmaY3[] = { 10300, 15000, 33000, 33000, 33000, 33000, 33000 };

			// Double_t trdSigmaX[] = {trdHitErr, trdHitErr, trdHitErr};             // Resolution in x [mum]
			// // Resolutions in y - station and angle dependent [mum]
			// Double_t trdSigmaY1[] = {trdHitErr, trdHitErr, trdHitErr, trdHitErr, trdHitErr, trdHitErr, trdHitErr };
			// Double_t trdSigmaY2[] = {trdHitErr, trdHitErr, trdHitErr, trdHitErr, trdHitErr, trdHitErr, trdHitErr };
			// Double_t trdSigmaY3[] = {trdHitErr, trdHitErr, trdHitErr, trdHitErr, trdHitErr, trdHitErr, trdHitErr };

			CbmTrdHitProducerSmearing* trdHitProd =
					new CbmTrdHitProducerSmearing("TRD Hitproducer", "TRD task", radiator);
			// CbmTrdHitProducerSmearing* trdHitProd =
			// new CbmTrdHitProducerSmearing("TRD Hitproducer", "TRD task", NULL);

			trdHitProd->SetSigmaX(trdSigmaX);
			trdHitProd->SetSigmaY(trdSigmaY1, trdSigmaY2, trdSigmaY3);
			run->AddTask(trdHitProd);

//			CbmTrdDigitizer* trdDigitizer = new CbmTrdDigitizer(
//					"TRD Digitizer", "TRD task", radiator);
//			run->AddTask(trdDigitizer);
//
//			CbmTrdHitProducerDigi* trdHitProd = new CbmTrdHitProducerDigi(
//					"TRD Hit Producer", "TRD task");
//			run->AddTask(trdHitProd);
			// ------------------------------------------------------------------------
		}

		if (IsTof(parFile)) {
			// ------ TOF hits --------------------------------------------------------
			CbmTofHitProducer* tofHitProd = new CbmTofHitProducer(
					"TOF HitProducer", 1);
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
		reconstructionQa->SetMinNofPointsSts(normStsPoints);
		reconstructionQa->SetMinNofPointsTrd(normTrdPoints);
		reconstructionQa->SetMinNofPointsMuch(normMuchPoints);
		reconstructionQa->SetMinNofPointsTof(normTofPoints);
		reconstructionQa->SetQuota(0.7);
		reconstructionQa->SetMinNofHitsTrd(normTrdHits);
		reconstructionQa->SetMinNofHitsMuch(normMuchHits);
		reconstructionQa->SetVerbose(normTofHits);
		reconstructionQa->SetMomentumRange(momMin, momMax);
		reconstructionQa->SetNofBinsMom(momBins);
		reconstructionQa->SetOutputDir(std::string(imageDir));
		run->AddTask(reconstructionQa);
		// ------------------------------------------------------------------------
	}

	if (opt == "all") {
		// -----   Primary vertex finding   ---------------------------------------
		CbmPrimaryVertexFinder* pvFinder = new CbmPVFinderKF();
		CbmFindPrimaryVertex* findVertex = new CbmFindPrimaryVertex(pvFinder);
		run->AddTask(findVertex);
		// ------------------------------------------------------------------------
	}

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

	// -----   Initialize and run   --------------------------------------------
	run->LoadGeometry();
	run->Init();
	run->Run(0, nEvents);
	// ------------------------------------------------------------------------

	// -----   Finish   -------------------------------------------------------
	timer.Stop();
	cout << endl << endl;
	cout << "Macro finished successfully." << endl;
	cout << "Output file is " << globalRecoFile << endl;
	cout << "Parameter file is " << parFile << endl;
	cout << "Real time " << timer.RealTime() << " s, CPU time "
			<< timer.CpuTime() << " s" << endl;
	cout << endl;
	// ------------------------------------------------------------------------
}
