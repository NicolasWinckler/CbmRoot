void prop_ana(Int_t nEvents = 1000)
{
	TString dir = "/home/d/andrey/test/trunk/global_e/";
	TString mcFile = dir + "mc.0000.root";
	TString globalTracksFile = dir + "global.tracks.ideal.0000.root";
	TString parFile = dir + "param.0000.root";
	TString outFile = dir + "propagation.ana.0000.root";

	TStopwatch timer;
	timer.Start();

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
	cbmrootlibs();

	FairRunAna *run= new FairRunAna();
	run->SetInputFile(mcFile);
	run->AddFriend(globalTracksFile);
	run->SetOutputFile(outFile);

//	FairGeane* Geane = new FairGeane(inFile.Data());

	// -------------------------------------------------------------------------
	CbmLitPropagationAnalysis* propAna = new CbmLitPropagationAnalysis();
	propAna->SetNofPlanes(13);
	propAna->SetNofTrdHits(12);
	propAna->SetNofMuchHits(0);
	propAna->SetNofTofHits(1);
	run->AddTask(propAna);
	// -------------------------------------------------------------------------

	// -----  Parameter database   --------------------------------------------
	FairRuntimeDb* rtdb = run->GetRuntimeDb();
	FairParRootFileIo* parIo1 = new FairParRootFileIo();
	parIo1->open(parFile.Data());
	rtdb->setFirstInput(parIo1);
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
	cout << "Output file is "    << outFile << endl;
	cout << "Parameter file is " << parFile << endl;
	cout << "Real time " << timer.RealTime() << " s, CPU time " << timer.CpuTime() << " s" << endl;
	cout << endl;
	// ------------------------------------------------------------------------
}
