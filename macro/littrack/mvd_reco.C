/** mvd_reco.C
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2011
 * @version 1.0
 **/

void mvd_reco(Int_t nEvents = 100)
{
	TString script = TString(gSystem->Getenv("SCRIPT"));
	TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");

	TString dir, imageDir, mcFile, parFile, mvdRecoFile, trackingType;
	TList *parFileList = new TList();
	TObjString stsDigiFile;
	Int_t normStsPoints;
	Float_t momMin, momMax;
	Int_t momBins;
	if (script != "yes") {
		// Output directory
		dir = "/d/cbm02/andrey/mvd/std_10e_urqmd/";
		// MC transport file
		mcFile = dir + "mc.0000.root";
		// Parameters file
		parFile = dir + "param.0000.root";
		// Output file with reconstructed tracks and hits
		mvdRecoFile = dir + "mvd.reco.0000.root";
		// Digi scheme for STS
		TObjString stsDigiFile = parDir + "/sts/sts_standard.digi.par";
		parFileList->Add(&stsDigiFile);
		// Directory for output images
		TString imageDir = "./test/";
		// Tracking type
		trackingType = "branch";
		// Normalization for efficiency
		normStsPoints = 4;
		//
		momMin = 0.;
		momMax = 12.;
		momBins = 12.;
	} else {
		mcFile = TString(gSystem->Getenv("MCFILE"));
		parFile = TString(gSystem->Getenv("PARFILE"));
		globalRecoFile = TString(gSystem->Getenv("MVDRECOFILE"));
		imageDir = TString(gSystem->Getenv("IMAGEDIR"));
		trackingType = TString(gSystem->Getenv("TRACKINGTYPE"));
		TObjString stsDigiFile = TString(gSystem->Getenv("STSDIGI"));
		parFileList->Add(&stsDigiFile);
		normStsPoints = TString(gSystem->Getenv("NORMSTSPOINTS"))->Atoi();
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
	run->SetInputFile(mcFile);
	run->SetOutputFile(mvdRecoFile);

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
	FairTask* findTracks = new CbmStsFindTracks(iVerbose, trackFinder, false);
	run->AddTask(findTracks);

	FairTask* stsMatchTracks = new CbmStsMatchTracks("STSMatchTracks", iVerbose);
	run->AddTask(stsMatchTracks);

	CbmStsTrackFitter* trackFitter = new CbmStsKFTrackFitter();
	FairTask* fitTracks = new CbmStsFitTracks("STS Track Fitter", trackFitter, iVerbose);
	run->AddTask(fitTracks);

	//	FairTask* stsFHQa = new CbmStsFindHitsQa("STSFindHitsQA",iVerbose);
	//	run->AddTask(stsFHQa);

	//FairTask* stsRecoQa = new CbmStsReconstructionQa(kFALSE, 4, 0.7, 1);
	//run->AddTask(stsRecoQa);
       // ------------------------------------------------------------------------


	CbmLitFindMvdTracks* finder = new CbmLitFindMvdTracks();
	finder->SetTrackingType(std::string(trackingType));
	run->AddTask(finder);


	CbmLitFindGlobalTracks* globalFinder = new CbmLitFindGlobalTracks();
	run->AddTask(globalFinder);


	// -----   Track finding QA check   ------------------------------------
	CbmLitReconstructionQa* reconstructionQa = new CbmLitReconstructionQa();
	reconstructionQa->SetMinNofPointsSts(normStsPoints);
//	reconstructionQa->SetMinNofPointsTrd(normTrdPoints);
//	reconstructionQa->SetMinNofPointsMuch(normMuchPoints);
//	reconstructionQa->SetMinNofPointsTof(normTofPoints);
	reconstructionQa->SetQuota(0.7);
//	reconstructionQa->SetMinNofHitsTrd(normTrdHits);
//	reconstructionQa->SetMinNofHitsMuch(normMuchHits);
	reconstructionQa->SetVerbose(1);
	reconstructionQa->SetMomentumRange(momMin, momMax);
	reconstructionQa->SetNofBinsMom(momBins);
	reconstructionQa->SetOutputDir(std::string(imageDir));
	run->AddTask(reconstructionQa);
	// ------------------------------------------------------------------------



//	// -----   Primary vertex finding   ---------------------------------------
//	CbmPrimaryVertexFinder* pvFinder = new CbmPVFinderKF();
//	CbmFindPrimaryVertex* findVertex = new CbmFindPrimaryVertex(pvFinder);
//	run->AddTask(findVertex);
//	// ------------------------------------------------------------------------


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
	cout << "Output file is " << mvdRecoFile << endl;
	cout << "Parameter file is " << parFile << endl;
	cout << "Real time " << timer.RealTime() << " s, CPU time "
			<< timer.CpuTime() << " s" << endl;
	cout << endl;
	// ------------------------------------------------------------------------
}

