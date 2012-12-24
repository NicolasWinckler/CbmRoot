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
#include "CbmMuchPadRadial.h"
#include "CbmClusteringGeometry.h"
#include "CbmMuchModule.h"
#include "CbmMuchModuleGem.h"
#include "CbmMuchModuleGemRadial.h"
#include "CbmClusteringA1.h"
#include "CbmClusteringSL.h"
#include "CbmClusteringWard.h"
#include "CbmMuchStation.h"
#include "CbmMuchCluster.h"
#include "CbmMuchStrawHit.h"
#include "CbmMuchDigiMatch.h"

#include "FairRootManager.h"

//#include "TCanvas.h"
//#include "TClonesArray.h"
#include "TMath.h"

#include "draw/CbmDrawHist.h"
#include "TCanvas.h"
#include "TH1F.h"

#include <cassert>
#include <map>
#include <vector>
#include <iostream>
#include <iomanip>

using std::cout;
using std::endl;

CbmMuchClustering::CbmMuchClustering(const char* digiFileName):
		FairTask(),
		fClustersSL(),
		fClustersA1(),
		fClustersWard(),
		fModulesGeometryArray(),
		fDigiFile(digiFileName),
		fScheme(CbmMuchGeoScheme::Instance()),
		fCluster(),
		fMuchDigi(),
		fMuchPoint(),
		fHit(),
		//fStrawHit(),
		fMuchPad(),
		fNofClusters(),
		fhDigisInCluster(NULL),
		fhPoints(NULL),
		fDigiMatch()
{
	fAlgorithmVersion = 2;
	fGeoVersion = 1;
	fNofModules = 0;
	fNofEvents = 0;
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
	/*if (fStrawHit)
	{
		fStrawHit->Delete();
		delete fStrawHit;
	}*/
}

InitStatus CbmMuchClustering::Init()
{
   std::cout << "CbmMuchClustering::Init" << std::endl;
   /*fScheme = CbmMuchGeoScheme::Instance();
   //TString muchDigiFile = "/u/gkozlov/cbm/trunk/cbmroot/parameters/much/much_v11a.digi.root";
   TString muchDigiFile = "/home/kozlov/cbm/cbmroot_new/cbmroot/parameters/much/much_v11a.digi.root";
   fScheme->Init(muchDigiFile);*/

   fhDigisInCluster = new TH1F("fhDigisInCluster", "hDigisInCluster;nofDigis;nofClusters", 15, 0.5, 15.5);

   //fhPoints = new TH1F("fhPoints", "hPoints;nofPoint;nofDigis", 1000, 0.5, 1000.5);

   TFile* oldfile=gFile;
   TFile* file=new TFile(fDigiFile);
   TObjArray* stations = (TObjArray*) file->Get("stations");
   file->Close();
   file->Delete();
   gFile=oldfile;
   fScheme->Init(stations);

   ReadDataBranches();
   CreateModulesGeometryArray();

   //if (fStrawHit) fStrawHit->Clear();

   return kSUCCESS;
}

void CbmMuchClustering::Exec(Option_t* opt)
{
	static Int_t eventNo = 0;
	std::cout << " - CbmMuchClustering::Exec: eventNo=" << eventNo++ << std::endl;
	fNofEvents++;
	if( fMuchDigi->GetEntriesFast() == 0 ){
		cout << "-I- " << GetName() << "::Exec: No digis found, event skipped. " << endl;
		fNofEvents--;
	        //fCounter++;
		return;
	}
	fNofClusters = 0;
	fHit->Clear();
	fCluster->Delete();
	//if (fHit) fHit->Clear();
	//if (fCluster) fCluster->Clear();
	SetDigiCharges();
	ClusteringMainFunction();
	ClearDigiCharges();
	//TestPoints();
	std::cout<<" - Much clusters:   "<<fNofClusters<<"\n";
}

void CbmMuchClustering::Finish()
{
   std::cout << "CbmMuchClustering::Finish" << std::endl;

   SetDefaultDrawStyle();
   fhDigisInCluster->Scale(1. / fNofEvents);
   fhDigisInCluster->Write();
   TCanvas* canvasDC = new TCanvas("fhDigisInCluster", "fhDigisInCluster", 1000, 500);
   DrawH1(fhDigisInCluster, kLinear, kLog);

   /*fhPoints->Scale(1. / fNofEvents);
   fhPoints->Write();
   TCanvas* canvasP = new TCanvas("fhPoints", "fhPoints", 1000, 500);
   DrawH1(fhPoints, kLinear, kLog);*/
}

void CbmMuchClustering::ReadDataBranches()
{
	FairRootManager* ioman = FairRootManager::Instance();
	assert(ioman != NULL);
	fMuchDigi = (TClonesArray*) ioman->GetObject("MuchDigi");

	fMuchPoint = (TClonesArray*) ioman->GetObject("MuchPoint");
	fDigiMatch = (TClonesArray*) ioman->GetObject("MuchDigiMatch");

	fCluster = new TClonesArray("CbmMuchCluster", 1000);
	ioman->Register("MuchCluster", "Cluster in MUCH", fCluster, kTRUE);
	fHit = new TClonesArray("CbmMuchPixelHit", 1000);
	ioman->Register("MuchPixelHit", "Hit in MUCH", fHit, kTRUE);
	/*fStrawHit = new TClonesArray("CbmMuchStrawHit", 1000);
	ioman->Register("MuchStrawHit", "StrawHit in MUCH", fStrawHit, kTRUE);*/

	/*fStrawHit = new TClonesArray("CbmMuchStrawHit", 1000);
	ioman->Register("MuchStrawHit", "StrawHit in MUCH", fStrawHit, kTRUE);*/
}

