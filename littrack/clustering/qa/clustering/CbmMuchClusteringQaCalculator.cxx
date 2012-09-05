/**
 * \file CbmMuchClusteringQaCalculator.cxx
 * \brief Performance calculator for clustering QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */

#include "CbmMuchClusteringQaCalculator.h"
#include "CbmHistManager.h"
#include "CbmHit.h"
#include "CbmBaseHit.h"
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "TH1.h"
#include <assert.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <cassert>
#include <math.h>

#include "CbmMuchGeoScheme.h"
#include "CbmMuchStation.h"
#include "CbmMuchLayer.h"
#include "CbmMuchLayerSide.h"
#include "CbmMuchModule.h"
#include "CbmMuchModuleGem.h"
#include "CbmMuchPad.h"
#include "CbmMuchDigiMatch.h"
#include "CbmMuchCluster.h"
#include "CbmMuchDigi.h"
#include "CbmMuchPoint.h"
#include "CbmMuchPixelHit.h"

CbmMuchClusteringQaCalculator::CbmMuchClusteringQaCalculator(
      CbmHistManager* histManager):
      fMvdPoints(NULL),
      fMvdDigis(NULL),
      fMvdClusters(NULL),
      fMvdHits(NULL),
      fStsPoints(NULL),
      fStsDigis(NULL),
      fStsClusters(NULL),
      fStsHits(NULL),
      fRichHits(NULL),
      fRichPoints(NULL),
      fMuchPoints(NULL),
      fMuchHits(NULL),
      fMuchDigis(NULL),
      fMuchDigiMatches(NULL),
      fMuchClusters(NULL),
      fMuchPixelHits(NULL),
      fMuchStrawHits(NULL),
      fTrdPoints(NULL),
      fTrdDigis(NULL),
      fTrdClusters(NULL),
      fTrdHits(NULL),
      fTofPoints(NULL),
      fTofHits(NULL),
      fHM(histManager),
      fClustersRelations(),
      fNofLayers(),
      fNofPoints(),
      fMuchGeoScheme(),
      fDigisByPoint(),
      fNofDigis(),
      fNofClusters(),
      fAccuracyArray(),
      fNofHits()
{

}

CbmMuchClusteringQaCalculator::~CbmMuchClusteringQaCalculator()
{

}

void CbmMuchClusteringQaCalculator::Init()
{
	/*fMuchGeoScheme = CbmMuchGeoScheme::Instance();
	TString muchDigiFile = "/u/gkozlov/cbm/trunk/cbmroot/parameters/much/much_v11a.digi.root";
	fMuchGeoScheme->Init(muchDigiFile);*/
	/*fGeoScheme = CbmMuchGeoScheme::Instance();
	TString muchDigiFile = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters/much/much_v11a.digi.root");
	fGeoScheme->Init(muchDigiFile);*/
	ReadDataBranches();
}

void CbmMuchClusteringQaCalculator::Exec()
{
   // Increase event counter
   fHM->H1("hen_EventNo_ClusteringQa")->Fill(0.5);
   std::cout << "CbmMuchClusteringQaCalculator::Exec: event=" << fHM->H1("hen_EventNo_ClusteringQa")->GetEntries() << std::endl;

   //---
   if(fAccuracyArray) delete [] fAccuracyArray;
   if(fClustersRelations) delete [] fClustersRelations;
   fNofClusters = fMuchClusters->GetEntriesFast();
   fNofHits = fMuchPixelHits->GetEntriesFast();
   fNofDigis = fMuchDigis->GetEntriesFast();
   fNofPoints = fMuchPoints->GetEntriesFast();
   /*fAccuracyArray = new AccuracyStruct[fNofClusters];
   fClustersRelations = new ClusterInformation[fNofClusters];
   for(Int_t iCl = 0; iCl < fNofClusters; iCl++)
   {
	   fAccuracyArray[iCl].fErrorX = 0;
	   fAccuracyArray[iCl].fErrorY = 0;
	   fAccuracyArray[iCl].fErrorXY = 0;
	   fAccuracyArray[iCl].fHit = 0;
	   fAccuracyArray[iCl].fPoint = 0;
	   fClustersRelations[iCl].fStation = 0;
	   fClustersRelations[iCl].fLeyer = 0;
	   fClustersRelations[iCl].fSide = 0;
	   fClustersRelations[iCl].fNofPoints = 0;
	   fClustersRelations[iCl].fBestPoint = 0;
	   fClustersRelations[iCl].fPointsInCluster.clear();
	   fClustersRelations[iCl].fDigisByPoint.clear();
	   fClustersRelations[iCl].fClusterToPointRelations.clear();
	   fClustersRelations[iCl].fNDigisToClusterRelations.clear();
   }
   fDigisByPoint = new Int_t[fNofPoints];*/

   //---------
   /*for(Int_t iCl = 0; iCl < fNofClusters; iCl++)
   {
	   CbmMuchCluster* cluster = (CbmMuchCluster*) fMuchClusters->At(iCl);
	   std::cout<<"Cl: "<<iCl<<"; Digi: "<<cluster->GetDigiIndex(0)<<"; nofDigis: "<<
			   fNofDigis<<"\n";
	   CbmMuchPixelHit* hit = (CbmMuchPixelHit*)fMuchPixelHits->At(iCl);
	   std::cout<<"hitX: "<<hit->GetX()<<"; hitY: "<<hit->GetY()<<"\n";
	   if(cluster->GetDigiIndex(0) > fNofDigis)
	   {
		   std::cout<<"--------------------------------\n";
		   for(Int_t iDigi = 0; iDigi < cluster->GetNDigis(); iDigi++)
		   {
			   std::cout<<"---Digi: "<<cluster->GetDigiIndex(iDigi)<<"\n";
		   }
		   CbmMuchPixelHit* hit = (CbmMuchPixelHit*)fMuchPixelHits->At(iCl);
		   std::cout<<"hitX: "<<hit->GetX()<<"; hitY: "<<hit->GetY()<<"\n";
		   std::cout<<"--------------------------------\n";
	   }
   }*/
   //---------

   CalculateClustersRelations();
   CalculateAccuracy();

   std::cout<<"--Finished CalculateAccuracy()\n";
   FillErrorsAndQualityByRadiusHistograms();
   std::cout<<"--Finished FillErrorsAndQualityByRadiusHistograms()\n";
   FillEfficiencyByLayerHistograms();
   std::cout<<"--Finished FillEfficiencyByLayerHistograms()\n";
   //---

   ProcessHits();
   IncreaseCounters();
}

