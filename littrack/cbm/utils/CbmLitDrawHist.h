/** CbmLitDrawHist.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 1.0
 * Some help functions for drawing 1D and 2D histograms.
 **/
#ifndef CBMLITDRAWHIST_H_
#define CBMLITDRAWHIST_H_

#include "TH1.h"
#include "TH1D.h"
#include "TH2.h"
#include "TPad.h"
#include "TLegend.h"
#include "TGraph.h"
#include "TGraph2D.h"

// Set of default settings for drawing
const Int_t LIT_COLOR1 = kRed;
const Int_t LIT_COLOR2 = kBlue;
const Int_t LIT_COLOR3 = kGreen+3;
const Int_t LIT_COLOR4 = kYellow+2;
const Int_t LIT_LINE_WIDTH = 2;
const Int_t LIT_LINE_STYLE1 = 1;
const Int_t LIT_LINE_STYLE2 = 2;
const Int_t LIT_LINE_STYLE3 = 3;
const Int_t LIT_LINE_STYLE4 = 4;
const Int_t LIT_MARKER_SIZE = 1;
const Int_t LIT_MARKER_STYLE1 = kOpenCircle;
const Int_t LIT_MARKER_STYLE2 = kOpenSquare;
const Int_t LIT_MARKER_STYLE3 = kOpenTriangleUp;
const Int_t LIT_MARKER_STYLE4 = kOpenDiamond;
const Double_t LIT_TEXT_SIZE = 0.06;

enum LitScale { kLitLog = 0, kLitLinear = 1};


/* Set default styles for histograms. */
void SetStyles();

/* Draws 1D histogram.
 * @param hist Pointer to the histogram.
 * @param titleX X axis title.
 * @param titleY Y axis title.
 * @param logx If true than X axis is plotted in logarithmic scale.
 * @param logy If true than Y axis is plotted in logarithmic scale.
 * @param drawOpt Other drawing options (see ROOT documentation for details).
 * @param color Color.
 * @param lineWidth Line width.
 * @param lineStyle Line style (see ROOT documentation for details).
 * @param markerSize Marker size.
 * @param markerStyle Marker style (see ROOt documentation for details).
 */
void DrawH1(
   TH1* hist,
   LitScale logx = kLitLinear,
   LitScale logy = kLitLinear,
   const std::string& drawOpt = "",
   Int_t color = LIT_COLOR1,
   Int_t lineWidth = LIT_LINE_WIDTH,
   Int_t lineStyle = LIT_LINE_STYLE1,
   Int_t markerSize = LIT_MARKER_SIZE,
   Int_t markerStyle = LIT_MARKER_STYLE1);


/* Draws 2D histogram.
 * @param hist Pointer to the histogram.
 * @param titleX X axis title.
 * @param titleY Y axis title.
 * @param titleZ Z axis title.
 * @param logx If true than X axis is plotted in logarithmic scale.
 * @param logy If true than Y axis is plotted in logarithmic scale.
 * @param logz If true than Z axis is plotted in logarithmic scale.
 * @param drawOpt Other drawing options (see ROOT documentation for details).
 */
void DrawH2(
   TH2* hist,
   LitScale logx = kLitLinear,
   LitScale logy = kLitLinear,
   LitScale logz = kLitLinear,
   const std::string& drawOpt = "COLZ");


/* Draws up to 3 1D histograms. If hist == NULL than histogram will not be drawn.
 * @param hist1 Pointer to the first histogram.
 * @param hist2 Pointer to the second histogram.
 * @param hist3 Pointer to the third histogram.
 * @param hist4 Pointer to the fourth histogram.
 * @param legendLabel Legend label text.
 * @param xAxisLabel X axis label text.
 * @param yAxisLabel Y axis label text.
 * @param hist1label Label text for the first histogram.
 * @param hist2label Label text for the second histogram.
 * @param hist3label Label text for the third histogram.
 * @param hist4label Label text for the fourth histogram
 * @param logx If true than X axis is plotted in logarithmic scale.
 * @param logy If true than Y axis is plotted in logarithmic scale.
 * @param drawLegend If true than legend will be drawn.
 * @param x1 coordinates of the Legend in the current pad
 * @param y1 coordinates of the Legend in the current pad
 * @param x2 coordinates of the Legend in the current pad
 * @param y2 coordinates of the Legend in the current pad
 * @param drawOpt Draw option for TH1->Draw method
 */
