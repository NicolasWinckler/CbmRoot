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
		TString opt = "all")
   // if opt == "all" STS + hit producers + global tracking are executed
   // if opt == "hits" STS + hit producers are executed
   // if opt == "tracking" global tracking is executed
{
	TString script = TString(gSystem->Getenv("SCRIPT"));
	TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");

	TString dir, imageDir, mcFile, parFile, globalRecoFile, globalTracksFile,
	   globalHitsFile, stsDigiFile, muchDigiFile, trackingType;
	if (script != "yes") {
		// Output directory
		dir  = "data/";
		// MC transport file
		mcFile = dir + "mc.root";
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
		stsDigiFile = parDir+ "/sts/sts_v11a.digi.par";
		// Digi scheme file for MUCH.
		// MUST be consistent with MUCH geometry used in MC transport.
		muchDigiFile = parDir + "/much/much_v11a.digi.root";
		// Directory for output images
		TString imageDir = "./test/";
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
		exit(0);
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
			CbmMuchDigitizeGem* muchDigitize = new CbmMuchDigitizeGem(muchDigiFile.Data());
			run->AddTask(muchDigitize);
			CbmMuchDigitizeStraws* strawDigitize = new CbmMuchDigitizeStraws("MuchDigitizeStraws", muchDigiFile.Data(), iVerbose);
			run->AddTask(strawDigitize);

			CbmMuchFindHitsGem* muchFindHits = new CbmMuchFindHitsGem(muchDigiFile.Data());
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

                        CbmTrdHitProducerSmearing* trdHitProd = new CbmTrdHitProducerSmearing(radiator);
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


                // -----   Primary vertex finding   ---------------------------------------
                CbmPrimaryVertexFinder* pvFinder = new CbmPVFinderKF();
                CbmFindPrimaryVertex* findVertex = new CbmFindPrimaryVertex(pvFinder);
                run->AddTask(findVertex);
                // ------------------------------------------------------------------------

		// -----   Track finding QA check   ------------------------------------
                // Execute the task only if BOOST is available, otherwise
                // the macro will crash.
                // The ENV variable is set in the cmake generated test scripts. 
                TString bla = gSystem->Getenv("BOOST");
                Int_t boost=bla.Atoi();

/*
                if (boost) {
  		  CbmLitTrackingQa* trackingQa = new CbmLitTrackingQa();
		  trackingQa->SetMinNofPointsSts(4);
		  trackingQa->SetMinNofPointsTrd(10);
		  trackingQa->SetMinNofPointsMuch(11);
		  trackingQa->SetMinNofPointsTof(1);
		  trackingQa->SetQuota(0.7);
		  trackingQa->SetMinNofHitsTrd(9);
		  trackingQa->SetMinNofHitsMuch(11);
		  trackingQa->SetVerbose(1);
		  trackingQa->SetOutputDir("");
		  run->AddTask(trackingQa);
                }
*/
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

  cout << " Test passed" << endl;
  cout << " All ok " << endl;

}
