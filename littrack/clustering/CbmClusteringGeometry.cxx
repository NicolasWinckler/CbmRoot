/*
 * CbmClusteringGeometry.cxx
 *
 *  Created on: Feb 22, 2012
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
#include "CbmMuchStation.h"
#include "CbmMuchLayer.h"
#include "CbmMuchLayerSide.h"
#include "CbmMuchSectorRectangular.h"
#include "CbmMuchPadRectangular.h"

#include "FairRootManager.h"

#include "TClonesArray.h"

#include <iostream>
#include <cassert>

using std::cout;
using std::endl;

CbmClusteringGeometry::CbmClusteringGeometry()
{
	fNofPads = 0;
	//fStation = 0;
	//fLayer = 0;
	//fSide = 0;
	//fModule = 0;
	fNofActivePads = 0;
}

CbmClusteringGeometry::CbmClusteringGeometry(Int_t nStation, Int_t nLayer, Bool_t nSide, Int_t nModule, CbmMuchGeoScheme* scheme)
{
	SetMuchModuleGeometry(nStation, nLayer, nSide, nModule, scheme);
}

CbmClusteringGeometry::~CbmClusteringGeometry()
{
	delete [] fPadList;
}


void CbmClusteringGeometry::SetMuchModuleGeometry(Int_t nStation, Int_t nLayer, Bool_t nSide, Int_t nModule, CbmMuchGeoScheme* scheme)
{
	//std::cout<<"\n\nCreating module geometry for clustering...";
	//--CbmMuchGeoScheme* scheme = CbmMuchGeoScheme::Instance();
	//sch init - name
	//TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");
	//--TString muchDigiFile = "/home/kozlov/cbm/cbmroot_new/cbmroot/parameters/much/much_v11a.digi.root";
	//--scheme->Init(muchDigiFile);
	CbmMuchModuleGem* module = (CbmMuchModuleGem*) scheme->GetModule(nStation, nLayer, nSide, nModule);
	fNofPads = module->GetNPads();
	fDetId = module->GetDetectorId();
	fPadList = new PadInformation[fNofPads];
	Int_t nofSectors = module->GetNSectors();
	Int_t padIterator = 0;
	for(Int_t iSector = 0; iSector < nofSectors; iSector++)
	{
		CbmMuchSectorRectangular* sector = (CbmMuchSectorRectangular*) module->GetSector(iSector); //???
		Int_t nofPadsInSector = sector->GetNChannels();//(sector->GetNCols())*(sector->GetNRows());
		//Double_t padsDx_2 = (sector->GetPadDx())/2;
		//Double_t padsDy_2 = (sector->GetPadDy())/2;
		//std::cout<<"DX: "<<padsDx_2<<"\n";
		Int_t padNx = sector->GetPadNx();
		Int_t padNy = sector->GetPadNy();
		//for(Int_t iPad = 0; iPad < nofPadsInSector; iPad++)
		/*for(Int_t i = 0; i < padNx; i++)
		{
			for(Int_t j = 0; j < padNy; j++)
			{*/
		for(Int_t iPad = 0; iPad < nofPadsInSector; iPad++)
		{
			CbmMuchPadRectangular* pad = (CbmMuchPadRectangular*) sector->GetPadByChannelIndex(iPad);	//!!!
			fPadList[padIterator].fDigiNum = 0;
			fPadList[padIterator].fCharge = 0;
			fPadList[padIterator].fX = pad->GetX();
			fPadList[padIterator].fY = pad->GetY();
			fPadList[padIterator].fDx = sector->GetPadDx();
			fPadList[padIterator].fDy = sector->GetPadDy();
			fPadList[padIterator].fNeighbors.clear();
			//fPadList[padIterator].x1 = fPadList[padIterator].xc - padsDx_2;
			//fPadList[padIterator].x2 = fPadList[padIterator].xc + padsDx_2;
			//fPadList[padIterator].y1 = fPadList[padIterator].yc - padsDy_2;
			//fPadList[padIterator].y2 = fPadList[padIterator].yc + padsDy_2;
			fPadList[padIterator].channelID = pad->GetChannelId();
			fPadByChannelId[pad->GetChannelId()] = padIterator;
			/*std::cout<<"Pad: "<<padIterator<<"; ChannelID: "<<pad->GetChannelId()<<"\n";
			std::cout<<"Xc = "<<fSingleLayerGeo[padIterator].xc;
			std::cout<<"Yc = "<<fSingleLayerGeo[padIterator].yc<<"\n";*/
			padIterator++;
		}
			/*}
		}*/
	}
	padIterator = 0;
	for(Int_t iPadMain = 0; iPadMain < fNofPads; iPadMain++)
	{
		//padIterator = 0;
		//Int_t padIterator2 = 0;
		fPadList[iPadMain].fNofNeighbors = 0;
		fPadList[iPadMain].fNofGoodNeighbors = 0;
		Float_t xLeft_1 = fPadList[iPadMain].fX - (fPadList[iPadMain].fDx / 2);
		Float_t xRight_1 = fPadList[iPadMain].fX + (fPadList[iPadMain].fDx / 2);
		Float_t yDown_1 = fPadList[iPadMain].fY - (fPadList[iPadMain].fDy / 2);
		Float_t yUp_1 = fPadList[iPadMain].fY + (fPadList[iPadMain].fDy / 2);
		for(Int_t iPadNeighbor = 0; iPadNeighbor < fNofPads; iPadNeighbor++)
		{
			Float_t xLeft_2 = fPadList[iPadNeighbor].fX - (fPadList[iPadNeighbor].fDx / 2);
			Float_t xRight_2 = fPadList[iPadNeighbor].fX + (fPadList[iPadNeighbor].fDx / 2);
			Float_t yDown_2 = fPadList[iPadNeighbor].fY - (fPadList[iPadNeighbor].fDy / 2);
			Float_t yUp_2 = fPadList[iPadNeighbor].fY + (fPadList[iPadNeighbor].fDy / 2);
			Float_t dX = fabs(xLeft_1 - xRight_1) * 0.1;
			Float_t dY = fabs(yDown_1 - yUp_1) * 0.1;
			/*if((((SubEqual(xLeft_1, xRight_2, dX) ||
			SubEqual(xRight_1, xLeft_2, dX)) &&
			(((GetMax(yDown_2, yUp_2) + dY) >=
			(GetMin(yDown_1, yUp_1) - dY)) &&
			((GetMin(yDown_2, yUp_2) - dY) <=
			(GetMax(yDown_1, yUp_1) + dY)))) ||
			((SubEqual(yDown_1, yUp_2, dY) ||
			SubEqual(yUp_1, yDown_2, dY)) &&
			(((GetMax(xLeft_2, xRight_2) + dX) >=
			(GetMin(xLeft_1, xRight_1) - dX)) &&
			((GetMin(xLeft_2, xRight_2) - dX) <=
			(GetMax(xLeft_1, xRight_1) + dX))))) &&
			(iPadMain != iPadNeighbor))
			{
				fSingleLayerGeo[iPadMain].allNeighbors[padIterator] = iPadNeighbor;
				fSingleLayerGeo[iPadMain].nofNeighbors++;
				padIterator++;
				//std::cout<<"Neighbours: P1 = "<<iPadMain<<"; P2 = "<<iPadNeighbor<<"\n";
			}*/
			if((((SubEqual(xLeft_1, xRight_2, dX) ||
			SubEqual(xRight_1, xLeft_2, dX)) &&
			(((GetMax(yDown_2, yUp_2) - dY) >
			(GetMin(yDown_1, yUp_1) + dY)) &&
			((GetMin(yDown_2, yUp_2) + dY) <
			(GetMax(yDown_1, yUp_1) - dY)))) ||
			((SubEqual(yDown_1, yUp_2, dY) ||
			SubEqual(yUp_1, yDown_2, dY)) &&
			(((GetMax(xLeft_2, xRight_2) - dX) >
			(GetMin(xLeft_1, xRight_1) + dX)) &&
			((GetMin(xLeft_2, xRight_2) + dX) <
			(GetMax(xLeft_1, xRight_1) - dX))))) &&
			(iPadMain != iPadNeighbor))
			{
				fPadList[iPadMain].fNeighbors.push_back(iPadNeighbor);
				//fSingleLayerGeo[iPadMain].goodNeighbors[padIterator2] = iPadNeighbor;
				fPadList[iPadMain].fNofGoodNeighbors++;
				//padIterator2++;
			}
		}
	}
	//fStation = nStation;
	//fLayer = nLayer;
	//fSide = nSide;
	//fModule = nModule;
	//std::cout<<" Geometry created:\nStation "<<fStation<<"; Layer "<<fLayer<<"; Side "<<fSide<<"; Module "<<fModule;
	//std::cout<<"\nPads in module: "<<fNofPads<<"\n";
}

