/*
 * CbmClusteringA1.cxx
 *
 *  Created on: Apr 4, 2012
 *      Author: kozlov
 */

#include "CbmMuchGeoCl.h"
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

CbmClusteringA1::CbmClusteringA1()
{
	fNofPads = 0;
	fNofActivePads = 0;
	fNofClusters = 0;
}

CbmClusteringA1::CbmClusteringA1(CbmMuchGeoCl* moduleGeo)
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
	delete fA1;
	delete fA2;
	delete fS;
	delete fNumbersOfPads;
	delete fClusters;
	/*if (fClFull) {
		fClFull->Delete();
	    delete fClFull;
	  }*/
}

void CbmClusteringA1::MainClusteringA1(CbmMuchGeoCl* moduleGeo, Int_t algVersion)
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
				for(Int_t nPad = 0; nPad < moduleGeo->GetNeighborsNum(iPad); nPad++)
				{
					/*std::cout<<"WORKED 1\n";
					std::cout<<"iPad: "<<iPad<<"; nPad: "<<nPad<<"; localMaximum: "<<localMaximum<<"\n";
					std::cout<<"fA1_1: "<<fA1[moduleGeo->GetNeighbor(iPad, nPad)];
					std::cout<<"; fA1_2: "<<fA1[localMaximum]<<"\n";*/
					if(fA1[moduleGeo->GetNeighbor(iPad, nPad)] > fA1[localMaximum])
					{
						localMaximum = moduleGeo->GetNeighbor(iPad, nPad);
						//std::cout<<"Get Neighbour\n";
						//std::cout<<iPad<<" -> "<<moduleGeo->GetNeighbor(iPad, nPad)<<"\n";
					}
				}
			}
			if(algVersion == 2){
				for(Int_t nPad = 0; nPad < moduleGeo->GetGoodNeighborsNum(iPad); nPad++)
				{
					if(fA1[moduleGeo->GetGoodNeighbor(iPad, nPad)] > fA1[localMaximum])
					{
						localMaximum = moduleGeo->GetGoodNeighbor(iPad, nPad);
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
				temp = fNumbersOfPads[iPad];
				for(Int_t nPad = 0; nPad < fNofPads; nPad++)
				{
					if(fNumbersOfPads[nPad] == temp)
					{
						fNumbersOfPads[nPad] = fNumbersOfPads[localMaximum];
					}
				}
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

	fClusters = new Cluster[fNofActivePads];
	for(Int_t iPad = 0; iPad < fNofActivePads; iPad++)
	{
		fClusters[iPad].nofCluster = 0;
		fClusters[iPad].nofPads = 0;
		fClusters[iPad].sumClCharge = 0;
		fClusters[iPad].xc = 0;
		fClusters[iPad].yc = 0;
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
			for(Int_t nPad = 0; nPad < fNofPads; nPad++)
			{

				if((fNumbersOfPads[nPad] == Replase) &&
				(moduleGeo->GetPadCharge(nPad) > 0) && (fS[nPad] == 1))
				{
					//Renumerovka klastera i ego 94eek
					fNumbersOfPads[nPad] = nomCl;
					fS[nPad] = 0;
					fClusters[nomCl - 1].nofCluster = nomCl;
					//std::cout<<"X0: "<<moduleGeo->GetX0(nPad)<<"\n";
					//Float_t x = moduleGeo->GetX0(nPad);
					//fClusters[nomCl].xc += x;
					//std::cout<<"\nXC1 = "<<fClusters[nomCl - 1].xc<<"\n";
					//std::cout<<" += "<<moduleGeo->GetX0(nPad)<<"\n";
					fClusters[nomCl - 1].xc += (moduleGeo->GetX0(nPad) * moduleGeo->GetPadCharge(nPad));
					//std::cout<<"XC2 = "<<fClusters[nomCl - 1].xc<<"\n";
					fClusters[nomCl - 1].yc += (moduleGeo->GetY0(nPad) * moduleGeo->GetPadCharge(nPad));
					fClusters[nomCl - 1].sumClCharge += moduleGeo->GetPadCharge(nPad);
					fClusters[nomCl - 1].padsInCluster[padInCluster] = moduleGeo->GetDigiNum(nPad);
					padInCluster++;
					fClusters[nomCl - 1].nofPads = padInCluster;
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
		if(fClusters[iCl].sumClCharge == 0)
		{
			cout<<"DIVISION ON NULL";
			break;
		}
		fClusters[iCl].xc = fClusters[iCl].xc / fClusters[iCl].sumClCharge;
		fClusters[iCl].yc = fClusters[iCl].yc / fClusters[iCl].sumClCharge;
	}
	fNofClusters = nomCl;
	/*std::cout<<"Found clusters: "<<fNofClusters<<"\n";
	for(Int_t iCl = 0; iCl < fNofClusters; iCl++)
	{
		std::cout<<"Cl Nomber: "<<fClusters[iCl].nofCluster<<"\n";
		std::cout<<"nofPads: "<<fClusters[iCl].nofPads<<"\n";
		std::cout<<"X: "<<fClusters[iCl].xc<<"; Y: "<<fClusters[iCl].yc<<"; Charge: "<<fClusters[iCl].sumClCharge<<"\n";
	}*/
}

Int_t CbmClusteringA1::GetCluster(Int_t iCluster)
{
	return fClusters[iCluster].nofCluster;
}
Float_t CbmClusteringA1::GetX0(Int_t iCluster)
{
	return fClusters[iCluster].xc;
}
Float_t CbmClusteringA1::GetY0(Int_t iCluster)
{
	return fClusters[iCluster].yc;
}
UInt_t CbmClusteringA1::GetClCharge(Int_t iCluster)
{
	return fClusters[iCluster].sumClCharge;
}
Int_t CbmClusteringA1::GetNofPads(Int_t iCluster)
{
	return fClusters[iCluster].nofPads;
}
Int_t CbmClusteringA1::GetPadInCluster(Int_t iCluster, Int_t iPad)
{
	return fClusters[iCluster].padsInCluster[iPad];
}

Int_t* CbmClusteringA1::GetPads(Int_t iCluster)
{
	return fClusters[iCluster].padsInCluster;
}
