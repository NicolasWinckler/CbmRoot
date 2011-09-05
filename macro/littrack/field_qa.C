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
	dir = "/data.local1/andrey/events/std_electron/";
	// MC transport file
	mcFile = dir + "mc.0000.root";
	// Parameter file
	parFile = dir + "param.0000.root";
	// Dummy output file
	outFile = dir + "field.qa.0000.root";

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

	CbmLitFieldQa* fieldQa = new CbmLitFieldQa();
	fieldQa->SetXangle(35.);
	fieldQa->SetYangle(35.);
	fieldQa->SetNofBinsX(30);
	fieldQa->SetNofBinsY(30);
	fieldQa->IsCheckFieldApproximation(false);
	fieldQa->IsCheckFieldMap(true);
	fieldQa->IsCheckGridCreator(true);
	fieldQa->IsDrawBx(true);
	fieldQa->IsDrawBy(true);
	fieldQa->IsDrawBz(true);
	fieldQa->IsDrawMod(true);
	fieldQa->IsDrawFieldMap(true);
	fieldQa->IsDrawPoly(false);
	fieldQa->IsDrawSlices(true);
	fieldQa->IsFixedBounds(false);
	fieldQa->SetUseEllipseAcc(false);
	fieldQa->SetPolynomDegreeIndex(0);
	fieldQa->SetOutputDir("test3/");
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
	cout << endl << endl;
	cout << "Macro finished successfully." << endl;
	cout << "Output file is " << outFile << endl;
	cout << "Parameter file is " << parFile << endl;
	cout << "Real time " << timer.RealTime() << " s, CPU time " << timer.CpuTime() << " s" << endl;
	cout << endl;
	// ------------------------------------------------------------------------
}
