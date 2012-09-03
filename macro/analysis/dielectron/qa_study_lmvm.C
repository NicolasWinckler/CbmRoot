/**
 * \file qa_study_lmvm.C
 * \brief Macro for generation of summary reports for LMVM analysis.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2012
 */

void qa_study_lmvm()
{
   gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
   basiclibs();
   gROOT->LoadMacro("$VMCWORKDIR/macro/rich/cbmlibs.C");
   cbmlibs();
   gSystem->Load("libAnalysis");

   std::vector<std::string> results, names;
   std::string outputDir;
   results.push_back("/u/slebedev/JUL09/trunk/macro/analysis/dielectron/results/all/100field/nomvd/");
   results.push_back("/u/slebedev/JUL09/trunk/macro/analysis/dielectron/results/all/70field/nomvd/");
   results.push_back("/u/slebedev/JUL09/trunk/macro/analysis/dielectron/results/all/100field/mvd/");
   results.push_back("/u/slebedev/JUL09/trunk/macro/analysis/dielectron/results/all/70field/mvd/");


   names.push_back("100field/nomvd");
   names.push_back("70field/nomvd");
   names.push_back("100field/mvd");
   names.push_back("70field/mvd");

   outputDir = "/u/slebedev/JUL09/trunk/macro/analysis/dielectron/results/all/study_report/";

   CbmAnaDielectronReports reports;
   reports.CreateStudyReport("LMVM", results, names, outputDir);
}
