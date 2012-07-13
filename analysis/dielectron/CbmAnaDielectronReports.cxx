/**
 * \file CbmAnaDielectronReports.cxx
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2012
 */
#include "CbmAnaDielectronReports.h"
#include "CbmAnaDielectronStudyReport.h"
#include "CbmReport.h"
#include "CbmStudyReport.h"

#include "TSystem.h"

#include <fstream>

using namespace std;

CbmAnaDielectronReports::CbmAnaDielectronReports()
{
}

CbmAnaDielectronReports::~CbmAnaDielectronReports()
{
}

void CbmAnaDielectronReports::CreateStudyReport(
      const string& title,
      const vector<string>& resultDirectories,
      const vector<string>& studyNames,
      const string& outputDir)
{
   if (outputDir != "") gSystem->mkdir(outputDir.c_str(), true);

   CbmStudyReport* report = new CbmAnaDielectronStudyReport();
   report->SetTitle(title);
   cout << "Report can be found here: " << outputDir << endl;
   ofstream foutHtml(string(outputDir + "lmvm_study.html").c_str());
   ofstream foutLatex(string(outputDir + "lmvm_study.tex").c_str());
   ofstream foutText(string(outputDir + "lmvm_study.txt").c_str());
   report->Create(kHtmlReport, foutHtml, resultDirectories, studyNames);
   report->Create(kLatexReport, foutLatex, resultDirectories, studyNames);
   report->Create(kTextReport, foutText, resultDirectories, studyNames);
   delete report;
}

ClassImp(CbmAnaDielectronReports);
