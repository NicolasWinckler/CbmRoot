/** CbmLitHitChiSq.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 1.0
 **
 ** Class for temporary storage of hit and
 ** chi-square information.
 **/

#ifndef CBMLITHITCHISQ_H_
#define CBMLITHITCHISQ_H_

#include "base/CbmLitFloat.h"
#include "data/CbmLitTrackParam.h"

class CbmLitHit;

class CbmLitHitChiSq
{
public:
	/* Constructor */
	CbmLitHitChiSq():
		fHit(NULL),
		fChiSq(0.),
		fParam(){}

	/* Destructor */
	~CbmLitHitChiSq() {}

	/* Sets hit pointer */
	void SetHit(const CbmLitHit* hit) {fHit = hit;}

	/* Sets chi-square value */
	void SetChiSq(myf chiSq) {fChiSq = chiSq;}

	/* Set track parameter pointer */
	void SetParam(const CbmLitTrackParam* param) {fParam = *param;}

	/* Returns hit pointer */
	const CbmLitHit* GetHit() const {return fHit;}

	/* Returns chi-square value */
	myf GetChiSq() const {return fChiSq;}

	/* Returns track parameters pointer */
	const CbmLitTrackParam* GetParam() const {return &fParam;}

private:
	/* Pointer to the hit */
	const CbmLitHit* fHit;
	/* Chi-square value */
	myf fChiSq;
	/* Pointer to the track parameters */
	CbmLitTrackParam fParam;
};

class CompareHitChiSqLess:
	public std::binary_function<
		const CbmLitHitChiSq&,
		const CbmLitHitChiSq&,
		bool>
{
public:
	bool operator()(const CbmLitHitChiSq& hit1, const CbmLitHitChiSq& hit2) const {
		return hit1.GetChiSq() < hit2.GetChiSq();
	}
};

#endif /* CBMLITHITCHISQ_H_ */
