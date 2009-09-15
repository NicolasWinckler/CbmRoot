
void trd_elid_reco_many(Int_t fileNum, Int_t trdGeoType)
{
	Int_t nEvents = 100;

	TString fileNumSt, trdGeomFile;

	Double_t momentum;
	if (fileNum == 0)fileNumSt = "0";
	if (fileNum == 1)fileNumSt = "1";
	if (fileNum == 2)fileNumSt = "2";
	if (fileNum == 3)fileNumSt = "3";
	if (fileNum == 4)fileNumSt = "4";
	if (fileNum == 5)fileNumSt = "5";
	if (fileNum == 6)fileNumSt = "6";
	if (fileNum == 7)fileNumSt = "7";
	if (fileNum == 8)fileNumSt = "8";
	if (fileNum == 9)fileNumSt = "9";


	TString dir;
	if (trdGeoType == 1)dir = "/d/cbm02/slebedev/trd/JUL09/st/";
	if (trdGeoType == 2)dir = "/d/cbm02/slebedev/trd/JUL09/mb/";

	TString geoTypeSt;
	if (trdGeoType == 1) geoTypeSt = "st";
	if (trdGeoType == 2) geoTypeSt = "mb";

	TString inFile = dir + "piel.000" + fileNumSt + ".mc.root";
	TString parFile = dir + "piel.000" + fileNumSt + ".params.root";
	TString outFile = dir + "piel.000" + fileNumSt + ".reco.root";

	TString outTxtFileNameEl =
		"/d/cbm02/slebedev/trd/JUL09/"+geoTypeSt+"/"+geoTypeSt+"_electrons_mom_000"+fileNumSt+".txt";
	TString outTxtFileNamePi =
		"/d/cbm02/slebedev/trd/JUL09/"+geoTypeSt+"/"+geoTypeSt+"_pions_mom_000"+fileNumSt+".txt";

	Int_t iVerbose = 0;
	//TString stsDigiFile = "sts_standard.digi.par";

	gDebug = 0;

	TStopwatch timer;
	timer.Start();

	// ----  Load libraries   -------------------------------------------------
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gSystem->Load("libGeoBase");
	gSystem->Load("libParBase");
	gSystem->Load("libBase");
	gSystem->Load("libCbmBase");
	gSystem->Load("libCbmData");
	gSystem->Load("libField");
	gSystem->Load("libGen");
	gSystem->Load("libPassive");
	gSystem->Load("libMvd");
	gSystem->Load("libSts");
	gSystem->Load("libRich");
	gSystem->Load("libTrd");
	gSystem->Load("libTof");
	gSystem->Load("libEcal");
	gSystem->Load("libGlobal");
	gSystem->Load("libKF");
	gSystem->Load("libL1");

	FairRunAna *run = new FairRunAna();
	run->SetInputFile(inFile);
	// run->AddFile(inFile2);
	run->SetOutputFile(outFile);


	// Update of the values for the radiator F.U. 17.08.07
	Int_t trdNFoils = 130; // number of polyetylene foils
	Float_t trdDFoils = 0.0013; // thickness of 1 foil [cm]
	Float_t trdDGap = 0.02; // thickness of gap between foils [cm]
	Bool_t simpleTR = kTRUE; // use fast and simple version for TR
	// production

	CbmTrdRadiator *radiator = new CbmTrdRadiator(simpleTR, trdNFoils,
			trdDFoils, trdDGap);

	// -----   TRD hit producer   ----------------------------------------------
	Double_t trdSigmaX[] = { 300, 400, 500 }; // Resolution in x [mum]
	// Resolutions in y - station and angle dependent [mum]
	Double_t trdSigmaY1[] = { 2700, 3700, 15000, 27600, 33000, 33000, 33000 };
	Double_t trdSigmaY2[] = { 6300, 8300, 33000, 33000, 33000, 33000, 33000 };
	Double_t trdSigmaY3[] = { 10300, 15000, 33000, 33000, 33000, 33000, 33000 };

	CbmTrdHitProducerSmearing* trdHitProd = new CbmTrdHitProducerSmearing(
			"TRD Hitproducer", "TRD task", radiator);

	trdHitProd->SetSigmaX(trdSigmaX);
	trdHitProd->SetSigmaY(trdSigmaY1, trdSigmaY2, trdSigmaY3);
	run->AddTask(trdHitProd);

	CbmTrdTrackFinder* trdTrackFinder = new CbmTrdTrackFinderIdeal();
	CbmTrdFindTracks* trdFindTracks = new CbmTrdFindTracks("TRD Track Finder");
	trdFindTracks->UseFinder(trdTrackFinder);
	run->AddTask(trdFindTracks);

	CbmTrdMatchTracks* trdMatchTracks = new CbmTrdMatchTracks(iVerbose);
	run->AddTask(trdMatchTracks);

	CbmTrdSetTracksPidANN* trdSetTracksPidAnnTask = new CbmTrdSetTracksPidANN(
			"Ann", "Ann");
	//trdSetTracksPidAnnTask->SetTRDGeometryType("mb");
	run->AddTask(trdSetTracksPidAnnTask);


	CbmTrdElectronsQa* elQa = new CbmTrdElectronsQa();
	elQa->SetOutFileNameEl((const char*)outTxtFileNameEl);
	elQa->SetOutFileNamePi((const char*)outTxtFileNamePi);
	elQa->SetGeoType(geoTypeSt);
	run->AddTask(elQa);


	// -----  Parameter database   --------------------------------------------
	//TString stsDigi = gSystem->Getenv("VMCWORKDIR");
	//stsDigi += "/parameters/sts/";
	//stsDigi += stsDigiFile;
	FairRuntimeDb* rtdb = run->GetRuntimeDb();
	FairParRootFileIo* parIo1 = new FairParRootFileIo();
	// FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
	parIo1->open(parFile.Data());
	// parIo2->open(stsDigi.Data(),"in");
	rtdb->setFirstInput(parIo1);
	// rtdb->setSecondInput(parIo2);
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
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	cout << endl << endl;
	cout << "Macro finished succesfully." << endl;
	cout << "Output file is " << outFile << endl;
	cout << "Parameter file is " << parFile << endl;
	cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
	cout << endl;
	// ------------------------------------------------------------------------

	exit(0);
}

