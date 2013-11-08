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
//
#include "CbmMCTrack.h"
//

#include "TRandom3.h"
#include "TString.h"
#include "TVector3.h"
#include "TMath.h"
#include "TH1.h"
#include "TH2.h"
#include "TH1F.h"
#include "TF1.h"

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
      fHM(NULL),
      fhResidual_X_St1(NULL),
      fhResidual_Y_St1(NULL),
      fhResidual_X_St2(NULL),
      fhResidual_Y_St2(NULL),
      fhPull_X_St1(NULL),
      fhPull_Y_St1(NULL),
      fhPull_X_St2(NULL),
      fhPull_Y_St2(NULL),
      fhResidual_X(NULL),
      fhResidual_Y(NULL),
		fhErrorsHit_MCPoint(NULL),
		fhErrorsHit_Pixel(NULL),
		fhErrorsPixel_MCPoint(NULL),
		fhDigisInCluster(NULL),
		fhMaxDigiCharges(NULL),
		fhMaxDigiChargesStation1(NULL),
		fhMaxDigiChargesStation2(NULL),
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
//	fNofDigisBySt[0] = 0;
//	fNofDigisBySt[1] = 0;
//	fNofPointsBySt[0] = 0;
//	fNofPointsBySt[1] = 0;
	fEfficiency = 0;
	fNofEvents = 0;
	fLooseCl = 0;
	fFakeCl = 0;
//	fNofClustersBySt[0] = 0;
//	fNofClustersBySt[1] = 0;
//	fLooseClustersBySt[0] = 0;
//	fLooseClustersBySt[1] = 0;
//	fFakeClustersBySt[0] = 0;
//	fFakeClustersBySt[1] = 0;
	for(Int_t i = 0; i < 4; i++){
	   fNofDigisBySt[i] = 0;
	   fNofPointsBySt[i] = 0;
	   fNofClustersBySt[i] = 0;
	   fLooseClustersBySt[i] = 0;
	   fFakeClustersBySt[i] = 0;
	}
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
	if ( fClusters ) {
	    delete [] fClusters;
	    }
	if ( fMatches ) {
	fMatches->Delete();
	delete fMatches;
	}
	if (fHM) delete fHM;
}

InitStatus CbmMvdClustering::Init()
{
   std::cout << "CbmMvdClustering::Init" << std::endl;

//   fHM = new CbmHistManager();

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

   fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
//----------------------------------------------------------------------------------------------->>>
   /*fhErrorsHit_MCPoint = new TH1F("fhErrorsHit_MCPoint", "hErrorsHit_MCPoint;Error(cm);nofHits", 100, 0, 0.002);
   fhErrorsHit_Pixel = new TH1F("fhErrorsHit_Pixel", "hErrorsHit_Pixel;Error(cm);nofHits", 100, 0, 0.002);
   fhErrorsPixel_MCPoint = new TH1F("fhErrorsPixel_MCPoint", "hErrorsPixel_MCPoint;Error(cm);nofHits", 100, 0, 0.002);

   fhDigisInCluster = new TH1F("fhDigisInCluster", "hDigisInCluster;nofDigis;nofClusters", 100, 0, 100);
   fhMaxDigiCharges = new TH1F("fhMaxDigiCharges", "hMaxDigiCharges;nofDigis;Charge", 100, 0, 10000);
   fhMaxDigiChargesStation1 = new TH1F("fhMaxDigiChargesStation1", "hMaxDigiChargesStation1;nofDigis;Charge", 100, 0, 10000);
   fhMaxDigiChargesStation2 = new TH1F("fhMaxDigiChargesStation2", "hMaxDigiChargesStation2;nofDigis;Charge", 100, 0, 10000);

   fhResidual_X_St1 = new TH1F("fhResidual_X_St1", "fhResidual_X_St1;Residual[cm];Counter", 100, -0.0015, 0.0015);
   fhResidual_Y_St1 = new TH1F("fhResidual_Y_St1", "fhResidual_Y_St1;Residual[cm];Counter", 100, -0.0015, 0.0015);
   fhResidual_X_St2 = new TH1F("fhResidual_X_St2", "fhResidual_X_St2;Residual[cm];Counter", 100, -0.0015, 0.0015);
   fhResidual_Y_St2 = new TH1F("fhResidual_Y_St2", "fhResidual_Y_St2;Residual[cm];Counter", 100, -0.0015, 0.0015);
   fhResidual_X = new TH1F("fhResidual_X", "fhResidual_X;Residual[cm];Counter", 100, -0.0015, 0.0015);
   fhResidual_Y = new TH1F("fhResidual_Y", "fhResidual_Y;Residual[cm];Counter", 100, -0.0015, 0.0015);
   fhPull_X_St1 = new TH1F("fhPull_X_St1", "fhPull_X_St1;Pull[cm];Counter", 100, -0.0015, 0.0015);
   fhPull_Y_St1 = new TH1F("fhPull_Y_St1", "fhPull_Y_St1;Pull[cm];Counter", 100, -0.0015, 0.0015);
   fhPull_X_St2 = new TH1F("fhPull_X_St2", "fhPull_X_St2;Pull[cm];Counter", 100, -0.0015, 0.0015);
   fhPull_Y_St2 = new TH1F("fhPull_Y_St2", "fhPull_Y_St2;Pull[cm];Counter", 100, -0.0015, 0.0015);*/
//---------------------------------------------------------------------------------------------------
   /*TH1F* h = new TH1F("fhResidual_X_St1", "fhResidual_X_St1;Residual;Counter", 100, -0.0015, 0.0015);
   fHM->Add("fhResidual_X_St1", h);
   delete h;
   TH1F* h = new TH1F("fhResidual_Y_St1", "fhResidual_Y_St1;Residual;Counter", 100, -0.0015, 0.0015);
   TH1F* h = new TH1F("fhResidual_X_St2", "fhResidual_X_St2;Residual;Counter", 100, -0.0015, 0.0015);
   TH1F* h = new TH1F("fhResidual_Y_St2", "fhResidual_Y_St2;Residual;Counter", 100, -0.0015, 0.0015);
   TH1F* h = new TH1F("fhPull_X_St1", "fhPull_X_St1;Pull;Counter", 100, -0.0015, 0.0015);
   TH1F* h = new TH1F("fhPull_Y_St1", "fhPull_Y_St1;Pull;Counter", 100, -0.0015, 0.0015);
   TH1F* h = new TH1F("fhPull_X_St2", "fhPull_X_St2;Pull;Counter", 100, -0.0015, 0.0015);
   TH1F* h = new TH1F("fhPull_Y_St2", "fhPull_Y_St2;Pull;Counter", 100, -0.0015, 0.0015);*/

   //TString fname = "/u/gkozlov/cbm/events/cc.25gev.centr_draw10.txt";
   //TString fname = "/u/gkozlov/cbm/events/pc.25gev.centr_draw10.txt";
   //TString fname = "/u/gkozlov/cbm/events/pAu.25gev.centr_draw10.txt";
   /*TString fname = "/u/gkozlov/cbm/events/mvd.clustering.out.txt";
   FILE* f1 = fopen(fname , "w");
   fprintf(f1, "Output file:\n\n");
   fclose(f1);*/
   return kSUCCESS;
}

