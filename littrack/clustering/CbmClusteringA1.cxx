/*
 * CbmClusteringA1.cxx
 *
 *  Created on: Apr 4, 2012
 *      Author: kozlov
 */

#include "CbmClusteringGeometry.h"
#include "CbmStsHit.h"
#include "CbmStsPoint.h"
#include "CbmMuchPixelHit.h"
#include "CbmMuchPoint.h"
#include "CbmMCTrack.h"
#include "CbmMuchGeoScheme.h"
#include "CbmMuchLayerSide.h"
#include "CbmMuchModuleGem.h"
#include "CbmMuchPad.h"
#include "CbmMuchDigi.h"
#include "CbmClusteringA1.h"

#include "FairRootManager.h"

//#include "TClonesArray.h"

#include <iostream>
#include <cassert>

using std::cout;
using std::endl;

CbmClusteringA1::CbmClusteringA1():
		fA1(),
		fA2(),
		fS(),
		fClusters(),
		fNumbersOfPads()
{
	fNofPads = 0;
	fNofActivePads = 0;
	fNofClusters = 0;
}

CbmClusteringA1::CbmClusteringA1(CbmClusteringGeometry* moduleGeo):
		fClusters()
{
	fNofPads = moduleGeo->GetNPads();
	fNofActivePads = moduleGeo->GetAPadsNom();
	fNofClusters = fNofActivePads;
	fA1 = new UInt_t [fNofPads];
	fA2 = new UInt_t [fNofPads];
	fS = new Bool_t [fNofPads];
	fNumbersOfPads = new Int_t [fNofPads];
	Int_t nom = 0;
	for(Int_t iPad = 0; iPad < fNofPads; iPad++)
	{
		fA1[iPad] = moduleGeo->GetPadCharge(iPad);
		fA2[iPad] = 0;
		fS[iPad] = 0;
		fNumbersOfPads[iPad] = 0;
		if(fA1[iPad] > 0)
		{
			fS[iPad] = 1;
			nom++;
			fNumbersOfPads[iPad] = nom;
		}
	}
}

CbmClusteringA1::~CbmClusteringA1()
{
	delete [] fA1;
	delete [] fA2;
	delete [] fS;
	delete [] fNumbersOfPads;
	delete [] fClusters;
	/*if (fClFull) {
		fClFull->Delete();
	    delete fClFull;
	  }*/
}

