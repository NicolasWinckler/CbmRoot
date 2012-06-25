/**
 * \file CbmLitClusteringQaDraw.cxx
 * \brief Draw histograms for clustering QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitClusteringQaDraw.h"
#include "qa/base/CbmLitHistManager.h"
#include "cbm/qa/draw/CbmLitDrawHist.h"
#include "utils/CbmLitUtils.h"

#include "TCanvas.h"

CbmLitClusteringQaDraw::CbmLitClusteringQaDraw():
   fHM(NULL),
   fOutputDir("")
{

}

CbmLitClusteringQaDraw::~CbmLitClusteringQaDraw()
{

}

void CbmLitClusteringQaDraw::Draw(
      CbmLitHistManager* histManager,
      const string& outputDir)
{
   assert(histManager != NULL);
   fHM = histManager;
   fOutputDir = outputDir;

   DrawHistogramsByPattern("hno_NofObjects_.*_Station");
}

void CbmLitClusteringQaDraw::DrawHistogramsByPattern(
      const string& histNamePattern)
{
	vector<TH1*> histos = fHM->H1Vector(histNamePattern);
	if (histos.size() == 0) return;
	Int_t nofHistos = histos.size();
	for (UInt_t iHist = 0; iHist < nofHistos; iHist++) {
		TH1* hist = histos[iHist];
		string canvasName = string("clustering_qa_") + hist->GetName();
		TCanvas* canvas = new TCanvas(canvasName.c_str(), canvasName.c_str(), 800, 500);
		DrawH1(hist, kLitLinear, kLitLinear);
		if (fOutputDir != "") lit::SaveCanvasAsImage(canvas, fOutputDir);
	}

}
