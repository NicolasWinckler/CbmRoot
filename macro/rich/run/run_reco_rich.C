void run_reco_rich(Int_t nEvents = 100) {
	Int_t iVerbose = 0;

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
		inFile2 = TString(gSystem->Getenv("RECOFILE"));
		parFile = TString(gSystem->Getenv("PARFILE"));
		outFile = TString(gSystem->Getenv("RECORICHFILE"));
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
	if (inFile1 != "") run->SetInputFile(inFile2);
	if (inFile2 != "") run->AddFriend(inFile1);
	if (outFile != "") run->SetOutputFile(outFile);

   FairTask* kalman= new CbmKF();
   run->AddTask(kalman);

	// ---------------------RICH Hit Producer ----------------------------------
	Double_t richPmtRad = 0.4; // PMT radius [cm]
	Double_t richPmtDist = 0.; // Distance between PMTs [cm]
	Int_t richDetType = 4; // Detector type Hamamatsu H8500-03
	Int_t richNoise = 220; // Number of noise points per event
	Double_t collectionEff = 1.;
	Double_t richSMirror = 0.06; // Sigma for additional point smearing due to light scattering in mirror
	CbmRichHitProducer* richHitProd = new CbmRichHitProducer(richPmtRad,
			richPmtDist, richDetType, richNoise, iVerbose, collectionEff,richSMirror);
	run->AddTask(richHitProd);

	if(isRichTrackingOn){
		 //----------------------RICH Track Extrapolation ---------------------------
		 Int_t    richNSts = 4;     // minimum number of STS hits for extrapolation
		 Double_t richZPos = 300.;  // z position for extrapolation [cm]
		 CbmRichTrackExtrapolation* richExtra = new CbmRichTrackExtrapolationKF(richNSts, iVerbose);
		 CbmRichExtrapolateTracks* richExtrapolate = new CbmRichExtrapolateTracks();
		 richExtrapolate->UseExtrapolation(richExtra,richZPos);
		 run->AddTask(richExtrapolate);

		 Int_t richZFlag = 1;       // Projection from IM plane (default)
		 CbmRichProjectionProducer* richProj =
		 new CbmRichProjectionProducer(iVerbose, richZFlag);
		 run->AddTask(richProj);
	}

	CbmRichReconstruction* richReco = new CbmRichReconstruction();
   run->AddTask(richReco);

	// ------------------- RICH Ring matching  ---------------------------------
	CbmRichMatchRings* matchRings = new CbmRichMatchRings();
	run->AddTask(matchRings);

	//--------------------- RICH ring-track assignment ------------------------
	Double_t richDistance = 10.; // Max. distance between ring center to track [cm]
	Int_t richNPoints = 5; // Minimum number of hits in ring
	CbmRichRingTrackAssign* richAssign = new CbmRichRingTrackAssignClosestD(
			richDistance, richNPoints, iVerbose);
	CbmRichAssignTrack* assignTrack = new CbmRichAssignTrack();
	assignTrack->UseAssign(richAssign);
	run->AddTask(assignTrack);

	// ===                 End of RICH local reconstruction                  ===
	// =========================================================================

//	CbmLitReconstructionQa* reconstructionQa = new CbmLitReconstructionQa();
//	reconstructionQa->SetMinNofPointsSts(4);
//	reconstructionQa->SetMinNofPointsTrd(9);
//	reconstructionQa->SetMinNofPointsMuch(10);
//	reconstructionQa->SetMinNofPointsTof(1);
//	reconstructionQa->SetQuota(0.7);
//	reconstructionQa->SetMinNofHitsTrd(9);
//	reconstructionQa->SetMinNofHitsMuch(10);
//	reconstructionQa->SetVerbose(0);
//	reconstructionQa->SetMomentumRange(0, 12);
//	reconstructionQa->SetNofBinsMom(12);
//	reconstructionQa->SetMinNofHitsRich(7);
//	reconstructionQa->SetQuotaRich(0.6);
//	reconstructionQa->SetOutputDir("recoIm/");
//	run->AddTask(reconstructionQa);

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

	run->Init();
	cout << "Starting run" << endl;
	run->Run(0, nEvents);

	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	cout << endl << endl;
	cout << "Macro finished successfully." << endl;
	cout << "Output file is " << outFile << endl;
	cout << "Parameter file is " << parFile << endl;
	cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
	cout << endl;
	cout << " Test passed" << endl;
	cout << " All ok " << endl;
	exit(0);
}
