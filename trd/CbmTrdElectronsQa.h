
/** CbmTrdElectronsQa
 * Author: Semen Lebedev (S.Lebedev@gsi.de)
 **/


#ifndef CBM_TRD_ELECTRONS_QA
#define CBM_TRD_ELECTRONS_QA 1


#include "FairTask.h"
#include "CbmTrdTrack.h"

#include "TH1F.h"

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
	void BuildEnergyLossesAnaHistos();

	void SetOutFileNamePi(TString name){fOutFileNamePi = name;}
	void SetOutFileNameEl(TString name){fOutFileNameEl = name;}

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

    TH1F* fhPiELoss;
    TH1F* fhPiELossSum;
    TH1F* fhEldEdX;
    TH1F* fhElTR;
    TH1F* fhElELoss;
    TH1F* fhElElossSum;
    TH1F* fhElNofZeroTR;


	Int_t fEventNum;

	ClassDef(CbmTrdElectronsQa,1);

};

#endif
