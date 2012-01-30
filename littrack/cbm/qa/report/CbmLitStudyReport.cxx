/**
 * \file CbmLitStudyReport.cxx
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitStudyReport.h"
#include <boost/property_tree/json_parser.hpp>
#include <cassert>
#include <iostream>
using boost::property_tree::json_parser_error;
using std::cout;

CbmLitStudyReport::CbmLitStudyReport()
{

}

CbmLitStudyReport::~CbmLitStudyReport()
{

}

bool CbmLitStudyReport::PropertyExists(
      const std::string& name) const
{
   assert(fQa.size() != 0);
   return (fQa[0].count(name) != 0) ? true : false;
}

void CbmLitStudyReport::Create(
      LitReportType reportType,
      ostream& out,
      const vector<string>& resultDirectories,
      const vector<string>& studyNames)
{
   CreateReportElement(reportType);

   int nofStudies = resultDirectories.size();

   fResultDirectories = resultDirectories;
   fStudyNames = studyNames;

   fQa.resize(nofStudies);
   fCheck.resize(nofStudies);
   for(int iStudy = 0; iStudy < nofStudies; iStudy++) {
      try {
         read_json(resultDirectories[iStudy] + GetQaFileName(), fQa[iStudy]);
         read_json(resultDirectories[iStudy] + GetCheckFileName(), fCheck[iStudy]);
      } catch (json_parser_error error) {
         cout << error.what();
      }
   }

   try {
      read_json(GetIdealFileName(), fIdeal);
   } catch (json_parser_error error) {
      cout << error.what();
   }

   // User has to implement this function!
   Create(out);

   fResultDirectories.clear();
   fStudyNames.clear();
   fQa.clear();
   fCheck.clear();
   fIdeal.clear();

   DeleteReportElement();
}
