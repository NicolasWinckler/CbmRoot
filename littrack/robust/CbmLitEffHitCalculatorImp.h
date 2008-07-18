#ifndef CBMLITEFFHITCALCULATORIMP_H_
#define CBMLITEFFHITCALCULATORIMP_H_

#include "CbmLitEffHitCalculator.h"
#include "CbmLitTypes.h"

class CbmLitEffHitCalculatorImp : public CbmLitEffHitCalculator
{
public:
	CbmLitEffHitCalculatorImp();
	virtual ~CbmLitEffHitCalculatorImp();
	
	virtual LitStatus Initialize();
	virtual LitStatus Finalize();
	
	virtual CbmLitHit DoCalculate(
			HitIterator itBegin,
			HitIterator itEnd);
	
	virtual CbmLitHit DoCalculate(
			HitVector& hits);
private:
	
	inline void Inverse(
			Double_t v00, Double_t v01, Double_t v11,
			Double_t& u00, Double_t& u01, Double_t& u11);
	
	ClassDef(CbmLitEffHitCalculatorImp, 1);
};

#endif /*CBMLITEFFHITCALCULATORIMP_H_*/
