/** CbmLitDrawHist.cxx
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 **/
#include "CbmLitDrawHist.h"
#include "std/utils/CbmLitUtils.h"

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
#include <string>
#include <limits>
#include <iostream>
#include <sstream>

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


/* Draws 1D histogram.*/
void DrawHist1D(
   TH1* hist,
   const std::string& titleX,
   const std::string& titleY,
   Int_t color,
   Int_t lineWidth,
   Int_t lineStyle,
   Int_t markerSize,
   Int_t markerStyle,
   Bool_t logx,
   Bool_t logy,
   const std::string& drawOpt)
{
   Double_t textSize = LIT_TEXT_SIZE;
   hist->GetXaxis()->SetTitle(titleX.c_str());
   hist->GetYaxis()->SetTitle(titleY.c_str());
   hist->SetLineColor(color);
   hist->SetLineWidth(lineWidth);
   hist->SetLineStyle(lineStyle);
   hist->SetMarkerColor(color);
   hist->SetMarkerSize(markerSize);
   hist->SetMarkerStyle(markerStyle);
   if (logx) { gPad->SetLogx(); }
   if (logy) { gPad->SetLogy(); }
   hist->GetXaxis()->SetLabelSize(textSize);
   hist->GetXaxis()->SetNdivisions(505, kTRUE);
   hist->GetYaxis()->SetLabelSize(textSize);
   hist->GetXaxis()->SetTitleSize(textSize);
   hist->GetYaxis()->SetTitleSize(textSize);
   hist->GetXaxis()->SetTitleOffset(1.0);
   hist->GetYaxis()->SetTitleOffset(1.3);
   gPad->SetLeftMargin(0.17);
   gPad->SetBottomMargin(0.15);
   hist->Draw(drawOpt.c_str());
}

/* Draws 1D histogram.*/
void DrawHist2D(
   TH2* hist,
   const std::string& titleX,
   const std::string& titleY,
   const std::string& titleZ,
   Bool_t logx,
   Bool_t logy,
   Bool_t logz,
   const std::string& drawOpt)
{
   Double_t textSize = LIT_TEXT_SIZE;
   hist->GetXaxis()->SetTitle(titleX.c_str());
   hist->GetYaxis()->SetTitle(titleY.c_str());
   hist->GetZaxis()->SetTitle(titleZ.c_str());
   if (logx) { gPad->SetLogx(); }
   if (logy) { gPad->SetLogy(); }
   if (logz) { gPad->SetLogz(); }
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

   hist->Draw(drawOpt.c_str());
}

/* Draws up to 4 1D histograms. If hist == NULL than histogram will not be drawn. */
void DrawHist1D(
   TH1* hist1,
   TH1* hist2,
   TH1* hist3,
   TH1* hist4,
   const std::string& legendLabel,
   const std::string& xAxisLabel,
   const std::string& yAxisLabel,
   const std::string& hist1label,
   const std::string& hist2label,
   const std::string& hist3label,
   const std::string& hist4label,
   Bool_t logx,
   Bool_t logy,
   Bool_t drawLegend,
   Double_t x1,
   Double_t y1,
   Double_t x2,
   Double_t y2)
{
   Double_t max;
   if (hist1 != NULL) {
      DrawHist1D(hist1, xAxisLabel, yAxisLabel,
                 LIT_COLOR1, LIT_LINE_WIDTH, LIT_LINE_STYLE1, LIT_MARKER_SIZE,
                 LIT_MARKER_STYLE1, logx, logy, "");
      max = hist1->GetMaximum();
   }

   if (hist2 != NULL) {
      DrawHist1D(hist2, xAxisLabel, yAxisLabel,
                 LIT_COLOR2, LIT_LINE_WIDTH, LIT_LINE_STYLE2, LIT_MARKER_SIZE,
                 LIT_MARKER_STYLE2, logx, logy, "SAME");
      if (max < hist2->GetMaximum()) { max = hist2->GetMaximum(); }
   }

   if (hist3 != NULL) {
      DrawHist1D(hist3, xAxisLabel, yAxisLabel,
                 LIT_COLOR3, LIT_LINE_WIDTH, LIT_LINE_STYLE3, LIT_MARKER_SIZE,
                 LIT_MARKER_STYLE3, logx, logy, "SAME");
      if (max < hist3->GetMaximum()) { max = hist3->GetMaximum(); }
   }

   if (hist4 != NULL) {
      DrawHist1D(hist4, xAxisLabel, yAxisLabel,
                 LIT_COLOR4, LIT_LINE_WIDTH, LIT_LINE_STYLE4, LIT_MARKER_SIZE,
                 LIT_MARKER_STYLE4, logx, logy, "SAME");
      if (max < hist4->GetMaximum()) { max = hist4->GetMaximum(); }
   }

   if (hist1 != NULL) { hist1->SetMaximum(max * 1.05); }

   TLegend* l1 = new TLegend(x1, y1, x2, y2);
   l1->SetFillColor(kWhite);
   if (legendLabel != "") { l1->SetHeader(legendLabel.c_str()); }
   if (hist1 != NULL) { l1->AddEntry(hist1,hist1label.c_str(),"lp"); }
   if (hist2 != NULL) { l1->AddEntry(hist2,hist2label.c_str(),"lp"); }
   if (hist3 != NULL) { l1->AddEntry(hist3,hist3label.c_str(),"lp"); }
   if (hist4 != NULL) { l1->AddEntry(hist4,hist4label.c_str(),"lp"); }
   if (drawLegend) { l1->Draw(); }
}


