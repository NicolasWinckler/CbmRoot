void much_hits(Int_t nEvents = 1000)
{
	TString script = TString(gSystem->Getenv("SCRIPT"));

	TString dir, mcFile, parFile, muchHitsFile, digiFile, geoType;
	if (script != "yes") {
		dir  = "/home/d/andrey/events/newmuch/standard/10mu/mu_urqmd/";
		mcFile = dir + "mc.0000.root";
		parFile = dir + "param.0000.root";
		muchHitsFile = dir + "much.hits.0000.root";
		geoType = "new_much_standard";
	} else {
		mcFile = TString(gSystem->Getenv("MCFILE"));
		parFile = TString(gSystem->Getenv("PARFILE"));
		muchHitsFile = TString(gSystem->Getenv("MUCHHITSFILE"));
		geoType = TString(gSystem->Getenv("GEOTYPE"));
	}

	TString muchParDir = TString(gSystem->Getenv("VMCWORKDIR"));
	if (geoType == "new_much_standard") digiFile = muchParDir + "/parameters/much/much_standard.digi.root";
	if (geoType == "new_much_compact") digiFile = muchParDir + "/parameters/much/much_compact.digi.root";
	if (geoType == "much_standard") digiFile = muchParDir + "/parameters/much/much_standard.digi.par";
	if (geoType == "much_compact") digiFile = muchParDir + "/parameters/much/much_compact.digi.par";
	if (geoType == "much_straw_standard") digiFile = muchParDir + "/parameters/much/much_standard.digi.par";

	Int_t iVerbose = 0;
	TStopwatch timer;
	timer.Start();

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
	cbmrootlibs();

	// -----   Reconstruction run   -------------------------------------------
	FairRunAna *run= new FairRunAna();
	run->SetInputFile(mcFile);
	run->SetOutputFile(muchHitsFile);
	// ------------------------------------------------------------------------

	// ---  MuCh digitizer ----------------------------------------------------
	if (geoType.Contains("new")) {
		CbmMuchDigitize* muchDigitize = new CbmMuchDigitize("MuchDigitize", digiFile.Data(), iVerbose);
		muchDigitize->SetUseAvalanche(0); // 0 - Not account for avalanches; 1 - Account for avalanches
		//muchDigitize->SetMeanNoise(0);
		run->AddTask(muchDigitize);

		CbmMuchFindHits* muchFindHits = new CbmMuchFindHits("MuchFindHits", digiFile.Data(), iVerbose);
		muchFindHits->SetUseClustering(0);
		run->AddTask(muchFindHits);
	} else {
		CbmMuchDigitize* muchDigitize = new CbmMuchDigitize("MuchDigitize", iVerbose);
		run->AddTask(muchDigitize);

		CbmMuchFindHits* muchFindHits = new CbmMuchFindHits("MuchFindHits", iVerbose);
		run->AddTask(muchFindHits);
	}

	// -----  Parameter database   --------------------------------------------
	FairRuntimeDb* rtdb = run->GetRuntimeDb();
	FairParRootFileIo* parIo1 = new FairParRootFileIo();
	parIo1->open(parFile.Data());
	rtdb->setFirstInput(parIo1);
	if (!geoType.Contains("new")) {
		FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
		parIo2->open(digiFile.Data());
		rtdb->setSecondInput(parIo2);
	}
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
	cout << "Output file is "    << muchHitsFile << endl;
	cout << "Parameter file is " << parFile << endl;
	cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
	cout << endl;
	// ------------------------------------------------------------------------

	cout << " Test passed" << endl;
	cout << " All ok " << endl;
}
