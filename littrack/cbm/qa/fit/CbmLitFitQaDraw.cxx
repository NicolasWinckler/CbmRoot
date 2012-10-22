/**
 * \file CbmLitFitQaDraw.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */

#include "CbmLitFitQaDraw.h"
#include "CbmHistManager.h"
#include "CbmDrawHist.h"
#include "utils/CbmLitUtils.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TH1.h"
#include "TF1.h"
#include "TDirectory.h"
#include "TStyle.h"
#include <cassert>

CbmLitFitQaDraw::CbmLitFitQaDraw():
	fHM(NULL),
	fOutputDir("")
{

}

CbmLitFitQaDraw::~CbmLitFitQaDraw()
{

}

void CbmLitFitQaDraw::Draw(
      CbmHistManager* histManager,
      const string& outputDir)
{
   assert(histManager != NULL);
   fHM = histManager;
   fOutputDir = outputDir;

   DrawResidualAndPullHistograms("Sts");
   DrawResidualAndPullHistograms("Trd");
   DrawResidualAndPullHistograms("Much");

   DrawTrackParamsAtVertex();
}

void CbmLitFitQaDraw::DrawHistSigmaRMS(
   Double_t sigma,
   Double_t rms)
{
   string txt1 = lit::NumberToString<Double_t>(sigma, 2) + " / " + lit::NumberToString<Double_t>(rms, 2);
   TLatex text;
   text.SetTextAlign(21);
   text.SetTextSize(0.08);
   text.DrawTextNDC(0.5, 0.83, txt1.c_str());
}

void CbmLitFitQaDraw::DrawResidualAndPullHistograms(
      const string& detName)
{
   string parameterNames[] = { "X", "Y", "Tx", "Ty", "Qp" };
   string catNames[] = { "Res", "Pull", "WrongCov" };

   // [0] - for the first track parameter, [1] - for the last track parameter
   for (Int_t i = 0; i < 2; i++) {
	   string trackParamName = (i == 0) ? "FirstParam" : "LastParam";

	   string canvasName = "fit_qa_" + detName + "_" + trackParamName;
	   TCanvas* canvas = new TCanvas(canvasName.c_str(), canvasName.c_str(), 1400, 900);
	   canvas->Divide(5, 3);

	   // [0] - "Res", [1] - "Pull", [2] - "WrongCov"
	   for (Int_t iCat = 0; iCat < 3; iCat++) {
		   for (Int_t iPar = 0; iPar < 5; iPar++) {
			   string histName = "htf_" + detName + "_" + trackParamName + "_" + catNames[iCat] + "_" + parameterNames[iPar];
			   if (!fHM->Exists(histName)) return;
			   Int_t histId = iCat * 5 + iPar;
			   canvas->cd(histId + 1);
			   TH1* hist = fHM->H1(histName);
			   DrawH1(hist, kLinear, kLog);

			   if (histId < 10) { // Fit only residual and pull histograms
				   hist->Fit("gaus");
				   hist->SetMaximum(hist->GetMaximum() * 1.50);
				   TF1* fit = hist->GetFunction("gaus");
				   Double_t sigma = (NULL != fit) ? fit->GetParameter(2) : 0.;
				   Double_t rms = hist->GetRMS();
				   DrawHistSigmaRMS(sigma, rms);
			   }
		   }
	   }
	   lit::SaveCanvasAsImage(canvas, fOutputDir);
   }
}

void CbmLitFitQaDraw::DrawTrackParamsAtVertex()
{
   TCanvas* canvas1 = new TCanvas("fit_qa_momentum", "fit_qa_momentum", 1200, 1200);
   canvas1->Divide(2,2);
   canvas1->cd(1);

   canvas1->cd(1);
   DrawH2((TH2*)fHM->H1("htf_MomRes_Mom"));
   gPad->SetGridx(true);
   gPad->SetGridy(true);

   canvas1->cd(2);
   TH1* projY = ((TH2*)(fHM->H1("htf_MomRes_Mom")))->ProjectionY("htf_MomRes_Mom_ProjectionY");
   DrawH1(projY, kLinear, kLinear);
   projY->SetStats(true);
   projY->Fit("gaus");
   projY->SetMaximum(projY->GetMaximum() * 1.25);
   gPad->SetGridx(true);
   gPad->SetGridy(true);

   canvas1->cd(3);
   ((TH2*)(fHM->H1("htf_MomRes_Mom")))->FitSlicesY();
   TH1* momslice = (TH1*) gDirectory->Get("htf_MomRes_Mom_2");
   momslice->GetXaxis()->SetTitle("P [GeV/c]");
   momslice->GetYaxis()->SetTitle("dP/P, #sigma [%]");
   momslice->SetMinimum(0.);
   momslice->SetMaximum(3.);
   DrawH1(momslice, kLinear, kLinear);
   gPad->SetGridx(true);
   gPad->SetGridy(true);

   canvas1->cd(4);
   TH2* hMomres = (TH2*) fHM->H1("htf_MomRes_Mom");
   Int_t nBins = hMomres->GetNbinsX();
   TH1* momResRms = hMomres->ProjectionX();
   for (Int_t i = 1; i < nBins; i++){
      TH1* projY = hMomres->ProjectionY("_py", i, i);
      Double_t rms = projY->GetRMS();
      momResRms->SetBinContent(i, rms);
      momResRms->SetBinError(i, momslice->GetBinError(i));
   }
   momResRms->GetXaxis()->SetTitle("P [GeV/c]");
   momResRms->GetYaxis()->SetTitle("dP/P, RMS [%]");
   momResRms->SetMinimum(0.);
   momResRms->SetMaximum(3.);
   DrawH1(momResRms, kLinear, kLinear, "P");
   gPad->SetGridx(true);
   gPad->SetGridy(true);

   lit::SaveCanvasAsImage(canvas1, fOutputDir);

   TCanvas* canvas2 = new TCanvas("fit_qa_chi_primary", "fit_qa_chi_primary", 600, 600);
   canvas2->cd(1);
   TH1* hChiprim = fHM->H1("htf_ChiPrimary");
   hChiprim->Scale(1. / hChiprim->Integral());
   DrawH1(hChiprim, kLinear, kLog);
   gPad->SetGridx(true);
   gPad->SetGridy(true);

   lit::SaveCanvasAsImage(canvas2, fOutputDir);
}
