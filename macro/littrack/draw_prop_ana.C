#include <string>
#include <sstream>
#include <ostream>
#include <iomanip>

// residual (x,y,tx,ty,qp), pull (x,y,tx,ty,qp), resolution p %, chi-square
const int nofPar = 12;
const int nofLayers = 12;

// 0-propagation, 1-filter, 2-smoother
double sigma[3][nofLayers][nofPar];
double rms[3][nofLayers][nofPar];
TCanvas* canvas[3][nofLayers];

bool drawPropagation = false;
bool drawFilter = true;
bool drawSmoother = false;

//TString dir = "/home/d/andrey/events/newmuch/standard/10mu/mu/";
//TFile *file = new TFile(dir + "much.ana.root");

TString dir  = "/home/d/andrey/events/trd/monolithic/10e/e/";
TFile *file = new TFile(dir + "trd.ana.0000.root");

TString outDir = "./test/";

void draw_prop_ana()
{
	gStyle->SetOptStat("");
	gStyle->SetOptFit(0);
	gStyle->SetOptTitle(0);

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
			canvas[c][i] = new TCanvas(oss.str().c_str(),oss.str().c_str(),1200,1000);
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

	}
	stringstream oss1;
	oss1 << outDir << "layer" << var[v] << layer << ".gif";
	canvas[v][layer]->SaveAs(TString(oss1.str().c_str()));
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
	text.SetTextSize(0.1);
	text.DrawTextNDC(0.5, 0.83, oss1.str().c_str());
	oss2 << char(index+97) << ")";
	text.DrawTextNDC(0.8, 0.7, oss2.str().c_str());
}

