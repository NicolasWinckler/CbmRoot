/*
 * CbmClusteringGeometry.h
 *
 *  Created on: Feb 22, 2012
 *      Author: kozlov
 */

#ifndef CBMCLUSTERINGGEOMETRY_H_
#define CBMCLUSTERINGGEOMETRY_H_

#include "FairTask.h"
#include "CbmMuchGeoScheme.h"

//class CbmMuchGeoScheme;
//class CbmMuchCluster;
//class CbmMuchPad;
//class TClonesArray;

class CbmClusteringGeometry//: public FairTask
{
public:
   /**
    * \brief Constructor.
    */
	CbmClusteringGeometry();
	CbmClusteringGeometry(Int_t nStation, Int_t nLayer, Bool_t nSide, Int_t nModule, CbmMuchGeoScheme* scheme, Int_t geoVersion);
	/*CbmClusteringGeometry(CbmClusteringGeometry* geo, Float_t x, Float_t y, Float_t dx, Float_t dy, Double_t phi1, Double_t phi2, Float_t r1, Float_t r2,
	   Int_t digiNum, UInt_t charge, Int_t nofNeighbors, Int_t nofGoodNeighbors, vector<Int_t> neighbors, Long64_t chID);*/
	/*CbmClusteringGeometry(CbmClusteringGeometry* geo, Int_t nPad,
			Int_t digiNum, Int_t nofGoodNeighbors, vector<Int_t> neighbors, Long64_t chID);*/
	CbmClusteringGeometry(CbmClusteringGeometry* geo, Int_t nofPads);
	void CbmClusteringSetPad(Int_t nPad, Float_t x, Float_t y, Float_t dx, Float_t dy, Double_t phi1, Double_t phi2, Float_t r1, Float_t r2,
			   Int_t digiNum, UInt_t charge, /*Int_t nofNeighbors, Int_t nofGoodNeighbors, vector<Int_t> neighbors, */Long64_t chID);
   /**
    * \brief Destructor.
    */
   virtual ~CbmClusteringGeometry();

   void SetMuchModuleGeometry(Int_t nStation, Int_t nLayer, Bool_t nSide, Int_t nModule, CbmMuchGeoScheme* scheme);
   void SetMuchModuleGeometryRadial(Int_t nStation, Int_t nLayer, Bool_t nSide, Int_t nModule, CbmMuchGeoScheme* scheme);

   //Int_t GetStation()		const { return fStation; }
   //Int_t GetLayer()			const { return fLayer; }
   //Bool_t GetLayerSide()	const { return fSide; }
   //Int_t GetModule()		const { return fModule; }
   Int_t GetDetId()			const { return fDetId; }

   Int_t GetNPads()			const { return fNofPads; }
   Int_t GetAPadsNom()		const { return fNofActivePads;}
   void SetAPadsNom(Int_t nPads);
   void SetAPadsPlusOne();

   Float_t GetDx(Int_t iPad);
   Float_t GetDy(Int_t iPad);
   //Float_t GetY1(Int_t iPad);
   //Float_t GetY2(Int_t iPad);
   Float_t GetX0(Int_t iPad);
   Float_t GetY0(Int_t iPad);

   Int_t GetDigiNum(Int_t iPad);
   void SetDigiNum(Int_t iPad, Int_t iDigi);
   Int_t GetNeighborsNum(Int_t iPad);
   Int_t GetGoodNeighborsNum(Int_t iPad);
   Int_t GetNeighbor(Int_t iPad, Int_t iNeighbor);
   //Int_t GetGoodNeighbor(Int_t iPad, Int_t iNeighbor);
   Long64_t GetPadID(Int_t iPad);

   UInt_t GetPadCharge(Int_t iPad);
   void SetPadCharge(Int_t iPad, UInt_t iCharge);

   Int_t GetPadByChannelId(Long64_t chId);

   Double_t GetPhi1(Int_t iPad);
   Double_t GetPhi2(Int_t iPad);
   Float_t GetR1(Int_t iPad);
   Float_t GetR2(Int_t iPad);
   vector<Int_t> GetNeighbors(Int_t iPad);
   Long64_t GetChannelID(Int_t iPad);



private:

   template <typename T1>
   T1 GetMin(T1& a, T1& b);
   template <typename T2>
   T2 GetMax(T2& a, T2& b);

   Bool_t SubEqual(Double_t x1, Double_t x2, Double_t l);

   Long64_t fNofPads;
   //Int_t fStation;
   //Int_t fLayer;
   //Bool_t fSide;
   //Int_t fModule;
   Int_t fDetId;
   Int_t fNofActivePads;

   map <Long64_t, Int_t> fPadByChannelId;
   struct PadInformation{
	   Float_t fX, fY;
	   Float_t fDx, fDy;
	   Double_t fPhi1, fPhi2;
	   Float_t fR1, fR2;
	   Int_t fDigiNum;
	   UInt_t fCharge;
	   Int_t fNofNeighbors;
	   Int_t fNofGoodNeighbors;
	   vector<Int_t> fNeighbors;
	   Long64_t channelID;
   };
   PadInformation* fPadList;

   //ClassDef(CbmClusteringGeometry, 1);
};

#endif
