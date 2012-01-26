/**
 * \file CbmLitClusteringQaStudyReport.cxx
 * \brief Study summary report for clustering QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitClusteringQaStudyReport.h"
#include "../report/CbmLitReportElement.h"
#include "../std/utils/CbmLitUtils.h"

#include "TSystem.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/assign/list_of.hpp>
using boost::assign::list_of;
using boost::property_tree::json_parser_error;
using lit::ToString;

CbmLitClusteringQaStudyReport::CbmLitClusteringQaStudyReport()
{

}

CbmLitClusteringQaStudyReport::~CbmLitClusteringQaStudyReport()
{

}

void CbmLitClusteringQaStudyReport::Create(
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
         read_json(resultDirectories[iStudy] + "/clustering_qa.json", fQa[iStudy]);
         read_json(resultDirectories[iStudy] + "/clustering_qa_check.json", fCheck[iStudy]);
      } catch (json_parser_error error) {
         cout << error.what();
      }
   }
   string idealFile = string(gSystem->Getenv("VMCWORKDIR")) + ("/littrack/cbm/qa/clustering/clustering_qa_ideal.json");
   try {
      read_json(idealFile.c_str(), fIdeal);
   } catch (json_parser_error error) {
      cout << error.what();
   }

   Create(out);

   fResultDirectories.clear();
   fStudyNames.clear();
   fQa.clear();
   fCheck.clear();
   fIdeal.clear();

   DeleteReportElement();
}

void CbmLitClusteringQaStudyReport::Create(
      ostream& out)
{
   out.precision(3);
   out << fR->DocumentBegin();
   out << fR->Title(0, fTitle);

   out << fR->TableBegin("Number of objects", list_of(string("")).range(fStudyNames));

   if (PropertyExists("hNofMvdPoints")) out << PrintRow("hNofMvdPoints", "MVD points");
   if (PropertyExists("hNofMvdDigis")) out << PrintRow("hNofMvdDigis", "MVD digis");
   if (PropertyExists("hNofMvdClusters")) out << PrintRow("hNofMvdClusters", "MVD clusters");
   if (PropertyExists("hNofMvdHits")) out << PrintRow("hNofMvdHits", "MVD hits");

   if (PropertyExists("hNofStsPoints")) out << PrintRow("hNofStsPoints", "STS points");
   if (PropertyExists("hNofStsDigis")) out << PrintRow("hNofStsDigis", "STS digis");
   if (PropertyExists("hNofStsClusters")) out << PrintRow("hNofStsClusters", "STS clusters");
   if (PropertyExists("hNofStsHits")) out << PrintRow("hNofStsHits", "STS hits");

   if (PropertyExists("hNofRichPoints")) out << PrintRow("hNofRichPoints", "RICH points");
   if (PropertyExists("hNofRichHits")) out << PrintRow("hNofRichHits", "RICH hits");

   if (PropertyExists("hNofTrdPoints")) out << PrintRow("hNofTrdPoints", "TRD points");
   if (PropertyExists("hNofTrdDigis")) out << PrintRow("hNofTrdDigis", "TRD digis");
   if (PropertyExists("hNofTrdClusters")) out << PrintRow("hNofTrdClusters", "TRD clusters");
   if (PropertyExists("hNofTrdHits")) out << PrintRow("hNofTrdHits", "TRD hits");

   if (PropertyExists("hNofMuchPoints")) out << PrintRow("hNofMuchPoints", "MUCH points");
   if (PropertyExists("hNofMuchDigis")) out << PrintRow("hNofMuchDigis", "MUCH digis");
   if (PropertyExists("hNofMuchClusters")) out << PrintRow("hNofMuchClusters", "MUCH clusters");
   if (PropertyExists("hNofMuchPixelHits")) out << PrintRow("hNofMuchPixelHits", "MUCH pixel hits");
   if (PropertyExists("hNofMuchStrawHits")) out << PrintRow("hNofMuchStrawHits", "MUCH straw hits");

   if (PropertyExists("hNofTofPoints")) out << PrintRow("hNofTofPoints", "TOF points");
   if (PropertyExists("hNofTofHits")) out << PrintRow("hNofTofHits", "TOF hits");

   out << fR->TableEnd();
   out <<  fR->DocumentEnd();
}

string CbmLitClusteringQaStudyReport::PrintRow(
      const string& property,
      const string& name)
{
   vector<string> row;
   row.push_back(name);
   for (int iStudyId = 0; iStudyId < fStudyNames.size(); iStudyId++) {
      row.push_back(ToString<float>(fQa[iStudyId].get(property, -1.)));
   }
   return fR->TableRow(row);
}
