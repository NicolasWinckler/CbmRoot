/*
Class  : CbmRichRingQa
Description : Quality checks or ring finders: efficiency calculation etc.
Author : Semen Lebedev (S.Lebedev@gsi.de)
*/

#ifndef CBMRICHRINGQA_H
#define CBMRICHRINGQA_H

#include "CbmRichRing.h"

#include "FairTask.h"
#include "CbmRichRingFitterCOP.h"
#include "CbmRichRingFitterEllipseTau.h"
#include "CbmRichRingSelectImpl.h"
#include "TH1D.h"
#include "TH2D.h"

#include <map>
#include <fstream>


class CbmRichRingQa : public FairTask{

    TClonesArray* fRings;        // Array of CbmRichRings
    TClonesArray* fPoints;       // Array of FairMCPoints
    TClonesArray* fTracks;       // Array of CbmMCTracks
    TClonesArray* fHits;         // Array of CbmRichHits
    TClonesArray* fMatches;      // Array of CbmRichRingMatch
    TClonesArray* fProj;      // Array of CbmRichRingMatch
    TClonesArray* fTrackMatch;  //Array of STSTrackMatch
    TClonesArray* gTrackArray;   // Array of Global Tracks

    std::map<Int_t, Int_t> fRingMap;
    std::map<Int_t, CbmRichRing > fRingMapWithHits;
    Int_t fEventNumber;
    Int_t fNofHitsInRingCut;

    public:

    /** Default constructor **/
    CbmRichRingQa();

    /** Standard Constructor with name, title, and verbosity level
        *@param verbose      verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
    */
    CbmRichRingQa(const char *name, const char *title, Int_t verbose);

    /** Destructor **/
    virtual ~CbmRichRingQa();

    /** Initialization of the task **/
    virtual InitStatus Init();

    /** Executed task **/
    virtual void Exec(Option_t* option);

    /** Finish task **/
    virtual void FinishTask();

    void EfficiencyCalc();
    void DiffFakeTrue();
    void DiffFakeTrueCircle();
    Bool_t DoesRingHaveProjection(Int_t trackId);
    Double_t GetStsMomentum(CbmRichRing * ring);
    void RingTrackMatchEff();

    CbmRichRingFitterCOP* fFitCOP;
    CbmRichRingFitterEllipseTau* fFitEllipse;
    CbmRichRingSelectImpl* fSelectImpl;

    Int_t fNofAllRings;      //number of all MC rings
    Int_t fNofElRings;  //number of all electron MC rings
    Int_t fNofPiRings;  //number of all pion MC rings
    Int_t fNofElRingsProj; //number of El MC rings with proj
    Int_t fNofPiRingsProj; //number of Pi MC rings with proj
    Int_t fNofElRingsProjHitCut;//number of El MC rings with proj and # hits more than HitCut
    Int_t fNofPiRingsProjHitCut;//number of Pi MC rings with proj and # hits more than HitCut

    Int_t fNofTrueFoundElRingsProjHitCut;
    Int_t fNofTrueFoundPiRingsProjHitCut;
    Int_t fNofCloneRings;
    Int_t fNofFakeRings;

    Int_t fNofTrueElBadFit;

    TH1D* fh_TrueFoundElRingsProjHitCutMom;///Histogram of found rings vs momentum. Electron. Proj
    TH1D* fh_MCElRingsProjHitCutMom;///Histogram of MC rings vs momentum. Electron. Proj

    TH1D* fh_TrueFoundElRingsProjHitCutRadPos;///Histogram of found rings vs RadialPosition. Electron. Proj
    TH1D* fh_MCElRingsProjHitCutRadPos;///Histogram of MC rings vs RadialPosition. Electron. Proj

    TH1D* fh_TrueFoundElRingsProjHitCutNofHits;///Histogram of found rings vs nof Hits. Electron. Proj
    TH1D* fh_MCElRingsProjHitCutNofHits;///Histogram of MC rings vs Nof hits. Electron. Proj

    TH1D* fh_TrueFoundElRingsProjHitCutBoverA;
    TH1D* fh_MCElRingsProjHitCutBoverA;

    TH2D* fh_FakeFoundRingsXYAll;


/// Difference Fake and True rings histograms BEGIN
    TH1D*  fh_FakeNofHits;
    TH1D*  fh_TrueElNofHits;

    TH1D*   fh_FakeDistance;
    TH1D*   fh_TrueElDistance;

    TH1D*   fh_FakeAngle;
    TH1D*   fh_TrueElAngle;

    TH1D*   fh_FakeNofHitsOnRing;
    TH1D*   fh_TrueElNofHitsOnRing;

    TH1D* fh_FakeChi2;
    TH1D* fh_TrueElChi2;

    TH1D* fh_FakeRadPos;
    TH1D* fh_TrueElRadPos;

    TH1D* fh_FakeRadius;
    TH1D* fh_TrueElRadius;

    TH1D* fh_FakeA;
    TH1D* fh_TrueElA;

    TH1D* fh_FakeB;
    TH1D* fh_TrueElB;

    TH1D* fh_FakePhi;
    TH1D* fh_TrueElPhi;

    TH1D* fh_FakeStsMom;
    TH1D* fh_TrueElStsMom;

    TH2D* fh_TrueElPhiVsRadAngle;
    TH2D* fh_FakePhiVsRadAngle;

    TH2D* fh_TrueElRadiusVsMom;
    TH2D* fh_FakeRadiusVsMom;
/// Difference Fake and True rings histograms END

    /// Ring Track matching BEGIN
    TH1D* fh_WrongMatchElDistance;
    TH1D* fh_TrueMatchElDistance;
    TH1D*  fh_TrueMatchElMom;
    /// Ring Track matching END

    std::ofstream  foutFakeAndTrue;

    TH2D*  fh_HitsXY;//hits distribution (x,y)
    TH1D*  fh_HitsRadPos;
    TH2D*  fh_TrueFoundRingsXYE;// (x,y) of true electron rings
    TH2D*  fh_TrueFoundRingsXYPi; // (x,y) of true pions rings

    TH1D*  fh_NhitsPerEvent;//Nof hits per event
    TH1D*  fh_NprojPerEvent;//Nof projections/ event


    ClassDef(CbmRichRingQa,1)

};

#endif
