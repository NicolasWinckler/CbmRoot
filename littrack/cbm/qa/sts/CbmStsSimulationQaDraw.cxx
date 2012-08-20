/**
 * \file CbmStsSimulationQaDraw.cxx
 * \author Volker Friese <v.friese@gsi.de>
 * \date 2012
 */
#include "CbmStsSimulationQaDraw.h"
#include "CbmHistManager.h"
#include "CbmDrawHist.h"
#include "utils/CbmLitUtils.h"

#include "TCanvas.h"

CbmStsSimulationQaDraw::CbmStsSimulationQaDraw():
   fHM(NULL),
   fOutputDir("")
{

}

CbmStsSimulationQaDraw::~CbmStsSimulationQaDraw()
{

}

void CbmStsSimulationQaDraw::Draw(
      CbmHistManager* histManager,
      const string& outputDir)
{
   assert(histManager != NULL);
   fHM = histManager;
   fOutputDir = outputDir;

   DrawHistogramsByPattern("hss_NofObjects_.*_Station");
}

void CbmStsSimulationQaDraw::DrawHistogramsByPattern(
      const string& histNamePattern)
{
	vector<TH1*> histos = fHM->H1Vector(histNamePattern);
	if (histos.size() == 0) return;
	Int_t nofHistos = histos.size();
	for (UInt_t iHist = 0; iHist < nofHistos; iHist++) {
		TH1* hist = histos[iHist];
		string canvasName = string("sts_simulation_qa_") + hist->GetName();
		TCanvas* canvas = new TCanvas(canvasName.c_str(), canvasName.c_str(), 800, 500);
		DrawH1(hist, kLinear, kLinear);
		if (fOutputDir != "") lit::SaveCanvasAsImage(canvas, fOutputDir);
	}
}
