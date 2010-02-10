/** create_field.C
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2010
 * @version 1.0
 *
 * Macro convert an ASCII file with the magnetic field
 * to the ROOT format.
 **/

void create_field()
{
	// Input ASCII file
	const char* fieldASCII = "NewField";
	//Output ROOT file
	const char* fieldROOT = "/d/cbm02/andrey/field/NewField.root";
	const char* mapName = "NewMap";

	TStopwatch timer;
	timer.Start();

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
	cbmrootlibs();

	CbmFieldMapSym2* fieldMap = new CbmFieldMapSym2(fieldASCII, "A");
	fieldMap->Init();
	fieldMap->Print();
	fieldMap->WriteRootFile(fieldROOT, mapName);

	// -----   Finish   -------------------------------------------------------
	timer.Stop();
	cout << endl << endl;
	cout << "Macro finished successfully." << endl;
	cout << "Output file is " << fieldROOT << endl;
	cout << "Real time " << timer.RealTime() << " s, CPU time " << timer.CpuTime() << " s" << endl;
	// ------------------------------------------------------------------------
}
