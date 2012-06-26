/**
 * \file CbmLitFitQaDraw.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */

#include "CbmLitFitQaDraw.h"
#include "../base/CbmLitHistManager.h"
#include "../draw/CbmLitDrawHist.h"
#include "utils/CbmLitUtils.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TF1.h"
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
      CbmLitHistManager* histManager,
      const string& outputDir)
{
   assert(histManager != NULL);
   fHM = histManager;
   fOutputDir = outputDir;

   DrawResidualAndPullHistograms("Sts");
   DrawResidualAndPullHistograms("Trd");
   DrawResidualAndPullHistograms("Much");
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
			   DrawH1(hist, kLitLinear, kLitLog);

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
