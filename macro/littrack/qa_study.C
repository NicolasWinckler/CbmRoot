#include "../../cbmbase/CbmDetectorList.h"

void qa_study()
{
   gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
   basiclibs();
   gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
   cbmrootlibs();

   TString script = TString(gSystem->Getenv("SCRIPT"));

   std::vector<std::string> results, names;
   std::string outputDir;
   Int_t electronSetup, mvd, sts, rich, trd, much, tof;
   if (script == "yes") {
      Int_t nofStudies = TString(gSystem->Getenv("NSTUDIES")).Atoi();
      for (Int_t i = 0; i < nofStudies; i++) {
         std::ostringstream ssresult, ssname;
         ssresult << "STUDYRESULT" << i+1;
         ssname << "STUDYNAME" << i+1;
         results.push_back(gSystem->Getenv(ssresult.str().c_str()));
         names.push_back(gSystem->Getenv(ssname.str().c_str()));

	 std::cout << "STUDYRESULT" << i+1 << "=" << results[i] << std::endl;
         std::cout << "STUDYNAME" << i+1 << "=" << names[i] << std::endl;

      }
      outputDir = std::string(gSystem->Getenv("STUDYOUTPUTDIR"));
      std::cout << "STUDYOUTPUTDIR=" << outputDir << std::endl;
      electronSetup = TString(gSystem->Getenv("STUDYELECTRONSETUP")).Atoi();
      mvd = TString(gSystem->Getenv("STUDYMVD")).Atoi();
      sts = TString(gSystem->Getenv("STUDYSTS")).Atoi();
      rich = TString(gSystem->Getenv("STUDYRICH")).Atoi();
      trd = TString(gSystem->Getenv("STUDYTRD")).Atoi();
      much = TString(gSystem->Getenv("STUDYMUCH")).Atoi();
      tof = TString(gSystem->Getenv("STUDYTOF")).Atoi();
   } else {
   //   results.push_back("/u/andrey/cbm/results/STS_QA/muon/sts_v09a.geo_sts_v09a.digi.par_results_muon");
//      results.push_back("/u/andrey/cbm/results/STS_QA/muon/sts_v11a.geo_sts_v11a.digi.par_results_muon");
//      results.push_back("/u/andrey/cbm/results/STS_QA/muon/sts_v11a.geo_sts_v11a_0-4.digi.par_results_muon");
//      results.push_back("/u/andrey/cbm/results/STS_QA/muon/sts_v11a.geo_sts_v11a_0-6.digi.par_results_muon");
//      results.push_back("/u/andrey/cbm/results/STS_QA/muon/sts_v11a.geo_sts_v11a_0-7.digi.par_results_muon");
//      results.push_back("/u/andrey/cbm/results/STS_QA/muon/sts_v11a.geo_sts_v11a_0-8.digi.par_results_muon");
      results.push_back("/u/andrey/cbm/results/STS_QA/muon/sts_v11b.geo_sts_v11b.digi.par_results_muon");
      results.push_back("/u/andrey/cbm/results/STS_QA/muon/sts_v11b.geo_sts_v11b_0-4.digi.par_results_muon");
      results.push_back("/u/andrey/cbm/results/STS_QA/muon/sts_v11b.geo_sts_v11b_0-6.digi.par_results_muon");
      results.push_back("/u/andrey/cbm/results/STS_QA/muon/sts_v11b.geo_sts_v11b_0-8.digi.par_results_muon");

   //   names.push_back("v09a");
//      names.push_back("v11a");
//      names.push_back("v11a_0-4");
//      names.push_back("v11a_0-6");
   //   names.push_back("v11a_0-7");
//      names.push_back("v11a_0-8");
      names.push_back("v11b");
      names.push_back("v11b_0-4");
      names.push_back("v11b_0-6");
      names.push_back("v11b_0-8");

//      results.push_back("/u/andrey/cbm/trunk/cbmroot/macro/littrack/sts_qa/4conspoints");
//      results.push_back("/u/andrey/cbm/trunk/cbmroot/macro/littrack/sts_qa/4points");
//      results.push_back("/u/andrey/cbm/trunk/cbmroot/macro/littrack/sts_qa/5points");
//      results.push_back("/u/andrey/cbm/trunk/cbmroot/macro/littrack/sts_qa/6points");
//      results.push_back("/u/andrey/cbm/trunk/cbmroot/macro/littrack/sts_qa/7points");
//      results.push_back("/u/andrey/cbm/trunk/cbmroot/macro/littrack/sts_qa/8points");
//
//      names.push_back("4 con points");
//      names.push_back("4 points");
//      names.push_back("5 points");
//      names.push_back("6 points");
//      names.push_back("7 points");
//      names.push_back("8 points");

      outputDir = "./sts_qa/muon/";
      electronSetup = 0;
      mvd = 0;
      sts = 1;
      rich = 0;
      trd = 0;
      much = 1;
      tof = 1;
   }

   CbmLitTrackingQa qa;
   qa.SetOutputDir(outputDir);
   qa.SetIsElectronSetup(electronSetup);
   qa.SetDetectorPresence(kMVD, mvd);
   qa.SetDetectorPresence(kSTS, sts);
   qa.SetDetectorPresence(kRICH, rich);
   qa.SetDetectorPresence(kTRD, trd);
   qa.SetDetectorPresence(kMUCH, much);
   qa.SetDetectorPresence(kTOF, tof);
   qa.CreateReport(kLitHtml, "Event reconstruction performance", results, names);
}
