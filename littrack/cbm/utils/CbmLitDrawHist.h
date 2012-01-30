/**
 * \file CbmLitDrawHist.h
 * \brief Helper functions for drawing 1D and 2D histograms and graphs.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
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

/**
 * Default settings for drawing.
 */
const Int_t LIT_COLOR1 = kRed;
const Int_t LIT_COLOR2 = kBlue;
const Int_t LIT_COLOR3 = kGreen + 3;
const Int_t LIT_COLOR4 = kYellow + 2;
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

/**
 * \enum LitScale
 * \brief Define linear or logarithmic scale for drawing.
 */
enum LitScale {
   kLitLog = 0, /**> Linear scale. */
   kLitLinear = 1 /**> Logarithmic scale */
};


/**
 * \fn SetStyles
 * \brief Set default styles for histograms.
 */
void SetStyles();

/**
 * \fn DrawH1
 * \brief Draw 1D histogram.
 * \param[in] hist Pointer to histogram.
 * \param[in] logx Specify logarithmic or linear scale for X axis.
 * \param[in] logy Specify logarithmic or linear scale for Y axis.
 * \param[in] drawOpt Other drawing options (see ROOT documentation for details).
 * \param[in] color Color.
 * \param[in] lineWidth Line width.
 * \param[in] lineStyle Line style (see ROOT documentation for details).
 * \param[in] markerSize Marker size.
 * \param[in] markerStyle Marker style (see ROOT documentation for details).
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


/**
 * \fn DrawH2
 * \brief Draw 2D histogram.
 * \param[in] logx Specify logarithmic or linear scale for X axis.
 * \param[in] logy Specify logarithmic or linear scale for Y axis.
 * \param[in] logz Specify logarithmic or linear scale for Z axis.
 * \param[in] drawOpt Other drawing options (see ROOT documentation for details).
 */
void DrawH2(
   TH2* hist,
   LitScale logx = kLitLinear,
   LitScale logy = kLitLinear,
   LitScale logz = kLitLinear,
   const std::string& drawOpt = "COLZ");


/**
 * \fn DrawH1
 * \brief Draw up to 4 1D histograms. If hist == NULL than histogram will not be drawn.
 * \param[in] hist1 Pointer to the first histogram.
 * \param[in] hist2 Pointer to the second histogram.
 * \param[in] hist3 Pointer to the third histogram.
 * \param[in] hist4 Pointer to the fourth histogram.
 * \param[in] hist1label[in] Label text for the first histogram.
 * \param[in] hist2label[in] Label text for the second histogram.
 * \param[in] hist3label[in] Label text for the third histogram.
 * \param[in] hist4label[in] Label text for the fourth histogram.
 * \param[in] logx Specify logarithmic or linear scale for X axis.
 * \param[in] logy Specify logarithmic or linear scale for Y axis.
 * \param[in] drawLegend If true than legend will be drawn.
 * \param[in] x1 coordinates of the Legend in the current pad
 * \param[in] y1 coordinates of the Legend in the current pad
 * \param[in] x2 coordinates of the Legend in the current pad
 * \param[in] y2 coordinates of the Legend in the current pad
 * \param[in] drawOpt Draw options for TH1->Draw method.
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

/**
 * \fn DrawHistSigmaRMS
 * \brief Draw sigma and RMS on histogram.
 * \param[in] index Letter index for histogram.
 * \param[in] sigma Sigma value.
 * \param[in] rms RMS value.
 */
void DrawHistSigmaRMS(
   Int_t index,
   Double_t sigma,
   Double_t rms);

/**
 * \fn DrawGraph // TODO Do not use axis titles.
 * \brief Draw TGraph.
 * \param[in] graph Pointer to TGraph.
 * \param[in] titleX X axis title.
 * \param[in] titleY Y axis title.
 * \param[in] color Color.
 * \param[in] lineWidth Line width.
 * \param[in] lineStyle Line style (see ROOT documentation for details).
 * \param[in] markerSize Marker size.
 * \param[in] markerStyle Marker style (see ROOt documentation for details).
 * \param[in] logx Specify logarithmic or linear scale for X axis.
 * \param[in] logy Specify logarithmic or linear scale for Y axis.
 * \param[in] drawOpt Other drawing options (see ROOT documentation for details).
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


/**
 * \fn DrawGraph. // TODO Do not use axis titles.
 * \brief Draw upto 3 TGraphs. If graph == NULL than graph will not be drawn.
 * \param[in] graph1 Pointer to the first TGraph.
 * \param[in] graph2 Pointer to the second TGraph.
 * \param[in] graph3 Pointer to the third TGraph.
 * \param[in] legendLabel Legend label text.
 * \param[in] xAxisLabel X axis label text.
 * \param[in] yAxisLabel Y axis label text.
 * \param[in] hist1label Label text for the first TGraph.
 * \param[in] hist2label Label text for the second TGraph.
 * \param[in] hist3label Label text for the third TGraph.
 * \param[in] logx Specify logarithmic or linear scale for X axis.
 * \param[in] logy Specify logarithmic or linear scale for Y axis.
 * \param[in] drawLegend If true than legend will be drawn.
 * \param[in] x1 coordinates of the Legend in the current pad
 * \param[in] y1 coordinates of the Legend in the current pad
 * \param[in] x2 coordinates of the Legend in the current pad
 * \param[in] y2 coordinates of the Legend in the current pad
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

/**
 * \fn DrawGraph2D // TODO Do not use axis titles.
 * \brief Draw 2D graph.
 * \param[in] graph Pointer to TGraph2D.
 * \param[in] titleX X axis title.
 * \param[in] titleY Y axis title.
 * \param[in] titleZ Z axis title.
 * \param[in] logx Specify logarithmic or linear scale for X axis.
 * \param[in] logy Specify logarithmic or linear scale for Y axis.
 * \param[in] logz Specify logarithmic or linear scale for Z axis.
 * \param[in] drawOpt Other drawing options (see ROOT documentation for details).
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

/**
 * \fn DivideH1
 * \brief Divide two histograms.
 * \param[in] h1 Pointer to the first histogram.
 * \param[in] h2 Pointer to the second histogram.
 * \param[in] name Name of a new histogram.
 * \param[in] title Histogram title.
 * \param[in] axisX X axis title.
 * \param[in] axisY Y axis title.
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
