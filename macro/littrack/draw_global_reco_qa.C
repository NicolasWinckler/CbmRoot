#include "../../cbmbase/CbmDetectorList.h"

void draw_global_reco_qa()
{
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
	cbmrootlibs();

	std::string fileName = "/d/cbm02/slebedev/rich/JUL09/auau.25gev.centr.0000.reco.root";
	std::string imageDir = "recoIm/";

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
