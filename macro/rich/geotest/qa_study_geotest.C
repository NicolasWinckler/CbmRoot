/**
 * \file qa_study_geotest.C
 * \brief Macro for generation of summary reports for RICH geometry testing.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2012
 */

void qa_study_geotest()
{
   gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
   basiclibs();
   gROOT->LoadMacro("$VMCWORKDIR/macro/rich/cbmlibs.C");
   cbmlibs();

   std::vector<std::string> results, names;
   std::string outputDir;
   results.push_back("/u/slebedev/JUL09/trunk/macro/rich/geotest/results0000/");
  // results.push_back("/u/slebedev/JUL09/trunk/macro/rich/geotest/results0001/");
  // results.push_back("/u/slebedev/JUL09/trunk/macro/rich/geotest/results0002/");
  // results.push_back("/u/slebedev/JUL09/trunk/macro/rich/geotest/results0003/");
   //results.push_back("/u/slebedev/JUL09/trunk/macro/rich/geotest/results0004/");
  // results.push_back("/u/slebedev/JUL09/trunk/macro/rich/geotest/results0005/");
//   results.push_back("/u/slebedev/JUL09/trunk/macro/rich/geotest/results0006/");
//   results.push_back("/u/slebedev/JUL09/trunk/macro/rich/geotest/results0007/");
//   results.push_back("/u/slebedev/JUL09/trunk/macro/rich/geotest/results0008/");
//   results.push_back("/u/slebedev/JUL09/trunk/macro/rich/geotest/results0009/");

   names.push_back("0000");
 //  names.push_back("0001");
 //  names.push_back("0002");
 //  names.push_back("0003");
  // names.push_back("0004");
 //  names.push_back("0005");
//   names.push_back("0006");
//   names.push_back("0007");
//   names.push_back("0008");
//   names.push_back("0009");

   outputDir = "/u/slebedev/JUL09/trunk/macro/rich/geotest/results/";

   CbmRichGeoTest richTest;
   richTest.CreateStudyReport("RICH geometry test", results, names, outputDir);
}
