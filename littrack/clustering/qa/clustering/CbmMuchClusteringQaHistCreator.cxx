/**
 * \file CbmMuchClusteringQaHistCreator.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \brief Create histograms for clustering QA.
 * \date 2011
 */

#include "CbmMuchClusteringQaHistCreator.h"
#include "CbmHistManager.h"
#include "TH1F.h"
#include <cassert>

#include "CbmMuchGeoScheme.h"
#include "CbmMuchStation.h"

CbmMuchClusteringQaHistCreator::CbmMuchClusteringQaHistCreator():
   fHM(NULL),
   fDet(),
   fMuchGeoScheme()
{

}

CbmMuchClusteringQaHistCreator::~CbmMuchClusteringQaHistCreator()
{

}

void CbmMuchClusteringQaHistCreator::Create(
      CbmHistManager* histManager,
      CbmMuchGeoScheme* muchGeoScheme)
{
	//std::cout<<"--Hist creator Create\n";
	fMuchGeoScheme = muchGeoScheme;

	//std::cout<<"--Hist manager create\n";
	assert(histManager != NULL);
   fHM = histManager;

   //std::cout<<"--Detector determine setup\n";
   fDet.DetermineSetup();

   std::cout<<"--Creation of old histograms\n";
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

   //std::cout<<"--Creation of new histograms\n";
   Int_t nofLayersInMuch = 0;
   Int_t nofMuchStations = fMuchGeoScheme->GetNStations();
   //std::cout<<"---Start creation\n";
   for(Int_t iStation = 0; iStation < nofMuchStations; iStation++)
   {
	   CbmMuchStation* muchStation = (CbmMuchStation*)fMuchGeoScheme->GetStation(iStation);
	   nofLayersInMuch += muchStation->GetNLayers();
	   //std::cout<<"---Much station created\n";
	   for(Int_t iLayer = 0; iLayer < muchStation->GetNLayers(); iLayer++)
	   {
		   //const char* St = (const char*)(iStation);
		   char St[8]={0};
		   char L[8]={0};
		   sprintf(St, "%d", iStation);
		   sprintf(L, "%d", iLayer);
		   //std::cout<<"----Layer: "<<iLayer<<"; St: "<<St<<"\n";
		   CreateErrorsByRadiusHistograms(kMUCH, "Much", St, L, "...", 102, muchStation->GetRmax());
		   CreateQualClToPointByRadiusHistograms(kMUCH, "Much", St, L, "...", 102, muchStation->GetRmax());
		   CreateQualDigisToClByRadiusHistograms(kMUCH, "Much", St, L, "...", 102, muchStation->GetRmax());
		   //std::cout<<"----H working\n";
	   }
   }
   //std::cout<<"--New histograms by radius created\n";
   CreateErrorsByLayersHistograms(kMUCH, "Much", "...", nofLayersInMuch);
   CreateEfficiencyByLayersHistograms(kMUCH, "Much", "...", nofLayersInMuch);
   CreateQualClToPointByLayersHistograms(kMUCH, "Much", "...", nofLayersInMuch);
   CreateQualDigisToClByLayersHistograms(kMUCH, "Much", "...", nofLayersInMuch);
   //std::cout<<"--New histograms by layers created\n";
}

