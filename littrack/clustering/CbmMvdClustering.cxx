/*
 * CbmMvdClustering.cxx
 *
 *  Created on: Jun 7, 2012
 *      Author: kozlov
 */

// Includes from MVD
#include "CbmMvdGeoPar.h"
#include "CbmMvdHit.h"
#include "CbmMvdCluster.h"
#include "CbmMvdHitMatch.h"
#include "CbmMvdPileupManager.h"
#include "CbmMvdPoint.h"
#include "CbmMvdDigitize.h"
#include "CbmMvdFindHits.h"
#include "CbmMvdStation.h"

// Includes from base
#include "FairGeoNode.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "CbmMCTrack.h"

// Includes from ROOT
#include "TGeoManager.h"
#include "TGeoTube.h"
#include "TArrayD.h"
#include "TObjArray.h"
#include "TRefArray.h"
#include "TCanvas.h"

#include "TClonesArray.h"

#include "TRandom3.h"
#include "TString.h"
#include "TVector3.h"
#include "TMath.h"
#include "TH1.h"
#include "TH2.h"
#include "TH1F.h"

#include "CbmMvdClustering.h"

// Includes from C++
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>

using std::cout;
using std::endl;
using std::map;
using std::setw;
using std::left;
using std::right;
using std::fixed;
using std::pair;
using std::setprecision;
using std::ios_base;
using std::vector;

CbmMvdClustering::CbmMvdClustering():
		fhMvdClusters(NULL),
		fhMvdErrors(NULL),
		fhMvdErrorsByPixels(NULL),
		fhErrPoint_Hit(NULL),
		fhErrPoint_Pixel(NULL),
		fhErrHit_Pixel(NULL)
{
	fNofDigisBySt[0] = 0;
	fNofDigisBySt[1] = 0;
	fNofPointsBySt[0] = 0;
	fNofPointsBySt[1] = 0;
	fEfficiency = 0;
	fNofEvents = 0;
	fLooseCl = 0;
	fFakeCl = 0;
}

CbmMvdClustering::~CbmMvdClustering()
{
	if ( fHits ) {
	fHits->Delete();
	delete fHits;
	}
}

InitStatus CbmMvdClustering::Init()
{
   std::cout << "CbmMvdClustering::Init" << std::endl;

   Int_t nStations = GetMvdGeometry();
   if ( ! nStations ) {
   cout << "-W- " << GetName() << "::Init: No MVD stations in geometry!"
       << endl << "   Task will be inactive!" << endl;
	   fActive = kFALSE;
   }

   FairRootManager* ioman = FairRootManager::Instance();
   if ( ! ioman ){
	   cout << "-E- " << GetName() << "::Init: No FairRootManager!" << endl;
	   return kFATAL;
   }

   fDigis = (TClonesArray*) ioman->GetObject("MvdDigi");
   if ( ! fDigis ) {
    	cout << "-E- " << GetName() << "::Init: No FairRootManager!" << endl;
    	return kFATAL;
   }
   fPoints = (TClonesArray*) ioman->GetObject("MvdPoint");

   fHits = new TClonesArray("CbmMvdHit", 1000);
   ioman->Register("MvdHit", "Hit in MVD", fHits, kTRUE);
   //Register();
   //ReadDataBranches();
   fhMvdClusters = new TH1F("fhMvdClusters", "hMvdClusters;Charge;nofClusters", 17, 0, 0);
   fhMvdErrors = new TH1F("fhMvdErrors", "hMvdErrors;Error;nofClusters", 17, 0, 0);

   fhMvdErrorsByPixels = new TH1F("fhMvdErrorsByPixels", "fhMvdErrorsByPixels;Error;nofClusters", 12, 0, 0);

   fhErrPoint_Hit = new TH1F("fhErrPoint_Hit", "hErrPoint_Hit;Radius;Error", nRadiusSteps + 2, 0, 0);
   fhErrPoint_Pixel = new TH1F("fhErrPoint_Pixel", "hErrPoint_Pixel;Radius;Error", nRadiusSteps + 2, 0, 0);
   fhErrHit_Pixel = new TH1F("fhErrHit_Pixel", "hErrHit_Pixel;Radius;Error", nRadiusSteps + 2, 0, 0);

   TString fname = "/u/gkozlov/cbm/events/cc.25gev.centr_draw10.txt";
   FILE* f1 = fopen(fname , "w");
   fprintf(f1, "Output file:\n\n");
   fclose(f1);
   return kSUCCESS;
}

void CbmMvdClustering::Exec(Option_t* opt)
{
	static Int_t eventNo = 0;
	std::cout << "\n---<>---\nCbmMvdClustering::Exec: event No=" << eventNo++ << std::endl;
	fNofEvents++;

	if( fDigis->GetEntriesFast() == 0 ){
		cout << "-I- " << GetName() << "::Exec: No digis found, event skipped. " << endl;
		fNofEvents--;
	        //fCounter++;
		return;
	}
	map<Int_t, CbmMvdStation*>::iterator stationIt;
	for (stationIt=fStationMap.begin(); stationIt!=fStationMap.end();
			stationIt++) (*stationIt).second->Clear();

	CbmMvdDigi* digi = NULL;
	//Int_t nDigis = fDigis->GetEntriesFast();
	fNofDigis = fDigis->GetEntriesFast();

	for (stationIt=fStationMap.begin(); stationIt!=fStationMap.end(); stationIt++) {
		CbmMvdStation* station = (*stationIt).second;
	        Double_t layerRadius = station->GetRmax();
		if( gDebug>0 ){
		    cout << "-I- " << GetName() << ": station->GetVolumeId(): " << station->GetVolumeId() << " , layerRadius " << layerRadius << endl;
	}
	}
	if (fHits) fHits->Clear();
	fDigiMap.clear();
	fClusterMap.clear();
	fChargeMap1.clear();
	fChargeMap1.clear();
	fActiveMap.clear();
	fNofDigisBySt[0] = 0;
	fNofDigisBySt[1] = 0;
	for(Int_t k = 0; k < fNofDigis; k++){
	    digi = (CbmMvdDigi*) fDigis->At(k);
	    // test if station is correct and apply fNeighThreshold
			//if( digi->GetStationNr() != station->GetStationNr() ){ continue; }
	    //if(GetAdcCharge(digi->GetCharge()) < fNeighThreshold ) continue;
	    pair<Int_t, Int_t> a (digi->GetPixelX(),digi->GetPixelY());
	    fDigiMap[a] = k;
	    fClusterMap[a] = k;
	    fChargeMap1[a] = digi->GetCharge();
	    fChargeMap1[a] = 0;
	    fActiveMap[a] = 1;
	    if(digi->GetStationNr() == 1)
	    {
	    	fNofDigisBySt[0]++;
	    }
	    if(digi->GetStationNr() == 2)
	    {
	    	fNofDigisBySt[1]++;
	    }
	}
	//}
	fNofPoints = fPoints->GetEntriesFast();
	fNofPointsBySt[0] = 0;
	fNofPointsBySt[1] = 0;
	fNofClusters = 0;
	if(fClusters)
	{
		delete [] fClusters;
	}
	for(Int_t iPoint = 0; iPoint < fNofPoints; iPoint++)
	{
		CbmMvdPoint* point = (CbmMvdPoint*) fPoints->At(iPoint);
		if(point->GetStationNr() == 1)
		{
			fNofPointsBySt[0]++;
		}
		if(point->GetStationNr() == 2)
		{
			fNofPointsBySt[1]++;
		}
	}

	std::cout<<"Digis: "<<fNofDigis<<"\n";
	std::cout<<"Points: "<<fNofPoints<<"\n";
	FindClusters();

	CalculateAcuracy();

	//FillHistogramsForRadius();

	//TestStrangeClusters();
	PrintClusters();

}