void CbmMuchClusteringQaCalculator::Finish()
{
	std::cout<<"--Calculator Finish start\n";
	Int_t nofEvents = fHM->H1("hen_EventNo_ClusteringQa")->GetEntries();
	std::cout<<"--nofEvents loaded\n";
	//???
	fHM->ScaleByPattern("hno_NofObjects_.*_Station", 1. / nofEvents);
	fHM->ScaleByPattern("hno_ErrorsByRadius_Much_Station_.*", 1. / nofEvents);
	fHM->ScaleByPattern("hno_QualClToPointByRadius_Much_Station_.*", 1. / nofEvents);
	fHM->ScaleByPattern("hno_QualDigisToClByRadius_Much_Station_.*", 1. / nofEvents);
	std::cout<<"--Histograms normalized\n";
	//fHM->ShrinkEmptyBinsByPattern("hno_NofObjects_.*_Station");
	std::cout<<"--Calculator Finish finished\n";
}

void CbmMuchClusteringQaCalculator::AddMuchGeoScheme(CbmMuchGeoScheme* geoScheme)
{
	fMuchGeoScheme = geoScheme;
}

void CbmMuchClusteringQaCalculator::ReadDataBranches()
{
   FairRootManager* ioman = FairRootManager::Instance();
   assert(ioman != NULL);

   fMvdPoints = (TClonesArray*) ioman->GetObject("MvdPoint");
   fMvdDigis = (TClonesArray*) ioman->GetObject("MvdDigi");
   fMvdClusters = (TClonesArray*) ioman->GetObject("MvdCluster");
   fMvdHits = (TClonesArray*) ioman->GetObject("MvdHit");

   fStsPoints = (TClonesArray*) ioman->GetObject("StsPoint");
   fStsDigis = (TClonesArray*) ioman->GetObject("StsDigi");
   fStsClusters = (TClonesArray*) ioman->GetObject("StsCluster");
   fStsHits = (TClonesArray*) ioman->GetObject("StsHit");

   fRichHits = (TClonesArray*) ioman->GetObject("RichHit");
   fRichPoints = (TClonesArray*) ioman->GetObject("RichPoint");

   fMuchPoints = (TClonesArray*) ioman->GetObject("MuchPoint");
   //fMuchHits = (TClonesArray*) ioman->GetObject("MuchHit");
   fMuchDigis = (TClonesArray*) ioman->GetObject("MuchDigi");
   fMuchDigiMatches = (TClonesArray*) ioman->GetObject("MuchDigiMatch");
   fMuchClusters = (TClonesArray*) ioman->GetObject("MuchCluster");
   fMuchPixelHits = (TClonesArray*) ioman->GetObject("MuchPixelHit");
   fMuchStrawHits = (TClonesArray*) ioman->GetObject("MuchStrawHit");

   fTrdPoints = (TClonesArray*) ioman->GetObject("TrdPoint");
   fTrdDigis = (TClonesArray*) ioman->GetObject("TrdDigi");
   fTrdClusters = (TClonesArray*) ioman->GetObject("TrdCluster");
   fTrdHits = (TClonesArray*) ioman->GetObject("TrdHit");

   fTofPoints = (TClonesArray*) ioman->GetObject("TofPoint");
   fTofHits = (TClonesArray*) ioman->GetObject("TofHit");
}

