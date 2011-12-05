void trd_elid_qa(Int_t nEvents = 10)
{
	TString inMc = "/d/cbm06/user/slebedev/trd/JUL09/sim/st/piel.0000.mc.root";
	TString inRec = "/d/cbm06/user/slebedev/trd/JUL09/reco/param1/st/piel.0000.reco.root";
	TString parFile = "/d/cbm06/user/slebedev/trd/JUL09/sim/st/piel.0000.params.root";
	TString outFile = "/d/cbm06/user/slebedev/trd/JUL09/reco/param1/st/piel.0000.electronsQa.root";

	gDebug = 0;

	TStopwatch timer;
	timer.Start();

   // ----  Load libraries   -------------------------------------------------
   gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
   basiclibs();
   gROOT->LoadMacro("$VMCWORKDIR/macro/rich/cbmlibs.C");
   cbmlibs();
   gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/determine_setup.C");


	FairRunAna *run= new FairRunAna();
	run->SetInputFile(inMc);
	run->AddFriend(inRec);
	run->SetOutputFile(outFile);

	CbmTrdElectronsQa* elQa = new CbmTrdElectronsQa();
	run->AddTask(elQa);

	// -----  Parameter database   --------------------------------------------
	FairRuntimeDb* rtdb = run->GetRuntimeDb();
	FairParRootFileIo* parIo1 = new FairParRootFileIo();
	parIo1->open(parFile.Data());
	rtdb->setFirstInput(parIo1);
	rtdb->setOutput(parIo1);
	rtdb->saveOutput();

	run->Init();
	run->Run(0,nEvents);

}
