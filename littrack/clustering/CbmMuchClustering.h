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

   void MuchClustering(Int_t algVersion, CbmMuchGeoScheme* scheme);

   void CreateModulesGeometryArray();

   void SetDigiCharges();

   void ClusteringMainFunction();

private:

   void ReadDataBranches();

   Int_t fAlgorithmVersion;
   Int_t fNofModules;

   CbmClusteringA1* fClustersA1;
   CbmClusteringSL* fClustersSL;
   CbmClusteringWard* fClustersWard;

   CbmMuchGeoScheme* fScheme;

   CbmMuchGeoCl** fModulesGeometryArray;
   map <Int_t, Int_t> fModulesByDetId;

   TClonesArray* fMuchDigi;
   TClonesArray* fMuchPad;
   TClonesArray* fMuchPoint;

   TClonesArray* fCluster;
   TClonesArray* fHit;
   //TClonesArray* fStrawHit;

   void ClusteringA1(CbmMuchGeoCl* m1, CbmMuchModuleGem* m2, Int_t Ver, Int_t &nHit, Int_t &nCluster);
   void ClusteringSL(CbmMuchGeoCl* m1, CbmMuchModuleGem* m2, Int_t Ver, Int_t &nHit, Int_t &nCluster);

   ClassDef(CbmMuchClustering, 1);
};

#endif
