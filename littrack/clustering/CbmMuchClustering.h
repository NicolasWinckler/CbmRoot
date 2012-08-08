/*
 * CbmMuchClustering.h
 *
 *  Created on: Mar 5, 2012
 *      Author: kozlov
 */

#ifndef CBMMUCHCLUSTERING_H_
#define CBMMUCHCLUSTERING_H_

#include "FairTask.h"
#include "CbmMuchGeoCl.h"
#include "CbmClusteringA1.h"
#include "CbmClusteringSL.h"
#include "CbmClusteringWard.h"
#include "CbmMuchTest.h"
#include "CbmMuchGeoScheme.h"
#include "CbmMuchModuleGem.h"
//#include "TClonesArray.h"

class TClonesArray;

class CbmMuchClustering: public FairTask
{
public:
   /**
    * \brief Constructor.
    */
	CbmMuchClustering();

   /**
    * \brief Destructor.
    */
   virtual ~CbmMuchClustering();

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

   void SetAlgorithmVersion(Int_t AlgorithmVersion) { fAlgorithmVersion = AlgorithmVersion;}

   void SetPadsCharge(CbmMuchGeoCl* moduleGeo, CbmMuchGeoScheme* geoScheme);//Int_t nStation, Int_t nLayer, Bool_t nSide, Int_t nModule);

   void DeletePadsCharge(CbmMuchGeoCl* moduleGeo);

   void MuchClustering(CbmMuchGeoCl* moduleGeo, Int_t algVersion, CbmMuchGeoScheme* scheme);

private:

   void ReadDataBranches();

   Int_t fNofPads;
   Int_t fNofPadsInModule;
   Int_t fNofActivePads;
   Int_t fAlgorithmVersion;

   Int_t fTotalClusters;
   //Float_t* fPadsCharges;
   CbmMuchGeoCl* fModuleGeo;
   CbmClusteringA1* fClustersA1;
   CbmClusteringSL* fClustersSL;
   CbmClusteringWard* fClustersWard;

   CbmMuchGeoScheme* fScheme;

   CbmMuchTest* fTestData;

   TClonesArray* fMuchDigi;
   TClonesArray* fMuchPad;
   TClonesArray* fMuchPoint;

   TClonesArray* fClusters;

   TClonesArray* fCluster;
   TClonesArray* fHit;
   TClonesArray* fStrawHit;

   void ClusteringA1(CbmMuchGeoCl* m1, CbmMuchModuleGem* m2, Int_t Ver, Int_t &k, Int_t &nHit, Int_t &nCluster, Int_t &nClusters);
   void ClusteringSL(CbmMuchGeoCl* m1, CbmMuchModuleGem* m2, Int_t Ver, Int_t &k, Int_t &nHit, Int_t &nCluster, Int_t &nClusters);

   ClassDef(CbmMuchClustering, 1);
};

#endif
