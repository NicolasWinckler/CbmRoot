
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
	void FillProbabilityHistos(const std::vector<Double_t>& vec, Bool_t isEl);

	void SetOutFileNamePi(TString name){fOutFileNamePi = name;}
	void SetOutFileNameEl(TString name){fOutFileNameEl = name;}
	void SetGeoType(TString geoType){fGeoType = geoType;}
private:
	Double_t GetMomAtFirstTrdLayer(CbmTrdTrack* trdtrack);
	void GetELossInfo(CbmTrdTrack* trdtrack, Double_t *sumELoss, Double_t  eLossdEdX[],
			  Double_t  eLossTR[], Double_t  eLoss[]);
	Int_t GetNofClusters(CbmTrdTrack* trdtrack);
	Double_t GetElossRMS(CbmTrdTrack* trdtrack);
	Double_t fEnergyCutForClusters;

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
    TH1D* fhElNofClusters; // photon cluster counting method
    TH1D* fhPiNofClusters;
    TH1D* fhElElossRMS;
    TH1D* fhPiElossRMS;
    TH1D* fhElElossMediana; // mediana
    TH1D* fhPiElossMediana;

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

	CbmTrdElectronsQa(const CbmTrdElectronsQa&);
	CbmTrdElectronsQa& operator=(const CbmTrdElectronsQa&);

	ClassDef(CbmTrdElectronsQa,1);

};

#endif
