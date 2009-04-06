#include "../../cbmbase/CbmDetectorList.h";
void prop_ana(Int_t nEvents = 1000)
{
//	TString dir = "/home/d/andrey/events/trd/monolithic/10e/e/";
	TString dir = "/home/d/andrey/test/mu/";//events/much/standard/10mu/mu/";

	TString mcFile = dir + "mc.0000.root";
	TString stsRecoFile = dir + "sts.reco.0000.root";
	TString hitsFile = dir + "much.hits.0000.root";
	TString parFile = dir + "param.0000.root";
	TString outFile = dir + "much.ana.0000.root";

	TStopwatch timer;
	timer.Start();

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
	cbmrootlibs();

	FairRunAna *run= new FairRunAna();
	run->SetInputFile(mcFile);
	run->AddFriend(stsRecoFile);
	run->AddFriend(hitsFile);
	run->SetOutputFile(outFile);

//	FairGeane* Geane = new FairGeane(inFile.Data());

	// -------------------------------------------------------------------------
	CbmMuchTrackFinder* muchTrackFinder    = new CbmMuchTrackFinderIdeal();
	CbmMuchFindTracks* muchFindTracks = new CbmMuchFindTracks("Much Track Finder");
	muchFindTracks->UseFinder(muchTrackFinder);
	run->AddTask(muchFindTracks);

	CbmMuchMatchTracks* muchMatchTracks = new CbmMuchMatchTracks();
	run->AddTask(muchMatchTracks);

//	CbmTrdTrackFinder* trdTrackFinder    = new CbmTrdTrackFinderIdeal();
//	CbmTrdFindTracks* trdFindTracks = new CbmTrdFindTracks("Trd Track Finder");
//	trdFindTracks->UseFinder(trdTrackFinder);
//	run->AddTask(trdFindTracks);
//
//	CbmTrdMatchTracks* trdMatchTracks = new CbmTrdMatchTracks();
//	run->AddTask(trdMatchTracks);

	CbmLitPropagationAnalysis* propAna = new CbmLitPropagationAnalysis(kMUCH);
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
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	cout << endl << endl;
	cout << "Macro finished succesfully." << endl;
	cout << "Output file is "    << outFile << endl;
	cout << "Parameter file is " << parFile << endl;
	cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
	cout << endl;
	// ------------------------------------------------------------------------

	cout << " Test passed" << endl;
	cout << " All ok " << endl;
}
