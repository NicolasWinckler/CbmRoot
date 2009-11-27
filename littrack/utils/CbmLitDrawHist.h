/** draw_hist.C
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 1.0
 * Some help functions for drawing 1D and 2D histograms.
 **/

#include "TH1.h"
#include "TH2.h"
#include "TPad.h"

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
		Int_t color = kBlack,
		Int_t lineWidth = 1,
		Int_t lineStyle = 1,
		Int_t markerSize = 1,
		Int_t markerStyle = kDot,
		Bool_t logx = false,
		Bool_t logy = false,
		const std::string& drawOpt = ""
		)
{
	Double_t textSize = 0.06;
	hist->GetXaxis()->SetTitle(titleX.c_str());
	hist->GetYaxis()->SetTitle(titleY.c_str());
	hist->SetLineColor(color);
	hist->SetLineWidth(lineWidth);
	hist->SetLineStyle(lineStyle);
	hist->SetMarkerColor(color);
	hist->SetMarkerSize(markerSize);
	hist->SetMarkerStyle(markerStyle);
	if (logx) gPad->SetLogx();
	if (logy) gPad->SetLogy();
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

/* Draws 1D histogram.
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
		Bool_t logx = false,
		Bool_t logy = false,
		Bool_t logz = false,
		const std::string& drawOpt = ""
		)
{
	hist->GetXaxis()->SetTitle(titleX.c_str());
	hist->GetYaxis()->SetTitle(titleY.c_str());
	hist->GetYaxis()->SetTitle(titleZ.c_str());
	if (logx) gPad->SetLogx();
	if (logy) gPad->SetLogy();
	if (logz) gPad->SetLogz();
	hist->Draw(drawOpt.c_str());
}

