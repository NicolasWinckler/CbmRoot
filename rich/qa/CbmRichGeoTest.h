 /**
 * \file CbmRichGeoTest.h
 *
 * \brief RICH geometry checking and testing.
 *
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 **/

#ifndef CBMRICHGEOTEST_H
#define CBMRICHGEOTEST_H

#include "FairTask.h"

class TH1D;
class TH2D;
class TClonesArray;
class CbmRichRingFitterCOP;
class CbmRichRingFitterEllipseTau;
class CbmRichRing;
class CbmGeoRichPar;

#include <vector>

using namespace std;

/**
 * \class CbmRichGeoTest
 *
 * \brief RICH geometry checking and testing.
 *
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 **/
class CbmRichGeoTest : public FairTask
{

public:
   /**
    * \brief Standard constructor.
    */
   CbmRichGeoTest();

   /**
    * \brief Standard destructor.
    */
   virtual ~CbmRichGeoTest();

   /**
    * \brief SetParContainers.
    */
   void SetParContainers();

   /**
    * \brief Inherited from FairTask.
    */
   virtual InitStatus Init();

   /**
    * \brief Inherited from FairTask.
    */
   virtual void Exec(
		   Option_t* option);

   /**
    * \brief Inherited from FairTask.
    */
   virtual void Finish();

private:
   /**
    * \brief Loop over all rings in array and fill ring parameters histograms.
    */
	void RingParameters();
	
   /**
    * \brief Fit ring using ellipse fitter and fill histograms.
    * \param[in] histIndex Fitting type index, 0 - hit fitting, 1 - MC points fitting.
    * \param[in] ring Pointer to CbmRichRing to be fitted and filled in histograms.
    * \param[in] momentum MC momentum of particle produced ring.
    * \param[in] x,y X and Y coordinates of MC point (is used only in case of MC points fitting).
    */
	void FitAndFillHistEllipse(
	      Int_t histIndex,
	      CbmRichRing* ring,
	      Double_t momentum,
	      const vector<Double_t>& x,
	      const vector<Double_t>& y);

   /**
    * \brief Fit ring using circle fitter and fill histograms.
    * \param[in] histIndex Fitting type index, 0 - hit fitting, 1 - MC points fitting.
    * \param[in] ring Pointer to CbmRichRing to be fitted and filled in histograms.
    * \param[in] momentum MC momentum of particle produced ring.
    * \param[in] x,y X and Y coordinates of MC point (is used only in case of MC points fitting).
    */
   void FitAndFillHistCircle(
         Int_t histIndex,
         CbmRichRing* ring,
         Double_t momentum,
         const vector<Double_t>& x,
         const vector<Double_t>& y);

   /**
    * \brief Calculate difference between ellipse parameters
    *  for two fitting using hits and MC points for fit and fill
    *  corresponding histograms.
    * \param[in] ring Ring fitted using hits.
    * \param[in] ringMc Ring fitted using MC points
    */
   void FillMcVsHitFitEllipse(
         CbmRichRing* ring,
         CbmRichRing* ringMc);

   /**
    * \brief Calculate difference between circle parameters
    *  for two fitting using hits and MC points for fit and fill
    *  corresponding histograms.
    * \param[in] ring Ring fitted using hits.
    * \param[in] ringMc Ring fitted using MC points
    */
   void FillMcVsHitFitCircle(
         CbmRichRing* ring,
         CbmRichRing* ringMc);

   /**
    * \brief Calculate residuals between hits and Mc points and fill histograms.
    */
   void HitsAndPoints();

	void Draw();

	TClonesArray* fRichHits;
	TClonesArray* fRichRings;
	TClonesArray* fRichPoints; 
	TClonesArray* fMCTracks;
	TClonesArray* fRichRingMatches; 
	
   Double_t fDetZOrig; // X-coordinate of photodetector (original from parameter file)
   Double_t fTheta; // angle by which photodetector was tilted around x-axis
   Double_t fPhi; // angle by which photodetector was tilted around y-axis
   TObjArray* fSensNodes;
   TObjArray* fPassNodes;
   CbmGeoRichPar* fPar;

   // rings will be fitted on a fly
	CbmRichRingFitterCOP* fCopFit;
	CbmRichRingFitterEllipseTau* fTauFit;

	Int_t fEventNum;

	vector<TH1D*> fhNofHits; // number of hits per ring
	// ellipse fitting parameters
	vector<TH2D*> fhAaxisVsMom; // major axis (A) vs. MC momentum
	vector<TH2D*> fhBaxisVsMom; // minor axis (B) vs. MC momentum
	vector<TH1D*> fhBoverA; // B/A distribution
	vector<TH2D*> fhXcYcEllipse; // (Xc, Yc) of ellipse center
   // circle fitting parameters
	vector<TH2D*> fhXcYcCircle; // (Xc, Yc) of circle center
	vector<TH2D*> fhRadiusVsMom; // circle radius vs. MC momentum
	
	// Difference between Mc Points and Hits fit
   // ellipse fitting parameters
   TH2D* fhDiffAaxis; // major axis (A)
   TH2D* fhDiffBaxis; // minor axis (B)
   TH2D* fhDiffXcEllipse; // Xc of ellipse center
   TH2D* fhDiffYcEllipse; // Yc of ellipse center
   // circle fitting parameters
   TH2D* fhDiffXcCircle; // Xc of circle center
   TH2D* fhDiffYcCircle; // Xc of circle center
   TH2D* fhDiffRadius; // circle radius

   // Hits and points
   TH1D* fhDiffXhit;
   TH1D* fhDiffYhit;

   // fitting efficiency
   Double_t fMinAaxis;
   Double_t fMaxAaxis;
   Double_t fMinBaxis;
   Double_t fMaxBaxis;
   Double_t fMinRadius;
   Double_t fMaxRadius;
   TH1D* fhNofHitsAll; // distribution of the number of hits in ring for all
   TH1D* fhNofHitsCircleFit; // distribution of the number of hits in ring
                             // for good fitted rings using circle  fitting
   TH1D* fhNofHitsEllipseFit; // distribution of the number of hits in ring
                              // for good fitted rings using ellipse  fitting

   TH1D* fhNofHitsCircleFitEff;
   TH1D* fhNofHitsEllipseFitEff;

	vector<TH1*> fHists; // store all TH1 pointers of the histogram

	ClassDef(CbmRichGeoTest,1)
};

#endif

