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

   TString script = TString(gSystem->Getenv("SCRIPT"));

   std::vector<std::string> results, names;
   std::string outputDir;
   if (script == "yes") {
      Int_t nofStudies = TString(gSystem->Getenv("NSTUDIES")).Atoi();
      for (Int_t i = 0; i < nofStudies; i++) {
         std::ostringstream ssresult, ssname;
         ssresult << "STUDYRESULT" << i+1;
         ssname << "STUDYNAME" << i+1;
         results.push_back(gSystem->Getenv(ssresult.str().c_str()));
         names.push_back(gSystem->Getenv(ssname.str().c_str()));
      }
      outputDir = std::string(gSystem->Getenv("STUDYOUTPUTDIR"));
   } else {
      results.push_back("/u/andrey/cbm/results/trd_test_jan12/results_trd_v10b_clustering/");
      results.push_back("/u/andrey/cbm/results/trd_test_jan12/results_trd_v10b_smearing/");
      results.push_back("/u/andrey/cbm/results/trd_test_jan12/results_trd_v11a_smearing/");
      results.push_back("/u/andrey/cbm/results/trd_test_jan12/results_trd_v11c_clustering/");
      results.push_back("/u/andrey/cbm/results/trd_test_jan12/results_trd_v11c_smearing/");

      names.push_back("v10b_clustering");
      names.push_back("v10b_smearing");
      names.push_back("v11a_smearing");
      names.push_back("v11c_clustering");
      names.push_back("v11c_smearing");

      outputDir = "./trd_test/";
   }

   CbmLitTrackingQa trackingQa;
   trackingQa.SetOutputDir(outputDir);
   trackingQa.CreateStudyReport("Tracking QA", results, names);

//   CbmLitClusteringQa clusteringQa;
//   clusteringQa.SetOutputDir(outputDir);
//   clusteringQa.CreateStudyReport("Clustering QA", results, names);
}