void CbmMuchClustering::TestPoints()
{
	Int_t pa = fMuchPoint->GetEntriesFast();
	Int_t *pointsArr = new Int_t[pa];
	for(Int_t i = 0; i < fMuchPoint->GetEntriesFast(); i++)
	{
		pointsArr[i] = 0;
	}
	for(Int_t i = 0; i < fDigiMatch->GetEntriesFast(); i++)
	{
		CbmMuchDigiMatch* digiMatch = (CbmMuchDigiMatch*) fDigiMatch->At(i);
		for(Int_t j = 0; j < digiMatch->GetNPoints(); j++){
		Int_t nPoints = digiMatch->GetRefIndex(j);
		pointsArr[nPoints]++;}
	}
	for(Int_t i = 0; i < fMuchPoint->GetEntriesFast(); i++)
	{
		const CbmMuchPoint* muchPoint = static_cast<const CbmMuchPoint*>(fMuchPoint->At(i));
		if(abs(muchPoint->GetTrackID()) == 13)
		{
			std::cout<<"Point "<<i<<": "<<pointsArr[i]<<"\n";
		}
		//std::cout<<"Point "<<i<<": "<<pointsArr[i]<<"; ID: "<<muchPoint->GetTrackID()<<"\n";
		fhPoints->Fill(i, pointsArr[i]);
	}
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
				fModulesGeometryArray[iModule] = new CbmClusteringGeometry(iSt, iL, 0, iMod, fScheme, fGeoVersion);
				fModulesByDetId[fModulesGeometryArray[iModule]->GetDetId()] = iModule;
				iModule++;
			}
			lside = (CbmMuchLayerSide*)layer->GetSideB();
			nModules = lside->GetNModules();
			for(Int_t iMod = 0; iMod < nModules; iMod++)
			{
				fModulesGeometryArray[iModule] = new CbmClusteringGeometry(iSt, iL, 1, iMod, fScheme, fGeoVersion);
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
		//if(muchDigi->GetADCCharge() > 10){
		Int_t detID = muchDigi->GetDetectorId();
		Long64_t chId = muchDigi->GetChannelId();
		Int_t iPad = fModulesGeometryArray[fModulesByDetId[detID]]->GetPadByChannelId(chId);
		fModulesGeometryArray[fModulesByDetId[detID]]->SetPadCharge(iPad, muchDigi->GetADCCharge());
		fModulesGeometryArray[fModulesByDetId[detID]]->SetDigiNum(iPad, iDigi);
		fModulesGeometryArray[fModulesByDetId[detID]]->SetAPadsPlusOne();//}
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

/*void CbmMuchClustering::SetPadsCharge(CbmClusteringGeometry* moduleGeo, CbmMuchGeoScheme* geoScheme)
{
	Int_t nofActivePads = 0;
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
	moduleGeo->SetAPadsNom(nofActivePads);
}*/

void CbmMuchClustering::DeletePadsCharge(CbmClusteringGeometry* moduleGeo)
{
	for(Int_t iPad = 0; iPad < moduleGeo->GetNPads(); iPad++)
	{
		moduleGeo->SetPadCharge(iPad, 0);
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
			ClusteringA1(fModulesGeometryArray[iMod], module, 1/*, nHit, nCluster*/);
			break;
		}
		case 2:
		{//A1.v2
			ClusteringA1(fModulesGeometryArray[iMod], module, 2/*, nHit, nCluster*/);
			break;
		}
		case 3:
		{//SL.v1
			ClusteringSL(fModulesGeometryArray[iMod], module, 1/*, nHit, nCluster*/);
			break;
		}
		case 4:
		{//SL.v2
			ClusteringSL(fModulesGeometryArray[iMod], module, 2/*, nHit, nCluster*/);
			break;
		}
		case 5:
		{//Ward
			ClusteringWard(fModulesGeometryArray[iMod], module/*, 2, nHit, nCluster*/);
			/*fClustersWard = new CbmClusteringWard(fModulesGeometryArray[iMod], 1000);
			fClustersWard->WardMainFunction(fModulesGeometryArray[iMod], 100);
			std::cout<<"Module:"<<module->GetDetectorId()<<" - Ward: nofClusters: "<<fClustersWard->GetNofClusters()<<"\n";
//					for(Int_t iCl = 0; iCl < fClustersWard->GetNofClusters(); iCl++)
//					{
//						new ((*fClusters)[nClusters++]) CbmMuchClFull(fClustersWard->GetX0(iCl),
//								fClustersWard->GetY0(iCl), fClustersWard->GetClCharge(iCl),
//								fClustersWard->GetNofPads(iCl), 0,
//								layerSideF->GetDetectorId(), moduleF->GetDetId());
//					}
			delete fClustersWard;*/
			break;
		}
		default: std::cout<<"Wrong version of the algorithm.\n"; break;
		}
	}
}

