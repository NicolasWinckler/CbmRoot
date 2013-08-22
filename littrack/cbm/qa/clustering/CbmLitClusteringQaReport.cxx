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
#include "TF1.h"
#include "TProfile.h"
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
   if(HM()->Exists("hno_NofObjects_MuchHits_Station")){
      Out() << PrintMuchAcuracy();
   }
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

string CbmLitClusteringQaReport::PrintMuchAcuracy() const
{
   Float_t totalMuonEff = 0;
   Float_t nofMuonPoints = 0;
   Float_t nofMuonHits = 0;
   Float_t totalEff = 0;
   Float_t nofPoints = 0;
   Float_t nofHits = 0;
   string str = R()->TableBegin("Efficiency of clustering", list_of("Layer")("Muon Points")("Muon Hits")("Muon Efficiency (%)")
         ("Total Points")("Total Hits")("Total Efficiency (%)"));
   for (Int_t i = 1; i <= HM()->H2("hsh_Much_Muon_Points")->GetXaxis()->GetNbins(); i++){
      Float_t muonEff = 0;
      if(HM()->H1("hsh_Much_Muon_Points")->GetBinContent(i) != 0){
         muonEff = (100. * (Float_t)(HM()->H1("hsh_Much_Muon_PixelHits")->GetBinContent(i))) /
               (Float_t)(HM()->H1("hsh_Much_Muon_Points")->GetBinContent(i));
      }
      Float_t eff = 0;
      if(HM()->H1("hsh_Much_Points")->GetBinContent(i) != 0){
         eff = (100. * (Float_t)(HM()->H1("hsh_Much_True_PixelHits")->GetBinContent(i))) /
               (Float_t)(HM()->H1("hsh_Much_Points")->GetBinContent(i));
      }
      nofMuonPoints += HM()->H1("hsh_Much_Muon_Points")->GetBinContent(i);
      nofPoints += HM()->H1("hsh_Much_Points")->GetBinContent(i);
      nofMuonHits += HM()->H1("hsh_Much_Muon_PixelHits")->GetBinContent(i);
      nofHits += HM()->H1("hsh_Much_True_PixelHits")->GetBinContent(i);
      str += R()->TableRow(list_of(NumberToString<Int_t>(i))(NumberToString<Float_t>(HM()->H1("hsh_Much_Muon_Points")->GetBinContent(i)))
            (NumberToString<Float_t>(HM()->H1("hsh_Much_Muon_PixelHits")->GetBinContent(i)))(NumberToString<Float_t>(muonEff))
            (NumberToString<Float_t>(HM()->H1("hsh_Much_Points")->GetBinContent(i)))
            (NumberToString<Float_t>(HM()->H1("hsh_Much_True_PixelHits")->GetBinContent(i)))
            (NumberToString<Float_t>(eff)));
   }
   if(nofMuonPoints != 0){
      totalMuonEff = (100. * nofMuonHits) / nofMuonPoints;
   }
   if(nofPoints != 0){
      totalEff = (100. * nofHits) / nofPoints;
   }
   std::stringstream stream1;
   stream1 << "Total";
   str += R()->TableRow(list_of(stream1.str())(NumberToString<Float_t>(nofMuonPoints))
         (NumberToString<Float_t>(nofMuonHits))(NumberToString<Float_t>(totalMuonEff))
         (NumberToString<Float_t>(nofPoints))(NumberToString<Float_t>(nofHits))
         (NumberToString<Float_t>(totalEff)));
   str += R()->TableEnd();
   //---
   str += R()->TableBegin("Much Hit Residual", list_of("Layer")("Mean X")("Mean Y")("RMS X")("RMS Y")("Sigma X")("Sigma Y"));
   for (Int_t i = 1; i <= HM()->H2("hr_Much_ResidualX_2D")->GetXaxis()->GetNbins(); i++){
      str += R()->TableRow(list_of(NumberToString<Int_t>(i))(NumberToString<Float_t>(HM()->H1("h_Much_Mean_X")->GetBinContent(i)))
            (NumberToString<Float_t>(HM()->H1("h_Much_Mean_Y")->GetBinContent(i)))(NumberToString<Float_t>(HM()->H1("h_Much_RMS_X")->GetBinContent(i)))
            (NumberToString<Float_t>(HM()->H1("h_Much_RMS_Y")->GetBinContent(i)))(NumberToString<Float_t>(HM()->H1("h_Much_Sigma_X")->GetBinContent(i)))
            (NumberToString<Float_t>(HM()->H1("h_Much_Sigma_Y")->GetBinContent(i))));
   }
   str += R()->TableEnd();
   str += R()->TableBegin("Much Hit Pull", list_of("Layer")("Mean X")("Mean Y")("RMS X")("RMS Y")("Sigma X")("Sigma Y"));
   for (Int_t i = 1; i <= HM()->H2("hr_Much_PullX_2D")->GetXaxis()->GetNbins(); i++){
      str += R()->TableRow(list_of(NumberToString<Int_t>(i))(NumberToString<Float_t>(HM()->H1("h_Much_PullMean_X")->GetBinContent(i)))
            (NumberToString<Float_t>(HM()->H1("h_Much_PullMean_Y")->GetBinContent(i)))(NumberToString<Float_t>(HM()->H1("h_Much_PullRMS_X")->GetBinContent(i)))
            (NumberToString<Float_t>(HM()->H1("h_Much_PullRMS_Y")->GetBinContent(i)))(NumberToString<Float_t>(HM()->H1("h_Much_PullSigma_X")->GetBinContent(i)))
            (NumberToString<Float_t>(HM()->H1("h_Much_PullSigma_Y")->GetBinContent(i))));
   }
   str += R()->TableEnd();
   str += R()->TableEnd();
   return str;
}

