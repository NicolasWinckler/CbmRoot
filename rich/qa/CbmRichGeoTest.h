 
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

class CbmRichGeoTest : public FairTask
{

public:
   CbmRichGeoTest();

   CbmRichGeoTest(
	      const char *name,
	      const char *title,
	      Int_t verbose);

   virtual ~CbmRichGeoTest();

   void SetParContainers();

   virtual InitStatus Init();

   virtual void Exec(
		   Option_t* option);

   virtual void Finish();

private:

	void RingParameters();
	
	void FitAndFillHistEllipse(
	      Int_t histIndex,
	      CbmRichRing* ring,
	      Double_t momentum,
	      const vector<Double_t>& x,
	      const vector<Double_t>& y);

   void FitAndFillHistCircle(
         Int_t histIndex,
         CbmRichRing* ring,
         Double_t momentum,
         const vector<Double_t>& x,
         const vector<Double_t>& y);

   void FillMcVsHitFitEllipse(
         CbmRichRing* ring,
         CbmRichRing* ringMc);

   void FillMcVsHitFitCircle(
         CbmRichRing* ring,
         CbmRichRing* ringMc);

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
   TH1D* fhNofHitsAll;
   TH1D* fhNofHitsCircleFit;
   TH1D* fhNofHitsEllipseFit;
   TH1D* fhNofHitsCircleFitEff;
   TH1D* fhNofHitsEllipseFitEff;

	vector<TH1*> fHists; // store all TH1 pointers of the histogramm

	ClassDef(CbmRichGeoTest,1)
};

#endif

