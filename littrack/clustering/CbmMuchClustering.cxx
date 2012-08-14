/*
 * CbmMuchClustering.cxx
 *
 *  Created on: Mar 5, 2012
 *      Author: kozlov
 */

#include "CbmMuchClustering.h"
#include "CbmMuchPixelHit.h"
#include "CbmMuchPoint.h"
#include "CbmMCTrack.h"
#include "CbmMuchGeoScheme.h"
#include "CbmMuchDigi.h"
#include "CbmMuchPad.h"
#include "CbmClusteringGeometry.h"
#include "CbmMuchModule.h"
#include "CbmMuchModuleGem.h"
#include "CbmClusteringA1.h"
#include "CbmClusteringSL.h"
#include "CbmClusteringWard.h"
#include "CbmMuchStation.h"
#include "CbmMuchCluster.h"
#include "CbmMuchStrawHit.h"

#include "FairRootManager.h"

//#include "TCanvas.h"
//#include "TClonesArray.h"
#include "TMath.h"

#include <cassert>
#include <map>
#include <vector>
#include <iostream>
#include <iomanip>

using std::cout;
using std::endl;

CbmMuchClustering::CbmMuchClustering()
{
	fAlgorithmVersion = 2;
	fNofModules = 0;
}

CbmMuchClustering::~CbmMuchClustering()
{
	if (fHit)
	{
		fHit->Delete();
		delete fHit;
	}
	if (fCluster)
	{
		fCluster->Delete();
		delete fCluster;
	}
}

InitStatus CbmMuchClustering::Init()
{
   std::cout << "CbmMuchClustering::Init" << std::endl;
   fScheme = CbmMuchGeoScheme::Instance();
   TString muchDigiFile = "/u/gkozlov/cbm/trunk/cbmroot/parameters/much/much_v11a.digi.root";
   fScheme->Init(muchDigiFile);

   ReadDataBranches();
   CreateModulesGeometryArray();
   if (fHit) fHit->Clear();
   if (fCluster) fCluster->Clear();

   return kSUCCESS;
}

void CbmMuchClustering::Exec(Option_t* opt)
{
	static Int_t eventNo = 0;
	std::cout << "CbmMuchClustering::Exec: eventNo=" << eventNo++ << std::endl;
	//if (fHit) fHit->Clear();
	//if (fCluster) fCluster->Clear();
	SetDigiCharges();
	ClusteringMainFunction();
	ClearDigiCharges();
	//MuchClustering(fAlgorithmVersion, fScheme);
}

void CbmMuchClustering::Finish()
{
   std::cout << "CbmMuchClustering::Finish" << std::endl;
}

void CbmMuchClustering::ReadDataBranches()
{
	FairRootManager* ioman = FairRootManager::Instance();
	assert(ioman != NULL);
	fMuchDigi = (TClonesArray*) ioman->GetObject("MuchDigi");

	fCluster = new TClonesArray("CbmMuchCluster", 1000);
	ioman->Register("MuchCluster", "Cluster in MUCH", fCluster, kTRUE);
	fHit = new TClonesArray("CbmMuchPixelHit", 1000);
	ioman->Register("MuchPixelHit", "Hit in MUCH", fHit, kTRUE);

	/*fStrawHit = new TClonesArray("CbmMuchStrawHit", 1000);
	ioman->Register("MuchStrawHit", "StrawHit in MUCH", fStrawHit, kTRUE);*/
}

