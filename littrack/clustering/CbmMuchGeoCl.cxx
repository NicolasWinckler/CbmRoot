/*
 * CbmMuchGeoCl.cxx
 *
 *  Created on: Feb 22, 2012
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
#include "CbmMuchStation.h"
#include "CbmMuchLayer.h"
#include "CbmMuchLayerSide.h"

#include "FairRootManager.h"

#include "TClonesArray.h"

#include <iostream>
#include <cassert>

using std::cout;
using std::endl;

CbmMuchGeoCl::CbmMuchGeoCl()
{
	fNofPads = 0;
	fStation = 0;
	fLayer = 0;
	fSide = 0;
	fModule = 0;
	fActivePads = 0;
}

CbmMuchGeoCl::CbmMuchGeoCl(Int_t nStation, Int_t nLayer, Bool_t nSide, Int_t nModule, CbmMuchGeoScheme* scheme)
{
	SetLayerGeom(nStation, nLayer, nSide, nModule, scheme);
}

CbmMuchGeoCl::~CbmMuchGeoCl()
{
	delete [] fSingleLayerGeo;
}


void CbmMuchGeoCl::SetLayerGeom(Int_t nStation, Int_t nLayer, Bool_t nSide, Int_t nModule, CbmMuchGeoScheme* scheme)
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
	fSingleLayerGeo = new LayerGeom[fNofPads];
	Int_t nofSectors = module->GetNSectors();
	Int_t padIterator = 0;
	//Цикл фор1 по секторам
	for(Int_t iSector = 0; iSector < nofSectors; iSector++)
	{
	//-получить число пэдов в секторе, высоту, ширину пэда
		CbmMuchSector* sector = (CbmMuchSector*) module->GetSector(iSector); //???
		Int_t nofPadsInSector = sector->GetNChannels();//(sector->GetNCols())*(sector->GetNRows());
		Double_t padsDx_2 = (sector->GetDx())/2;
		Double_t padsDy_2 = (sector->GetDy())/2;
		//std::cout<<"DX: "<<padsDx_2<<"\n";
	//-Цикл фор2 по пэдам в секторе
		for(Int_t iPad = 0; iPad < nofPadsInSector; iPad++)
	//--получить номер и координаты пэда
		{
			CbmMuchPad* pad = (CbmMuchPad*) sector->GetPad(iPad);
			fSingleLayerGeo[padIterator].Ndigi = 0;
			fSingleLayerGeo[padIterator].charge = 0;
			fSingleLayerGeo[padIterator].xc = pad->GetX0();
			fSingleLayerGeo[padIterator].yc = pad->GetY0();
			fSingleLayerGeo[padIterator].x1 = fSingleLayerGeo[padIterator].xc - padsDx_2;
			fSingleLayerGeo[padIterator].x2 = fSingleLayerGeo[padIterator].xc + padsDx_2;
			fSingleLayerGeo[padIterator].y1 = fSingleLayerGeo[padIterator].yc - padsDy_2;
			fSingleLayerGeo[padIterator].y2 = fSingleLayerGeo[padIterator].yc + padsDy_2;
			fSingleLayerGeo[padIterator].channelID = pad->GetChannelId();
			/*std::cout<<"Pad: "<<padIterator<<"; ChannelID: "<<pad->GetChannelId()<<"\n";
			std::cout<<"Xc = "<<fSingleLayerGeo[padIterator].xc;
			std::cout<<"Yc = "<<fSingleLayerGeo[padIterator].yc<<"\n";*/
			padIterator++;
		}
	//-Конец фор2
	}
	//Конец фор1
	//Составить список соседей. М.б. можно взять из старой версии.
	padIterator = 0;
	for(Int_t iPadMain = 0; iPadMain < fNofPads; iPadMain++)
	{
		padIterator = 0;
		Int_t padIterator2 = 0;
		fSingleLayerGeo[iPadMain].nofNeighbors = 0;
		fSingleLayerGeo[iPadMain].nofGoodNeighbors = 0;
		for(Int_t iPadNeighbor = 0; iPadNeighbor < fNofPads; iPadNeighbor++)
		{
			Float_t dX = fabs(fSingleLayerGeo[iPadMain].x1 - fSingleLayerGeo[iPadMain].x2) * 0.1;
			Float_t dY = fabs(fSingleLayerGeo[iPadMain].y1 - fSingleLayerGeo[iPadMain].y2) * 0.1;
			if((((SubEqual(fSingleLayerGeo[iPadMain].x1, fSingleLayerGeo[iPadNeighbor].x2, dX) ||
			SubEqual(fSingleLayerGeo[iPadMain].x2, fSingleLayerGeo[iPadNeighbor].x1, dX)) &&
			(((GetMax(fSingleLayerGeo[iPadNeighbor].y1, fSingleLayerGeo[iPadNeighbor].y2) + dY) >=
			(GetMin(fSingleLayerGeo[iPadMain].y1, fSingleLayerGeo[iPadMain].y2) - dY)) &&
			((GetMin(fSingleLayerGeo[iPadNeighbor].y1, fSingleLayerGeo[iPadNeighbor].y2) - dY) <=
			(GetMax(fSingleLayerGeo[iPadMain].y1, fSingleLayerGeo[iPadMain].y2) + dY)))) ||
			((SubEqual(fSingleLayerGeo[iPadMain].y1, fSingleLayerGeo[iPadNeighbor].y2, dY) ||
			SubEqual(fSingleLayerGeo[iPadMain].y2, fSingleLayerGeo[iPadNeighbor].y1, dY)) &&
			(((GetMax(fSingleLayerGeo[iPadNeighbor].x1, fSingleLayerGeo[iPadNeighbor].x2) + dX) >=
			(GetMin(fSingleLayerGeo[iPadMain].x1, fSingleLayerGeo[iPadMain].x2) - dX)) &&
			((GetMin(fSingleLayerGeo[iPadNeighbor].x1, fSingleLayerGeo[iPadNeighbor].x2) - dX) <=
			(GetMax(fSingleLayerGeo[iPadMain].x1, fSingleLayerGeo[iPadMain].x2) + dX))))) &&
			(iPadMain != iPadNeighbor))
			{
				fSingleLayerGeo[iPadMain].allNeighbors[padIterator] = iPadNeighbor;
				fSingleLayerGeo[iPadMain].nofNeighbors++;
				padIterator++;
				//std::cout<<"Neighbours: P1 = "<<iPadMain<<"; P2 = "<<iPadNeighbor<<"\n";
			}
			if((((SubEqual(fSingleLayerGeo[iPadMain].x1, fSingleLayerGeo[iPadNeighbor].x2, dX) ||
			SubEqual(fSingleLayerGeo[iPadMain].x2, fSingleLayerGeo[iPadNeighbor].x1, dX)) &&
			(((GetMax(fSingleLayerGeo[iPadNeighbor].y1, fSingleLayerGeo[iPadNeighbor].y2) - dY) >
			(GetMin(fSingleLayerGeo[iPadMain].y1, fSingleLayerGeo[iPadMain].y2) + dY)) &&
			((GetMin(fSingleLayerGeo[iPadNeighbor].y1, fSingleLayerGeo[iPadNeighbor].y2) + dY) <
			(GetMax(fSingleLayerGeo[iPadMain].y1, fSingleLayerGeo[iPadMain].y2) - dY)))) ||
			((SubEqual(fSingleLayerGeo[iPadMain].y1, fSingleLayerGeo[iPadNeighbor].y2, dY) ||
			SubEqual(fSingleLayerGeo[iPadMain].y2, fSingleLayerGeo[iPadNeighbor].y1, dY)) &&
			(((GetMax(fSingleLayerGeo[iPadNeighbor].x1, fSingleLayerGeo[iPadNeighbor].x2) - dX) >
			(GetMin(fSingleLayerGeo[iPadMain].x1, fSingleLayerGeo[iPadMain].x2) + dX)) &&
			((GetMin(fSingleLayerGeo[iPadNeighbor].x1, fSingleLayerGeo[iPadNeighbor].x2) + dX) <
			(GetMax(fSingleLayerGeo[iPadMain].x1, fSingleLayerGeo[iPadMain].x2) - dX))))) &&
			(iPadMain != iPadNeighbor))
			{
				fSingleLayerGeo[iPadMain].goodNeighbors[padIterator2] = iPadNeighbor;
				fSingleLayerGeo[iPadMain].nofGoodNeighbors++;
				padIterator2++;
			}
		}
	}
	fStation = nStation;
	fLayer = nLayer;
	fSide = nSide;
	fModule = nModule;
	//std::cout<<" Geometry created:\nStation "<<fStation<<"; Layer "<<fLayer<<"; Side "<<fSide<<"; Module "<<fModule;
	//std::cout<<"\nPads in module: "<<fNofPads<<"\n";
}