void CbmMuchClusteringQaCalculator::ProcessHits()
{
   if (NULL != fMvdHits && fHM->Exists("hno_NofObjects_MvdHits_Station")) {
      for (Int_t i = 0; i < fMvdHits->GetEntriesFast(); i++) {
         const CbmHit* hit = static_cast<const CbmHit*>(fMvdHits->At(i));
         fHM->H1("hno_NofObjects_MvdHits_Station")->Fill(hit->GetStationNr());
      }
   }
   if (NULL != fStsHits && fHM->Exists("hno_NofObjects_StsHits_Station")) {
      for (Int_t i = 0; i < fStsHits->GetEntriesFast(); i++) {
         const CbmHit* hit = static_cast<const CbmHit*>(fStsHits->At(i));
         fHM->H1("hno_NofObjects_StsHits_Station")->Fill(hit->GetStationNr());
      }
   }
   if (NULL != fTrdHits && fHM->Exists("hno_NofObjects_TrdHits_Station")) {
      for (Int_t i = 0; i < fTrdHits->GetEntriesFast(); i++) {
         const CbmBaseHit* hit = static_cast<const CbmBaseHit*>(fTrdHits->At(i));
         fHM->H1("hno_NofObjects_TrdHits_Station")->Fill(hit->GetPlaneId());
      }
   }
   if (NULL != fMuchPixelHits && fHM->Exists("hno_NofObjects_MuchHits_Station")) {
      for (Int_t i = 0; i < fMuchPixelHits->GetEntriesFast(); i++) {
         const CbmBaseHit* hit = static_cast<const CbmBaseHit*>(fMuchPixelHits->At(i));
         fHM->H1("hno_NofObjects_MuchHits_Station")->Fill(hit->GetPlaneId());
      }
   }
   if (NULL != fMuchStrawHits && fHM->Exists("hno_NofObjects_MuchHits_Station")) {
      for (Int_t i = 0; i < fMuchStrawHits->GetEntriesFast(); i++) {
         const CbmBaseHit* hit = static_cast<const CbmBaseHit*>(fMuchStrawHits->At(i));
         fHM->H1("hno_NofObjects_MuchHits_Station")->Fill(hit->GetPlaneId());
      }
   }
   if (NULL != fTofHits && fHM->Exists("hno_NofObjects_TofHits_Station")) {
      for (Int_t i = 0; i < fTofHits->GetEntriesFast(); i++) {
         const CbmBaseHit* hit = static_cast<const CbmBaseHit*>(fTofHits->At(i));
         fHM->H1("hno_NofObjects_TofHits_Station")->Fill(hit->GetPlaneId());
      }
   }

   if(NULL != fMuchPixelHits && fHM->Exists("hno_EfficiencyByLayers_Much"))
   {
	   //
   }
}