void CbmMvdClustering::Exec(Option_t* opt)
{
	static Int_t eventNo = 0;
	std::cout << "\n---<>---\nCbmMvdClustering::Exec: event No=" << eventNo++ << std::endl;
	fNofEvents++;

	fHits->Clear();
	fCluster->Clear();
	fMatches->Clear();

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
	fNofDigisBySt[2] = 0;
	fNofDigisBySt[3] = 0;
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
	    if((digi->GetStationNr() > 0) && (digi->GetStationNr() < 5)){
           fNofDigisBySt[digi->GetStationNr() - 1]++;
        }
	    /*if(digi->GetStationNr() == 1)
	    {
	    	fNofDigisBySt[0]++;
	    }
	    if(digi->GetStationNr() == 2)
	    {
	    	fNofDigisBySt[1]++;
	    }*/
	}
	//}
	fNofPoints = fPoints->GetEntriesFast();
	fNofPointsBySt[0] = 0;
	fNofPointsBySt[1] = 0;
	fNofPointsBySt[2] = 0;
	fNofPointsBySt[3] = 0;
	fNofClustersBySt[0] = 0;
	fNofClustersBySt[1] = 0;
	fNofClustersBySt[2] = 0;
	fNofClustersBySt[3] = 0;
	fNofClusters = 0;
	if(fClusters)
	{
		delete [] fClusters;
	}
	for(Int_t iPoint = 0; iPoint < fNofPoints; iPoint++)
	{
		CbmMvdPoint* point = (CbmMvdPoint*) fPoints->At(iPoint);
		if((point->GetStationNr() > 0) && (point->GetStationNr() < 5)){
           fNofPointsBySt[point->GetStationNr() - 1]++;
        }
		/*if(point->GetStationNr() == 1)
		{
			fNofPointsBySt[0]++;
		}
		if(point->GetStationNr() == 2)
		{
			fNofPointsBySt[1]++;
		}*/
	}

	//std::cout<<"Digis: "<<fNofDigis<<"\n";
	//std::cout<<"Points: "<<fNofPoints<<"\n";
	TStopwatch timer;
    timer.Start();
	FindClusters();
	timer.Stop();
    globalCTime += timer.CpuTime();
    globalRTime += timer.RealTime();
    std::cout<<"---FindClusters Finished\n";
//	CalculateAcuracy();
//	std::cout<<"---CalculateAcuracy Finished\n";
	//FillHistogramsForRadius();

	//TestStrangeClusters();
//	PrintClusters();
//	std::cout<<"---PrintClusters Finished\n";

}