void CbmMuchClustering::CreateModulesGeometryArray()
{
	fNofModules = 0;
	Int_t nStations = fScheme->GetNStations();
	for(Int_t iSt = 0; iSt < nStations; iSt++)
	{
		CbmMuchStation* station = (CbmMuchStation*)fScheme->GetStation(iSt);
		Int_t nLayers = station->GetNLayers();
		for(Int_t iL = 0; iL < nLayers; iL++)
		{
			CbmMuchLayer* layer = (CbmMuchLayer*)station->GetLayer(iL);
			CbmMuchLayerSide* lside = (CbmMuchLayerSide*)layer->GetSideF();
			fNofModules += lside->GetNModules();
			lside = (CbmMuchLayerSide*)layer->GetSideB();
			fNofModules += lside->GetNModules();
		}
	}
	fModulesGeometryArray = new CbmClusteringGeometry* [fNofModules];
	Int_t iModule = 0;
	for(Int_t iSt = 0; iSt < nStations; iSt++)
	{
		CbmMuchStation* station = (CbmMuchStation*)fScheme->GetStation(iSt);
		Int_t nLayers = station->GetNLayers();
		for(Int_t iL = 0; iL < nLayers; iL++)
		{
			CbmMuchLayer* layer = (CbmMuchLayer*)station->GetLayer(iL);
			CbmMuchLayerSide* lside = (CbmMuchLayerSide*)layer->GetSideF();
			Int_t nModules = lside->GetNModules();
			for(Int_t iMod = 0; iMod < nModules; iMod++)
			{
				fModulesGeometryArray[iModule] = new CbmClusteringGeometry(iSt, iL, 0, iMod, fScheme);
				fModulesByDetId[fModulesGeometryArray[iModule]->GetDetId()] = iModule;
				iModule++;
			}
			lside = (CbmMuchLayerSide*)layer->GetSideB();
			nModules = lside->GetNModules();
			for(Int_t iMod = 0; iMod < nModules; iMod++)
			{
				fModulesGeometryArray[iModule] = new CbmClusteringGeometry(iSt, iL, 1, iMod, fScheme);
				fModulesByDetId[fModulesGeometryArray[iModule]->GetDetId()] = iModule;
				iModule++;
			}
			std::cout<<"Layer "<<iL<<" geometry created successful.\n";
		}
		std::cout<<"Station "<<iSt<<" geometry created successful.\n";
	}
}

void CbmMuchClustering::SetDigiCharges()
{
	for(Int_t iDigi = 0; iDigi < fMuchDigi->GetEntriesFast(); iDigi++)
	{
		const CbmMuchDigi* muchDigi = static_cast<const CbmMuchDigi*>(fMuchDigi->At(iDigi));
		Int_t detID = muchDigi->GetDetectorId();
		Long64_t chId = muchDigi->GetChannelId();
		Int_t iPad = fModulesGeometryArray[fModulesByDetId[detID]]->GetPadByChannelId(chId);
		fModulesGeometryArray[fModulesByDetId[detID]]->SetPadCharge(iPad, muchDigi->GetADCCharge());
		fModulesGeometryArray[fModulesByDetId[detID]]->SetDigiNum(iPad, iDigi);
		fModulesGeometryArray[fModulesByDetId[detID]]->SetAPadsPlusOne();
	}
}

void CbmMuchClustering::ClearDigiCharges()
{
	for(Int_t iDigi = 0; iDigi < fMuchDigi->GetEntriesFast(); iDigi++)
	{
		const CbmMuchDigi* muchDigi = static_cast<const CbmMuchDigi*>(fMuchDigi->At(iDigi));
		Int_t detID = muchDigi->GetDetectorId();
		Long64_t chId = muchDigi->GetChannelId();
		Int_t iPad = fModulesGeometryArray[fModulesByDetId[detID]]->GetPadByChannelId(chId);
		fModulesGeometryArray[fModulesByDetId[detID]]->SetPadCharge(iPad, 0);
		fModulesGeometryArray[fModulesByDetId[detID]]->SetDigiNum(iPad, 0);
		fModulesGeometryArray[fModulesByDetId[detID]]->SetAPadsNom(0);
	}
}

