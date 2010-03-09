// --------------------------------------------------------------------------------------
// -----                 CbmRichRingFinderHoughImpl source file                         -----
// ----- Algorithm idea: G.A. Ososkov (ososkov@jinr.ru) and Semen Lebedev (s.lebedev@gsi.de)                            -----
// ----- Implementation: Semen Lebedev (s.lebedev@gsi.de)-----

#ifndef CBM_RICH_RING_FINDER_HOUGH_IMPL_H
#define CBM_RICH_RING_FINDER_HOUGH_IMPL_H

#include "CbmRichRing.h"
#include "CbmRichRingFinder.h"
#include "CbmRichRingFitterCOP.h"
#include "CbmRichRingFitterEllipseTau.h"
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
    UShort_t fId;
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
	bool operator()(const CbmRichRingLight* ring1, const CbmRichRingLight* ring2) const {
		return ring1->GetSelectionNN() > ring2->GetSelectionNN();
	}
};

class CbmRichRingFinderHoughImpl{

protected:
	static const UShort_t kMAX_NOF_HITS = 65500;

	Float_t fMaxDistance;
	Float_t fMinDistance;
    Float_t fMinDistanceSq; ///= fMinDistance*fMinDistance
    Float_t fMaxDistanceSq;

	Float_t fMinRadius;
	Float_t fMaxRadius;

	Float_t fDx;
	Float_t fDy;
	Float_t fDr;
	UShort_t  fNofBinsX;
	UShort_t  fNofBinsY;
	UShort_t  fNofBinsXY;
	UShort_t  fHTCut;
	UShort_t  fHitCut;

	UShort_t  fNofBinsR;
	UShort_t  fHTCutR;
	UShort_t  fHitCutR;

	UShort_t fMinNofHitsInArea;

	UShort_t fNofParts;

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

	vector<UShort_t> fHist;
	vector<UShort_t> fHistR;
    vector< vector<UShort_t> > fHitInd;

	vector<CbmRichRingLight*> fFoundRings;///collect found rings

	CbmRichRingFitterCOP* fFitCOP;
	CbmRichRingSelectNeuralNet* fANNSelect;
	CbmRichRingFitterEllipseTau* fFitEllipse;

	TString fGeometryType;

public:
  	CbmRichRingFinderHoughImpl ();

  	/** Standard constructor **/
  	CbmRichRingFinderHoughImpl (TString geometry);

	virtual ~CbmRichRingFinderHoughImpl();
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

	virtual void HoughTransformReconstruction();
	virtual void DefineLocalAreaAndHits(Float_t x0, Float_t y0, Int_t *indmin, Int_t *indmax);
	virtual void HoughTransform(unsigned short int indmin,
			unsigned short int indmax);
	virtual void HoughTransformGroup(unsigned short int indmin,
			unsigned short int indmax, Int_t iPart);
	void FindPeak(Int_t indmin, Int_t indmax);
	void FuzzyKE(TClonesArray* rHitArray);
    void RingSelection();
    void ReAssingSharedHits(Int_t ringInd1, Int_t ringInd2);
    void ReAssingSharedHitsEllipse(Int_t ringInd1, Int_t ringInd2);
    Int_t GetHitIndex(UShort_t hitInd);
    Bool_t AreRingsCloseEnough(CbmRichRingLight* ring1, CbmRichRingLight* ring2);
    void RemoveHitsAroundEllipse(Int_t indmin, Int_t indmax, CbmRichRing * ring);
    void RemoveHitsAroundRing(Int_t indmin, Int_t indmax, CbmRichRingLight* ring);
    void InitHist();
	void Init();
	void DoFind();

	void SetData(const vector<CbmRichHoughHit>& data){
		fData.clear();
		fData = data;
	}

	vector<CbmRichRingLight*>& GetFoundRings(){
		return fFoundRings;
	}

	//ClassDef(CbmRichRingFinderHoughImpl,1)

};
#endif // CBM_RICH_RING_FINDER_HOUGH_IMPL_H
