// --------------------------------------------------------------------------------------
// -----                 CbmRichRingFinderHough source file                         -----
// ----- Algorithm idea: G.A. Ososkov (ososkov@jinr.ru) and Semen Lebedev (s.lebedev@gsi.de)                            -----
// ----- Implementation: Semen Lebedev (s.lebedev@gsi.de)-----

#ifndef CBM_RICH_RING_FINDER_HOUGH_H
#define CBM_RICH_RING_FINDER_HOUGH_H

#include "CbmRichRingFinder.h"

#include "TString.h"

#include <vector>
#include <map>
#include <functional>

class CbmRichRingFinderHoughImpl;
class CbmRichRingFinderHoughSimd;
class CbmRichRing;

class CbmRichRingFinderHough : public CbmRichRingFinder {

protected:
	Int_t fNEvent; /// event number
	Bool_t fIsFindOptPar;
	Int_t fVerbose; /// Verbosity level
	TString fGeometryType;
	Int_t fRingCount;
	CbmRichRingFinderHoughImpl *fHTImpl;
	//CbmRichRingFinderHoughSimd *fHTImpl;

	Double_t fExecTime;//evaluate execution time

public:
  	CbmRichRingFinderHough ();

  	/** Standard constructor **/
  	CbmRichRingFinderHough ( Int_t verbose, TString geometry);

	virtual ~CbmRichRingFinderHough();
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

    void AddRingsToOutputArray(TClonesArray *rRingArray,std::vector<CbmRichRing*>& rings);

	virtual void Init();

	virtual void Finish();

	virtual Int_t DoFind(TClonesArray* rHitArray,
	 		      		 TClonesArray* rProjArray,
		       	      	 TClonesArray* rRingArray);

	void SetFindOptPar(Bool_t findOptPar){fIsFindOptPar = findOptPar;}

	ClassDef(CbmRichRingFinderHough,1)

};
#endif // CBM_RICH_RING_FINDER_HOUGH_H
