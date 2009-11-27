
void draw_dedx() {
	gSystem->Load("/home/soft/tbb22_004oss/libtbb");

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
	cbmrootlibs();

	CbmLitCheckEnergyLoss checker;
	checker.SetMaterial("iron");
	checker.Check();

//	CbmLitCheckBrem checker;
//	checker.SetMaterial("iron");
//	checker.Check();
}
