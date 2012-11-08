/**
 * \file CbmStudyReport.cxx
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmStudyReport.h"
#include "CbmReportElement.h"
#include "CbmHistManager.h"
#include <cassert>
#include <iostream>
#include <fstream>
#include <vector>
#include "TFile.h"

using std::ofstream;
using std::vector;

CbmStudyReport::CbmStudyReport()
 : CbmReport(),
   fHM(),
   fResultDirectory(),
   fStudyNames()
{

}

CbmStudyReport::~CbmStudyReport()
{

}
//
//bool CbmStudyReport::PropertyExists(
//      const std::string& name) const
//{
//   assert(fQa.size() != 0);
//   return (fQa[0].count(name) != 0) ? true : false;
//}

//void CbmStudyReport::Create(
//      ReportType reportType,
//      ostream& out,
//      const vector<string>& resultDirectories,
//      const vector<string>& studyNames)
//{
//   CreateReportElement(reportType);
//
//   Int_t nofStudies = resultDirectories.size();
//
//   fResultDirectories = resultDirectories;
//   fStudyNames = studyNames;
//
//   fQa.resize(nofStudies);
//   fCheck.resize(nofStudies);
//   for (Int_t iStudy = 0; iStudy < nofStudies; iStudy++) {
//      try {
//         read_json(resultDirectories[iStudy] + GetQaFileName(), fQa[iStudy]);
//         read_json(resultDirectories[iStudy] + GetCheckFileName(), fCheck[iStudy]);
//      } catch (json_parser_error& error) {
//         cout << error.what();
//      }
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
//   fResultDirectories.clear();
//   fStudyNames.clear();
//   fQa.clear();
//   fCheck.clear();
//   fIdeal.clear();
//
//   DeleteReportElement();
//}

//string CbmStudyReport::PrintImageTable(
//      const string& title,
//      const string& file) const
//{
//   int nofStudies = fStudyNames.size();
//   int nofCols = 2;
//   int nofLoops = ((nofStudies % nofCols) == 0) ? nofStudies / nofCols : nofStudies / nofCols + 1;
//   stringstream ss;
//   ss << fR->TableBegin(title, list_of(string("")).repeat(nofCols - 1, string("")));
//   for (int iL = 0; iL < nofLoops; iL++) {
//      std::vector<string> images;
//      for (int iC = 0; iC < nofCols; iC++) {
//         int index = nofCols * iL + iC;
//         if (index >= nofStudies) continue;
//         string fileName = fResultDirectories[index] + "/" + file;
//         images.push_back(fR->Image(fStudyNames[index], fileName));
//      }
//      ss << fR->TableRow(images);
//   }
//   ss << fR->TableEnd();
//   return ss.str();
//}
//
//string CbmStudyReport::PrintImages(
//		const string& pattern) const
//{
//	string str = "";
//	vector<string> images = GetImages(fResultDirectories[0], pattern);
//	vector<string>::const_iterator it;
//	for(it = images.begin(); it != images.end(); it++) {
//		str += PrintImageTable(*it, *it);
//	}
//	return str;
//}

void CbmStudyReport::Create(
      const vector<CbmHistManager*>& histManagers,
      const vector<string>& studyNames,
      const string& resultDirectory)
{
   assert(histManagers.size() == studyNames.size());
   fHM = histManagers;
   fStudyNames = studyNames;
   fResultDirectory = resultDirectory;

   ofstream out(string(fResultDirectory + "/" + fName + ".html").c_str());
   CreateReportElement(kHtmlReport);
   Create(out); // User has to implement this function!
   DeleteReportElement();
}

void CbmStudyReport::Create(
      const vector<string>& fileNames,
      const vector<string>& studyNames,
      const string& resultDirectory)
{
	assert(fileNames.size() == studyNames.size());
	Int_t nofStudies = fileNames.size();
	vector<TFile*> files(nofStudies);
	fHM.resize(nofStudies);
	for (Int_t i = 0; i < nofStudies; i++) {
		fHM[i] = new CbmHistManager();
		files[i] = new TFile(fileNames[i].c_str());
		fHM[i]->ReadFromFile(files[i]);
	}
	fResultDirectory = resultDirectory;

	ofstream out(string(fResultDirectory + "/" + fName + ".html").c_str());
	CreateReportElement(kHtmlReport);
	Create(out); // User has to implement this function!
	DeleteReportElement();

	for (Int_t i = 0; i < nofStudies; i++) {
		delete fHM[i];
		delete files[i];
	}
	fHM.clear();
	files.clear();
}
