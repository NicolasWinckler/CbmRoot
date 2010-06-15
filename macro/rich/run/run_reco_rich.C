void run_reco_rich(Int_t nEvents = 700) {
	Int_t iVerbose = 0;

	//      TString outDir  = "/d/cbm06/user/slebedev/rich/ringsperevent/";
	//      TString inFile1 = outDir + "mc.0050.root";
	//      TString inFile2 = outDir + "reco.0050.root";
	//      TString parFile = outDir + "params.0050.root";
	//      TString outFile = outDir + "reco.0050.root";

	TString script = TString(gSystem->Getenv("SCRIPT"));
	TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");

	gRandom->SetSeed(10);

	TString inFile1 = "", inFile2 = "", inFile3 = "", parFile = "", outFile ="";

	Bool_t isRichTrackingOn = false;
	if (script != "yes") {
		TString inFile1 ="/d/cbm02/slebedev/rich/JUL09/auau.25gev.centr.0000.mc.root";
		TString inFile2 ="/d/cbm02/slebedev/rich/JUL09/auau.25gev.centr.0000.reco.root";
		TString parFile ="/d/cbm02/slebedev/rich/JUL09/auau.25gev.centr.0000.params.root";
		TString outFile ="/d/cbm02/slebedev/rich/JUL09/auau.25gev.centr.0000.recorich.root";
	} else {
		isRichTrackingOn = true;
		inFile1 = TString(gSystem->Getenv("MCFILE"));
		inFile2 = TString(gSystem->Getenv("GLOBALHITSFILE"));
		inFile3 = TString(gSystem->Getenv("GLOBALTRACKSFILE"));
		parFile = TString(gSystem->Getenv("PARFILE"));
		outFile = TString(gSystem->Getenv("RICHFILE"));
	}

	TString stsDigiFile = "sts_standard.digi.par";

	gDebug = 0;
	TStopwatch timer;
	timer.Start();

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/rich/cbmlibs.C");
	cbmlibs();

	FairRunAna *run = new FairRunAna();
	if (inFile1 != "") run->SetInputFile(inFile1);
	if (inFile2 != "") run->AddFriend(inFile2);
	if (inFile3 != "") run->AddFriend(inFile3);
	if (inFile1 != "") run->SetOutputFile(outFile);

	// =========================================================================
	// ===                        RICH reconstruction                        ===
	// =========================================================================

	// ---------------------RICH Hit Producer ----------------------------------
	Double_t richPmtRad = 0.4; // PMT radius [cm]
	Double_t richPmtDist = 0.; // Distance between PMTs [cm]
	Int_t richDetType = 4; // Detector type Hamamatsu H8500-03
	Int_t richNoise = 220; // Number of noise points per event
	Double_t collectionEff = 1.;
	Double_t richSMirror = 0.00001; // Sigma for additional point smearing due to light scattering in mirror
	CbmRichHitProducer* richHitProd = new CbmRichHitProducer(richPmtRad,
			richPmtDist, richDetType, richNoise, iVerbose, collectionEff,
			richSMirror);
	run->AddTask(richHitProd);
	//--------------------------------------------------------------------------
	if(isRichTrackingOn){
		 //----------------------RICH Track Extrapolation ---------------------------
		 Int_t    richNSts = 4;     // minimum number of STS hits for extrapolation
		 Double_t richZPos = 300.;  // z position for extrapolation [cm]
		 CbmRichTrackExtrapolation* richExtra
		 = new CbmRichTrackExtrapolationKF(richNSts, iVerbose);
		 CbmRichExtrapolateTracks* richExtrapolate = new CbmRichExtrapolateTracks();
		 richExtrapolate->UseExtrapolation(richExtra,richZPos);
		 run->AddTask(richExtrapolate);
		 //--------------------------------------------------------------------------


		 //--------------------- Rich Track Projection to photodetector -------------
		 Int_t richZFlag = 1;       // Projetion from IM plane (default)
		 CbmRichProjectionProducer* richProj =
		 new CbmRichProjectionProducer(iVerbose, richZFlag);
		 run->AddTask(richProj);
		 //--------------------------------------------------------------------------
	}

	//--------------------- RICH Ring Finding ----------------------------------
	//CbmL1RichENNRingFinder* richFinder = new CbmL1RichENNRingFinder(iVerbose);
	TString richGeoType = "compact";
	CbmRichRingFinderHough* richFinder = new CbmRichRingFinderHough(iVerbose,
			richGeoType);
	CbmRichFindRings* richFindRings = new CbmRichFindRings();
	richFindRings->UseFinder(richFinder);
	run->AddTask(richFindRings);
	//--------------------------------------------------------------------------

	//-------------------- RICH Ring Fitting -----------------------------------
	CbmRichRingFitter* richFitter = new CbmRichRingFitterEllipseTau(iVerbose,
			1, richGeoType);
	CbmRichFitRings* fitRings = new CbmRichFitRings("", "", richFitter);
	run->AddTask(fitRings);
	//--------------------------------------------------------------------------

	// ------------------- RICH Ring matching  ---------------------------------
	CbmRichMatchRings* matchRings = new CbmRichMatchRings(iVerbose);
	run->AddTask(matchRings);
	// -------------------------------------------------------------------------

	//--------------------- RICH ring-track assignment ------------------------
	Double_t richDistance = 10.; // Max. dist. ring centre to track [cm]
	Int_t richNPoints = 5; // Minmum number of hits on ring
	CbmRichRingTrackAssign* richAssign = new CbmRichRingTrackAssignClosestD(
			richDistance, richNPoints, iVerbose);
	CbmRichAssignTrack* assignTrack = new CbmRichAssignTrack();
	assignTrack->UseAssign(richAssign);
	run->AddTask(assignTrack);
	// ------------------------------------------------------------------------

	CbmRichRingQa* richQa = new CbmRichRingQa("Qa", "qa", 0);
	run->AddTask(richQa);

	// ------------------------------------------------------------------------

	// ===                 End of RICH local reconstruction                  ===
	// =========================================================================


	// -----  Parameter database   --------------------------------------------
	TString stsDigi = gSystem->Getenv("VMCWORKDIR");
	stsDigi += "/parameters/sts/";
	stsDigi += stsDigiFile;
	FairRuntimeDb* rtdb = run->GetRuntimeDb();
	FairParRootFileIo* parIo1 = new FairParRootFileIo();
	FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
	parIo1->open(parFile.Data());
	parIo2->open(stsDigi.Data(), "in");
	rtdb->setFirstInput(parIo1);
	rtdb->setSecondInput(parIo2);
	rtdb->setOutput(parIo1);
	rtdb->saveOutput();
	// ------------------------------------------------------------------------

	// -----   Intialise and run   --------------------------------------------
	run->LoadGeometry();
	run->Init();
	cout << "Starting run" << endl;
	run->Run(0, nEvents);
	// ------------------------------------------------------------------------

	// -----   Finish   -------------------------------------------------------
	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	cout << endl << endl;
	cout << "Macro finished succesfully." << endl;
	cout << "Output file is " << outFile << endl;
	cout << "Parameter file is " << parFile << endl;
	cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
	cout << endl;
	// ------------------------------------------------------------------------

	cout << " Test passed" << endl;
	cout << " All ok " << endl;
	exit(0);
}
