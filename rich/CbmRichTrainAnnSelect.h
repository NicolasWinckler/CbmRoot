/**
 * \file CbmRichTrainAnnSelect.h
 *
 * \brief Train ANN for fake rejection.
 *
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2005
 **/

#ifndef CbmRichTrainAnnSelect_H
#define CbmRichTrainAnnSelect_H

#include "FairTask.h"
#include <vector>
#include "CbmRichRingFitterCOP.h"
#include "CbmRichRingSelectImpl.h"

using std::vector;

class TH1D;
class TH1;
//class CbmRichRingFitterCOP;
//class CbmRichRingSelectImpl;
class CbmRichRing;

/**
 * \class RingSelectParam
 *
 * \brief Input Parameters for ANN.
 *
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2005
 **/
class RingSelectParam
{
public:
   Double_t fNofHits;
   Double_t fAngle;
   Double_t fHitsOnRing;
   Double_t fRadPos;
   Double_t fChi2;
   Double_t fRadius;
};

/**
 * \class CbmRichTrainAnnSelect
 *
 * \brief Train ANN for fake rejection.
 *
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2005
 **/
class CbmRichTrainAnnSelect : public FairTask
{
public:

   /**
    * \brief Default constructor
    */
   CbmRichTrainAnnSelect();

   /**
    * \brief Destructor
    */
   virtual ~CbmRichTrainAnnSelect();

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
    * \brief Set recFlag weather ring was found correctly or not.
    */
   void SetRecFlag();

   /**
    * \brief Fill ring selection parameters in array and histograms.
    */
   void DiffFakeTrueCircle();

   /**
    * \brief Train and test ANN.
    */
   void TrainAndTestAnn();

   /**
    * \brief Draw results.
    */
   void Draw();

   TClonesArray* fRings; // Array of CbmRichRings
   TClonesArray* fTracks; // Array of CbmMCTracks
   TClonesArray* fMatches; // Array of CbmRichRingMatch

   Int_t fEventNumber;
   Double_t fQuota; // Quality quota for reconstructed ring
   Int_t fMaxNofTrainSamples; // maximum number of train samples for ANN

   // Difference Fake and True rings histograms BEGIN
   // [0] = is true; [1] = is fake
   vector<TH1D*> fhNofHits;
   vector<TH1D*> fhAngle;
   vector<TH1D*> fhNofHitsOnRing;
   vector<TH1D*> fhChi2;
   vector<TH1D*> fhRadPos;
   vector<TH1D*> fhRadius;

   // ANN outputs
   // [0] = is true; [1] = is fake
   vector<TH1D*> fhAnnOutput;
   vector<TH1D*> fhCumProb;

   // Data for ANN input
   // [0] = true, [1] = fake
   vector<vector<RingSelectParam> > fRSParams;

   CbmRichRingFitterCOP* fFitCOP; // Pointer to COP fitter
   CbmRichRingSelectImpl* fSelectImpl; // Pointer to ellipse fitter

   vector<TH1*> fHists; // Store pointer for all histograms

   ClassDef(CbmRichTrainAnnSelect,1)
};

#endif
