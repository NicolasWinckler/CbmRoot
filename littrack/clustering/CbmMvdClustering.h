/*
 * CbmMvdClustering.h
 *
 *  Created on: Jun 7, 2012
 *      Author: kozlov
 */

#ifndef CBMMVDCLUSTERING_H_
#define CBMMVDCLUSTERING_H_

#include "FairTask.h"
#include "CbmMvdPoint.h"
#include "CbmMvdDigi.h"

#include "TArrayS.h"
#include "TObjArray.h"
#include "TRefArray.h"
#include "TCanvas.h"


#include "TRandom3.h"
#include "TStopwatch.h"
#include "TString.h"
#include "TMath.h"
#include "TH1.h"
#include "TH2.h"
#include "TH1F.h"
#include <vector>
#include <list>
#include <map>
#include <utility>

class TClonesArray;
class TRandom3;
class CbmMvdGeoPar;
class CbmMvdPileupManager;
class CbmMvdStation;

class CbmHistManager;

class TH1F;

class CbmMvdClustering: public FairTask
{
public:
   /**
    * \brief Constructor.
    */
	CbmMvdClustering();

   /**
    * \brief Destructor.
    */
   virtual ~CbmMvdClustering();

   /**
    * \brief Derived from FairTask.
    */
   virtual InitStatus Init();

   /**
    * \brief Derived from FairTask.
    */
   virtual void Exec(Option_t* opt);

   /**
    * \brief Derived from FairTask.
    */
   virtual void Finish();

   void FindClusters();

   void CalculateAcuracy();

   //void FillHistogramsForRadius();

   //void TestStrangeClusters();

   void PrintClusters();


private:

   void ReadDataBranches();

   CbmHistManager* fHM; // Histogram manager

   //
   TClonesArray* fMCTracks;
   //
   TClonesArray* fDigis;
   TClonesArray* fPoints;
   TClonesArray* fHits;
   TClonesArray* fCluster;
   TClonesArray* fMatches;
   map<pair<Int_t, Int_t>, Int_t> fDigiMap;
   map<pair<Int_t, Int_t>, Int_t>::iterator fDigiMapIt;

   map<pair<Int_t, Int_t>, Int_t> fClusterMap;
   map<pair<Int_t, Int_t>, Int_t>::iterator fClusterMapIt;

   map<pair<Int_t, Int_t>, Bool_t> fActiveMap;
   map<pair<Int_t, Int_t>, Bool_t>::iterator fActiveMapIt;

   map<pair<Int_t, Int_t>, Bool_t> fChargeMap1;
   map<pair<Int_t, Int_t>, Bool_t>::iterator fChargeMap1It;

   /*map<pair<Int_t, Int_t>, Int_t> fChargeMap2;
   map<pair<Int_t, Int_t>, Int_t>::iterator fChargeMap2It;*/
   Int_t fNofDigis;
   Int_t fNofPoints;

   Int_t fNofPointsBySt[4];
   Int_t fNofDigisBySt[4];
   Int_t fNofClustersBySt[4];

   Double_t globalCTime;
   Double_t globalRTime;

   static const Int_t nDigiByCl = 150;
   Int_t fNofClusters;
   struct Cluster{
	   Int_t nofCluster;
   	   Float_t xc;
   	   Float_t yc;
   	   Float_t sumClCharge;
   	   Int_t nofDidis;
   	   //Int_t digisInCluster[nDigiByCl];
   	   vector<Int_t> digisInCluster;
   	   Int_t nDigiMax;
   	   Int_t nMCPoint;
   };
   Cluster* fClusters;

   map<Int_t, CbmMvdStation*> fStationMap;

   /*static const Int_t nRadiusSteps = 100;
   TH1F* fhErrPoint_Hit;
   TH1F* fhErrPoint_Pixel;
   TH1F* fhErrHit_Pixel;*/

   TH1F* fhErrorsHit_MCPoint;
   TH1F* fhErrorsHit_Pixel;
   TH1F* fhErrorsPixel_MCPoint;

   TH1F* fhDigisInCluster;
   TH1F* fhMaxDigiCharges;
   TH1F* fhMaxDigiChargesStation1;
   TH1F* fhMaxDigiChargesStation2;

   TH1F* fhResidual_X_St1;
   TH1F* fhResidual_Y_St1;
   TH1F* fhResidual_X_St2;
   TH1F* fhResidual_Y_St2;
   TH1F* fhResidual_X;
   TH1F* fhResidual_Y;
   TH1F* fhPull_X_St1;
   TH1F* fhPull_Y_St1;
   TH1F* fhPull_X_St2;
   TH1F* fhPull_Y_St2;

   Int_t fNofEvents;
   Float_t fEfficiency;
   Float_t fErrHit_Pixel;
   Float_t fErrHit_Point;
   Float_t fErrPoint_Pixel;
   Int_t fLooseCl;
   Int_t fLooseClustersBySt[4];
   Int_t fFakeCl;
   Int_t fFakeClustersBySt[4];

   Int_t GetMvdGeometry();
   void Register();

   void ChangeClusters(pair<Int_t, Int_t> h, Int_t nCl0, Int_t nCl1);

   CbmMvdClustering(const CbmMvdClustering&);
   CbmMvdClustering& operator=(const CbmMvdClustering&);

   ClassDef(CbmMvdClustering, 1);
};

#endif /* CBMMVDCLUSTERING_H_ */
