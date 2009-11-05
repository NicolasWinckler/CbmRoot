/** draw_prop_ana.C
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2007
 * @version 1.0
 * Macro draws histograms produced by CbmLitPropagationAnalysis class.
 **/

#include <string>
#include <sstream>
#include <ostream>
#include <iomanip>

/** Number of parameters.
 * [0-4] - residuals (x,y,tx,ty,qp)
 * [5-9] - pulls (x,y,tx,ty,qp)
 * [10] - resolution p %
 * [11] - chi-square
 */
const int nofPar = 12;

/** Number of detector planes. */
const int nofLayers = 25;

// Drawing options. If true than specified histograms are drawn.
bool drawPropagation = false;
bool drawFilter = true;
bool drawSmoother = false;

/* Arrays to store RMS and sigma values of the histogram fits.
 * First index: 0-propagation, 1-filter, 2-smoother.
 * Second index: plane number.
 * Third index: parameter number.
 */
double sigma[3][nofLayers][nofPar];
double rms[3][nofLayers][nofPar];

/* Canvases for drawing.
 * First index is parameter (0-propagation, 1-filter, 2-smoother).
 * Second index is a plane number.
 */
TCanvas* canvas[3][nofLayers];


// Input directory
TString dir = "/home/d/andrey/straw_10mu/";
// Input file with propagation analysis
TFile *file = new TFile(dir + "propagation.ana.0000.root");

//Output directory for images and fit results.
TString outDir = "./phd/";

void draw_prop_ana()
{
	gStyle->SetOptStat("");
	gStyle->SetOptFit(0);
	gStyle->SetOptTitle(0);

	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/style.C");
	style();

	ofstream fout(outDir + "out.txt");

	CreateCanvas();

	if (drawPropagation) {
		DrawHistos(0);
		Print(fout, 0);
	}
	if (drawFilter) {
		DrawHistos(1);
		Print(fout, 1);
	}
	if (drawSmoother) {
		DrawHistos(2);
		Print(fout, 2);
	}

//	DrawForPhd(0);
//	DrawForPhd(1);
//	DrawForPhd(2);

	fout.close();
}



void CreateCanvas()
{
	string var[] = {"c_propagation_", "c_filter_", "c_smoother_"};
	for (int c = 0; c < 3; c++) {
		if (c == 0 && !drawPropagation) continue;
		if (c == 1 && !drawFilter) continue;
		if (c == 2 && !drawSmoother) continue;
		for (int i = 0; i < nofLayers; i++) {
			stringstream oss;
			oss << var[c] << i;
			canvas[c][i] = new TCanvas(oss.str().c_str(),oss.str().c_str(),1200,800);
			canvas[c][i]->Divide(4, 3);
		}
	}
}

void DrawHistos(
		int v)
{
	for (int i = 0; i < nofLayers; i++) DrawHistos(i, v);
}

void DrawHistos(
		int layer,
		int v)
{
	string names[] = { "h_resx", "h_resy", "h_restx", "h_resty", "h_resqp",
			"h_pullx", "h_pully", "h_pulltx", "h_pullty", "h_pullqp", "h_resp", "h_chisq" };
	string var[] = {"_p_", "_f_", "_s_"};
	for (int i = 0; i < nofPar; i++) {
		canvas[v][layer]->cd(i+1);
		stringstream oss;
		oss << names[i] << var[v] << layer;
		TH1F* hist1 = (TH1F*) file->Get(oss.str().c_str());
		if (i != 11) hist1->Fit("gaus");
		hist1->SetMaximum(hist1->GetMaximum() * 1.20);
		hist1->GetXaxis()->SetLabelSize(0.075);
		hist1->GetXaxis()->SetNdivisions(505, kTRUE);
		hist1->GetYaxis()->SetLabelSize(0.075);

		hist1->Draw();
		TF1 *fit1 = hist1->GetFunction("gaus");
		if (fit1) {
			sigma[v][layer][i] = fit1->GetParameter(2);
			rms[v][layer][i] = hist1->GetRMS();
		}

		DrawText(i, sigma[v][layer][i], rms[v][layer][i]);
		gPad->SetLogy();

//		stringstream oss1;
//		oss1 << outDir << "layer" << var[v] << layer << "_" << names[i] << ".eps";
//		hist1->SaveAs(TString(oss1.str().c_str()));
//
//		stringstream oss2;
//		oss2 << outDir << "layer" << var[v] << layer << "_" << names[i] << ".svg";
//		hist1->SaveAs(TString(oss2.str().c_str()));

	}
	stringstream oss1;
	oss1 << outDir << "layer" << var[v] << layer << ".eps";
	canvas[v][layer]->SaveAs(TString(oss1.str().c_str()));

	stringstream oss2;
	oss2 << outDir << "layer" << var[v] << layer << ".svg";
	canvas[v][layer]->SaveAs(TString(oss2.str().c_str()));

	stringstream oss2;
	oss2 << outDir << "layer" << var[v] << layer << ".gif";
	canvas[v][layer]->SaveAs(TString(oss2.str().c_str()));
}

