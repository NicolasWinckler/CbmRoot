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
      TH1* hist)
{
   TCanvas* canvas = new TCanvas(name.c_str(), name.c_str(), 800, 400);
   DrawH1(hist, kLitLinear, kLitLinear);
   if (fOutputDir != "") lit::SaveCanvasAsImage(canvas, fOutputDir);
}

void CbmLitClusteringQaDraw::DrawHitsStationHistos()
{
   DrawHitsStationHisto("clustering_qa_mvd_hits_station", fHM->H1("hMvdNofHitsInStation"));
   DrawHitsStationHisto("clustering_qa_sts_hits_station", fHM->H1("hStsNofHitsInStation"));
   DrawHitsStationHisto("clustering_qa_trd_hits_station", fHM->H1("hTrdNofHitsInStation"));
   DrawHitsStationHisto("clustering_qa_much_hits_station", fHM->H1("hMuchNofHitsInStation"));
   DrawHitsStationHisto("clustering_qa_tof_hits_station", fHM->H1("hTofNofHitsInStation"));
}
