
/** CbmTrdElectronsQa
 * Author: Semen Lebedev (S.Lebedev@gsi.de)
 **/


#ifndef CBM_TRD_ELECTRONS_QA
#define CBM_TRD_ELECTRONS_QA 1


#include "FairTask.h"
#include "CbmTrdTrack.h"

#include "TH1D.h"

class TClonesArray;

#include <fstream>

class CbmTrdElectronsQa : public FairTask
{
public:

	CbmTrdElectronsQa();

	~CbmTrdElectronsQa();


	virtual InitStatus Init();
	virtual void Exec(Option_t* opt);
	virtual void Finish();
	void MakeTxtFile();
	void InitHistos();
	void FillEnergyLossesAnaHistos();
	void FillTestHistos();
	void ElIdAnalysis();

	void SetOutFileNamePi(TString name){fOutFileNamePi = name;}
	void SetOutFileNameEl(TString name){fOutFileNameEl = name;}
	void SetGeoType(TString geoType){fGeoType = geoType;}
private:
	Double_t GetMomAtFirstTrdLayer(CbmTrdTrack* trdtrack);
	void GetELossInfo(CbmTrdTrack* trdtrack, Double_t *sumELoss, Double_t  eLossdEdX[],
			  Double_t  eLossTR[], Double_t  eLoss[]);

	TClonesArray* fMCTracks;
	TClonesArray* fTrdPoints;
	TClonesArray* fTrdTracks;
	TClonesArray* fTrdTrackMatches;
	TClonesArray* fTrdHits;

	std::ofstream fOutPi;
	std::ofstream fOutEl;
	TString fOutFileNamePi;
	TString fOutFileNameEl;
	TString fGeoType;

    TH1D* fhPiELoss;
    TH1D* fhPiELossSum;
    TH1D* fhEldEdX;
    TH1D* fhElTR;
    TH1D* fhElELoss;
    TH1D* fhElElossSum;
    TH1D* fhElNofZeroTR;


// Histograms for testing current Pid routines in TRD
	TH1D* fhNofTrdHitsEl;
	TH1D* fhNofTrdHitsPi;
	TH1D* fhPidANNEl[7];
	TH1D* fhPidANNPi[7];

	TH1D* fhElossSortPi[12];
	TH1D* fhElossSortEl[12];
	TH1D* fhCumProbSortEl[12];
	TH1D* fhCumProbSortPi[12];

	Int_t fEventNum;

	ClassDef(CbmTrdElectronsQa,1);

};

#endif
