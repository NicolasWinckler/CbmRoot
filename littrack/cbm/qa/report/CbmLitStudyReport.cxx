/**
 * \file CbmLitStudyReport.cxx
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitStudyReport.h"
#include "../std/utils/CbmLitUtils.h"

#include "TSystem.h"

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

CbmLitStudyReport::CbmLitStudyReport(LitReportType reportType):
   CbmLitReport(reportType)
{

}

CbmLitStudyReport::~CbmLitStudyReport()
{

}

void CbmLitStudyReport::CreateReport(
      ostream& out,
      const string& title,
      const vector<string>& results,
      const vector<string>& names)
{
   SetTitle(title);
   vector<boost::property_tree::ptree> qa;
   vector<boost::property_tree::ptree> check;
   qa.resize(results.size());
   check.resize(results.size());

   boost::property_tree::ptree ideal;
   string qaIdealFile = string(gSystem->Getenv("VMCWORKDIR")) + ("/littrack/cbm/qa/rec_qa_ideal.json");
   read_json(qaIdealFile.c_str(), ideal);

   for (int i = 0; i < results.size(); i++) {
      read_json(results[i] + "/rec_qa.json", qa[i]);
      read_json(results[i] + "/rec_qa_check.json", check[i]);
   }

   SetPtreeQa(qa);
   SetPtreeIdeal(&ideal);
   SetPtreeCheck(check);
   SetStudyNames(names);
   SetStudyResults(results);

   SetErrorColor("#FF0000");
   SetWarningColor("#FFFF00");
   SetNormalColor("#00FF00");
   SetIsUseChecking(true);

   Create(out);
}
