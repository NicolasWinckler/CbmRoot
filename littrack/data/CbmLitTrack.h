#ifndef CBMLITTRACK_H_
#define CBMLITTRACK_H_

#include "base/CbmLitFloat.h"
#include "base/CbmLitTypes.h"
#include "base/CbmLitEnums.h"
#include "data/CbmLitTrackParam.h"
#include "data/CbmLitFitNode.h"

#include <string>

class CbmLitTrack
{
public:
	CbmLitTrack();
	virtual ~CbmLitTrack();

	CbmLitTrack& operator=(const CbmLitTrack& track);
	CbmLitTrack(const CbmLitTrack& track);

	int GetNofHits() const { return fHits.size(); }
	int GetNofHits(int planeId);
	LitTrackQa GetQuality() const { return fQuality; }
	myf GetChi2() const { return fChi2; }
	int GetNDF() const { return fNDF; }
	int GetPreviousTrackId() const { return fPreviousTrackId; }
	int GetPDG() const {return fPDG;};
	const CbmLitTrackParam* GetParamFirst() const { return &fParamFirst; }
	const CbmLitTrackParam* GetParamLast() const { return &fParamLast; }
	int GetLastPlaneId() const { return fLastPlaneId; };
	const CbmLitHit* GetHit(int index) const { return fHits[index];}
	const HitPtrVector& GetHits() const { return fHits;}
	const CbmLitFitNode* GetFitNode(int index) const {return &fFitNodes[index];}
	const FitNodeVector& GetFitNodes() const {return fFitNodes;}
	HitPtrIteratorPair GetHits(int planeId);
	void GetHitBounds(std::vector<HitPtrIteratorPair>& bounds);
	int GetNofMissingHits() const {return fNofMissingHits;}
	int GetRefId() const {return fRefId;}

	void SetQuality(LitTrackQa quality) { fQuality = quality; }
	void SetChi2(myf chi2) { fChi2 = chi2; }
	void SetNDF(int ndf) { fNDF = ndf; }
	void SetPreviousTrackId(int id) { fPreviousTrackId = id; }
	void SetPDG(int pdg) { fPDG = pdg; }
	void SetParamFirst(const CbmLitTrackParam* par) { fParamFirst = *par; }
	void SetParamLast(const CbmLitTrackParam* par) { fParamLast = *par; }
	//TODO temporarily needed for equal_range algorithm
	void SetNofHits(int nofHits) { fHits.resize(nofHits); }
	void SetLastPlaneId(int lastPlaneId) { fLastPlaneId = lastPlaneId; }
	void AddHit(const CbmLitHit* hit);
	void SetFitNodes(const FitNodeVector& nodes) {fFitNodes = nodes;}
	void SortHits(bool downstream = true);
	void SetNofMissingHits(int nofMissingHits) {fNofMissingHits = nofMissingHits;}
	void SetRefId(int refId) {fRefId = refId;}

	void RemoveHit(int index);

	bool CheckParams() const;
	void ClearHits();

	std::string ToString() const;

private:

	HitPtrVector fHits;
	CbmLitTrackParam fParamFirst;
	CbmLitTrackParam fParamLast;
	FitNodeVector fFitNodes;
	LitTrackQa fQuality;
	myf fChi2;
	int fNDF;
	int fPreviousTrackId;
	int fLastPlaneId;
	int fPDG;
	int fNofMissingHits;
	int fRefId;
};

#endif /*CBMLITTRACK_H_*/
