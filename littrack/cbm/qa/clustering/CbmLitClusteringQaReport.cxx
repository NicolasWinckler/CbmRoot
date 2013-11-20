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
using std::stringstream;
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

//string CbmLitClusteringQaReport::PrintMuchAcuracy() const
//{
//   Float_t totalMuonEff = 0;
//   Float_t nofMuonPoints = 0;
//   Float_t nofMuonHits = 0;
//   Float_t totalEff = 0;
//   Float_t nofPoints = 0;
//   Float_t nofHits = 0;
//   string str = R()->TableBegin("Efficiency of clustering", list_of("Layer")("Muon Points")("Muon Hits")("Muon Efficiency (%)")
//         ("Total Points")("Total Hits")("Total Efficiency (%)"));
//   for (Int_t i = 1; i <= HM()->H2("hsh_Much_Muon_Points")->GetXaxis()->GetNbins(); i++){
//      Float_t muonEff = 0;
//      if(HM()->H1("hsh_Much_Muon_Points")->GetBinContent(i) != 0){
//         muonEff = (100. * (Float_t)(HM()->H1("hsh_Much_Muon_PixelHits")->GetBinContent(i))) /
//               (Float_t)(HM()->H1("hsh_Much_Muon_Points")->GetBinContent(i));
//      }
//      Float_t eff = 0;
//      if(HM()->H1("hsh_Much_Points")->GetBinContent(i) != 0){
//         eff = (100. * (Float_t)(HM()->H1("hsh_Much_True_PixelHits")->GetBinContent(i))) /
//               (Float_t)(HM()->H1("hsh_Much_Points")->GetBinContent(i));
//      }
//      nofMuonPoints += HM()->H1("hsh_Much_Muon_Points")->GetBinContent(i);
//      nofPoints += HM()->H1("hsh_Much_Points")->GetBinContent(i);
//      nofMuonHits += HM()->H1("hsh_Much_Muon_PixelHits")->GetBinContent(i);
//      nofHits += HM()->H1("hsh_Much_True_PixelHits")->GetBinContent(i);
//      str += R()->TableRow(list_of(NumberToString<Int_t>(i))(NumberToString<Float_t>(HM()->H1("hsh_Much_Muon_Points")->GetBinContent(i)))
//            (NumberToString<Float_t>(HM()->H1("hsh_Much_Muon_PixelHits")->GetBinContent(i)))(NumberToString<Float_t>(muonEff))
//            (NumberToString<Float_t>(HM()->H1("hsh_Much_Points")->GetBinContent(i)))
//            (NumberToString<Float_t>(HM()->H1("hsh_Much_True_PixelHits")->GetBinContent(i)))
//            (NumberToString<Float_t>(eff)));
//   }
//   if(nofMuonPoints != 0){
//      totalMuonEff = (100. * nofMuonHits) / nofMuonPoints;
//   }
//   if(nofPoints != 0){
//      totalEff = (100. * nofHits) / nofPoints;
//   }
//   std::stringstream stream1;
//   stream1 << "Total";
//   str += R()->TableRow(list_of(stream1.str())(NumberToString<Float_t>(nofMuonPoints))
//         (NumberToString<Float_t>(nofMuonHits))(NumberToString<Float_t>(totalMuonEff))
//         (NumberToString<Float_t>(nofPoints))(NumberToString<Float_t>(nofHits))
//         (NumberToString<Float_t>(totalEff)));
//   str += R()->TableEnd();
//   //---
//   str += R()->TableBegin("Much Hit Residual", list_of("Layer")("Mean X")("Mean Y")("RMS X")("RMS Y")("Sigma X")("Sigma Y"));
//   for (Int_t i = 1; i <= HM()->H2("hr_Much_ResidualX_2D")->GetXaxis()->GetNbins(); i++){
//      str += R()->TableRow(list_of(NumberToString<Int_t>(i))(NumberToString<Float_t>(HM()->H1("h_Much_Mean_X")->GetBinContent(i)))
//            (NumberToString<Float_t>(HM()->H1("h_Much_Mean_Y")->GetBinContent(i)))(NumberToString<Float_t>(HM()->H1("h_Much_RMS_X")->GetBinContent(i)))
//            (NumberToString<Float_t>(HM()->H1("h_Much_RMS_Y")->GetBinContent(i)))(NumberToString<Float_t>(HM()->H1("h_Much_Sigma_X")->GetBinContent(i)))
//            (NumberToString<Float_t>(HM()->H1("h_Much_Sigma_Y")->GetBinContent(i))));
//   }
//   str += R()->TableEnd();
//   str += R()->TableBegin("Much Hit Pull", list_of("Layer")("Mean X")("Mean Y")("RMS X")("RMS Y")("Sigma X")("Sigma Y"));
//   for (Int_t i = 1; i <= HM()->H2("hr_Much_PullX_2D")->GetXaxis()->GetNbins(); i++){
//      str += R()->TableRow(list_of(NumberToString<Int_t>(i))(NumberToString<Float_t>(HM()->H1("h_Much_PullMean_X")->GetBinContent(i)))
//            (NumberToString<Float_t>(HM()->H1("h_Much_PullMean_Y")->GetBinContent(i)))(NumberToString<Float_t>(HM()->H1("h_Much_PullRMS_X")->GetBinContent(i)))
//            (NumberToString<Float_t>(HM()->H1("h_Much_PullRMS_Y")->GetBinContent(i)))(NumberToString<Float_t>(HM()->H1("h_Much_PullSigma_X")->GetBinContent(i)))
//            (NumberToString<Float_t>(HM()->H1("h_Much_PullSigma_Y")->GetBinContent(i))));
//   }
//   str += R()->TableEnd();
//   str += R()->TableEnd();
//   return str;
//}

