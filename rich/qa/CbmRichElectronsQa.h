#ifndef CBMRICHELECTRONSQA_H
#define CBMRICHELECTRONSQA_H

#include "FairTask.h"
#include "CbmRichRing.h"
#include "CbmTrdTrack.h"
#include "CbmRichElectronIdAnn.h"
#include "CbmGlobalTrack.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TClonesArray.h"


#include <map>
#include <fstream>

using std::map;

class CbmRichElectronsQa : public FairTask{

public:

	CbmRichElectronsQa(const char *name, const char *title, Int_t verbose);

	virtual ~CbmRichElectronsQa();
	virtual InitStatus Init();
	virtual void Exec(Option_t* option);
	virtual void FinishTask();
	void SetDefaultParameters();
	Bool_t SetParameters();
	void SetGeoType(TString richGeo){fRichGeoType = richGeo;}
    void SetTrdAnnCut(Double_t annCut){fTrdAnnCut = annCut;}
    void SetRichAnnCut(Double_t richAnnCut){fRichAnnCut = richAnnCut;}
    void SetMeanA(Double_t meanA){fMeanA = meanA;}
    void SetMeanB(Double_t meanB){fMeanB = meanB;}
    void SetRmsA(Double_t rmsA){fRmsA = rmsA;}
    void SetRmsB(Double_t rmsB){fRmsB = rmsB;}
    void SetRmsCoeff(Double_t rmsCoeff){fRmsCoeff = rmsCoeff;}
    void SetDistCut(Double_t dist){fDistCut = dist;}
    void SetUseRichAnn(Bool_t useRichAnn){fUseRichAnn = useRichAnn;}

private:
	Int_t fEventNum;

	TClonesArray* fRichHits;
	TClonesArray* fRichRings;
	TClonesArray* fRichPoints;
	TClonesArray* fMCTracks;
	TClonesArray* fRichRingMatches;
	TClonesArray* fRichProj;
	TClonesArray* fStsTrackMatches;
	TClonesArray* fGlobalTracks;
	TClonesArray* fTrdTracks;
	TClonesArray* fStsTracks;
	TClonesArray* fTrdTrackMatches;
	TClonesArray* fTofPoints;
	TClonesArray* fTofHits;

    Int_t fMinNofHitsInRichRing;
    Int_t fMinNofTrdHits;
    TString fRichGeoType;
    Double_t fTrdAnnCut;
    Double_t fRichAnnCut;
    Double_t fMeanA;
    Double_t fMeanB;
    Double_t fRmsA;
    Double_t fRmsB;
    Double_t fRmsCoeff;
    Double_t fDistCut;
    Bool_t fUseRichAnn;

    Bool_t DoesRingHaveProjection(Int_t trackId);
    Bool_t IsRichElectron(CbmRichRing* ring, Double_t momentum);
    Bool_t IsTrdElectron(CbmTrdTrack* trdTrk);
    Bool_t IsTofElectron(CbmGlobalTrack* gTrack, Double_t momentum);
    void AccTracks();
    void FoundRichRings();
    void RingTrackMatch();
    void GlobalTracksMatchEff();
    void GlobalTracksElIdEff();
    void DiffElandPi();


    std::ofstream  fOutElandPi;
    CbmRichElectronIdAnn * fElIdAnn;

//RICH histograms and counters
	Int_t fNofMCRings;
    Int_t fNofAccRings;
    Int_t fNofTrueFoundRings;
	Int_t fNofTrueMatchRings;
	Int_t fNofTrueElIdRings;

    Int_t fNofFakeRings;
    Int_t fNofCloneRings;


	TH1D* fhMCRings;
	TH1D* fhAccRings;
	TH1D* fhTrueFoundRings;
	TH1D* fhTrueMatchRings;
	TH1D* fhTrueElIdRings;

//GLOBAL histograms and counters
	Int_t fNofAccRichTrdGlobal;
	Int_t fNofAccRichTrdTofGlobal;

	Int_t fNofTrueMatchStsRichGlobal;
	Int_t fNofTrueMatchStsRichTrdGlobal;
	Int_t fNofTrueMatchStsRichTrdTofGlobal;

	TH1D* fhAccRichTrdGlobal;
	TH1D* fhAccRichTrdTofGlobal;

	TH1D* fhTrueMatchStsRichGlobal;
	TH1D* fhTrueMatchStsRichTrdGlobal;
	TH1D* fhTrueMatchStsRichTrdTofGlobal;

//ELECTRON IDENTIFICATION
	Int_t fNofTrueIdRich;
	TH1D* fhTrueIdRich;
	Int_t fNofAccPi;
	TH1D* fhAccPi;
	Int_t fNofPiasElRich;
	TH1D* fhPiasElRich;

	Int_t fNofTrueIdRichTrd;
	TH1D* fhTrueIdRichTrd;
	Int_t fNofPiasElRichTrd;
	TH1D* fhPiasElRichTrd;

	Int_t fNofTrueIdRichTrdTof;
	TH1D* fhTrueIdRichTrdTof;
	Int_t fNofPiasElRichTrdTof;
	TH1D* fhPiasElRichTrdTof;

// difference between electrons and pions
	TH1D* fhAaxisEl;
	TH1D* fhAaxisPi;
	TH1D* fhBAxisEl;
	TH1D* fhBAxisPi;
	TH1D* fhAaxisCorEl;
	TH1D* fhAaxisCorPi;
	TH1D* fhBAxisCorEl;
	TH1D* fhBAxisCorPi;
	TH1D* fhDistEl;
	TH1D* fhDistPi;
	TH1D* fhNofHitsEl;
	TH1D* fhNofHitsPi;
	TH1D* fhChi2El;
	TH1D* fhChi2Pi;
	TH1D* fhRadPosEl;
	TH1D* fhRadPosPi;
	TH2D* fhAaxisVsMomEl;
	TH2D* fhAaxisVsMomPi;
	TH2D* fhBAxisVsMomEl;
	TH2D* fhBAxisVsMomPi;
	TH2D* fhPhiVsRadAngEl;
	TH2D* fhPhiVsRadAngPi;

	TH1D* fhTrdAnnEl;
	TH1D* fhTrdAnnPi;


	ClassDef(CbmRichElectronsQa,1)

};

#endif

