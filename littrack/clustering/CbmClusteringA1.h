/*
 * CbmClusteringA1.h
 *
 *  Created on: Apr 4, 2012
 *      Author: kozlov
 */

#ifndef CBMCLUSTERINGA1_H_
#define CBMCLUSTERINGA1_H_

#include "FairTask.h"
#include "CbmClusteringGeometry.h"
//#include "TClonesArray.h"
//#include "CbmMuchClFull.h"

//class CbmMuchGeoScheme;
//class CbmMuchCluster;
//class CbmMuchPad;
//class TClonesArray;

class CbmClusteringA1//: public FairTask
{
public:
   /**
    * \brief Constructor.
    */
	CbmClusteringA1();

	CbmClusteringA1(CbmClusteringGeometry* moduleGeo);
   /**
    * \brief Destructor.
    */
   virtual ~CbmClusteringA1();

   void MainClusteringA1(CbmClusteringGeometry* moduleGeo, Int_t algVersion);
   void ChangeClusters(CbmClusteringGeometry* moduleGeo, Int_t nPad, Int_t Cl0, Int_t Cl1);

   Int_t GetNofClusters()	const { return fNofClusters;}
   Int_t GetNofPads()		const { return fNofPads;}
   Int_t GetNofActivePads()	const { return fNofActivePads;}

   Int_t GetCluster(Int_t iCluster);
   Float_t GetX0(Int_t iCluster);
   Float_t GetY0(Int_t iCluster);
   UInt_t GetClCharge(Int_t iCluster);
   Int_t GetNofPads(Int_t iCluster);
   Int_t GetPadInCluster(Int_t iCluster, Int_t iPad);
   Int_t GetNPadInCluster(Int_t iCluster, Int_t iPad);
   UInt_t GetPadCharge(Int_t iCluster, Int_t iPad);
   vector<Int_t> GetPads(Int_t iCluster);

private:

   Int_t fNofPads;
   Int_t fNofActivePads;

   UInt_t* fA1; //g_MAX_PADS
   UInt_t* fA2; //g_MAX_PADS
   Bool_t* fS; //g_MAX_PADS
   Int_t* fNumbersOfPads; //g_MAX_PADS

   //Float_t* fXc; //g_MAX_CENTERS
   //Float_t* fYc; //g_MAX_CENTERS
   //UInt_t* fSumClCharges; //g_MAX_CENTERS
   //Int_t* fNumbersOfClusters; //g_MAX_CENTERS - ???

   Int_t fNofClusters;
   struct Cluster{
	   Int_t fNCluster;
	   Float_t fX;
	   Float_t fY;
	   UInt_t fCharge;
	   Int_t fNofPads;
	   //Int_t padsInCluster[30];
	   vector<Int_t> fPadsInCluster;
	   vector<Int_t> fNPadsInCluster;
	   vector<Int_t> fPadsCharges;
   };
   Cluster* fClusters;
   //TClonesArray* fClFull;
};

#endif /* CBMCLUSTERINGA1_H_ */
