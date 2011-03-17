/** CbmLitGating.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 1.0
 *
 * Class implements validation gate technique.
 * It optionally use fast search of hits.
 **/

#ifndef CBMLITGATING_H_
#define CBMLITGATING_H_

#include "base/CbmLitTypes.h"
#include "base/CbmLitEnums.h"

class CbmLitTrackParam;
class CbmLitHit;
class CbmLitStation;

class CbmLitGating
{
public:
	/* Constructor */
	CbmLitGating();

	/* Destructor */
	virtual ~CbmLitGating();

	/* Returns true if hit is in validation gate */
	bool IsHitInValidationGate(
			const CbmLitHit* hit,
			myf chiSq,
			const CbmLitTrackParam* par) const;

	/* Returns pair of hit iterators which define maximum and
	 * minimum possible size of the validation gate.
	 * It is used in the fast hit search algorithm. */
	HitPtrIteratorPair MinMaxIndex(
			const CbmLitTrackParam* par,
			HitPtrIteratorPair hits,
			const CbmLitStation& station,
			const std::pair<myf, char>& maxErr) const;

	/* Sets sigma value */
    void SetSigmaCoef(myf sigmaCoef) { fSigmaCoef = sigmaCoef;}

    /* Sets chi-square cut for strip hits */
    void SetChiSqStripHitCut(myf chiSqStripHitCut) { fChiSqStripHitCut = chiSqStripHitCut;}

    /* Sets chi-square cut for pixel hits */
    void SetChiSqPixelHitCut(myf chiSqPixelHitCut) { fChiSqPixelHitCut = chiSqPixelHitCut;}

    /* Set use fast hit search */
    void IsUseFastSearch(bool useFastSearch) { fUseFastSearch = useFastSearch;}

    /* Returns sigma value */
    myf GetSigmaCoef() const { return fSigmaCoef;}

    /* Returns chi-square cut for strip hits */
    myf GetChiSqStripHitCut() const { return fChiSqStripHitCut;}

    /* Returns chi-square cut for pixel hits */
    myf GetChiSqPixelHitCut() const { return fChiSqPixelHitCut;}

    /* Set use fast hit search */
    bool IsUseFastSearch() const { return fUseFastSearch;}

protected:
    /* Chi-square cut for strip hits */
	myf fChiSqStripHitCut;
	/* Chi-square cut for pixel hits */
	myf fChiSqPixelHitCut;
	/* If true than fast search of hits will be used */
	bool fUseFastSearch;
	/* Sigma coefficient for fast hit search */
	myf fSigmaCoef;
	/* Maximum squared position error */
	myf fMaxCovSq;
};

#endif /* CBMLITGATING_H_ */