void  DrawHistSigmaRMS(
   Int_t index,
   Double_t sigma,
   Double_t rms)
{
   std::string txt1 = lit::ToString<Double_t>(sigma) + " / " + lit::ToString<Double_t>(rms);
   TLatex text;
   text.SetTextAlign(21);
   text.SetTextSize(0.08); //0.1
// if (index != 11)
   text.DrawTextNDC(0.5, 0.83, txt1.c_str());
   std::string txt2 = lit::ToString<char>(index+97) + ")";
   text.DrawTextNDC(0.8, 0.7, txt2.c_str());
}


void DrawGraph(
   TGraph* graph,
   const std::string& titleX,
   const std::string& titleY,
   Int_t color,
   Int_t lineWidth,
   Int_t lineStyle,
   Int_t markerSize,
   Int_t markerStyle,
   Bool_t logx,
   Bool_t logy,
   const std::string& drawOpt)
{
   Double_t textSize = LIT_TEXT_SIZE;
   graph->SetLineColor(color);
   graph->SetLineWidth(lineWidth);
   graph->SetLineStyle(lineStyle);
   graph->SetMarkerColor(color);
   graph->SetMarkerSize(markerSize);
   graph->SetMarkerStyle(markerStyle);
   if (drawOpt.find("A") != std::string::npos) {
      if (logx) { gPad->SetLogx(); }
      if (logy) { gPad->SetLogy(); }
      graph->GetXaxis()->SetTitle(titleX.c_str());
      graph->GetYaxis()->SetTitle(titleY.c_str());
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


/* Draws up to 3 graphs. If graph == NULL than graph will not be drawn. */
void DrawGraph(
   TGraph* graph1,
   TGraph* graph2,
   TGraph* graph3,
   const std::string& legendLabel,
   const std::string& xAxisLabel,
   const std::string& yAxisLabel,
   const std::string& hist1label,
   const std::string& hist2label,
   const std::string& hist3label,
   Bool_t logx,
   Bool_t logy,
   Bool_t drawLegend,
   Double_t x1,
   Double_t y1,
   Double_t x2,
   Double_t y2)
{
   double max = std::numeric_limits<double>::min();
   double min = std::numeric_limits<double>::max();

   if (graph1 != NULL) {
      DrawGraph(graph1, xAxisLabel, yAxisLabel,
                LIT_COLOR1, LIT_LINE_WIDTH, LIT_LINE_STYLE1, LIT_MARKER_SIZE,
                LIT_MARKER_STYLE1, logx, logy, "ACP");
      max = graph1->GetYaxis()->GetXmax();
      min = graph1->GetYaxis()->GetXmin();
   }

   if (graph2 != NULL) {
      DrawGraph(graph2, xAxisLabel, yAxisLabel,
                LIT_COLOR2, LIT_LINE_WIDTH, LIT_LINE_STYLE2, LIT_MARKER_SIZE,
                LIT_MARKER_STYLE2, logx, logy, "CP");
      max = std::max(max, graph2->GetYaxis()->GetXmax());
      min = std::min(min, graph2->GetYaxis()->GetXmin());
   }

   if (graph3 != NULL) {
      DrawGraph(graph3, xAxisLabel, yAxisLabel,
                LIT_COLOR3, LIT_LINE_WIDTH, LIT_LINE_STYLE3, LIT_MARKER_SIZE,
                LIT_MARKER_STYLE3, logx, logy, "CP");
      max = std::max(max, graph3->GetYaxis()->GetXmax());
      min = std::min(min, graph3->GetYaxis()->GetXmin());
   }

   if (graph1 != NULL) {
      graph1->SetMaximum(max);
      graph1->SetMinimum(min);
   }

   TLegend* l1 = new TLegend(x1, y1, x2, y2);
   l1->SetFillColor(kWhite);
   l1->SetHeader(legendLabel.c_str());
   if (graph1 != NULL) { l1->AddEntry(graph1,hist1label.c_str(),"lp"); }
   if (graph2 != NULL) { l1->AddEntry(graph2,hist2label.c_str(),"lp"); }
   if (graph3 != NULL) { l1->AddEntry(graph3,hist3label.c_str(),"lp"); }
   if (drawLegend) { l1->Draw(); }
}

/* Draws 2D graph.*/
void DrawGraph2D(
   TGraph2D* graph,
   const std::string& titleX,
   const std::string& titleY,
   const std::string& titleZ,
   Bool_t logx,
   Bool_t logy,
   Bool_t logz,
   const std::string& drawOpt)
{
   Double_t textSize = LIT_TEXT_SIZE;
   graph->GetXaxis()->SetTitle(titleX.c_str());
   graph->GetYaxis()->SetTitle(titleY.c_str());
   graph->GetZaxis()->SetTitle(titleZ.c_str());
   if (logx) { gPad->SetLogx(); }
   if (logy) { gPad->SetLogy(); }
   if (logz) { gPad->SetLogz(); }
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

TH1D* Divide1DHists(
   TH1D* h1,
   TH1D* h2,
   const std::string& name,
   const std::string& title,
   const std::string& axisX,
   const std::string& axisY)
{
   h1->Sumw2();
   h2->Sumw2();
   TH1D* h3 = new TH1D(name.c_str(), std::string(title +";"+axisX+";"+ axisY).c_str(),
                       h1->GetNbinsX(), h1->GetXaxis()->GetXmin(),h1->GetXaxis()->GetXmax());
   h3->Divide(h1, h2, 1., 1., "B");
   return h3;
}

std::string CalcEfficiency(
   TH1* histRec,
   TH1* histAcc)
{
   if (histAcc->GetEntries() == 0) { return "0.0"; }
   else {
      Double_t eff = Double_t(histRec->GetEntries()) / Double_t(histAcc->GetEntries());
      std::stringstream ss;
      ss.precision(3);
      ss << eff;
      return ss.str();
   }
}
