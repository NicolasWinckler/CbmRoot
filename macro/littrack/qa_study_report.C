/**
 * \file qa_study.C
 * \brief Macro for generation of summary reports.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */

#include "../../cbmbase/CbmDetectorList.h"

void qa_study()
{
   gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/loadlibs.C");
   loadlibs();

   TString script = TString(gSystem->Getenv("LIT_SCRIPT"));

   std::vector<std::string> fileNames, studyNames;
   std::string outputDir;
   if (script == "yes") {
      Int_t nofStudies = TString(gSystem->Getenv("LIT_NOF_STUDIES")).Atoi();
      for (Int_t i = 0; i < nofStudies; i++) {
         std::ostringstream ssresult, ssname;
         ssresult << "LIT_FILE_NAME" << i+1;
         ssname << "LIT_STUDY_NAME" << i+1;
         fileNames.push_back(gSystem->Getenv(ssresult.str().c_str()));
         studyNames.push_back(gSystem->Getenv(ssname.str().c_str()));
      }
      outputDir = std::string(gSystem->Getenv("LIT_STUDY_OUTPUT_DIR"));
   } else {
      fileNames.push_back("/Users/andrey/Development/cbm/trunk/cbmroot/macro/littrack/scripts/trd_v12alds_smearing_branch/");
      fileNames.push_back("/Users/andrey/Development/cbm/trunk/cbmroot/macro/littrack/scripts/trd_v12altof_smearing_branch/");
      fileNames.push_back("/Users/andrey/Development/cbm/trunk/cbmroot/macro/littrack/scripts/trd_v12alnotrd_smearing_branch/");

      studyNames.push_back("v12alds");
      studyNames.push_back("v12altof");
      studyNames.push_back("v12alnotrd");

      outputDir = "./test/";
   }

   CbmStudyReport* trackingQa = new CbmLitTrackingQaStudyReport();
   trackingQa->Create(fileNames, studyNames, outputDir);

   CbmStudyReport* clusteringQa = new CbmLitClusteringQaStudyReport();
   clusteringQa->Create(fileNames, studyNames, outputDir);
}
