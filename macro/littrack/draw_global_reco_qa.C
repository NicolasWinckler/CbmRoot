#include "../../cbmbase/CbmDetectorList.h"

void draw_global_reco_qa()
{
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
	cbmrootlibs();

	std::string fileName = "/d/cbm02/andrey/electron/std_10e_urqmd_no_mvd/global.reco.0000.root";
	std::string imageDir = "./test/";

	CbmLitReconstructionQa* reconstructionQa = new CbmLitReconstructionQa();
	reconstructionQa->SetOutputDir(imageDir);
	reconstructionQa->SetIsElectronSetup(true);
	reconstructionQa->SetDetectorPresence(kMVD, false);
	reconstructionQa->SetDetectorPresence(kSTS, true);
	reconstructionQa->SetDetectorPresence(kRICH, true);
	reconstructionQa->SetDetectorPresence(kTRD, true);
	reconstructionQa->SetDetectorPresence(kMUCH, false);
	reconstructionQa->SetDetectorPresence(kTOF, true);
	reconstructionQa->DrawHistosFromFile(fileName);
}