void CbmMuchClusteringQaHistCreator::CreateH1F(
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

void CbmMuchClusteringQaHistCreator::CreateNofObjectsHistograms(
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

void CbmMuchClusteringQaHistCreator::CreateNofObjectsHistograms(
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

void CbmMuchClusteringQaHistCreator::CreateErrorsByRadiusHistograms(
		DetectorId detId,
	   	const string& detName,
	   	const string& nStation,
	   	const string& nLayer,
	   	const string& xTitle,
   		Int_t nBins,
   		Double_t rMax)
{
	assert(detId == kMVD || detId == kSTS || detId == kRICH || detId == kMUCH || detId == kTRD || detId == kTOF);
	Int_t nofBins = nBins;
	Double_t minX = 0.;
	Double_t maxX = rMax;
	if (fDet.GetDet(detId))
	{
		string name = "hno_ErrorsByRadius_" + detName;
		CreateH1F(name + "_Station_" + nStation + "_Layer_" + nLayer, xTitle, "Errors by radius", nofBins, minX, maxX);
	}
}

void CbmMuchClusteringQaHistCreator::CreateQualClToPointByRadiusHistograms(
   		DetectorId detId,
   	   	const string& detName,
   	   	const string& nStation,
   	   	const string& nLayer,
   	   	const string& xTitle,
   	   	Int_t nBins,
   	   	Double_t rMax)
{
	assert(detId == kMVD || detId == kSTS || detId == kRICH || detId == kMUCH || detId == kTRD || detId == kTOF);
	Int_t nofBins = nBins;
	Double_t minX = 0.;
	Double_t maxX = rMax;
	if (fDet.GetDet(detId))
	{
		string name = "hno_QualClToPointByRadius_" + detName;
		CreateH1F(name + "_Station_" + nStation + "_Layer_" + nLayer, xTitle, "Quality by radius", nofBins, minX, maxX);
	}
}

void CbmMuchClusteringQaHistCreator::CreateQualDigisToClByRadiusHistograms(
   		DetectorId detId,
   	   	const string& detName,
   	   	const string& nStation,
   	   	const string& nLayer,
   	   	const string& xTitle,
   	   	Int_t nBins,
   	   	Double_t rMax)
{
	assert(detId == kMVD || detId == kSTS || detId == kRICH || detId == kMUCH || detId == kTRD || detId == kTOF);
	Int_t nofBins = nBins;
	Double_t minX = 0.;
	Double_t maxX = rMax;
	if (fDet.GetDet(detId))
	{
		string name = "hno_QualDigisToClByRadius_" + detName;
		CreateH1F(name + "_Station_" + nStation + "_Layer_" + nLayer, xTitle, "Quality by radius", nofBins, minX, maxX);
	}
}

void CbmMuchClusteringQaHistCreator::CreateErrorsByLayersHistograms(
		DetectorId detId,
	   	const string& detName,
	   	const string& xTitle,
	   	Int_t nLayers)
{
	assert(detId == kMVD || detId == kSTS || detId == kRICH || detId == kMUCH || detId == kTRD || detId == kTOF);
	Int_t nofBins = nLayers + 2;
	Double_t minX = 0.;
	Double_t maxX = (Double_t)nLayers;
	if (fDet.GetDet(detId))
	{
		string name = "hno_ErrorsByLayers_" + detName;
		CreateH1F(name, xTitle, "Errors by layers", nofBins, minX, maxX);
	}
}

void CbmMuchClusteringQaHistCreator::CreateEfficiencyByLayersHistograms(
		DetectorId detId,
	   	const string& detName,
	   	const string& xTitle,
	   	Int_t nLayers)
{
	assert(detId == kMVD || detId == kSTS || detId == kRICH || detId == kMUCH || detId == kTRD || detId == kTOF);
	Int_t nofBins = nLayers + 2;
	Double_t minX = 0.;
	Double_t maxX = (Double_t)nLayers;
	if (fDet.GetDet(detId))
	{
		string name = "hno_EfficiencyByLayers_" + detName;
		CreateH1F(name, xTitle, "Efficiency by layers", nofBins, minX, maxX);
	}
}

void CbmMuchClusteringQaHistCreator::CreateQualClToPointByLayersHistograms(
		DetectorId detId,
	   	const string& detName,
	   	const string& xTitle,
	   	Int_t nLayers)
{
	assert(detId == kMVD || detId == kSTS || detId == kRICH || detId == kMUCH || detId == kTRD || detId == kTOF);
	Int_t nofBins = nLayers + 2;
	Double_t minX = 0.;
	Double_t maxX = (Double_t)nLayers;
	if (fDet.GetDet(detId))
	{
		string name = "hno_QualityClToPointByLayers_" + detName;
		CreateH1F(name, xTitle, "Quality by layers", nofBins, minX, maxX);
	}
}

void CbmMuchClusteringQaHistCreator::CreateQualDigisToClByLayersHistograms(
		DetectorId detId,
	   	const string& detName,
	   	const string& xTitle,
	   	Int_t nLayers)
{
	assert(detId == kMVD || detId == kSTS || detId == kRICH || detId == kMUCH || detId == kTRD || detId == kTOF);
	Int_t nofBins = nLayers;
	Double_t minX = 0.;
	Double_t maxX = (Double_t)nLayers;
	if (fDet.GetDet(detId))
	{
		string name = "hno_QualityDigisToClByLayers_" + detName;
		CreateH1F(name, xTitle, "Quality by layers", nofBins, minX, maxX);
	}
}
