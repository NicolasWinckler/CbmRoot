/**
 * \file simple_geo_qa.C
 *
 * \brief Check simplified geometry creator.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 *
 **/

void simple_geo_qa(Int_t nEvents = 1)
{
	TString dir = "/data.local1/andrey/events/std_electron_5jpsi/";
	TString mcFile = dir + "mc.0000.root";
	TString parFile = dir + "param.0000.root";
	TString outFile = dir + "simple.geo.qa.0000.root";

	TStopwatch timer;
	timer.Start();

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
	cbmrootlibs();

	FairRunAna *run= new FairRunAna();
	run->SetInputFile(mcFile);
	run->SetOutputFile(outFile);

	// -------------------------------------------------------------------------
	CbmLitSimpleGeoQa* geo = new CbmLitSimpleGeoQa();
	run->AddTask(geo);
	// -------------------------------------------------------------------------


	TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");
	TString stsDigiFile = parDir+ "/sts/sts_v11a.digi.par";

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
	run->Init();
	run->Run(0, nEvents);
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
