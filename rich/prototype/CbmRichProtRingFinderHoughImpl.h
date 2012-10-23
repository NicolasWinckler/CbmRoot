// --------------------------------------------------------------------------------------
// -----                 CbmRichProtRingFinderHoughImpl source file                         -----
// ----- Algorithm idea: G.A. Ososkov (ososkov@jinr.ru) and Semen Lebedev (s.lebedev@gsi.de)                            -----
// ----- Implementation: Semen Lebedev (s.lebedev@gsi.de)-----

#ifndef CBM_RICH_PROT_RING_FINDER_HOUGH_IMPL_H
#define CBM_RICH_PROT_RING_FINDER_HOUGH_IMPL_H

#include "CbmRichRingLight.h"
#include "CbmRichRing.h"
//#include "CbmRichRingFitterEllipseTau.h"
#include "CbmRichRingSelectAnn.h"
#include "TClonesArray.h"
#include "TString.h"

#include <vector>
#include <map>
#include <functional>

class CbmRichRingFitterCOP;

class CbmRichProtHoughHit {
public:
	CbmRichHitLight fHit;
	Float_t fX2plusY2;
        UShort_t fId;
	Bool_t fIsUsed;
   
 CbmRichProtHoughHit() : fHit(), fX2plusY2(0.), fId(0), fIsUsed(false) {}
      
};

class CbmRichProtHoughHitCmpUp:
       public std::binary_function<
	          const CbmRichProtHoughHit,
	          const CbmRichProtHoughHit,
	          bool>
{
public:
	bool operator()(const CbmRichProtHoughHit &m1, const CbmRichProtHoughHit &m2) const {
		return m1.fHit.fX < m2.fHit.fX;
	}
};

class CbmRichProtRingComparatorMore:
       public std::binary_function<
	          const CbmRichRingLight*,
	          const CbmRichRingLight*,
	          bool>
{
public:
	bool operator()(const CbmRichRingLight* ring1, const CbmRichRingLight* ring2) const {
		return ring1->GetSelectionNN() > ring2->GetSelectionNN();
	}
};

class CbmRichProtRingFinderHoughImpl{

protected:
	UShort_t kMAX_NOF_HITS;
	UShort_t fNofParts;

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

	Float_t fRmsCoeffEl;
	Float_t fMaxCutEl;
	Float_t fRmsCoeffCOP;
	Float_t fMaxCutCOP;

	Float_t fAnnCut;
	Float_t fUsedHitsCut;
	Float_t fUsedHitsAllCut;

	Float_t fCurMinX;
	Float_t fCurMinY;

	std::vector<CbmRichProtHoughHit> fData;  ///Rich hits
	std::vector<CbmRichProtHoughHit> fDataPart1;  ///Rich hits
	std::vector<CbmRichProtHoughHit> fDataPart2;  ///Rich hits

	std::vector<UShort_t> fHist;
	std::vector<UShort_t> fHistR;
	std::vector< std::vector<UShort_t> > fHitInd;

	std::vector<CbmRichRingLight*> fFoundRings;///collect found rings

	CbmRichRingFitterCOP* fFitCOP;
	CbmRichRingSelectAnn* fANNSelect;
	//CbmRichRingFitterEllipseTau* fFitEllipse;

	TString fGeometryType;

public:
  	CbmRichProtRingFinderHoughImpl ();

  	/** Standard constructor **/
  	CbmRichProtRingFinderHoughImpl (TString geometry);

	virtual ~CbmRichProtRingFinderHoughImpl();
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

	void SetData(const std::vector<CbmRichProtHoughHit>& data){
		fData.clear();
		fData = data;
	}

	std::vector<CbmRichRingLight*>& GetFoundRings(){
		return fFoundRings;
	}

private:
   CbmRichProtRingFinderHoughImpl(const CbmRichProtRingFinderHoughImpl&);
   CbmRichProtRingFinderHoughImpl operator=(const CbmRichProtRingFinderHoughImpl&);

};
#endif // CBM_RICH_RING_FINDER_HOUGH_IMPL_H
