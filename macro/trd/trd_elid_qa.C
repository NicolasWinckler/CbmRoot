void trd_elid_qa(){

	Int_t nEvents = 1000;

	TString inFile = "/d/cbm02/slebedev/trd/JUN09/MB/piel.0000.mc.root";
	//TString inFile2 = "/d/cbm02/slebedev/trd/JUN09/MB/piel.0006.mc_1.root";
	TString inFile3 = "/d/cbm02/slebedev/trd/JUN09/MB/piel.0000.reco.root";

	TString parFile = "/d/cbm02/slebedev/trd/JUN09/MB/piel.0000.params.root";

	TString stsDigiFile = "sts_standard.digi.par";

	TString outFile = "/d/cbm02/slebedev/trd/JUN09/MB/piel.0000.electronsQa.root";

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
	gSystem->Load("libTrd");
	// ------------------------------------------------------------------------


	FairRunAna *run= new FairRunAna();
	run->SetInputFile(inFile);
	//  run->AddFile(inFile2);
	run->AddFriend(inFile3);
	run->SetOutputFile(outFile);


	CbmTrdElectronsQa* elQa = new CbmTrdElectronsQa();
	//set file name to "" if you don't want to convert energy losses to txt file
	elQa->SetOutFileNameEl("electrons_mom_0.txt");
	elQa->SetOutFileNamePi("pions_mom_0.txt");
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
	run->Run(0,nEvents);
	// ------------------------------------------------------------------------

	// -----   Finish   -------------------------------------------------------
	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	cout << endl << endl;
	cout << "Macro finished succesfully." << endl;
	cout << "Output file is "    << outFile << endl;
	cout << "Parameter file is " << parFile << endl;
	cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
	cout << endl;
	// ------------------------------------------------------------------------

	exit(0);
}
