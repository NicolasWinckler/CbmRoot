/**
 * \file CbmSimulationReport.cxx
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmSimulationReport.h"
#include "CbmReportElement.h"
#include "CbmHistManager.h"
#include "TFile.h"

#include <iostream>
#include <fstream>
using std::cout;
using std::ofstream;

CbmSimulationReport::CbmSimulationReport():
    CbmReport(),
	fHM(),
	fResultDirectory("")
{

}

CbmSimulationReport::~CbmSimulationReport()
{

}

//bool CbmSimulationReport::PropertyExists(
//      const std::string& name) const
//{
//   return (fQa.count(name) != 0) ? true : false;
//}

//void CbmSimulationReport::Create(
//      ReportType reportType,
//      ostream& out,
//      const string& resultDirectory)
//{
//   CreateReportElement(reportType);
//   fResultDirectory = resultDirectory;
//
//   try {
//      read_json(resultDirectory + GetQaFileName(), fQa);
//   } catch (json_parser_error& error) {
//      cout << error.what();
//   }
//
//   try {
//      read_json(resultDirectory + GetCheckFileName(), fCheck);
//   } catch (json_parser_error& error) {
//      cout << error.what();
//   }
//
//   try {
//      read_json(GetIdealFileName(), fIdeal);
//   } catch (json_parser_error& error) {
//      cout << error.what();
//   }
//
//   // User has to implement this function!
//   Create(out);
//
//   fQa.clear();
//   fCheck.clear();
//   fIdeal.clear();
//
//   DeleteReportElement();
//}

//string CbmSimulationReport::PrintImages(
//		const string& pattern) const
//{
//	string str = "";
//	vector<string> images = GetImages(fResultDirectory, pattern);
//	vector<string>::const_iterator it;
//	for(it = images.begin(); it != images.end(); it++) {
//		str += fR->Image(*it, *it);
//	}
//	return str;
//}

void CbmSimulationReport::Create(
      CbmHistManager* histManager,
      const string& resultDirectory)
{
   assert(histManager != NULL);
   fHM = histManager;
   fResultDirectory = resultDirectory;

   ofstream out(string(fResultDirectory + "/" + fName + ".html").c_str());
   CreateReportElement(kHtmlReport);
   Create(out); // User has to implement this function!
   DeleteReportElement();
}

void CbmSimulationReport::Create(
      const string& fileName,
      const string& resultDirectory)
{
   fHM = new CbmHistManager();
   TFile* file = new TFile(fileName.c_str());
   fHM->ReadFromFile(file);
   fResultDirectory = resultDirectory;

   ofstream out(string(fResultDirectory + "/" + fName + ".html").c_str());
   CreateReportElement(kHtmlReport);
   Create(out); // User has to implement this function!
   DeleteReportElement();

   delete fHM;
   delete file;
}