Float_t CbmMuchGeoCl::GetX0(Int_t iPad)
{
	return fSingleLayerGeo[iPad].xc;
}

Float_t CbmMuchGeoCl::GetX1(Int_t iPad)
{
	return fSingleLayerGeo[iPad].x1;
}

Float_t CbmMuchGeoCl::GetX2(Int_t iPad)
{
	return fSingleLayerGeo[iPad].x2;
}

Float_t CbmMuchGeoCl::GetY0(Int_t iPad)
{
	return fSingleLayerGeo[iPad].yc;
}

Float_t CbmMuchGeoCl::GetY1(Int_t iPad)
{
	return fSingleLayerGeo[iPad].y1;
}

Float_t CbmMuchGeoCl::GetY2(Int_t iPad)
{
	return fSingleLayerGeo[iPad].y2;
}

Int_t CbmMuchGeoCl::GetDigiNum(Int_t iPad)
{
	return fSingleLayerGeo[iPad].Ndigi;
}

void CbmMuchGeoCl::SetDigiNum(Int_t iPad, Int_t iDigi)
{
	fSingleLayerGeo[iPad].Ndigi = iDigi;
}

Int_t CbmMuchGeoCl::GetNeighborsNum(Int_t iPad)
{
	return fSingleLayerGeo[iPad].nofNeighbors;
}