void CbmMuchClustering::ClusteringA1(CbmClusteringGeometry* m1, CbmMuchModuleGem* m2, Int_t Ver/*, Int_t &nHit, Int_t &nCluster*/)
{
	fClustersA1 = new CbmClusteringA1(m1);
	fClustersA1->MainClusteringA1(m1, Ver);
	//if(fClustersA1->GetNofClusters() != 0)std::cout<<"Module:"<<m2->GetDetectorId()<<" - A1.v"<<Ver<<": nofClusters: "<<fClustersA1->GetNofClusters()<<"\n";
	fNofClusters += fClustersA1->GetNofClusters();
	vector<Int_t> digiIndices;
	for(Int_t iCl = 0; iCl < fClustersA1->GetNofClusters(); iCl++)
	{
		digiIndices.clear();
		//---
		Int_t detId = m2->GetDetectorId();
		Double_t sumq=0, sumx=0, sumy=0, sumt=0, sumdx2=0, sumdy2=0, sumdxy2=0, sumdt2=0;
		Double_t q=0,x=0,y=0,t=0,z=0,dx=0,dy=0,dxy=0,dt=0;
		x = fClustersA1->GetX0(iCl);
		y = fClustersA1->GetY0(iCl);
		z = m2->GetPosition()[2];
		//Double_t dx = std::numeric_limits<Double_t>::max();
		//Double_t dy = std::numeric_limits<Double_t>::max();
		CbmMuchPad* pad = NULL;
		//std::cout<<"->iCl: "<<iCl<<"; Npads: "<<fClustersA1->GetNofPads(iCl);
		fhDigisInCluster->Fill(fClustersA1->GetNofPads(iCl));
		//if(fClustersA1->GetNofPads(iCl) > 4)
		//{
		//	SubclusteringA1(iCl, m1, m2, Ver, fClustersA1, 0);
			/*std::cout<<"Cl: "<<iCl<<"; Pads: "<<fClustersA1->GetNofPads(iCl)<<"\n";
			CbmClusteringGeometry* subCluster = new CbmClusteringGeometry(m1, fClustersA1->GetNofPads(iCl));
			Int_t maxPad = 0;
			UInt_t maxPadCharge = fClustersA1->GetPadCharge(iCl, 0);
			UInt_t secondPadCharge = 0;
			UInt_t therdPadCharge = 0;
			for(Int_t iPad = 0; iPad < fClustersA1->GetNofPads(iCl); iPad++)
			{
				if(fClustersA1->GetPadCharge(iCl, iPad) > maxPadCharge)
				{
					therdPadCharge = secondPadCharge;
					secondPadCharge = maxPadCharge;
					maxPadCharge = fClustersA1->GetPadCharge(iCl, iPad);
					maxPad = iPad;
				}
				std::cout<<"=>Cl: "<<iCl<<"\n";
				std::cout<<"Charges: "<<maxPadCharge<<" --- "<<secondPadCharge<<" --- "<<therdPadCharge<<"\n";
				Int_t pad = fClustersA1->GetNPadInCluster(iCl, iPad);
//				std::cout<<"nPad: "<<pad<<"\n";
//				std::cout<<"X: "<<m1->GetX0(pad)<<"\n";
//				std::cout<<"Y: "<<m1->GetY0(pad)<<"\n";
//				std::cout<<"dX: "<<m1->GetDx(pad)<<"\n";
//				std::cout<<"dY: "<<m1->GetDy(pad)<<"\n";
//				std::cout<<"phi1: "<<m1->GetPhi1(pad)<<"\n";
//				std::cout<<"phi2: "<<m1->GetPhi2(pad)<<"\n";
//				std::cout<<"R1: "<<m1->GetR1(pad)<<"\n";
//				std::cout<<"R2: "<<m1->GetR2(pad)<<"\n";
//				std::cout<<"Digi: "<<m1->GetDigiNum(pad)<<"\n";
//				std::cout<<"Charge: "<<m1->GetPadCharge(pad)<<"\n";
//				std::cout<<"NNeighbors: "<<m1->GetNeighborsNum(pad)<<"\n";
//				std::cout<<"NGoodNeighbors: "<<m1->GetGoodNeighborsNum(pad)<<"\n";
//				std::cout<<"   neighbors:   ";
//				for(Int_t i = 0; i < m1->GetGoodNeighborsNum(pad); i++)
//				{
//					std::cout<<m1->GetNeighbor(pad, i)<<";   ";
//				}
//				std::cout<<"\nChannelID: "<<m1->GetChannelID(pad)<<"\n";
//				subCluster->CbmClusteringSetPad(fClustersA1->GetPadInCluster(iCl, iPad), m1->GetX0(iPad), m1->GetY0(iPad), m1->GetDx(iPad), m1->GetDy(iPad),
//						m1->GetPhi1(iPad), m1->GetPhi2(iPad), m1->GetR1(iPad), m1->GetR2(iPad), m1->GetDigiNum(iPad), m1->GetPadCharge(iPad),
//						m1->GetNeighborsNum(iPad), m1->GetGoodNeighborsNum(iPad), m1->GetNeighbors(iPad), m1->GetChannelID(iPad));
				std::cout<<"---SetPad "<<pad<<"; nDigi: "<<m1->GetDigiNum(pad)<<"\n";
				subCluster->CbmClusteringSetPad(pad, m1->GetX0(pad), m1->GetY0(pad), m1->GetDx(pad), m1->GetDy(pad),
						m1->GetPhi1(pad), m1->GetPhi2(pad), m1->GetR1(pad), m1->GetR2(pad), m1->GetDigiNum(pad), m1->GetPadCharge(pad),
						m1->GetChannelID(pad));
				for(Int_t i = 0; i < m1->GetGoodNeighborsNum(pad); i++)
				{
					Bool_t f = 0;
					for(Int_t j = 0; j < fClustersA1->GetNofPads(iCl); j++)
					{
						//std::cout<<"->Pad: "<<fClustersA1->GetNPadInCluster(iCl, j)<<" --- Neigh: "<<m1->GetNeighbor(pad, i)<<"\n";
						if(m1->GetNeighbor(pad, i) == fClustersA1->GetNPadInCluster(iCl, j))
						{
							subCluster->AddPadNeighbor(pad, m1->GetNeighbor(pad, i));
							f = 1;
							//continue;
						}
					}
					//if(f == 1)continue;
				}
			}
			std::cout<<"mP1: "<<maxPad<<"\n";
			maxPad = fClustersA1->GetNPadInCluster(iCl, maxPad);
			std::cout<<"mP2: "<<maxPad<<"\n";
			UInt_t newCharge = 1;
			if(therdPadCharge > 1)
			{
				newCharge = therdPadCharge - 1;
			}
			std::cout<<"maxPad: "<<maxPad<<"; newCharge: "<<newCharge<<"\n";
			subCluster->SetPadCharge(maxPad, newCharge);
			CbmClusteringA1* clustersA1 = new CbmClusteringA1(subCluster);
			std::cout<<"---Start clustering in cluster "<<iCl<<"---\n";
			clustersA1->MainClusteringA1(subCluster, Ver);
			std::cout<<"---Stop Clustering---\n";
			std::cout<<"nClusters: "<<clustersA1->GetNofClusters()<<"\n";
			//if(fClustersA1->GetNofClusters() != 0)std::cout<<"Module:"<<m2->GetDetectorId()<<" - A1.v"<<Ver<<": nofClusters: "<<fClustersA1->GetNofClusters()<<"\n";
			fNofClusters += clustersA1->GetNofClusters();
			for(Int_t jCl = 0; jCl < clustersA1->GetNofClusters(); jCl++){
				digiIndices.clear();
				x = clustersA1->GetX0(jCl);
				y = clustersA1->GetY0(jCl);
			for(Int_t iPad = 0; iPad < clustersA1->GetNofPads(jCl); iPad++)
			{
				Int_t iDigi = clustersA1->GetPadInCluster(jCl, iPad);
				std::cout<<"jCl: "<<jCl<<"; iPad: "<<iPad<<"; nPad: "<<clustersA1->GetNPadInCluster(jCl, iPad)<<"; iDigi: "<<iDigi<<"\n";
				const CbmMuchDigi* muchDigi = static_cast<const CbmMuchDigi*>(fMuchDigi->At(iDigi));
				digiIndices.push_back(iDigi);
				Long64_t channelId = muchDigi->GetChannelId();
				std::cout<<"iPad: "<<iPad<<"; ChID: "<<channelId<<"\n";
				pad = m2->GetPad(channelId);
				t   = muchDigi->GetTime();
				q   = muchDigi->GetADCCharge();
				dx  = pad->GetDx();
				dy  = pad->GetDy();
				dxy = pad->GetDxy();
				dt = muchDigi->GetDTime();
				sumq    += q;
				sumt    += q*t;
				sumdx2  += q*q*dx*dx;
				sumdy2  += q*q*dy*dy;
				sumdxy2 += q*q*dxy*dxy;
				sumdt2  += q*q*dt*dt;
			}
			t   = sumt/sumq;
			dx  = sqrt(sumdx2/12)/sumq;
			dy  = sqrt(sumdy2/12)/sumq;
			dxy = sqrt(sumdxy2/12)/sumq;
			dt = sqrt(sumdt2)/sumq;
			Int_t nCluster = fCluster->GetEntriesFast();
			new ((*fCluster)[nCluster]) CbmMuchCluster(digiIndices);
			Int_t planeId = fScheme->GetLayerSideNr(detId);
			Int_t nHit = fHit->GetEntriesFast();
			new ((*fHit)[nHit]) CbmMuchPixelHit(detId, x, y, z, dx, dy, 0, dxy, nCluster, planeId, t, dt);}
			delete subCluster;
			delete clustersA1;*/
			//-------------------------------------------------------------------
//			CbmClusteringA1* clustersA1;
//			CbmClusteringGeometry* subCluster = new CbmClusteringGeometry;
//			for(Int_t i = 0; i < fClustersA1->GetNofPads(iCl); i++)
//			{
//				const CbmMuchDigi* muchDigi1 = static_cast<const CbmMuchDigi*>(fMuchDigi->At(fClustersA1->GetPadInCluster(iCl, i)));
//				Long64_t channelId = muchDigi1->GetChannelId();
//				CbmMuchPad* pad1 = m2->GetPad(channelId);
//				CbmMuchPadRadial* pad2 = (CbmMuchPadRadial*)m2->GetPad(channelId);
//				Int_t nPad = m1->GetPadByChannelId(channelId);
//				//std::cout<<"; nPad: "<<nPad<<"; nn: "<<m1->GetGoodNeighborsNum(nPad)<<" >>> ";
//				for(Int_t j = 0; j < m1->GetGoodNeighborsNum(nPad); nPad++)
//				{
//					//Int_t nb = m1->GetNeighbor(nPad, j);
//					//std::cout<<" - "<<m1->GetNeighbor(nPad, j);
//				}
//				//std::cout<<muchDigi1->GetADCCharge();//pad2->GetY();
//				std::cout<<"->Pad: "<<fClustersA1->GetPadInCluster(iCl, i)<<"; charge: "<<muchDigi1->GetADCCharge()<<"; chID: "<<channelId
//							<<"; X: "<<pad2->GetX()<<"; Y: "<<pad2->GetY();
//				std::cout<<"\n";
//			}
		//}
		for(Int_t iPad = 0; iPad < fClustersA1->GetNofPads(iCl); iPad++)
		{
			Int_t iDigi = fClustersA1->GetPadInCluster(iCl, iPad);
			const CbmMuchDigi* muchDigi = static_cast<const CbmMuchDigi*>(fMuchDigi->At(iDigi));
			digiIndices.push_back(iDigi);
			//std::cout<<"-"<<a<<"-";
			Long64_t channelId = muchDigi->GetChannelId();
			pad = m2->GetPad(channelId);
			//if (dx > pad->GetDx()) dx = pad->GetDx();
			//if (dy > pad->GetDy()) dy = pad->GetDy();
			//x   = pad->GetX();
			//y   = pad->GetY();
			t   = muchDigi->GetTime();
			q   = muchDigi->GetADCCharge();
			dx  = pad->GetDx();
			dy  = pad->GetDy();
			dxy = pad->GetDxy();
			dt = muchDigi->GetDTime();
			sumq    += q;
			//sumx    += q*x;
			//sumy    += q*y;
			sumt    += q*t;
			sumdx2  += q*q*dx*dx;
			sumdy2  += q*q*dy*dy;
			sumdxy2 += q*q*dxy*dxy;
			sumdt2  += q*q*dt*dt;
		}
		t   = sumt/sumq;
		dx  = sqrt(sumdx2/12)/sumq;
		dy  = sqrt(sumdy2/12)/sumq;
		dxy = sqrt(sumdxy2/12)/sumq;
		dt = sqrt(sumdt2)/sumq;
		//std::cout<<"; qMax: "<<qMax<<"; SumCharge; "<<sumCharge<<"; Size: "<<digiIndices.size()<<"\n";
		Int_t nCluster = fCluster->GetEntriesFast();
		new ((*fCluster)[nCluster]) CbmMuchCluster(digiIndices);
		//---
		//Double_t sigmaX = dx / TMath::Sqrt(12.);
		//Double_t sigmaY = dy / TMath::Sqrt(12.);
		Int_t planeId = fScheme->GetLayerSideNr(detId);
		Int_t nHit = fHit->GetEntriesFast();
		/*std::cout<<"\nCluster: "<<nHit<<"; detId: "<<detId<<"\n";
		std::cout<<"x: "<<x<<"; y: "<<y<<"; z: "<<z<<"\n";
		std::cout<<"dx: "<<dx<<"; dy: "<<dy<<"; dxy: "<<dxy<<"\n";
		std::cout<<"planeId: "<<planeId<<"; t: "<<t<<"; dt: "<<dt<<"\n";
		std::cout<<"-------\n";*/
		new ((*fHit)[nHit]) CbmMuchPixelHit(detId, x, y, z, dx, dy, 0, dxy, nCluster/*fClustersA1->GetCluster(iCl)*/, planeId, t, dt);
		//---
		/*TVector3 pos, dpos;
		pos.SetXYZ(x, y, z);
		dpos.SetXYZ(sigmaX, sigmaY, 0.);
		new ((*fStrawHit)[nHit]) CbmMuchStrawHit(detId, pos, dpos, 0);*/
		//---
		/*new ((*fClusters)[nClusters++]) CbmMuchClFull(fClustersA1->GetX0(iCl),
				fClustersA1->GetY0(iCl), fClustersA1->GetClCharge(iCl),
				fClustersA1->GetNofPads(iCl), fClustersA1->GetPads(iCl),
				layerSideF->GetDetectorId(), m1->GetDetId());*/
	}
	delete fClustersA1;
}

