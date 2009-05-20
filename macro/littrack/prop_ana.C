void prop_ana(Int_t nEvents = 10000)
{
	TString dir = "/home/d/andrey/test/trunk/global_mu/";
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
	propAna->SetNofPlanes(14);
	propAna->SetNofTrdHits(0);
	propAna->SetNofMuchHits(13);
	propAna->SetNofTofHits(1);
	run->AddTask(propAna);
	// -------------------------------------------------------------------------
	TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");
	TString stsDigiFile = parDir+ "/sts/sts_Standard_s3055AAFK5.SecD.digi.par";
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
	cout << "Output file is "    << outFile << endl;
	cout << "Parameter file is " << parFile << endl;
	cout << "Real time " << timer.RealTime() << " s, CPU time " << timer.CpuTime() << " s" << endl;
	cout << endl;
	// ------------------------------------------------------------------------
}
