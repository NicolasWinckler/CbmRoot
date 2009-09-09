#ifndef CBMLITGATING_H_
#define CBMLITGATING_H_

//#include "CbmLitTool.h"

#include "base/CbmLitTypes.h"
#include "base/CbmLitEnums.h"

class CbmLitTrackParam;
class CbmLitHit;
class CbmLitStation;

class CbmLitGating //: public CbmLitTool
{
public:
	CbmLitGating();
	virtual ~CbmLitGating();

	// derived from CbmTool
	virtual LitStatus Initialize();
	virtual LitStatus Finalize();

	bool IsHitInValidationGate(
			const CbmLitTrackParam* par,
			const CbmLitHit* hit) const;

	HitPtrIteratorPair MinMaxIndex(
			const CbmLitTrackParam* par,
			HitPtrIteratorPair hits,
			const CbmLitStation& station,
			const std::pair<myf, char>& maxErr) const;

    void SetSigmaCoef(myf sigmaCoef) { fSigmaCoef = sigmaCoef;}
    void SetChiSqStripHitCut(myf chiSqStripHitCut) { fChiSqStripHitCut = chiSqStripHitCut;}
    void SetChiSqPixelHitCut(myf chiSqPixelHitCut) { fChiSqPixelHitCut = chiSqPixelHitCut;}
    void IsUseFastSearch(bool useFastSearch) { fUseFastSearch = useFastSearch;}

    myf GetSigmaCoef() const { return fSigmaCoef;}
    myf GetChiSqStripHitCut() const { return fChiSqStripHitCut;}
    myf GetChiSqPixelHitCut() const { return fChiSqPixelHitCut;}
    bool IsUseFastSearch() const { return fUseFastSearch;}

private:
	myf fChiSqStripHitCut;
	myf fChiSqPixelHitCut;
	bool fUseFastSearch;
	myf fSigmaCoef;
	myf fMaxCovSq;
};

#endif /* CBMLITGATING_H_ */
