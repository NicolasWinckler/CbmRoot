/*
 * CbmClusteringSL.h
 *
 *  Created on: Apr 10, 2012
 *      Author: kozlov
 */

#ifndef CBMCLUSTERINGSL_H_
#define CBMCLUSTERINGSL_H_

#include "FairTask.h"
#include "CbmMuchGeoCl.h"

class CbmClusteringSL//: public FairTask
{
public:
   /**
    * \brief Constructor.
    */
	CbmClusteringSL();

	CbmClusteringSL(CbmMuchGeoCl* moduleGeo);
   /**
    * \brief Destructor.
    */
   virtual ~CbmClusteringSL();

   void SLRec1(CbmMuchGeoCl* moduleGeo, Int_t activePad);
   void SLRec2(CbmMuchGeoCl* moduleGeo, Int_t activePad);
   void MainClusteringSL(CbmMuchGeoCl* moduleGeo, Int_t algVersion);

   Int_t GetNofClusters()	const { return fNofClusters;}
   Int_t GetNofPads()		const { return fNofPads;}
   Int_t GetNofActivePads()	const { return fNofActivePads;}

   Int_t GetCluster(Int_t iCluster);
   Float_t GetX0(Int_t iCluster);
   Float_t GetY0(Int_t iCluster);
   UInt_t GetClCharge(Int_t iCluster);
   Int_t GetNofPads(Int_t iCluster);
   Int_t GetPadInCluster(Int_t iCluster, Int_t iPad);

private:

   Int_t fNofPads;
   Int_t fNofActivePads;

   UInt_t* fA1; //g_MAX_PADS
   Bool_t* fS; //g_MAX_PADS
   Int_t* fNumbersOfPads; //g_MAX_PADS

   Int_t fNofClusters;
   struct Cluster{
	   Int_t nofCluster;
	   Float_t xc;
	   Float_t yc;
	   UInt_t sumClCharge;
	   Int_t nofPads;
	   Int_t padsInCluster[50];
   };
   Cluster* fClusters;
};

#endif /* CBMCLUSTERINGSL_H_ */
