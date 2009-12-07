/** global_reco.C
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

void global_reco(Int_t nEvents = 1000)
{
	TString script = TString(gSystem->Getenv("SCRIPT"));
	TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");

	TString dir, mcFile, parFile, globalRecoFile, muchDigiFile;
	if (script != "yes") {
		// Output directory
		dir  = "/d/cbm02/andrey/std13_10mu_urqmd/";
		// MC transport file
		mcFile = dir + "mc.0000.root";
		// Parameters file
		parFile = dir + "param.0000.root";
		// Output file with reconstructed tracks and hits
		globalRecoFile = dir + "global.reco.newsts.0000.root";
		// Digi scheme file for MUCH.
		// MUST be consistent with MUCH geometry used in MC transport.
		muchDigiFile = parDir + "/much/much_standard.digi.root";
//		muchDigiFile = parDir + "/much/much_standard_trd.digi.root";
//		muchDigiFile = parDir + "/much/much_standard_straw.digi.root";
	} else {
		mcFile = TString(gSystem->Getenv("MCFILE"));
		parFile = TString(gSystem->Getenv("PARFILE"));
		globalRecoFile = TString(gSystem->Getenv("GLOBALRECOFILE"));
		muchDigiFile = TString(gSystem->Getenv("MUCHDIGI"));
	}

	Int_t iVerbose = 1;
	TStopwatch timer;
	timer.Start();

        gSystem->Load("/u/andrey/soft/tbb/libtbb");

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
	cbmrootlibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/determine_setup.C");

	FairRunAna *run= new FairRunAna();
	run->SetInputFile(mcFile);
	run->SetOutputFile(globalRecoFile);

	TString stsDigiFile = parDir+ "/sts/sts_standard.digi.par";

	// ----- STS reconstruction   ---------------------------------------------
	FairTask* stsDigitize = new CbmStsDigitize("STSDigitize", iVerbose);
	run->AddTask(stsDigitize);

        FairTask* stsClusterFinder = new CbmStsClusterFinder("STS Cluster Finder", iVerbose);
        run->AddTask(stsClusterFinder);

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

		CbmTrdHitProducerSmearing* trdHitProd = new
				 CbmTrdHitProducerSmearing("TRD Hitproducer", "TRD task", radiator);

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

	// ------ Global track reconstruction -------------------------------------
	CbmLitFindGlobalTracks* finder = new CbmLitFindGlobalTracks();
	// Tracking method to be used
	// "branch" - branching tracking
	// "nn" - nearest neighbor tracking
	// "weight" - weighting tracking
	finder->SetTrackingType("nn");

	// Hit-to-track merger method to be used
	// "nearest_hit" - assigns nearest hit to the track
	finder->SetMergerType("nearest_hit");

	run->AddTask(finder);
	// ------------------------------------------------------------------------

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
	reconstructionQa->SetMinNofPointsMuch(12);
	reconstructionQa->SetMinNofPointsTof(1);
	reconstructionQa->SetQuota(0.7);
	reconstructionQa->SetMinNofHitsTrd(3);
	reconstructionQa->SetMinNofHitsMuch(11);
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