void CbmMvdClustering::Finish()
{
   std::cout << "CbmMvdClustering::Finish" << std::endl;

   fEfficiency = 100 * fEfficiency / (Float_t)fNofEvents;
   std::cout<<"Efficiency: "<<fEfficiency<<"\n";
   fErrHit_Pixel = fErrHit_Pixel / (Float_t)fNofEvents;
   fErrHit_Point = fErrHit_Point / (Float_t)fNofEvents;
   fErrPoint_Pixel = fErrPoint_Pixel / (Float_t)fNofEvents;
   std::cout<<"ErrHit_Pixel: "<<fErrHit_Pixel<<"\nErrHit_Point: "<<fErrHit_Point<<
		   "\nErrPoint_Pixel: "<<fErrPoint_Pixel<<"\n";
   std::cout<<"Loosing Clusters: "<<fLooseCl<<"\nFake Clusters: "<<fFakeCl<<"\n";
   TString fname = "/u/gkozlov/cbm/events/cc.25gev.centr_draw10.txt";
   FILE* f1 = fopen(fname , "a");
   fprintf(f1, "\n\n\nEfficiency: %.3f\nErrHit_Pixel: %.7f\nErrHit_Point: %.7f\nErrPoint_Pixel: %.7f",
		   fEfficiency, fErrHit_Pixel, fErrHit_Point, fErrPoint_Pixel);
   fclose(f1);
   /*fhMvdClusters->Write();
   TCanvas* canvasL = new TCanvas("mvd_clusters", "mvd_clusters", 700, 700);
   canvasL->Divide(1, 1);
   canvasL->cd(1);
   fhMvdClusters->Draw();

   fhMvdErrors->Write();
   TCanvas* canvasE = new TCanvas("mvd_errors", "mvd_errors", 700, 700);
   canvasE->Divide(1, 1);
   canvasE->cd(1);
   fhMvdErrors->Draw();

   fhMvdErrorsByPixels->Write();
   TCanvas* canvasEp = new TCanvas("fhMvdErrorsByPixels", "fhMvdErrorsByPixels", 700, 700);
   canvasEp->Divide(1, 1);
   canvasEp->cd(1);
   fhMvdErrorsByPixels->Draw();

   fhErrPoint_Hit->Write();
   TCanvas* canvasE1 = new TCanvas("fhErrPoint_Hit", "fhErrPoint_Hit", 700, 700);
   canvasE1->Divide(1, 1);
   canvasE1->cd(1);
   fhErrPoint_Hit->Draw();

   fhErrPoint_Pixel->Write();
   TCanvas* canvasE2 = new TCanvas("fhErrPoint_Pixel", "fhErrPoint_Pixel", 700, 700);
   canvasE2->Divide(1, 1);
   canvasE2->cd(1);
   fhErrPoint_Pixel->Draw();

   fhErrHit_Pixel->Write();
   TCanvas* canvasE3 = new TCanvas("fhErrHit_Pixel", "fhErrHit_Pixel", 700, 700);
   canvasE3->Divide(1, 1);
   canvasE3->cd(1);
   fhErrHit_Pixel->Draw();*/
}

