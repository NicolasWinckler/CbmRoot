/**
 * \file trd_hits_qa.C
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2010
 **/

#include <iostream>
using std::cout;
using std::endl;

void trd_hits_qa(Int_t nEvents = 500)
{
	// Output directory
	TString dir = "/d/cbm02/andrey/electron/std_10e_urqmd_no_mvd/";
	// MC transport file
	TString mcFile = dir + "mc.0000.root";
	// Parameter file
	TString parFile = dir + "param.0000.root";
	// File with reconstructed STS tracks, STS, MUCH, TRD and TOF hits and digis.
	TString globalHitsFile = dir + "global.hits.digi.0000.root";
	// Output file with TRD hits qa histograms
	TString trdHitsQaFile = dir + "trd.hits.qa.0000.root";

	Int_t iVerbose = 1;
	TStopwatch timer;
	timer.Start();

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
	cbmrootlibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/determine_setup.C");

	FairRunAna *run= new FairRunAna();
	run->SetInputFile(mcFile);
	run->AddFriend(globalHitsFile);
	run->SetOutputFile(trdHitsQaFile);

	//------------------------------------------------
	CbmTrdHitsQa* trdHitsQa = new CbmTrdHitsQa();
	trdHitsQa->SetOutputDir("./test/");
	run->AddTask(trdHitsQa);
	//------------------------------------------------

	// -----  Parameter database   --------------------------------------------
	FairRuntimeDb* rtdb = run->GetRuntimeDb();
	FairParRootFileIo* parIo1 = new FairParRootFileIo();
	parIo1->open(parFile.Data());
	rtdb->setFirstInput(parIo1);
	rtdb->setOutput(parIo1);
	rtdb->saveOutput();
	// ------------------------------------------------------------------------

	// -----   Intialise and run   --------------------------------------------
	run->Init();
	run->Run(0, nEvents);
	// ------------------------------------------------------------------------

	// -----   Finish   -------------------------------------------------------
	timer.Stop();
	cout << endl << endl;
	cout << "Macro finished succesfully." << endl;
	cout << "Output file is " << trdHitsQaFile << endl;
	cout << "Parameter file is " << parFile << endl;
	cout << "Real time " << timer.RealTime() << " s, CPU time " << timer.CpuTime() << " s" << endl;
	cout << endl;
	// ------------------------------------------------------------------------
}
