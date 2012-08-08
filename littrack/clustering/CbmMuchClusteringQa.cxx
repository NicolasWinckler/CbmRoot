/*
 * CbmMuchClusteringQa.cxx
 *
 *  Created on: May 24, 2012
 *      Author: kozlov
 */

#include "CbmMCTrack.h"
#include "CbmMuchClusteringQa.h"
#include "CbmMuchGeoScheme.h"
#include "CbmMuchStation.h"
#include "CbmMuchModuleGem.h"
#include "CbmMuchSector.h"
#include "CbmMuchGeoCl.h"
#include "CbmMuchLayerSide.h"
#include "CbmMuchPoint.h"
#include "FairRootManager.h"
#include "CbmMuchCluster.h"
#include "CbmMuchDigi.h"
#include "CbmMuchDigiMatch.h"
#include "CbmMuchPixelHit.h"
#include "CbmMuchStrawHit.h"
#include "CbmMuchPad.h"

#include "TClonesArray.h"

#include "TArrayI.h"
#include "TObjArray.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TH2.h"
#include "TH1F.h"

#include <iostream>
#include <cassert>
#include <vector>

using std::cout;
using std::endl;
using std::map;
using std::pair;


CbmMuchClusteringQa::CbmMuchClusteringQa():
		fhMuchEfficiencyByLayerSide(NULL),
		fhMuchErrorsByLayerSide(NULL),
		fhMuchQualityClToPointByLayer(NULL),
		fhMuchQualityDigiToClByLayer(NULL),
		fhMuchErrorsByRadiusS1L0(NULL),
		fhMuchErrorsByRadiusS2L0(NULL),
		fhMuchErrorsByRadiusS3L0(NULL),
		fhMuchErrorsByRadiusS4L0(NULL),
		fhMuchQualityClToPointByRadiusS1L0(NULL),
		fhMuchQualityClToPointByRadiusS2L0(NULL),
		fhMuchQualityClToPointByRadiusS3L0(NULL),
		fhMuchQualityClToPointByRadiusS4L0(NULL),
		fhMuchQualityDigiToClByRadiusS1L0(NULL),
		fhMuchQualityDigiToClByRadiusS2L0(NULL),
		fhMuchQualityDigiToClByRadiusS3L0(NULL),
		fhMuchQualityDigiToClByRadiusS4L0(NULL)
  /*:FairTask(name,verbose),
    fGeoScheme( CbmMuchGeoScheme::Instance()),
    fGeoFileName("much.digi.root")*/
{
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
CbmMuchClusteringQa::~CbmMuchClusteringQa()
{
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
InitStatus CbmMuchClusteringQa::Init()
{
	std::cout << "CbmMuchClusteringQa::Init" << std::endl;
	FairRootManager* ioman = FairRootManager::Instance();
	//fClFull    = (TClonesArray*) ioman->GetObject("MuchClFull");
	fMuchDigi    = (TClonesArray*) ioman->GetObject("MuchDigi");
	fMuchDigiMatch    = (TClonesArray*) ioman->GetObject("MuchDigiMatch");
	fMuchCluster    = (TClonesArray*) ioman->GetObject("MuchCluster");
	fMuchHit    = (TClonesArray*) ioman->GetObject("MuchPixelHit");
	fMuchPoint = (TClonesArray*) ioman->GetObject("MuchPoint");
	fGeoScheme = CbmMuchGeoScheme::Instance();
	TString muchDigiFile = "/home/kozlov/cbm/cbmroot_new/cbmroot/parameters/much/much_v11a.digi.root";
	fGeoScheme->Init(muchDigiFile);

	fhMuchEfficiencyByLayerSide = new TH1F("fhMuchEfficiencyByLayer", "hMuchEfficiencyByLayer;Layer;Efficiency", 19, 0, 0);
	fhMuchErrorsByLayerSide = new TH1F("fhMuchErrorsByLayer", "hMuchErrorsByLayer;Layer;Errors", 19, 0, 0);
	fhMuchQualityClToPointByLayer = new TH1F("fhMuchQualityClToPointByLayer", "hMuchQualityClToPoint;Layer;Quality", 19, 0, 0);
	fhMuchQualityDigiToClByLayer = new TH1F("fhMuchQualityDigiToClByLayer", "fhMuchQualityDigiToCl;Layer;Quality", 19, 0, 0);

	fhMuchErrorsByRadiusS1L0 = new TH1F("fhMuchErrorsByRadiusS1L0", "hMuchErrorsByRadiusS1L0;Radius;Errors", nRadiusSteps + 2, 0, 0);
	fhMuchErrorsByRadiusS2L0 = new TH1F("fhMuchErrorsByRadiusS2L0", "hMuchErrorsByRadiusS2L0;Radius;Errors", nRadiusSteps + 2, 0, 0);
	fhMuchErrorsByRadiusS3L0 = new TH1F("fhMuchErrorsByRadiusS3L0", "hMuchErrorsByRadiusS3L0;Radius;Errors", nRadiusSteps + 2, 0, 0);
	fhMuchErrorsByRadiusS4L0 = new TH1F("fhMuchErrorsByRadiusS4L0", "hMuchErrorsByRadiusS4L0;Radius;Errors", nRadiusSteps + 2, 0, 0);

	fhMuchQualityClToPointByRadiusS1L0 = new TH1F("fhQualClToPointByRadiusS1L0", "hQualClToPointByRadiusS1L0;Radius;Quality", nRadiusSteps + 2, 0, 0);
	fhMuchQualityClToPointByRadiusS2L0 = new TH1F("fhQualClToPointByRadiusS2L0", "hQualClToPointByRadiusS2L0;Radius;Quality", nRadiusSteps + 2, 0, 0);
	fhMuchQualityClToPointByRadiusS3L0 = new TH1F("fhQualClToPointByRadiusS3L0", "hQualClToPointByRadiusS3L0;Radius;Quality", nRadiusSteps + 2, 0, 0);
	fhMuchQualityClToPointByRadiusS4L0 = new TH1F("fhQualClToPointByRadiusS4L0", "hQualClToPointByRadiusS4L0;Radius;Quality", nRadiusSteps + 2, 0, 0);

	fhMuchQualityDigiToClByRadiusS1L0 = new TH1F("fhQualDigiToClByRadiusS1L0", "hQualDigiToClByRadiusS1L0;Radius;Quality", nRadiusSteps + 2, 0, 0);
	fhMuchQualityDigiToClByRadiusS2L0 = new TH1F("fhQualDigiToClByRadiusS2L0", "hQualDigiToClByRadiusS2L0;Radius;Quality", nRadiusSteps + 2, 0, 0);
	fhMuchQualityDigiToClByRadiusS3L0 = new TH1F("fhQualDigiToClByRadiusS3L0", "hQualDigiToClByRadiusS3L0;Radius;Quality", nRadiusSteps + 2, 0, 0);
	fhMuchQualityDigiToClByRadiusS4L0 = new TH1F("fhQualDigiToClByRadiusS4L0", "hQualDigiToClByRadiusS4L0;Radius;Quality", nRadiusSteps + 2, 0, 0);

  return kSUCCESS;
}
// -------------------------------------------------------------------------



// -------------------------------------------------------------------------
void CbmMuchClusteringQa::Exec(Option_t * option)
{
	std::cout << "CbmMuchClusteringQa::Exec" << std::endl;
	fNofClusters = fMuchCluster->GetEntriesFast();
	fNofDigis = fMuchDigi->GetEntriesFast();
	fNofPoints = fMuchPoint->GetEntriesFast();
	for(Int_t iStation = 0; iStation < fGeoScheme->GetNStations(); iStation++)
	{
		const CbmMuchStation* station = static_cast<CbmMuchStation*>(fGeoScheme->GetStation(iStation));
		for(Int_t iLayer = 0; iLayer < station->GetNLayers(); iLayer++)
		{
			const CbmMuchLayerSide* layerSideF = static_cast<CbmMuchLayerSide*>(fGeoScheme->GetLayerSide(iStation, iLayer, 0));
			const CbmMuchLayerSide* layerSideB = static_cast<CbmMuchLayerSide*>(fGeoScheme->GetLayerSide(iStation, iLayer, 1));
			fModulesByLayerSide[iStation][iLayer][0] = layerSideF->GetNModules();
			fModulesByLayerSide[iStation][iLayer][1] = layerSideB->GetNModules();
			fClustersByLayerSide[iStation][iLayer][0];
			fPointsBylayerSide[iStation][iLayer][0];
			fEfficiencyLayerSide[iStation][iLayer][0];
			fAccuracyLayerSide[iStation][iLayer][0];
			fClustersByLayerSide[iStation][iLayer][1];
			fPointsBylayerSide[iStation][iLayer][1];
			fEfficiencyLayerSide[iStation][iLayer][1];
			fAccuracyLayerSide[iStation][iLayer][1];
		}
	}
	fAccuracyArray = new AccuracyStruct[fNofClusters];
	SetMCPoints();
	LinkingClustersToMCPoints();
	CreationOfRelations();
	std::cout<<"Clusters: "<<fNofClusters<<"\n";
	CalculateEfficienciByLayerSide();
	CalculateAccuracy();

	FillEfficiencyByLayerSideHistograms();
	FillErrorsByLayerSideHistograms();

	FillQualityClToPointHistograms();
	FillQualityDigiToClHistograms();

	FillErrorsByRadiusHistograms();
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmMuchClusteringQa::FinishTask()
{
	fhMuchEfficiencyByLayerSide->Write();
	TCanvas* canvasEffLS = new TCanvas("much_eff_by_ls", "much_eff_by_ls", 700, 700);
	canvasEffLS->Divide(1, 1);
	canvasEffLS->cd(1);
	fhMuchEfficiencyByLayerSide->Draw();

	fhMuchErrorsByLayerSide->Write();
	TCanvas* canvasErrLS = new TCanvas("much_err_by_ls", "much_err_by_ls", 700, 700);
	canvasErrLS->Divide(1, 1);
	canvasErrLS->cd(1);
	fhMuchErrorsByLayerSide->Draw();

	fhMuchQualityClToPointByLayer->Write();
	TCanvas* canvasQCP = new TCanvas("much_q_cp_l", "much_q_cp_l", 700, 700);
	canvasQCP->Divide(1, 1);
	canvasQCP->cd(1);
	fhMuchQualityClToPointByLayer->Draw();

	fhMuchQualityDigiToClByLayer->Write();
	TCanvas* canvasQDC = new TCanvas("much_q_dc_l", "much_q_dc_l", 700, 700);
	canvasQDC->Divide(1, 1);
	canvasQDC->cd(1);
	fhMuchQualityDigiToClByLayer->Draw();

	fhMuchErrorsByRadiusS1L0->Write();
	fhMuchErrorsByRadiusS2L0->Write();
	fhMuchErrorsByRadiusS3L0->Write();
	fhMuchErrorsByRadiusS4L0->Write();
	TCanvas* canvasERad = new TCanvas("MuchErrorsByRadius", "MuchErrorsByRadius", 1200, 1200);
	canvasERad->Divide(2, 2);
	canvasERad->cd(1);
	fhMuchErrorsByRadiusS1L0->Draw();
	canvasERad->cd(2);
	fhMuchErrorsByRadiusS2L0->Draw();
	canvasERad->cd(3);
	fhMuchErrorsByRadiusS3L0->Draw();
	canvasERad->cd(4);
	fhMuchErrorsByRadiusS4L0->Draw();

	fhMuchQualityClToPointByRadiusS1L0->Write();
	fhMuchQualityClToPointByRadiusS2L0->Write();
	fhMuchQualityClToPointByRadiusS3L0->Write();
	fhMuchQualityClToPointByRadiusS4L0->Write();
	TCanvas* canvasQRad = new TCanvas("MuchQualityClToPointByRadius", "MuchQualityClToPointByRadius", 1200, 1200);
	canvasQRad->Divide(2, 2);
	canvasQRad->cd(1);
	fhMuchQualityClToPointByRadiusS1L0->Draw();
	canvasQRad->cd(2);
	fhMuchQualityClToPointByRadiusS2L0->Draw();
	canvasQRad->cd(3);
	fhMuchQualityClToPointByRadiusS3L0->Draw();
	canvasQRad->cd(4);
	fhMuchQualityClToPointByRadiusS4L0->Draw();

	fhMuchQualityDigiToClByRadiusS1L0->Write();
	fhMuchQualityDigiToClByRadiusS2L0->Write();
	fhMuchQualityDigiToClByRadiusS3L0->Write();
	fhMuchQualityDigiToClByRadiusS4L0->Write();
	TCanvas* canvasQ2Rad = new TCanvas("MuchQualityDigiToClByRadius", "MuchQualityDigiToClByRadius", 1200, 1200);
	canvasQ2Rad->Divide(2, 2);
	canvasQ2Rad->cd(1);
	fhMuchQualityDigiToClByRadiusS1L0->Draw();
	canvasQ2Rad->cd(2);
	fhMuchQualityDigiToClByRadiusS2L0->Draw();
	canvasQ2Rad->cd(3);
	fhMuchQualityDigiToClByRadiusS3L0->Draw();
	canvasQ2Rad->cd(4);
	fhMuchQualityDigiToClByRadiusS4L0->Draw();
}
// -------------------------------------------------------------------------

void CbmMuchClusteringQa::SetMCPoints()
{
	//fNofPoints = fMuchPoint->GetEntriesFast();
	fRealPoints = new M_Point [fNofPoints];
	for(Int_t iPoint = 0; iPoint < fNofPoints; iPoint++)
	{
		const CbmMuchPoint* muchPoint = static_cast<const CbmMuchPoint*>(fMuchPoint->At(iPoint));
		const CbmMuchLayerSide* layerSide = static_cast<CbmMuchLayerSide*>(fGeoScheme->GetLayerSideByDetId(muchPoint->GetDetectorId()));
		const CbmMuchModule* module = static_cast<CbmMuchModule*>(fGeoScheme->GetModuleByDetId(muchPoint->GetDetectorId()));
		fRealPoints[iPoint].x1 = muchPoint->GetXIn();
		fRealPoints[iPoint].y1 = muchPoint->GetYIn();
		fRealPoints[iPoint].x2 = muchPoint->GetXOut();
		fRealPoints[iPoint].y2 = muchPoint->GetYOut();
		fRealPoints[iPoint].pLenght = sqrt((fRealPoints[iPoint].x1 - fRealPoints[iPoint].x2) *
				(fRealPoints[iPoint].x1 - fRealPoints[iPoint].x2) +
				(fRealPoints[iPoint].y1 - fRealPoints[iPoint].y2) *
				(fRealPoints[iPoint].y1 - fRealPoints[iPoint].y2));
		fRealPoints[iPoint].xc = (fRealPoints[iPoint].x1 + fRealPoints[iPoint].x2) / 2;
		fRealPoints[iPoint].yc = (fRealPoints[iPoint].y1 + fRealPoints[iPoint].y2) / 2;
		fRealPoints[iPoint].nofClusters = 0;
		for(Int_t i = 0; i < 50; i++)
		{
			fRealPoints[iPoint].clustersToPoint[i] = 0;
			fRealPoints[iPoint].digisByCluster[i] = 0;
		}
		fRealPoints[iPoint].nofDigis = 0;
		//fRealPoints[iPoint].lSideId = layerSide->GetDetectorId();
		//fRealPoints[iPoint].moduleId = module->GetDetectorId();
		for(Int_t iStation = 0; iStation < 6; iStation++)
		{
			for(Int_t iLayer = 0; iLayer < 3; iLayer++)
			{
				const CbmMuchLayerSide* sideF = static_cast<CbmMuchLayerSide*>(fGeoScheme->GetLayerSide(iStation, iLayer, 0));
				const CbmMuchLayerSide* sideB = static_cast<CbmMuchLayerSide*>(fGeoScheme->GetLayerSide(iStation, iLayer, 1));
				if(sideF->GetDetectorId() == layerSide->GetDetectorId())
				{
					fPointsBylayerSide[iStation][iLayer][0]++;
					fRealPoints[iPoint].station = iStation;
					fRealPoints[iPoint].leyer = iLayer;
					fRealPoints[iPoint].side = 0;
					//std::cout<<"add point to st: "<<iStation<<"; l: "<<iLayer<<"\n";
				}
				if(sideB->GetDetectorId() == layerSide->GetDetectorId())
				{
					fPointsBylayerSide[iStation][iLayer][1]++;
					fRealPoints[iPoint].station = iStation;
					fRealPoints[iPoint].leyer = iLayer;
					fRealPoints[iPoint].side = 1;
				}
			}
		}
	}
	//std::cout<<"_S0.L0.F: "<<fPointsBylayerSide[0][0][0]<<"\n";
	std::cout<<"Points: "<<fNofPoints<<"\n";
}

void CbmMuchClusteringQa::LinkingClustersToMCPoints()
{
	fClusters = new Cluster [fNofClusters];
	for(Int_t iCl = 0; iCl < fNofClusters; iCl++)
	{
		fClusters[iCl].nofMPoints = 0;
		for(Int_t i = 0; i < 30; i++)
		{
			fClusters[iCl].mClToPoint[i] = 0;
			fClusters[iCl].mDigisByPoint[i] = 0;
			fClusters[iCl].mPointToCl[i] = 0;
			fClusters[iCl].mPointsInCluster[i] = 0;
			fClusters[iCl].mPointsQuality[i] = 0;
		}
	}
	for(Int_t iDigi = 0; iDigi < fNofDigis; iDigi++)
	{
		CbmMuchDigiMatch* digiMatch = (CbmMuchDigiMatch*) fMuchDigiMatch->At(iDigi);
		Int_t nPoints = digiMatch->GetNPoints();
		for(Int_t iPoint = 0; iPoint < nPoints; iPoint++)
		{
			Int_t refIndex = digiMatch->GetRefIndex(iPoint);
			fRealPoints[refIndex].nofDigis++;
		}
	}
	//map<pair<Int_t, Int_t>, Bool_t> digiMap;
	//pair<Int_t, Int_t> a(0, 0);
	Int_t cls = 2111;//2533;
	for(Int_t iCl = 0; iCl < fNofClusters; iCl++)
	{
		CbmMuchCluster* cluster = (CbmMuchCluster*) fMuchCluster->At(iCl);
		Int_t nDigis = cluster->GetNDigis();
		for(Int_t iDigi = 0; iDigi < nDigis; iDigi++)
		{
			Int_t digiIndex = cluster->GetDigiIndex(iDigi);
			//std::cout<<"digiIndex: "<<digiIndex<<"\n";
			CbmMuchDigiMatch* digiMatch = (CbmMuchDigiMatch*) fMuchDigiMatch->At(digiIndex);
			Int_t nPoints = digiMatch->GetNPoints();
			if((iCl > cls)&&(iCl < cls + 10))/*if(nDigis > 6)*/std::cout<<"\niCl: "<<iCl<<"; iDigi: "<<digiIndex<<"; NP: "<<nPoints<<"\n";
			for(Int_t iPoint = 0; iPoint < nPoints; iPoint++)
			{
				Int_t refIndex = digiMatch->GetRefIndex(iPoint);
				//---
				const CbmMuchPoint* muchPoint = static_cast<const CbmMuchPoint*>(fMuchPoint->At(refIndex));
				const CbmMuchPixelHit* hit = static_cast<const CbmMuchPixelHit*>(fMuchHit->At(iCl));
				//---
				if((iCl > cls)&&(iCl < cls + 10))std::cout<<"->iPoint: "<<refIndex<<"\n";
				if(fClusters[iCl].nofMPoints == 0)
				{
					fClusters[iCl].mPointsInCluster[fClusters[iCl].nofMPoints] = refIndex;
					fClusters[iCl].mDigisByPoint[fClusters[iCl].nofMPoints]++;
					fClusters[iCl].nofMPoints++;
					if((iCl > cls)&&(iCl < cls + 10))
					{
						std::cout<<"-->Make point in cl\n";
						std::cout<<"xP: "<<muchPoint->GetXIn()<<"; xC: "<<hit->GetX()<<"\n";
					}
				}
				else
				{
					Bool_t ip = 0;
					for(Int_t i = 0; i < fClusters[iCl].nofMPoints; i++)
					{
						if(fClusters[iCl].mPointsInCluster[i] == refIndex)
						{
							ip = 1;
							fClusters[iCl].mDigisByPoint[i]++;
							if((iCl > cls)&&(iCl < cls + 10))
							{
								std::cout<<"-->Add digi to point\n";
								std::cout<<"xP: "<<muchPoint->GetXIn()<<"; xC: "<<hit->GetX()<<"\n";
							}
							//std::cout<<"Add digi to cluster "<<iCl<<"\n";
						}
					}
					if(ip == 0)
					{
						fClusters[iCl].mPointsInCluster[fClusters[iCl].nofMPoints] = refIndex;
						fClusters[iCl].mDigisByPoint[fClusters[iCl].nofMPoints]++;
						fClusters[iCl].nofMPoints++;
						if((iCl > cls)&&(iCl < cls + 10))
						{
							std::cout<<"-->Add point to cl\n";
							std::cout<<"xP: "<<muchPoint->GetXIn()<<"; xC: "<<hit->GetX()<<"\n";
						}
						//std::cout<<"Add point to cluster "<<iCl<<"\n";
					}
				}
			}
		}
		if(iCl == cls){
		std::cout<<"nofMpoints: "<<fClusters[cls].nofMPoints<<"\n";
		for(Int_t i = 0; i < fClusters[cls].nofMPoints; i++)
		{
			std::cout<<"->MP: "<<fClusters[cls].mPointsInCluster[i]<<"; nD: "<<fClusters[cls].mDigisByPoint[i]<<"\n";
			std::cout<<"-->P: "<<fRealPoints[fClusters[cls].mPointsInCluster[i]].nofDigis<<"\n";
		}}
		//---
		CbmMuchDigi* digi = (CbmMuchDigi*) fMuchDigi->At(cluster->GetDigiIndex(0));
		const CbmMuchLayerSide* layerSide = static_cast<CbmMuchLayerSide*>(fGeoScheme->GetLayerSideByDetId(digi->GetDetectorId()));
		for(Int_t iStation = 0; iStation < 6; iStation++)
		{
			for(Int_t iLayer = 0; iLayer < 3; iLayer++)
			{
				const CbmMuchLayerSide* sideF = static_cast<CbmMuchLayerSide*>(fGeoScheme->GetLayerSide(iStation, iLayer, 0));
				const CbmMuchLayerSide* sideB = static_cast<CbmMuchLayerSide*>(fGeoScheme->GetLayerSide(iStation, iLayer, 1));
				if(sideF->GetDetectorId() == layerSide->GetDetectorId())
				{
					fClustersByLayerSide[iStation][iLayer][0]++;
					fClusters[iCl].station = iStation;
					fClusters[iCl].leyer = iLayer;
					fClusters[iCl].side = 0;
					//std::cout<<"add cl to st: "<<iStation<<"; l: "<<iLayer<<"\n";
				}
				if(sideB->GetDetectorId() == layerSide->GetDetectorId())
				{
					fClustersByLayerSide[iStation][iLayer][1]++;
					fClusters[iCl].station = iStation;
					fClusters[iCl].leyer = iLayer;
					fClusters[iCl].side = 1;
				}
			}
		}
	}
	//std::cout<<"_S0.L0.F: "<<fClustersByLayerSide[0][0][0]<<"\n";
}

void CbmMuchClusteringQa::CreationOfRelations()
{
	//TString fname = "/home/kozlov/cbm/cbmroot_new/events/03/TestResults/ClToPoints.txt";
	//FILE* f1 = fopen(fname , "w");
	for(Int_t iCl = 0; iCl < fNofClusters; iCl++)
	{
		CbmMuchCluster* cluster = (CbmMuchCluster*) fMuchCluster->At(iCl);
		if(fClusters[iCl].nofMPoints == 1)
		{
			fClusters[iCl].mClToPoint[0] = 100.0 * (Float_t)fClusters[iCl].mDigisByPoint[0] /
					(Float_t)fRealPoints[fClusters[iCl].mPointsInCluster[0]].nofDigis;
			if(fClusters[iCl].mClToPoint[0] > 100) fClusters[iCl].mClToPoint[0] = 100;
			fClusters[iCl].bestPoint = 0;
			fClusters[iCl].mDigiToCl[0] = 100.0 * (Float_t)(fClusters[iCl].mDigisByPoint[0]) /
					(Float_t)(cluster->GetNDigis());
			//		fRealPoints[fClusters[iCl].mPointsInCluster[0]].digisByCluster[0];
			/*fClusters[iCl].mPointToCl[0] = 100 * fRealPoints[fClusters[iCl].mPointsInCluster[0]].digisByCluster[0] /
					fClusters[iCl].mDigisByPoint[0];*/
			/*if(iCl < 1000/fClusters[iCl].mClToPoint[0] != 100/)std::cout<<"Cl: "<<iCl<<"; mC-P: "<<fClusters[iCl].mClToPoint[0]<<"; mD-P: "<<
					fClusters[iCl].mDigisByPoint[0]<<"; digis: "<<
					fRealPoints[fClusters[iCl].mPointsInCluster[0]].nofDigis<<
					"; MPoint: "<<fClusters[iCl].mPointsInCluster[0]<<
					"; D-C: "<<cluster->GetNDigis()<<"\n";*/
			/*if(iCl < 111/fClusters[iCl].mClToPoint[iPoint] != 100/)std::cout<<"Cl: "<<iCl<<"; D: "<<
					cluster->GetNDigis()<<"; mClToP: "<<fClusters[iCl].mClToPoint[0]<<"; mDigisByPoint: "<<
					fClusters[iCl].mDigisByPoint[0]<<"; digis: "<<
					fRealPoints[fClusters[iCl].mPointsInCluster[0]].nofDigis<<
					"; MPoint: "<<fClusters[iCl].mPointsInCluster[0]<<"\n";*/
			//if((fClusters[iCl].station == 3) && (fClusters[iCl].leyer == 1))std::cout<<"Cl: "<<iCl<<"; nDigi: "<<cluster->GetNDigis()<<"; mDigiToCl: "<<fClusters[iCl].mDigiToCl[0]<<"\n";
		}
		else
		{
			Int_t bp = 0;
			//if(fClusters[iCl].nofMPoints > 50)std::cout<<"ERR: "<<fClusters[iCl].nofMPoints<<"\n";
			for(Int_t iPoint = 0; iPoint < fClusters[iCl].nofMPoints; iPoint++)
			{
				fClusters[iCl].mClToPoint[iPoint] = 100.0 * (Float_t)fClusters[iCl].mDigisByPoint[iPoint] /
						(Float_t)fRealPoints[fClusters[iCl].mPointsInCluster[iPoint]].nofDigis;
				if(fClusters[iCl].mClToPoint[iPoint] > 100) fClusters[iCl].mClToPoint[iPoint] = 100;
				fClusters[iCl].mDigiToCl[iPoint] = 100.0 * (Float_t)(fClusters[iCl].mDigisByPoint[iPoint]) /
						(Float_t)(cluster->GetNDigis());
				if(fClusters[iCl].mDigisByPoint[iPoint] > bp)
				{
					bp = fClusters[iCl].mDigisByPoint[iPoint];
					fClusters[iCl].bestPoint = iPoint;
				}
				//		(Float_t)fRealPoints[fClusters[iCl].mPointsInCluster[iPoint]].digisByCluster[iPoint];
				/*fClusters[iCl].mPointToCl[iPoint] = 100.0 * (Float_t)fRealPoints[fClusters[iCl].mPointsInCluster[iPoint]].digisByCluster[iPoint] /
						(Float_t)fClusters[iCl].mDigisByPoint[iPoint];*/
				/*if(iCl < 111/fClusters[iCl].mClToPoint[iPoint] != 100/)std::cout<<"Cl: "<<iCl<<"; D: "<<
						cluster->GetNDigis()<<"; mClToP: "<<fClusters[iCl].mClToPoint[iPoint]<<"; mDigisByPoint: "<<
						fClusters[iCl].mDigisByPoint[iPoint]<<"; digis: "<<
						fRealPoints[fClusters[iCl].mPointsInCluster[iPoint]].nofDigis<<
						"; MPoint: "<<fClusters[iCl].mPointsInCluster[iPoint]<<"\n";*/
				//if((fClusters[iCl].station == 3) && (fClusters[iCl].leyer == 1))std::cout<<"Cl: "<<iCl<<"; nDigi: "<<cluster->GetNDigis()<<"; mDigiToCl: "<<fClusters[iCl].mDigiToCl[iPoint]<<"\n";
			}
			//if(fClusters[iCl].nofMPoints > 1)std::cout<<"bp: "<<fClusters[iCl].bestPoint<<"\n";
		}
		/*fprintf(f1, "Cluster: %d; digis: %d; Point: %d; digis: %d; Res: %.2f\n", iCl,
				cluster->GetNDigis(), fClusters[iCl].mPointsInCluster[fClusters[iCl].bestPoint],
				fClusters[iCl].mDigisByPoint[fClusters[iCl].bestPoint],
				fClusters[iCl].mClToPoint[fClusters[iCl].bestPoint]);*/
		//if(fClusters[iCl].bestPoint != 0)std::cout<<"bp: "<<fClusters[iCl].bestPoint<<"\n";
	}
	//fclose(f1);
}

Float_t CbmMuchClusteringQa::CalculateEfficiency()
{
	if(fNofPoints <= 0)
	{
		std::cout<<"Error! MCPoints is not loaded!\n";
		return 0;
	}
	fEfficiency = 100.0 * (Float_t)fNofClusters / fNofPoints;
	return fEfficiency;
}

void CbmMuchClusteringQa::CalculateEfficienciByLayerSide()
{
	for(Int_t iStation = 0; iStation < fGeoScheme->GetNStations(); iStation++)
	{
		const CbmMuchStation* station = static_cast<CbmMuchStation*>(fGeoScheme->GetStation(iStation));
		for(Int_t iLayer = 0; iLayer < station->GetNLayers(); iLayer++)
		{
			fEfficiencyLayerSide[iStation][iLayer][0] = 100 * (Float_t)fClustersByLayerSide[iStation][iLayer][0] /
					(Float_t)fPointsBylayerSide[iStation][iLayer][0];
			fEfficiencyLayerSide[iStation][iLayer][1] = 100 * (Float_t)fClustersByLayerSide[iStation][iLayer][1] /
					(Float_t)fPointsBylayerSide[iStation][iLayer][1];
			//std::cout<<"Station "<<iStation<<"; Layer "<<iLayer<<"; F: "<<fEfficiencyLayerSide[iStation][iLayer][0]<<"\n";
			//std::cout<<"Station "<<iStation<<"; Layer "<<iLayer<<"; B: "<<fEfficiencyLayerSide[iStation][iLayer][1]<<"\n";
		}
	}
}

void CbmMuchClusteringQa::CalculateAccuracy()
{
	for(Int_t iCl = 0; iCl < fNofClusters; iCl++)
	{
		const CbmMuchPixelHit* cluster = static_cast<const CbmMuchPixelHit*>(fMuchHit->At(iCl));
		Float_t dist = sqrt((fRealPoints[fClusters[iCl].mPointsInCluster[fClusters[iCl].bestPoint]].xc - cluster->GetX()) *
				(fRealPoints[fClusters[iCl].mPointsInCluster[fClusters[iCl].bestPoint]].xc - cluster->GetX()) +
				(fRealPoints[fClusters[iCl].mPointsInCluster[fClusters[iCl].bestPoint]].yc - cluster->GetY()) *
				(fRealPoints[fClusters[iCl].mPointsInCluster[fClusters[iCl].bestPoint]].yc - cluster->GetY()));
		Float_t distX = fabs(fRealPoints[fClusters[iCl].mPointsInCluster[fClusters[iCl].bestPoint]].xc - cluster->GetX());
		Float_t distY = fabs(fRealPoints[fClusters[iCl].mPointsInCluster[fClusters[iCl].bestPoint]].yc - cluster->GetY());
		fAccuracyArray[iCl].errorXY = dist;
		fAccuracyArray[iCl].errorX = distX;
		fAccuracyArray[iCl].errorY = distY;
		fAccuracyArray[iCl].nHit = iCl;
		fAccuracyArray[iCl].nPoint = fClusters[iCl].mPointsInCluster[fClusters[iCl].bestPoint];
		//std::cout<<"_Cl: "<<iCl<<"; dXY: "<<fAccuracyArray[iCl].errorXY<<"\n";
		//std::cout<<"xCl: "<<cluster->GetX()<<"; xP: "<<fRealPoints[fClusters[iCl].mPointsInCluster[fClusters[iCl].bestPoint]].xc<<"\n";
		fErrorByLayerSide[fClusters[iCl].station][fClusters[iCl].leyer][fClusters[iCl].side] += fAccuracyArray[iCl].errorXY;
	}
	for(Int_t iStation = 0; iStation < 6; iStation++)
	{
		for(Int_t iLayer = 0; iLayer < 3; iLayer++)
		{
			fErrorByLayerSide[iStation][iLayer][0] = fErrorByLayerSide[iStation][iLayer][0] /
					fClustersByLayerSide[iStation][iLayer][0];
			fErrorByLayerSide[iStation][iLayer][1] = fErrorByLayerSide[iStation][iLayer][1] /
					fClustersByLayerSide[iStation][iLayer][1];
		}
	}
	//std::cout<<"_S0.L0.F: "<<fErrorByLayerSide[0][0][0]<<"\n";
}

void CbmMuchClusteringQa::FillEfficiencyByLayerSideHistograms()
{
	for(Int_t iStation = 0; iStation < 6; iStation++)
	{
		for(Int_t iLayer = 0; iLayer < 3; iLayer++)
		{
			fhMuchEfficiencyByLayerSide->Fill((iStation * 3 + iLayer), fEfficiencyLayerSide[iStation][iLayer][0]);
		}
	}
}
void CbmMuchClusteringQa::FillErrorsByLayerSideHistograms()
{
	for(Int_t iStation = 0; iStation < 6; iStation++)
	{
		for(Int_t iLayer = 0; iLayer < 3; iLayer++)
		{
			fhMuchErrorsByLayerSide->Fill((iStation * 3 + iLayer), fErrorByLayerSide[iStation][iLayer][0]);
		}
	}
}

void CbmMuchClusteringQa::FillQualityClToPointHistograms()
{
	Float_t qualityByLayer[6][3];
	for(Int_t iStation = 0; iStation < 6; iStation++)
	{
		for(Int_t iLayer = 0; iLayer < 3; iLayer++)
		{
			qualityByLayer[iStation][iLayer] = 0;
		}
	}
	for(Int_t iCl = 0; iCl < fNofClusters; iCl++)
	{
		qualityByLayer[fClusters[iCl].station][fClusters[iCl].leyer] +=
				fClusters[iCl].mClToPoint[fClusters[iCl].bestPoint];
	}
	for(Int_t iStation = 0; iStation < 6; iStation++)
	{
		for(Int_t iLayer = 0; iLayer < 3; iLayer++)
		{
			qualityByLayer[iStation][iLayer] = qualityByLayer[iStation][iLayer] /
					(fClustersByLayerSide[iStation][iLayer][0] + fClustersByLayerSide[iStation][iLayer][1]);
			fhMuchQualityClToPointByLayer->Fill((iStation * 3 + iLayer), qualityByLayer[iStation][iLayer]);
		}
	}
	Float_t qualityByRadius[nRadiusSteps][4];
	Float_t quality2ByRadius[nRadiusSteps][4];
	Int_t clustersByRadius[nRadiusSteps][4];
	for(Int_t i = 0; i < nRadiusSteps; i++)
	{
		for(Int_t j = 0; j < 4; j++)
		{
			qualityByRadius[i][j] = 0;
			clustersByRadius[i][j] = 0;
			quality2ByRadius[i][j] = 0;
		}
	}
	TString fnameClToPoint = "/home/kozlov/cbm/cbmroot_new/events/03/TestResults/_ClToPoint.txt";
	FILE* ClToPoint = fopen(fnameClToPoint , "w");
	for(Int_t iCl = 0; iCl < fNofClusters; iCl++)
	{
		if(fClusters[iCl].bestPoint != 0)std::cout<<"Cl: "<<iCl<<"; bp: "<<fClusters[iCl].bestPoint<<"\n";
	}
	std::cout<<"-Cl_2796"<<"; bp: "<<fClusters[2796].bestPoint<<"\n";
	for(Int_t iCl = 0; iCl < fNofClusters; iCl++)
	{
		for(Int_t i = 0; i < 4; i++){
		if(fClusters[fAccuracyArray[iCl].nHit].station == i)
		{
			const CbmMuchStation* station = static_cast<CbmMuchStation*>(fGeoScheme->GetStation(i));
			Float_t rMin = station->GetRmin();
			Float_t rMax = station->GetRmax();
			Float_t step = (rMax - rMin) / nRadiusSteps;
			if(fClusters[fAccuracyArray[iCl].nHit].leyer == 0)
			{
				const CbmMuchPixelHit* hit = static_cast<const CbmMuchPixelHit*>(fMuchHit->At(fAccuracyArray[iCl].nHit));
				Float_t rad = sqrt((hit->GetX() * hit->GetX()) + (hit->GetY() * hit->GetY()));
				Int_t n = static_cast<Int_t>((rad - rMin) / step);
				if(n >= nRadiusSteps)std::cout<<"Error! n = "<<n<<"\n";
				if(fClusters[iCl].bestPoint != 0)std::cout<<"Cl: "<<iCl<<"; BP: "<<fClusters[iCl].bestPoint<<"\n";
				//if(fClusters[iCl].bestPoint != 0)std::cout<<"CP: "<<fClusters[iCl].mDigiToCl[fClusters[iCl].bestPoint]<<"\n";
				qualityByRadius[n][i] += fClusters[iCl].mClToPoint[fClusters[iCl].bestPoint];
				quality2ByRadius[n][i] += fClusters[iCl].mDigiToCl[fClusters[iCl].bestPoint];
				clustersByRadius[n][i]++;
				/*if(n == 0)std::cout<<"S: "<<i<<"; Q1: "<<qualityByRadius[n][i]<<
						"; K: "<<clustersByRadius[n][i]<<"\n";*/
				fprintf(ClToPoint, "Cluster: %d; ClToPoint: %.3f\n", iCl, fClusters[iCl].mClToPoint[fClusters[iCl].bestPoint]);
			}
		}}
	}
	fclose(ClToPoint);
	for(Int_t i = 0; i < nRadiusSteps; i++)
	{
		for(Int_t j = 0; j < 4; j++)
		{
			if(clustersByRadius[i][j] == 0)
				{
				clustersByRadius[i][j] = 1;
				qualityByRadius[i][j] = 0;
				quality2ByRadius[i][j] = 0;
				}
			qualityByRadius[i][j] = qualityByRadius[i][j] / clustersByRadius[i][j];
			quality2ByRadius[i][j] = quality2ByRadius[i][j] / clustersByRadius[i][j];
		}
		fhMuchQualityClToPointByRadiusS1L0->Fill(i, qualityByRadius[i][0]);
		fhMuchQualityClToPointByRadiusS2L0->Fill(i, qualityByRadius[i][1]);
		fhMuchQualityClToPointByRadiusS3L0->Fill(i, qualityByRadius[i][2]);
		fhMuchQualityClToPointByRadiusS4L0->Fill(i, qualityByRadius[i][3]);

		fhMuchQualityDigiToClByRadiusS1L0->Fill(i, quality2ByRadius[i][0]);
		fhMuchQualityDigiToClByRadiusS2L0->Fill(i, quality2ByRadius[i][1]);
		fhMuchQualityDigiToClByRadiusS3L0->Fill(i, quality2ByRadius[i][2]);
		fhMuchQualityDigiToClByRadiusS4L0->Fill(i, quality2ByRadius[i][3]);
	}
}
void CbmMuchClusteringQa::FillQualityDigiToClHistograms()
{
	Float_t qualityByLayer[6][3];
	for(Int_t iStation = 0; iStation < 6; iStation++)
	{
		for(Int_t iLayer = 0; iLayer < 3; iLayer++)
		{
			qualityByLayer[iStation][iLayer] = 0;
		}
	}
	for(Int_t iCl = 0; iCl < fNofClusters; iCl++)
	{
		qualityByLayer[fClusters[iCl].station][fClusters[iCl].leyer] +=
				fClusters[iCl].mDigiToCl[fClusters[iCl].bestPoint];
		/*if((fClusters[iCl].station == 3) && (fClusters[iCl].leyer == 1))
		{
			std::cout<<"add cl "<<iCl<<"; dtc: "<<fClusters[iCl].mDigiToCl[fClusters[iCl].bestPoint]<<"\n";
		}*/
	}
	for(Int_t iStation = 0; iStation < 6; iStation++)
	{
		for(Int_t iLayer = 0; iLayer < 3; iLayer++)
		{
			//std::cout<<"___S: "<<iStation<<"; L: "<<iLayer<<"; q: "<<qualityByLayer[iStation][iLayer]<<"\n";
			qualityByLayer[iStation][iLayer] = qualityByLayer[iStation][iLayer] /
					(fClustersByLayerSide[iStation][iLayer][0] + fClustersByLayerSide[iStation][iLayer][1]);
			fhMuchQualityDigiToClByLayer->Fill((iStation * 3 + iLayer), qualityByLayer[iStation][iLayer]);
			//std::cout<<"_S: "<<iStation<<"; L: "<<iLayer<<"; q: "<<qualityByLayer[iStation][iLayer]<<"\n";
		}
	}
}

void CbmMuchClusteringQa::FillErrorsByRadiusHistograms()
{
	Float_t errS0L0[nRadiusSteps], errS1L0[nRadiusSteps], errS2L0[nRadiusSteps], errS3L0[nRadiusSteps];
	Int_t nofClS0L0[nRadiusSteps], nofClS1L0[nRadiusSteps], nofClS2L0[nRadiusSteps], nofClS3L0[nRadiusSteps];
	for(Int_t i = 0; i < nRadiusSteps; i++)
	{
		errS0L0[i] = 0;
		errS1L0[i] = 0;
		errS2L0[i] = 0;
		errS3L0[i] = 0;
		nofClS0L0[i] = 0;
		nofClS1L0[i] = 0;
		nofClS2L0[i] = 0;
		nofClS3L0[i] = 0;
	}
	Float_t errSiL0[nRadiusSteps][4];
	Int_t nofClSiL0[nRadiusSteps][4];
	for(Int_t i = 0; i < nRadiusSteps; i++)
	{
		for(Int_t j = 0; j < 4; j++)
		{
			errSiL0[i][j] = 0;
			nofClSiL0[i][j] = 0;
		}
	}
	for(Int_t iCl = 0; iCl < fNofClusters; iCl++)
	{
		for(Int_t i = 0; i < 4; i++){
		if(fClusters[fAccuracyArray[iCl].nHit].station == i)
		{
			const CbmMuchStation* station = static_cast<CbmMuchStation*>(fGeoScheme->GetStation(i));
			Float_t rMin = station->GetRmin();
			Float_t rMax = station->GetRmax();
			Float_t step = (rMax - rMin) / nRadiusSteps;
			if(fClusters[fAccuracyArray[iCl].nHit].leyer == 0)
			{
				const CbmMuchCluster* cluster = static_cast<const CbmMuchCluster*>(fMuchCluster->At(fAccuracyArray[iCl].nHit));
				const CbmMuchDigi* digi = static_cast<const CbmMuchDigi*>(fMuchDigi->At(cluster->GetDigiIndex(0)));
				Int_t detId = digi->GetDetectorId();
				Long64_t chId = digi->GetChannelId();
				CbmMuchModuleGem* module = static_cast<CbmMuchModuleGem*>(fGeoScheme->GetModuleByDetId(detId));
				const CbmMuchPad* pad = static_cast<const CbmMuchPad*>(module->GetPad(chId));
				Float_t padSize = pad->GetLx();
				const CbmMuchPixelHit* hit = static_cast<const CbmMuchPixelHit*>(fMuchHit->At(fAccuracyArray[iCl].nHit));
				Float_t rad = sqrt((hit->GetX() * hit->GetX()) + (hit->GetY() * hit->GetY()));
				Int_t n = static_cast<Int_t>((rad - rMin) / step);
				if(n >= nRadiusSteps)std::cout<<"Error! n = "<<n<<"\n";
				errSiL0[n][i] += fAccuracyArray[iCl].errorXY / padSize;
				nofClSiL0[n][i]++;
				//errS0L0[n] += fAccuracyArray[iCl].errorXY / padSize;
				//nofClS0L0[n]++;
			}
		}}
		/*if(fClusters[fAccuracyArray[iCl].nHit].station == 1)
		{
			const CbmMuchStation* station = static_cast<CbmMuchStation*>(fGeoScheme->GetStation(1));
			Float_t rMin = station->GetRmin();
			Float_t rMax = station->GetRmax();
			Float_t step = (rMax - rMin) / nRadiusSteps;
			if(fClusters[fAccuracyArray[iCl].nHit].leyer == 0)
			{
				const CbmMuchPixelHit* hit = static_cast<const CbmMuchPixelHit*>(fMuchHit->At(fAccuracyArray[iCl].nHit));
				Float_t rad = sqrt((hit->GetX() * hit->GetX()) + (hit->GetY() * hit->GetY()));
				Int_t n = static_cast<Int_t>((rad - rMin) / step);
				if(n >= nRadiusSteps)std::cout<<"Error! n = "<<n<<"\n";
				errS1L0[n] += fAccuracyArray[iCl].errorXY;
				nofClS1L0[n]++;
			}
		}
		if(fClusters[fAccuracyArray[iCl].nHit].station == 2)
		{
			const CbmMuchStation* station = static_cast<CbmMuchStation*>(fGeoScheme->GetStation(2));
			Float_t rMin = station->GetRmin();
			Float_t rMax = station->GetRmax();
			Float_t step = (rMax - rMin) / nRadiusSteps;
			if(fClusters[fAccuracyArray[iCl].nHit].leyer == 0)
			{
				const CbmMuchPixelHit* hit = static_cast<const CbmMuchPixelHit*>(fMuchHit->At(fAccuracyArray[iCl].nHit));
				Float_t rad = sqrt((hit->GetX() * hit->GetX()) + (hit->GetY() * hit->GetY()));
				Int_t n = static_cast<Int_t>((rad - rMin) / step);
				if(n >= nRadiusSteps)std::cout<<"Error! n = "<<n<<"\n";
				errS2L0[n] += fAccuracyArray[iCl].errorXY;
				nofClS2L0[n]++;
			}
		}
		if(fClusters[fAccuracyArray[iCl].nHit].station == 3)
		{
			const CbmMuchStation* station = static_cast<CbmMuchStation*>(fGeoScheme->GetStation(3));
			Float_t rMin = station->GetRmin();
			Float_t rMax = station->GetRmax();
			Float_t step = (rMax - rMin) / nRadiusSteps;
			if(fClusters[fAccuracyArray[iCl].nHit].leyer == 0)
			{
				const CbmMuchPixelHit* hit = static_cast<const CbmMuchPixelHit*>(fMuchHit->At(fAccuracyArray[iCl].nHit));
				Float_t rad = sqrt((hit->GetX() * hit->GetX()) + (hit->GetY() * hit->GetY()));
				Int_t n = static_cast<Int_t>((rad - rMin) / step);
				if(n >= nRadiusSteps)std::cout<<"Error! n = "<<n<<"\n";
				errS3L0[n] += fAccuracyArray[iCl].errorXY;
				nofClS3L0[n]++;
			}
		}*/
	}
	for(Int_t i = 0; i < nRadiusSteps; i++)
	{
		if(nofClSiL0[i][0] == 0)nofClSiL0[i][0] = 1;
		errSiL0[i][0] = errSiL0[i][0] / nofClSiL0[i][0];
		fhMuchErrorsByRadiusS1L0->Fill(i, errSiL0[i][0]);
		if(nofClSiL0[i][1] == 0)nofClSiL0[i][1] = 1;
		errSiL0[i][1] = errSiL0[i][1] / nofClSiL0[i][1];
		fhMuchErrorsByRadiusS2L0->Fill(i, errSiL0[i][1]);
		if(nofClSiL0[i][2] == 0)nofClSiL0[i][2] = 1;
		errSiL0[i][2] = errSiL0[i][2] / nofClSiL0[i][2];
		fhMuchErrorsByRadiusS3L0->Fill(i, errSiL0[i][2]);
		if(nofClSiL0[i][3] == 0)nofClSiL0[i][3] = 1;
		errSiL0[i][3] = errSiL0[i][3] / nofClSiL0[i][3];
		fhMuchErrorsByRadiusS4L0->Fill(i, errSiL0[i][3]);
		/*if(nofClS0L0[i] == 0)nofClS0L0[i] = 1;
		errS0L0[i] = errS0L0[i] / nofClS0L0[i];
		fhMuchErrorsByRadiusS1L0->Fill(i, errS0L0[i]);
		if(nofClS1L0[i] == 0)nofClS1L0[i] = 1;
		errS1L0[i] = errS1L0[i] / nofClS1L0[i];
		fhMuchErrorsByRadiusS2L0->Fill(i, errS1L0[i]);
		if(nofClS2L0[i] == 0)nofClS2L0[i] = 1;
		errS2L0[i] = errS2L0[i] / nofClS2L0[i];
		fhMuchErrorsByRadiusS3L0->Fill(i, errS2L0[i]);
		if(nofClS3L0[i] == 0)nofClS3L0[i] = 1;
		errS3L0[i] = errS3L0[i] / nofClS3L0[i];
		fhMuchErrorsByRadiusS4L0->Fill(i, errS3L0[i]);*/
		//std::cout<<i<<" - "<<errS0L0[i]<<" - "<<errS1L0[i]<<" - "<<errS2L0[i]<<" - "<<errS3L0[i]<<"\n";
	}
}

ClassImp(CbmMuchClusteringQa)
