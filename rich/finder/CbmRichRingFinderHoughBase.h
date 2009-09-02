// --------------------------------------------------------------------------------------
// -----                 CbmRichRingFinderHough source file                         -----
// ----- Algorithm idea: G.A. Ososkov (ososkov@jinr.ru) and Semen Lebedev (salebedev@jinr.ru)                            -----
// ----- Implementation: Semen Lebedev (salebedev@jinr.ru)-----

#ifndef CBM_RICH_RING_FINDER_HOUGH_BASE_H
#define CBM_RICH_RING_FINDER_HOUGH_BASE_H

#include "CbmRichRing.h"
#include "CbmRichRingFinder.h"
#include "CbmRichRingFitterCOP.h"
#include "CbmRichRingFitterEllipseTau.h"
#include "CbmRichRingFitterEllipse.h"
#include "CbmRichRingSelectNeuralNet.h"
#include "TClonesArray.h"
#include "TString.h"

#include <vector>
#include <map>
#include <functional>

class CbmRichHoughHit {
public:
	Float_t fX;
	Float_t fY;
	Float_t fX2plusY2;
	unsigned short int fId;
	Bool_t fIsUsed;
};

class CbmRichHoughHitCmpUp:
       public std::binary_function<
	          const CbmRichHoughHit*,
	          const CbmRichHoughHit*,
	          bool>
{
public:
	bool operator()(const CbmRichHoughHit &m1, const CbmRichHoughHit &m2) const {
		return m1.fX < m2.fX;
	}
};


class CbmRichRingComparatorMore:
       public std::binary_function<
	          const CbmRichRing*,
	          const CbmRichRing*,
	          bool>
{
public:
	bool operator()(const CbmRichRing& ring1, const CbmRichRing& ring2) const {
		return ring1.GetSelectionNN() > ring2.GetSelectionNN();
	}
};

class CbmRichRingFinderHoughBase : public CbmRichRingFinder {

protected:
	static const unsigned short int kMAX_NOF_HITS = 65500;
	unsigned short int kMAX_NOF_HITS_IN_AREA;
	Bool_t fIsFindOptPar;

	Float_t fMaxDistance;
	Float_t fMinDistance;
    Float_t fMinDistanceSq; ///= fMinDistance*fMinDistance
    Float_t fMaxDistanceSq;

	Float_t fMinRadius;
	Float_t fMaxRadius;

	Float_t fDx;
	Float_t fDy;
	Float_t fDr;
	unsigned short int  fNofBinsX;
	unsigned short int  fNofBinsY;
	unsigned short int  fNofBinsXY;
	unsigned short int  fHTCut;
	unsigned short int  fHitCut;

	unsigned short int  fNofBinsR;
	unsigned short int  fHTCutR;
	unsigned short int  fHitCutR;

	unsigned short int fMinNofHitsInArea;

	unsigned short int fNofParts;

	Float_t fRmsCoeffEl;
	Float_t fMaxCutEl;
	Float_t fRmsCoeffCOP;
	Float_t fMaxCutCOP;

	Float_t fAnnCut;
	Float_t fUsedHitsCut;
	Float_t fUsedHitsAllCut;

	Float_t fCurMinX;
	Float_t fCurMinY;

	vector<CbmRichHoughHit> fData;  ///Rich hits
	vector<pair<Float_t, Float_t> > fTrackProj; // x and y coordinate of projected tracks

	vector<unsigned short> fHist;
	vector< vector<unsigned short> > fRingHits;
	vector<unsigned short> fHistR;
	vector< vector< unsigned short> > fRingHitsR;
    vector< vector<unsigned short> > fHitInd;

	vector<CbmRichRing> fFoundRings;///collect found rings
	Int_t fRingCount;

	Int_t fNEvent; /// event number
	CbmRichRingFitterCOP* fFitCOP;
	CbmRichRingFitterEllipseTau* fFitEllipseTau;

	CbmRichRingSelectNeuralNet* fANNSelect;

	Int_t fVerbose; /// Verbosity level
	TString fGeometryType;

	std::vector<Double_t> fExecTime;//evaluate execution time

public:
  	CbmRichRingFinderHoughBase ();

  	/** Standard constructor **/
  	CbmRichRingFinderHoughBase ( Int_t verbose, TString geometry);

	virtual ~CbmRichRingFinderHoughBase();
	void SetParameters(Int_t nofParts,
			Float_t maxDistance, Float_t minDistance,
			Float_t minRadius, Float_t maxRadius,
			Int_t HTCut, Int_t hitCut,
			Int_t HTCutR, Int_t hitCutR,
			Int_t nofBinsX, Int_t nofBinsY,
			Int_t nofBinsR, Float_t annCut,
			Float_t usedHitsCut, Float_t usedHitsAllCut,
			Float_t rmsCoeffEl, Float_t maxCutEl,
			Float_t rmsCoeffCOP, Float_t maxCutCOP);
	void SetParameters(TString geometry);

	///Calculate circle center and radius
	void CalculateRingParameters(Float_t x2y2[],
				Float_t rx[],
				Float_t ry[],
				Float_t x0,
				Float_t y0,
				Float_t *xc,
				Float_t *yc,
				Float_t *r);

	void CalculateRingParametersOld(Float_t x[],
				Float_t y[],
				Float_t *xc,
				Float_t *yc,
				Float_t *r);

	virtual void HoughTransformReconstruction(){
		;
	}
	void DefineLocalAreaAndHits(Float_t x0, Float_t y0, Int_t *indmin, Int_t *indmax);
	void HoughTransform(unsigned short int indmin, unsigned short int indmax);
	void FindPeak(Int_t indmin, Int_t indmax);
	void FindMaxBinsXYR(Int_t *maxBinXY, Int_t *maxBinR);
	void FindPrelimXYR(Float_t *xc, Float_t *yc, Float_t *r);
	void FuzzyKE(TClonesArray* rHitArray);
    void RingSelection();
    void RemoveHitsAroundEllipse(Int_t indmin, Int_t indmax, CbmRichRing * ring);
    void RemoveHitsAroundRing(Int_t indmin, Int_t indmax, CbmRichRing * ring);
    void AddRingsToOutputArray(TClonesArray *rRingArray);

	virtual void Init();

	virtual void Finish();

	virtual Int_t DoFind(TClonesArray* rHitArray,
	 		      		 TClonesArray* rProjArray,
		       	      	 TClonesArray* rRingArray);

	void SetFindOptPar(Bool_t findOptPar){fIsFindOptPar = findOptPar;}

	ClassDef(CbmRichRingFinderHoughBase,1)

};
#endif // CBM_RICH_RING_FINDER_HOUGH_BASE_H