void CbmMuchClusteringQaCalculator::IncreaseCounters()
{
   if (NULL != fMvdPoints && fHM->Exists("hno_NofObjects_MvdPoints")) fHM->H1("hno_NofObjects_MvdPoints")->Fill(fMvdPoints->GetEntriesFast());
   if (NULL != fMvdDigis && fHM->Exists("hno_NofObjects_MvdDigis")) fHM->H1("hno_NofObjects_MvdDigis")->Fill(fMvdDigis->GetEntriesFast());
   if (NULL != fMvdClusters && fHM->Exists("hno_NofObjects_MvdClusters")) fHM->H1("hno_NofObjects_MvdClusters")->Fill(fMvdClusters->GetEntriesFast());
   if (NULL != fMvdHits && fHM->Exists("hno_NofObjects_MvdHits")) fHM->H1("hno_NofObjects_MvdHits")->Fill(fMvdHits->GetEntriesFast());

   if (NULL != fStsPoints && fHM->Exists("hno_NofObjects_StsPoints")) fHM->H1("hno_NofObjects_StsPoints")->Fill(fStsPoints->GetEntriesFast());
   if (NULL != fStsDigis && fHM->Exists("hno_NofObjects_StsDigis")) fHM->H1("hno_NofObjects_StsDigis")->Fill(fStsDigis->GetEntriesFast());
   if (NULL != fStsClusters && fHM->Exists("hno_NofObjects_StsClusters")) fHM->H1("hno_NofObjects_StsClusters")->Fill(fStsClusters->GetEntriesFast());
   if (NULL != fStsHits && fHM->Exists("hno_NofObjects_StsHits")) fHM->H1("hno_NofObjects_StsHits")->Fill(fStsHits->GetEntriesFast());

   if (NULL != fRichHits && fHM->Exists("hno_NofObjects_RichHits")) fHM->H1("hno_NofObjects_RichHits")->Fill(fRichHits->GetEntriesFast());
   if (NULL != fRichPoints && fHM->Exists("hno_NofObjects_RichPoints")) fHM->H1("hno_NofObjects_RichPoints")->Fill(fRichPoints->GetEntriesFast());

   if (NULL != fTrdPoints && fHM->Exists("hno_NofObjects_TrdPoints")) fHM->H1("hno_NofObjects_TrdPoints")->Fill(fTrdPoints->GetEntriesFast());
   if (NULL != fTrdDigis && fHM->Exists("hno_NofObjects_TrdDigis")) fHM->H1("hno_NofObjects_TrdDigis")->Fill(fTrdDigis->GetEntriesFast());
   if (NULL != fTrdClusters && fHM->Exists("hno_NofObjects_TrdClusters")) fHM->H1("hno_NofObjects_TrdClusters")->Fill(fTrdClusters->GetEntriesFast());
   if (NULL != fTrdHits && fHM->Exists("hno_NofObjects_TrdHits")) fHM->H1("hno_NofObjects_TrdHits")->Fill(fTrdHits->GetEntriesFast());

   if (NULL != fMuchPoints && fHM->Exists("hno_NofObjects_MuchPoints")) fHM->H1("hno_NofObjects_MuchPoints")->Fill(fMuchPoints->GetEntriesFast());
   if (NULL != fMuchDigis && fHM->Exists("hno_NofObjects_MuchDigis")) fHM->H1("hno_NofObjects_MuchDigis")->Fill(fMuchDigis->GetEntriesFast());
   if (NULL != fMuchClusters && fHM->Exists("hno_NofObjects_MuchClusters")) fHM->H1("hno_NofObjects_MuchClusters")->Fill(fMuchClusters->GetEntriesFast());
   if (NULL != fMuchPixelHits && fHM->Exists("hno_NofObjects_MuchPixelHits")) fHM->H1("hno_NofObjects_MuchPixelHits")->Fill(fMuchPixelHits->GetEntriesFast());
   if (NULL != fMuchStrawHits && fHM->Exists("hno_NofObjects_MuchStrawHits")) fHM->H1("hno_NofObjects_MuchStrawHits")->Fill(fMuchStrawHits->GetEntriesFast());

   if (NULL != fTofPoints && fHM->Exists("hno_NofObjects_TofPoints")) fHM->H1("hno_NofObjects_TofPoints")->Fill(fTofPoints->GetEntriesFast());
   if (NULL != fTofHits && fHM->Exists("hno_NofObjects_TofHits")) fHM->H1("hno_NofObjects_TofHits")->Fill(fTofHits->GetEntriesFast());
}