void CbmMuchClustering::SubclusteringA1(Int_t iCl, CbmClusteringGeometry* m1, CbmMuchModuleGem* m2, Int_t Ver, CbmClusteringA1* clA1, Int_t it1)
{
	std::cout<<"Cl: "<<iCl<<"; Pads: "<<clA1->GetNofPads(iCl)<<"\n";
	CbmClusteringGeometry* subCluster = new CbmClusteringGeometry(m1, clA1->GetNofPads(iCl));
	vector<Int_t> digiIndices;
	map<Int_t, Int_t> padNumbers;
	Double_t sumq=0, sumx=0, sumy=0, sumt=0, sumdx2=0, sumdy2=0, sumdxy2=0, sumdt2=0;
	Double_t q=0,x=0,y=0,t=0,z=0,dx=0,dy=0,dxy=0,dt=0;
	CbmMuchPad* pad = NULL;
	Int_t detId = m2->GetDetectorId();
	Int_t maxPad = 0;
	UInt_t maxPadCharge = clA1->GetPadCharge(iCl, 0);
	UInt_t secondPadCharge = 0;
	UInt_t therdPadCharge = 0;
	for(Int_t iPad = 0; iPad < clA1->GetNofPads(iCl); iPad++)
	{
		padNumbers[clA1->GetNPadInCluster(iCl, iPad)] = iPad;
	}
	for(Int_t iPad = 0; iPad < clA1->GetNofPads(iCl); iPad++)
	{
		if(clA1->GetPadCharge(iCl, iPad) > maxPadCharge)
		{
			therdPadCharge = secondPadCharge;
			secondPadCharge = maxPadCharge;
			maxPadCharge = clA1->GetPadCharge(iCl, iPad);
			maxPad = iPad;
		}
		std::cout<<"=>Cl: "<<iCl<<"\n";
		std::cout<<"Charges: "<<maxPadCharge<<" --- "<<secondPadCharge<<" --- "<<therdPadCharge<<"\n";
		Int_t pad = clA1->GetNPadInCluster(iCl, iPad);
		//padNumbers[clA1->GetNPadInCluster(iCl, iPad)] = iPad;
		std::cout<<"---SetPad "<<padNumbers[pad]<<"; nDigi: "<<m1->GetDigiNum(pad)<<"\n";
		subCluster->CbmClusteringSetPad(/*pad*/padNumbers[pad], m1->GetX0(pad), m1->GetY0(pad), m1->GetDx(pad), m1->GetDy(pad),
				m1->GetPhi1(pad), m1->GetPhi2(pad), m1->GetR1(pad), m1->GetR2(pad), m1->GetDigiNum(pad), m1->GetPadCharge(pad),
				m1->GetChannelID(pad));
		for(Int_t i = 0; i < m1->GetGoodNeighborsNum(pad); i++)
		{
			Bool_t f = 0;
			for(Int_t j = 0; j < clA1->GetNofPads(iCl); j++)
			{
				//std::cout<<"->Pad: "<<fClustersA1->GetNPadInCluster(iCl, j)<<" --- Neigh: "<<m1->GetNeighbor(pad, i)<<"\n";
				if(m1->GetNeighbor(pad, i) == clA1->GetNPadInCluster(iCl, j))
				{
					std::cout<<"---pp: "<<padNumbers[pad]<<"; nn: "<<padNumbers[m1->GetNeighbor(pad, i)]<<"\n";
					subCluster->AddPadNeighbor(padNumbers[pad], padNumbers[m1->GetNeighbor(pad, i)]);
					f = 1;
					//continue;
				}
			}
			//if(f == 1)continue;
		}
	}
	std::cout<<"mP1: "<<maxPad<<"\n";
	maxPad = padNumbers[clA1->GetNPadInCluster(iCl, maxPad)];
	std::cout<<"mP2: "<<maxPad<<"\n";
	UInt_t newCharge = 1;
	if(therdPadCharge > 1)
	{
		newCharge = therdPadCharge - 1;
	}
	std::cout<<"maxPad: "<<maxPad<<"; newCharge: "<<newCharge<<"\n";
	subCluster->SetPadCharge(maxPad, newCharge);
	//-CbmClusteringA1* clustersA1 = new CbmClusteringA1(subCluster);
	std::cout<<"---Start clustering in cluster "<<iCl<<"---\n";
	//-clustersA1->MainClusteringA1(subCluster, Ver);
	CbmClusteringWard* clustersA1 = new CbmClusteringWard(subCluster, 100);
	clustersA1->WardMainFunction(subCluster, 1000);
	std::cout<<"---Stop Clustering---\n";
	std::cout<<"nClusters: "<<clustersA1->GetNofClusters()<<"; nofPads: "<<clustersA1->GetNofPads()<<"; nofAPAds: "<<clustersA1->GetNofAPads()<<"\n";
	//if(fClustersA1->GetNofClusters() != 0)std::cout<<"Module:"<<m2->GetDetectorId()<<" - A1.v"<<Ver<<": nofClusters: "<<fClustersA1->GetNofClusters()<<"\n";
	fNofClusters += clustersA1->GetNofClusters();
	for(Int_t jCl = 0; jCl < clustersA1->GetNofClusters(); jCl++){
		/*if(clustersA1->GetNofPads(jCl) > 4)
		{
			std::cout<<"<><><>jCl: "<<jCl<<"; nofPads: "<<clustersA1->GetNofPads(jCl)<<"\n";
			Int_t it2 = it1 + 1;
			if(it2 < 5){
			SubclusteringA1(jCl, m1, m2, Ver, clA1, it2);}
		}*/
		digiIndices.clear();
		x = clustersA1->GetX0(jCl);
		y = clustersA1->GetY0(jCl);
		z = m2->GetPosition()[2];
		std::cout<<"_Cl "<<jCl<<"; nofPads: "<<clustersA1->GetNofPads(jCl)<<"\n";
	for(Int_t iPad = 0; iPad < clustersA1->GetNofPads(jCl); iPad++)
	{
		Int_t iDigi = clustersA1->GetPadInCluster(jCl, iPad);
		//-std::cout<<"jCl: "<<jCl<<"; iPad: "<<iPad<<"; nPad: "<<clustersA1->GetNPadInCluster(jCl, iPad)<<"; iDigi: "<<iDigi<<"\n";
		const CbmMuchDigi* muchDigi = static_cast<const CbmMuchDigi*>(fMuchDigi->At(iDigi));
		digiIndices.push_back(iDigi);
		Long64_t channelId = muchDigi->GetChannelId();
		std::cout<<"iPad: "<<iPad<<"; ChID: "<<channelId<<"\n";
		pad = m2->GetPad(channelId);
		t   = muchDigi->GetTime();
		q   = muchDigi->GetADCCharge();
		dx  = pad->GetDx();
		dy  = pad->GetDy();
		dxy = pad->GetDxy();
		dt = muchDigi->GetDTime();
		sumq    += q;
		sumt    += q*t;
		sumdx2  += q*q*dx*dx;
		sumdy2  += q*q*dy*dy;
		sumdxy2 += q*q*dxy*dxy;
		sumdt2  += q*q*dt*dt;
	}
	t   = sumt/sumq;
	dx  = sqrt(sumdx2/12)/sumq;
	dy  = sqrt(sumdy2/12)/sumq;
	dxy = sqrt(sumdxy2/12)/sumq;
	dt = sqrt(sumdt2)/sumq;
	std::cout<<">>>d calculated.\n";
	Int_t nCluster = fCluster->GetEntriesFast();
	new ((*fCluster)[nCluster]) CbmMuchCluster(digiIndices);
	Int_t planeId = fScheme->GetLayerSideNr(detId);
	Int_t nHit = fHit->GetEntriesFast();
	new ((*fHit)[nHit]) CbmMuchPixelHit(detId, x, y, z, dx, dy, 0, dxy, nCluster, planeId, t, dt);}
	delete clustersA1;
	delete subCluster;
}