void CbmLitClusteringQaReport::Draw()
{
   ScaleHistogramsByNofEvents();

   if(HM()->Exists("hno_NofObjects_MuchHits_Station")){
      DrawH1ByPattern("hss_Much_ClusterToPointRatio");
      DrawH1ByPattern("hss_Much_ClusterQuality");
      DrawH1ByPattern("hss_Much_NofPointsInCluster");
      DrawH1ByPattern("hss_Much_NofPointsInDigi");

      DrawNofObjectsHistigrams("hsh_Much_PixelHits", "hsh_Much_Points", "hsh_Much_Clusters", "hsh_Much_Digis", "nofObjects_Histograms");

      DrawNofObjectsHistigrams("hsh_Much_True_PixelHits", "hsh_Much_Points", "hsh_Much_Clusters", "hsh_Much_Digis", "nofTrueObjects_Histograms");

      DrawNofObjectsHistigrams("hsh_Much_Muon_PixelHits", "hsh_Much_Muon_Points",
            "hsh_Much_Muon_Clusters", "hsh_Much_Muon_Digis", "nofObjects_Muon_Histograms");

      DrawH1ByPattern("hss_Much_NofDigisByPoint");

      DrawH2ByPattern("hsc_Much_.*_2D");

      DrawLogYHistogramsByPattern("hss_Much_.*_Total");

      DrawH2ByPattern("hr_Much_.*_2D");

      DrawResidualHistogrms();

      DrawH1ByPattern("h_Much_.*RMS.*");
      DrawH1ByPattern("h_Much_.*Mean.*");
      DrawH1ByPattern("h_Much_.*Sigma.*");
   }
}

void CbmLitClusteringQaReport::DrawLogYHistogramsByPattern(
      const string& histNamePattern)
{
    vector<TH1*> histos = HM()->H1Vector(histNamePattern);
    if (histos.size() == 0) return;
    Int_t nofHistos = histos.size();
    for (UInt_t iHist = 0; iHist < nofHistos; iHist++) {
        TH1* hist = histos[iHist];
        string canvasName = GetReportName() + hist->GetName();
        TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 800, 500);
        DrawH1(hist, kLinear, kLog);
    }
}

void CbmLitClusteringQaReport::DrawHistogramsByLayer(const string& histNamePattern, const string& histName)
{
	vector<TH1*> histos = HM()->H1Vector(histNamePattern);
	if (histos.size() == 0) return;
	Int_t nofHistos = histos.size();
	string canvasName = GetReportName() + histName;
	TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 1600, 900);
	Int_t nColumns = 6;
	Int_t nRows = (Int_t)(nofHistos / nColumns);
	canvas->Divide(nColumns,nRows);
	Int_t h = nofHistos;
	for (UInt_t iHist = 0; iHist < nofHistos; iHist++) {
		TH1* hist = histos[iHist];
		canvas->cd(h);
		if(h > 1)h--;
		DrawH1(hist, kLinear, kLog);
	}
}

void CbmLitClusteringQaReport::DrawNofObjectsHistigrams(
      const string& histNamePatternPixelHits,
      const string& histNamePatternPoints,
      const string& histNamePatternClusters,
      const string& histNamePatternDigis,
      const string& histName)
{
   string canvasName = GetReportName() + histName;
   TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 800, 500);
   canvas->SetGrid();
   canvas->cd();
   Int_t nofStudies = 4;
   vector<string> labels(nofStudies);;
   vector<TH1*> histos(nofStudies);
   histos[0] = HM()->P1(histNamePatternPixelHits);
   labels[0] = "PixelHits";
   histos[1] = HM()->P1(histNamePatternPoints);
   labels[1] = "MCPoints";
   histos[2] = HM()->P1(histNamePatternClusters);
   labels[2] = "Clusters";
   histos[3] = HM()->P1(histNamePatternDigis);
   labels[3] = "Digis";
   DrawH1(histos, labels, kLinear, kLog, true, 0.65, 0.75, 0.95, 0.99);
}

