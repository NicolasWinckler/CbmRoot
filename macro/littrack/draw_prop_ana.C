#include <string>
#include <sstream>
#include <ostream>

const int nofPar = 11; // residual (x,y,tx,ty,qp) pull (x,y,tx,ty,qp) resolution p %
const int nofLayers = 27;

double sigma_layer[nofLayers][nofPar];
double rms_layer[nofLayers][nofPar];
double sigma_fit[2][nofPar];
double rms_fit[2][nofPar];
//double sigma_sr[nofLayers][2];
//double rms_sr[nofLayers][2];

TCanvas* c_layer[nofLayers];
TCanvas* c_fit[2];
//TCanvas* c_sr[2];

TString dir = "/d/cbm02/andrey/events/muchstraw/large/10mu/mu/";
//TString dir = "/d/cbm02/andrey/events/much/10stations/10mu/mu/";
TFile *file = new TFile(dir + "much.ana.root"); 

//TString dir  = "/d/cbm02/andrey/events/trd/standard/e";
//TFile *file = new TFile(dir + "/trd.ana.auau.25gev.e.centr.root"); 

TString outDir = "./test/";

void draw_prop_ana()
{
	gStyle->SetOptStat("");
	gStyle->SetOptFit(0);
	gStyle->SetOptTitle(0);

	for (int i = 0; i < nofLayers; i++) {
		stringstream oss;
		oss << "c_layer_" << i;
		c_layer[i] = new TCanvas(oss.str().c_str(),oss.str().c_str(),1200,1000);
		c_layer[i]->Divide(4, 3);
	}
	
	c_fit[0] = new TCanvas("c_fit_first","c_fit_first",1200,1000);
	c_fit[1] = new TCanvas("c_fit_last","c_fit_last",1200,1000);
	c_fit[0]->Divide(4, 3);
	c_fit[1]->Divide(4, 3);	
	
//	c_sr[0] = new TCanvas("c_sr_x","c_sr_x",1200,1000);
//	c_sr[1] = new TCanvas("c_sr_y","c_sr_y",1200,1000);
//	c_sr[0]->Divide(4, 3);
//	c_sr[1]->Divide(4, 3);	

	
	for (int i = 0; i < nofLayers; i++) layer_histos(i);

	fit_histos();
	
//	sr_histos();

	ofstream fout(outDir + "out.txt");
	print(fout);
	fout.close();
}

void layer_histos(
		int layer) 
{
	string names[] = { "h_resx", "h_resy", "h_restx", "h_resty", "h_resqp",
			"h_pullx", "h_pully", "h_pulltx", "h_pullty", "h_pullqp", "h_resp" };
	for (int i = 0; i < nofPar; i++) {
		c_layer[layer]->cd(i+1);
		stringstream oss;
		oss << names[i] << layer;
		TH1F* hist1 = (TH1F*) file->Get(oss.str().c_str());
		hist1->Fit("gaus");
		hist1->SetMaximum(hist1->GetMaximum() * 1.10);
		hist1->Draw();
		TF1 *fit1 = hist1->GetFunction("gaus");
		sigma_layer[layer][i] = fit1->GetParameter(2);
		rms_layer[layer][i] = hist1->GetRMS();
	
		draw_text(i, sigma_layer[layer][i], rms_layer[layer][i]);
	}
	stringstream oss1;
	oss1 << outDir << "layer_" << layer << ".gif"; 
	c_layer[layer]->SaveAs(TString(oss1.str().c_str()));
}

void fit_histos() 
{
	string names[] = { "h_resx", "h_resy", "h_restx", "h_resty", "h_resqp",
			"h_pullx", "h_pully", "h_pulltx", "h_pullty", "h_pullqp", "h_resp" };
	string layer[] = {"first", "last"};
	
	for (int ilayer=0; ilayer < 2; ilayer++) {
		for (int i = 0; i < nofPar; i++) {
			c_fit[ilayer]->cd(i+1);
			stringstream oss;
			oss << names[i] << "_" << layer[ilayer];
			TH1F* hist1 = (TH1F*) file->Get(oss.str().c_str());
			hist1->SetMaximum(hist1->GetMaximum() * 1.10);
			hist1->Fit("gaus");
			hist1->Draw();
			TF1 *fit1 = hist1->GetFunction("gaus");
			sigma_fit[ilayer][i] = fit1->GetParameter(2);
			rms_fit[ilayer][i] = hist1->GetRMS();   
			draw_text(i, sigma_fit[ilayer][i], rms_fit[ilayer][i]);
		}
		stringstream oss1;
		oss1 << outDir << "fit_" << layer[ilayer] << ".gif"; 
		c_fit[ilayer]->SaveAs(TString(oss1.str().c_str()));
	}
}

//void sr_histos()
//{
//	string coor[] = {"srx", "sry"};
//	
//	for (int icoord=0; icoord < 2; icoord++) {
//		for (int i = 0; i < nofLayers; i++) {
//			c_sr[icoord]->cd(i+1);
//			stringstream oss;
//			oss << "h_" << coor[icoord] << i;
//			TH1F* hist1 = (TH1F*) file->Get(oss.str().c_str());
//			hist1->Fit("gaus");
//			hist1->Draw();
//			TF1 *fit1 = hist1->GetFunction("gaus");
//			sigma_sr[i][icoord] = fit1->GetParameter(2);
//			rms_sr[i][icoord] = hist1->GetRMS();   
//		}
//		stringstream oss;
//		oss << coor[icoord]; 
//		c_sr[icoord]->SaveAs(TString(oss.str().c_str()));
//	}
//}

void print(
		ostream& out)
{
	out.precision(4);
	
//   out << "------------search region sigma (rms)---------------" << endl;
//   for (int i = 0; i < nofLayers; i++) {
//      out << "srx[" << i << "]=" << sigma_sr[i][0] << "(" << rms_sr[i][0] << ");   "
//           << "sry[" << i << "]=" << sigma_sr[i][1] << "(" << rms_sr[i][1] << ");" << endl;
//   }
   
   out << "------------resolution sigma (rms)---------------" << endl;
   out << "layer x   y   tx   ty   qp" << endl;
   for (int i = 0; i < nofLayers; i++) {
   	out << i << " ";
	for (int j = 0; j < 5; j++) {
		out << sigma_layer[i][j] << "(" << rms_layer[i][j] << ") ";
    }
	out << endl;
   }
   
   out << "------------pulls sigma (rms)---------------" << endl;
   out << "layer x   y   tx   ty   qp" << endl;
   for (int i = 0; i < nofLayers; i++) {
   	out << i << " ";
	for (int j = 5; j < 10; j++) {
      out << sigma_layer[i][j] << "(" << rms_layer[i][j] << ") ";
   	}
	out << endl;
   }

   out << "------------momentum residual sigma (rms)---------------" << endl;
   for (int i = 0; i < nofLayers; i++) {
      out << i << " " << sigma_layer[i][10] << "(" << rms_layer[i][10] << ")" << endl; 
   }  
      
   out << "------------residuals and pull on the last and first station---------" << endl;
   for (int i = 0; i < 2; i++) {
   	out << i << " ";
	for (int j = 0; j < nofPar; j++) {
      out << sigma_fit[i][j] << "(" << rms_fit[i][j] << ") ";
   	}
	out << endl;
   }
}

void draw_text(
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

