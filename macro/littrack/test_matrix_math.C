//#include "../../littrack/utils/CbmLitMatrixMath.cxx"
//#include "../../littrack/utils/CbmLitMatrixMath.h"

void test_matrix_math()
{
	gSystem->Load("/home/soft/tbb/libtbb");
	gSystem->Load("/u/andrey/soft/tbb/Lenny64/libtbb");
	gSystem->Load("/u/andrey/soft/tbb/Etch32/libtbb");

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
	cbmrootlibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/determine_setup.C");


	CbmLitTestMatrixMath* test = new CbmLitTestMatrixMath();
	test->RunTest();

}