void CbmMuchClustering::SetPadsCharge(CbmClusteringGeometry* moduleGeo, CbmMuchGeoScheme* geoScheme)
{
	/*Int_t nofActivePads = 0;
	for(Int_t iDigi = 0; iDigi < fMuchDigi->GetEntriesFast(); iDigi++)
	{
		const CbmMuchDigi* muchDigi = static_cast<const CbmMuchDigi*>(fMuchDigi->At(iDigi));
		Int_t detID = muchDigi->GetDetectorId();
		Int_t stationNom = CbmMuchGeoScheme::GetStationIndex(detID);
		Int_t layerNom = CbmMuchGeoScheme::GetLayerIndex(detID);
		Int_t sideNom = CbmMuchGeoScheme::GetLayerSideIndex(detID);
		Int_t moduleNom = CbmMuchGeoScheme::GetModuleIndex(detID);
		if((stationNom == moduleGeo->GetStation())&&(layerNom == moduleGeo->GetLayer())&&
				(sideNom == moduleGeo->GetLayerSide())&&(moduleNom == moduleGeo->GetModule()))
		{
			CbmMuchModuleGem* module = (CbmMuchModuleGem*)(geoScheme->GetModuleByDetId(detID));
			Long64_t channelID = muchDigi->GetChannelId();
			CbmMuchPad* pad = static_cast<CbmMuchPad*>(module->GetPad(channelID));
			Double_t xPad = pad->GetX();
			Double_t yPad = pad->GetY();
			for(Int_t iPad = 0; iPad < moduleGeo->GetNPads(); iPad++)
			{
				if(moduleGeo->GetPadID(iPad) == muchDigi->GetChannelId())
				{
					UInt_t charge = muchDigi->GetADCCharge();
					moduleGeo->SetPadCharge(iPad, charge);
					moduleGeo->SetDigiNum(iPad, iDigi);
					nofActivePads++;
				}
			}
		}
	}
	moduleGeo->SetAPadsNom(nofActivePads);*/
}

void CbmMuchClustering::DeletePadsCharge(CbmClusteringGeometry* moduleGeo)
{
	for(Int_t iPad = 0; iPad < moduleGeo->GetNPads(); iPad++)
	{
		moduleGeo->SetPadCharge(iPad, 0);
	}
}