void DrawH1(
   TH1* hist1,
   TH1* hist2,
   TH1* hist3,
   TH1* hist4,
   const std::string& hist1label,
   const std::string& hist2label,
   const std::string& hist3label,
   const std::string& hist4label,
   LitScale logx,
   LitScale logy,
   Bool_t drawLegend,
   Double_t x1,
   Double_t y1,
   Double_t x2,
   Double_t y2,
   const std::string& drawOpt = "");

/* Draws gma/RMS value on the histogram
 * @param index Index of letter for the histogram
 * @param sigma Sigma value
 * @param rms RMS value
 */
void DrawHistSigmaRMS(
   Int_t index,
   Double_t sigma,
   Double_t rms);

/* Draws graph.
 * @param graph Pointer to the TGraph.
 * @param titleX X axis title.
 * @param titleY Y axis title.
 * @param color Color.
 * @param lineWidth Line width.
 * @param lineStyle Line style (see ROOT documentation for details).
 * @param markerSize Marker size.
 * @param markerStyle Marker style (see ROOt documentation for details).
 * @param logx If true than X axis is plotted in logarithmic scale.
 * @param logy If true than Y axis is plotted in logarithmic scale.
 * @param drawOpt Other drawing options (see ROOT documentation for details).
 */
void DrawGraph(
   TGraph* graph,
   const std::string& titleX,
   const std::string& titleY,
   Int_t color,
   Int_t lineWidth,
   Int_t lineStyle,
   Int_t markerSize,
   Int_t markerStyle,
   LitScale logx,
   LitScale logy,
   const std::string& drawOpt);


/* Draws up to TGraphs. If graph == NULL than graph will not be drawn.
 * @param graph1 Pointer to the first TGraph.
 * @param graph2 Pointer to the second TGraph.
 * @param graph3 Pointer to the third TGraph.
 * @param legendLabel Legend label text.
 * @param xAxisLabel X axis label text.
 * @param yAxisLabel Y axis label text.
 * @param hist1label Label text for the first TGraph.
 * @param hist2label Label text for the second TGraph.
 * @param hist3label Label text for the third TGraph.
 * @param logx If true than X axis is plotted in logarithmic scale.
 * @param logy If true than Y axis is plotted in logarithmic scale.
 * @param drawLegend If true than legend will be drawn.
 * @param x1 coordinates of the Legend in the current pad
 * @param y1 coordinates of the Legend in the current pad
 * @param x2 coordinates of the Legend in the current pad
 * @param y2 coordinates of the Legend in the current pad
 */
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
   LitScale logx,
   LitScale logy,
   Bool_t drawLegend,
   Double_t x1,
   Double_t y1,
   Double_t x2,
   Double_t y2);

/* Draws 2D graph.
 * @param graph Pointer to the TGraph2D.
 * @param titleX X axis title.
 * @param titleY Y axis title.
 * @param titleZ Z axis title.
 * @param logx If true than X axis is plotted in logarithmic scale.
 * @param logy If true than Y axis is plotted in logarithmic scale.
 * @param logz If true than Z axis is plotted in logarithmic scale.
 * @param drawOpt Other drawing options (see ROOT documentation for details).
 */
void DrawGraph2D(
   TGraph2D* graph,
   const std::string& titleX,
   const std::string& titleY,
   const std::string& titleZ,
   LitScale logx,
   LitScale logy,
   LitScale logz,
   const std::string& drawOpt);

/* Divide two histograms.
 * @param h1 Pointer to the first histogram.
 * @param h2 Pointer to the second histogram.
 * @param name name of a new histogram.
 * @param title histogram title.
 * @param axisX X axis title.
 * @param axisY Y axis title.
 */
TH1D* DivideH1(
   TH1D* h1,
   TH1D* h2,
   const std::string& name,
   const std::string& title,
   const std::string& axisX,
   const std::string& axisY);


std::string CalcEfficiency(
   TH1* histRec,
   TH1* histAcc);


#endif