Int_t CbmMuchGeoCl::GetGoodNeighborsNum(Int_t iPad)
{
	return fSingleLayerGeo[iPad].nofGoodNeighbors;
}

Int_t CbmMuchGeoCl::GetNeighbor(Int_t iPad, Int_t iNeighbor)
{
	return fSingleLayerGeo[iPad].allNeighbors[iNeighbor];
}

Int_t CbmMuchGeoCl::GetGoodNeighbor(Int_t iPad, Int_t iNeighbor)
{
	return fSingleLayerGeo[iPad].goodNeighbors[iNeighbor];
}

Long64_t CbmMuchGeoCl::GetPadID(Int_t iPad)
{
	return fSingleLayerGeo[iPad].channelID;
}

UInt_t CbmMuchGeoCl::GetPadCharge(Int_t iPad)
{
	return fSingleLayerGeo[iPad].charge;
}

void CbmMuchGeoCl::SetPadCharge(Int_t iPad, UInt_t iCharge)
{
	fSingleLayerGeo[iPad].charge = iCharge;
}

void CbmMuchGeoCl::SetAPadsNom(Int_t nPads)
{
	fActivePads = nPads;
}

template <typename T1>
T1 CbmMuchGeoCl::GetMin(T1& a, T1& b)
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
T2 CbmMuchGeoCl::GetMax(T2& a, T2& b)
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

Bool_t CbmMuchGeoCl::SubEqual(Float_t x1, Float_t x2, Float_t l)
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
