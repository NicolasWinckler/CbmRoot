#include "../../cbmbase/CbmDetectorList.h";
void tof_hits(Int_t nEvents = 100)
{
	TString script = TString(gSystem->Getenv("SCRIPT"));

	TString dir, mcFile, parFile, trdHitsFile;
	if (script != "yes") {
//		dir  = "/d/cbm02/andrey/events/trd/segmented/10e/e/";
		dir  = "/home/d/andrey/test/trunk/global_mu_urqmd/";
		mcFile = dir + "mc.0000.root";
		parFile = dir + "param.0000.root";
		tofHitsFile = dir + "tof.hits.0000.root";
	} else {
		mcFile = TString(gSystem->Getenv("MCFILE"));
		parFile = TString(gSystem->Getenv("PARFILE"));
		tofHitsFile = TString(gSystem->Getenv("TOFHITSFILE"));
	}

	TStopwatch timer;
	timer.Start();

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
	cbmrootlibs();

	// -----   Reconstruction run   -------------------------------------------
	FairRunAna *run= new FairRunAna();
	run->SetInputFile(mcFile);
	run->SetOutputFile(tofHitsFile);
	// ------------------------------------------------------------------------


	// ------   TOF hit producer   ---------------------------------------------
	CbmTofHitProducer* tofHitProd = new CbmTofHitProducer("TOF HitProducer", 1);
	run->AddTask(tofHitProd);
	// -------------------------------------------------------------------------


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
	run->Run(0,nEvents);
	// ------------------------------------------------------------------------

	// -----   Finish   -------------------------------------------------------
	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	cout << endl << endl;
	cout << "Macro finished succesfully." << endl;
	cout << "Output file is "    << trdHitsFile << endl;
	cout << "Parameter file is " << parFile << endl;
	cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
	cout << endl;
	// ------------------------------------------------------------------------

	cout << " Test passed" << endl;
	cout << " All ok " << endl;
}
