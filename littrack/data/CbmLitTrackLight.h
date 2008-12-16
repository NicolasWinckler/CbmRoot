#ifndef CBMLITTRACKLIGHT_H_
#define CBMLITTRACKLIGHT_H_

#include "TObject.h"

class CbmLitTrackLight
{
public:
	CbmLitTrackLight();
	virtual ~CbmLitTrackLight();
	
	Int_t GetNofHits() const { return fHits.size(); }
	Double_t GetChi2() const { return fChi2; }
	Int_t GetNDF() const { return fNDF; }
	const CbmLitTrackParam* GetParam() const { return &fParam; }
	const CbmLitHit* GetHit(Int_t index) const { return fHits[index];}
	const HitPtrVector& GetHits() const { return fHits;}

	void SetChi2(Double_t chi2) { fChi2 = chi2; }
	void SetNDF(Int_t ndf) { fNDF = ndf; }
	void SetParam(const CbmLitTrackParam* par) { fParam = *par; }
	void AddHit(const CbmLitHit* hit);
	void SortHits(Bool_t downstream = true);
		
	std::string ToString() const;

private:
	HitPtrVector fHits;
	CbmLitTrackParam fParam;
	Double_t fChi2;
	Int_t fNDF;
	
	ClassDef(CbmLitTrackLight, 1);
};

#endif /*CBMTRACKCANDIDATE_H_*/
