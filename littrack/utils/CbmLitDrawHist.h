/** CbmLitDrawHist.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 1.0
 * Some help functions for drawing 1D and 2D histograms.
 **/
#ifndef CBMLITDRAWHIST_H_
#define CBMLITDRAWHIST_H_

#include "TH1.h"
#include "TH2.h"
#include "TPad.h"
#include "TLegend.h"
#include "TGraph.h"

// Set of standard settings for drawing
const Int_t LIT_COLOR1 = kRed;
const Int_t LIT_COLOR2 = kBlue;
const Int_t LIT_COLOR3 = kGreen+3;
const Int_t LIT_LINE_WIDTH = 1;
const Int_t LIT_LINE_STYLE1 = 1;
const Int_t LIT_LINE_STYLE2 = 2;
const Int_t LIT_LINE_STYLE3 = 3;
const Int_t LIT_MARKER_SIZE = 1;
const Int_t LIT_MARKER_STYLE1 = kOpenCircle;
const Int_t LIT_MARKER_STYLE2 = kOpenSquare;
const Int_t LIT_MARKER_STYLE3 = kOpenTriangleUp;

/* Set default styles for histograms. */
void SetStyles();

/* Draws 1D histogram.
 * @param hist Pointer to the histogram.
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
		const std::string& drawOpt);



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
void DrawHist2D(
		TH2* hist,
		const std::string& titleX,
		const std::string& titleY,
		const std::string& titleZ,
		Bool_t logx,
		Bool_t logy,
		Bool_t logz,
		const std::string& drawOpt);



/* Draws up to 3 1D histograms. If hist == NULL than histogram will not be drawn.
 * @param hist1 Pointer to the first histogram.
 * @param hist2 Pointer to the second histogram.
 * @param hist3 Pointer to the third histogram.
 * @param legendLabel Legend label text.
 * @param xAxisLabel X axis label text.
 * @param yAxisLabel Y axis label text.
 * @param hist1label Label text for the first histogram.
 * @param hist2label Label text for the second histogram.
 * @param hist3label Label text for the third histogram.
 * @param logx If true than X axis is plotted in logarithmic scale.
 * @param logy If true than Y axis is plotted in logarithmic scale.
 * @param drawLegend If true than legend will be drawn.
 * @param x1 coordinates of the Legend in the current pad
 * @param y1 coordinates of the Legend in the current pad
 * @param x2 coordinates of the Legend in the current pad
 * @param y2 coordinates of the Legend in the current pad
 */
void DrawHist1D(
		TH1* hist1,
		TH1* hist2,
		TH1* hist3,
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
		Double_t y2);



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
		Bool_t logx,
		Bool_t logy,
		const std::string& drawOpt);

#endif
