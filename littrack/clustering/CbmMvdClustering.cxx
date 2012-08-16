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
#include "draw/CbmDrawHist.h"

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
		/*fhErrPoint_Hit(NULL),
		fhErrPoint_Pixel(NULL),
		fhErrHit_Pixel(NULL),*/
		fhErrorsHit_MCPoint(NULL),
		fhErrorsHit_Pixel(NULL),
		fhErrorsPixel_MCPoint(NULL),
		fErrHit_Point(),
		fErrHit_Pixel(),
		fErrPoint_Pixel(),
		fNofClusters(),
		fClusters(),
		fNofDigis(),
		fDigis(),
		fNofPoints(),
		fPoints(),
		fHits(),
		fCluster(),
		fMatches()
{
	fNofDigisBySt[0] = 0;
	fNofDigisBySt[1] = 0;
	fNofPointsBySt[0] = 0;
	fNofPointsBySt[1] = 0;
	fEfficiency = 0;
	fNofEvents = 0;
	fLooseCl = 0;
	fFakeCl = 0;
	fNofClustersBySt[0] = 0;
	fNofClustersBySt[1] = 0;
	fLooseClustersBySt[0] = 0;
	fLooseClustersBySt[1] = 0;
	fFakeClustersBySt[0] = 0;
	fFakeClustersBySt[1] = 0;
}