void CbmMuchClustering::MuchClustering(Int_t algVersion, CbmMuchGeoScheme* scheme)
{
	Int_t nClusters = 0;
	Int_t nCluster = 0;
	Int_t nHit = 0;
	std::cout<<"\nCOMMENT\n";
	Int_t nStations = scheme->GetNStations();
	std::cout<<"NStations: "<<nStations<<"\n";
	for(Int_t i = 0; i < nStations; i++)
	{
		CbmMuchStation* station = (CbmMuchStation*) scheme->GetStation(i);
		Int_t nLayers = station->GetNLayers();
		std::cout<<"\n-Station: "<<i<<"; NLayers: "<<nLayers<<"\n";
		for(Int_t j = 0; j < nLayers; j++)
		{
			CbmMuchLayer* layer = (CbmMuchLayer*) station->GetLayer(j);
			CbmMuchLayerSide* layerSideF = (CbmMuchLayerSide*) layer->GetSideF();
			CbmMuchLayerSide* layerSideB = (CbmMuchLayerSide*) layer->GetSideB();
			Int_t nModulesF = layerSideF->GetNModules();
			Int_t nModulesB = layerSideB->GetNModules();
			std::cout<<"--Layer: "<<j<<"; NModules in F: "<<nModulesF<<"\n";
			//if((i == 0)&&(j == 0))
			if(true)
			{
			for(Int_t k = 0; k < nModulesF; k++)
			{
				CbmClusteringGeometry* moduleF = new CbmClusteringGeometry(i, j, 0, k, scheme);
				SetPadsCharge(moduleF, scheme);
				CbmMuchModuleGem* module = (CbmMuchModuleGem*)scheme->GetModule(i, j, 0, k);
				if(moduleF->GetAPadsNom() > 0)
				{
				switch(algVersion){
				case 1:
				{
					//A1.v1
					ClusteringA1(moduleF, module, 1, nHit, nCluster);
					break;
				}
				case 2:
				{
					//A1.v2
					ClusteringA1(moduleF, module, 2, nHit, nCluster);
					break;
				}
				case 3:
				{
					//SL.v1
					ClusteringSL(moduleF, module, 1, nHit, nCluster);
					break;
				}
				case 4:
				{
					//SL.v2
					ClusteringSL(moduleF, module, 2, nHit, nCluster);
					break;
				}
				case 5:
				{
					//Ward
					fClustersWard = new CbmClusteringWard(moduleF, 1000);
					fClustersWard->WardMainFunction(moduleF, 100);
					std::cout<<"Module:"<<k<<" - Ward: nofClusters: "<<fClustersWard->GetNofClusters()<<"\n";
//					for(Int_t iCl = 0; iCl < fClustersWard->GetNofClusters(); iCl++)
//					{
//						new ((*fClusters)[nClusters++]) CbmMuchClFull(fClustersWard->GetX0(iCl),
//								fClustersWard->GetY0(iCl), fClustersWard->GetClCharge(iCl),
//								fClustersWard->GetNofPads(iCl), 0,
//								layerSideF->GetDetectorId(), moduleF->GetDetId());
//					}
					delete fClustersWard;
					break;
				}
				default: std::cout<<"Wrong version of the algorithm.\n"; break;
				}
				}
				DeletePadsCharge(moduleF);
				delete moduleF;
			}
			}
			std::cout<<"--Layer: "<<j<<"; NModules in B: "<<nModulesF<<"\n";
			//if((i == 0)&&(j == 0))
			if(true)
			{
			for(Int_t k = 0; k < nModulesB; k++)
			{
				CbmClusteringGeometry* moduleB = new CbmClusteringGeometry(i, j, 1, k, scheme);
				SetPadsCharge(moduleB, scheme);
				CbmMuchModuleGem* module = (CbmMuchModuleGem*)scheme->GetModule(i, j, 1, k);
				if(moduleB->GetAPadsNom() > 0)
				{
				switch(algVersion){
				case 1:
				{
					//A1.v1
					ClusteringA1(moduleB, module, 1, nHit, nCluster);
					break;
				}
				case 2:
				{
					//A1.v2
					ClusteringA1(moduleB, module, 2, nHit, nCluster);
					break;
				}
				case 3:
				{
					//SL.v1
					ClusteringSL(moduleB, module, 1, nHit, nCluster);
					break;
				}
				case 4:
				{
					//SL.v2
					ClusteringSL(moduleB, module, 2, nHit, nCluster);
					break;
				}
				case 5:
				{
					//Ward
					fClustersWard = new CbmClusteringWard(moduleB, 1000);
					fClustersWard->WardMainFunction(moduleB, 100);
					std::cout<<"Module:"<<k<<" - Ward: nofClusters: "<<fClustersWard->GetNofClusters()<<"\n";
//					for(Int_t iCl = 0; iCl < fClustersWard->GetNofClusters(); iCl++)
//					{
//						new ((*fClusters)[nClusters++]) CbmMuchClFull(fClustersWard->GetX0(iCl),
//								fClustersWard->GetY0(iCl), fClustersWard->GetClCharge(iCl),
//								fClustersWard->GetNofPads(iCl), 0,
//								layerSideB->GetDetectorId(), moduleB->GetDetId());
//					}
					delete fClustersWard;
					break;
				}
				default: std::cout<<"Wrong version of the algorithm.\n";break;
				}
				}
				DeletePadsCharge(moduleB);
				delete moduleB;
			}
			}
		}
	}
}

