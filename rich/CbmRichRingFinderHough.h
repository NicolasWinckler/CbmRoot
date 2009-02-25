// --------------------------------------------------------------------------------------
// -----                 CbmRichRingFinderHough source file                         -----
// ----- Algorithm idea: G.A. Ososkov (ososkov@jinr.ru) and Simeon Lebedev (salebedev@jinr.ru)                            -----
// ----- Implementation: Simeon Lebedev (salebedev@jinr.ru)-----


#ifndef CBM_RICH_RING_FINDER_HOUGH_H
#define CBM_RICH_RING_FINDER_HOUGH_H

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

class CbmRichMyPoint {
public:
	Double_t fX;
	Double_t fY;
	Double_t fX2plusY2;
	Int_t fId;
	Int_t fRefIndex;
	Bool_t fIsUsed;

	static Bool_t CmpUp(const CbmRichMyPoint &m1,
			          const CbmRichMyPoint &m2){
		return m1.fX < m2.fX;
	};
	//CbmRichMyPoint{fX=fY=fId=0;}
	//~CbmRichMyPoint{;}
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

class CbmRichRingFinderHough : public CbmRichRingFinder {

	Double_t fMaxDistance;
	Double_t fMinDistance;
    Double_t fMinDistance2; ///= fMinDistance*fMinDistance
    Double_t fMaxDistance2;

	Double_t fMinRadius;
	Double_t fMaxRadius;
	Int_t fNofBinsX;
	Int_t fNofBinsY;
	Int_t fHTCut;
	Int_t fHitCut;

	Int_t fNofBinsR;
	Int_t fHTCutR;
	Int_t fHitCutR;

	Double_t fAnnCut;
	Double_t fUsedHitsCut;
	Double_t fUsedHitsAllCut;

	std::vector<CbmRichMyPoint> fData;  ///Rich hits

	std::vector< std::vector<Int_t> > fHist;
	std::vector< std::vector< std::vector<Int_t> > > fRingHits;
	std::vector< Int_t > fHistR;
	std::vector< std::vector<Int_t> > fRingHitsR;
	std::vector< std::vector<Int_t> > fFoundRingsHitId;


	std::vector<CbmRichRing> fFoundRings;///collect found rings

	Int_t fNEvent; /// event number
	CbmRichRingFitterCOP* fFitCOP;
	CbmRichRingFitterEllipseTau* fFitEllipseTau;
	//CbmRichRingFitterEllipse* fFitEllipse;

	CbmRichRingSelectNeuralNet* fANNSelect;

	Int_t fVerbose; /// Verbosity level

public:

  	/** Default constructor **/
  	CbmRichRingFinderHough();

  	/** Standard constructor **/
  	CbmRichRingFinderHough ( Int_t verbose);

	~CbmRichRingFinderHough();

	void SetParameters(TString geometry);

	///Calculate circle center and radius
	void CalculateRingParameters(Double_t x2y2[],
				     Double_t rx[],
				     Double_t ry[],
					 Double_t x0,
					 Double_t y0,
				     Double_t *xc,
				     Double_t *yc,
				     Double_t *r);

	void CalculateRingParametersOld(Double_t x[],
					Double_t y[],
					Double_t *xc,
					Double_t *yc,
					Double_t *r);

	void HoughTransformReconstruction();

	void FindPeak(Int_t indmin, Int_t indmax);
	void FindMaxBinsXYR(Int_t *maxBinX, Int_t *maxBinY, Int_t *maxBinR);
	void FuzzyKE(TClonesArray* rHitArray);
    void RingSelection();
    void RemoveHitsAroundEllipse(Int_t indmin, Int_t indmax, CbmRichRing * ring);
    void RemoveHitsAroundRing(Int_t indmin, Int_t indmax, CbmRichRing * ring);

	virtual void Init();

	virtual void Finish();

	virtual Int_t DoFind(TClonesArray* rHitArray,
	 		      		 TClonesArray* rProjArray,
		       	      	 TClonesArray* rRingArray);

	ClassDef(CbmRichRingFinderHough,1)

};
#endif // CBM_RICH_RING_FINDER_HOUGH_H
