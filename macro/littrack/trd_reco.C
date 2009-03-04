#include "../../cbmbase/CbmDetectorList.h";
void trd_reco(Int_t nEvents = 500)
{
	TString script = TString(gSystem->Getenv("SCRIPT"));

	TString dir, mcFile, parFile, stsRecoFile, trdHitsFile, trdTracksFile;
	if (script != "yes") {
		dir  = "/home/d/andrey/events/trd/segmented/10e/e_urqmd/";
		mcFile = dir + "mc.0000.root";
		parFile = dir + "param.0000.root";
		stsRecoFile = dir + "sts.reco.0000.root";
		trdHitsFile = dir + "trd.hits.0000.root";
		trdTracksFile = dir + "trd.tracks.nn.0000.root";
	} else {
		mcFile = TString(gSystem->Getenv("MCFILE"));
		parFile = TString(gSystem->Getenv("PARFILE"));
		stsRecoFile = TString(gSystem->Getenv("STSRECOFILE"));
		trdHitsFile = TString(gSystem->Getenv("TRDHITSFILE"));
		trdTracksFile = TString(gSystem->Getenv("TRDTRACKSFILE"));
	}

	Int_t iVerbose = 1;
	TStopwatch timer;
	timer.Start();

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
	cbmrootlibs();

	// -----   Reconstruction run   -------------------------------------------
	FairRunAna *run= new FairRunAna();
	run->SetInputFile(mcFile);
	run->AddFriend(stsRecoFile);
	run->AddFriend(trdHitsFile);
	run->SetOutputFile(trdTracksFile);
	// ------------------------------------------------------------------------

//	CbmKF* kalman = new CbmKF();
//	run->AddTask(kalman);
//	CbmL1* l1 = new CbmL1();
//	run->AddTask(l1);

	// -----   TRD track finding   ---------------------------------------------
	//CbmTrdTrackFinder* trdTrackFinder    = new CbmL1TrdTrackFinderSts();
	//((CbmL1TrdTrackFinderSts*)trdTrackFinder)->SetVerbose(iVerbose);
	CbmTrdTrackFinder* trdTrackFinder    = new CbmLitTrdTrackFinderNN();
	CbmTrdFindTracks* trdFindTracks = new CbmTrdFindTracks("TRD Track Finder");
	trdFindTracks->UseFinder(trdTrackFinder);
	run->AddTask(trdFindTracks);
	// -------------------------------------------------------------------------

	// -----   TRD track matching   --------------------------------------------
	CbmTrdMatchTracks* trdMatchTracks = new CbmTrdMatchTracks(1);
	run->AddTask(trdMatchTracks);

	// -----   TRD track finding QA check   ------------------------------------
	CbmLitRecQa* trdRecQa = new CbmLitRecQa(8, 0.7, kTRD, 1);
	trdRecQa->SetNormType(2); // '2' to number of STS tracks
	run->AddTask(trdRecQa);

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
	run->Run(0,nEvents);
	// ------------------------------------------------------------------------

	// -----   Finish   -------------------------------------------------------
	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	cout << endl << endl;
	cout << "Macro finished succesfully." << endl;
	cout << "Output file is "    << trdTracksFile << endl;
	cout << "Parameter file is " << parFile << endl;
	cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
	cout << endl;
	// ------------------------------------------------------------------------

	cout << " Test passed" << endl;
	cout << " All ok " << endl;
}