void CbmMuchClustering::ClusteringMainFunction()
{
	Int_t nHit = 0;
	Int_t nCluster = 0;
	for(Int_t iMod = 0; iMod < fNofModules; iMod++)
	{
		CbmMuchModuleGem* module = (CbmMuchModuleGem*)fScheme->GetModuleByDetId(
				fModulesGeometryArray[iMod]->GetDetId());
		switch(fAlgorithmVersion){
		case 1:
		{//A1.v1
			ClusteringA1(fModulesGeometryArray[iMod], module, 1, nHit, nCluster);
			break;
		}
		case 2:
		{//A1.v2
			ClusteringA1(fModulesGeometryArray[iMod], module, 2, nHit, nCluster);
			break;
		}
		case 3:
		{//SL.v1
			ClusteringSL(fModulesGeometryArray[iMod], module, 1, nHit, nCluster);
			break;
		}
		case 4:
		{//SL.v2
			ClusteringSL(fModulesGeometryArray[iMod], module, 2, nHit, nCluster);
			break;
		}
		case 5:
		{//Ward
			fClustersWard = new CbmClusteringWard(fModulesGeometryArray[iMod], 1000);
			fClustersWard->WardMainFunction(fModulesGeometryArray[iMod], 100);
			std::cout<<"Module:"<<module->GetDetectorId()<<" - Ward: nofClusters: "<<fClustersWard->GetNofClusters()<<"\n";
//					for(Int_t iCl = 0; iCl < fClustersWard->GetNofClusters(); iCl++)
//					{
//						new ((*fClusters)[nClusters++]) CbmMuchClFull(fClustersWard->GetX0(iCl),
//								fClustersWard->GetY0(iCl), fClustersWard->GetClCharge(iCl),
//								fClustersWard->GetNofPads(iCl), 0,
//								layerSideF->GetDetectorId(), moduleF->GetDetId());
//					}
			delete fClustersWard;
			break;
		}
		default: std::cout<<"Wrong version of the algorithm.\n"; break;
		}
	}
}

void CbmMuchClustering::ClusteringA1(CbmClusteringGeometry* m1, CbmMuchModuleGem* m2, Int_t Ver, Int_t &nHit, Int_t &nCluster)
{
	fClustersA1 = new CbmClusteringA1(m1);
	fClustersA1->MainClusteringA1(m1, Ver);
	if(fClustersA1->GetNofClusters() != 0)std::cout<<"Module:"<<m2->GetDetectorId()<<" - A1.v"<<Ver<<": nofClusters: "<<fClustersA1->GetNofClusters()<<"\n";
	for(Int_t iCl = 0; iCl < fClustersA1->GetNofClusters(); iCl++)
	{
		vector<Int_t> digiIndices;
		//---
		Int_t detId = m2->GetDetectorId();
		Double_t x, y, z;
		x = fClustersA1->GetX0(iCl);
		y = fClustersA1->GetY0(iCl);
		z = m2->GetPosition()[2];
		Double_t dx = std::numeric_limits<Double_t>::max();
		Double_t dy = std::numeric_limits<Double_t>::max();
		CbmMuchPad* pad = NULL;
		//std::cout<<"->iCl: "<<iCl<<"; Npads: "<<fClustersA1->GetNofPads(iCl);
		for(Int_t iPad = 0; iPad < fClustersA1->GetNofPads(iCl); iPad++)
		{
			Int_t iDigi = fClustersA1->GetPadInCluster(iCl, iPad);
			const CbmMuchDigi* muchDigi = static_cast<const CbmMuchDigi*>(fMuchDigi->At(iDigi));
			digiIndices.push_back(iDigi);
			//std::cout<<"-"<<a<<"-";
			Long64_t channelId = muchDigi->GetChannelId();
			pad = m2->GetPad(channelId);
			if (dx > pad->GetDx()) dx = pad->GetDx();
			if (dy > pad->GetDy()) dy = pad->GetDy();
		}
		//std::cout<<"; qMax: "<<qMax<<"; SumCharge; "<<sumCharge<<"; Size: "<<digiIndices.size()<<"\n";
		new ((*fCluster)[nCluster++]) CbmMuchCluster(digiIndices);
		//---
		Double_t sigmaX = dx / TMath::Sqrt(12.);
		Double_t sigmaY = dy / TMath::Sqrt(12.);
		Int_t planeId = fScheme->GetLayerSideNr(detId);
		new ((*fHit)[nHit]) CbmMuchPixelHit(detId, x, y, z, sigmaX, sigmaY, 0, 0, fClustersA1->GetCluster(iCl), planeId, 0, 0);
		//---
		/*TVector3 pos, dpos;
		pos.SetXYZ(x, y, z);
		dpos.SetXYZ(sigmaX, sigmaY, 0.);
		new ((*fStrawHit)[nHit]) CbmMuchStrawHit(detId, pos, dpos, 0);*/
		nHit++;
		//---
		/*new ((*fClusters)[nClusters++]) CbmMuchClFull(fClustersA1->GetX0(iCl),
				fClustersA1->GetY0(iCl), fClustersA1->GetClCharge(iCl),
				fClustersA1->GetNofPads(iCl), fClustersA1->GetPads(iCl),
				layerSideF->GetDetectorId(), m1->GetDetId());*/
	}
	delete fClustersA1;
}

