/*
 * CbmClusteringSL.cxx
 *
 *  Created on: Apr 10, 2012
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
#include "CbmClusteringSL.h"

#include "FairRootManager.h"

#include "TClonesArray.h"

#include <iostream>
#include <cassert>

using std::cout;
using std::endl;

CbmClusteringSL::CbmClusteringSL()
{
	fNofPads = 0;
	fNofActivePads = 0;
	fNofClusters = 0;
}

CbmClusteringSL::CbmClusteringSL(CbmMuchGeoCl* moduleGeo)
{
	fNofPads = moduleGeo->GetNPads();
	fNofActivePads = moduleGeo->GetAPadsNom();
	fNofClusters = fNofActivePads;
	fA1 = new UInt_t [fNofPads];
	fS = new Bool_t [fNofPads];
	fNumbersOfPads = new Int_t [fNofPads];
	Int_t nom = 0;
	for(Int_t iPad = 0; iPad < fNofPads; iPad++)
	{
		fA1[iPad] = moduleGeo->GetPadCharge(iPad);
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

CbmClusteringSL::~CbmClusteringSL()
{
	delete fA1;
	delete fS;
	delete fNumbersOfPads;
	delete fClusters;
}

void CbmClusteringSL::SLRec1(CbmMuchGeoCl* moduleGeo, Int_t activePad)
{
	for(Int_t iPad = 0; iPad < moduleGeo->GetNeighborsNum(activePad); iPad++)
	{
		//Vipoln9ets9 tol'ko dl9 aktivnih sosedei
		if(fS[moduleGeo->GetNeighbor(activePad, iPad)] == 1)
		{
			fNumbersOfPads[moduleGeo->GetNeighbor(activePad, iPad)] = fNumbersOfPads[activePad];
			//Sn9tie aktivnosti 94eiki
			fS[moduleGeo->GetNeighbor(activePad, iPad)] = 0;
			SLRec1(moduleGeo, moduleGeo->GetNeighbor(activePad, iPad));
        }
	}
}

void CbmClusteringSL::SLRec2(CbmMuchGeoCl* moduleGeo, Int_t activePad)
{
	for(Int_t iPad = 0; iPad < moduleGeo->GetGoodNeighborsNum(activePad); iPad++)
	{
		//Vipoln9ets9 tol'ko dl9 aktivnih sosedei
		if(fS[moduleGeo->GetGoodNeighbor(activePad, iPad)] == 1)
		{
			fNumbersOfPads[moduleGeo->GetGoodNeighbor(activePad, iPad)] = fNumbersOfPads[activePad];
			//Sn9tie aktivnosti 94eiki
			fS[moduleGeo->GetGoodNeighbor(activePad, iPad)] = 0;
			SLRec1(moduleGeo, moduleGeo->GetGoodNeighbor(activePad, iPad));
        }
	}
}

void CbmClusteringSL::MainClusteringSL(CbmMuchGeoCl* moduleGeo, Int_t algVersion)
{
	//algVersion == 1 -> all neighbors
	//algVersion == 2 -> only good neighbors
	for(Int_t iPad = 0; iPad < fNofPads; iPad++)
	{
		if((fA1[iPad] > 0) && (fS[iPad] == 1))
		{
			if(algVersion == 1)
			{
				SLRec1(moduleGeo, iPad);
			}
			if(algVersion == 2)
			{
				SLRec2(moduleGeo, iPad);
			}
			if((algVersion != 1)&&(algVersion != 2))
			{
				std::cout<<"Error! Unsupported version of the algorithm.\n";
			}
		}
	}
	//Povtorna9 aktivizatsi9 94eek
	for(Int_t iPad = 0; iPad < fNofPads; iPad++)
	{
		if(fA1[iPad] > 0)
		{
			fS[iPad] = 1;
		}
	}

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
		if((fA1[iPad] != 0) && (fS[iPad] == 1))
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

Int_t CbmClusteringSL::GetCluster(Int_t iCluster)
{
	return fClusters[iCluster].nofCluster;
}
Float_t CbmClusteringSL::GetX0(Int_t iCluster)
{
	return fClusters[iCluster].xc;
}
Float_t CbmClusteringSL::GetY0(Int_t iCluster)
{
	return fClusters[iCluster].yc;
}
UInt_t CbmClusteringSL::GetClCharge(Int_t iCluster)
{
	return fClusters[iCluster].sumClCharge;
}
Int_t CbmClusteringSL::GetNofPads(Int_t iCluster)
{
	return fClusters[iCluster].nofPads;
}
Int_t CbmClusteringSL::GetPadInCluster(Int_t iCluster, Int_t iPad)
{
	return fClusters[iCluster].padsInCluster[iPad];
}