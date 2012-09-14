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

   std::string dir = "/u/slebedev/JUL09/trunk/macro/analysis/dielectron/results/10gev/all/";

   results.push_back(dir + "100field/nomvd/real_pid/");
   results.push_back(dir + "100field/nomvd/ideal_pid/");
   results.push_back(dir + "70field/nomvd/real_pid/");
   results.push_back(dir + "70field/nomvd/ideal_pid/");
   results.push_back(dir + "100field/mvd/real_pid/");
   results.push_back(dir + "100field/mvd/ideal_pid/");
   results.push_back(dir + "70field/mvd/real_pid/");
   results.push_back(dir + "70field/mvd/ideal_pid/");

   names.push_back("100field/nomvd/real");
   names.push_back("100field/nomvd/ideal");
   names.push_back("70field/nomvd/real");
   names.push_back("70field/nomvd/ideal");
   names.push_back("100field/mvd/real");
   names.push_back("100field/mvd/ideal");
   names.push_back("70field/mvd/real");
   names.push_back("70field/mvd/ideal");

/*   std::string dir = "/u/slebedev/JUL09/trunk/macro/analysis/dielectron/results/all/100field/mvd/";
   results.push_back(dir + "real_pid/");
   results.push_back(dir + "pi_misid_0_01/");
   results.push_back(dir + "pi_misid_0_002/");
   results.push_back(dir + "pi_misid_0_001/");
   results.push_back(dir + "pi_misid_0_0002/");
   results.push_back(dir + "pi_misid_0_0001/");
   results.push_back(dir + "pi_misid_0_0/");

   names.push_back("real pid");
   names.push_back("0.01");
   names.push_back("0.002");
   names.push_back("0.001");
   names.push_back("0.0002");
   names.push_back("0.0001");
   names.push_back("ideal pid");
   */

   outputDir = "/u/slebedev/JUL09/trunk/macro/analysis/dielectron/results/10gev/all/study_report/";

   CbmAnaDielectronReports reports;
   reports.CreateStudyReport("LMVM", results, names, outputDir);
}
