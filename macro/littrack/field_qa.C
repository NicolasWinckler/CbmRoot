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
	fieldQa->SetCheckFieldMap(true);
	fieldQa->SetCheckFieldApproximation(true);
	fieldQa->SetCheckGridCreator(true);

	std::vector<Double_t> zPos;
//   zPos.push_back(30.);
//   zPos.push_back(50.);
   zPos.push_back(170.);
   zPos.push_back(180.);
   zPos.push_back(190.);
   zPos.push_back(200.);
   zPos.push_back(210.);
//   zPos.push_back(150.);
//   zPos.push_back(200.);
//   zPos.push_back(300.);
//   zPos.push_back(400.);
   fieldQa->SetSliceZPosition(zPos);

   std::vector<UInt_t> degrees;
// degrees.push_back(3);
//   degrees.push_back(5);
   degrees.push_back(7);
// degrees.push_back(9);
   fieldQa->SetPolynomDegrees(degrees);

	fieldQa->SetXangle(35.);
	fieldQa->SetYangle(35.);
	fieldQa->SetNofBinsX(100);
	fieldQa->SetNofBinsY(100);

	fieldQa->SetDrawFieldMap(true);
	fieldQa->SetDrawFieldApproximation(true);
    fieldQa->SetDrawGridCreator(true);
	fieldQa->SetDrawBx(true);
	fieldQa->SetDrawBy(true);
	fieldQa->SetDrawBz(true);
	fieldQa->SetDrawMod(true);
	fieldQa->IsFixedBounds(false);
	fieldQa->SetUseEllipseAcc(false);
	fieldQa->SetPolynomDegreeIndex(0);
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