Float_t CbmClusteringGeometry::GetX0(Int_t iPad)
{
	return fPadList[iPad].fX;
}

Float_t CbmClusteringGeometry::GetDx(Int_t iPad)
{
	return fPadList[iPad].fDx;
}

Float_t CbmClusteringGeometry::GetDy(Int_t iPad)
{
	return fPadList[iPad].fDy;
}

Float_t CbmClusteringGeometry::GetY0(Int_t iPad)
{
	return fPadList[iPad].fY;
}

/*Float_t CbmMuchGeoCl::GetY1(Int_t iPad)
{
	return fSingleLayerGeo[iPad].y1;
}

Float_t CbmMuchGeoCl::GetY2(Int_t iPad)
{
	return fSingleLayerGeo[iPad].y2;
}*/

Int_t CbmClusteringGeometry::GetDigiNum(Int_t iPad)
{
	return fPadList[iPad].fDigiNum;
}

void CbmClusteringGeometry::SetDigiNum(Int_t iPad, Int_t iDigi)
{
	fPadList[iPad].fDigiNum = iDigi;
}

Int_t CbmClusteringGeometry::GetNeighborsNum(Int_t iPad)
{
	return fPadList[iPad].fNofNeighbors;
}

Int_t CbmClusteringGeometry::GetGoodNeighborsNum(Int_t iPad)
{
	return fPadList[iPad].fNofGoodNeighbors;
}