void CbmMuchClustering::ClusteringSL(CbmClusteringGeometry* m1, CbmMuchModuleGem* m2, Int_t Ver/*, Int_t &nHit, Int_t &nCluster*/)
{
	fClustersSL = new CbmClusteringSL(m1);
	fClustersSL->MainClusteringSL(m1, Ver);
	fNofClusters += fClustersSL->GetNofClusters();
	//std::cout<<"Module:"<<m2->GetDetectorId()<<" - SL.v2: nofClusters: "<<fClustersSL->GetNofClusters()<<"\n";
	/*for(Int_t iCl = 0; iCl < fClustersSL->GetNofClusters(); iCl++)
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
			std::cout<<iPad<<"; ChId: "<<channelId<<"; iCl: "<<iCl<<"\n";
			pad = m2->GetPad(channelId);
			if (dx > pad->GetDx()) dx = pad->GetDx();
			if (dy > pad->GetDy()) dy = pad->GetDy();
		}
		//std::cout<<"; qMax: "<<qMax<<"; SumCharge; "<<sumCharge<<"; Size: "<<digiIndices.size()<<"\n";
		Int_t nCluster = fCluster->GetEntriesFast();
		new ((*fCluster)[nCluster]) CbmMuchCluster(digiIndices);
		//---
		Double_t sigmaX = dx / TMath::Sqrt(12.);
		Double_t sigmaY = dy / TMath::Sqrt(12.);
		Int_t planeId = fScheme->GetLayerSideNr(detId);
		Int_t nHit = fHit->GetEntriesFast();
		new ((*fHit)[nHit]) CbmMuchPixelHit(detId, x, y, z, sigmaX, sigmaY, 0, 0, fClustersSL->GetCluster(iCl), planeId, 0, 0);
		//---
	}*/
	for(Int_t iCl = 0; iCl < fClustersSL->GetNofClusters(); iCl++)
	{
		vector<Int_t> digiIndices;
		//---
		Int_t detId = m2->GetDetectorId();
		Double_t sumq=0, sumx=0, sumy=0, sumt=0, sumdx2=0, sumdy2=0, sumdxy2=0, sumdt2=0;
		Double_t q=0,x=0,y=0,t=0,z=0,dx=0,dy=0,dxy=0,dt=0;
		x = fClustersSL->GetX0(iCl);
		y = fClustersSL->GetY0(iCl);
		z = m2->GetPosition()[2];
		//Double_t dx = std::numeric_limits<Double_t>::max();
		//Double_t dy = std::numeric_limits<Double_t>::max();
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
			//if (dx > pad->GetDx()) dx = pad->GetDx();
			//if (dy > pad->GetDy()) dy = pad->GetDy();
			//x   = pad->GetX();
			//y   = pad->GetY();
			t   = muchDigi->GetTime();
			q   = muchDigi->GetADCCharge();
			dx  = pad->GetDx();
			dy  = pad->GetDy();
			dxy = pad->GetDxy();
			dt = muchDigi->GetDTime();
			sumq    += q;
			//sumx    += q*x;
			//sumy    += q*y;
			sumt    += q*t;
			sumdx2  += q*q*dx*dx;
			sumdy2  += q*q*dy*dy;
			sumdxy2 += q*q*dxy*dxy;
			sumdt2  += q*q*dt*dt;
		}
		t   = sumt/sumq;
		dx  = sqrt(sumdx2/12)/sumq;
		dy  = sqrt(sumdy2/12)/sumq;
		dxy = sqrt(sumdxy2/12)/sumq;
		dt = sqrt(sumdt2)/sumq;
		//std::cout<<"; qMax: "<<qMax<<"; SumCharge; "<<sumCharge<<"; Size: "<<digiIndices.size()<<"\n";
		Int_t nCluster = fCluster->GetEntriesFast();
		new ((*fCluster)[nCluster]) CbmMuchCluster(digiIndices);
		//---
		//Double_t sigmaX = dx / TMath::Sqrt(12.);
		//Double_t sigmaY = dy / TMath::Sqrt(12.);
		Int_t planeId = fScheme->GetLayerSideNr(detId);
		Int_t nHit = fHit->GetEntriesFast();
		new ((*fHit)[nHit]) CbmMuchPixelHit(detId, x, y, z, dx, dy, 0, dxy, nCluster/*fClustersSL->GetCluster(iCl)*/, planeId, t, dt);
	}
	delete fClustersSL;
}

