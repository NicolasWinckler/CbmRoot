/**
 * \file CbmLitClusteringQaDraw.cxx
 * \brief Draw histograms for clustering QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitClusteringQaDraw.h"
#include "qa/base/CbmLitHistManager.h"
#include "utils/CbmLitDrawHist.h"
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

   DrawHitsStationHistos();
}

void CbmLitClusteringQaDraw::DrawHitsStationHisto(
      const string& name,
      TH1F* hist)
{
   TCanvas* canvas = new TCanvas(name.c_str(), name.c_str(), 1200, 600);
   DrawHist1D(hist, kLitLinear, kLitLinear, "HIST TEXT0");
   if (fOutputDir != "") lit::SaveCanvasAsImage(canvas, fOutputDir);
}

void CbmLitClusteringQaDraw::DrawHitsStationHistos()
{
   DrawHitsStationHisto("clustering_qa_mvd_hits_station", fHM->H1F("hMvdNofHitsInStation"));
   DrawHitsStationHisto("clustering_qa_sts_hits_station", fHM->H1F("hStsNofHitsInStation"));
   DrawHitsStationHisto("clustering_qa_trd_hits_station", fHM->H1F("hTrdNofHitsInStation"));
   DrawHitsStationHisto("clustering_qa_much_hits_station", fHM->H1F("hMuchNofHitsInStation"));
   DrawHitsStationHisto("clustering_qa_tof_hits_station", fHM->H1F("hTofNofHitsInStation"));
}