Int_t CbmClusteringGeometry::GetNeighbor(Int_t iPad, Int_t iNeighbor)
{
	//return fSingleLayerGeo[iPad].allNeighbors[iNeighbor];
	return fPadList[iPad].fNeighbors[iNeighbor];
}

/*Int_t CbmMuchGeoCl::GetGoodNeighbor(Int_t iPad, Int_t iNeighbor)
{
	return fSingleLayerGeo[iPad].goodNeighbors[iNeighbor];
}*/

Long64_t CbmClusteringGeometry::GetPadID(Int_t iPad)
{
	return fPadList[iPad].channelID;
}

UInt_t CbmClusteringGeometry::GetPadCharge(Int_t iPad)
{
	return fPadList[iPad].fCharge;
}

void CbmClusteringGeometry::SetPadCharge(Int_t iPad, UInt_t iCharge)
{
	fPadList[iPad].fCharge = iCharge;
}

void CbmClusteringGeometry::SetAPadsNom(Int_t nPads)
{
	fNofActivePads = nPads;
}

void CbmClusteringGeometry::SetAPadsPlusOne()
{
	fNofActivePads++;
}

template <typename T1>
T1 CbmClusteringGeometry::GetMin(T1& a, T1& b)
{
	if(a > b)
	{
		return b;
	}
	else
	{
		return a;
	}
}

template <typename T2>
T2 CbmClusteringGeometry::GetMax(T2& a, T2& b)
{
	if(a < b)
	{
		return b;
	}
	else
	{
		return a;
	}
}

Bool_t CbmClusteringGeometry::SubEqual(Float_t x1, Float_t x2, Float_t l)
{
	//l = l * 0.1;
	if((x1 < (x2 + l)) && (x1 > (x2 - l)))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

Int_t CbmClusteringGeometry::GetPadByChannelId(Long64_t chId)
{
	return fPadByChannelId[chId];
}
