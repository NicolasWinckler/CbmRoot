#include "../../cbmbase/CbmDetectorList.h"

void qa_study()
{
   gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
   basiclibs();
   gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
   cbmrootlibs();

   std::vector<std::string> results, names;
//   results.push_back("/u/andrey/cbm/results/STS_QA/muon/sts_v09a.geo_sts_v09a.digi.par_results_muon");
   results.push_back("/u/andrey/cbm/results/STS_QA/sts_v11a.geo_sts_v11a.digi.par_results_muon");
//   results.push_back("/u/andrey/cbm/results/STS_QA/sts_v11a.geo_sts_v11a_0-4.digi.par_results_muon");
   results.push_back("/u/andrey/cbm/results/STS_QA/sts_v11a.geo_sts_v11a_0-6.digi.par_results_muon");
//   results.push_back("/u/andrey/cbm/results/STS_QA/muon/sts_v11a.geo_sts_v11a_0-7.digi.par_results_muon");
   results.push_back("/u/andrey/cbm/results/STS_QA/sts_v11a.geo_sts_v11a_0-8.digi.par_results_muon");
//   results.push_back("/u/andrey/cbm/results/STS_QA/sts_v11b.geo_sts_v11b.digi.par_results_muon");
//   results.push_back("/u/andrey/cbm/results/STS_QA/sts_v11b.geo_sts_v11b_0-4.digi.par_results_muon");
//   results.push_back("/u/andrey/cbm/results/STS_QA/sts_v11b.geo_sts_v11b_0-6.digi.par_results_muon");
//   results.push_back("/u/andrey/cbm/results/STS_QA/sts_v11b.geo_sts_v11b_0-8.digi.par_results_muon");

//   names.push_back("v09a");
   names.push_back("v11a");
//   names.push_back("v11a_0-4");
   names.push_back("v11a_0-6");
//   names.push_back("v11a_0-7");
   names.push_back("v11a_0-8");
//   names.push_back("v11b");
//   names.push_back("v11b_0-4");
//   names.push_back("v11b_0-6");
//   names.push_back("v11b_0-8");

   CbmLitReconstructionQa qa;
   qa.SetIsElectronSetup(false);
   qa.SetDetectorPresence(kMVD, false);
   qa.SetDetectorPresence(kSTS, true);
   qa.SetDetectorPresence(kRICH, false);
   qa.SetDetectorPresence(kTRD, false);
   qa.SetDetectorPresence(kMUCH, true);
   qa.SetDetectorPresence(kTOF, true);
   qa.CreateStudyHTML("Event reconstruction for different stereo angles in STS", results, names);
}