void CbmMuchClusteringQaCalculator::CalculateClustersRelations()
{
	//std::cout<<"-----Calculator Step 1\n";
	fClustersRelations = new ClusterInformation[fNofClusters];
	for(Int_t iCl = 0; iCl < fNofClusters; iCl++)
	{
		fClustersRelations[iCl].fStation = 0;
		fClustersRelations[iCl].fLeyer = 0;
		fClustersRelations[iCl].fSide = 0;
		fClustersRelations[iCl].fNofPoints = 0;
		fClustersRelations[iCl].fBestPoint = 0;
		fClustersRelations[iCl].fPointsInCluster.clear();
		fClustersRelations[iCl].fDigisByPoint.clear();
		fClustersRelations[iCl].fClusterToPointRelations.clear();
		fClustersRelations[iCl].fNDigisToClusterRelations.clear();
	}
	//std::cout<<"-----Calculator Step 2\n";
	fDigisByPoint = new Int_t[fNofPoints];
	for(Int_t iPoint = 0; iPoint < fNofPoints; iPoint++)
	{
		fDigisByPoint[iPoint] = 0;
	}
	//std::cout<<"-----Calculator Step 3\n";
	for(Int_t iDigi = 0; iDigi < fNofDigis; iDigi++)
	{
		CbmMuchDigiMatch* digiMatch = (CbmMuchDigiMatch*) fMuchDigiMatches->At(iDigi);
		Int_t nPoints = digiMatch->GetNPoints();
		for(Int_t iPoint = 0; iPoint < nPoints; iPoint++)
		{
			Int_t refIndex = digiMatch->GetRefIndex(iPoint);
			fDigisByPoint[refIndex]++;
		}
	}
	//std::cout<<"-----Calculator Step 4\n";
	for(Int_t iCl = 0; iCl < fNofClusters; iCl++)
	{
		CbmMuchCluster* cluster = (CbmMuchCluster*) fMuchClusters->At(iCl);
		Int_t nDigis = cluster->GetNDigis();
		//std::cout<<"------Calculator Step 4.1\n";
		//std::cout<<"------NDigiMatces: "<<fMuchDigiMatches->GetEntriesFast()<<"\n";
		for(Int_t iDigi = 0; iDigi < nDigis; iDigi++)
		{
			Int_t digiIndex = cluster->GetDigiIndex(iDigi);
			/*if(digiIndex >= fMuchDigiMatches->GetEntriesFast())
			{
				std::cout<<"Err; DigiMatch = "<<digiIndex<<"\n";
			}
			else
			{*/
			//std::cout<<"digiIndex: "<<digiIndex<<"\n";
			//std::cout<<"-------Calculator Step 4.1.1\n";
			//std::cout<<"--------Digi Index: "<<digiIndex<<"\n";
			CbmMuchDigiMatch* digiMatch = (CbmMuchDigiMatch*) fMuchDigiMatches->At(digiIndex);
			//std::cout<<"--------CbmMuchDigiMatch created\n";
			UInt_t c = digiMatch->GetCharge(0);
			//std::cout<<"c = "<<c<<"\n";
			Int_t nPoints = digiMatch->GetNPoints();
			//std::cout<<"-------Calculator Step 4.1.2\n";
			for(Int_t iPoint = 0; iPoint < nPoints; iPoint++)
			{
				Int_t refIndex = digiMatch->GetRefIndex(iPoint);
				//---
				//const CbmMuchPoint* muchPoint = static_cast<const CbmMuchPoint*>(fMuchPoints->At(refIndex));
				//const CbmMuchPixelHit* hit = static_cast<const CbmMuchPixelHit*>(fMuchHits->At(iCl));
				//---
				//std::cout<<"-------Calculator Step 4.1.3\n";
				if(fClustersRelations[iCl].fNofPoints == 0)
				{
					fClustersRelations[iCl].fPointsInCluster.push_back(refIndex);
					//fClustersRelations[iCl].fPointsInCluster[fClustersRelations[iCl].fNofPoints] = refIndex;
					fClustersRelations[iCl].fDigisByPoint.push_back(1);
					fClustersRelations[iCl].fNofPoints++;
					//std::cout<<"-------Calculator Step 4.1.4a\n";
				}
				else
				{
					Bool_t ip = 0;
					//std::cout<<"-------Calculator Step 4.1.4b\n";
					for(Int_t i = 0; i < fClustersRelations[iCl].fNofPoints; i++)
					{
						if(fClustersRelations[iCl].fPointsInCluster[i] == refIndex)
						{
							ip = 1;
							fClustersRelations[iCl].fDigisByPoint[i]++;
							//std::cout<<"Add digi to cluster "<<iCl<<"\n";
						}
					}
					if(ip == 0)
					{
						//fClustersRelations[iCl].fPointsInCluster[fClusters[iCl].nofMPoints] = refIndex;
						fClustersRelations[iCl].fPointsInCluster.push_back(refIndex);
						//fClusters[iCl].mDigisByPoint[fClusters[iCl].nofMPoints]++;
						fClustersRelations[iCl].fDigisByPoint.push_back(1);
						fClustersRelations[iCl].fNofPoints++;
						//std::cout<<"Add point to cluster "<<iCl<<"\n";
					}
					//std::cout<<"-------Calculator Step 4.1.4bb\n";
				}
			}
			//}
		}
		//std::cout<<"------Calculator Step 4.2\n";
		//---
		//std::cout<<"nDigis: "<<fMuchDigis->GetEntriesFast()<<"\n";
		/*if(cluster->GetDigiIndex(0) >= fMuchDigis->GetEntriesFast())
		{
			std::cout<<"Err; Digi = "<<cluster->GetDigiIndex(0)<<"\n";
		}
		else
		{*/
		CbmMuchDigi* digi = (CbmMuchDigi*) fMuchDigis->At(cluster->GetDigiIndex(0));
		const CbmMuchLayerSide* layerSide = static_cast<CbmMuchLayerSide*>(fMuchGeoScheme->GetLayerSideByDetId(digi->GetDetectorId()));
		Int_t nStations = fMuchGeoScheme->GetNStations();
		//std::cout<<"------Calculator Step 4.3\n";
		for(Int_t iStation = 0; iStation < nStations; iStation++)
		{
			//CbmMuchStation* muchStation = (CbmMuchStation)fMuchGeoScheme->GetStation()
			for(Int_t iLayer = 0; iLayer < 3; iLayer++)
			{
				const CbmMuchLayerSide* sideF = static_cast<CbmMuchLayerSide*>(fMuchGeoScheme->GetLayerSide(iStation, iLayer, 0));
				const CbmMuchLayerSide* sideB = static_cast<CbmMuchLayerSide*>(fMuchGeoScheme->GetLayerSide(iStation, iLayer, 1));
				if(sideF->GetDetectorId() == layerSide->GetDetectorId())
				{
					//---!!!---
					//fClustersByLayerSide[iStation][iLayer][0]++;
					fClustersRelations[iCl].fStation = iStation;
					fClustersRelations[iCl].fLeyer = iLayer;
					fClustersRelations[iCl].fSide = 0;
					//std::cout<<"add cl to st: "<<iStation<<"; l: "<<iLayer<<"\n";
				}
				if(sideB->GetDetectorId() == layerSide->GetDetectorId())
				{
					//---!!!---
					//fClustersByLayerSide[iStation][iLayer][1]++;
					fClustersRelations[iCl].fStation = iStation;
					fClustersRelations[iCl].fLeyer = iLayer;
					fClustersRelations[iCl].fSide = 1;
				}
			}
		}
		//}
		//std::cout<<"------Calculator Step 4.4\n";
	}
	//std::cout<<"-----Calculator Step 5\n";
	for(Int_t iCl = 0; iCl < fNofClusters; iCl++)
	{
		CbmMuchCluster* cluster = (CbmMuchCluster*) fMuchClusters->At(iCl);
		if(fClustersRelations[iCl].fNofPoints == 1)
		{
			/*if(fDigisByPoint[fClustersRelations[iCl].fPointsInCluster[0]] == 0)
			{
				std::cout<<"DEVISION BY ZERO!!!\nCluster: "<<iCl<<"; Point: "<<fClustersRelations[iCl].fPointsInCluster[0]<<"\n";
			}
			else
			{
				std::cout<<"Ok. Cluster: "<<iCl<<"; Point: "<<fClustersRelations[iCl].fPointsInCluster[0]<<"\n";
				std::cout<<"DBP1: "<<fClustersRelations[iCl].fDigisByPoint[0]<<
						"; DBP2: "<<fDigisByPoint[fClustersRelations[iCl].fPointsInCluster[0]]<<"\n";
			}*/
			fClustersRelations[iCl].fClusterToPointRelations.push_back(100.0 * (Float_t)fClustersRelations[iCl].fDigisByPoint[0] /
					(Float_t)fDigisByPoint[fClustersRelations[iCl].fPointsInCluster[0]]);
			if(fClustersRelations[iCl].fClusterToPointRelations[0] > 100) fClustersRelations[iCl].fClusterToPointRelations[0] = 100;
			fClustersRelations[iCl].fBestPoint = 0;
			fClustersRelations[iCl].fNDigisToClusterRelations.push_back(100.0 * (Float_t)(fClustersRelations[iCl].fDigisByPoint[0]) /
					(Float_t)(cluster->GetNDigis()));
			if(fClustersRelations[iCl].fNDigisToClusterRelations[0] > 100) fClustersRelations[iCl].fNDigisToClusterRelations[0] = 100;
		}
		else
		{
			Int_t bp = 0;
			for(Int_t iPoint = 0; iPoint < fClustersRelations[iCl].fNofPoints; iPoint++)
			{
				//Make vector.push_back()!!!
				fClustersRelations[iCl].fClusterToPointRelations.push_back(100.0 * (Float_t)fClustersRelations[iCl].fDigisByPoint[iPoint] /
						(Float_t)fDigisByPoint[fClustersRelations[iCl].fPointsInCluster[iPoint]]);
				if(fClustersRelations[iCl].fClusterToPointRelations[iPoint] > 100) fClustersRelations[iCl].fClusterToPointRelations[iPoint] = 100;
				fClustersRelations[iCl].fNDigisToClusterRelations.push_back(100.0 * (Float_t)(fClustersRelations[iCl].fDigisByPoint[iPoint]) /
						(Float_t)(cluster->GetNDigis()));
				if(fClustersRelations[iCl].fNDigisToClusterRelations[iPoint] > 100) fClustersRelations[iCl].fNDigisToClusterRelations[iPoint] = 100;
				if(fClustersRelations[iCl].fDigisByPoint[iPoint] > bp)
				{
					bp = fClustersRelations[iCl].fDigisByPoint[iPoint];
					fClustersRelations[iCl].fBestPoint = iPoint;
				}
			}
		}
	}
	//std::cout<<"-----Calculator Step 6\n";
}

