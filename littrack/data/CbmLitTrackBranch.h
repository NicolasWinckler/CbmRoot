#ifndef CBMLITTRACKBRANCH_H_
#define CBMLITTRACKBRANCH_H_

#include "CbmLitTypes.h"

#include <string>

class CbmLitTrackParam;
class CbmLitHit;

class CbmLitTrackBranch
{
public:
	CbmLitTrackBranch();
	virtual ~CbmLitTrackBranch();

	int GetNofHits() const { return fHits.size(); }
	double GetChi2() const { return fChi2; }
	int GetNDF() const { return fNDF; }
	const CbmLitTrackParam* GetParam() const { return &fParam; }
	const CbmLitHit* GetHit(int index) const { return fHits[index];}
	const HitPtrVector& GetHits() const { return fHits;}

	void SetChi2(double chi2) { fChi2 = chi2; }
	void SetNDF(int ndf) { fNDF = ndf; }
	void SetParam(const CbmLitTrackParam* par) { fParam = *par; }
	void AddHit(CbmLitHit* hit);
	void SortHits(Bool_t downstream = true);

	std::string ToString() const;

private:
	HitPtrVector fHits;
	CbmLitTrackParam fParam;
	double fChi2;
	int fNDF;
};

#endif /*CBMLITTRACKBRANCH_H_*/