void CbmMvdClustering::Finish()
{
   std::cout << "CbmMvdClustering::Finish" << std::endl;
//--------------------------------------------------------------------------------------------->>>
   /*fEfficiency = 100 * fEfficiency / (Float_t)fNofEvents;
   std::cout<<"Efficiency: "<<fEfficiency<<"\n";
   fErrHit_Pixel = fErrHit_Pixel / (Float_t)fNofEvents;
   fErrHit_Point = fErrHit_Point / (Float_t)fNofEvents;

   globalCTime = globalCTime /  (Double_t)fNofEvents;
   globalRTime = globalRTime /  (Double_t)fNofEvents;
   //std::cout<<"---fErrHit_Point: "<<fErrHit_Point<<"; fNofEvents: "<<fNofEvents<<"\n";
   fErrPoint_Pixel = fErrPoint_Pixel / (Float_t)fNofEvents;
   std::cout<<"Loosing Clusters: "<<fLooseCl<<"; St1: "<<fLooseClustersBySt[0]<<
		   "; St2: "<<fLooseClustersBySt[1]<<"; St3: "<<fLooseClustersBySt[2]<<
		   "; St4: "<<fLooseClustersBySt[3]<<"\nFake Clusters: "<<fFakeCl<<
		   "; St1: "<<fFakeClustersBySt[0]<<"; St2: "<<fFakeClustersBySt[1]<<
		   "; St3: "<<fFakeClustersBySt[2]<<"; St4: "<<fFakeClustersBySt[3]<<"\n";*/
//----------------------------------------------------------------------------------------------
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
//----------------------------------------------------------------------------------------------------->>>
   /*SetDefaultDrawStyle();

   fhErrorsHit_MCPoint->Scale(1. / fNofEvents);
   fhErrorsHit_MCPoint->Write();
   TCanvas* canvasHMP = new TCanvas("fhErrorsHit_MCPoint", "fhErrorsHit_MCPoint", 1000, 500);
   //canvasHMP->Divide(1, 1);
   //canvasHMP->cd(1);
   //fhErrorsHit_MCPoint->Draw();
   DrawH1(fhErrorsHit_MCPoint);//, kLinear, kLog);

   fhDigisInCluster->Scale(1. / fNofEvents);
   fhDigisInCluster->Write();
   TCanvas* canvasDC = new TCanvas("fhDigisInCluster", "fhDigisInCluster", 1000, 500);
   DrawH1(fhDigisInCluster);

   fhMaxDigiCharges->Scale(1. / fNofEvents);
   fhMaxDigiCharges->Write();
   TCanvas* canvasMDC = new TCanvas("fhMaxDigiCharges", "fhMaxDigiCharges", 1000, 500);
   DrawH1(fhMaxDigiCharges, kLog, kLog);

   fhMaxDigiChargesStation1->Scale(1. / fNofEvents);
   fhMaxDigiChargesStation2->Scale(1. / fNofEvents);
   fhMaxDigiChargesStation1->Write();
   fhMaxDigiChargesStation2->Write();
   TCanvas* canvasMDCS = new TCanvas("fhMaxDigiChargesStations", "fhMaxDigiChargesStations", 1000, 1000);
   canvasMDCS->Divide(1, 2);
   canvasMDCS->cd(1);
   DrawH1(fhMaxDigiChargesStation1, kLog, kLog);
   canvasMDCS->cd(2);
   DrawH1(fhMaxDigiChargesStation2, kLog, kLog);*/
//---------------------------------------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------------------>>>
   /*fhResidual_X_St1->Fit("gaus");
   fhResidual_Y_St1->Fit("gaus");
   fhResidual_X_St2->Fit("gaus");
   fhResidual_Y_St2->Fit("gaus");
   fhResidual_X->Fit("gaus");
   fhResidual_Y->Fit("gaus");
   Double_t vol = 0;
   vol = fhResidual_X_St1->GetRMS() * 100000;
   std::cout<<"Res X St 1 RMS: "<<vol<<"\n";
   vol = fhResidual_X_St2->GetRMS() * 100000;
   std::cout<<"Res X St 2 RMS: "<<vol<<"\n";
   vol = fhResidual_Y_St1->GetRMS() * 100000;
   std::cout<<"Res Y St 1 RMS: "<<vol<<"\n";
   vol = fhResidual_Y_St2->GetRMS() * 100000;
   std::cout<<"Res Y St 2 RMS: "<<vol<<"\n";
   vol = fhResidual_X->GetRMS() * 100000;
   std::cout<<"Res X RMS: "<<vol<<"\n";
   vol = fhResidual_Y->GetRMS() * 100000;
   std::cout<<"Res Y RMS: "<<vol<<"\n";
   vol = fhResidual_X_St1->GetMean() * 100000;
   std::cout<<"Res X St 1 Mean: "<<vol<<"\n";
   vol = fhResidual_X_St2->GetMean() * 100000;
   std::cout<<"Res X St 2 Mean: "<<vol<<"\n";
   vol = fhResidual_Y_St1->GetMean() * 100000;
   std::cout<<"Res Y St 1 Mean: "<<vol<<"\n";
   vol = fhResidual_Y_St2->GetMean() * 100000;
   std::cout<<"Res Y St 2 Mean: "<<vol<<"\n";
   vol = fhResidual_X->GetMean() * 100000;
   std::cout<<"Res X Mean: "<<vol<<"\n";
   vol = fhResidual_Y->GetMean() * 100000;
   std::cout<<"Res Y Mean: "<<vol<<"\n";
   TF1* fitX1 = fhResidual_X_St1->GetFunction("gaus");
   vol = fitX1->GetParameter(2) * 100000;
   std::cout<<"Res X St 1 Sigma: "<<vol<<"\n";
   TF1* fitX2 = fhResidual_X_St2->GetFunction("gaus");
   vol = fitX2->GetParameter(2) * 100000;
   std::cout<<"Res X St 2 Sigma: "<<vol<<"\n";
   TF1* fitY1 = fhResidual_Y_St1->GetFunction("gaus");
   vol = fitY1->GetParameter(2) * 100000;
   std::cout<<"Res Y St 1 Sigma: "<<vol<<"\n";
   TF1* fitY2 = fhResidual_Y_St2->GetFunction("gaus");
   vol = fitY2->GetParameter(2) * 100000;
   std::cout<<"Res Y St 2 Sigma: "<<vol<<"\n";
   TF1* fitX = fhResidual_X->GetFunction("gaus");
   vol = fitX->GetParameter(2) * 100000;
   std::cout<<"Res X Sigma: "<<vol<<"\n";
   TF1* fitY = fhResidual_Y->GetFunction("gaus");
   vol = fitY->GetParameter(2) * 100000;
   std::cout<<"Res Y Sigma: "<<vol<<"\n";
   fhResidual_X_St1->Write();
   fhResidual_Y_St1->Write();
   fhResidual_X_St2->Write();
   fhResidual_Y_St2->Write();
   fhResidual_X->Write();
   fhResidual_Y->Write();
   fhPull_X_St1->Write();
   fhPull_Y_St1->Write();
   fhPull_X_St2->Write();
   fhPull_Y_St2->Write();
   TCanvas* canvasRes = new TCanvas("fhResidual", "fhResidual", 1000, 1000);
   canvasRes->Divide(2, 2);
   canvasRes->cd(1);
   DrawH1(fhResidual_X_St1, kLinear, kLog);
   canvasRes->cd(2);
   DrawH1(fhResidual_Y_St1, kLinear, kLog);
   canvasRes->cd(3);
   DrawH1(fhResidual_X_St2, kLinear, kLog);
   canvasRes->cd(4);
   DrawH1(fhResidual_Y_St2, kLinear, kLog);
   TCanvas* canvasResX = new TCanvas("fhResidual_X", "fhResidual_X", 600, 600);
   DrawH1(fhResidual_X, kLinear, kLog);
   TCanvas* canvasResY = new TCanvas("fhResidual_Y", "fhResidual_Y", 600, 600);
   DrawH1(fhResidual_Y, kLinear, kLog);*/
//--------------------------------------------------------------------------------------------
   /*std::cout<<"Res X St 1 RMS: "<<fhResidual_X_St1->GetRMS()<<"\n";
   std::cout<<"Res X St 2 RMS: "<<fhResidual_X_St2->GetRMS()<<"\n";
   std::cout<<"Res Y St 1 RMS: "<<fhResidual_Y_St1->GetRMS()<<"\n";
   std::cout<<"Res Y St 2 RMS: "<<fhResidual_Y_St2->GetRMS()<<"\n";
   std::cout<<"Res X RMS: "<<fhResidual_X->GetRMS()<<"\n";
   std::cout<<"Res Y RMS: "<<fhResidual_Y->GetRMS()<<"\n";
   std::cout<<"Res X St 1 Mean: "<<fhResidual_X_St1->GetMean()<<"\n";
   std::cout<<"Res X St 2 Mean: "<<fhResidual_X_St2->GetMean()<<"\n";
   std::cout<<"Res Y St 1 Mean: "<<fhResidual_Y_St1->GetMean()<<"\n";
   std::cout<<"Res Y St 2 Mean: "<<fhResidual_Y_St2->GetMean()<<"\n";
   std::cout<<"Res X Mean: "<<fhResidual_X->GetMean()<<"\n";
   std::cout<<"Res Y Mean: "<<fhResidual_Y->GetMean()<<"\n";
   TF1* fitX1 = fhResidual_X_St1->GetFunction("gaus");
   std::cout<<"Res X St 1 Sigma: "<<fitX1->GetParameter(2)<<"\n";
   TF1* fitX2 = fhResidual_X_St2->GetFunction("gaus");
   std::cout<<"Res X St 2 Sigma: "<<fitX2->GetParameter(2)<<"\n";
   TF1* fitY1 = fhResidual_Y_St1->GetFunction("gaus");
   std::cout<<"Res Y St 1 Sigma: "<<fitY1->GetParameter(2)<<"\n";
   TF1* fitY2 = fhResidual_Y_St2->GetFunction("gaus");
   std::cout<<"Res Y St 2 Sigma: "<<fitY2->GetParameter(2)<<"\n";
   TF1* fitX = fhResidual_X->GetFunction("gaus");
   std::cout<<"Res X Sigma: "<<fitX->GetParameter(2)<<"\n";
   TF1* fitY = fhResidual_Y->GetFunction("gaus");
   std::cout<<"Res Y Sigma: "<<fitY->GetParameter(2)<<"\n";*/
   /*TCanvas* canvasPull = new TCanvas("fhPull", "fhPull", 1000, 1000);
   canvasPull->Divide(2, 2);
   canvasPull->cd(1);
   DrawH1(fhPull_X_St1, kLinear, kLog);
   canvasPull->cd(2);
   DrawH1(fhPull_Y_St1, kLinear, kLog);
   canvasPull->cd(3);
   DrawH1(fhPull_X_St2, kLinear, kLog);
   canvasPull->cd(4);
   DrawH1(fhPull_Y_St2, kLinear, kLog);*/
   std::cout<<"---Mean operating time---\n";
   printf("RealTime=%f seconds, CpuTime=%f seconds\n", globalRTime, globalCTime);
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

	Int_t nHitsT = 0;
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
//std::cout<<"Ev "<<fNofEvents<<" - "<<"At 1\n";
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
//std::cout<<"Ev "<<fNofEvents<<" - "<<"At 2\n";
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
//std::cout<<"Ev "<<fNofEvents<<" - "<<"At 3\n";
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
//std::cout<<"Ev "<<fNofEvents<<" - "<<"At 4\n";
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
//std::cout<<"Ev "<<fNofEvents<<" - "<<"At 5\n";
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
	         std::cout<<"FindClusters Finish Clustering Step\n";
	std::cout<<"Points: "<<fNofPoints<<"; P1: "<<fNofPointsBySt[0]<<"; P2:"<<fNofPointsBySt[1]<<
	      "; P3:"<<fNofPointsBySt[2]<<"; P4:"<<fNofPointsBySt[3]<<"\n";
	std::cout<<"Digis: "<<fNofDigis<<"; D1: "<<fNofDigisBySt[0]<<"; D2:"<<fNofDigisBySt[1]<<
	      "; D3:"<<fNofDigisBySt[2]<<"; D4:"<<fNofDigisBySt[3]<<"\n";
	//std::cout<<"Clusters: "<<nofClusters<<"\n";
	//std::cout<<"nCl: "<<nCl<<"\n";
				//nofClusters = nCl;	//10000 - OK ???

	/*Int_t cTest = 0;
	vector<UInt_t> ch0;
	vector<Int_t> x1;
	vector<Int_t> y1;
	ch0.clear(); x1.clear(); y1.clear();
	for(Int_t iDigi = 0; iDigi < fNofDigis; iDigi++){
		CbmMvdDigi* digi = (CbmMvdDigi*) fDigis->At(iDigi);
//std::cout<<"Ev "<<fNofEvents<<" - "<<"At 6\n";
		pair<Int_t, Int_t> n(digi->GetPixelX(), digi->GetPixelY());
		if(fChargeMap1[n] == 1){
//			x1[cTest] = n.first;
//			y1[cTest] = n.second;
			x1.push_back(n.first);
			y1.push_back(n.second);
			for(Int_t i = 0; i < cTest; i++){
				if((x1[i] == x1[cTest]) && (y1[i] == y1[cTest])){
					//std::cout<<"->i = "<<i<<"; cTest = "<<cTest<<"\n";
					CbmMvdDigi* digi1 = (CbmMvdDigi*) fDigis->At(ch0[i]);
//std::cout<<"Ev "<<fNofEvents<<" - "<<"At 7\n";
					//std::cout<<"---Ch2: "<<digi->GetCharge()<<"; St: "<<digi->GetStationNr()<<"\n";
					//std::cout<<">DigiMap: "<<fDigiMap[n]<<"\n";
					if(digi1->GetStationNr() > digi->GetStationNr()){
						n = std::make_pair(digi1->GetPixelX(), digi1->GetPixelY());
					}
					//std::cout<<"---Ch1: "<<digi1->GetCharge()<<"; St: "<<digi1->GetStationNr()<<"\n";
					//std::cout<<">DigiMap: "<<fDigiMap[n]<<"\n";
					fChargeMap1[n] = 0;
					fClusterMap[n] = 0;
					fActiveMap[n] = 0;
				}
			}
			//ch0[cTest] = iDigi;
			ch0.push_back(iDigi);
			cTest++;
			//std::cout<<"cTest: "<<cTest<<"; x: "<<n.first<<"; y: "<<n.second<<"\n";
		}
	}
	ch0.clear();
	x1.clear();
	y1.clear();*/

    //std::cout<<"FindClusters Charge Map - ?\n";
    std::cout<<"fNofClusters: "<<fNofClusters<<"; nofClusters: "<<nofClusters<<"\n";

	fClusters = new Cluster[nofClusters];
	fNofClusters = nofClusters;
	for(Int_t iCl = 0; iCl < nofClusters; iCl++)
	{
		fClusters[iCl].nofCluster = 0;
		fClusters[iCl].nofDidis = 0;
		fClusters[iCl].sumClCharge = 0;
		fClusters[iCl].xc = 0;
		fClusters[iCl].yc = 0;
		fClusters[iCl].digisInCluster.clear();
	}
	//---New variant of cluster recalculation---
	map<Int_t, Int_t> clusterNumbers;
	vector<Int_t> usedNumbers;
	Int_t nomCl = 0;
	for(Int_t iDigi = 0; iDigi < fNofDigis; iDigi++){
	   digi = (CbmMvdDigi*) fDigis->At(iDigi);
	   Int_t stNr = digi->GetStationNr();
	   channelX = digi->GetPixelX();
	   channelY = digi->GetPixelY();
	   pair<Int_t, Int_t> a(channelX, channelY);
	   if((fClusterMap[a] != 0) && (digi->GetCharge() > 0)){
	      CbmMvdStation* station= fStationMap[digi->GetStationNr()];
          Float_t pixelSizeX  = digi->GetPixelSizeX();
          Float_t pixelSizeY  = digi->GetPixelSizeY();
          Double_t layerRadius = station->GetRmax();
          Double_t Xdigi = (0.5 + (Double_t)channelX) * pixelSizeX - layerRadius;
          Double_t Ydigi = (0.5 + (Double_t)channelY) * pixelSizeY - layerRadius;
          Float_t digiCharge = digi->GetCharge();
          std::sort(usedNumbers.begin(), usedNumbers.end());
	      if(!std::binary_search(usedNumbers.begin(), usedNumbers.end(), fClusterMap[a])){
	         usedNumbers.push_back(fClusterMap[a]);
	         clusterNumbers[fClusterMap[a]] = nomCl;
	         fClusters[nomCl].nofCluster = nomCl + 1;
             fClusters[nomCl].xc += Xdigi * digiCharge;
             fClusters[nomCl].yc += Ydigi * digiCharge;
             fClusters[nomCl].sumClCharge += digiCharge;
             fClusters[nomCl].digisInCluster.push_back(fDigiMap[a]);
             fClusters[nomCl].nofDidis++;
             nomCl++;
	      }
	      else{
	         CbmMvdDigi* firstDigi = (CbmMvdDigi*) fDigis->At(fClusters[clusterNumbers[fClusterMap[a]]].digisInCluster[0]);
	         if(stNr == firstDigi->GetStationNr()){
	         fClusters[clusterNumbers[fClusterMap[a]]].xc += Xdigi * digiCharge;
             fClusters[clusterNumbers[fClusterMap[a]]].yc += Ydigi * digiCharge;
             fClusters[clusterNumbers[fClusterMap[a]]].sumClCharge += digiCharge;
             fClusters[clusterNumbers[fClusterMap[a]]].digisInCluster.push_back(fDigiMap[a]);
             fClusters[clusterNumbers[fClusterMap[a]]].nofDidis++;}
	      }
	      fClusterMap[a] = nomCl;
	      fActiveMap[a] = 0;
	   }
	}
	std::cout<<"nomCl: "<<nomCl<<"\n";
	clusterNumbers.clear();
	usedNumbers.clear();
	//------------------------------------------
/*	Int_t nomCl = 0;
	Int_t Replase = 0;
	for(Int_t iDigi = 0; iDigi < fNofDigis; iDigi++){
		digi = (CbmMvdDigi*) fDigis->At(iDigi);
//std::cout<<"Ev "<<fNofEvents<<" - "<<"At 8\n";
		Int_t stNr = digi->GetStationNr();
		channelX = digi->GetPixelX();
		channelY = digi->GetPixelY();
		//std::cout<<"\n->Digi: "<<iDigi;
		pair<Int_t, Int_t> a(channelX, channelY);
		if((fChargeMap1[a] == 1) && (fClusterMap[a] != 0) && (digi->GetCharge() > 0)){
			//std::cout<<" - good"<<"; nomCl = "<<nomCl;
			Replase = fClusterMap[a];
			Int_t nofDigisInCluster = 0;
			for(Int_t jDigi = 0; jDigi < fNofDigis; jDigi++){
				digiNeighbor = (CbmMvdDigi*) fDigis->At(jDigi);
//std::cout<<"Ev "<<fNofEvents<<" - "<<"At 9\n";
				//if(digiNeighbor->GetCharge() == 0)std::cout<<"WARNING!___";
				if(stNr == digiNeighbor->GetStationNr()){
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
					Double_t Xdigi = (0.5 + (Double_t)chX) * pixelSizeX - layerRadius;
					Double_t Ydigi = (0.5 + (Double_t)chY) * pixelSizeY - layerRadius;
					//std::cout<<"Cluster: "<<nomCl<<"; Digi: "<<jDigi<<"; cX: "<<chX<<"; cY: "<<chY<<"; Xd: "<<Xdigi<<"; Yd: "<<Ydigi<<"\n";
					//UInt_t digiNeighborCharge = (UInt_t)digiNeighbor->GetCharge();
					Float_t digiNeighborCharge = digiNeighbor->GetCharge();
					//---FOR NONE AMPLITUDE TEST---//
					//digiNeighborCharge = 1;
					//---END---//
					fClusters[nomCl].xc += Xdigi * digiNeighborCharge;
					fClusters[nomCl].yc += Ydigi * digiNeighborCharge;
					fClusters[nomCl].sumClCharge += digiNeighborCharge;
					//fClusters[nomCl].digisInCluster[nofDigisInCluster] = fDigiMap[b];
					fClusters[nomCl].digisInCluster.push_back(fDigiMap[b]);
					nofDigisInCluster++;
					fClusters[nomCl].nofDidis = nofDigisInCluster;
				}}
			}
			nomCl++;
//std::cout<<"Ev "<<fNofEvents<<" - "<<"At 10\n";
		}
//		else
//		{
//			digiNeighbor = (CbmMvdDigi*) fDigis->At(iDigi);
//			std::cout<<"fCMap: "<<fClusterMap[a]<<"; fAMap: "<<fActiveMap[a]<<"; Ch: "<<digiNeighbor->GetCharge()<<"\n";
//		}
	}*/
/*	for(Int_t i = 0; i < nomCl; i++){
       std::cout<<">>>-----------------<<<\nCl: "<<i<<"; nCl: "<<fClusters[i].nofCluster<<"\n";
       std::cout<<"X: "<<fClusters[i].xc<<"; Y: "<<fClusters[i].yc<<"; Charge: "<<fClusters[i].sumClCharge<<"\n";
       std::cout<<"nofDigis: "<<fClusters[i].nofDidis<<" - "<<fClusters[i].digisInCluster.size()<<"\n";
    }*/

    std::cout<<"FindClusters Clusters calculated\n";

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
//				for(Int_t iDigi = 0; iDigi < fClusters[jCl+1].nofDidis; iDigi++){
//					fClusters[jCl].digisInCluster[iDigi] = fClusters[jCl+1].digisInCluster[iDigi];
//				}
				fClusters[jCl].digisInCluster = fClusters[jCl+1].digisInCluster;
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

    std::cout<<"FindClusters Fake clusters deleted\nfNofClusters: "<<fNofClusters<<"\n";

	TVector3 pos, dpos;
	 dpos.SetXYZ(0.0002, 0.0002, 0.);
	for(Int_t iCl = 0; iCl < fNofClusters; iCl++)
	{
		CbmMvdDigi* digi = (CbmMvdDigi*) fDigis->At(fClusters[iCl].digisInCluster[0]);
//std::cout<<"Ev "<<fNofEvents<<" - "<<"At 10\n";
		Int_t stNr = digi->GetStationNr();
		CbmMvdStation* station= fStationMap[stNr];

		//---
		CbmMvdDigi* centralDigi;
		Short_t chargeArray[49], dominatorArray[49];
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
			    for(Int_t k = 0; k < fClusters[iCl].digisInCluster.size(); k++)//fClusters[iCl].nofDidis; k++ )
			    {
			    	//std::cout<<"--Step 2\n";
			    	CbmMvdDigi* digi1 = (CbmMvdDigi*) fDigis->At(fClusters[iCl].digisInCluster[k]);
//std::cout<<"Ev "<<fNofEvents<<" - "<<"At 11\n";
//std::cout<<"---i: "<<i<<"; j: "<<j<<"; k: "<<k<<"\n";
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
//std::cout<<"Ev "<<fNofEvents<<" - "<<"At 12\n";
		new ((*fMatches)[nHits]) CbmMvdHitMatch(0, 0, centralDigi->GetTrackID(), centralDigi->GetPointID(), clusterNew->GetContributors());
		//std::cout<<"-Step 10\n";
//		if(fClusters[iCl].nofDidis > 50)
//		{
//			const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(fMCTracks->At(centralDigi->GetTrackID()));
//			Int_t pdgCode = mcTrack->GetPdgCode();
//			std::cout<<"nofD: "<<fClusters[iCl].nofDidis<<"; TID: "<<centralDigi->GetTrackID()<<
//					"; PDG: "<<pdgCode<<"\n";
//		}
		nHitsT++;
	}
    std::cout<<"FindClusters Writing in file finished\nnHitsT: "<<nHitsT<<"\n";
    /*for(Int_t i = 0; i < fNofClusters; i++){
       std::cout<<"iCl: "<<fClusters[i].nofCluster<<"; Charge: "<<fClusters[i].sumClCharge<<"; nDigi: "<<fClusters[i].digisInCluster.size()<<"\n";
    }*/
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
   std::cout.precision(5);
   //---
   for(Int_t iCl = 0; iCl < fNofClusters; iCl++){
      CbmMvdDigi* digi = (CbmMvdDigi*) fDigis->At(fClusters[iCl].digisInCluster[0]);
//      std::cout<<"=>iCl: "<<iCl<<"\n";
//      std::cout<<"=>TID: "<<digi->GetTrackID()<<"\n";
      if(digi->GetTrackID() < 0){
//         std::cout<<"==>Start cluster recombining\n";
         for(Int_t jCl = iCl; jCl < (fNofClusters - 1); jCl++){
            fClusters[jCl].nDigiMax = fClusters[jCl + 1].nDigiMax;
            fClusters[jCl].nMCPoint = fClusters[jCl + 1].nMCPoint;
            fClusters[jCl].nofCluster = fClusters[jCl + 1].nofCluster;
            fClusters[jCl].nofDidis = fClusters[jCl + 1].nofDidis;
            fClusters[jCl].sumClCharge = fClusters[jCl + 1].sumClCharge;
            fClusters[jCl].xc = fClusters[jCl + 1].xc;
            fClusters[jCl].yc = fClusters[jCl + 1].yc;
            fClusters[jCl].digisInCluster = fClusters[jCl + 1].digisInCluster;
         }
         Int_t stNr = digi->GetStationNr();
         fNofClusters--;
         iCl--;
//         std::cout<<"==>New fNofClusters: "<<fNofClusters<<"\n";
      }
   }
	Float_t totalError = 0;
	Float_t* errorX = new Float_t[fNofClusters];
	Float_t* errorY = new Float_t[fNofClusters];
	Float_t* errorXY = new Float_t[fNofClusters];

	Float_t* errorXYp = new Float_t[fNofClusters];

	Int_t* Points = new Int_t[fNofClusters];

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
		for(Int_t iDigi = 0; iDigi < fClusters[iCl].digisInCluster.size(); iDigi++)//fClusters[iCl].nofDidis; iDigi++)
		{
			CbmMvdDigi* digi = (CbmMvdDigi*) fDigis->At(fClusters[iCl].digisInCluster[iDigi]);
			if(digi->GetCharge() > nDigiCharge)
			{
				nDigiCharge = digi->GetCharge();
				fClusters[iCl].nDigiMax = fClusters[iCl].digisInCluster[iDigi];
			}
		}
		CbmMvdDigi* digi = (CbmMvdDigi*) fDigis->At(fClusters[iCl].nDigiMax);
		if(digi->GetTrackID() < 0)continue;
		Int_t stNr = digi->GetStationNr();
		fNofClustersBySt[stNr - 1]++;
		for(Int_t iPoint = 0; iPoint < fNofPoints; iPoint++)
		{
			CbmMvdPoint* point = (CbmMvdPoint*) fPoints->At(iPoint);
			if(stNr == point->GetStationNr())
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
		//if(errorXY[iCl] > 0.002)std::cout<<"\nBIG ERROR!\nerr = "<<errorXY[iCl]<<"\nCl: "<<iCl<<"; Point: "<<nPoint<<"\n";
		if(errorXY[iCl] < dMin)dMin = errorXY[iCl];
		if(errorXY[iCl] > dMax)dMax = errorXY[iCl];
		fClusters[iCl].nMCPoint = nPoint;
		/*for(Int_t jCl = 0; jCl < iCl; jCl++){
           if(fClusters[iCl].nMCPoint == fClusters[jCl].nMCPoint){
              std::cout<<">>>WARNING!\n"<<"Point: "<<fClusters[iCl].nMCPoint<<"\n";
              std::cout<<">Cl_i: "<<iCl<<"\n";
              std::cout<<">Cl_j: "<<jCl<<"\n";
           }
        }*/
//		std::cout<<"Cl: "<<iCl<<"; X: "<<fClusters[iCl].xc<<"; Y: "<<
//				fClusters[iCl].yc<<"; Charge: "<<fClusters[iCl].sumClCharge<<
//				"; nDigi: "<<fClusters[iCl].nofDidis<<"\n";
//		std::cout<<"Point: "<<nPoint<<"; Xp: "<<xx<<"; Yp: "<<yy<<"; Error: "<<
//				distMin<<"\n";
//		for(Int_t i = 0; i < fClusters[iCl].nofDidis; i++)
//		{
//			CbmMvdDigi* xdigi = (CbmMvdDigi*) fDigis->At(fClusters[iCl].digisInCluster[i]);
//			std::cout<<"---Digi: "<<fClusters[iCl].digisInCluster[i]<<
//					"; X: "<<xdigi->GetPixelX()<<"; Y: "<<xdigi->GetPixelY()<<
//					"; Charge: "<<(UInt_t)xdigi->GetCharge()<<"\n";
//		}
	}

	std::cout<<"Clusters: "<<fNofClusters<<"; St1: "<<fNofClustersBySt[0]<<
			"; St2: "<<fNofClustersBySt[1]<<"; St3: "<<fNofClustersBySt[2]<<
			"; St4: "<<fNofClustersBySt[3]<<"\n";

	//Float_t clcl = 0;
	Float_t fakeCl[4] = {0, 0, 0, 0};
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
	fFakeClustersBySt[2] += fakeCl[2] / 2;
	fFakeClustersBySt[3] += fakeCl[3] / 2;
	Float_t nP = fNofPoints;
	for(Int_t i = 0; i < 4; i++){
	   if((fNofDigisBySt[i] == 0) && (fNofPointsBySt[i] != 0))
	    {
	        nP -= fNofPointsBySt[i];
	    }
	}
	/*if((fNofDigisBySt[0] == 0) && (fNofPointsBySt[0] != 0))
	{
		nP -= fNofPointsBySt[0];
	}
	if((fNofDigisBySt[1] == 0) && (fNofPointsBySt[1] != 0))
	{
		nP -= fNofPointsBySt[1];
	}*/
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
	for(Int_t iSt = 0; iSt < 4; iSt++)
	{
		if(fNofPointsBySt[iSt] > fNofClustersBySt[iSt])
		{
			fLooseClustersBySt[iSt] += fNofPointsBySt[iSt] + (Int_t)(fakeCl[iSt] / 2) - fNofClustersBySt[iSt];
		}
	}
	fFakeCl = fFakeClustersBySt[0] + fFakeClustersBySt[1] + fFakeClustersBySt[2] + fFakeClustersBySt[3];
	fLooseCl = fLooseClustersBySt[0] + fLooseClustersBySt[1] + fLooseClustersBySt[2] + fLooseClustersBySt[3];
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

	delete [] errorX;
	delete [] errorY;
	delete [] errorXY;
	delete [] errorXYp;
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
		if(digi->GetTrackID() < 0)continue;
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
		Float_t eX = fClusters[iCl].xc - xP;
		Float_t eY = fClusters[iCl].yc - yP;
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

		fhDigisInCluster->Fill(fClusters[iCl].nofDidis);
		fhMaxDigiCharges->Fill(digi->GetCharge());
		if(digi->GetStationNr() == 1)
		{
			fhMaxDigiChargesStation1->Fill(digi->GetCharge());
			fhResidual_X_St1->Fill(eX);
			fhResidual_Y_St1->Fill(eY);
		}
		if(digi->GetStationNr() == 2)
		{
			fhMaxDigiChargesStation2->Fill(digi->GetCharge());
			fhResidual_X_St2->Fill(eX);
			fhResidual_Y_St2->Fill(eY);
		}
		fhResidual_X->Fill(eX);
		fhResidual_Y->Fill(eY);
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
