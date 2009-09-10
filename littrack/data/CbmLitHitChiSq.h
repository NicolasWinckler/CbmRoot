#ifndef CBMLITHITCHISQ_H_
#define CBMLITHITCHISQ_H_

class CbmLitHitChiSq
{
public:
	void SetHit(const CbmLitHit* hit) {fHit = hit;}
	void SetChiSq(myf chiSq) {fChiSq = chiSq;}
	void SetParam(const CbmLitTrackParam* param) {fParam = *param;}

	const CbmLitHit* GetHit() const {return fHit;}
	myf GetChiSq() const {return fChiSq;}
	const CbmLitTrackParam* GetParam() const {return &fParam;}
private:
	const CbmLitHit* fHit;
	myf fChiSq;
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
