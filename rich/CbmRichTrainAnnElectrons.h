/**
 * \file CbmRichTrainAnnElectrons.h
 *
 * \brief Train ANN for electron identification in RICH.
 *
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 **/

#ifndef CbmRichTrainAnnElectrons_H
#define CbmRichTrainAnnElectrons_H

#include "FairTask.h"
#include "TH2D.h"

//class TH1;
//class TH1D;
//class TH2D;
class TClonesArray;

#include <vector>

using std::vector;

/**
 * \class RingElectronParam
 *
 * \brief Input Parameters for ANN for electron identification in RICH.
 *
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 **/
class RingElectronParam
{
public:
   Double_t fAaxis;
   Double_t fBaxis;
   Double_t fPhi;
   Double_t fRadAngle;
   Double_t fChi2;
   Double_t fRadPos;
   Double_t fNofHits;
   Double_t fDistance;
   Double_t fMomentum;
};


/**
 * \class CbmRichTrainAnnElectrons
 *
 * \brief Train ANN for electron identification in RICH.
 *
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 **/
class CbmRichTrainAnnElectrons: public FairTask{

public:

   /**
    * \brief Default constructor
    */
   CbmRichTrainAnnElectrons();

   /**
    * \brief Destructor
    */
	virtual ~CbmRichTrainAnnElectrons();

   /**
    * \brief Inherited from FairTask
    */
	virtual InitStatus Init();

   /**
    * \brief Inherited from FairTask
    */
	virtual void Exec(
	      Option_t* option);

   /**
    * \brief Inherited from FairTask
    */
	virtual void FinishTask();

private:
   /**
    * \brief Fill input parameters for ANN in array and histograms.
    */
   void DiffElandPi();

   /**
    * \brief Train and test ANN.
    */
   void TrainAndTestAnn();

   /**
    * \brief Draw results.
    */
   void Draw();

   Int_t fEventNum;
   TClonesArray* fRichHits;
   TClonesArray* fRichRings;
   TClonesArray* fRichPoints;
   TClonesArray* fMCTracks;
   TClonesArray* fRichRingMatches;
   TClonesArray* fRichProj;
   TClonesArray* fStsTrackMatches;
   TClonesArray* fGlobalTracks;
   TClonesArray* fStsTracks;

   Int_t fMinNofHitsInRichRing;
   Double_t fQuota;
   Int_t fMaxNofTrainSamples; // maximum number of train samples for ANN

   Int_t fNofPiLikeEl;
   Int_t fNofElLikePi;
   Double_t fAnnCut;
   // ANN outputs
   // [0] = is true; [1] = is fake
   vector<TH1D*> fhAnnOutput;
   vector<TH1D*> fhCumProb;

   // Data for ANN input
   // [0] = true, [1] = fake
   vector<vector<RingElectronParam> > fRElIdParams;

   // difference between electrons and pions
   // [0] = is electron; [1] = is pion
   vector<TH1D*> fhAaxis;
   vector<TH1D*> fhBaxis;
   vector<TH1D*> fhAaxisCor;
   vector<TH1D*> fhBaxisCor;
   vector<TH1D*> fhDist;
   vector<TH1D*> fhNofHits;
   vector<TH1D*> fhChi2;
   vector<TH1D*> fhRadPos;
   vector<TH2D*> fhAaxisVsMom;
   vector<TH2D*> fhBaxisVsMom;
   vector<TH2D*> fhPhiVsRadAng;

   vector<TH1*> fHists; // Store pointer for all histograms

   /**
    * \brief Copy constructor.
    */
   CbmRichTrainAnnElectrons(const CbmRichTrainAnnElectrons&);

   /**
    * \brief Assignment operator.
    */
   CbmRichTrainAnnElectrons& operator=(const CbmRichTrainAnnElectrons&);

   ClassDef(CbmRichTrainAnnElectrons,1)
};

#endif

