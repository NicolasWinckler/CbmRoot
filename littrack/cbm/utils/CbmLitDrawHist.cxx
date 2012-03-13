/**
 * \file CbmLitDrawHist.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 **/
#include "CbmLitDrawHist.h"
#include "../../std/utils/CbmLitUtils.h"

#include "TH1.h"
#include "TH2.h"
#include "TH1D.h"
#include "TPad.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TStyle.h"
#include "TGraph.h"
#include "TGraph2D.h"
#include "TMath.h"
#include "TGaxis.h"

#include <string>
#include <limits>
#include <iostream>
#include <sstream>
#include <cassert>

using std::stringstream;

/* Set default styles for histograms. */
void SetStyles()
{
   gStyle->SetOptStat("");
   gStyle->SetOptFit(0);
   gStyle->SetOptTitle(0);

   gStyle->SetCanvasColor(kWhite);
   gStyle->SetFrameFillColor(kWhite);
   gStyle->SetFrameBorderMode(0);
   gStyle->SetPadColor(kWhite);
   gStyle->SetStatColor(kWhite);
   gStyle->SetTitleFillColor(kWhite);
   gStyle->SetPalette(1);
}

/* Draw TH1 histogram.*/
void DrawH1(
   TH1* hist,
   LitScale logx,
   LitScale logy,
   const string& drawOpt,
   Int_t color,
   Int_t lineWidth,
   Int_t lineStyle,
   Int_t markerSize,
   Int_t markerStyle)
{
   Double_t textSize = LitDrawingOptions::TextSize();
   hist->SetLineColor(color);
   hist->SetLineWidth(lineWidth);
   hist->SetLineStyle(lineStyle);
   hist->SetMarkerColor(color);
   hist->SetMarkerSize(markerSize);
   hist->SetMarkerStyle(markerStyle);
   if (logx == kLitLog) { gPad->SetLogx(); }
   if (logy == kLitLog) { gPad->SetLogy(); }
   hist->GetXaxis()->SetLabelSize(textSize);
   hist->GetXaxis()->SetNdivisions(505, kTRUE);
   hist->GetYaxis()->SetLabelSize(textSize);
   hist->GetXaxis()->SetTitleSize(textSize);
   hist->GetYaxis()->SetTitleSize(textSize);
   hist->GetXaxis()->SetTitleOffset(1.0);
   hist->GetYaxis()->SetTitleOffset(1.3);
   gPad->SetLeftMargin(0.17);
   gPad->SetBottomMargin(0.15);
   gPad->SetTicks(1, 1);
   hist->Draw(drawOpt.c_str());
}

/* Draw TH2 histogram.*/
void DrawH2(
   TH2* hist,
   LitScale logx,
   LitScale logy,
   LitScale logz,
   const string& drawOpt)
{
   Double_t textSize = LitDrawingOptions::TextSize();
   if (logx == kLitLog) { gPad->SetLogx(); }
   if (logy == kLitLog) { gPad->SetLogy(); }
   if (logz == kLitLog) { gPad->SetLogz(); }
   hist->GetXaxis()->SetLabelSize(textSize);
   hist->GetXaxis()->SetNdivisions(505, kTRUE);
   hist->GetYaxis()->SetLabelSize(textSize);
   hist->GetYaxis()->SetNdivisions(505, kTRUE);
   hist->GetZaxis()->SetLabelSize(textSize);
// hist->GetZaxis()->SetNdivisions(505, kTRUE);
   hist->GetXaxis()->SetTitleSize(textSize);
   hist->GetYaxis()->SetTitleSize(textSize);
   hist->GetZaxis()->SetTitleSize(textSize);
   hist->GetXaxis()->SetTitleOffset(1.0);
   hist->GetYaxis()->SetTitleOffset(1.3);
   hist->GetZaxis()->SetTitleOffset(1.5);
   gPad->SetLeftMargin(0.17);
   gPad->SetRightMargin(0.30);
   gPad->SetBottomMargin(0.15);
   gPad->SetTicks(1, 1);
   hist->Draw(drawOpt.c_str());
}

/* Draw several TH1 histograms. */
void DrawH1(
   const vector<TH1*>& histos,
   const vector<string>& histLabels,
   LitScale logx,
   LitScale logy,
   Bool_t drawLegend,
   Double_t x1,
   Double_t y1,
   Double_t x2,
   Double_t y2,
   const string& drawOpt)
{
   assert(histos.size() != 0 && histLabels.size() == histos.size());
   Double_t max = std::numeric_limits<Double_t>::min();
   Int_t nofHistos = histos.size();
   TLegend* legend = new TLegend(x1, y1, x2, y2);
   legend->SetFillColor(kWhite);
   for (UInt_t iHist = 0; iHist < nofHistos; iHist++) {
      TH1* hist = histos[iHist];
      string opt = (iHist == 0) ? drawOpt : (iHist == nofHistos - 1) ? "SAME" + drawOpt : "SAME" + drawOpt;
      DrawH1(hist, logx, logy, opt, LitDrawingOptions::Color(iHist), LitDrawingOptions::LineWidth(),
            LitDrawingOptions::LineStyle(iHist), LitDrawingOptions::MarkerSize(), LitDrawingOptions::MarkerStyle(iHist));
      max = std::max(max, hist->GetMaximum());
      legend->AddEntry(hist, histLabels[iHist].c_str(), "lp");
   }
   histos[0]->SetMaximum(max * 1.10);
   if (drawLegend) { legend->Draw(); }
}

