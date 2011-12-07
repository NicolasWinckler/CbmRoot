/**
 * \file draw_global_reco_qa.C
 * \brief Macroe draws histograms produced by tracking QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2010
 */

#include "../../cbmbase/CbmDetectorList.h"

void draw_global_reco_qa()
{
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
	cbmrootlibs();

	std::string fileName = "/data.local1/andrey/sts_qa/sts4_events_electron/global.tracks.0000.root";
	std::string imageDir = "/data.local1/andrey/sts_qa/sts4_results_electron/";

	CbmLitTrackingQa* reconstructionQa = new CbmLitTrackingQa();
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
