#include "../../cbmbase/CbmDetectorList.h";
void global_reco(Int_t nEvents = 5)
{
	TString script = TString(gSystem->Getenv("SCRIPT"));

	TString dir, mcFile, parFile, stsRecoFile, trdHitsFile;
	if (script != "yes") {
//		dir  = "/home/d/andrey/events/trd/segmented/10e/e_urqmd/";
		dir  = "/home/d/andrey/test/trunk/global_e/";
		mcFile = dir + "mc.0000.root";
		parFile = dir + "param.0000.root";
		stsRecoFile = dir + "sts.reco.0000.root";
		trdHitsFile = dir + "trd.hits.0000.root";
		tofHitsFile = dir + "tof.hits.0000.root";
		globalTracksFile = dir + "global.tracks.0000.root";
	} else {
		mcFile = TString(gSystem->Getenv("MCFILE"));
		parFile = TString(gSystem->Getenv("PARFILE"));
		stsRecoFile = TString(gSystem->Getenv("STSRECOFILE"));
		trdHitsFile = TString(gSystem->Getenv("TRDHITSFILE"));
		trdTracksFile = TString(gSystem->Getenv("TRDTRACKSFILE"));
		tofHitsFile = TString(gSystem->Getenv("TOFHITSFILE"));
		globalTracksFile = TString(gSystem->Getenv("GLOBALTRACKSFILE"));
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
	run->AddFriend(tofHitsFile);
	run->SetOutputFile(globalTracksFile);
	// ------------------------------------------------------------------------



	CbmLitFindGlobalTracks* finder = new CbmLitFindGlobalTracks();
	run->AddTask(finder);

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
	cout << "Output file is "    << globalTracksFile << endl;
	cout << "Parameter file is " << parFile << endl;
	cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
	cout << endl;
	// ------------------------------------------------------------------------

	cout << " Test passed" << endl;
	cout << " All ok " << endl;
}
