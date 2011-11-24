/**
 * \file CbmLitQaBaseReportStudy.cxx
 *
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitQaBaseReportStudy.h"
#include "../std/utils/CbmLitUtils.h"

#include "TSystem.h"

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

CbmLitQaBaseReportStudy::CbmLitQaBaseReportStudy():
   fIsElectronSetup(true),
   fIsMvd(true),
   fIsSts(true),
   fIsRich(true),
   fIsTrd(true),
   fIsMuch(true),
   fIsTof(true)
{

}

CbmLitQaBaseReportStudy::~CbmLitQaBaseReportStudy()
{

}

void CbmLitQaBaseReportStudy::SetDetectorPresence(
         DetectorId detId,
         bool isDet)
{
   switch(detId) {
      case kMVD: fIsMvd = isDet; break;
      case kSTS: fIsSts = isDet; break;
      case kRICH: fIsRich = isDet; break;
      case kTRD: fIsTrd = isDet; break;
      case kMUCH: fIsMuch = isDet; break;
      case kTOF: fIsTof = isDet; break;
      default: break;
   }
}

void CbmLitQaBaseReportStudy::CreateReport(
      ostream& out,
      const string& title,
      const vector<string>& results,
      const vector<string>& names)
{
   fTitle = title;
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

   fR->SetPtreeQa(qa);
   fR->SetPtreeIdeal(&ideal);
   fR->SetPtreeCheck(check);
   fR->SetStudyNames(names);
   fR->SetStudyResults(results);

   fR->SetErrorColor("#FF0000");
   fR->SetWarningColor("#FFFF00");
   fR->SetNormalColor("#00FF00");
   fR->SetIsUseChecking(true);

   Create(out);
}