void CbmClusteringA1::MainClusteringA1(CbmClusteringGeometry* moduleGeo, Int_t algVersion)
{
	//algVersion == 1 -> all neighbors
	//algVersion == 2 -> only good neighbors
	Int_t localMaximum;
	Int_t temp;
	UInt_t temp1;
	for(Int_t iPad = 0; iPad < fNofPads; iPad++)
	{
		//Proverka dostato4nosti amplitudi i sushestvovani9 klastera
		if(fA1[iPad] > 0)
		{
			//Prisvoenie 94eikoi sebe nomera lokal'nogo maximuma
			localMaximum = iPad;
			//Opredelenie novogo lokal'nogo maximuma sredi sosedei
			if(algVersion == 1){
				//std::cout<<"neighbors: "<<moduleGeo->GetNeighborsNum(iPad)<<"\n";
				/*for(Int_t nPad = 0; nPad < moduleGeo->GetNeighborsNum(iPad); nPad++)
				{
					if(fA1[moduleGeo->GetNeighbor(iPad, nPad)] > fA1[localMaximum])
					{
						localMaximum = moduleGeo->GetNeighbor(iPad, nPad);
						//std::cout<<"Get Neighbour\n";
						//std::cout<<iPad<<" -> "<<moduleGeo->GetNeighbor(iPad, nPad)<<"\n";
					}
				}*/
				std::cout<<"SORRY, NOT WORKING NOW.\n";
			}
			if(algVersion == 2){
				for(Int_t nPad = 0; nPad < moduleGeo->GetGoodNeighborsNum(iPad); nPad++)
				{
					//std::cout<<"NN: "<<moduleGeo->GetGoodNeighborsNum(iPad)<<"\n";
					if(fA1[moduleGeo->GetNeighbor(iPad, nPad)] > fA1[localMaximum])
					{
						localMaximum = moduleGeo->GetNeighbor(iPad, nPad);
					}
				}
			}
			if((algVersion != 1)&&(algVersion != 2))
			{
				std::cout<<"Error! Unsupported version of the algorithm.\n";
			}
			//temp1 = fA2[iPad];
			//Perestroenie matritsi amplitud
			//std::cout<<"Pad: "<<iPad<<"; Max: "<<localMaximum<<";\n";
			//std::cout<<"xP: "<<moduleGeo->GetX0(iPad)<<"; yP: "<<moduleGeo->GetY0(iPad)<<"\n\n";
			fA2[localMaximum] += fA1[iPad];
			//activePad1[iPad] = 0;
			//Возможно, следует добавить fA2[iPad] -= fA1[iPad]; - ???
			//fA2[iPad] = 0;
			//Ymen'shenie 4isla klasterov
			if(iPad != localMaximum)
			{
				//fA2[iPad] = temp1; //Перепроверить - ???
				fA2[iPad] = 0;
				fNofActivePads--;
				//Ob'edinenie klasterov
				/*temp = fNumbersOfPads[iPad];
				for(Int_t nPad = 0; nPad < fNofPads; nPad++)
				{
					if(fNumbersOfPads[nPad] == temp)
					{
						fNumbersOfPads[nPad] = fNumbersOfPads[localMaximum];
					}
				}*/
				ChangeClusters(moduleGeo, iPad, fNumbersOfPads[iPad], fNumbersOfPads[localMaximum]);
			}
		}
		/* Perestroika matritsi amplitud vo vrem9 raboti algoritma
		 * TRUE - mnogo prisoedinenii uvili4ivayt klaster
		 * FALSE - bol'shoi tsentr mass uvili4ivart klaster
		 */
		/*if(version == true)
		{
			for(Int_t i = 1; i <= fNofPads; i++)
			{
				fA1[i] = fA2[i];
			}
		}*/
	}
	Int_t iter = 0;
	/*for(Int_t iPad = 0; iPad < fNofPads; iPad++)
	{
		if(fS[iPad] == 1)
		{
			iter++;
			std::cout<<"N: "<<iter<<"; Pad: "<<iPad<<"; nofPad: "<<fNumbersOfPads[iPad]<<"; Charge1: "<<fA1[iPad]<<"; Charge2: "<<fA2[iPad]<<"\n";
		}
	}*/
	//std::cout<<"fNofActivePads before: "<<fNofActivePads<<"\n";

	//delete [] fClusters;
	fClusters = new Cluster[(Int_t)(fNofActivePads)];
	for(Int_t iPad = 0; iPad < (Int_t)(fNofActivePads); iPad++)
	{
		fClusters[iPad].fNCluster = 0;
		fClusters[iPad].fNofPads = 0;
		fClusters[iPad].fCharge = 0;
		fClusters[iPad].fX = 0;
		fClusters[iPad].fY = 0;
		fClusters[iPad].fPadsInCluster.clear();
		fClusters[iPad].fNPadsInCluster.clear();
		fClusters[iPad].fPadsCharges.clear();
	}

	Int_t nomCl = 0;
	Int_t Replase = 0;
	for(Int_t iPad = 0; iPad < fNofPads; iPad++)
	{
		if((fA2[iPad] != 0) && (fS[iPad] == 1))
		//if(activePad1[iPad] != 0)
		{
			Replase = fNumbersOfPads[iPad];
			nomCl++;
			Int_t padInCluster = 0;
Int_t it = 0;
			for(Int_t nPad = 0; nPad < fNofPads; nPad++)
			{
				if((fNumbersOfPads[nPad] == Replase) &&
				(moduleGeo->GetPadCharge(nPad) > 0) && (fS[nPad] == 1))
				{
					//Renumerovka klastera i ego 94eek
					fNumbersOfPads[nPad] = nomCl;
					fS[nPad] = 0;
					fClusters[nomCl - 1].fNCluster = nomCl;
					//std::cout<<"X0: "<<moduleGeo->GetX0(nPad)<<"\n";
					//Float_t x = moduleGeo->GetX0(nPad);
					//fClusters[nomCl].xc += x;
					//std::cout<<"\nXC1 = "<<fClusters[nomCl - 1].xc<<"\n";
					//std::cout<<" += "<<moduleGeo->GetX0(nPad)<<"\n";
					fClusters[nomCl - 1].fX += (moduleGeo->GetX0(nPad) * moduleGeo->GetPadCharge(nPad));
					//std::cout<<"XC2 = "<<fClusters[nomCl - 1].xc<<"\n";
					fClusters[nomCl - 1].fY += (moduleGeo->GetY0(nPad) * moduleGeo->GetPadCharge(nPad));
					fClusters[nomCl - 1].fCharge += moduleGeo->GetPadCharge(nPad);
					//fClusters[nomCl - 1].padsInCluster[padInCluster] = moduleGeo->GetDigiNum(nPad);
					//std::cout<<">>>>>Pad: "<<nPad<<"; Push digi: "<<moduleGeo->GetDigiNum(nPad)<<"; ";
					fClusters[nomCl - 1].fPadsInCluster.push_back(moduleGeo->GetDigiNum(nPad));
					//std::cout<<"Pushed digi: "<<fClusters[nomCl - 1].fPadsInCluster[it]<<"\n";
					fClusters[nomCl - 1].fNPadsInCluster.push_back(nPad);
					fClusters[nomCl - 1].fPadsCharges.push_back(fA1[nPad]);
					padInCluster++;
					fClusters[nomCl - 1].fNofPads = padInCluster;
it++;
					//std::cout<<"nPad: "<<nPad<<"; A: "<<fA2[nPad]<<"\n";
				}
			}
			//std::cout<<"iPad: "<<iPad<<"; A: "<<fA2[iPad]<<"\n";
		}
	}
	//std::cout<<"fNofActivePads after: "<<fNofActivePads<<"\n";
	//std::cout<<"nomCl: "<<nomCl<<"\n";
	//Vichislenie koordinat tsentra klastera Ch2
	for(Int_t iCl = 0; iCl < nomCl; iCl++)
	{
		//-!!!DELENIE!!!-
		if(fClusters[iCl].fCharge == 0)
		{
			cout<<"DIVISION ON NULL";
			break;
		}
		fClusters[iCl].fX = fClusters[iCl].fX / fClusters[iCl].fCharge;
		fClusters[iCl].fY = fClusters[iCl].fY / fClusters[iCl].fCharge;
	}
	fNofClusters = nomCl;
	/*std::cout<<"Found clusters: "<<fNofClusters<<"\n";
	for(Int_t iCl = 0; iCl < fNofClusters; iCl++)
	{
		std::cout<<"Cl Nomber: "<<fClusters[iCl].nofCluster<<"\n";
		std::cout<<"nofPads: "<<fClusters[iCl].nofPads<<"\n";
		std::cout<<"X: "<<fClusters[iCl].xc<<"; Y: "<<fClusters[iCl].yc<<"; Charge: "<<fClusters[iCl].sumClCharge<<"\n";
	}*/
	//---Make more hits---
	/*for(Int_t iCl = 0; iCl < fNofClusters; iCl++)
	{
		if(fClusters[iCl].fNofPads > 3)
		{
		Int_t maxPad = -1;
		Float_t maxPadCharge = 0;
		//std::cout<<"---Cl: "<<iCl<<"\n";
		for(Int_t iPad = 0; iPad < fClusters[iCl].fNofPads; iPad++)
		{
			if(fClusters[iCl].fPadsCharges[iPad] > maxPadCharge)
			{
				maxPad = iPad;
				maxPadCharge = fClusters[iCl].fPadsCharges[iPad];
				//std::cout<<"mpc: "<<maxPadCharge<<"\n";
			}
		}
		maxPadCharge = maxPadCharge * 0.3;
		for(Int_t iPad = 0; iPad < fClusters[iCl].fNofPads; iPad++)
		{
			if((fClusters[iCl].fPadsCharges[iPad] > maxPadCharge) && (iPad != maxPad))
			{
				fClusters[nomCl].fCharge = fClusters[iCl].fPadsCharges[iPad];
				fClusters[nomCl].fNCluster = nomCl + 1;
				fClusters[nomCl].fNofPads = 1;
				//fClusters[nomCl].fPadsCharges.clear();
				//fClusters[nomCl].fPadsInCluster.clear();
				fClusters[nomCl].fPadsCharges.push_back(fClusters[iCl].fPadsCharges[iPad]);//[0] = fClusters[iCl].fPadsCharges[iPad];
				fClusters[nomCl].fPadsInCluster.push_back(fClusters[iCl].fPadsInCluster[iPad]);//[0] = 0;//fClusters[iCl].fPadsInCluster[iPad];
				fClusters[nomCl].fX = fClusters[iCl].fX;
				fClusters[nomCl].fY = fClusters[iCl].fY;
				nomCl++;
			}
		}
		}
	}
	fNofClusters = nomCl;*/
	//--------------------
}