void CbmMuchClustering::ClusteringWard(CbmClusteringGeometry* m1, CbmMuchModuleGem* m2)
{
	fClustersWard = new CbmClusteringWard(m1, 1000);
	fClustersWard->WardMainFunction(m1, 5000);
	//if(fClustersA1->GetNofClusters() != 0)std::cout<<"Module:"<<m2->GetDetectorId()<<" - A1.v"<<Ver<<": nofClusters: "<<fClustersA1->GetNofClusters()<<"\n";
	fNofClusters += fClustersWard->GetNofClusters();
	for(Int_t iCl = 0; iCl < fClustersWard->GetNofClusters(); iCl++)
	{
		vector<Int_t> digiIndices;
		//---
		std::cout<<"=>Cluster "<<iCl<<"\n";
		Int_t detId = m2->GetDetectorId();
		Double_t sumq=0, sumx=0, sumy=0, sumt=0, sumdx2=0, sumdy2=0, sumdxy2=0, sumdt2=0;
		Double_t q=0,x=0,y=0,t=0,z=0,dx=0,dy=0,dxy=0,dt=0;
		x = fClustersWard->GetX0(iCl);
		y = fClustersWard->GetY0(iCl);
		z = m2->GetPosition()[2];
		//Double_t dx = std::numeric_limits<Double_t>::max();
		//Double_t dy = std::numeric_limits<Double_t>::max();
		CbmMuchPad* pad = NULL;
		//std::cout<<"->iCl: "<<iCl<<"; Npads: "<<fClustersA1->GetNofPads(iCl);
		fhDigisInCluster->Fill(fClustersWard->GetNofPads(iCl));
		/*if(fClustersWard->GetNofPads(iCl) > 4)
		{
			std::cout<<"Cl: "<<iCl<<"; Pads: "<<fClustersWard->GetNofPads(iCl)<<"\n";
			for(Int_t i = 0; i < fClustersWard->GetNofPads(iCl); i++)
			{
				const CbmMuchDigi* muchDigi1 = static_cast<const CbmMuchDigi*>(fMuchDigi->At(fClustersWard->GetPadInCluster(iCl, i)));
				Long64_t channelId = muchDigi1->GetChannelId();
				CbmMuchPad* pad1 = m2->GetPad(channelId);
				CbmMuchPadRadial* pad2 = (CbmMuchPadRadial*)m2->GetPad(channelId);
				//std::cout<<muchDigi1->GetADCCharge();//pad2->GetY();
				std::cout<<"->Pad: "<<fClustersWard->GetPadInCluster(iCl, i)<<"; charge: "<<muchDigi1->GetADCCharge()<<"; chID: "<<channelId
						<<"; X: "<<pad2->GetX()<<"; Y: "<<pad2->GetY();
				Int_t nPad = m1->GetPadByChannelId(channelId);
				//std::cout<<"; nPad: "<<nPad<<"; nn: "<<m1->GetGoodNeighborsNum(nPad)<<" >>> ";
				for(Int_t j = 0; j < m1->GetGoodNeighborsNum(nPad); nPad++)
				{
					//Int_t nb = m1->GetNeighbor(nPad, j);
					//std::cout<<" - "<<m1->GetNeighbor(nPad, j);
				}
				std::cout<<"\n";
			}
		}*/
		for(Int_t iPad = 0; iPad < fClustersWard->GetNofPads(iCl); iPad++)
		{
			Int_t iDigi = fClustersWard->GetPadInCluster(iCl, iPad);
			std::cout<<"iDigi: "<<iDigi<<"\n";
			const CbmMuchDigi* muchDigi = static_cast<const CbmMuchDigi*>(fMuchDigi->At(iDigi));
			digiIndices.push_back(iDigi);
			//std::cout<<"-"<<a<<"-";
			Long64_t channelId = muchDigi->GetChannelId();
			pad = m2->GetPad(channelId);
			//if (dx > pad->GetDx()) dx = pad->GetDx();
			//if (dy > pad->GetDy()) dy = pad->GetDy();
			//x   = pad->GetX();
			//y   = pad->GetY();
			t   = muchDigi->GetTime();
			q   = muchDigi->GetADCCharge();
			dx  = pad->GetDx();
			dy  = pad->GetDy();
			dxy = pad->GetDxy();
			dt = muchDigi->GetDTime();
			sumq    += q;
			//sumx    += q*x;
			//sumy    += q*y;
			sumt    += q*t;
			sumdx2  += q*q*dx*dx;
			sumdy2  += q*q*dy*dy;
			sumdxy2 += q*q*dxy*dxy;
			sumdt2  += q*q*dt*dt;
		}
		t   = sumt/sumq;
		dx  = sqrt(sumdx2/12)/sumq;
		dy  = sqrt(sumdy2/12)/sumq;
		dxy = sqrt(sumdxy2/12)/sumq;
		dt = sqrt(sumdt2)/sumq;
		//std::cout<<"; qMax: "<<qMax<<"; SumCharge; "<<sumCharge<<"; Size: "<<digiIndices.size()<<"\n";
		Int_t nCluster = fCluster->GetEntriesFast();
		new ((*fCluster)[nCluster]) CbmMuchCluster(digiIndices);
		//---
		//Double_t sigmaX = dx / TMath::Sqrt(12.);
		//Double_t sigmaY = dy / TMath::Sqrt(12.);
		Int_t planeId = fScheme->GetLayerSideNr(detId);
		Int_t nHit = fHit->GetEntriesFast();
		/*std::cout<<"\nCluster: "<<nHit<<"; detId: "<<detId<<"\n";
		std::cout<<"x: "<<x<<"; y: "<<y<<"; z: "<<z<<"\n";
		std::cout<<"dx: "<<dx<<"; dy: "<<dy<<"; dxy: "<<dxy<<"\n";
		std::cout<<"planeId: "<<planeId<<"; t: "<<t<<"; dt: "<<dt<<"\n";
		std::cout<<"-------\n";*/
		new ((*fHit)[nHit]) CbmMuchPixelHit(detId, x, y, z, dx, dy, 0, dxy, nCluster/*fClustersA1->GetCluster(iCl)*/, planeId, t, dt);
		//---
		/*TVector3 pos, dpos;
		pos.SetXYZ(x, y, z);
		dpos.SetXYZ(sigmaX, sigmaY, 0.);
		new ((*fStrawHit)[nHit]) CbmMuchStrawHit(detId, pos, dpos, 0);*/
		//---
		/*new ((*fClusters)[nClusters++]) CbmMuchClFull(fClustersA1->GetX0(iCl),
				fClustersA1->GetY0(iCl), fClustersA1->GetClCharge(iCl),
				fClustersA1->GetNofPads(iCl), fClustersA1->GetPads(iCl),
				layerSideF->GetDetectorId(), m1->GetDetId());*/
	}
	delete fClustersWard;
}
ClassImp(CbmMuchClustering);
