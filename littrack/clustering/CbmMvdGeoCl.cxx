/*
 * CbmMvdGeoCl.cxx
 *
 *  Created on: Jun 1, 2012
 *      Author: kozlov
 */

#include "CbmMvdGeoCl.h"

#include "CbmMCTrack.h"
#include "CbmMvdGeoPar.h"
#include "CbmMvdHit.h"
#include "CbmMvdCluster.h"
#include "CbmMvdHitMatch.h"
#include "CbmMvdPileupManager.h"
#include "CbmMvdPoint.h"
#include "CbmMvdDigitize.h"
#include "CbmMvdFindHits.h"
#include "CbmMvdStation.h"

#include "TGeoManager.h"
#include "TGeoTube.h"
#include "TArrayD.h"
#include "TObjArray.h"
#include "TRefArray.h"
#include "TCanvas.h"

#include "FairRootManager.h"

#include "TClonesArray.h"

#include <iostream>
#include <cassert>
#include <iomanip>
#include <vector>
#include <map>

using std::cout;
using std::endl;

CbmMvdGeoCl::CbmMvdGeoCl():
		fMvdGeo()
{
	fNofPads = 0;
	fActivePads = 0;
}

/*CbmMuchGeoCl::CbmMuchGeoCl(Int_t nStation, Int_t nLayer, Bool_t nSide, Int_t nModule, CbmMuchGeoScheme* scheme)
{
	SetLayerGeom(nStation, nLayer, nSide, nModule, scheme);
}*/

CbmMvdGeoCl::~CbmMvdGeoCl()
{
	//delete [] fMvdGeo;
}

Int_t CbmMvdGeoCl::GetMvdGeometry() {

  //cout << "-I- " << GetName() << " : Reading MVD geometry..." << endl;
  Int_t iStation =  1;
  Int_t volId    = -1;

  do {

    // Volume name according to convention
    TString volName  = Form("mvdstation%02i", iStation);
    volId = gGeoManager->GetUID(volName);
    if ( volId > -1 ) {

      // Get shape parameters
      TGeoVolume* volume = gGeoManager->GetVolume(volName.Data());
      TGeoTube* tube = (TGeoTube*) volume->GetShape();
      Double_t rmin = tube->GetRmin();
      Double_t rmax = tube->GetRmax();
      Double_t d    = 2. * tube->GetDz();

      // Full path to node
      TString nodeName = "/cave_1/pipevac1_0/" + volName + "_0";

      // Get z position of node
      Bool_t nodeFound = gGeoManager->cd(nodeName.Data());
      if ( ! nodeFound ) {
	/*cout << "-E- " << GetName() << "::SetMvdGeometry: Node " << nodeName
	     << " not found in geometry!" << endl;*/
	Fatal("SetMvdGeometry", "Node not found");
      }
      Double_t local[3] = {0., 0., 0.};  // Local centre of volume
      Double_t global[3];                // Global centre of volume
      gGeoManager->LocalToMaster(local, global);
      Double_t z = global[2];


      // Check for already existing station with the same ID
      // (Just in case, one never knows...)
      if ( fStationMap.find(iStation) != fStationMap.end() ) {
	/*cout << "-E- " << GetName() << "::GetMvdGeometry: "
	     << "Station " << volId << " already in map!" << endl;*/
	Fatal("GetMvdGeometry", "Double volume ID in TGeoManager!");
      }

      // Create new CbmMvdStation and add it to the map
      fStationMap[iStation] = new CbmMvdStation(volName.Data(), iStation, volId,
			 		        z, d, rmin, rmax);
      fStationMap[iStation]->Print();

      iStation++;

    }     // Volume found

  } while ( volId > -1 );


  return iStation - 1;
}

Float_t CbmMvdGeoCl::GetX0(Int_t iPad)
{
	return fMvdGeo[iPad].xc;
}

Float_t CbmMvdGeoCl::GetX1(Int_t iPad)
{
	return fMvdGeo[iPad].x1;
}

Float_t CbmMvdGeoCl::GetX2(Int_t iPad)
{
	return fMvdGeo[iPad].x2;
}

Float_t CbmMvdGeoCl::GetY0(Int_t iPad)
{
	return fMvdGeo[iPad].yc;
}

Float_t CbmMvdGeoCl::GetY1(Int_t iPad)
{
	return fMvdGeo[iPad].y1;
}

Float_t CbmMvdGeoCl::GetY2(Int_t iPad)
{
	return fMvdGeo[iPad].y2;
}

Int_t CbmMvdGeoCl::GetPadNum(Int_t iPad)
{
	return fMvdGeo[iPad].N;
}

Int_t CbmMvdGeoCl::GetNeighborsNum(Int_t iPad)
{
	return fMvdGeo[iPad].nofNeighbors;
}

Int_t CbmMvdGeoCl::GetGoodNeighborsNum(Int_t iPad)
{
	return fMvdGeo[iPad].nofGoodNeighbors;
}

Int_t CbmMvdGeoCl::GetNeighbor(Int_t iPad, Int_t iNeighbor)
{
	return fMvdGeo[iPad].allNeighbors[iNeighbor];
}

Int_t CbmMvdGeoCl::GetGoodNeighbor(Int_t iPad, Int_t iNeighbor)
{
	return fMvdGeo[iPad].goodNeighbors[iNeighbor];
}

Long64_t CbmMvdGeoCl::GetPadID(Int_t iPad)
{
	return fMvdGeo[iPad].channelID;
}

UInt_t CbmMvdGeoCl::GetPadCharge(Int_t iPad)
{
	return fMvdGeo[iPad].charge;
}

void CbmMvdGeoCl::SetPadCharge(Int_t iPad, UInt_t iCharge)
{
	fMvdGeo[iPad].charge = iCharge;
}

void CbmMvdGeoCl::SetAPadsNom(Int_t nPads)
{
	fActivePads = nPads;
}

template <typename T1>
T1 CbmMvdGeoCl::GetMin(T1& a, T1& b)
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
T2 CbmMvdGeoCl::GetMax(T2& a, T2& b)
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

Bool_t CbmMvdGeoCl::SubEqual(Float_t x1, Float_t x2, Float_t l)
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
