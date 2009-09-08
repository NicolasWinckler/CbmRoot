// --------------------------------------------------------------------------------------
// -----                 CbmRichRingFinderHough source file                         -----
// ----- Algorithm idea: G.A. Ososkov (ososkov@jinr.ru) and Semen Lebedev (salebedev@jinr.ru)                            -----
// ----- Implementation: Semen Lebedev (salebedev@jinr.ru)-----

#ifndef CBM_RICH_RING_FINDER_HOUGH_PARALLEL_H
#define CBM_RICH_RING_FINDER_HOUGH_PARALLEL_H

#include "CbmRichRingFinderHoughImpl.h"
#include "CbmRichRing.h"
#include "CbmRichRingFinder.h"
#include "TClonesArray.h"
#include "TString.h"

#include <vector>
#include <map>
#include <functional>


class CbmRichRingFinderHoughParallel : public CbmRichRingFinder {

protected:

	Int_t fRingCount;
	Int_t fNEvent; /// event number
	Int_t fVerbose; /// Verbosity level
	TString fGeometryType;

	Double_t fExecTime;//evaluate execution time

	CbmRichRingFinderHoughImpl* fHTImpl1;
	CbmRichRingFinderHoughImpl* fHTImpl2;
public:
  	CbmRichRingFinderHoughParallel ();

  	/** Standard constructor **/
  	CbmRichRingFinderHoughParallel ( Int_t verbose, TString geometry);

	virtual ~CbmRichRingFinderHoughParallel();
	void SetParameters(TString geometry);
    void AddRingsToOutputArray(TClonesArray *rRingArray, vector<CbmRichRing>& rings);

	virtual void Init();

	virtual void Finish();

	virtual Int_t DoFind(TClonesArray* rHitArray,
	 		      		 TClonesArray* rProjArray,
		       	      	 TClonesArray* rRingArray);

	ClassDef(CbmRichRingFinderHoughParallel,1)

};
#endif // CBM_RICH_RING_FINDER_HOUGH_PARALLEL_H
