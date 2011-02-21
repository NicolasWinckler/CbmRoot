/** create_data.C
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2010
 * @version 1.0
 *
 * Macro creates text files with data produced by CBMROOT
 * simulations which can be later used to run
 * the standalone tracking package.
 **/

void create_data(Int_t nEvents = 100)
{
   	// Output directory
	TString dir = "/d/cbm02/andrey/test_muons/";
	// MC transport file
	TString mcFile = dir + "mc.0000.root";
	// Parameter file
	TString parFile = dir + "param.0000.root";
	// File with reconstructed STS tracks, STS, MUCH, TRD and TOF hits and digis
	TString globalRecoFile = dir + "global.reco.0000.root";
	// Dummy output file
    TString dummyFile = dir + "dummy.root";

	TStopwatch timer;
	timer.Start();

	gSystem->Load("/home/soft/tbb/libtbb");
	gSystem->Load("/u/andrey/soft/tbb/Lenny64/libtbb");
//	gSystem->Load("/u/andrey/soft/tbb/Etch32/libtbb");

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
	cbmrootlibs();

	// -----   Reconstruction run   -------------------------------------------
	FairRunAna *run= new FairRunAna();
	run->SetInputFile(mcFile);
	run->AddFriend(globalRecoFile);
	run->SetOutputFile(dummyFile);
	// ------------------------------------------------------------------------

	// ------------------------------------------------------------------------
	CbmLitCreateStandaloneData* creator = new CbmLitCreateStandaloneData();
	run->AddTask(creator);
	// ------------------------------------------------------------------------

	// -----  Parameter database   --------------------------------------------
	FairRuntimeDb* rtdb = run->GetRuntimeDb();
	FairParRootFileIo* parIo1 = new FairParRootFileIo();
	parIo1->open(parFile.Data());
	rtdb->setFirstInput(parIo1);
	rtdb->setOutput(parIo1);
	rtdb->saveOutput();
	// ------------------------------------------------------------------------

	// -----   Initialize and run   --------------------------------------------
	run->LoadGeometry();
	run->Init();
	run->Run(0, nEvents);
	// ------------------------------------------------------------------------

	// -----   Finish   -------------------------------------------------------
	timer.Stop();
	cout << endl << endl;
	cout << "Macro finished successfully." << endl;
	cout << "Output file is " << dummyFile << endl;
	cout << "Parameter file is " << parFile << endl;
	cout << "Real time " << timer.RealTime() << " s, CPU time " << timer.CpuTime() << " s" << endl;
	cout << endl;
	// ------------------------------------------------------------------------
}
