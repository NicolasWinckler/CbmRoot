// --------------------------------------------------------------------------------------
// -----                 CbmRichRingFinderHough source file                         -----
// ----- Algorithm idea: G.A. Ososkov (ososkov@jinr.ru) and Semen Lebedev (s.lebedev@gsi.de)                            -----
// ----- Implementation: Semen Lebedev (s.lebedev@gsi.de)-----

#ifndef CBM_RICH_RING_FINDER_HOUGH_H
#define CBM_RICH_RING_FINDER_HOUGH_H

#include "CbmRichRingFinder.h"
#include "CbmRichRingLight.h"
#include "CbmRichRingFinderHoughImpl.h"


#include <vector>
#include <map>
#include <functional>

class CbmRichRingFinderHoughImpl;
class CbmRichRingFinderHoughSimd;
class CbmRichRing;

#define HOUGH_SERIAL
//#define HOUGH_SIMD


class CbmRichRingFinderHough : public CbmRichRingFinder {

protected:
	Int_t fNEvent; // event number
	Int_t fRingCount;

#ifdef HOUGH_SERIAL
	CbmRichRingFinderHoughImpl *fHTImpl;
#endif

#ifdef HOUGH_SIMD
	CbmRichRingFinderHoughSimd *fHTImpl;
#endif

	Double_t fExecTime;//evaluate execution time

public:
  	CbmRichRingFinderHough ();

  	CbmRichRingFinderHough ( Int_t verbose, TString geometry);

	virtual ~CbmRichRingFinderHough();

	void AddRingsToOutputArray(TClonesArray *rRingArray,std::vector<CbmRichRingLight*>& rings);

	virtual void Init();

	virtual void Finish();

	virtual Int_t DoFind(TClonesArray* rHitArray,
	 		      		 TClonesArray* rProjArray,
		       	      	 TClonesArray* rRingArray);

	Int_t DoFind(const std::vector<CbmRichHoughHit>& data);

	ClassDef(CbmRichRingFinderHough,1)

};
#endif // CBM_RICH_RING_FINDER_HOUGH_H
