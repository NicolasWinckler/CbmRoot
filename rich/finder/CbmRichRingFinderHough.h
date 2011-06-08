/** CbmRichRingFinderHough.h
 * @author Semen Lebedev <s.lebedev@gsi.de>
 * @since 2007
 * @version 2.0
 **/

#ifndef CBM_RICH_RING_FINDER_HOUGH_H
#define CBM_RICH_RING_FINDER_HOUGH_H

#include "CbmRichRingFinder.h"
#include "CbmRichRingFinderHoughImpl.h"

#include <vector>
#include <map>
#include <functional>

class CbmRichRingFinderHoughSimd;
class CbmRichRing;
class CbmRichRingLight;

#define HOUGH_SERIAL
//#define HOUGH_SIMD

using std::vector;

class CbmRichRingFinderHough : public CbmRichRingFinder
{
protected:
	Int_t fNEvent; // event number
	Int_t fRingCount; // number of fiund rings
	Double_t fExecTime; // evaluate execution time

// choose between serial and SIMD implementation of the ring finder
#ifdef HOUGH_SERIAL
	CbmRichRingFinderHoughImpl *fHTImpl;
#endif

#ifdef HOUGH_SIMD
	CbmRichRingFinderHoughSimd *fHTImpl;
#endif

public:
	/* Default constructor */
  	CbmRichRingFinderHough ();

   /* Standard constructor
    * @param verbose Verbosity level
    * @param geometry RICH geometry: compact or large (now it is obsolete)*/
  	CbmRichRingFinderHough (
  	      Int_t verbose,
  	      TString geometry);

   /* Destructor */
	virtual ~CbmRichRingFinderHough();

	/* Initialization at the beginning */
	virtual void Init();

	/* Finish at the end */
	virtual void Finish();

	/* Task execution each event */
	virtual Int_t DoFind(
	      TClonesArray* rHitArray,
	 		TClonesArray* rProjArray,
		   TClonesArray* rRingArray);

	/* Start ring finder
	 * @param data input data - RICH hits*/
	Int_t DoFind(
	      const vector<CbmRichHoughHit>& data);

	/* Add found rings to the output TClonesArray
	 * @param rRingArray output array
	 * @param rings Found rings */
	void AddRingsToOutputArray(
	      TClonesArray *rRingArray,
	      const vector<CbmRichRingLight*>& rings);

	ClassDef(CbmRichRingFinderHough,1)

};
#endif // CBM_RICH_RING_FINDER_HOUGH_H