void Print(
		ostream& out,
		int v)
{
	string var[] = {"[propagation]", "[filter]", "[smoother]"};
	out << var[v] << endl;

    out.precision(3);
	out << "layer     resx             resy           restx             resty           resqp            pullx         pully        pulltx       pullty      pullqp      resmom        chisq" << endl;
	for (int i = 0; i < nofLayers; i++) {
		out << setw(3) << i << " ";
		for (int j = 0; j < nofPar; j++) {
			int w = (j < 5)? 7 : 5;
			out << setw(w) << sigma[v][i][j] << "(" << setw(w) << rms[v][i][j] << ") ";
		}
		out << endl;
	}
}

void DrawText(
		int index,
		double sigma,
		double rms)
{
	stringstream oss1, oss2;
	oss1.precision(3);
	oss1 << sigma << " / " << rms;
	TLatex text;
	text.SetTextAlign(21);
	text.SetTextSize(0.08); //0.1
	text.DrawTextNDC(0.5, 0.83, oss1.str().c_str());
	oss2 << char(index+97) << ")";
	text.DrawTextNDC(0.8, 0.7, oss2.str().c_str());
}


void DrawForPhd(
		int v)
{
	std::string var[] = {"_p_", "_f_", "_s_"};

	const Int_t NC = 3;
	TCanvas* canvas[NC];
	for(Int_t i = 0; i < NC; i++) {
		stringstream oss;
		oss << var[v] << i;
		canvas[i] = new TCanvas(oss.str().c_str(),oss.str().c_str(),1200, 500);
		canvas[i]->Divide(3, 1);
	}

	const Int_t NH = 3;
	TH1F* hist[NC][NH];
	Int_t stnum[NC] = {0, 6, 12};
	std::string hname[] = {"h_resx", "h_pullx", "h_resp"};
	std::string xtitles[] = {"residual X [cm]", "pull X", "relative momentum error [%]"};
	for (Int_t i = 0; i < NC; i++) {
		for (Int_t j = 0; j < NH; j++) {
			stringstream oss;
			oss<< hname[j] << var[v] << stnum[i];
			hist[i][j] = (TH1F*) file->Get(oss.str().c_str());
			std::cout << oss.str() << std::endl;
			if (!hist[i][j]) std::cout << "-E- no histogram " << oss.str() << std::endl;
		}
	}

	Double_t sigma[NC][NH], rms[NC][NH];

	for (int i = 0; i < NC; i++) {
		for (int j = 0; j < NH; j++) {
			canvas[i]->cd(j+1);
			hist[i][j]->Fit("gaus");
			hist[i][j]->SetMaximum(hist[i][j]->GetMaximum() * 2.5);
			hist[i][j]->GetXaxis()->SetLabelSize(0.055);
			hist[i][j]->GetXaxis()->SetNdivisions(505, kTRUE);
			hist[i][j]->GetYaxis()->SetLabelSize(0.055);
			hist[i][j]->GetXaxis()->SetTitleOffset(1.0);
			hist[i][j]->GetXaxis()->SetTitleSize(0.055);
			hist[i][j]->GetXaxis()->SetTitle(xtitles[j].c_str());

			gPad->SetBottomMargin(0.15);
			hist[i][j]->Draw();
			TF1 *fit1 = hist[i][j]->GetFunction("gaus");
			sigma[i][j] = fit1->GetParameter(2);
			rms[i][j] = hist[i][j]->GetRMS();

			DrawText(j, sigma[i][j], rms[i][j]);
			gPad->SetLogy();

	//		stringstream oss1;
	//		oss1 << outDir << "layer" << var[v] << layer << "_" << names[i] << ".eps";
	//		hist1->SaveAs(TString(oss1.str().c_str()));
	//
	//		stringstream oss2;
	//		oss2 << outDir << "layer" << var[v] << layer << "_" << names[i] << ".svg";
	//		hist1->SaveAs(TString(oss2.str().c_str()));

		}
		stringstream oss1;
		oss1 << outDir << "phd_layer" << var[v] << i << ".eps";
		canvas[i]->SaveAs(TString(oss1.str().c_str()));

		stringstream oss2;
		oss2 << outDir << "phd_layer" << var[v] << i << ".svg";
		canvas[i]->SaveAs(TString(oss2.str().c_str()));
	}
}