void CbmClusteringA1::ChangeClusters(CbmClusteringGeometry* moduleGeo, Int_t nPad, Int_t Cl0, Int_t Cl1)
{
	fNumbersOfPads[nPad] = Cl1;
	for(Int_t iPad = 0; iPad < moduleGeo->GetGoodNeighborsNum(nPad); iPad++)
	{
		if(fNumbersOfPads[moduleGeo->GetNeighbor(nPad, iPad)] == Cl0)
		{
			//fNumbersOfPads[nPad] = Cl1;
			ChangeClusters(moduleGeo, moduleGeo->GetNeighbor(nPad, iPad), Cl0, Cl1);
		}
	}
}

Int_t CbmClusteringA1::GetCluster(Int_t iCluster)
{
	return fClusters[iCluster].fNCluster;
}
Float_t CbmClusteringA1::GetX0(Int_t iCluster)
{
	return fClusters[iCluster].fX;
}
Float_t CbmClusteringA1::GetY0(Int_t iCluster)
{
	return fClusters[iCluster].fY;
}
UInt_t CbmClusteringA1::GetClCharge(Int_t iCluster)
{
	return fClusters[iCluster].fCharge;
}
Int_t CbmClusteringA1::GetNofPads(Int_t iCluster)
{
	return fClusters[iCluster].fNofPads;
}
Int_t CbmClusteringA1::GetPadInCluster(Int_t iCluster, Int_t iPad)
{
	return fClusters[iCluster].fPadsInCluster[iPad];
}

Int_t CbmClusteringA1::GetNPadInCluster(Int_t iCluster, Int_t iPad)
{
	return fClusters[iCluster].fNPadsInCluster[iPad];
}

vector<Int_t> CbmClusteringA1::GetPads(Int_t iCluster)
{
	return fClusters[iCluster].fPadsInCluster;
}

UInt_t CbmClusteringA1::GetPadCharge(Int_t iCluster, Int_t iPad)
{
	return fClusters[iCluster].fPadsCharges[iPad];
}
