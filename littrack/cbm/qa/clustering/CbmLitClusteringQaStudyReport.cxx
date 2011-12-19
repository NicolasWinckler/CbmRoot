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

   if (fIsMvd) {
      out << PrintRow("hNofMvdPoints", "MVD points");
      out << PrintRow("hNofMvdDigis", "MVD digis");
      out << PrintRow("hNofMvdClusters", "MVD clusters");
      out << PrintRow("hNofMvdHits", "MVD hits");
   }

   if (fIsSts) {
      out << PrintRow("hNofStsPoints", "STS points");
      out << PrintRow("hNofStsDigis", "STS digis");
      out << PrintRow("hNofStsClusters", "STS clusters");
      out << PrintRow("hNofStsHits", "STS hits");
   }

   if (fIsRich) {
      out << PrintRow("hNofRichPoints", "RICH points");
      out << PrintRow("hNofRichHits", "RICH hits");
   }

   if (fIsTrd) {
      out << PrintRow("hNofTrdPoints", "TRD points");
      out << PrintRow("hNofTrdDigis", "TRD digis");
      out << PrintRow("hNofTrdClusters", "TRD clusters");
      out << PrintRow("hNofTrdHits", "TRD hits");
   }

   if (fIsMuch) {
      out << PrintRow("hNofMuchPoints", "MUCH points");
      out << PrintRow("hNofMuchDigis", "MUCH digis");
      out << PrintRow("hNofMuchClusters", "MUCH clusters");
      out << PrintRow("hNofMuchPixelHits", "MUCH pixel hits");
      out << PrintRow("hNofMuchStrawHits", "MUCH straw hits");
   }

   if (fIsTof) {
      out << PrintRow("hNofTofPoints", "TOF points");
      out << PrintRow("hNofTofHits", "TOF hits");
   }
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
