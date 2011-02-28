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
        TString outFile ="/d/cbm02/slebedev/rich/JUL09/auau.25gev.centr.0000.elidqa.root";
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
	 gSystem->Load("libAnalysis");

	FairRunAna *run = new FairRunAna();
	if (inFile1 != "") run->SetInputFile(inFile1);
	if (inFile2 != "") run->AddFriend(inFile2);

	run->SetOutputFile(outFile);

	CbmAnaElectronsQa* electronsQa = new CbmAnaElectronsQa("CbmAnaElectronsQa", "CbmAnaElectronsQa");
	electronsQa->SetRichAnnCut(-0.5);
	electronsQa->SetUseRichAnn(true);
	electronsQa->SetTrdAnnCut(0.8);
	//electronsQa->SetMeanA(5.02);
	//electronsQa->SetMeanB(4.68);
	//electronsQa->SetRmsA(0.22);
	//electronsQa->SetRmsB(0.167);
	//electronsQa->SetRmsCoeff(3.5);
	//electronsQa->SetDistCut(1.);
	run->AddTask(electronsQa);

	FairRuntimeDb* rtdb = run->GetRuntimeDb();
	FairParRootFileIo* parIo1 = new FairParRootFileIo();
	//FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
	parIo1->open(parFile.Data());
	//parIo2->open(stsDigi.Data(), "in");
	rtdb->setFirstInput(parIo1);
	//rtdb->setSecondInput(parIo2);
	rtdb->setOutput(parIo1);
	rtdb->saveOutput();

	run->LoadGeometry();
	run->Init();
	cout << "Starting run" << endl;
	run->Run(0, nEvents);

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

