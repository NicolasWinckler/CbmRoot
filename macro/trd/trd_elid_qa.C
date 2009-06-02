void trd_elid_qa(){

	Int_t nEvents = 1000;

	TString fileNumber = "0009";

	TString inFile = "/d/cbm02/slebedev/trd/JUN09/MB/piel."+fileNumber+".mc.root";
//	TString inFile2 = "/d/cbm02/slebedev/trd/JUN09/MB/piel."+fileNumber+".mc_1.root";
	TString inFile3 = "/d/cbm02/slebedev/trd/JUN09/MB/piel."+fileNumber+".reco.root";

	TString parFile = "/d/cbm02/slebedev/trd/JUN09/MB/piel."+fileNumber+".params.root";

	TString stsDigiFile = "sts_standard.digi.par";

	TString outFile = "/d/cbm02/slebedev/trd/JUN09/MB/piel."+fileNumber+".electronsQa.root";

//set file name to "" if you don't want to convert energy losses to txt file
	TString outTxtFileNameEl = "mb_electrons_mom_"+fileNumber+".txt";
	TString outTxtFileNamePi = "mb_pions_mom_"+fileNumber+".txt";

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
	run->AddFriend(inFile3);
 //   run->AddFile(inFile2);
	run->SetOutputFile(outFile);


	CbmTrdElectronsQa* elQa = new CbmTrdElectronsQa();
	elQa->SetOutFileNameEl((const char*)outTxtFileNameEl);
	elQa->SetOutFileNamePi((const char*)outTxtFileNamePi);
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
