#ifndef CBMLITWEIGHTEDHITCALCULATORIMP_H_
#define CBMLITWEIGHTEDHITCALCULATORIMP_H_

//#include "CbmLitEffHitCalculator.h"
#include "CbmLitTypes.h"
class CbmLitHit;
class CbmLitStripHit;
class CbmLitPixelHit;

class CbmLitWeightedHitCalculatorImp //: public CbmLitEffHitCalculator
{
public:
	CbmLitWeightedHitCalculatorImp();
	virtual ~CbmLitWeightedHitCalculatorImp();

	virtual LitStatus Initialize();
	virtual LitStatus Finalize();

	virtual void DoCalculate(
			HitPtrIterator itBegin,
			HitPtrIterator itEnd,
			CbmLitHit* hit);

	virtual void DoCalculate(
			HitPtrVector& hits,
			CbmLitHit* hit);
private:
	void CalculateWeightedHit(
			HitPtrIterator itBegin,
			HitPtrIterator itEnd,
			CbmLitStripHit* hit);

	void CalculateWeightedHit(
			HitPtrIterator itBegin,
			HitPtrIterator itEnd,
			CbmLitPixelHit* hit);

	inline void Inverse(
			Double_t v00, Double_t v01, Double_t v11,
			Double_t& u00, Double_t& u01, Double_t& u11);

	ClassDef(CbmLitWeightedHitCalculatorImp, 1);
};

#endif /*CBMLITWEIGHTEDHITCALCULATORIMP_H_*/
