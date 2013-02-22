/**
 * \file qa_study_report.C
 * \brief Macro for generation of summary reports.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */

void qa_study_report()
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
      fileNames.push_back("scripts/events_trd_v10b/global.reco.smearing.branch_new.0000.root");
      fileNames.push_back("scripts/events_trd_v10b/global.reco.smearing.nn_new.0000.root");
      fileNames.push_back("scripts/events_trd_v11c/global.reco.smearing.branch_new.0000.root");
      fileNames.push_back("scripts/events_trd_v11c/global.reco.smearing.nn_new.0000.root");

      studyNames.push_back("v10b_branch");
      studyNames.push_back("v10b_nn");
      studyNames.push_back("v11c_branch");
      studyNames.push_back("v11c_nn");

      outputDir = "./test/";
   }

   CbmStudyReport* trackingQa = new CbmLitTrackingQaStudyReport();
   trackingQa->Create(fileNames, studyNames, outputDir);

   CbmStudyReport* clusteringQa = new CbmLitClusteringQaStudyReport();
   clusteringQa->Create(fileNames, studyNames, outputDir);
}