void CbmLitClusteringQaReport::Draw()
{
   ScaleAndShrinkHistograms();

   DrawNofObjectsHistograms("Mvd", "Event");
   DrawNofObjectsHistograms("Sts", "Event");
   DrawNofObjectsHistograms("Rich", "Event");
   DrawNofObjectsHistograms("Trd", "Event");
   DrawNofObjectsHistograms("Much", "Event");
   DrawNofObjectsHistograms("Tof", "Event");

   DrawNofObjectsHistograms("Mvd", "Station");
   DrawNofObjectsHistograms("Sts", "Station");
   DrawNofObjectsHistograms("Trd", "Station");
   DrawNofObjectsHistograms("Much", "Station");

   DrawH1ByPattern("hpa_.*Cluster_NofDigisInCluster_H1");
   DrawH2ByPattern("hpa_.*Cluster_NofDigisInCluster_H2", kLinear, kLinear, kLinear, "colz");

   DrawH1ByPattern("hpa_.*(Digi|Cluster|Hit)_NofPointsIn(Digi|Cluster|Hit)_H1");
   DrawH2ByPattern("hpa_.*(Digi|Cluster|Hit)_NofPointsIn(Digi|Cluster|Hit)_H2", kLinear, kLinear, kLinear, "colz");

   DrawH1ByPattern("hpa_.*Hit_Sigma.*_H1");
   DrawH2ByPattern("hpa_.*Hit_Sigma.*_H2", kLinear, kLinear, kLinear, "colz");

   DrawResidualsAndPulls("Trd");
   DrawResidualsAndPulls("Much");
}

void CbmLitClusteringQaReport::DrawNofObjectsHistograms(
	  const string& detName,
      const string& parameter)
{
	if (!HM()->Exists("hno_NofObjects_" + detName + "Points_" + parameter)) return;
	string canvasName = GetReportName() + "_NofObjects_" + detName + "_" + parameter;
	TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 800, 500);
	canvas->SetGrid();
	canvas->cd();
	vector<string> labels = list_of("Points")("Digis")("Clusters")("Hits");
	vector<TH1*> histos = list_of(HM()->H1("hno_NofObjects_" + detName + "Points_" + parameter))
		(HM()->H1("hno_NofObjects_" + detName + "Digis_" + parameter))
		(HM()->H1("hno_NofObjects_" + detName + "Clusters_" + parameter));
	if (HM()->Exists("hno_NofObjects_" + detName + "PixelHits_" + parameter)) histos.push_back(HM()->H1("hno_NofObjects_" + detName + "PixelHits_" + parameter));
	else if (HM()->Exists("hno_NofObjects_" + detName + "StrawHits_" + parameter)) histos.push_back(HM()->H1("hno_NofObjects_" + detName + "StrawHits_" + parameter));
	else if (HM()->Exists("hno_NofObjects_" + detName + "Hits_" + parameter)) histos.push_back(HM()->H1("hno_NofObjects_" + detName + "Hits_" + parameter));
	DrawH1(histos, labels, kLinear, kLinear, true, 0.65, 0.75, 0.95, 0.99);
}

