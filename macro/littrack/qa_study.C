/**
 * \file qa_study.C
 * \brief Macro for generation of summary reports.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */

#include "../../cbmbase/CbmDetectorList.h"

void qa_study()
{
   gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
   basiclibs();
   gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
   cbmrootlibs();

   TString script = TString(gSystem->Getenv("LIT_SCRIPT"));

   std::vector<std::string> results, names;
   std::string outputDir;
   if (script == "yes") {
      Int_t nofStudies = TString(gSystem->Getenv("LIT_NOF_STUDIES")).Atoi();
      for (Int_t i = 0; i < nofStudies; i++) {
         std::ostringstream ssresult, ssname;
         ssresult << "LIT_STUDY_RESULT" << i+1;
         ssname << "LIT_STUDY_NAME" << i+1;
         results.push_back(gSystem->Getenv(ssresult.str().c_str()));
         names.push_back(gSystem->Getenv(ssname.str().c_str()));
      }
      outputDir = std::string(gSystem->Getenv("LIT_STUDY_OUTPUT_DIR"));
   } else {
      results.push_back("/Users/andrey/Development/cbm/trunk/cbmroot/macro/littrack/scripts/trd_v12alds_smearing_branch/");
      results.push_back("/Users/andrey/Development/cbm/trunk/cbmroot/macro/littrack/scripts/trd_v12altof_smearing_branch/");
      results.push_back("/Users/andrey/Development/cbm/trunk/cbmroot/macro/littrack/scripts/trd_v12alnotrd_smearing_branch/");
//      results.push_back("/Users/andrey/Development/cbm/trunk/cbmroot/macro/littrack/scripts/trd_v12alds_smearing_branch/");
//      results.push_back("/Users/andrey/Development/cbm/trunk/cbmroot/macro/littrack/scripts/trd_v12ales_smearing_branch/");
//      results.push_back("/Users/andrey/Development/cbm/trunk/cbmroot/macro/littrack/scripts/trd_v12alfs_smearing_branch/");
//      results.push_back("/Users/andrey/Development/cbm/trunk/cbmroot/macro/littrack/scripts/trd_v12algs_smearing_branch/");
//      results.push_back("/Users/andrey/Development/cbm/trunk/cbmroot/macro/littrack/scripts/trd_v12alhs_smearing_branch/");
//      results.push_back("/Users/andrey/Development/cbm/trunk/cbmroot/macro/littrack/scripts/trd_v12alis_smearing_branch/");
//      results.push_back("/Users/andrey/Development/cbm/trunk/cbmroot/macro/littrack/scripts/trd_v12aljs_smearing_branch/");

      names.push_back("v12alds");
      names.push_back("v12altof");
      names.push_back("v12alnotrd");
//      names.push_back("v12alds");
//      names.push_back("v12ales");
//      names.push_back("v12alfs");
//      names.push_back("v12algs");
//      names.push_back("v12alhs");
//      names.push_back("v12alis");
//      names.push_back("v12aljs");

      outputDir = "./test/";
   }

   CbmLitTrackingQa trackingQa;
   trackingQa.SetOutputDir(outputDir);
   trackingQa.CreateStudyReport("Tracking QA", results, names);

   CbmLitClusteringQa clusteringQa;
   clusteringQa.SetOutputDir(outputDir);
   clusteringQa.CreateStudyReport("Clustering QA", results, names);
}
