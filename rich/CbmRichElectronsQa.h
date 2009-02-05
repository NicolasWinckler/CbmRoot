#ifndef CBMRICHELECTRONSQA_H
#define CBMRICHELECTRONSQA_H

#include "CbmTask.h"
#include "CbmRichRing.h"
#include "CbmTrdTrack.h"
#include "CbmRichElectronIdAnn.h"

#include "TH1D.h"
#include "TH2D.h"
#include "TClonesArray.h"


#include <map>
#include <fstream>

using std::map;

class CbmRichElectronsQa : public CbmTask{

public:

	/** Default constructor **/
	CbmRichElectronsQa();

	/** Standard Constructor with name, title, and verbosity level
	 *@param verbose      verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
	 */
	CbmRichElectronsQa(const char *name, const char *title, Int_t verbose);

	/** Destructor **/
	virtual ~CbmRichElectronsQa();

	/** Initialization of the task **/
	virtual InitStatus Init();

	/** Executed task **/
	virtual void Exec(Option_t* option);

	/** Finish task **/
	virtual void Finish();

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
    Double_t fTrdAnnCut;
    Bool_t DoesRingHaveProjection(Int_t trackId);
    Bool_t IsRichElectron(CbmRichRing* ring, Double_t momentum);
    Bool_t IsTrdElectron(CbmTrdTrack* trdTrk);
    Bool_t IsTofElectron();
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

// difference between electrons and pions
	TH1D* fhAaxisEl;
	TH1D* fhAaxisPi;
	TH1D* fhBAxisEl;
	TH1D* fhBAxisPi;
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

