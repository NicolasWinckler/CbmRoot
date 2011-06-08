/** CbmRichRingFinderHoughImpl.h
 * @author Semen Lebedev <s.lebedev@gsi.de>
 * @since 2008
 * @version 2.0
 **/

#ifndef CBM_RICH_RING_FINDER_HOUGH_IMPL_H
#define CBM_RICH_RING_FINDER_HOUGH_IMPL_H

#include "CbmRichRingLight.h"
#include "TObject.h"

#include <vector>
#include <map>
#include <functional>

class TClonesArray;
class CbmRichRing;
class CbmRichRingFitterCOPLight;
class CbmRichRingSelectNeuralNet;

class CbmRichHoughHit {
public:
	CbmRichHitLight fHit;
	Float_t fX2plusY2;
   UShort_t fId;
	Bool_t fIsUsed;
};

class CbmRichHoughHitCmpUp:
       public std::binary_function<
	          const CbmRichHoughHit,
	          const CbmRichHoughHit,
	          bool>
{
public:
	bool operator()(const CbmRichHoughHit &m1, const CbmRichHoughHit &m2) const {
		return m1.fHit.fX < m2.fHit.fX;
	}
};

class CbmRichRingComparatorMore:
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

class CbmRichRingFinderHoughImpl{

protected:
	UShort_t kMAX_NOF_HITS; // maximum number of hits in RICH detector

// parameters of the Hough Transform algorithm

	UShort_t fNofParts; // number of groups of hits during HT

	Float_t fMaxDistance; // maximum distance between two hits
	Float_t fMinDistance; // minimum distance between two hits
   Float_t fMinDistanceSq; // = fMinDistance*fMinDistance
   Float_t fMaxDistanceSq; // = fMaxDistance*fMaxDistance

	Float_t fMinRadius; // minimum radius of the ring
	Float_t fMaxRadius; // maximum radius of the ring

	Float_t fDx; // x bin width of the ring center histogram
	Float_t fDy; // y bin width of the ring center histogram
	Float_t fDr; // width of the ring radius histogram
	UShort_t fNofBinsX; // number of bins in X direction
	UShort_t fNofBinsY; // number of bins in Y direction
	UShort_t fNofBinsXY; // fNofBinsX*fNofBinsY

	UShort_t fHTCut; // cut number of entries in maximum bin of XY histogram
	UShort_t fHitCut; // cut on hit contribution in maximum bin of XY histogram

	UShort_t fNofBinsR;// number of bins in radius histogram
	UShort_t fHTCutR; // cut number of entries in maximum bin of Radius histogram
	UShort_t fHitCutR; // cut on hit contribution in maximum bin of Radius histogram

	UShort_t fMinNofHitsInArea; // minimum number of hits in the local area

	Float_t fRmsCoeffEl;
	Float_t fMaxCutEl;
	Float_t fRmsCoeffCOP;
	Float_t fMaxCutCOP;

	Float_t fAnnCut; //remove found hits only for good quality rings
	Float_t fUsedHitsAllCut; // percent of used hits

	Float_t fCurMinX; // current minimum X position of the local area
	Float_t fCurMinY; // current minimum Y position of the local area

	std::vector<CbmRichHoughHit> fData; // Rich hits
	std::vector<CbmRichHoughHit> fDataPart1; // Rich hits
	std::vector<CbmRichHoughHit> fDataPart2; // Rich hits

	std::vector<UShort_t> fHist; // XY histogram
	std::vector<UShort_t> fHistR; // Radius histogram
	std::vector< std::vector<UShort_t> > fHitInd;

	std::vector<CbmRichRingLight*> fFoundRings; //collect found rings

	CbmRichRingFitterCOPLight* fFitCOP; // COP ring fitter
	CbmRichRingSelectNeuralNet* fANNSelect; // ANN selection criteria

public:
	/* Standard constructor */
  	CbmRichRingFinderHoughImpl ();

   /* Distructor */
	virtual ~CbmRichRingFinderHoughImpl();

	/* Set parameters of the algorithm*/
	void SetParameters();

	/* Calculate circle center and radius
	 * @param x[] array of 3 X positions
	 * @param y[] array of 3 Y positions
	 * @param xc OUTPUT X coordinate of the ring center
	 * @param yc OUTPUT Y coordinate of the ring center
	 * @param r OUTPUT ring radius*/
	void CalculateRingParameters(
	      Float_t x[],
			Float_t y[],
			Float_t *xc,
			Float_t *yc,
			Float_t *r);

	/* Runs HT for each hit*/
	virtual void HoughTransformReconstruction();

	/* Find hits in the local area
	 * @param x0 X coordinate of the local area center
	 * @param y0 Y coordinate of the local area center
	 * @param indmin OUTPUT minimum index of the hit in local area
	 * @param indmax OUTPUT maximum index of the hit in local area*/
	virtual void DefineLocalAreaAndHits(
	      Float_t x0,
	      Float_t y0,
	      Int_t *indmin,
	      Int_t *indmax);

	/* runs HoughTransformGroup for each group of hits
	 * @param indmin minimum index of the hit in local area
	 * @param indmax maximum index of the hit in local area*/
	virtual void HoughTransform(
	      unsigned short int indmin,
			unsigned short int indmax);

	/* main procedure for Hough Transform
	 * @param indmin minimum index of the hit in local area
    * @param indmax maximum index of the hit in local area
    * @param iPart index of the hit group*/
	virtual void HoughTransformGroup(
	      unsigned short int indmin,
			unsigned short int indmax,
			Int_t iPart);

	/* Find peak in the HT histograms */
	void FindPeak(
	      Int_t indmin,
	      Int_t indmax);

	/* Ring selection procedure */
   void RingSelection();

    void ReAssingSharedHits(
          Int_t ringInd1,
          Int_t ringInd2);

    void ReAssingSharedHitsEllipse(
          Int_t ringInd1,
          Int_t ringInd2);

    Int_t GetHitIndex(
          UShort_t hitInd);

	void FuzzyKE(
	      TClonesArray* rHitArray);

    void RemoveHitsAroundEllipse(
          Int_t indmin,
          Int_t indmax,
          CbmRichRing * ring);

    void RemoveHitsAroundRing(
          Int_t indmin,
          Int_t indmax,
          CbmRichRingLight* ring);

	void Init();

	void DoFind();

	void SetData(
	      const std::vector<CbmRichHoughHit>& data)
	{
		fData.clear();
		fData = data;
	}

	std::vector<CbmRichRingLight*>& GetFoundRings()
	{
		return fFoundRings;
	}
};
#endif // CBM_RICH_RING_FINDER_HOUGH_IMPL_H
