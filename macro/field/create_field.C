/**
 * \file create_field.C
 * \brief Macro converts ASCII file with magnetic field to ROOT file.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2010
 */

#include <iostream>
using std::cout;
using std::endl;

void create_field(TString fileName="CwPolus6_Cs_2", const char* fileType="A", TString symmetry="sym3", TString outputFile="field_v12b.root", TString mapName="field_v12b")
{
	TStopwatch timer;
	timer.Start();

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
	cbmrootlibs();

	if (symmetry == "sym2" ) {
	  CbmFieldMapSym2* fieldMap = new CbmFieldMapSym2(fileName, fileType);
	} else if (symmetry == "sym3" ) {
	  CbmFieldMapSym3* fieldMap = new CbmFieldMapSym3(fileName, fileType);
	} else {
          CbmFieldMap* fieldMap = new CbmFieldMap(fileName, fileType);
	}
	fieldMap->Init();
	fieldMap->Print();
	fieldMap->WriteRootFile(outputFile, mapName);
	fieldMap->Print();

	// -----   Finish   -------------------------------------------------------
	timer.Stop();
	cout << endl << endl;
	cout << "Macro finished successfully." << endl;
	cout << "Output file is " << outputFile << endl;
	cout << "Real time " << timer.RealTime() << " s, CPU time " << timer.CpuTime() << " s" << endl;
	// ------------------------------------------------------------------------
}
