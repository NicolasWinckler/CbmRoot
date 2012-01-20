/**
 * \file CbmLitClusteringQaStudyReport.cxx
 * \brief Study summary report for clustering QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitClusteringQaStudyReport.h"
#include "../report/CbmLitReportElement.h"
#include "../std/utils/CbmLitUtils.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/assign/list_of.hpp>
using boost::assign::list_of;
using lit::ToString;

CbmLitClusteringQaStudyReport::CbmLitClusteringQaStudyReport(
      LitReportType reportType) : CbmLitStudyReport(reportType)
{

}

CbmLitClusteringQaStudyReport::~CbmLitClusteringQaStudyReport()
{

}

void CbmLitClusteringQaStudyReport::Create(
      ostream& out,
      const vector<string>& studyNames,
      const vector<boost::property_tree::ptree*>& qa,
      boost::property_tree::ptree* ideal,
      const vector<boost::property_tree::ptree*>& check)
{
   fStudyNames = studyNames;
   fQa = qa;
   fIdeal = ideal;
   fCheck = check;

   out.precision(3);
   out << fR->DocumentBegin();
   out << (fTitle != "") ? fR->Title(0, fTitle) : string("");

   out << fR->TableBegin("Number of objects", list_of(string("")).range(fStudyNames));
//   out << fR->TableRow(list_of("hEventNo")("Number of events"));

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
      row.push_back(ToString<float>(fQa[iStudyId]->get(property, -1.)));
   }
   return fR->TableRow(row);
}
