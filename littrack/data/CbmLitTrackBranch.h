#ifndef CBMLITTRACKBRANCH_H_
#define CBMLITTRACKBRANCH_H_

#include "CbmLitFloat.h"
#include "CbmLitTypes.h"
#include "CbmLitEnums.h"

#include <string>

class CbmLitTrackParam;
class CbmLitHit;

class CbmLitTrackBranch
{
public:
	CbmLitTrackBranch();
	virtual ~CbmLitTrackBranch();

	int GetNofHits() const { return fHits.size(); }
	myf GetChi2() const { return fChi2; }
	int GetNDF() const { return fNDF; }
	int GetNofMissingHits() const {return fNofMissingHits;}
	int GetRefId() const {return fRefId;}
	LitTrackQa GetQuality() const { return fQuality; }
	int GetPreviousTrackId() const { return fPreviousTrackId; }
	int GetLastPlaneId() const { return fLastPlaneId; };
	const CbmLitTrackParam* GetParam() const { return &fParam; }
	const CbmLitHit* GetHit(int index) const { return fHits[index];}
	const HitPtrVector& GetHits() const { return fHits;}


	void SetChi2(myf chi2) { fChi2 = chi2; }
	void SetNDF(int ndf) { fNDF = ndf; }
	void SetQuality(LitTrackQa quality) { fQuality = quality; }
	void SetNofMissingHits(int nofMissingHits) {fNofMissingHits = nofMissingHits;}
	void SetRefId(int refId) {fRefId = refId;}
	void SetPreviousTrackId(int id) { fPreviousTrackId = id; }
	void SetLastPlaneId(int lastPlaneId) { fLastPlaneId = lastPlaneId; }
	void SetParam(const CbmLitTrackParam* par) { fParam = *par; }
	void AddHit(CbmLitHit* hit);
	void SortHits(bool downstream = true);

	std::string ToString() const;

private:
	HitPtrVector fHits;
	CbmLitTrackParam fParam;
	myf fChi2;
	int fNDF;
	int fNofMissingHits;
	int fRefId;
	LitTrackQa fQuality;
	int fPreviousTrackId;
	int fLastPlaneId;
};

#endif /*CBMLITTRACKBRANCH_H_*/