CbmMvdClustering::~CbmMvdClustering()
{
	if ( fHits ) {
	fHits->Delete();
	delete fHits;
	}
	if ( fCluster ) {
	fCluster->Delete();
	delete fCluster;
	}
	if ( fMatches ) {
	fMatches->Delete();
	delete fMatches;
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

   fCluster = new TClonesArray("CbmMvdCluster", 1000);
   ioman->Register("MvdCluster", "Cluster in MVD", fCluster, kTRUE);

   fMatches = new TClonesArray("CbmMvdHitMatch", 1000);
   ioman->Register("MvdHitMatch", "HitMatch in MVD", fMatches, kTRUE);
   //Register();
   //ReadDataBranches();

   /*fhErrPoint_Hit = new TH1F("fhErrPoint_HitByRadius", "hErrPoint_HitByRadius;Radius;Error", nRadiusSteps + 2, 0, 0);
   fhErrPoint_Pixel = new TH1F("fhErrPoint_PixelByRadius", "hErrPoint_PixelByRadius;Radius;Error", nRadiusSteps + 2, 0, 0);
   fhErrHit_Pixel = new TH1F("fhErrHit_PixelByRadius", "hErrHit_PixelByRadius;Radius;Error", nRadiusSteps + 2, 0, 0);*/

   fhErrorsHit_MCPoint = new TH1F("fhErrorsHit_MCPoint", "hErrorsHit_MCPoint;Error(cm);nofHits", 100, 0, 0.002);
   fhErrorsHit_Pixel = new TH1F("fhErrorsHit_Pixel", "hErrorsHit_Pixel;Error(cm);nofHits", 100, 0, 0.002);
   fhErrorsPixel_MCPoint = new TH1F("fhErrorsPixel_MCPoint", "hErrorsPixel_MCPoint;Error(cm);nofHits", 100, 0, 0.002);

   //TString fname = "/u/gkozlov/cbm/events/cc.25gev.centr_draw10.txt";
   //TString fname = "/u/gkozlov/cbm/events/pc.25gev.centr_draw10.txt";
   //TString fname = "/u/gkozlov/cbm/events/pAu.25gev.centr_draw10.txt";
   TString fname = "/u/gkozlov/cbm/events/mvd.clustering.out.txt";
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
	fDigiMap.clear();
	fClusterMap.clear();
	//fChargeMap1.clear();
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
	    //fChargeMap1[a] = digi->GetCharge();
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
	fNofClustersBySt[0] = 0;
	fNofClustersBySt[1] = 0;
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

	//std::cout<<"Digis: "<<fNofDigis<<"\n";
	//std::cout<<"Points: "<<fNofPoints<<"\n";
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
   //std::cout<<"---fErrHit_Point: "<<fErrHit_Point<<"; fNofEvents: "<<fNofEvents<<"\n";
   fErrPoint_Pixel = fErrPoint_Pixel / (Float_t)fNofEvents;
   /*std::cout<<"ErrHit_Pixel: "<<fhErrorsHit_Pixel->GetMean(1)<<"\nErrHit_Point: "<<fhErrorsHit_MCPoint->GetMean(1)<<
		   "\nErrPoint_Pixel: "<<fhErrorsPixel_MCPoint->GetMean(1)<<"\n";*/
   std::cout<<"Loosing Clusters: "<<fLooseCl<<"; St1: "<<fLooseClustersBySt[0]<<
		   "; St2: "<<fLooseClustersBySt[1]<<"\nFake Clusters: "<<fFakeCl<<
		   "; St1: "<<fFakeClustersBySt[0]<<"; St2: "<<fFakeClustersBySt[1]<<"\n";
   //TString fname = "/u/gkozlov/cbm/events/cc.25gev.centr_draw10.txt";
   //TString fname = "/u/gkozlov/cbm/events/pc.25gev.centr_draw10.txt";
   //TString fname = "/u/gkozlov/cbm/events/pAu.25gev.centr_draw10.txt";
   /*TString fname = "/u/gkozlov/cbm/events/mvd.clustering.out.txt";
   FILE* f1 = fopen(fname , "a");
   fprintf(f1, "\n\n\nEfficiency: %.3f\nErrHit_Pixel: %.7f\nErrHit_Point: %.7f\nErrPoint_Pixel: %.7f",
		   fEfficiency, fErrHit_Pixel, fErrHit_Point, fErrPoint_Pixel);
   fclose(f1);*/
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
   TCanvas* canvasE1 = new TCanvas("fhErrPoint_HitByRadius", "fhErrPoint_HitByRadius", 700, 700);
   canvasE1->Divide(1, 1);
   canvasE1->cd(1);
   fhErrPoint_Hit->Draw();

   fhErrPoint_Pixel->Write();
   TCanvas* canvasE2 = new TCanvas("fhErrPoint_PixelByRadius", "fhErrPoint_PixelByRadius", 700, 700);
   canvasE2->Divide(1, 1);
   canvasE2->cd(1);
   fhErrPoint_Pixel->Draw();

   fhErrHit_Pixel->Write();
   TCanvas* canvasE3 = new TCanvas("fhErrHit_PixelByRadius", "fhErrHit_PixelByRadius", 700, 700);
   canvasE3->Divide(1, 1);
   canvasE3->cd(1);
   fhErrHit_Pixel->Draw();*/

   SetDefaultDrawStyle();

   fhErrorsHit_MCPoint->Scale(1. / fNofEvents);
   fhErrorsHit_MCPoint->Write();
   TCanvas* canvasHMP = new TCanvas("fhErrorsHit_MCPoint", "fhErrorsHit_MCPoint", 1000, 500);
   //canvasHMP->Divide(1, 1);
   //canvasHMP->cd(1);
   //fhErrorsHit_MCPoint->Draw();
   DrawH1(fhErrorsHit_MCPoint);//, kLinear, kLog);

   /*fhErrorsHit_Pixel->Scale(1. / fNofEvents);
   fhErrorsHit_Pixel->Write();
   TCanvas* canvasHP = new TCanvas("fhErrorsHit_Pixel", "fhErrorsHit_Pixel", 1000, 500);
   canvasHP->Divide(1, 1);
   canvasHP->cd(1);
   fhErrorsHit_Pixel->Draw();

   fhErrorsPixel_MCPoint->Scale(1. / fNofEvents);
   fhErrorsPixel_MCPoint->Write();
   TCanvas* canvasPMP = new TCanvas("fhErrorsPixel_MCPoint", "fhErrorsPixel_MCPoint", 1000, 500);
   canvasPMP->Divide(1, 1);
   canvasPMP->cd(1);
   fhErrorsPixel_MCPoint->Draw();*/
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
	Int_t x1[2500];
	Int_t y1[2500];
	for(Int_t i = 0; i < 1201; i++)
	{
		x1[i] = 0;
		y1[i] = 0;
	}
	/*Int_t cTest0 = 0;
	for(Int_t iDigi = 0; iDigi < fNofDigis; iDigi++)
	{
		CbmMvdDigi* digi = (CbmMvdDigi*) fDigis->At(iDigi);
		pair<Int_t, Int_t> n(digi->GetPixelX(), digi->GetPixelY());
		if(fChargeMap1[n] == 1)
		{
			cTest0++;
			std::cout<<"---cTest0: "<<cTest0<<"; x: "<<n.first<<"; y: "<<n.second<<"\n";
		}
	}*/

	//std::cout<<"FindClusters Start\n";
	Int_t channelX = 0;
	Int_t channelY = 0;
	CbmMvdDigi* digi = NULL;
	CbmMvdDigi* digiNeighbor = NULL;
	//Int_t nofClusters = fNofDigisBySt[0];
	//Int_t nofClusters = fNofDigisBySt[1];	//2
	Int_t nofClusters = 0;
	//Int_t nCl = 0;
	//for(Int_t iDigi = 0; iDigi < fNofDigisBySt[0]; iDigi++)
//std::cout<<"FindClusters Point1\n";
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
//std::cout<<"FindClusters Point2\n";
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
			{20
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
//std::cout<<"FindClusters Point3\n";
		//---
		//std::cout<<"->Step "<<iDigi<<"; Station: "<<digi->GetStationNr()<<"; Charge: "<<digi->GetCharge()<<"\n";
		if((channelX == channelXmax) && (channelY == channelYmax))
		{
			a = std::make_pair(channelX, channelY);
			//fChargeMap2[a] += fChargeMap1[a];
			//fActiveMap[a] = 1;
			fChargeMap1[a] = 1;
			/*std::cout<<"MAKE CLUSTER: "<<nofClusters<<"; x: "<<a.first<<"; y: "<<a.second<<
					"; Ch: "<<digi->GetCharge()<<"\n";*/
			//x1[nofClusters] = a.first;
			//y1[nofClusters] = a.second;
			nofClusters++;
//std::cout<<"FindClusters Point3.1\n";
		}
		else
		{
			a = std::make_pair(channelX, channelY);
			maxDigi = std::make_pair(channelXmax, channelYmax);
			//nofClusters--;
			//fChargeMap2[maxDigi] += fChargeMap1[a];
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
//std::cout<<"FindClusters Point3.2\n";
			if(nCluster!=fClusterMap[maxDigi])
			{
				ChangeClusters(a, nCluster, fClusterMap[maxDigi]);
			}
//std::cout<<"FindClusters Point3.3\n";
		}}//---
//std::cout<<"FindClusters Point4\n";
		/*pair<Int_t, Int_t> z(digi->GetPixelX(), digi->GetPixelY());
		std::cout<<"->Step "<<iDigi<<"; Station: "<<digi->GetStationNr()<<"; Charge: "<<
				digi->GetCharge()<<"; Cl: "<<fClusterMap[z]<<"; cm: "<<fChargeMap1[z]<<"\n";*/
	}
	//std::cout<<"FindClusters Finish Clustering Step\n";
	std::cout<<"Points: "<<fNofPoints<<"; P1: "<<fNofPointsBySt[0]<<"; P2:"<<fNofPointsBySt[1]<<"\n";
	std::cout<<"Digis: "<<fNofDigis<<"; D1: "<<fNofDigisBySt[0]<<"; D2:"<<fNofDigisBySt[1]<<"\n";
	//std::cout<<"Clusters: "<<nofClusters<<"\n";
	//std::cout<<"nCl: "<<nCl<<"\n";
				//nofClusters = nCl;	//10000 - OK ???

	Int_t cTest = 0;
	UInt_t ch0[1201];
	for(Int_t iDigi = 0; iDigi < fNofDigis; iDigi++)
	{
		CbmMvdDigi* digi = (CbmMvdDigi*) fDigis->At(iDigi);
		pair<Int_t, Int_t> n(digi->GetPixelX(), digi->GetPixelY());
		if(fChargeMap1[n] == 1)
		{
			x1[cTest] = n.first;
			y1[cTest] = n.second;
			for(Int_t i = 0; i < cTest; i++)
			{
				if((x1[i] == x1[cTest]) && (y1[i] == y1[cTest]))
				{
					//std::cout<<"->i = "<<i<<"; cTest = "<<cTest<<"\n";
					CbmMvdDigi* digi1 = (CbmMvdDigi*) fDigis->At(ch0[i]);
					//std::cout<<"---Ch1: "<<digi1->GetCharge()<<"; St: "<<digi1->GetStationNr()<<"\n";
					//std::cout<<"---Ch2: "<<digi->GetCharge()<<"; St: "<<digi->GetStationNr()<<"\n";
					if(digi1->GetStationNr() > digi->GetStationNr())
					{
						n = std::make_pair(digi1->GetPixelX(), digi1->GetPixelY());
					}
					fChargeMap1[n] = 0;
				}
			}
			ch0[cTest] = iDigi;
			cTest++;
			//std::cout<<"cTest: "<<cTest<<"; x: "<<n.first<<"; y: "<<n.second<<"\n";
		}
	}

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
		if((fChargeMap1[a] == 1) && (fClusterMap[a] != 0) && (digi->GetCharge() > 0))
		{
			//std::cout<<" - good"<<"; nomCl = "<<nomCl;
			Replase = fClusterMap[a];
			Int_t nofDigisInCluster = 0;
			//for(Int_t jDigi = 0; jDigi < fNofDigisBySt[0]; jDigi++)
			for(Int_t jDigi = 0; jDigi < fNofDigis; jDigi++)
			//for(Int_t jDigi = fNofDigisBySt[0]; jDigi < fNofDigisBySt[0] + fNofDigisBySt[1]; jDigi++)	//2
			{
				digiNeighbor = (CbmMvdDigi*) fDigis->At(jDigi);
				//if(digiNeighbor->GetCharge() == 0)std::cout<<"WARNING!___";
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
					//UInt_t digiNeighborCharge = (UInt_t)digiNeighbor->GetCharge();
					Float_t digiNeighborCharge = digiNeighbor->GetCharge();
					fClusters[nomCl].xc += Xdigi * digiNeighborCharge;
					fClusters[nomCl].yc += Ydigi * digiNeighborCharge;
					fClusters[nomCl].sumClCharge += digiNeighborCharge;
					fClusters[nomCl].digisInCluster[nofDigisInCluster] = fDigiMap[b];
					nofDigisInCluster++;
					fClusters[nomCl].nofDidis = nofDigisInCluster;
					/*std::cout<<"Cl: "<<fClusters[nomCl].nofCluster<<"; Ch: "<<
							fClusters[nomCl].sumClCharge<<"; Digis: "<<
							fClusters[nomCl].nofDidis<<"\n";*/
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

	/////std::cout<<"nomCl: "<<nomCl<<"; fNofClusters: "<<fNofClusters<<"\n";
	//if(fNofClusters > nomCl)std::cout<<"---Ch: "<<fClusters[nomCl+1].sumClCharge<<"\n";
	for(Int_t iCl = 0; iCl < nomCl; iCl++)
	{
		if(fClusters[iCl].sumClCharge == 0)
		{
			std::cout<<"DIVISION ON ZERO\n"<<"Ch: "<<fClusters[iCl].sumClCharge<<"\n";
			std::cout<<"Cl: "<<iCl<<"; X: "<<fClusters[iCl].xc<<
					"; Y: "<<fClusters[iCl].yc<<"; Digis: "<<fClusters[iCl].nofDidis<<"\n";
			nomCl--;
			for(Int_t jCl = iCl; jCl < nomCl; jCl++)
			{
				fClusters[jCl].nDigiMax = fClusters[jCl+1].nDigiMax;
				fClusters[jCl].nMCPoint = fClusters[jCl+1].nMCPoint;
				fClusters[jCl].nofCluster = fClusters[jCl+1].nofCluster;
				fClusters[jCl].nofDidis = fClusters[jCl+1].nofDidis;
				fClusters[jCl].sumClCharge = fClusters[jCl+1].sumClCharge;
				fClusters[jCl].xc = fClusters[jCl+1].xc;
				fClusters[jCl].yc = fClusters[jCl+1].yc;
				for(Int_t iDigi = 0; iDigi < fClusters[jCl+1].nofDidis; iDigi++)
				{
					fClusters[jCl].digisInCluster[iDigi] = fClusters[jCl+1].digisInCluster[iDigi];
				}
			}
			iCl--;
		}
		else
		{
			fClusters[iCl].xc = fClusters[iCl].xc / fClusters[iCl].sumClCharge;
			fClusters[iCl].yc = fClusters[iCl].yc / fClusters[iCl].sumClCharge;
			//std::cout<<"Cl: "<<iCl<<"; X: "<<fClusters[iCl].xc<<
			//		"; Y: "<<fClusters[iCl].yc<<"; Digis: "<<fClusters[iCl].nofDidis<<"\n";
		}
	}
	fNofClusters = nomCl;


	TVector3 pos, dpos;
	 dpos.SetXYZ(0.0002, 0.0002, 0.);
	for(Int_t iCl = 0; iCl < fNofClusters; iCl++)
	{
		CbmMvdDigi* digi = (CbmMvdDigi*) fDigis->At(fClusters[iCl].digisInCluster[0]);
		Int_t stNr = digi->GetStationNr();
		CbmMvdStation* station= fStationMap[stNr];

		//---
		Short_t chargeArray[49], dominatorArray[49];
		CbmMvdDigi* centralDigi;
		Float_t xPos[5],yPos[5];
		Short_t contributors=-2;
		Bool_t digiFound;
		Double_t layerRadius = station->GetRmax();
		for (Int_t k = 0;k < 5;k++)
		{
			xPos[k]=0;
			yPos[k]=0;
		}
		//std::cout<<"-Step 1\n";
		for( Int_t i = 0; i < 7; i++ )
		{
			for( Int_t j = 0; j < 7; j++ )
			{
				digiFound = kFALSE;
			    for(Int_t k = 0; k < fClusters[iCl].nofDidis; k++ )
			    {
			    	//std::cout<<"--Step 2\n";
			    	CbmMvdDigi* digi1 = (CbmMvdDigi*) fDigis->At(fClusters[iCl].digisInCluster[k]);
			    	Float_t pixelSizeX = digi1->GetPixelSizeX();
			    	Float_t pixelSizeY = digi1->GetPixelSizeY();
			    	//std::cout<<"--Step 3\n";
			    	Int_t xSeed = int( (fClusters[iCl].xc + layerRadius) / pixelSizeX );  // find index of seed pixel in the detector frame
			    	Int_t ySeed = int( (fClusters[iCl].yc + layerRadius) / pixelSizeY );
			    	//std::cout<<"--Step 4\n";
			    	if( (digi1->GetPixelX() - xSeed == i-3) && (digi1->GetPixelY() - ySeed == j-3) )
	    			{
	    			    chargeArray[7*i+j] = digi1->GetCharge();//???GetAdcCharge(digi->GetCharge())
		    			Short_t l=-1,trackNr=-1;
		    			do {
		    				l=l+1;
		    				if( xPos[l]==digi1->GetDominatorX() && yPos[l]==digi1->GetDominatorY() )
		    				{
		    					trackNr=l;
		    				};
	    			    }while ( xPos[l]!=0 && l<4 );  //4 is  the size of xPos[] array -> save up to 4 mc points
	    			    if (trackNr>=0) { dominatorArray[7*i+j]=trackNr; }
	    			    else if( trackNr == -1 && l == 4 ) {
	    				dominatorArray[7*i+j] = l;
	    				if (l>contributors) {contributors=l;}
	    			    }
	    			    else {
	    				xPos[l] = digi1->GetDominatorX();
	    				yPos[l]=digi1->GetDominatorY();
	    				dominatorArray[7*i+j]=l;
	    				if(l>contributors){contributors=l;}
	    			    }
		    			    digiFound = kTRUE;
			    			} // if
		    			if( !digiFound ){
	    			    dominatorArray[7*i+j] = 0;
			    			}
		    			//std::cout<<"--Step 5\n";
			    } // for k
			} // for j
		} // for i
		//std::cout<<"-Step 6\n";
		Int_t detId = digi->GetDetectorId();
		Float_t pixelSizeX  = digi->GetPixelSizeX();
		Float_t pixelSizeY  = digi->GetPixelSizeY();
		Int_t nClusters = fCluster->GetEntriesFast();
		//std::cout<<"-Step 7\n";
		CbmMvdCluster* clusterNew = new ((*fCluster)[nClusters])CbmMvdCluster(detId, pos, dpos, 0,  chargeArray, pixelSizeX, pixelSizeY);
		clusterNew->SetDebuggingInfo(dominatorArray,xPos,yPos);
		clusterNew->SetContributors(contributors);
		//std::cout<<"-Step 8\n";
		//---

		pos.SetXYZ(fClusters[iCl].xc, fClusters[iCl].yc, station->GetZ());
		Int_t nHits = fHits->GetEntriesFast();
		new ((*fHits)[nHits]) CbmMvdHit(stNr, pos, dpos, 0);
		//std::cout<<"-Step 9\n";
		centralDigi = (CbmMvdDigi*)fDigis->At(fClusters[iCl].digisInCluster[0]);
		new ((*fMatches)[nHits]) CbmMvdHitMatch(0, 0, centralDigi->GetTrackID(), centralDigi->GetPointID(), clusterNew->GetContributors());
		//std::cout<<"-Step 10\n";
	}
}

void CbmMvdClustering::ChangeClusters(pair<Int_t, Int_t> h, Int_t nCl0, Int_t nCl1)
{
	Int_t chX = h.first;
	Int_t chY = h.second;
	fClusterMap[h] = nCl1;
	pair<Int_t, Int_t> a(0, 0);
//std::cout<<"FindClusters Point3.2.1\n";
	for(Int_t i = -1; i <= 1; i++)
	{
		for(Int_t j = -1; j <= 1; j++)
		{
			a = std::make_pair(chX + i, chY + j);
//std::cout<<"FindClusters Point3.2.2\n";
			fClusterMapIt = fClusterMap.find(a);
			if (!(fClusterMapIt == fClusterMap.end()))
			{
			if((fClusterMap[a] == nCl0) /*&&(fChargeMap1[a]==1)*/&& (fActiveMap[a] == 1)/*&&((i!=0)&&(j!=0))*//* && (((chX + i) != chX) && ((chY + j) != chY))*/)
			{
//std::cout<<"FindClusters Point3.2.3; ChX= "<<chX + i<<"; ChY= "<<chY + j<<"\n";
				//std::cout<<"---0: "<<nCl0<<"; 1: "<<nCl1<<"\n";
				//if(nCl0 == nCl1)std::cout<<"WARNING! 0==1\n";
				ChangeClusters(a, nCl0, nCl1);
//std::cout<<"FindClusters Point3.2.4\n";
			}
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

	Float_t dMin = 10000;
	Float_t dMax = 0;

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
		fNofClustersBySt[stNr - 1]++;
		for(Int_t iPoint = 0; iPoint < fNofPoints; iPoint++)
		{
			CbmMvdPoint* point = (CbmMvdPoint*) fPoints->At(iPoint);
			if(/*point->GetStationNr() == 2*/stNr == point->GetStationNr())
			{
				//Changing calculation of the centers of MCPoints
				//Float_t xP = point->GetXOut();
				//Float_t yP = point->GetYOut();
				Float_t xP = 0.5 * (point->GetX() + point->GetXOut());
				Float_t yP = 0.5 * (point->GetY() + point->GetYOut());
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
		/*std::cout<<"Cl: "<<iCl<<"; X: "<<fClusters[iCl].xc<<"; Y: "<<
				fClusters[iCl].yc<<"; Charge: "<<fClusters[iCl].sumClCharge<<
				"; nDigi: "<<fClusters[iCl].nofDidis<<"\n";
		std::cout<<"Point: "<<nPoint<<"; Xp: "<<xx<<"; Yp: "<<yy<<"; Error: "<<
				distMin<<"\n";
		for(Int_t i = 0; i < fClusters[iCl].nofDidis; i++)
		{
			CbmMvdDigi* xdigi = (CbmMvdDigi*) fDigis->At(fClusters[iCl].digisInCluster[i]);
			std::cout<<"---Digi: "<<fClusters[iCl].digisInCluster[i]<<
					"; X: "<<xdigi->GetPixelX()<<"; Y: "<<xdigi->GetPixelY()<<
					"; Charge: "<<(UInt_t)xdigi->GetCharge()<<"\n";
		}*/
	}

	std::cout<<"Clusters: "<<fNofClusters<<"; St1: "<<fNofClustersBySt[0]<<
			"; St2: "<<fNofClustersBySt[1]<<"\n";

	//Float_t clcl = 0;
	Float_t fakeCl[2] = {0, 0};
	for(Int_t i = 0; i < fNofClusters; i++)
	{
		for(Int_t j = 0; j < fNofClusters; j++)
		{
			if((Points[i] == Points[j]) && (i != j))
			{
				//std::cout<<"Cl1: "<<i<<"; Cl2: "<<j<<"\n";
				//std::cout<<"->err1: "<<errorXY[i]<<"\n";
				//clcl++;
				CbmMvdDigi* digi = (CbmMvdDigi*) fDigis->At(fClusters[i].nDigiMax);
				Int_t stNr = digi->GetStationNr();
				fakeCl[stNr - 1]++;
				//fFakeClustersBySt[stNr - 1]++;
			}
		}
	}
	fFakeClustersBySt[0] += fakeCl[0] / 2;
	fFakeClustersBySt[1] += fakeCl[1] / 2;
	Float_t nP = fNofPoints;
	if((fNofDigisBySt[0] == 0) && (fNofPointsBySt[0] != 0))
	{
		nP -= fNofPointsBySt[0];
	}
	if((fNofDigisBySt[1] == 0) && (fNofPointsBySt[1] != 0))
	{
		nP -= fNofPointsBySt[1];
	}
	Float_t eff = (Float_t)fNofClusters / nP;
	//for points without digits / must be changed!
	//if(eff < 0.7)eff = 0.9;
	//---
	if(eff > 1)
	{
		fEfficiency += 1;
	}
	else
	{
		fEfficiency += eff;
	}
	for(Int_t iSt = 0; iSt < 2; iSt++)
	{
		if(fNofPointsBySt[iSt] > fNofClustersBySt[iSt])
		{
			fLooseClustersBySt[iSt] += fNofPointsBySt[iSt] + (Int_t)(fakeCl[iSt] / 2) - fNofClustersBySt[iSt];
		}
		/*if(fNofPointsBySt[iSt] < fNofClustersBySt[iSt])
		{
			fFakeClustersBySt[iSt] += fNofClustersBySt[iSt] - fNofPointsBySt[iSt];
		}*/
	}
	fFakeCl = fFakeClustersBySt[0] + fFakeClustersBySt[1];
	fLooseCl = fLooseClustersBySt[0] + fLooseClustersBySt[1];
	//std::cout<<"clcl: "<<clcl<<"\n";

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
}

/*void CbmMvdClustering::FillHistogramsForRadius()
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
		Float_t xP = 0.5 * (point->GetX() + point->GetXOut());
		Float_t yP = 0.5 * (point->GetY() + point->GetYOut());
		Int_t chX = digi->GetPixelX();
		Int_t chY = digi->GetPixelY();
		Float_t pixelSizeX  = digi->GetPixelSizeX();
		Float_t pixelSizeY  = digi->GetPixelSizeY();
		Double_t Xdigi = (0.5 + (Double_t)chX) * pixelSizeX - layerRadius;
		Double_t Ydigi = (0.5 + (Double_t)chY) * pixelSizeY - layerRadius;
		errPoint_Hit[nRadius] += sqrt(((fClusters[iCl].xc - xP) *
				(fClusters[iCl].xc - xP)) +
				((fClusters[iCl].yc - yP) * (fClusters[iCl].yc - yP)));
		errPoint_Pixel[nRadius] += sqrt(((Xdigi - xP) * (Xdigi - xP)) +
				((Ydigi - yP) * (Ydigi - yP)));
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
		//std::cout<<"---"<<i<<"---\n";
		//std::cout<<"errPoint_Hit: "<<errPoint_Hit[i]<<"\n";
		//std::cout<<"errPoint_Pixel: "<<errPoint_Pixel[i]<<"\n";
		//std::cout<<"errHit_Pixel: "<<errHit_Pixel[i]<<"\n";
	}
}*/

/*void CbmMvdClustering::TestStrangeClusters()
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
}*/

void CbmMvdClustering::PrintClusters()
{
	Float_t eHit_Pixel = 0;
	Float_t eHit_Point = 0;
	Float_t ePoint_Pixel = 0;
	//TString fname = "/u/gkozlov/cbm/events/cc.25gev.centr_draw10.txt";
	//TString fname = "/u/gkozlov/cbm/events/pc.25gev.centr_draw10.txt";
	//TString fname = "/u/gkozlov/cbm/events/pAu.25gev.centr_draw10.txt";
	/*TString fname = "/u/gkozlov/cbm/events/mvd.clustering.out.txt";
	FILE* f1 = fopen(fname , "a");
	fprintf(f1, "\n--->START EVENT<---\n");
	fprintf(f1, "Points: %d; St1: %d; St2: %d\nDigis: %d; St1: %d; St2: %d\nClusters: %d\n\n",
			fNofPoints, fNofPointsBySt[0], fNofPointsBySt[1],
			fNofDigis, fNofDigisBySt[0], fNofDigisBySt[1], fNofClusters);*/
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
		Float_t xP = 0.5 * (point->GetX() + point->GetXOut());
		Float_t yP = 0.5 * (point->GetY() + point->GetYOut());
		Float_t errPoint_Pixel = sqrt(((Xdigi - xP) * (Xdigi - xP)) +
				((Ydigi - yP) * (Ydigi - yP)));
		Float_t errHit_Point = sqrt(((fClusters[iCl].xc - xP) *
				(fClusters[iCl].xc - xP)) + ((fClusters[iCl].yc - yP) *
						(fClusters[iCl].yc - yP)));
		eHit_Pixel += errHit_Pixel;
		eHit_Point += errHit_Point;
		ePoint_Pixel += errPoint_Pixel;

		fhErrorsHit_MCPoint->Fill(errHit_Point);
		fhErrorsHit_Pixel->Fill(errHit_Pixel);
		fhErrorsPixel_MCPoint->Fill(errPoint_Pixel);
		/*std::cout<<"Cl: "<<iCl<<"; Station: "<<digi->GetStationNr()<<"\n-Xc: "<<
				fClusters[iCl].xc<<"; Yc: "<<fClusters[iCl].yc<<"; Charge: "<<
				fClusters[iCl].sumClCharge<<"; nDigis: "<<fClusters[iCl].nofDidis<<
				"\n--errHit_Point: "<<errHit_Point<<
				"\n--errHit_Pixel: "<<errHit_Pixel<<
				"\n--errPoint_Pixel: "<<errPoint_Pixel<<"\n";*/
		/*fprintf(f1, "Cl: %d; Station: %d\n-Xc: %.7f; Yc: %.7f; Charge: %d; nDigis: %d\n--errHit_Point: %.7f\n--errHit_Pixel: %.7f\n--errPoint_Pixel: %.7f\n\n",
				iCl, digi->GetStationNr(), fClusters[iCl].xc, fClusters[iCl].yc,
				fClusters[iCl].sumClCharge, fClusters[iCl].nofDidis,
				errHit_Point, errHit_Pixel, errPoint_Pixel);*/
	}
	//fclose(f1);
	fErrHit_Pixel += eHit_Pixel / (Float_t)fNofClusters;
	fErrHit_Point += eHit_Point / (Float_t)fNofClusters;
	/*std::cout<<"--fErrHit_Point: "<<fErrHit_Point<<"; eHit_Point: "<<eHit_Point<<
			"; fNofClusters: "<<fNofClusters<<"\n";*/
	fErrPoint_Pixel += ePoint_Pixel / (Float_t)fNofClusters;
}
ClassImp(CbmMvdClustering);
