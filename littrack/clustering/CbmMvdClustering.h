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

   TClonesArray* fDigis;
   TClonesArray* fPoints;
   TClonesArray* fHits;
   map<pair<Int_t, Int_t>, Int_t> fDigiMap;
   map<pair<Int_t, Int_t>, Int_t>::iterator fDigiMapIt;

   map<pair<Int_t, Int_t>, Int_t> fClusterMap;
   map<pair<Int_t, Int_t>, Int_t>::iterator fClusterMapIt;

   map<pair<Int_t, Int_t>, Bool_t> fActiveMap;
   map<pair<Int_t, Int_t>, Bool_t>::iterator fActiveMapIt;

   map<pair<Int_t, Int_t>, Int_t> fChargeMap1;
   map<pair<Int_t, Int_t>, Int_t>::iterator fChargeMap1It;

   map<pair<Int_t, Int_t>, Int_t> fChargeMap2;
   map<pair<Int_t, Int_t>, Int_t>::iterator fChargeMap2It;
   Int_t fNofDigis;
   Int_t fNofPoints;

   Int_t fNofPointsBySt[2];
   Int_t fNofDigisBySt[2];
   Int_t fNofClustersBySt[2];

   static const Int_t nDigiByCl = 100;
   Int_t fNofClusters;
   struct Cluster{
	   Int_t nofCluster;
   	   Float_t xc;
   	   Float_t yc;
   	   UInt_t sumClCharge;
   	   Int_t nofDidis;
   	   Int_t digisInCluster[nDigiByCl];
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

   Int_t fNofEvents;
   Float_t fEfficiency;
   Float_t fErrHit_Pixel;
   Float_t fErrHit_Point;
   Float_t fErrPoint_Pixel;
   Int_t fLooseCl;
   Int_t fLooseClustersBySt[2];
   Int_t fFakeCl;
   Int_t fFakeClustersBySt[2];

   Int_t GetMvdGeometry();
   void Register();

   void ChangeClusters(pair<Int_t, Int_t> h, Int_t nCl0, Int_t nCl1);

   ClassDef(CbmMvdClustering, 1);
};

#endif /* CBMMVDCLUSTERING_H_ */