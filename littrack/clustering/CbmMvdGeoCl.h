/*
 * CbmMvdGeoCl.h
 *
 *  Created on: Jun 1, 2012
 *      Author: kozlov
 */

#ifndef CBMMVDGEOCL_H_
#define CBMMVDGEOCL_H_

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

//class CbmMuchGeoScheme;
//class CbmMuchCluster;
//class CbmMuchPad;
//class TClonesArray;

class CbmMvdGeoCl//: public FairTask
{
public:
   /**
    * \brief Constructor.
    */
	CbmMvdGeoCl();
	//CbmMvdGeoCl(Int_t nStation, Int_t nLayer, Bool_t nSide, Int_t nModule, CbmMuchGeoScheme* scheme);
   /**
    * \brief Destructor.
    */
   virtual ~CbmMvdGeoCl();

   //void SetLayerGeom(Int_t nStation, Int_t nLayer, Bool_t nSide, Int_t nModule, CbmMuchGeoScheme* scheme);

   //Int_t GetStation()		const { return fStation; }
   //Int_t GetLayer()			const { return fLayer; }
   //Bool_t GetLayerSide()	const { return fSide; }
   //Int_t GetModule()		const { return fModule; }
   //Int_t GetDetId()			const { return fDetId; }

   Int_t GetMvdGeometry();

   Int_t GetNPads()			const { return fNofPads; }
   Int_t GetAPadsNom()		const { return fActivePads;}
   void SetAPadsNom(Int_t nPads);

   Float_t GetX1(Int_t iPad);
   Float_t GetX2(Int_t iPad);
   Float_t GetY1(Int_t iPad);
   Float_t GetY2(Int_t iPad);
   Float_t GetX0(Int_t iPad);
   Float_t GetY0(Int_t iPad);

   Int_t GetPadNum(Int_t iPad);
   Int_t GetNeighborsNum(Int_t iPad);
   Int_t GetGoodNeighborsNum(Int_t iPad);
   Int_t GetNeighbor(Int_t iPad, Int_t iNeighbor);
   Int_t GetGoodNeighbor(Int_t iPad, Int_t iNeighbor);
   Long64_t GetPadID(Int_t iPad);

   UInt_t GetPadCharge(Int_t iPad);
   void SetPadCharge(Int_t iPad, UInt_t iCharge);

   /*std::map<pair<Int_t, Int_t>, Int_t> fDigiMap;
   std::map<pair<Int_t, Int_t>, Int_t>::iterator fDigiMapIt;*/



private:

   template <typename T1>
   T1 GetMin(T1& a, T1& b);
   template <typename T2>
   T2 GetMax(T2& a, T2& b);

   Bool_t SubEqual(Float_t x1, Float_t x2, Float_t l);

   Long64_t fNofPads;
   //Int_t fStation;
   //Int_t fLayer;
   //Bool_t fSide;
   //Int_t fModule;
   //Int_t fDetId;
   Int_t fActivePads;

   struct MvdGeom{
	   Float_t x1, x2, y1, y2, xc, yc;
	   Int_t N;
	   UInt_t charge;
	   Int_t nofNeighbors;
	   Int_t nofGoodNeighbors;
	   Int_t allNeighbors[15];
	   Int_t goodNeighbors[10];
	   Long64_t channelID;
   };
   MvdGeom* fMvdGeo;

   std::map<Int_t, CbmMvdStation*> fStationMap;

};

#endif /* CBMMVDGEOCL_H_ */
