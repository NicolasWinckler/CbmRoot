/**
 * \file CbmLitSimulationReport.cxx
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitSimulationReport.h"
#include "CbmLitReportElement.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <iostream>
using boost::property_tree::json_parser_error;
using std::cout;

CbmLitSimulationReport::CbmLitSimulationReport():
	fQa(),
	fIdeal(),
	fCheck(),
	fResultDirectory("")
{

}

CbmLitSimulationReport::~CbmLitSimulationReport()
{

}

bool CbmLitSimulationReport::PropertyExists(
      const std::string& name) const
{
   return (fQa.count(name) != 0) ? true : false;
}

void CbmLitSimulationReport::Create(
      LitReportType reportType,
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

string CbmLitSimulationReport::PrintImages(
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
