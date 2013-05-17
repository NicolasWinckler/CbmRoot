/*
 * CbmMuchClusteringQa0.h
 *
 *  Created on: May 24, 2012
 *      Author: kozlov
 */

#ifndef CBMMUCHCLUSTERINGQA0_H_
#define CBMMUCHCLUSTERINGQA0_H_

#include "FairTask.h"
#include "TClonesArray.h"

#include "TH1.h"
#include "TH2.h"
#include "TH1F.h"

class CbmMuchGeoScheme;
class TObjArray;

class TH1F;

class CbmMuchClusteringQa0 : public FairTask {

public:
	CbmMuchClusteringQa0();
	virtual ~CbmMuchClusteringQa0();
	virtual InitStatus Init();
	virtual void Exec(Option_t * option);
	virtual void FinishTask();

	void SetMCPoints();
	void LinkingClustersToMCPoints();
	void CreationOfRelations();
	Float_t CalculateEfficiency();
	void CalculateAccuracy();
	void CalculateEfficienciByLayerSide();

	void FillEfficiencyByLayerSideHistograms();
	void FillErrorsByLayerSideHistograms();

	void FillQualityClToPointHistograms();
	void FillQualityDigiToClHistograms();

	void FillErrorsByRadiusHistograms();

private:
	CbmMuchGeoScheme* fGeoScheme;
	Int_t fNofEvents;
	//TClonesArray* fClFull;
	TClonesArray* fMuchDigi;
	TClonesArray* fMuchDigiMatch;
	Int_t fNofDigis;
	TClonesArray* fMuchCluster;
	TClonesArray* fMuchHit;
	TClonesArray* fMuchPoint;
	Int_t fNofPoints;
	struct M_Point{
		Float_t x1, y1, x2, y2, xc, yc;
		//Int_t nofSubCenters;
		Float_t pLenght;
		//Int_t moduleId;
		//Int_t lSideId;
		Int_t station;
		Int_t leyer;
		Bool_t side;
		Int_t nofClusters;
		Int_t clustersToPoint[50];
		Int_t digisByCluster[50];
		Int_t nofDigis;
	};
	M_Point* fRealPoints;
	Int_t fNofClusters;
	Float_t fEfficiency;
	Int_t fModulesByLayerSide[6][3][2];
	Int_t fClustersByLayerSide[6][3][2];
	Int_t fPointsBylayerSide[6][3][2];
	Float_t fEfficiencyLayerSide[6][3][2];
	Float_t fErrorByLayerSide[6][3][2];
	Float_t fAccuracyLayerSide[6][3][2];
	struct AccuracyStruct{
		Float_t errorX;
		Float_t errorY;
		Float_t errorXY;
		//Int_t moduleId;
		//Int_t lSideId;
		Int_t nHit;
		Int_t nPoint;
	};
	AccuracyStruct* fAccuracyArray;
	struct Cluster{
		//Int_t nofCluster;
	   	//Float_t xc;
	   	//Float_t yc;
	   	//UInt_t sumClCharge;
	   	//Int_t nofPads;
	   	//Int_t padsInCluster[30];
		Int_t station;
		Int_t leyer;
		Bool_t side;
		Int_t bestPoint;
	   	Int_t nofMPoints;
	   	Int_t mPointsInCluster[30];
	   	Int_t mDigisByPoint[30];
	   	Int_t mPointsQuality[30];
	   	Float_t mClToPoint[30];
	   	Float_t mDigiToCl[30];
	   	Float_t mPointToCl[30];
	};
	Cluster* fClusters;

	TH1F* fhMuchEfficiencyByLayerSide;
	TH1F* fhMuchErrorsByLayerSide;
	TH1F* fhMuchQualityClToPointByLayer;
	TH1F* fhMuchQualityDigiToClByLayer;

	TH1F* fhMuchErrorsByRadiusS1L0;
	TH1F* fhMuchErrorsByRadiusS2L0;
	TH1F* fhMuchErrorsByRadiusS3L0;
	TH1F* fhMuchErrorsByRadiusS4L0;

	TH1F* fhMuchQualityClToPointByRadiusS1L0;
	TH1F* fhMuchQualityClToPointByRadiusS2L0;
	TH1F* fhMuchQualityClToPointByRadiusS3L0;
	TH1F* fhMuchQualityClToPointByRadiusS4L0;

	TH1F* fhMuchQualityDigiToClByRadiusS1L0;
	TH1F* fhMuchQualityDigiToClByRadiusS2L0;
	TH1F* fhMuchQualityDigiToClByRadiusS3L0;
	TH1F* fhMuchQualityDigiToClByRadiusS4L0;

	static const Int_t nRadiusSteps = 50;

  CbmMuchClusteringQa0(const CbmMuchClusteringQa0&);
  CbmMuchClusteringQa0& operator=(const CbmMuchClusteringQa0&);
  
  ClassDef(CbmMuchClusteringQa0,1)
};

#endif /* CBMMUCHCLUSTERINGQA0_H_ */
