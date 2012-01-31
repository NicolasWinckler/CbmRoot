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
class TH3D;
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
    * \brief Fill MC histogram for detector acceptance calculation.
    */
   void FillMcHist();

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
    * \brief Calculate residuals between hits and MC points and fill histograms.
    */
   void HitsAndPoints();

   /**
    * \brief Create histogram: RICH detector acceptance vs.
    * minimum required number of hits in ring
    */
	TH1D* CreateAccVsMinNofHitsHist();

   /**
    *  \brief Draw histograms.
    */
	void DrawHist();

	/**
	 * \brief Draw ring in separate TCanvas.
	 * \param[in] ring Ring to be drawn.
	 * \param[in] x MCPoints X coordinates.
	 * \param[in] y MCPoints Y coordinates.
	 */
	void DrawRing(
	      CbmRichRing* ring,
	      const vector<Double_t>& x,
	      const vector<Double_t>& y);

   /**
    * \brief Print out final results.
    */
	void PrintStatisctics();

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
	Int_t fMinNofHits; // Min number of hits in ring for detector acceptance calculation.

   // fitting parameters
	// [0] = hits fit, [1] = MC points fit
	vector<TH1D*> fhNofHits; // number of hits per ring
	// for ellipse
	vector<TH2D*> fhAaxisVsMom; // major axis (A) vs. MC momentum
	vector<TH2D*> fhBaxisVsMom; // minor axis (B) vs. MC momentum
	vector<TH1D*> fhBoverA; // B/A distribution
	vector<TH2D*> fhXcYcEllipse; // (Xc, Yc) of ellipse center
   vector<TH1D*> fhChi2Ellipse; // Chi2
   // for circle
	vector<TH2D*> fhXcYcCircle; // (Xc, Yc) of circle center
	vector<TH2D*> fhRadiusVsMom; // circle radius vs. MC momentum
   vector<TH1D*> fhChi2Circle; // chi2
   vector<TH1D*> fhDRCircle; // dR

   // R, A, B distribution for different number of hits from 0 to 40
   TH2D* fhRadiusVsNofHits;
   TH2D* fhAaxisVsNofHits;
   TH2D* fhBaxisVsNofHits;

	// Difference between MC Points and Hits fit
   // for ellipse fitting parameters
   TH2D* fhDiffAaxis; // major axis (A)
   TH2D* fhDiffBaxis; // minor axis (B)
   TH2D* fhDiffXcEllipse; // Xc of ellipse center
   TH2D* fhDiffYcEllipse; // Yc of ellipse center
   // for circle fitting parameters
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

   // Detector acceptance vs pt, y, p
   TH3D* fhMc3D;
   TH3D* fhAcc3D;

   // numbers in dependence on XY position onto the photodetector
   TH2D* fhNofHitsXY;
   TH2D* fhNofPointsXY;
   TH2D* fhBoverAXY;
   TH2D* fhBaxisXY;
   TH2D* fhAaxisXY;
   TH2D* fhRadiusXY;
   TH2D* fhCounterXY;

	vector<TH1*> fHists; // store all TH1 pointers of the histogram

	Int_t fNofDrawnRings; // store number of drawn rings

	ClassDef(CbmRichGeoTest,1)
};

#endif

