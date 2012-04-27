/**
* \file CbmRichGeoTest.h
*
* \brief RICH geometry checking and testing.
*
* \author Semen Lebedev <s.lebedev@gsi.de>
* \date 2011
**/

#ifndef CBMRICHGEOTEST
#define CBMRICHGEOTEST

#include "FairTask.h"

class TH1D;
class TH2D;
class TH3D;
class TClonesArray;
class CbmRichRingFitterCOP;
class CbmRichRingFitterEllipseTau;
class CbmRichRing;
class CbmRichRingLight;
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

   /**
    * \brief Creates summary report for different simulations. This is just an
    * interface. All functionality is implemented in CbmRichGeoTestStudyReport class.
    * \param[in] title Report title.
    * \param[in] resultDirectories Paths to directories with results for each simulation.
    * \param[in] studyNames Name of each study.
    * \param[in] outputDir Path to the output directory.
    */
   void CreateStudyReport(
         const string& title,
         const vector<string>& resultDirectories,
         const vector<string>& studyNames,
         const string& outputDir);

   /**
    * \brief Set output directory where you want to write results (figures and json).
    * \param[in] dir Path to the output directory.
    */
   void SetOutputDir(const string& dir) {fOutputDir = dir;}

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
    */
	void FitAndFillHistEllipse(
	      Int_t histIndex,
	      CbmRichRingLight* ring,
	      Double_t momentum);

   /**
    * \brief Fit ring using circle fitter and fill histograms.
    * \param[in] histIndex Fitting type index, 0 - hit fitting, 1 - MC points fitting.
    * \param[in] ring Pointer to CbmRichRingLight to be fitted and filled in histograms.
    * \param[in] momentum MC momentum of particle produced ring.
    */
   void FitAndFillHistCircle(
         Int_t histIndex,
         CbmRichRingLight* ring,
         Double_t momentum);

   /**
    * \brief Calculate difference between ellipse parameters
    *  for two fitting using hits and MC points for fit and fill
    *  corresponding histograms.
    * \param[in] ring Ring fitted using hits.
    * \param[in] ringMc Ring fitted using MC points
    */
   void FillMcVsHitFitEllipse(
         CbmRichRingLight* ring,
         CbmRichRingLight* ringMc);

   /**
    * \brief Calculate difference between circle parameters
    *  for two fittings using hits and MC points for fit and fill
    *  corresponding histograms.
    * \param[in] ring Ring fitted using hits.
    * \param[in] ringMc Ring fitted using MC points
    */
   void FillMcVsHitFitCircle(
         CbmRichRingLight* ring,
         CbmRichRingLight* ringMc);

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
	 * \param[in] ring Ring with RICH hits.
	 * \param[in] ringPoint Ring with MC RICH points.
	 */
	void DrawRing(
	      CbmRichRingLight* ringHit,
	      CbmRichRingLight* ringPoint);

	/**
	 * \brief Create property tree an store different statistics there.
	 */
	void CreatePTree();

   /**
    * \brief Copy constructor.
    */
   CbmRichGeoTest(const CbmRichGeoTest&);

   /**
    * \brief Assignment operator.
    */
   CbmRichGeoTest& operator=(const CbmRichGeoTest&);

   /**
    * \brief Calculate efficiency.
    */
   string CalcEfficiency(
      TH1* histRec,
      TH1* histAcc);

   /**
    * \fn DivideH1
    * \brief Divide two histograms.
    * \param[in] h1 Pointer to the first histogram.
    * \param[in] h2 Pointer to the second histogram.
    * \param[in] name Name of a new histogram.
    * \param[in] title Histogram title.
    * \param[in] axisX X axis title.
    * \param[in] axisY Y axis title.
    */
   TH1D* DivideH1(
      TH1D* h1,
      TH1D* h2,
      const string& name,
      const string& title,
      const string& axisX,
      const string& axisY);

   string fOutputDir; // output dir for results

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

	TH2D* fhHitsXY; // distribution of X and Y position of hits
	TH2D* fhPointsXY; // distribution of X and Y position of points


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
   TH2D* fhNofHitsXY; // number of hits
   TH2D* fhNofPointsXY; // number of points
   TH2D* fhBoverAXY; // B/A ratio
   TH2D* fhBaxisXY; // B axis
   TH2D* fhAaxisXY; // A axis
   TH2D* fhRadiusXY; // Radius
   TH2D* fhCounterXY; // counter for mean value calculation

	vector<TH1*> fHists; // store all TH1 pointers of the histogram

	Int_t fNofDrawnRings; // store number of drawn rings

	ClassDef(CbmRichGeoTest,1)
};

#endif

