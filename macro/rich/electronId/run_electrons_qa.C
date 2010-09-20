void run_electrons_qa(Int_t nEvents = 700)
{

    Int_t iVerbose = 0;

    TString script = TString(gSystem->Getenv("SCRIPT"));
    TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");

    gRandom->SetSeed(10);

    TString inFile1 = "", inFile2 = "", parFile = "", outFile ="";
    string outImageDir = "";
    if (script != "yes") {
        TString inFile1 ="/d/cbm02/slebedev/rich/JUL09/auau.25gev.centr.0000.mc.root";
        TString inFile2 ="/d/cbm02/slebedev/rich/JUL09/auau.25gev.centr.0000.reco.root";
        TString parFile ="/d/cbm02/slebedev/rich/JUL09/auau.25gev.centr.0000.params.root";
        TString outFile ="/d/cbm02/slebedev/rich/JUL09/auau.25gev.centr.0000.qa.root";
    } else {
    	inFile1 = TString(gSystem->Getenv("MCFILE"));
    	inFile2 = TString(gSystem->Getenv("RECOFILE"));
    	parFile = TString(gSystem->Getenv("PARFILE"));
    	outFile = TString(gSystem->Getenv("ELIDFILE"));
    	outImageDir = string(gSystem->Getenv("IMAGEDIR"));
    }
    //TString stsDigiFile = "sts_standard.digi.par";

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

	run->SetOutputFile(outFile);

	//--------------------- RICH ring-track assignment ------------------------
	Double_t richDistance = 10.; // Max. dist. ring center to track [cm]
	Int_t richNPoints = 5; // Minimum number of hits on ring
	CbmRichRingTrackAssign* richAssign = new CbmRichRingTrackAssignClosestD(
			richDistance, richNPoints, iVerbose);
	CbmRichAssignTrack* assignTrack = new CbmRichAssignTrack();
	assignTrack->UseAssign(richAssign);
	run->AddTask(assignTrack);
	// ------------------------------------------------------------------------

	// ----------- TRD track Pid Ann ----------------------
	CbmTrdSetTracksPidANN* trdSetTracksPidAnnTask = new
	    CbmTrdSetTracksPidANN("Ann","Ann");
	run->AddTask(trdSetTracksPidAnnTask);
	// ----------------------------------------------------

	CbmRichElectronsQa* richQa = new CbmRichElectronsQa("Qa", "qa", 0);
	richQa->SetGeoType("compact"); //or large
	richQa->SetImageOutDir(outImageDir);
	richQa->SetRichAnnCut(-0.5);
	richQa->SetUseRichAnn(true);
	richQa->SetTrdAnnCut(0.8);
	richQa->SetMeanA(5.02);
	richQa->SetMeanB(4.68);
	richQa->SetRmsA(0.22);
	richQa->SetRmsB(0.167);
	richQa->SetRmsCoeff(3.5);
	richQa->SetDistCut(1.);

	run->AddTask(richQa);

	// -----  Parameter database   --------------------------------------------
       // TString stsDigi = gSystem->Getenv("VMCWORKDIR");
       // stsDigi += "/parameters/sts/";
       // stsDigi += stsDigiFile;
	FairRuntimeDb* rtdb = run->GetRuntimeDb();
	FairParRootFileIo* parIo1 = new FairParRootFileIo();
	//FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
	parIo1->open(parFile.Data());
	//parIo2->open(stsDigi.Data(), "in");
	rtdb->setFirstInput(parIo1);
	//rtdb->setSecondInput(parIo2);
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

