/*
 * CbmMuchClustering.h
 *
 *  Created on: Mar 5, 2012
 *      Author: kozlov
 */

#ifndef CBMMUCHCLUSTERING_H_
#define CBMMUCHCLUSTERING_H_

#include "FairTask.h"
#include "CbmClusteringGeometry.h"
#include "CbmClusteringA1.h"
#include "CbmClusteringSL.h"
#include "CbmClusteringWard.h"
#include "CbmMuchGeoScheme.h"
#include "CbmMuchModuleGem.h"
//#include "TClonesArray.h"

#include "TH1F.h"

class TClonesArray;

class CbmMuchClustering: public FairTask
{
public:
   /**
    * \brief Constructor.
    */
	CbmMuchClustering(const char* digiFileName);

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
   void SetGeometryVersion(Int_t GeometryVersion)	{ fGeoVersion = GeometryVersion;}

   void DeletePadsCharge(CbmClusteringGeometry* moduleGeo);

   void CreateModulesGeometryArray();

   void SetDigiCharges();
   void ClearDigiCharges();

   void ClusteringMainFunction();

private:

   void ReadDataBranches();

   Int_t fAlgorithmVersion;
   Int_t fNofModules;
   Int_t fNofClusters;
   Int_t fGeoVersion;

   CbmClusteringA1* fClustersA1;
   CbmClusteringSL* fClustersSL;
   CbmClusteringWard* fClustersWard;

   CbmMuchGeoScheme* fScheme;
   TString            fDigiFile;

   CbmClusteringGeometry** fModulesGeometryArray;
   map <Int_t, Int_t> fModulesByDetId;

   TClonesArray* fMuchDigi;
   TClonesArray* fMuchPad;
   TClonesArray* fMuchPoint;

   TClonesArray* fCluster;
   TClonesArray* fHit;
   //TClonesArray* fStrawHit;
   TH1F* fhDigisInCluster;
   Int_t fNofEvents;

   void ClusteringA1(CbmClusteringGeometry* m1, CbmMuchModuleGem* m2, Int_t Ver/*, Int_t &nHit, Int_t &nCluster*/);
   void ClusteringSL(CbmClusteringGeometry* m1, CbmMuchModuleGem* m2, Int_t Ver/*, Int_t &nHit, Int_t &nCluster*/);
   void ClusteringWard(CbmClusteringGeometry* m1, CbmMuchModuleGem* m2/*, Int_t Ver, Int_t &nHit, Int_t &nCluster*/);

   ClassDef(CbmMuchClustering, 1);
};

#endif
