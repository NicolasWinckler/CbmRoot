/**
 * \file CbmLitClusteringQaReport.h
 * \brief Simulation report for clustering QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitClusteringQaReport.h"
#include "../report/CbmLitReportElement.h"
#include "../../../std/utils/CbmLitUtils.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/assign/list_of.hpp>
using std::endl;
using boost::assign::list_of;
using lit::ToString;

CbmLitClusteringQaReport::CbmLitClusteringQaReport(
      LitReportType reportType) : CbmLitSimulationReport(reportType)
{

}

CbmLitClusteringQaReport::~CbmLitClusteringQaReport()
{

}

void CbmLitClusteringQaReport::Create(
   ostream& out,
   boost::property_tree::ptree* qa,
   boost::property_tree::ptree* ideal,
   boost::property_tree::ptree* check)
{
   fQa = qa;
   fIdeal = ideal;
   fCheck = check;

   out.precision(3);
   out << fR->DocumentBegin();

  // out << "Number of events: " << PrintValue("hEventNo") << endl;

   // Number of objects table
   out << fR->TableBegin("Number of objects statistics", list_of("")("MVD")("STS")("RICH")("TRD")("MUCH pix")("MUCH st")("TOF"));
   out << PrintNofStatistics("Points", "hNofMvdPoints", "hNofStsPoints", "hNofRichPoints",
         "hNofTrdPoints", "hNofMuchPoints", "hNofMuchPoints", "hNofTofPoints");
   out << PrintNofStatistics("Digis", "hNofMvdDigis", "hNofStsDigis", "",
         "hNofTrdDigis", "hNofMuchDigis", "", "");
   out << PrintNofStatistics("Clusters", "hNofMvdClusters", "hNofStsClusters", "",
         "hNofTrdClusters", "hNofMuchClusters", "", "");
   out << PrintNofStatistics("Hits","hNofMvdHits", "hNofStsHits", "hNofRichHits",
         "hNofTrdHits", "hNofMuchPixelHits", "hNofMuchStrawHits", "hNofTofHits");
   out << fR->TableEnd();

   // Images
   out << fR->Image("MVD hits per station", "clustering_qa_mvd_hits_station");
   out << fR->Image("STS hits per station", "clustering_qa_sts_hits_station");
   out << fR->Image("TRD hits per station", "clustering_qa_trd_hits_station");
   out << fR->Image("MUCH hits per station", "clustering_qa_much_hits_station");
   out << fR->Image("TOF hits per station", "clustering_qa_tof_hits_station");

   out <<  fR->DocumentEnd();
}

string CbmLitClusteringQaReport::PrintNofStatistics(
        const string& name,
        const string& mvd,
        const string& sts,
        const string& rich,
        const string& trd,
        const string& muchP,
        const string& muchS,
        const string& tof)
{
   string st1 = (mvd != "" && PropertyExists(mvd)) ? ToString<Int_t>(fQa->get(mvd, -1.)) : "-";
   string st2 = (sts != "" && PropertyExists(sts)) ? ToString<Int_t>(fQa->get(sts, -1.)) : "-";
   string st3 = (rich != "" && PropertyExists(rich)) ? ToString<Int_t>(fQa->get(rich, -1.)) : "-";
   string st4 = (trd != "" && PropertyExists(trd)) ? ToString<Int_t>(fQa->get(trd, -1.)) : "-";
   string st5 = (muchP != "" && PropertyExists(muchP)) ? ToString<Int_t>(fQa->get(muchP, -1.)) : "-";
   string st6 = (muchS != "" && PropertyExists(muchS)) ? ToString<Int_t>(fQa->get(muchS, -1.)) : "-";
   string st7 = (tof!= "" && PropertyExists(tof)) ? ToString<Int_t>(fQa->get(tof, -1.)) : "-";
   return fR->TableRow(list_of(name)(st1)(st2)(st3)(st4)(st5)(st6)(st7));
}