void DrawGraph(
   TGraph* graph,
   LitScale logx,
   LitScale logy,
   const string& drawOpt,
   Int_t color,
   Int_t lineWidth,
   Int_t lineStyle,
   Int_t markerSize,
   Int_t markerStyle)
{
   Double_t textSize = LitDrawingOptions::TextSize();
   graph->SetLineColor(color);
   graph->SetLineWidth(lineWidth);
   graph->SetLineStyle(lineStyle);
   graph->SetMarkerColor(color);
   graph->SetMarkerSize(markerSize);
   graph->SetMarkerStyle(markerStyle);
   if (drawOpt.find("A") != string::npos) {
      if (logx == kLitLog) { gPad->SetLogx(); }
      if (logy == kLitLog) { gPad->SetLogy(); }
      graph->GetXaxis()->SetLabelSize(textSize);
      graph->GetXaxis()->SetNdivisions(505, kTRUE);
      graph->GetYaxis()->SetLabelSize(textSize);
      graph->GetXaxis()->SetTitleSize(textSize);
      graph->GetYaxis()->SetTitleSize(textSize);
      graph->GetXaxis()->SetTitleOffset(1.0);
      graph->GetYaxis()->SetTitleOffset(1.3);
   }
   gPad->SetLeftMargin(0.17);
   gPad->SetBottomMargin(0.15);
   graph->Draw(drawOpt.c_str());
}


/* Draw several TGraphs. */
void DrawGraph(
   const vector<TGraph*>& graphs,
   const vector<string>& graphLabels,
   LitScale logx,
   LitScale logy,
   Bool_t drawLegend,
   Double_t x1,
   Double_t y1,
   Double_t x2,
   Double_t y2)
{
   assert(graphs.size() != 0 && graphs.size() == graphLabels.size());

   Double_t max = std::numeric_limits<Double_t>::min();
   Double_t min = std::numeric_limits<Double_t>::max();
   TLegend* legend = new TLegend(x1, y1, x2, y2);
   legend->SetFillColor(kWhite);
   Int_t nofGraphs = graphs.size();
   for (UInt_t iGraph = 0; iGraph < nofGraphs; iGraph++) {
      TGraph* graph = graphs[iGraph];
      string opt = (iGraph == 0) ? "ACP" : "CP";
      DrawGraph(graph, logx, logy, opt, LitDrawingOptions::Color(iGraph), LitDrawingOptions::LineWidth(),
            LitDrawingOptions::LineStyle(iGraph), LitDrawingOptions::MarkerSize(), LitDrawingOptions::MarkerStyle(iGraph));
      max = std::max(graph->GetYaxis()->GetXmax(), max);
      min = std::min(graph->GetYaxis()->GetXmin(), min);
      legend->AddEntry(graph, graphLabels[iGraph].c_str(), "lp");
   }
   graphs[0]->SetMaximum(max);
   graphs[0]->SetMinimum(min);
   if (drawLegend) { legend->Draw(); }
}

/* Draws 2D graph.*/
void DrawGraph2D(
   TGraph2D* graph,
   LitScale logx,
   LitScale logy,
   LitScale logz,
   const string& drawOpt)
{
   Double_t textSize = LitDrawingOptions::TextSize();
   if (logx == kLitLog) { gPad->SetLogx(); }
   if (logy == kLitLog) { gPad->SetLogy(); }
   if (logz == kLitLog) { gPad->SetLogz(); }
   graph->GetXaxis()->SetLabelSize(textSize);
   graph->GetXaxis()->SetNdivisions(505, kTRUE);
   graph->GetYaxis()->SetLabelSize(textSize);
   graph->GetYaxis()->SetNdivisions(505, kTRUE);
   graph->GetZaxis()->SetLabelSize(textSize);
   graph->GetZaxis()->SetNdivisions(505, kTRUE);
   graph->GetXaxis()->SetTitleSize(textSize);
   graph->GetYaxis()->SetTitleSize(textSize);
   graph->GetZaxis()->SetTitleSize(textSize);
   graph->GetXaxis()->SetTitleOffset(1.7);
   graph->GetYaxis()->SetTitleOffset(2.0);
   graph->GetZaxis()->SetTitleOffset(1.7);
   gPad->SetLeftMargin(0.22);
// gPad->SetRightMargin(0.17);
// gPad->SetBottomMargin(0.15);

   graph->Draw(drawOpt.c_str());
}

void  DrawHistSigmaRMS(
   Double_t sigma,
   Double_t rms)
{
   string txt1 = lit::NumberToString<Double_t>(sigma, 2) + " / " + lit::NumberToString<Double_t>(rms, 2);
   TLatex text;
   text.SetTextAlign(21);
   text.SetTextSize(0.08);
   text.DrawTextNDC(0.5, 0.83, txt1.c_str());
}

TH1D* DivideH1(
   TH1D* h1,
   TH1D* h2,
   const string& name,
   const string& title,
   const string& axisX,
   const string& axisY)
{
   h1->Sumw2();
   h2->Sumw2();
   TH1D* h3 = new TH1D(name.c_str(), string(title +";"+axisX+";"+ axisY).c_str(),
                       h1->GetNbinsX(), h1->GetXaxis()->GetXmin(),h1->GetXaxis()->GetXmax());
   h3->Divide(h1, h2, 1., 1., "B");
   return h3;
}

string CalcEfficiency(
   TH1* histRec,
   TH1* histAcc)
{
   if (histAcc->GetEntries() == 0) { return "0.0"; }
   else {
      Double_t eff = Double_t(histRec->GetEntries()) / Double_t(histAcc->GetEntries());
      stringstream ss;
      ss.precision(3);
      ss << eff;
      return ss.str();
   }
}
