/** check_field.C
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 *
 * Macro draws field map and checks field map approximation.
 **/

void check_field(Int_t nEvents = 1)
{
	TString script = TString(gSystem->Getenv("SCRIPT"));

	TString dir, mcFile, parFile, outFile;
	// Output directory
	dir = "/d/cbm02/andrey/electron/v10b_10e_urqmd/";
	// MC transport file
	mcFile = dir + "mc.0000.root";
	// Parameter file
	parFile = dir + "param.0000.root";
	// Dummy output file
	outFile = dir + "field.check.0000.root";

	Int_t iVerbose = 1;
	TStopwatch timer;
	timer.Start();

//	gSystem->Load("/home/soft/tbb/libtbb");
//	gSystem->Load("/u/andrey/soft/tbb/Lenny64/libtbb");
//	gSystem->Load("/u/andrey/soft/tbb/Etch32/libtbb");

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
	cbmrootlibs();

	// -----   Reconstruction run   -------------------------------------------
	FairRunAna *run= new FairRunAna();
	run->SetInputFile(mcFile);
	run->SetOutputFile(outFile);
	// ------------------------------------------------------------------------

	CbmLitCheckField* checkField = new CbmLitCheckField();
	checkField->SetXangle(27.);
	checkField->SetYangle(27.);
	checkField->SetNofBinsX(30);
	checkField->SetNofBinsY(30);
	checkField->IsCheckFieldApproximation(true);
	checkField->IsCheckFieldMap(false);
	checkField->IsDrawBx(true);
	checkField->IsDrawBy(true);
	checkField->IsDrawBz(true);
	checkField->IsDrawMod(true);
	checkField->IsDrawFieldMap(true);
	checkField->IsDrawPoly(true);
	checkField->IsDrawSlices(true);
	checkField->IsFixedBounds(false);
	checkField->SetUseEllipseAcc(true);
	checkField->SetPolynomDegreeIndex(0);
	checkField->SetOutputDir("test/");
	run->AddTask(checkField);

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
	cout << endl << endl;
	cout << "Macro finished successfully." << endl;
	cout << "Output file is " << outFile << endl;
	cout << "Parameter file is " << parFile << endl;
	cout << "Real time " << timer.RealTime() << " s, CPU time " << timer.CpuTime() << " s" << endl;
	cout << endl;
	// ------------------------------------------------------------------------
}
