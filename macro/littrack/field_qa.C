/**
 * \file field_qa.C
 * \brief Macro runs CbmLitFieldQa task which checks field map and its approximation.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 *
 **/

#include <iostream>

void field_qa(Int_t nEvents = 1)
{
	TString script = TString(gSystem->Getenv("SCRIPT"));

	TString dir, mcFile, parFile, outFile;
	// Output directory
	dir = "./commit_tests/events_electron/";
	// MC transport file
	mcFile = dir + "mc.0000.root";
	// Parameter file
	parFile = dir + "param.0000.root";
	// Dummy output file
	outFile = dir + "field.qa.0000.root";

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
	run->SetOutputFile(outFile);
	// ------------------------------------------------------------------------

	CbmLitFieldQa* fieldQa = new CbmLitFieldQa();

	std::vector<Double_t> zPos;
    zPos.push_back(170.);
    zPos.push_back(180.);
    zPos.push_back(190.);
    zPos.push_back(200.);
    zPos.push_back(210.);
    fieldQa->SetSliceZPosition(zPos);
	fieldQa->SetAcceptanceAngleX(35.);
	fieldQa->SetAcceptanceAngleY(35.);
	fieldQa->SetNofBinsX(30);
	fieldQa->SetNofBinsY(30);
	fieldQa->SetZMin(-10);
	fieldQa->SetZMax(300);
	fieldQa->SetZStep(5);

	fieldQa->SetOutputDir("./test/");
	run->AddTask(fieldQa);

	// -----  Parameter database   --------------------------------------------
	FairRuntimeDb* rtdb = run->GetRuntimeDb();
	FairParRootFileIo* parIo1 = new FairParRootFileIo();
	parIo1->open(parFile.Data());
	rtdb->setFirstInput(parIo1);
	rtdb->setOutput(parIo1);
	rtdb->saveOutput();
	// ------------------------------------------------------------------------

	// -----   Initialize and run   --------------------------------------------
	run->Init();
	run->Run(0, nEvents);
	// ------------------------------------------------------------------------

	// -----   Finish   -------------------------------------------------------
	timer.Stop();
   std::cout << "Macro finished successfully." << std::endl;
   std::cout << "Output file is " << outFile << std::endl;
   std::cout << "Parameter file is " << parFile << std::endl;
   std::cout << "Real time " << timer.RealTime() << " s, CPU time " << timer.CpuTime() << " s" << std::endl;
	// ------------------------------------------------------------------------
}
