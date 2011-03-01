#ifndef CBMANAELECTRONSQA_H_
#define CBMANAELECTRONSQA_H_

#include "FairTask.h"
#include "CbmRichRing.h"
#include "CbmTrdTrack.h"
#include "CbmRichElectronIdAnn.h"
#include "CbmGlobalTrack.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TClonesArray.h"
#include "CbmRichHit.h"
#include "CbmVertex.h"
#include "CbmStsKFTrackFitter.h"

#include <map>
//#include <fstream>
#include <string>

using std::map;
using std::string;

class CbmAnaElectronsQa : public FairTask{

public:

	CbmAnaElectronsQa(const char *name, const char *title);

	virtual ~CbmAnaElectronsQa();
	virtual InitStatus Init();
	virtual void Exec(Option_t* option);
	virtual void FinishTask();
	void SetDefaultParameters();
	Bool_t SetParameters();
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


    CbmVertex *fPrimVertex;
    CbmStsKFTrackFitter fKFFitter;

//acceptance cuts
    Int_t fMinNofStsPoints;
    Int_t fMinNofRichRingHits;
    Int_t fMinNofTrdPoints;

//electron identification cuts
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

    void SignalElectronAcceptance();
    void SignalElectronMatching();
    void ElectronIdentificationEff();
    void TestDistributions();
    void StsQa();

    CbmRichElectronIdAnn * fElIdAnn;

//signal electron acceptance
	TH1D* fh_mc_el;
	TH1D* fh_acc_sts_el;
	TH1D* fh_acc_rich_el;
	TH1D* fh_acc_trd_el;
	TH1D* fh_acc_tof_el;

//signal electron matching
	TH1D* fh_match_rich;
	TH1D* fh_match_trd;
	TH1D* fh_match_tof;

//ELECTRON IDENTIFICATION AND PION SUPPRESSION
	TH1D* fh_elid_rich;
	TH1D* fh_elid_trd;
	TH1D* fh_elid_tof;

	TH1D* fh_acc_pi;
	TH1D* fh_pi_as_el_rich;
	TH1D* fh_pi_as_el_trd;
	TH1D* fh_pi_as_el_tof;

//test distributions
	TH1D* fh_nof_global;
	TH1D* fh_nof_sts_tracks;
	TH1D* fh_nof_rich_rings;
	TH1D* fh_nof_trd_tracks;

//STS Qa
    TH1D* fh_rec_mc_mom_signal;
    TH2D* fh_mom_res_vs_mom_signal;
    TH1D* fh_mean_mom_vs_mom_signal;
    TH1D* fh_count_mom_vs_mom_signal;
    TH1D* fh_chiprim_signal;
    TH1D* fh_chiprim_signal2;

	ClassDef(CbmAnaElectronsQa,1)
};

#endif /* CBMANAELECTRONSQA_H_ */
