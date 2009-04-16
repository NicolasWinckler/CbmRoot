void sts_reco(Int_t nEvents = 1000)
{
	TString script = TString(gSystem->Getenv("SCRIPT"));

	TString dir, mcFile, parFile, stsRecoFile, detector;
	if (script != "yes") {
//		dir  = "/home/d/andrey/events/newmuch/compact/10mu/mu_urqmd/";
		dir  = "/home/d/andrey/test/trunk/mu/";
		mcFile = dir + "mc.0000.root";
		parFile = dir + "param.0000.root";
		stsRecoFile = dir + "sts.reco.0000.root";
		detector = "much";
	} else {
		mcFile = TString(gSystem->Getenv("MCFILE"));
		parFile = TString(gSystem->Getenv("PARFILE"));
		stsRecoFile = TString(gSystem->Getenv("STSRECOFILE"));
		detector = TString(gSystem->Getenv("DETECTOR"));
	}

	//  TString digiFile = "sts_standard.digi.par";
	TString digiFile = "sts_Standard_s3055AAFK5.SecD.digi.par";//"sts_allstrips.digi.par";

	Int_t iVerbose = 1;
	TStopwatch timer;
	timer.Start();

	// ----  Load libraries   -------------------------------------------------
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
	cbmrootlibs();
	// ------------------------------------------------------------------------

	// -----   Reconstruction run   -------------------------------------------
	FairRunAna* run= new FairRunAna();
	run->SetInputFile(mcFile);
	run->SetOutputFile(stsRecoFile);
	// ------------------------------------------------------------------------

	// -----   MVD Hitproducer   -----------------------------------------------
	if (detector == "trd") {
//		CbmMvdHitProducer* mvdHitProd = new CbmMvdHitProducer("MVDHitProducer", 0, iVerbose);
//		run->AddTask(mvdHitProd);
	}
	// -------------------------------------------------------------------------

	// -----   STS digitiser   ------------------------------------------------
	FairTask* stsDigitize = new CbmStsDigitize("STSDigitize", iVerbose);
	run->AddTask(stsDigitize);
	// ------------------------------------------------------------------------

	// ---  STS hit finding   -------------------------------------------------
	FairTask* findHits = new CbmStsFindHits("STSFindHits", iVerbose);
	run->AddTask(findHits);
	// ------------------------------------------------------------------------

	// ---  STS hit matching   ------------------------------------------------
	FairTask* matchHits = new CbmStsMatchHits("STSMatchHits",
				  iVerbose);
	run->AddTask(matchHits);
	// ------------------------------------------------------------------------


	// -----   STS track finding   --------------------------------------------
	FairTask* kalman= new CbmKF();
	run->AddTask(kalman);
	FairTask* l1 = new CbmL1();
	run->AddTask(l1);
	CbmStsTrackFinder* trackFinder    = new CbmL1StsTrackFinder();
	FairTask* findTracks = new CbmStsFindTracks(iVerbose, trackFinder);
	run->AddTask(findTracks);
	// ------------------------------------------------------------------------

	// -----   STS track matching   -------------------------------------------
	FairTask* matchTracks = new CbmStsMatchTracks("Match tracks", iVerbose);
	run->AddTask(matchTracks);
	// ------------------------------------------------------------------------

	// -----   STS track fitting   --------------------------------------------
	CbmStsTrackFitter* trackFitter = new CbmStsKFTrackFitter();
	FairTask* fitTracks = new CbmStsFitTracks("STS Track Fitter", trackFitter, iVerbose);
	run->AddTask(fitTracks);
	// ------------------------------------------------------------------------

	// -----   STS reconstruction QA   ----------------------------------------
	FairTask* stsRecoQa = new CbmStsReconstructionQa(kFALSE, 4, 0.7, 2);
	run->AddTask(stsRecoQa);
	// ------------------------------------------------------------------------

	// -----  Parameter database   --------------------------------------------
	TString stsDigiFile = gSystem->Getenv("VMCWORKDIR");
	stsDigiFile += "/parameters/sts/";
	stsDigiFile += digiFile;
	FairRuntimeDb* rtdb = run->GetRuntimeDb();
	FairParRootFileIo*  parIo1 = new FairParRootFileIo();
	FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
	parIo1->open(parFile.Data());
	parIo2->open(stsDigiFile.Data(),"in");
	rtdb->setFirstInput(parIo1);
	rtdb->setSecondInput(parIo2);
	rtdb->setOutput(parIo1);
	rtdb->saveOutput();
	// ------------------------------------------------------------------------

	// -----   Initialise and run   -------------------------------------------
	run->LoadGeometry();
	run->Init();
	run->Run(0, nEvents);
	// ------------------------------------------------------------------------

	// -----   Finish   -------------------------------------------------------
	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	cout << endl << endl;
	cout << "Macro finished succesfully." << endl;
	cout << "Output file is "         << stsRecoFile << endl;
	cout << "Parameter file is "      << parFile << endl;
	cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
	cout << endl;
	// ------------------------------------------------------------------------
}
