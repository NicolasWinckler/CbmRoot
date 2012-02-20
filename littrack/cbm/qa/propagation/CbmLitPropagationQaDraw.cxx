/**
 * \file CbmLitPropagationQaDraw.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2012
 */
#include "CbmLitPropagationQaDraw.h"
#include "CbmLitPropagationQaHistCreator.h"
#include "../base/CbmLitHistManager.h"
#include "std/utils/CbmLitUtils.h"
#include "cbm/utils/CbmLitDrawHist.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TF1.h"
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
      CbmLitHistManager* histManager,
      const std::string& outputDir)
{
   assert(histManager != NULL);
   fHM = histManager;
   fOutputDir = outputDir;

   SetStyles();

   for (Int_t iAlgorithm = 0; iAlgorithm < 3; iAlgorithm++) {
      for (Int_t iPlane = 0; iPlane < fNofPlanes; iPlane++) {
         DrawHistos(iAlgorithm, iPlane);
      }
   }
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
      DrawH1(hist, kLitLinear, kLitLog);

      TF1* fit = hist->GetFunction("gaus");
      Double_t sigma = (NULL != fit) ? fit->GetParameter(2) : 0.;
      Double_t rms = hist->GetRMS();
      DrawHistSigmaRMS(sigma, rms);
   }
   SaveCanvasAsImage(canvas, fOutputDir);
}
