/**
 * \file CbmLitPropagationQaDraw.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2012
 */
#include "CbmLitPropagationQaDraw.h"
#include "CbmLitPropagationQaHistCreator.h"
#include "../base/CbmHistManager.h"
#include "std/utils/CbmLitUtils.h"
#include "CbmDrawHist.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TF1.h"
#include "TLatex.h"
#include <cassert>
using lit::SaveCanvasAsImage;
using lit::ToString;

CbmLitPropagationQaDraw::CbmLitPropagationQaDraw():
   fHM(NULL),
   fOutputDir(""),
   fNofPlanes(0)
{

}

CbmLitPropagationQaDraw::~CbmLitPropagationQaDraw()
{

}

void CbmLitPropagationQaDraw::Draw(
      CbmHistManager* histManager,
      const std::string& outputDir)
{
   assert(histManager != NULL);
   fHM = histManager;
   fOutputDir = outputDir;

   SetDefaultDrawStyle();

   for (Int_t iAlgorithm = 0; iAlgorithm < 3; iAlgorithm++) {
      for (Int_t iPlane = 0; iPlane < fNofPlanes; iPlane++) {
         DrawHistos(iAlgorithm, iPlane);
      }
   }
}

void CbmLitPropagationQaDraw::DrawHistSigmaRMS(
   Double_t sigma,
   Double_t rms)
{
   string txt1 = lit::NumberToString<Double_t>(sigma, 2) + " / " + lit::NumberToString<Double_t>(rms, 2);
   TLatex text;
   text.SetTextAlign(21);
   text.SetTextSize(0.08);
   text.DrawTextNDC(0.5, 0.83, txt1.c_str());
}

void CbmLitPropagationQaDraw::DrawHistos(
   Int_t algorithmIndex,
   Int_t planeIndex)
{
   string algorithms[] = { "propagator", "fitter", "smoother" };
   string canvasName = "propagation_qa_" + algorithms[algorithmIndex] + "_" + ToString<Int_t>(planeIndex);
   TCanvas* canvas = new TCanvas(canvasName.c_str(), canvasName.c_str(), 1200, 600);
   canvas->Divide(5, 2);

   for (Int_t iParam = 0; iParam < CbmLitPropagationQaHistCreator::NofQaParameters; iParam++) {
      canvas->cd(iParam + 1);
      TH1* hist = fHM->H1(CbmLitPropagationQaHistCreator::HistName(iParam, algorithmIndex, planeIndex));
      hist->Fit("gaus");
      hist->SetMaximum(hist->GetMaximum() * 1.50);
      DrawH1(hist, kLinear, kLog);

      TF1* fit = hist->GetFunction("gaus");
      Double_t sigma = (NULL != fit) ? fit->GetParameter(2) : 0.;
      Double_t rms = hist->GetRMS();
      DrawHistSigmaRMS(sigma, rms);
   }
   SaveCanvasAsImage(canvas, fOutputDir);
}
