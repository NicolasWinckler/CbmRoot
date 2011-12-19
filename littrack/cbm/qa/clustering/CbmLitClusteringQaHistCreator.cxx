/**
 * \file CbmLitClusteringQaHistCreator.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \brief Create histograms for clustering QA.
 * \date 2011
 */

#include "CbmLitClusteringQaHistCreator.h"
#include "qa/base/CbmLitHistManager.h"
#include "TH1F.h"
#include <assert.h>

CbmLitClusteringQaHistCreator::CbmLitClusteringQaHistCreator():
   fHM(NULL)
{

}

CbmLitClusteringQaHistCreator::~CbmLitClusteringQaHistCreator()
{

}

void CbmLitClusteringQaHistCreator::Create(
      CbmLitHistManager* histManager)
{
   assert(histManager != NULL);
   fHM = histManager;

   const int maxNofStations = 30;
   Create1FHist("hMvdNofHitsInStation", "Station number", "Number of hits", maxNofStations, 0, maxNofStations);
   Create1FHist("hStsNofHitsInStation", "Station number", "Number of hits", maxNofStations, 0, maxNofStations);
   Create1FHist("hTrdNofHitsInStation", "Station number", "Number of hits", maxNofStations, 0, maxNofStations);
   Create1FHist("hMuchNofHitsInStation", "Station number", "Number of hits", maxNofStations, 0, maxNofStations);
   Create1FHist("hTofNofHitsInStation", "Station number", "Number of hits", maxNofStations, 0, maxNofStations);

   int nofBinsC = 1000;
   float maxXC = 50000.;

   Create1FHist("hNofMvdPoints", "Points per event", "Counter", nofBinsC, 1., maxXC);
   Create1FHist("hNofStsPoints", "Points per event", "Counter", nofBinsC, 1., maxXC);
   Create1FHist("hNofRichPoints", "Points per event", "Counter", nofBinsC, 1., maxXC);
   Create1FHist("hNofTrdPoints", "Points per event", "Counter", nofBinsC, 1., maxXC);
   Create1FHist("hNofMuchPoints", "Points per event", "Counter", nofBinsC, 1., maxXC);
   Create1FHist("hNofTofPoints", "Points per event", "Counter", nofBinsC, 1., maxXC);

   Create1FHist("hNofMvdDigis", "Digis per event", "Counter", nofBinsC, 1., maxXC);
   Create1FHist("hNofStsDigis", "Digis per event", "Counter", nofBinsC, 1., maxXC);
   Create1FHist("hNofMuchDigis", "Digis per event", "Counter", nofBinsC, 1., maxXC);
   Create1FHist("hNofTrdDigis", "Digis per event", "Counter", nofBinsC, 1., maxXC);

   Create1FHist("hNofMvdClusters", "Clusters per event", "Counter", nofBinsC, 1., maxXC);
   Create1FHist("hNofStsClusters", "Clusters per event", "Counter", nofBinsC, 1., maxXC);
   Create1FHist("hNofMuchClusters", "Clusters per event", "Counter", nofBinsC, 1., maxXC);
   Create1FHist("hNofTrdClusters", "Clusters per event", "Counter", nofBinsC, 1., maxXC);

   Create1FHist("hNofMvdHits", "Hits per event", "Counter", nofBinsC, 1., maxXC);
   Create1FHist("hNofStsHits", "Hits per event", "Counter", nofBinsC, 1., maxXC);
   Create1FHist("hNofRichHits", "Hits per event", "Counter", nofBinsC, 1., maxXC);
   Create1FHist("hNofTrdHits", "Hits per event", "Counter", nofBinsC, 1., maxXC);
   Create1FHist("hNofMuchPixelHits", "Hits per event", "Counter", nofBinsC, 1., maxXC);
   Create1FHist("hNofMuchStrawHits", "Hits per event", "Counter", nofBinsC, 1., maxXC);
   Create1FHist("hNofTofHits", "Hits per event", "Counter", nofBinsC, 1., maxXC);

   // Histogram stores number of events
   //Create1FHist("hEventNo", "", "",1, 0, 1.);
}

void CbmLitClusteringQaHistCreator::Create1FHist(
      const string& name,
      const string& xTitle,
      const string& yTitle,
      int nofBins,
      float minBin,
      float maxBin)
{
   TH1F* h = new TH1F(name.c_str(),
         (name+";"+xTitle+";"+yTitle).c_str(), nofBins, minBin, maxBin);
   fHM->Add(name, h);
}