void CbmMuchClusteringQaCalculator::CalculateAccuracy()
{
	fAccuracyArray = new AccuracyStruct[fNofHits];
	for(Int_t iCl = 0; iCl < fNofHits; iCl++)
	{
		const CbmMuchPixelHit* cluster = static_cast<const CbmMuchPixelHit*>(fMuchPixelHits->At(iCl));
		const CbmMuchPoint* muchPoint = static_cast<const CbmMuchPoint*>(fMuchPoints->At(fClustersRelations[cluster->GetRefId()/*iCl*/].fPointsInCluster[fClustersRelations[cluster->GetRefId()/*iCl*/].fBestPoint]));
		Float_t xPoint = (muchPoint->GetXIn() + muchPoint->GetXOut()) / 2;
		Float_t yPoint = (muchPoint->GetYIn() + muchPoint->GetYOut()) / 2;
		Float_t dist = sqrt((xPoint - cluster->GetX()) *
				(xPoint - cluster->GetX()) +
				(yPoint - cluster->GetY()) *
				(yPoint - cluster->GetY()));
		Float_t distX = fabs(xPoint - cluster->GetX());
		Float_t distY = fabs(yPoint - cluster->GetY());
		fAccuracyArray[iCl].fErrorXY = dist;
		fAccuracyArray[iCl].fErrorX = distX;
		fAccuracyArray[iCl].fErrorY = distY;
		fAccuracyArray[iCl].fHit = iCl;
		fAccuracyArray[iCl].fPoint = fClustersRelations[cluster->GetRefId()/*iCl*/].fPointsInCluster[fClustersRelations[cluster->GetRefId()/*iCl*/].fBestPoint];
		//std::cout<<"_Cl: "<<iCl<<"; dXY: "<<fAccuracyArray[iCl].errorXY<<"\n";
		//std::cout<<"xCl: "<<cluster->GetX()<<"; xP: "<<fRealPoints[fClusters[iCl].mPointsInCluster[fClusters[iCl].bestPoint]].xc<<"\n";
	}
}