void CbmLitClusteringQaReport::DrawResidualsAndPulls(
      const string& detName)
{
   if (!(HM()->Exists("hrp_" + detName + "_ResidualX_H2") && HM()->Exists("hrp_" + detName + "_ResidualY_H2")
         && HM()->Exists("hrp_" + detName + "_PullX_H2") && HM()->Exists("hrp_" + detName + "_ResidualX_H2"))) return;
   vector<string> par = list_of("ResidualX")("ResidualY")("PullX")("PullY");
   Int_t nofCanvases = par.size();
   for (Int_t iCanvas = 0; iCanvas < nofCanvases; iCanvas++) {
      string histName = "hrp_" + detName + "_" + par[iCanvas] + "_H2";
      TH2* hist = HM()->H2(histName);
      string canvasName = GetReportName() + "_" + histName + "_station";
      TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 1600, 900);
      Int_t nofBins = 10;//hist->GetXaxis()->GetNbins();
      Int_t nofColumns = 5;
      Int_t nofRows = (nofBins / nofColumns) + ((nofBins % 5 == 0) ? 0 : 1);
      canvas->Divide(nofColumns, nofRows);

      std::cout << nofBins << " " << nofColumns << " " << nofRows << std::endl;
      for (Int_t iBin = 1; iBin <= nofBins; iBin++){
         stringstream ss;
         ss << histName << "_" << iBin << "_py";
         TH1* projY = hist->ProjectionY(ss.str().c_str(), iBin, iBin);
         projY->SetNameTitle(ss.str().c_str(), ss.str().c_str());
         projY->SetXTitle(par[iCanvas].c_str());
         projY->SetYTitle("Yield");
         canvas->cd(iBin);
         DrawH1(projY, kLinear, kLinear);
         projY->Fit("gaus");
      }
   }
   DrawH2ByPattern("hrp_" + detName + "_.*_H2", kLinear, kLinear, kLinear, "colz");
}

void CbmLitClusteringQaReport::ScaleAndShrinkHistograms()
{
   Int_t nofEvents = HM()->H1("hen_EventNo_ClusteringQa")->GetEntries();

   HM()->ScaleByPattern("hno_NofObjects_.*_Station", 1. / nofEvents);
   HM()->ShrinkEmptyBinsH1ByPattern("hno_NofObjects_.*_Station");

   HM()->NormalizeToIntegralByPattern("hpa_.*Cluster_NofDigisInCluster_.*");
   HM()->ShrinkEmptyBinsH1ByPattern("hpa_.*Cluster_NofDigisInCluster_H1");
   HM()->ShrinkEmptyBinsH2ByPattern("hpa_.*Cluster_NofDigisInCluster_H2");

   HM()->NormalizeToIntegralByPattern("hpa_.*(Digi|Cluster|Hit)_NofPointsIn(Digi|Cluster|Hit)_.*");
   HM()->ShrinkEmptyBinsH1ByPattern("hpa_.*(Digi|Cluster|Hit)_NofPointsIn(Digi|Cluster|Hit)_H1");
   HM()->ShrinkEmptyBinsH2ByPattern("hpa_.*(Digi|Cluster|Hit)_NofPointsIn(Digi|Cluster|Hit)_H2");

   HM()->NormalizeToIntegralByPattern("hrp_.*_.*_H2");
   HM()->ShrinkEmptyBinsH2ByPattern("hrp_.*_.*_H2");

   HM()->NormalizeToIntegralByPattern("hpa_.*Hit_Sigma.*_.*");
   HM()->ShrinkEmptyBinsH1ByPattern("hpa_.*Hit_Sigma.*_H1");
   HM()->ShrinkEmptyBinsH2ByPattern("hpa_.*Hit_Sigma.*_H2");
}

ClassImp(CbmLitClusteringQaReport)
