/**
 * \file CbmMuchClusteringQaReport.h
 * \brief Simulation report for clustering QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmMuchClusteringQaReport.h"
#include "CbmReportElement.h"
#include "CbmHistManager.h"
#include "../../../std/utils/CbmLitUtils.h"
#include "TH1.h"
#include <boost/assign/list_of.hpp>
#include <vector>
using std::vector;
using std::endl;
using boost::assign::list_of;
using lit::NumberToString;
using lit::Split;

CbmMuchClusteringQaReport::CbmMuchClusteringQaReport():
		CbmSimulationReport()
{
	SetReportName("clustering_qa");
}

CbmMuchClusteringQaReport::~CbmMuchClusteringQaReport()
{

}

void CbmMuchClusteringQaReport::Create()
{
   Out().precision(3);
   Out() << R()->DocumentBegin();
   Out() << R()->Title(0, GetTitle());

   Out() << "Number of events: " << HM()->H1("hen_EventNo_TrackingQa")->GetEntries() << endl;

   Out() << PrintNofObjects();
  // Out() << PrintImages(".*clustering_qa_.*png");
   Out() <<  R()->DocumentEnd();
}

string CbmMuchClusteringQaReport::PrintNofObjects() const
{
	vector<TH1*> histos = HM()->H1Vector("hno_NofObjects_.+");
	Int_t nofHistos = histos.size();
   	string str = R()->TableBegin("Number of objects per event", list_of("Name")("Value"));
	for (Int_t iHist = 0; iHist < nofHistos; iHist++) {
		string cellName = Split(histos[iHist]->GetName(), '_')[2];
		str += R()->TableRow(list_of(cellName)(NumberToString<Int_t>(histos[iHist]->GetMean())));
	}
   	str += R()->TableEnd();
   	return str;
}

