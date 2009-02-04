#ifndef CBMLITTRACKBRANCH_H_
#define CBMLITTRACKBRANCH_H_

#include "CbmLitTypes.h"

#include "TObject.h"

#include <string>

class CbmLitTrackParam;
class CbmLitHit;

class CbmLitTrackBranch
{
public:
	CbmLitTrackBranch();
	virtual ~CbmLitTrackBranch();

	Int_t GetNofHits() const { return fHits.size(); }
	Double_t GetChi2() const { return fChi2; }
	Int_t GetNDF() const { return fNDF; }
	const CbmLitTrackParam* GetParam() const { return &fParam; }
	const CbmLitHit* GetHit(Int_t index) const { return fHits[index];}
	const HitPtrVector& GetHits() const { return fHits;}

	void SetChi2(Double_t chi2) { fChi2 = chi2; }
	void SetNDF(Int_t ndf) { fNDF = ndf; }
	void SetParam(const CbmLitTrackParam* par) { fParam = *par; }
	void AddHit(CbmLitHit* hit);
	void SortHits(Bool_t downstream = true);

	std::string ToString() const;

private:
	HitPtrVector fHits;
	CbmLitTrackParam fParam;
	Double_t fChi2;
	Int_t fNDF;

	ClassDef(CbmLitTrackBranch, 1);
};

#endif /*CBMLITTRACKBRANCH_H_*/
