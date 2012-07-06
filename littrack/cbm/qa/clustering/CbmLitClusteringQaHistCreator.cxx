/**
 * \file CbmLitClusteringQaHistCreator.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \brief Create histograms for clustering QA.
 * \date 2011
 */

#include "CbmLitClusteringQaHistCreator.h"
#include "qa/base/CbmHistManager.h"
#include "TH1F.h"
#include <cassert>

CbmLitClusteringQaHistCreator::CbmLitClusteringQaHistCreator():
   fHM(NULL),
   fDet()
{

}

CbmLitClusteringQaHistCreator::~CbmLitClusteringQaHistCreator()
{

}

void CbmLitClusteringQaHistCreator::Create(
      CbmHistManager* histManager)
{
   assert(histManager != NULL);
   fHM = histManager;

   fDet.DetermineSetup();

   CreateNofObjectsHistograms(kMVD, "Mvd", "Station", "Station number");
   CreateNofObjectsHistograms(kSTS, "Sts", "Station", "Station number");
   CreateNofObjectsHistograms(kTRD, "Trd", "Station", "Station number");
   CreateNofObjectsHistograms(kMUCH, "Much", "Station", "Station number");
   CreateNofObjectsHistograms(kTOF, "Tof", "Station", "Station number");

   CreateNofObjectsHistograms(kMVD, "Mvd");
   CreateNofObjectsHistograms(kSTS, "Sts");
   CreateNofObjectsHistograms(kTRD, "Trd");
   CreateNofObjectsHistograms(kMUCH, "Much");
   CreateNofObjectsHistograms(kTOF, "Tof");
   CreateNofObjectsHistograms(kRICH, "Rich");

   // Histogram stores number of events
   CreateH1F("hen_EventNo_ClusteringQa", "", "", 1, 0, 1.);
}

void CbmLitClusteringQaHistCreator::CreateH1F(
      const string& name,
      const string& xTitle,
      const string& yTitle,
      Int_t nofBins,
      Double_t minBin,
      Double_t maxBin)
{
   TH1F* h = new TH1F(name.c_str(), (name + ";" + xTitle + ";" + yTitle).c_str(), nofBins, minBin, maxBin);
   fHM->Add(name, h);
}

void CbmLitClusteringQaHistCreator::CreateNofObjectsHistograms(
		DetectorId detId,
		const string& detName)
{
	assert(detId == kMVD || detId == kSTS || detId == kRICH || detId == kMUCH || detId == kTRD || detId == kTOF);
	Int_t nofBins = 100000;
	Double_t minX = 0.;
	Double_t maxX = 100000.;
	if (fDet.GetDet(detId)) {
	   string name = "hno_NofObjects_" + detName;
	   CreateH1F(name + "Points", "Points per event", "Counter", nofBins, minX, maxX);
	   CreateH1F(name + "Digis", "Digis per event", "Counter", nofBins, minX, maxX);
	   CreateH1F(name + "Clusters", "Clusters per event", "Counter", nofBins, minX, maxX);
	   if (detId == kMUCH) {
		   CreateH1F(name + "PixelHits", "Hits per event", "Counter", nofBins, minX, maxX);
		   CreateH1F(name + "StrawHits", "Hits per event", "Counter", nofBins, minX, maxX);
	   } else {
		   CreateH1F(name + "Hits", "Hits per event", "Counter", nofBins, minX, maxX);
	   }
	}
}

void CbmLitClusteringQaHistCreator::CreateNofObjectsHistograms(
		DetectorId detId,
		const string& detName,
		const string& parameter,
		const string& xTitle)
{
	assert(detId == kMVD || detId == kSTS || detId == kRICH || detId == kMUCH || detId == kTRD || detId == kTOF);
	Int_t nofBins = 100;
	Double_t minX = 0.;
	Double_t maxX = 100.;
	if (fDet.GetDet(detId)) {
	   string name = "hno_NofObjects_" + detName;
	   CreateH1F(name + "Points_" + parameter, xTitle, "Points per event", nofBins, minX, maxX);
	   CreateH1F(name + "Digis_" + parameter, xTitle, "Digis per event", nofBins, minX, maxX);
	   CreateH1F(name + "Clusters_" + parameter, xTitle, "Clusters per event", nofBins, minX, maxX);
	   CreateH1F(name + "Hits_" + parameter, xTitle, "Hits per event", nofBins, minX, maxX);
	}
}