Int_t CbmMvdClustering::GetMvdGeometry() {

  cout << "-I- " << GetName() << " : Reading MVD geometry..." << endl;
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
	cout << "-E- " << GetName() << "::SetMvdGeometry: Node " << nodeName
	     << " not found in geometry!" << endl;
	Fatal("SetMvdGeometry", "Node not found");
      }
      Double_t local[3] = {0., 0., 0.};  // Local centre of volume
      Double_t global[3];                // Global centre of volume
      gGeoManager->LocalToMaster(local, global);
      Double_t z = global[2];


      // Check for already existing station with the same ID
      // (Just in case, one never knows...)
      if ( fStationMap.find(iStation) != fStationMap.end() ) {
	cout << "-E- " << GetName() << "::GetMvdGeometry: "
	     << "Station " << volId << " already in map!" << endl;
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

/*void CbmMvdClustering::Register() {
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman) Fatal("Register",
		      "No FairRootManager");
  ioman->Register("MvdHit", "MVD", fHits, kTRUE);
  ioman->Register("MvdCluster", "MVD", fClusters, kTRUE);
  ioman->Register("MvdHitMatch", "MVD Hit Match", fMatches, kTRUE);


}*/

void CbmMvdClustering::ReadDataBranches()
{
   /*FairRootManager* ioman = FairRootManager::Instance();
   assert(ioman != NULL);

   fMuchDigi = (TClonesArray*) ioman->GetObject("MuchDigi");
   fMuchPoint = (TClonesArray*) ioman->GetObject("MuchPoint");*/
}

void CbmMvdClustering::FindClusters()
{
	std::cout<<"FindClusters Start\n";
	Int_t channelX = 0;
	Int_t channelY = 0;
	CbmMvdDigi* digi = NULL;
	CbmMvdDigi* digiNeighbor = NULL;
	//Int_t nofClusters = fNofDigisBySt[0];
	//Int_t nofClusters = fNofDigisBySt[1];	//2
	Int_t nofClusters = 0;
	//Int_t nCl = 0;
	//for(Int_t iDigi = 0; iDigi < fNofDigisBySt[0]; iDigi++)
	for(Int_t iDigi = 0; iDigi < fNofDigis; iDigi++)
	//for(Int_t iDigi = fNofDigisBySt[0]; iDigi < fNofDigisBySt[0] + fNofDigisBySt[1]; iDigi++)	//2
	{
		digi = (CbmMvdDigi*) fDigis->At(iDigi);
		//if(digi->GetStationNr() == 1){
		if(/*digi->GetStationNr() == 2*/1){	//2
			Int_t stNr = digi->GetStationNr();
		channelX = digi->GetPixelX();
		channelY = digi->GetPixelY();
		Int_t channelXmax = channelX;
		Int_t channelYmax = channelY;
		pair<Int_t, Int_t> a(channelX, channelY);
		pair<Int_t, Int_t> maxDigi(channelX, channelY);
		Float_t maxCharge = digi->GetCharge();

		a = std::make_pair(channelX + 1, channelY);
		fDigiMapIt = fDigiMap.find(a);
		if (!(fDigiMapIt == fDigiMap.end()))
		{
			Int_t i = fDigiMap[a];
			digiNeighbor = (CbmMvdDigi*)fDigis->At(i);
			if((maxCharge < digiNeighbor->GetCharge()) && (stNr == digiNeighbor->GetStationNr()))
			{
				maxCharge = digiNeighbor->GetCharge();
				channelXmax = channelX + 1;
				channelYmax = channelY;
			}
		}
		a = std::make_pair(channelX - 1, channelY);
		fDigiMapIt = fDigiMap.find(a);
		if (!(fDigiMapIt == fDigiMap.end()))
		{
			Int_t i = fDigiMap[a];
			digiNeighbor = (CbmMvdDigi*)fDigis->At(i);
			if(maxCharge < digiNeighbor->GetCharge() && (stNr == digiNeighbor->GetStationNr()))
			{
				maxCharge = digiNeighbor->GetCharge();
				channelXmax = channelX - 1;
				channelYmax = channelY;
			}
		}
		a = std::make_pair(channelX, channelY + 1);
		fDigiMapIt = fDigiMap.find(a);
		if (!(fDigiMapIt == fDigiMap.end()))
		{
			Int_t i = fDigiMap[a];
			digiNeighbor = (CbmMvdDigi*)fDigis->At(i);
			if(maxCharge < digiNeighbor->GetCharge() && (stNr == digiNeighbor->GetStationNr()))
			{
				maxCharge = digiNeighbor->GetCharge();
				channelXmax = channelX;
				channelYmax = channelY + 1;
			}
		}
		a = std::make_pair(channelX, channelY - 1);
		fDigiMapIt = fDigiMap.find(a);
		if (!(fDigiMapIt == fDigiMap.end()))
		{
			Int_t i = fDigiMap[a];
			digiNeighbor = (CbmMvdDigi*)fDigis->At(i);
			if(maxCharge < digiNeighbor->GetCharge() && (stNr == digiNeighbor->GetStationNr()))
			{
				maxCharge = digiNeighbor->GetCharge();
				channelXmax = channelX;
				channelYmax = channelY - 1;
			}
		}
		//---
		/*a = std::make_pair(channelX - 1, channelY - 1);
		fDigiMapIt = fDigiMap.find(a);
		if (!(fDigiMapIt == fDigiMap.end()))
		{
			Int_t i = fDigiMap[a];
			digiNeighbor = (CbmMvdDigi*)fDigis->At(i);
			if(maxCharge < digiNeighbor->GetCharge())
			{
				maxCharge = digiNeighbor->GetCharge();
				channelXmax = channelX - 1;
				channelYmax = channelY - 1;
			}
		}
		a = std::make_pair(channelX - 1, channelY + 1);
		fDigiMapIt = fDigiMap.find(a);
		if (!(fDigiMapIt == fDigiMap.end()))
		{
			Int_t i = fDigiMap[a];
			digiNeighbor = (CbmMvdDigi*)fDigis->At(i);
			if(maxCharge < digiNeighbor->GetCharge())
			{
				maxCharge = digiNeighbor->GetCharge();
				channelXmax = channelX - 1;
				channelYmax = channelY + 1;
			}
		}
		a = std::make_pair(channelX + 1, channelY - 1);
		fDigiMapIt = fDigiMap.find(a);
		if (!(fDigiMapIt == fDigiMap.end()))
		{
			Int_t i = fDigiMap[a];
			digiNeighbor = (CbmMvdDigi*)fDigis->At(i);
			if(maxCharge < digiNeighbor->GetCharge())
			{
				maxCharge = digiNeighbor->GetCharge();
				channelXmax = channelX + 1;
				channelYmax = channelY - 1;
			}
		}
		a = std::make_pair(channelX + 1, channelY + 1);
		fDigiMapIt = fDigiMap.find(a);
		if (!(fDigiMapIt == fDigiMap.end()))
		{
			Int_t i = fDigiMap[a];
			digiNeighbor = (CbmMvdDigi*)fDigis->At(i);
			if(maxCharge < digiNeighbor->GetCharge())
			{
				maxCharge = digiNeighbor->GetCharge();
				channelXmax = channelX + 1;
				channelYmax = channelY + 1;
			}
		}*/
		//---
		//std::cout<<"->Step "<<iDigi<<"; Station: "<<digi->GetStationNr()<<"; Charge: "<<digi->GetCharge()<<"\n";
		if((channelX == channelXmax) && (channelY == channelYmax))
		{
			a = std::make_pair(channelX, channelY);
			fChargeMap2[a] += fChargeMap1[a];
			//fActiveMap[a] = 1;
			fChargeMap1[a] = 1;
			nofClusters++;
		}
		else
		{
			a = std::make_pair(channelX, channelY);
			maxDigi = std::make_pair(channelXmax, channelYmax);
			//nofClusters--;
			fChargeMap2[maxDigi] += fChargeMap1[a];
			Int_t nCluster = fClusterMap[a];
			//fClusterMap[a] = fClusterMap[maxDigi];	//M.b. it should be deleted (?)
			//for(Int_t jDigi = 0; jDigi < fNofDigisBySt[0]; jDigi++)
			//---Without recursion. x3 slower---
			/*for(Int_t jDigi = 0; jDigi < fNofDigis; jDigi++)
			//for(Int_t jDigi = fNofDigisBySt[0]; jDigi < fNofDigisBySt[0] + fNofDigisBySt[1]; jDigi++)	//2
			{
				CbmMvdDigi* digiReplase = (CbmMvdDigi*)fDigis->At(jDigi);
				Int_t chX = digiReplase->GetPixelX();
				Int_t chY = digiReplase->GetPixelY();
				a = std::make_pair(chX, chY);
				if(fClusterMap[a] == nCluster)
				{
					fClusterMap[a] = fClusterMap[maxDigi];
				}
			}*/
			//---Recursion---
			ChangeClusters(a, nCluster, fClusterMap[maxDigi]);
		}}//---
		/*pair<Int_t, Int_t> z(digi->GetPixelX(), digi->GetPixelY());
		std::cout<<"->Step "<<iDigi<<"; Station: "<<digi->GetStationNr()<<"; Charge: "<<
				digi->GetCharge()<<"; Cl: "<<fClusterMap[z]<<"; cm: "<<fChargeMap1[z]<<"\n";*/
	}
	std::cout<<"FindClusters Finish Clustering Step\n";
	std::cout<<"Points: "<<fNofPoints<<"; P1: "<<fNofPointsBySt[0]<<"; P2:"<<fNofPointsBySt[1]<<"\n";
	std::cout<<"Digis: "<<fNofDigis<<"; D1: "<<fNofDigisBySt[0]<<"; D2:"<<fNofDigisBySt[1]<<"\n";
	std::cout<<"Clusters: "<<nofClusters<<"\n";
	//std::cout<<"nCl: "<<nCl<<"\n";
				//nofClusters = nCl;	//10000 - OK ???
	Int_t a100 = 0, a500 = 0, a1000 = 0, a0 = 0;

	fClusters = new Cluster[nofClusters];
	fNofClusters = nofClusters;
	for(Int_t iCl = 0; iCl < nofClusters; iCl++)
	{
		fClusters[iCl].nofCluster = 0;
		fClusters[iCl].nofDidis = 0;
		fClusters[iCl].sumClCharge = 0;
		fClusters[iCl].xc = 0;
		fClusters[iCl].yc = 0;
	}
	Int_t nomCl = 0;
	Int_t Replase = 0;
	//for(Int_t iDigi = 0; iDigi < fNofDigisBySt[0]; iDigi++)
	for(Int_t iDigi = 0; iDigi < fNofDigis; iDigi++)
	//for(Int_t iDigi = fNofDigisBySt[0]; iDigi < fNofDigisBySt[0] + fNofDigisBySt[1]; iDigi++)	//2
	{
		digi = (CbmMvdDigi*) fDigis->At(iDigi);
		Int_t stNr = digi->GetStationNr();
		channelX = digi->GetPixelX();
		channelY = digi->GetPixelY();
		//std::cout<<"\n->Digi: "<<iDigi;
		pair<Int_t, Int_t> a(channelX, channelY);
		if((fChargeMap1[a] == 1) && (fClusterMap[a] != 0))
		{
			//std::cout<<" - good"<<"; nomCl = "<<nomCl;
			Replase = fClusterMap[a];
			Int_t nofDigisInCluster = 0;
			//for(Int_t jDigi = 0; jDigi < fNofDigisBySt[0]; jDigi++)
			for(Int_t jDigi = 0; jDigi < fNofDigis; jDigi++)
			//for(Int_t jDigi = fNofDigisBySt[0]; jDigi < fNofDigisBySt[0] + fNofDigisBySt[1]; jDigi++)	//2
			{
				digiNeighbor = (CbmMvdDigi*) fDigis->At(jDigi);
				if(/*digiNeighbor->GetStationNr() == 2*/stNr == digiNeighbor->GetStationNr()){
				Int_t chX = digiNeighbor->GetPixelX();
				Int_t chY = digiNeighbor->GetPixelY();
				pair<Int_t, Int_t> b(chX, chY);
				if((fClusterMap[b] == Replase) && (fActiveMap[b] == 1))
				{
					fClusterMap[b] = nomCl + 1;	//!!!
					fActiveMap[b] = 0;
					fClusters[nomCl].nofCluster = nomCl + 1;	//!!!
					CbmMvdStation* station= fStationMap[digiNeighbor->GetStationNr()];
					Float_t pixelSizeX  = digiNeighbor->GetPixelSizeX();
					Float_t pixelSizeY  = digiNeighbor->GetPixelSizeY();
					//std::cout<<"PixelSize: X: "<<pixelSizeX<<"; Y: "<<pixelSizeY<<"\n";
					Double_t layerRadius = station->GetRmax();
					//Double_t Xdigi = ( 0.5+double(chX) )*pixelSizeX - layerRadius;
					//Double_t Ydigi = ( 0.5+double(chY) )*pixelSizeY - layerRadius;
					Double_t Xdigi = (0.5 + (Double_t)chX) * pixelSizeX - layerRadius;
					Double_t Ydigi = (0.5 + (Double_t)chY) * pixelSizeY - layerRadius;
					//std::cout<<"Cluster: "<<nomCl<<"; Digi: "<<jDigi<<"; cX: "<<chX<<"; cY: "<<chY<<"; Xd: "<<Xdigi<<"; Yd: "<<Ydigi<<"\n";
					fClusters[nomCl].xc += Xdigi * digiNeighbor->GetCharge();
					fClusters[nomCl].yc += Ydigi * digiNeighbor->GetCharge();
					fClusters[nomCl].sumClCharge += digiNeighbor->GetCharge();
					fClusters[nomCl].digisInCluster[nofDigisInCluster] = fDigiMap[b];
					nofDigisInCluster++;
					fClusters[nomCl].nofDidis = nofDigisInCluster;
				}}
			}
			nomCl++;
		}
		/*else
		{
			digiNeighbor = (CbmMvdDigi*) fDigis->At(iDigi);
			std::cout<<"fCMap: "<<fClusterMap[a]<<"; fAMap: "<<fActiveMap[a]<<"; Ch: "<<digiNeighbor->GetCharge()<<"\n";
		}*/
	}
	/*for(Int_t iCl = 0; iCl < fNofClusters; iCl++)
	{
		fClusters[iCl].xc = fClusters[iCl].xc / fClusters[iCl].sumClCharge;
		fClusters[iCl].yc = fClusters[iCl].yc / fClusters[iCl].sumClCharge;
	}*/

	//std::cout<<"nomCl: "<<nomCl<<"\n";
	Int_t ch[15];
	for(Int_t i = 0; i < 15; i++)
	{
		ch[i] = 0;
	}
	fhMvdClusters->Fill("100", 0);
	fhMvdClusters->Fill("200", 0);
	fhMvdClusters->Fill("300", 0);
	fhMvdClusters->Fill("400", 0);
	fhMvdClusters->Fill("500", 0);
	fhMvdClusters->Fill("600", 0);
	fhMvdClusters->Fill("700", 0);
	fhMvdClusters->Fill("800", 0);
	fhMvdClusters->Fill("900", 0);
	fhMvdClusters->Fill("1000", 0);
	fhMvdClusters->Fill("1150", 0);
	fhMvdClusters->Fill("1300", 0);
	fhMvdClusters->Fill("1450", 0);
	fhMvdClusters->Fill("1700", 0);
	fhMvdClusters->Fill(">1700", 0);
	for(Int_t iCl = 0; iCl < nomCl; iCl++)
	{
		//fhMvdClusters->Fill((Int_t)fClusters[iCl].sumClCharge);
		if(fClusters[iCl].sumClCharge == 0)
		{
			cout<<"DIVISION ON ZERO\n";
		}
		else
		{
			fClusters[iCl].xc = fClusters[iCl].xc / fClusters[iCl].sumClCharge;
			fClusters[iCl].yc = fClusters[iCl].yc / fClusters[iCl].sumClCharge;
			//fhMvdClusters->Fill((Int_t)(fClusters[iCl].sumClCharge / 100));
			if(fClusters[iCl].sumClCharge < 100)
			{ch[0]++;
				fhMvdClusters->Fill("100", 1);}
			if((fClusters[iCl].sumClCharge >= 100) && (fClusters[iCl].sumClCharge < 200))
			{ch[1]++;
				fhMvdClusters->Fill("200", 1);}
			if((fClusters[iCl].sumClCharge >= 200) && (fClusters[iCl].sumClCharge < 300))
			{ch[2]++;
				fhMvdClusters->Fill("300", 1);}
			if((fClusters[iCl].sumClCharge >= 300) && (fClusters[iCl].sumClCharge < 400))
			{ch[3]++;
				fhMvdClusters->Fill("400", 1);}
			if((fClusters[iCl].sumClCharge >= 400) && (fClusters[iCl].sumClCharge < 500))
			{ch[4]++;
				fhMvdClusters->Fill("500", 1);}
			if((fClusters[iCl].sumClCharge >= 500) && (fClusters[iCl].sumClCharge < 600))
			{ch[5]++;
				fhMvdClusters->Fill("600", 1);}
			if((fClusters[iCl].sumClCharge >= 600) && (fClusters[iCl].sumClCharge < 700))
			{ch[6]++;
				fhMvdClusters->Fill("700", 1);}
			if((fClusters[iCl].sumClCharge >= 700) && (fClusters[iCl].sumClCharge < 800))
			{ch[7]++;(7);
				fhMvdClusters->Fill("800", 1);}
			if((fClusters[iCl].sumClCharge >= 800) && (fClusters[iCl].sumClCharge < 900))
			{ch[8]++;
				fhMvdClusters->Fill("900", 1);}
			if((fClusters[iCl].sumClCharge >= 900) && (fClusters[iCl].sumClCharge < 1000))
			{ch[9]++;
				fhMvdClusters->Fill("1000", 1);}
			if((fClusters[iCl].sumClCharge >= 1000) && (fClusters[iCl].sumClCharge < 1150))
			{ch[10]++;
				fhMvdClusters->Fill("1150", 1);}
			if((fClusters[iCl].sumClCharge >= 1150) && (fClusters[iCl].sumClCharge < 1300))
			{ch[11]++;
				fhMvdClusters->Fill("1300", 1);}
			if((fClusters[iCl].sumClCharge >= 1300) && (fClusters[iCl].sumClCharge < 1450))
			{ch[12]++;
				fhMvdClusters->Fill("1450", 1);}
			if((fClusters[iCl].sumClCharge >= 1450) && (fClusters[iCl].sumClCharge <= 1700))
			{ch[13]++;
				fhMvdClusters->Fill("1700", 1);}
			if(fClusters[iCl].sumClCharge > 1700)
			{ch[14]++;
				fhMvdClusters->Fill(">1700", 1);}
			if(fClusters[iCl].sumClCharge < 100)
			{a100++;}
			else
			{
				if(fClusters[iCl].sumClCharge < 500)
				{a500++;}
				else
				{
					if(fClusters[iCl].sumClCharge < 1000)
					{a1000++;}
					else
					{a0++;}
				}
			}
		}
	}
	/*for(Int_t i = 0; i < 15; i++)
	{
		std::cout<<"Ch["<<i<<"] = "<<ch[i]<<"\n";
	}
	std::cout<<"<100: "<<a100<<"\n100 - 500: "<<a500<<"\n500 - 1000: "<<a1000<<"\n>1000: "<<a0<<"\n";*/
	fNofClusters = nomCl;

	Float_t eff = (Float_t)fNofClusters / (Float_t)fNofPoints;
	if(eff > 1)
	{
		fEfficiency += 1;
		fFakeCl += (fNofClusters - fNofPoints);
	}
	else
	{
		fEfficiency += eff;
		fLooseCl += (fNofPoints - fNofClusters);
	}
	/*TVector3 pos, dpos;
	 dpos.SetXYZ(0.0005, 0.0005, 0.);
	for(Int_t iCl = 0; iCl < fNofClusters; iCl++)
	{
		CbmMvdDigi* digi = (CbmMvdDigi*) fDigis->At(fClusters[iCl].nDigiMax);
		Int_t stNr = digi->GetStationNr();
		CbmMvdStation* station= fStationMap[stNr];
		pos.SetXYZ(fClusters[iCl].xc, fClusters[iCl].yc, station->GetZ());
		Int_t nHits = fHits->GetEntriesFast();
		new ((*fHits)[nHits]) CbmMvdHit(stNr, pos, dpos, 0);
	}*/
}

void CbmMvdClustering::ChangeClusters(pair<Int_t, Int_t> h, Int_t nCl0, Int_t nCl1)
{
	Int_t chX = h.first;
	Int_t chY = h.second;
	fClusterMap[h] = nCl1;
	pair<Int_t, Int_t> a(0, 0);
	for(Int_t i = -1; i <= 1; i++)
	{
		for(Int_t j = -1; j <= 1; j++)
		{
			a = std::make_pair(chX + i, chY + j);
			if((fClusterMap[a] == nCl0) && (fActiveMap[a] == 1))
			{
				ChangeClusters(a, nCl0, nCl1);
			}
		}
	}
}

void CbmMvdClustering::CalculateAcuracy()
{
	Float_t totalError = 0;
	Float_t* errorX = new Float_t[fNofClusters];
	Float_t* errorY = new Float_t[fNofClusters];
	Float_t* errorXY = new Float_t[fNofClusters];

	Float_t* errorXYp = new Float_t[fNofClusters];

	Int_t Points[fNofClusters];
	Int_t E[15];
	for(Int_t i = 0; i < 15; i++)
	{
		E[i] = 0;
	}

	Float_t dMin = 10000;
	Float_t dMax = 0;

	fhMvdErrors->Fill("< 0.001", 0);
	fhMvdErrors->Fill("0.002", 0);
	fhMvdErrors->Fill("0.003", 0);
	fhMvdErrors->Fill("0.004", 0);
	fhMvdErrors->Fill("0.005", 0);
	fhMvdErrors->Fill("0.007", 0);
	fhMvdErrors->Fill("0.009", 0);
	fhMvdErrors->Fill("0.010", 0);
	fhMvdErrors->Fill("0.015", 0);
	fhMvdErrors->Fill("0.020", 0);
	fhMvdErrors->Fill("0.025", 0);
	fhMvdErrors->Fill("0.030", 0);
	fhMvdErrors->Fill("0.035", 0);
	fhMvdErrors->Fill("0.040", 0);
	fhMvdErrors->Fill("> 0.040", 0);

	for(Int_t iCl = 0; iCl < fNofClusters; iCl++)
	{
		errorX[iCl] = 0;
		errorY[iCl] = 0;
		errorXY[iCl] = 0;
		errorXYp[iCl] = 0;
		Float_t distMin = 10000;
		Float_t dist = 10001;
		Int_t nCl = iCl;
		Int_t nPoint = 0;
		Float_t xx, yy;
		UInt_t nDigiCharge = 0;
		for(Int_t iDigi = 0; iDigi < fClusters[iCl].nofDidis; iDigi++)
		{
			CbmMvdDigi* digi = (CbmMvdDigi*) fDigis->At(fClusters[iCl].digisInCluster[iDigi]);
			if(digi->GetCharge() > nDigiCharge)
			{
				nDigiCharge = digi->GetCharge();
				fClusters[iCl].nDigiMax = fClusters[iCl].digisInCluster[iDigi];
			}
		}
		CbmMvdDigi* digi = (CbmMvdDigi*) fDigis->At(fClusters[iCl].nDigiMax);
		Int_t stNr = digi->GetStationNr();
		for(Int_t iPoint = 0; iPoint < fNofPoints; iPoint++)
		{
			CbmMvdPoint* point = (CbmMvdPoint*) fPoints->At(iPoint);
			if(/*point->GetStationNr() == 2*/stNr == point->GetStationNr())
			{
				Float_t xP = point->GetXOut();
				Float_t yP = point->GetYOut();
				//std::cout<<"Point: "<<iPoint<<"; X: "<<xP<<"; Y: "<<yP<<"\n";
				dist = sqrt((fClusters[iCl].xc - xP) * (fClusters[iCl].xc - xP) +
						(fClusters[iCl].yc - yP) * (fClusters[iCl].yc - yP));
				if(dist < distMin)
				{
					distMin = dist;
					nPoint = iPoint;
					xx = xP;
					yy = yP;
				}
			}
		}
		Points[iCl] = nPoint;
		errorX[iCl] = fabs(fClusters[iCl].xc - xx);
		errorY[iCl] = fabs(fClusters[iCl].yc - yy);
		errorXY[iCl] = distMin;
		if(errorXY[iCl] < dMin)dMin = errorXY[iCl];
		if(errorXY[iCl] > dMax)dMax = errorXY[iCl];
		fClusters[iCl].nMCPoint = nPoint;

		if(errorXY[iCl] < 0.001)
		{E[0]++;
		fhMvdErrors->Fill("< 0.001", 1);}
		if((errorXY[iCl] >= 0.001) && (errorXY[iCl] < 0.002))
		{E[1]++;
		fhMvdErrors->Fill("0.002", 1);}
		if((errorXY[iCl] >= 0.002) && (errorXY[iCl] < 0.003))
		{E[2]++;
		fhMvdErrors->Fill("0.003", 1);}
		if((errorXY[iCl] >= 0.003) && (errorXY[iCl] < 0.004))
		{E[3]++;
		fhMvdErrors->Fill("0.004", 1);}
		if((errorXY[iCl] >= 0.004) && (errorXY[iCl] < 0.005))
		{E[4]++;
		fhMvdErrors->Fill("0.005", 1);}
		if((errorXY[iCl] >= 0.005) && (errorXY[iCl] < 0.007))
		{E[5]++;
		fhMvdErrors->Fill("0.007", 1);}
		if((errorXY[iCl] >= 0.007) && (errorXY[iCl] < 0.009))
		{E[6]++;
		fhMvdErrors->Fill("0.009", 1);}
		if((errorXY[iCl] >= 0.009) && (errorXY[iCl] < 0.01))
		{E[7]++;
		fhMvdErrors->Fill("0.010", 1);}
		if((errorXY[iCl] >= 0.01) && (errorXY[iCl] < 0.015))
		{E[8]++;
		fhMvdErrors->Fill("0.015", 1);}
		if((errorXY[iCl] >= 0.015) && (errorXY[iCl] < 0.02))
		{E[9]++;
		fhMvdErrors->Fill("0.020", 1);}
		if((errorXY[iCl] >= 0.02) && (errorXY[iCl] < 0.025))
		{E[10]++;
		fhMvdErrors->Fill("0.025", 1);}
		if((errorXY[iCl] >= 0.025) && (errorXY[iCl] < 0.03))
		{E[11]++;
		fhMvdErrors->Fill("0.030", 1);}
		if((errorXY[iCl] >= 0.03) && (errorXY[iCl] < 0.035))
		{E[12]++;
		fhMvdErrors->Fill("0.035", 1);}
		if((errorXY[iCl] >= 0.035) && (errorXY[iCl] < 0.04))
		{E[13]++;
		fhMvdErrors->Fill("0.040", 1);}
		if(errorXY[iCl] >= 0.04)
		{E[14]++;
		fhMvdErrors->Fill("> 0.040", 1);}
		//std::cout<<"Cluster: "<<nCl<<"; Point: "<<nPoint<<"; dist: "<<errorXY[iCl]<<"\n";
	}
	/*std::cout<<"---fhMvdErrors---\n";
	std::cout<<"0.000 - 0.001: "<<E[0]<<"\n";
	std::cout<<"0.001 - 0.002: "<<E[1]<<"\n";
	std::cout<<"0.002 - 0.003: "<<E[2]<<"\n";
	std::cout<<"0.003 - 0.004: "<<E[3]<<"\n";
	std::cout<<"0.004 - 0.005: "<<E[4]<<"\n";
	std::cout<<"0.005 - 0.007: "<<E[5]<<"\n";
	std::cout<<"0.007 - 0.009: "<<E[6]<<"\n";
	std::cout<<"0.009 - 0.010: "<<E[7]<<"\n";
	std::cout<<"0.010 - 0.015: "<<E[8]<<"\n";
	std::cout<<"0.015 - 0.020: "<<E[9]<<"\n";
	std::cout<<"0.020 - 0.025: "<<E[10]<<"\n";
	std::cout<<"0.025 - 0.030: "<<E[11]<<"\n";
	std::cout<<"0.030 - 0.035: "<<E[12]<<"\n";
	std::cout<<"0.035 - 0.040: "<<E[13]<<"\n";
	std::cout<<"0.040 - 9.999: "<<E[14]<<"\n";
	std::cout<<"dMin: "<<dMin<<"; dMax: "<<dMax<<"\n";*/

	Float_t clcl = 0;
	for(Int_t i = 0; i < fNofClusters; i++)
	{
		for(Int_t j = 0; j < fNofClusters; j++)
		{
			if((Points[i] == Points[j]) && (i != j))
			{
				std::cout<<"Cl1: "<<i<<"; Cl2: "<<j<<"\n";
				std::cout<<"->err1: "<<errorXY[i]<<"\n";
				clcl++;
			}
		}
	}
	std::cout<<"clcl: "<<clcl<<"\n";
	//---fhMvdErrorsByPixels---
	Int_t Ep[10];
	for(Int_t i = 0; i < 10; i++)
	{
		Ep[i] = 0;
	}
	Float_t mm = 0, mn = 10000;
	for(Int_t iCl = 0; iCl < fNofClusters; iCl++)
	{
		CbmMvdDigi* digi = (CbmMvdDigi*) fDigis->At(fClusters[iCl].nDigiMax);
		CbmMvdStation* station= fStationMap[digi->GetStationNr()];
		Double_t layerRadius = station->GetRmax();
		Int_t chX = digi->GetPixelX();
		Int_t chY = digi->GetPixelY();
		Float_t pixelSizeX  = digi->GetPixelSizeX();
		Float_t pixelSizeY  = digi->GetPixelSizeY();
		Double_t Xdigi = (0.5 + (Double_t)chX) * pixelSizeX - layerRadius;
		Double_t Ydigi = (0.5 + (Double_t)chY) * pixelSizeY - layerRadius;
		errorXYp[iCl] = sqrt(((fClusters[iCl].xc - Xdigi) * (fClusters[iCl].xc - Xdigi)) +
				((fClusters[iCl].yc - Ydigi) * (fClusters[iCl].yc - Ydigi)));
		if(errorXYp[iCl] > mm)mm = errorXYp[iCl];
		if(errorXYp[iCl] < mn)mn = errorXYp[iCl];
	}
	std::cout<<"MAX ERROR: "<<mm<<"\n";
	std::cout<<"MIN ERROR: "<<mn<<"\n";
	fhMvdErrorsByPixels->Fill("< 0.0002", 0);
	fhMvdErrorsByPixels->Fill("0.0004", 0);
	fhMvdErrorsByPixels->Fill("0.0006", 0);
	fhMvdErrorsByPixels->Fill("0.0008", 0);
	fhMvdErrorsByPixels->Fill("0.0010", 0);
	fhMvdErrorsByPixels->Fill("0.0012", 0);
	fhMvdErrorsByPixels->Fill("0.0015", 0);
	fhMvdErrorsByPixels->Fill("0.0018", 0);
	fhMvdErrorsByPixels->Fill("0.0020", 0);
	fhMvdErrorsByPixels->Fill("> 0.0020", 0);
	for(Int_t iCl = 0; iCl < fNofClusters; iCl++)
	{
		if(errorXYp[iCl] < 0.0002)
		{Ep[0]++;
		fhMvdErrorsByPixels->Fill("< 0.0002", 1);}
		if((errorXYp[iCl] >= 0.0002) && (errorXYp[iCl] < 0.0004))
		{Ep[1]++;
		fhMvdErrorsByPixels->Fill("0.0004", 1);}
		if((errorXYp[iCl] >= 0.0004) && (errorXYp[iCl] < 0.0006))
		{Ep[2]++;
		fhMvdErrorsByPixels->Fill("0.0006", 1);}
		if((errorXYp[iCl] >= 0.0006) && (errorXYp[iCl] < 0.0008))
		{Ep[3]++;
		fhMvdErrorsByPixels->Fill("0.0008", 1);}
		if((errorXYp[iCl] >= 0.0008) && (errorXYp[iCl] < 0.0010))
		{Ep[4]++;
		fhMvdErrorsByPixels->Fill("0.0010", 1);}
		if((errorXYp[iCl] >= 0.0010) && (errorXYp[iCl] < 0.0012))
		{Ep[5]++;
		fhMvdErrorsByPixels->Fill("0.0012", 1);}
		if((errorXYp[iCl] >= 0.0012) && (errorXYp[iCl] < 0.0015))
		{Ep[6]++;
		fhMvdErrorsByPixels->Fill("0.0015", 1);}
		if((errorXYp[iCl] >= 0.0015) && (errorXYp[iCl] < 0.0018))
		{Ep[7]++;
		fhMvdErrorsByPixels->Fill("0.0018", 1);}
		if((errorXYp[iCl] >= 0.0018) && (errorXYp[iCl] <= 0.0020))
		//if((errorXYp[iCl] > 0.0020) && (errorXYp[iCl] <= 10000))
		{Ep[8]++;
		fhMvdErrorsByPixels->Fill("0.0020", 1);}
		if(errorXYp[iCl] > 0.0020)
		//if((errorXYp[iCl] > -10000) && (errorXYp[iCl] <= 0.0020))
		{Ep[9]++;
		fhMvdErrorsByPixels->Fill("> 0.0020", 1);}

		//fhMvdErrorsByPixels->Fill("all", 1);
	}
	/*std::cout<<"---fhMvdErrorsByPixels---\n";
	std::cout<<"0.0000 - 0.0002: "<<Ep[0]<<"\n";
	std::cout<<"0.0002 - 0.0004: "<<Ep[1]<<"\n";
	std::cout<<"0.0004 - 0.0006: "<<Ep[2]<<"\n";
	std::cout<<"0.0006 - 0.0008: "<<Ep[3]<<"\n";
	std::cout<<"0.0008 - 0.0010: "<<Ep[4]<<"\n";
	std::cout<<"0.0010 - 0.0012: "<<Ep[5]<<"\n";
	std::cout<<"0.0012 - 0.0015: "<<Ep[6]<<"\n";
	std::cout<<"0.0015 - 0.0018: "<<Ep[7]<<"\n";
	std::cout<<"0.0018 - 0.0020: "<<Ep[8]<<"\n";
	std::cout<<"> 0.0020: "<<Ep[9]<<"\n";*/
}

void CbmMvdClustering::FillHistogramsForRadius()
{
	Float_t errPoint_Hit[nRadiusSteps];
	Float_t errPoint_Pixel[nRadiusSteps];
	Float_t errHit_Pixel[nRadiusSteps];
	Int_t nofPoint_Hit[nRadiusSteps];
	Int_t nofPoint_Pixel[nRadiusSteps];
	Int_t nofHit_Pixel[nRadiusSteps];
	for(Int_t i = 0; i < nRadiusSteps; i++)
	{
		errPoint_Hit[i] = 0;
		errPoint_Pixel[i] = 0;
		errHit_Pixel[i] = 0;
		nofPoint_Hit[i] = 0;
		nofPoint_Pixel[i] = 0;
		nofHit_Pixel[i] = 0;
	}
	//(Int_t)(sqrt(x*x + y*y) / (layerRadius/10))
	for(Int_t iCl = 0; iCl < fNofClusters; iCl++)
	{
		CbmMvdDigi* digi = (CbmMvdDigi*) fDigis->At(fClusters[iCl].nDigiMax);
		CbmMvdStation* station= fStationMap[digi->GetStationNr()];
		Double_t layerRadius = station->GetRmax();
		Int_t nRadius = static_cast<int>((sqrt((fClusters[iCl].xc * fClusters[iCl].xc) +
				(fClusters[iCl].yc * fClusters[iCl].yc))) / (layerRadius / (Float_t)nRadiusSteps));
		if(nRadius >= nRadiusSteps){std::cout<<"Error! nRadius is "<<nRadius<<"\n";
		}
		nofPoint_Hit[nRadius]++;
		nofPoint_Pixel[nRadius]++;
		nofHit_Pixel[nRadius]++;
		CbmMvdPoint* point = (CbmMvdPoint*) fPoints->At(fClusters[iCl].nMCPoint);
		Int_t chX = digi->GetPixelX();
		Int_t chY = digi->GetPixelY();
		Float_t pixelSizeX  = digi->GetPixelSizeX();
		Float_t pixelSizeY  = digi->GetPixelSizeY();
		Double_t Xdigi = (0.5 + (Double_t)chX) * pixelSizeX - layerRadius;
		Double_t Ydigi = (0.5 + (Double_t)chY) * pixelSizeY - layerRadius;
		errPoint_Hit[nRadius] += sqrt(((fClusters[iCl].xc - point->GetXOut()) *
				(fClusters[iCl].xc - point->GetXOut())) +
				((fClusters[iCl].yc - point->GetYOut()) * (fClusters[iCl].yc - point->GetYOut())));
		errPoint_Pixel[nRadius] += sqrt(((Xdigi - point->GetXOut()) * (Xdigi - point->GetXOut())) +
				((Ydigi - point->GetYOut()) * (Ydigi - point->GetYOut())));
		errHit_Pixel[nRadius] += sqrt(((fClusters[iCl].xc - Xdigi) * (fClusters[iCl].xc - Xdigi)) +
				((fClusters[iCl].yc - Ydigi) * (fClusters[iCl].yc - Ydigi)));
	}
	for(Int_t i = 0; i < nRadiusSteps; i++)
	{
		if(nofPoint_Hit[i] == 0)nofPoint_Hit[i] = 1;
		if(nofPoint_Pixel[i] == 0)nofPoint_Pixel[i] = 1;
		if(nofHit_Pixel[i] == 0)nofHit_Pixel[i] = 1;
		errPoint_Hit[i] = errPoint_Hit[i] / nofPoint_Hit[i];
		errPoint_Pixel[i] = errPoint_Pixel[i] / nofPoint_Pixel[i];
		errHit_Pixel[i] = errHit_Pixel[i] / nofHit_Pixel[i];
		fhErrPoint_Hit->Fill(i, errPoint_Hit[i]);
		fhErrPoint_Pixel->Fill(i, errPoint_Pixel[i]);
		fhErrHit_Pixel->Fill(i, errHit_Pixel[i]);
		/*std::cout<<"---"<<i<<"---\n";
		std::cout<<"errPoint_Hit: "<<errPoint_Hit[i]<<"\n";
		std::cout<<"errPoint_Pixel: "<<errPoint_Pixel[i]<<"\n";
		std::cout<<"errHit_Pixel: "<<errHit_Pixel[i]<<"\n";*/
	}
}

void CbmMvdClustering::TestStrangeClusters()
{
	TString fnameCl = "/u/gkozlov/cbm/events/mvd/_sCl.txt";
	FILE* sCl = fopen(fnameCl , "w");
	Int_t nsCl = 0;
	for(Int_t iCl = 0; iCl < fNofClusters; iCl++)
	{
		CbmMvdDigi* digi = (CbmMvdDigi*) fDigis->At(fClusters[iCl].nDigiMax);
		CbmMvdStation* station= fStationMap[digi->GetStationNr()];
		Double_t layerRadius = station->GetRmax();
		Int_t chX = digi->GetPixelX();
		Int_t chY = digi->GetPixelY();
		Float_t pixelSizeX  = digi->GetPixelSizeX();
		Float_t pixelSizeY  = digi->GetPixelSizeY();
		Double_t Xdigi = (0.5 + (Double_t)chX) * pixelSizeX - layerRadius;
		Double_t Ydigi = (0.5 + (Double_t)chY) * pixelSizeY - layerRadius;
		Float_t errHit_Pixel = sqrt(((fClusters[iCl].xc - Xdigi) * (fClusters[iCl].xc - Xdigi)) +
				((fClusters[iCl].yc - Ydigi) * (fClusters[iCl].yc - Ydigi)));
		CbmMvdPoint* point = (CbmMvdPoint*) fPoints->At(fClusters[iCl].nMCPoint);
		Float_t errPoint_Pixel = sqrt(((Xdigi - point->GetXOut()) * (Xdigi - point->GetXOut())) +
				((Ydigi - point->GetYOut()) * (Ydigi - point->GetYOut())));
		if(errHit_Pixel > 0.00085)
		{
			std::cout<<"sCl: "<<iCl<<"; Ch: "<<fClusters[iCl].sumClCharge<<"; nofD: "
					<<fClusters[iCl].nofDidis<<"; errH_P: "<<errHit_Pixel<<"; errP_P: "<<errPoint_Pixel
					<<"; Xc: "<<fClusters[iCl].xc<<"; Yc: "<<fClusters[iCl].yc<<"\n";
			fprintf(sCl, "Cl: %d; Ch: %d; nofD: %d; err: %.5f; errP_P: %.5f; Xc: %.5f; Yc: %.5f\n",
					iCl, fClusters[iCl].sumClCharge,fClusters[iCl].nofDidis, errHit_Pixel,
					errPoint_Pixel, fClusters[iCl].xc, fClusters[iCl].yc);
			for(Int_t iDigi = 0; iDigi < fClusters[iCl].nofDidis; iDigi++)
			{
				CbmMvdDigi* ndigi = (CbmMvdDigi*) fDigis->At(fClusters[iCl].digisInCluster[iDigi]);
				fprintf(sCl, "->Digi: %d; Ch: %.3f; X: %d; Y: %.d\n", fClusters[iCl].digisInCluster[iDigi],
						ndigi->GetCharge(), ndigi->GetPixelX(), ndigi->GetPixelY());
			}
			fprintf(sCl, "\n");
			nsCl++;

		}
	}
	fclose(sCl);
	TString fnameSt = "/u/gkozlov/cbm/events/mvd/_sSt.txt";
	FILE* sSt = fopen(fnameSt , "w");
	CbmMvdStation* station1 = fStationMap[1];
	fprintf(sSt, "Station 1; Rmin = %.7f; Rmax = %.7f; Z = %.7f; Thickness = %.7f\n\n\n",
			station1->GetRmin(), station1->GetRmax(), station1->GetZ(), station1->GetD());
	CbmMvdStation* station2 = fStationMap[2];
	fprintf(sSt, "Station 2; Rmin = %.7f; Rmax = %.7f; Z = %.7f; Thickness = %.7f\n\n\n",
			station2->GetRmin(), station2->GetRmax(), station2->GetZ(), station2->GetD());
	fclose(sSt);
	std::cout<<"nsCl = "<<nsCl<<"\n";
}

void CbmMvdClustering::PrintClusters()
{
	Float_t eHit_Pixel = 0;
	Float_t eHit_Point = 0;
	Float_t ePoint_Pixel = 0;
	TString fname = "/u/gkozlov/cbm/events/cc.25gev.centr_draw10.txt";
	FILE* f1 = fopen(fname , "a");
	fprintf(f1, "\n--->START EVENT<---\n");
	fprintf(f1, "Points: %d; St1: %d; St2: %d\nDigis: %d; St1: %d; St2: %d\nClusters: %d\n\n",
			fNofPoints, fNofPointsBySt[0], fNofPointsBySt[1],
			fNofDigis, fNofDigisBySt[0], fNofDigisBySt[1], fNofClusters);
	for(Int_t iCl = 0; iCl < fNofClusters; iCl++)
	{
		CbmMvdDigi* digi = (CbmMvdDigi*) fDigis->At(fClusters[iCl].nDigiMax);
		CbmMvdStation* station= fStationMap[digi->GetStationNr()];
		Double_t layerRadius = station->GetRmax();
		Int_t chX = digi->GetPixelX();
		Int_t chY = digi->GetPixelY();
		Float_t pixelSizeX  = digi->GetPixelSizeX();
		Float_t pixelSizeY  = digi->GetPixelSizeY();
		Double_t Xdigi = (0.5 + (Double_t)chX) * pixelSizeX - layerRadius;
		Double_t Ydigi = (0.5 + (Double_t)chY) * pixelSizeY - layerRadius;
		Float_t errHit_Pixel = sqrt(((fClusters[iCl].xc - Xdigi) * (fClusters[iCl].xc - Xdigi)) +
				((fClusters[iCl].yc - Ydigi) * (fClusters[iCl].yc - Ydigi)));
		CbmMvdPoint* point = (CbmMvdPoint*) fPoints->At(fClusters[iCl].nMCPoint);
		Float_t errPoint_Pixel = sqrt(((Xdigi - point->GetXOut()) * (Xdigi - point->GetXOut())) +
				((Ydigi - point->GetYOut()) * (Ydigi - point->GetYOut())));
		Float_t errHit_Point = sqrt(((fClusters[iCl].xc - point->GetXOut()) *
				(fClusters[iCl].xc - point->GetXOut())) + ((fClusters[iCl].yc - point->GetYOut()) *
						(fClusters[iCl].yc - point->GetYOut())));
		eHit_Pixel += errHit_Pixel;
		eHit_Point += errHit_Point;
		ePoint_Pixel += errPoint_Pixel;
		std::cout<<"Cl: "<<iCl<<"; Station: "<<digi->GetStationNr()<<"\n-Xc: "<<
				fClusters[iCl].xc<<"; Yc: "<<fClusters[iCl].yc<<"; Charge: "<<
				fClusters[iCl].sumClCharge<<"; nDigis: "<<fClusters[iCl].nofDidis<<
				"\n--errHit_Point: "<<errHit_Point<<
				"\n--errHit_Pixel: "<<errHit_Pixel<<
				"\n--errPoint_Pixel: "<<errPoint_Pixel<<"\n";
		fprintf(f1, "Cl: %d; Station: %d\n-Xc: %.7f; Yc: %.7f; Charge: %d; nDigis: %d\n--errHit_Point: %.7f\n--errHit_Pixel: %.7f\n--errPoint_Pixel: %.7f\n\n",
				iCl, digi->GetStationNr(), fClusters[iCl].xc, fClusters[iCl].yc,
				fClusters[iCl].sumClCharge, fClusters[iCl].nofDidis,
				errHit_Point, errHit_Pixel, errPoint_Pixel);
	}
	fclose(f1);
	fErrHit_Pixel += eHit_Pixel / (Float_t)fNofClusters;
	fErrHit_Point += eHit_Point / (Float_t)fNofClusters;
	fErrPoint_Pixel += ePoint_Pixel / (Float_t)fNofClusters;
}
ClassImp(CbmMvdClustering);
