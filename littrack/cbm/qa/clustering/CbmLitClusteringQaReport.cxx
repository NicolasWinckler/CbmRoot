/**
 * \file CbmLitClusteringQaReport.h
 * \brief Simulation report for clustering QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitClusteringQaReport.h"
#include "CbmReportElement.h"
#include "CbmHistManager.h"
#include "CbmDrawHist.h"
#include "../../../std/utils/CbmLitUtils.h"
#include "TH1.h"
#include "TCanvas.h"
#include <boost/assign/list_of.hpp>
#include <vector>
using std::vector;
using std::endl;
using boost::assign::list_of;
using lit::NumberToString;
using lit::Split;

CbmLitClusteringQaReport::CbmLitClusteringQaReport():
		CbmSimulationReport()
{
	SetReportName("clustering_qa");
}

CbmLitClusteringQaReport::~CbmLitClusteringQaReport()
{

}

void CbmLitClusteringQaReport::Create()
{
   Out().precision(3);
   Out() << R()->DocumentBegin();
   Out() << R()->Title(0, GetTitle());

   Out() << "Number of events: " << HM()->H1("hen_EventNo_ClusteringQa")->GetEntries() << endl;

   Out() << PrintNofObjects();
   PrintCanvases();
   Out() <<  R()->DocumentEnd();
}

string CbmLitClusteringQaReport::PrintNofObjects() const
{
	vector<TH1*> histos = HM()->H1Vector("hno_NofObjects_.+_Event");
	Int_t nofHistos = histos.size();
   string str = R()->TableBegin("Number of objects per event", list_of("Name")("Value"));
	for (Int_t iHist = 0; iHist < nofHistos; iHist++) {
		string cellName = Split(histos[iHist]->GetName(), '_')[2];
		str += R()->TableRow(list_of(cellName)(NumberToString<Int_t>(histos[iHist]->GetMean())));
	}
   str += R()->TableEnd();
   return str;
}

void CbmLitClusteringQaReport::Draw()
{
   Int_t nofEvents = HM()->H1("hen_EventNo_ClusteringQa")->GetEntries();
   HM()->ScaleByPattern("hno_NofObjects_.*_Station", 1. / nofEvents);

	DrawHistogramsByPattern("hno_NofObjects_.*_Station");
}

void CbmLitClusteringQaReport::DrawHistogramsByPattern(
      const string& histNamePattern)
{
	vector<TH1*> histos = HM()->H1Vector(histNamePattern);
	if (histos.size() == 0) return;
	Int_t nofHistos = histos.size();
	for (UInt_t iHist = 0; iHist < nofHistos; iHist++) {
		TH1* hist = histos[iHist];
		string canvasName = GetReportName() + hist->GetName();
		TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 800, 500);
		DrawH1(hist, kLinear, kLinear);
	}
}

ClassImp(CbmLitClusteringQaReport)