void CbmMuchClusteringQaCalculator::FillErrorsAndQualityByRadiusHistograms()
{
	const Int_t nR = 100;
	Int_t nStations = fMuchGeoScheme->GetNStations();
	for(Int_t iStation = 0; iStation < nStations; iStation++)
	{
		CbmMuchStation* muchStation = (CbmMuchStation*)fMuchGeoScheme->GetStation(iStation);
		Int_t nLayers = muchStation->GetNLayers();
		for(Int_t iLayer = 0; iLayer < nLayers; iLayer++)
		{
			Float_t errorsByRadius[nR];
			Float_t hitsByRadius[nR];
			Float_t clustersByRadius[nR];
			Float_t qualityCPByRadius[nR];
			Float_t qualityDCByRadius[nR];
			for(Int_t i = 0; i < nR; i++)
			{
				errorsByRadius[i] = 0;
				qualityCPByRadius[i] = 0;
				qualityDCByRadius[i] = 0;
				hitsByRadius[i] = 0;
				clustersByRadius[i] = 0;
			}
			Float_t rMin = muchStation->GetRmin();
			Float_t rMax = muchStation->GetRmax();
			Float_t step = (rMax - rMin) / nR;
			for(Int_t iCl = 0; iCl < fNofHits; iCl++)
			{
				const CbmMuchPixelHit* h1 = static_cast<const CbmMuchPixelHit*>(fMuchPixelHits->At(iCl));
			if((fClustersRelations[h1->GetRefId()/*iCl*/].fStation == iStation) && (fClustersRelations[h1->GetRefId()/*iCl*/].fLeyer == iLayer))
			{
				const CbmMuchCluster* cluster = static_cast<const CbmMuchCluster*>(fMuchClusters->At(h1->GetRefId()/*fAccuracyArray[iCl].fHit*/));
				const CbmMuchDigi* digi = static_cast<const CbmMuchDigi*>(fMuchDigis->At(cluster->GetDigiIndex(0)));
				Int_t detId = digi->GetDetectorId();
				Long64_t chId = digi->GetChannelId();
				CbmMuchModuleGem* module = static_cast<CbmMuchModuleGem*>(fMuchGeoScheme->GetModuleByDetId(detId));
				const CbmMuchPad* pad = static_cast<const CbmMuchPad*>(module->GetPad(chId));
				Float_t padSize = pad->GetDx();
				//const CbmMuchPixelHit* hit = static_cast<const CbmMuchPixelHit*>(fMuchPixelHits->At(fAccuracyArray[iCl].fHit));
				Float_t rad = sqrt((h1->GetX() * h1->GetX()) + (h1->GetY() * h1->GetY()));
				Int_t n = static_cast<Int_t>((rad - rMin) / step);
				if((n >= nR) || (n < 0))
				{
					std::cout<<"Error! n = "<<n<<"; rad = "<<rad<<"; rMin = "<<rMin<<"; step = "<<step<<"\n";
					//std::cout<<"hX: "<<hit->GetX()<<"; hY: "<<hit->GetY()<<"\n";
				}
				else{
				/*std::cout<<"Cl: "<<iCl;
				std::cout<<"; n: "<<n;
				std::cout<<"; err: "<<errorsByRadius[n]<<"; AA: "<<fAccuracyArray[iCl].fErrorXY;
						std::cout<<"; size: "<<padSize<<"\n";*/
				errorsByRadius[n] += fAccuracyArray[iCl].fErrorXY / padSize;
				///!\\\qualityCPByRadius[n] += fClustersRelations[iCl].fClusterToPointRelations[fClustersRelations[iCl].fBestPoint];
				///!\\\qualityDCByRadius[n] += fClustersRelations[iCl].fNDigisToClusterRelations[fClustersRelations[iCl].fBestPoint];
				hitsByRadius[n]++;}
			}
			}
			for(Int_t iCl = 0; iCl < fNofClusters; iCl++)
			{
				CbmMuchPoint* p1 = (CbmMuchPoint*)(fMuchPoints->At(fClustersRelations[iCl].fBestPoint));
				Double_t rad = sqrt((((p1->GetXIn() + p1->GetXOut()) / 2) * ((p1->GetXIn() + p1->GetXOut()) / 2))
						+ (((p1->GetYIn() + p1->GetYOut()) / 2) * ((p1->GetYIn() + p1->GetYOut()) / 2)));
				Int_t n = static_cast<Int_t>((rad - rMin) / step);
				if((n >= nR) || (n < 0))
				{
					std::cout<<"Error! n = "<<n<<"; rad = "<<rad<<"; rMin = "<<rMin<<"; step = "<<step<<"\n";
				}
				else
				{
					qualityCPByRadius[n] += fClustersRelations[iCl].fClusterToPointRelations[fClustersRelations[iCl].fBestPoint];
					qualityDCByRadius[n] += fClustersRelations[iCl].fNDigisToClusterRelations[fClustersRelations[iCl].fBestPoint];
					clustersByRadius[n]++;
				}
			}
			for(Int_t i = 0; i < nR; i++)
			{
				if(hitsByRadius[i] == 0)hitsByRadius[i] = 1;
				errorsByRadius[i] = errorsByRadius[i] / hitsByRadius[i];
				char iSt[8]={0};
				char iL[8]={0};
				sprintf(iSt, "%d", iStation);
				sprintf(iL, "%d", iLayer);
				//string iSt = string(iSt1);
				//string iL = string(iL1);
				fHM->H1("hno_ErrorsByRadius_Much_Station_" + string(iSt) + "_Layer_" + string(iL))->Fill(i, errorsByRadius[i]);
				//
				qualityCPByRadius[i] = qualityCPByRadius[i] / clustersByRadius[i];
				qualityDCByRadius[i] = qualityDCByRadius[i] / clustersByRadius[i];
				fHM->H1("hno_QualClToPointByRadius_Much_Station_" + string(iSt) + "_Layer_" + string(iL))->Fill(i, qualityCPByRadius[i]);
				fHM->H1("hno_QualDigisToClByRadius_Much_Station_" + string(iSt) + "_Layer_" + string(iL))->Fill(i, qualityDCByRadius[i]);
			}
		}
	}
}

