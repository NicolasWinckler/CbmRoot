/*
 * CbmClusteringSL.h
 *
 *  Created on: Apr 10, 2012
 *      Author: kozlov
 */

#ifndef CBMCLUSTERINGSL_H_
#define CBMCLUSTERINGSL_H_

#include "FairTask.h"
#include "CbmClusteringGeometry.h"

class CbmClusteringSL//: public FairTask
{
public:
   /**
    * \brief Constructor.
    */
	CbmClusteringSL();

	CbmClusteringSL(CbmClusteringGeometry* moduleGeo);
   /**
    * \brief Destructor.
    */
   virtual ~CbmClusteringSL();

   void SLRec1(CbmClusteringGeometry* moduleGeo, Int_t activePad);
   void SLRec2(CbmClusteringGeometry* moduleGeo, Int_t activePad);
   void MainClusteringSL(CbmClusteringGeometry* moduleGeo, Int_t algVersion);

   Int_t GetNofClusters()	const { return fNofClusters;}
   Int_t GetNofPads()		const { return fNofPads;}
   Int_t GetNofActivePads()	const { return fNofActivePads;}

   //Int_t GetCluster(Int_t iCluster);
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
	   //Int_t fNofCluster;
	   Float_t fX;
	   Float_t fY;
	   UInt_t fCharge;
	   Int_t fNofPads;
	   vector<Int_t> fPadsInCluster;
   };
   Cluster* fClusters;
};

#endif /* CBMCLUSTERINGSL_H_ */
