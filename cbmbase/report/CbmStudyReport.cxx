/**
 * \file CbmStudyReport.cxx
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmStudyReport.h"
#include "CbmReportElement.h"
#include <boost/property_tree/json_parser.hpp>
#include <boost/assign/list_of.hpp>
#include <cassert>
#include <iostream>
#include <sstream>
using boost::property_tree::json_parser_error;
using boost::assign::list_of;
using std::cout;
using std::stringstream;

CbmStudyReport::CbmStudyReport()
{

}

CbmStudyReport::~CbmStudyReport()
{

}

bool CbmStudyReport::PropertyExists(
      const std::string& name) const
{
   assert(fQa.size() != 0);
   return (fQa[0].count(name) != 0) ? true : false;
}

void CbmStudyReport::Create(
      ReportType reportType,
      ostream& out,
      const vector<string>& resultDirectories,
      const vector<string>& studyNames)
{
   CreateReportElement(reportType);

   Int_t nofStudies = resultDirectories.size();

   fResultDirectories = resultDirectories;
   fStudyNames = studyNames;

   fQa.resize(nofStudies);
   fCheck.resize(nofStudies);
   for (Int_t iStudy = 0; iStudy < nofStudies; iStudy++) {
      try {
         read_json(resultDirectories[iStudy] + GetQaFileName(), fQa[iStudy]);
         read_json(resultDirectories[iStudy] + GetCheckFileName(), fCheck[iStudy]);
      } catch (json_parser_error& error) {
         cout << error.what();
      }
   }

   try {
      read_json(GetIdealFileName(), fIdeal);
   } catch (json_parser_error& error) {
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

string CbmStudyReport::PrintImageTable(
      const string& title,
      const string& file) const
{
   int nofStudies = fStudyNames.size();
   int nofCols = 2;
   int nofLoops = ((nofStudies % nofCols) == 0) ? nofStudies / nofCols : nofStudies / nofCols + 1;
   stringstream ss;
   ss << fR->TableBegin(title, list_of(string("")).repeat(nofCols - 1, string("")));
   for (int iL = 0; iL < nofLoops; iL++) {
      std::vector<string> images;
      for (int iC = 0; iC < nofCols; iC++) {
         int index = nofCols * iL + iC;
         if (index >= nofStudies) continue;
         string fileName = fResultDirectories[index] + "/" + file;
         images.push_back(fR->Image(fStudyNames[index], fileName));
      }
      ss << fR->TableRow(images);
   }
   ss << fR->TableEnd();
   return ss.str();
}

string CbmStudyReport::PrintImages(
		const string& pattern) const
{
	string str = "";
	vector<string> images = GetImages(fResultDirectories[0], pattern);
	vector<string>::const_iterator it;
	for(it = images.begin(); it != images.end(); it++) {
		str += PrintImageTable(*it, *it);
	}
	return str;
}