/*void CbmMuchClusteringQaCalculator::FillQualityByRadiusHistograms()
{
	const Int_t nR = 100;
	Int_t nStations = fMuchGeoScheme->GetNStations();
	for(Int_t iStation = 0; iStation < nStations; iStation++)
	{
		CbmMuchStation* muchStation = (CbmMuchStation*)fMuchGeoScheme->GetStation(iStation);
		Int_t nLayers = muchStation->GetNLayers();
		for(Int_t iLayer = 0; iLayer < nLayers; iLayer++)
		{
			Float_t qualityCPByRadius[nR];
			Float_t qualityDCByRadius[nR];
			Int_t clusterByRadius[nR];
			for(Int_t i = 0; i < nR; i++)
			{
				qualityCPByRadius[i] = 0;
				qualityDCByRadius[i] = 0;
				clusterByRadius[i] = 0;
			}
			for(Int_t iCl = 0; iCl < fNofClusters; iCl++)
			{
			if((fClustersRelations[iCl].fStation == iStation) && (fClustersRelations[iCl].fLeyer == iLayer))
			{
				//
			}
			}
		}
	}
}*/

void CbmMuchClusteringQaCalculator::FillEfficiencyByLayerHistograms()
{
	//std::map<Int_t, Int_t, Int_t>
}
