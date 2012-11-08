/**
 * \file CbmSimulationReport.cxx
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmSimulationReport.h"
#include "CbmReportElement.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <iostream>
using boost::property_tree::json_parser_error;
using std::cout;

CbmSimulationReport::CbmSimulationReport():
	fQa(),
	fIdeal(),
	fCheck(),
	fResultDirectory("")
{

}

CbmSimulationReport::~CbmSimulationReport()
{

}

bool CbmSimulationReport::PropertyExists(
      const std::string& name) const
{
   return (fQa.count(name) != 0) ? true : false;
}

void CbmSimulationReport::Create(
      ReportType reportType,
      ostream& out,
      const string& resultDirectory)
{
   CreateReportElement(reportType);
   fResultDirectory = resultDirectory;

   try {
      read_json(resultDirectory + GetQaFileName(), fQa);
   } catch (json_parser_error& error) {
      cout << error.what();
   }

   try {
      read_json(resultDirectory + GetCheckFileName(), fCheck);
   } catch (json_parser_error& error) {
      cout << error.what();
   }

   try {
      read_json(GetIdealFileName(), fIdeal);
   } catch (json_parser_error& error) {
      cout << error.what();
   }

   // User has to implement this function!
   Create(out);

   fQa.clear();
   fCheck.clear();
   fIdeal.clear();

   DeleteReportElement();
}

string CbmSimulationReport::PrintImages(
		const string& pattern) const
{
	string str = "";
	vector<string> images = GetImages(fResultDirectory, pattern);
	vector<string>::const_iterator it;
	for(it = images.begin(); it != images.end(); it++) {
		str += fR->Image(*it, *it);
	}
	return str;
}