void CbmMuchClustering::ClusteringSL(CbmClusteringGeometry* m1, CbmMuchModuleGem* m2, Int_t Ver, Int_t &nHit, Int_t &nCluster)
{
	fClustersSL = new CbmClusteringSL(m1);
	fClustersSL->MainClusteringSL(m1, Ver);
	std::cout<<"Module:"<<m2->GetDetectorId()<<" - SL.v2: nofClusters: "<<fClustersSL->GetNofClusters()<<"\n";
	for(Int_t iCl = 0; iCl < fClustersSL->GetNofClusters(); iCl++)
	{
		vector<Int_t> digiIndices;
		//---
		Int_t detId = m2->GetDetectorId();
		Double_t x, y, z;
		x = fClustersSL->GetX0(iCl);
		y = fClustersSL->GetY0(iCl);
		z = m2->GetPosition()[2];
		Double_t dx = std::numeric_limits<Double_t>::max();
		Double_t dy = std::numeric_limits<Double_t>::max();
		CbmMuchPad* pad = NULL;
		//std::cout<<"->iCl: "<<iCl<<"; Npads: "<<fClustersA1->GetNofPads(iCl);
		for(Int_t iPad = 0; iPad < fClustersSL->GetNofPads(iCl); iPad++)
		{
			Int_t iDigi = fClustersSL->GetPadInCluster(iCl, iPad);
			const CbmMuchDigi* muchDigi = static_cast<const CbmMuchDigi*>(fMuchDigi->At(iDigi));
			digiIndices.push_back(iDigi);
			//std::cout<<"-"<<a<<"-";
			Long64_t channelId = muchDigi->GetChannelId();
			pad = m2->GetPad(channelId);
			if (dx > pad->GetDx()) dx = pad->GetDx();
			if (dy > pad->GetDy()) dy = pad->GetDy();
		}
		//std::cout<<"; qMax: "<<qMax<<"; SumCharge; "<<sumCharge<<"; Size: "<<digiIndices.size()<<"\n";
		new ((*fCluster)[nCluster++]) CbmMuchCluster(digiIndices);
		//---
		Double_t sigmaX = dx / TMath::Sqrt(12.);
		Double_t sigmaY = dy / TMath::Sqrt(12.);
		Int_t planeId = fScheme->GetLayerSideNr(detId);
		new ((*fHit)[nHit++]) CbmMuchPixelHit(detId, x, y, z, sigmaX, sigmaY, 0, 0, fClustersSL->GetCluster(iCl), planeId, 0, 0);
		//---
		/*new ((*fClusters)[nClusters++]) CbmMuchClFull(fClustersA1->GetX0(iCl),
				fClustersA1->GetY0(iCl), fClustersA1->GetClCharge(iCl),
				fClustersA1->GetNofPads(iCl), fClustersA1->GetPads(iCl),
				layerSideF->GetDetectorId(), m1->GetDetId());*/
	}
	delete fClustersA1;
}
ClassImp(CbmMuchClustering);
