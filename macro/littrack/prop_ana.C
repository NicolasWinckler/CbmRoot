void prop_ana(Int_t nEvents = 1000)
{
	TString script = TString(gSystem->Getenv("SCRIPT"));

	Double_t trdHitErr = 100; // if == 0 than standard errors are used
	TString dir, imageDir, mcFile, parFile, globalTracksFile, propAnaFile;
	if (script != "yes") {
		dir = "/home/d/andrey/trdsimple_10pi/";
		mcFile = dir + "mc.0000.root";
		globalTracksFile = dir + "global.tracks.ideal.trd100.0000.root";
		parFile = dir + "param.0000.root";
		propAnaFile = dir + "propagation.ana.0000.root";
		imageDir = "./test/";
	} else {
		mcFile = TString(gSystem->Getenv("MCFILE"));
		parFile = TString(gSystem->Getenv("PARFILE"));
		globalTracksFile = TString(gSystem->Getenv("GLOBALTRACKSIDEALFILE"));
		imageDir = TString(gSystem->Getenv("IMAGEDIR"));
		propAnaFile = TString(gSystem->Getenv("PROPANAFILE"));
	}

	TStopwatch timer;
	timer.Start();

	gSystem->Load("/home/soft/tbb/libtbb");

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
	cbmrootlibs();

	FairRunAna *run= new FairRunAna();
	run->SetInputFile(mcFile);
	run->AddFriend(globalTracksFile);
	run->SetOutputFile(propAnaFile);

//	FairGeane* Geane = new FairGeane(inFile.Data());
//	FairGeane* Geane = new FairGeane(parFile.Data());
//	FairGeane *Geane = new FairGeane();
//	run->AddTask(Geane);

	// -------------------------------------------------------------------------
	CbmLitPropagationAnalysis* propAna = new CbmLitPropagationAnalysis();
	propAna->SetNofPlanes(12);
	propAna->SetNofTrdHits(12);
	propAna->SetNofMuchHits(0);
	propAna->SetNofTofHits(0);
	propAna->SetPDGCode(211);
	propAna->SetTestFastPropagation(false);
	propAna->SetOutputDir(std::string(imageDir));
	propAna->IsDrawPropagation(true);
	propAna->IsDrawFilter(true);
	propAna->IsDrawSmoother(false);
	propAna->IsCloseCanvas(true);
	propAna->IsFixedBounds(false);
	propAna->SetPlaneNoPhd(12);
	run->AddTask(propAna);
	// -------------------------------------------------------------------------
	TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");
	TString stsDigiFile = parDir+ "/sts/sts_standard.digi.par";
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

	// -----   Intialise and run   --------------------------------------------
	run->LoadGeometry();
	run->Init();
//	Geane->SetField(run->GetField());
	run->Run(0,nEvents);
	// ------------------------------------------------------------------------

	// -----   Finish   -------------------------------------------------------
	timer.Stop();
	cout << endl << endl;
	cout << "Macro finished succesfully." << endl;
	cout << "Output file is "    << propAnaFile << endl;
	cout << "Parameter file is " << parFile << endl;
	cout << "Real time " << timer.RealTime() << " s, CPU time " << timer.CpuTime() << " s" << endl;
	cout << endl;
	// ------------------------------------------------------------------------
}