void CbmLitClusteringQaReport::DrawResidualHistogrms()
{
   string canvasNameX = GetReportName() + "hr_Much_ResidualX_Layers";
   TCanvas* canvasRX = CreateCanvas(canvasNameX.c_str(), canvasNameX.c_str(), 1600, 900);
   Int_t nColumns = 6;
   Int_t nRows = (Int_t)(HM()->H2("hr_Much_ResidualX_2D")->GetXaxis()->GetNbins() / nColumns);
   canvasRX->Divide(nColumns,nRows);
   for (Int_t i = 0; i <= HM()->H2("hr_Much_ResidualX_2D")->GetXaxis()->GetNbins(); i++){
      std::stringstream stream1;
      stream1 << "hr_MuchResidalX_Layer_" << (i);
      TH1* lHistX = HM()->H2("hr_Much_ResidualX_2D")->ProjectionY(stream1.str().c_str(), i, i);
      lHistX->SetNameTitle(stream1.str().c_str(), stream1.str().c_str());
      lHistX->SetXTitle("Residual[cm]");
      lHistX->SetYTitle("Counter");
      if (lHistX == NULL) continue;
      lHistX->Fit("gaus");
      canvasRX->cd(i + 1);
      DrawH1(lHistX, kLinear, kLog);
   }
   string canvasNameY = GetReportName() + "hr_Much_ResidualY_Layers";
   TCanvas* canvasRY = CreateCanvas(canvasNameY.c_str(), canvasNameY.c_str(), 1600, 900);
   canvasRY->Divide(nColumns,nRows);
   for (Int_t i = 0; i <= HM()->H2("hr_Much_ResidualY_2D")->GetXaxis()->GetNbins(); i++){
      std::stringstream stream1;
      stream1 << "hr_MuchResidalY_Layer_" << (i);
      TH1* lHistY = HM()->H2("hr_Much_ResidualY_2D")->ProjectionY(stream1.str().c_str(), i, i);
      lHistY->SetNameTitle(stream1.str().c_str(), stream1.str().c_str());
      lHistY->SetXTitle("Residual[cm]");
      lHistY->SetYTitle("Counter");
      if (lHistY == NULL) continue;
      lHistY->Fit("gaus");
      canvasRY->cd(i + 1);
      DrawH1(lHistY, kLinear, kLog);
   }
   string canvasNamePX = GetReportName() + "hr_Much_PullX_Layers";
   TCanvas* canvasPX = CreateCanvas(canvasNamePX.c_str(), canvasNamePX.c_str(), 1600, 900);
   canvasPX->Divide(nColumns,nRows);
   for (Int_t i = 0; i <= HM()->H2("hr_Much_PullX_2D")->GetXaxis()->GetNbins(); i++){
      std::stringstream stream1;
      stream1 << "hr_MuchPullX_Layer_" << (i);
      TH1* lHistX = HM()->H2("hr_Much_PullX_2D")->ProjectionY(stream1.str().c_str(), i, i);
      lHistX->SetNameTitle(stream1.str().c_str(), stream1.str().c_str());
      lHistX->SetXTitle("Pull[cm]");
      lHistX->SetYTitle("Counter");
      if (lHistX == NULL) continue;
      lHistX->Fit("gaus");
      canvasPX->cd(i + 1);
      DrawH1(lHistX, kLinear, kLog);
   }
   string canvasNamePY = GetReportName() + "hr_Much_PullY_Layers";
   TCanvas* canvasPY = CreateCanvas(canvasNamePY.c_str(), canvasNamePY.c_str(), 1600, 900);
   canvasPY->Divide(nColumns,nRows);
   for (Int_t i = 0; i <= HM()->H2("hr_Much_PullY_2D")->GetXaxis()->GetNbins(); i++){
      std::stringstream stream1;
      stream1 << "hr_MuchPullY_Layer_" << (i);
      TH1* lHistY = HM()->H2("hr_Much_PullY_2D")->ProjectionY(stream1.str().c_str(), i, i);
      lHistY->SetNameTitle(stream1.str().c_str(), stream1.str().c_str());
      lHistY->SetXTitle("Pull[cm]");
      lHistY->SetYTitle("Counter");
      if (lHistY == NULL) continue;
      lHistY->Fit("gaus");
      canvasPY->cd(i + 1);
      DrawH1(lHistY, kLinear, kLog);
   }
}

void CbmLitClusteringQaReport::ScaleHistogramsByNofEvents()
{
   Int_t nofEvents = HM()->H1("hen_EventNo_ClusteringQa")->GetEntries();

   HM()->ScaleByPattern("hno_NofObjects_.*_Station", 1. / nofEvents);
   HM()->ShrinkEmptyBinsH1ByPattern("hno_NofObjects_.*_Station");

   if(HM()->Exists("hno_NofObjects_MuchHits_Station")){
      HM()->ScaleByPattern("(hss_Much_|hsc_Much_).+", 1. / nofEvents);
      HM()->ShrinkEmptyBinsH1ByPattern("(hss_Much_|hsh_Much_).+");
   }
}

ClassImp(CbmLitClusteringQaReport)
