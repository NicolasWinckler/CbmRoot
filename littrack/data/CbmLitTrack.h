#ifndef CBMLITTRACK_H_
#define CBMLITTRACK_H_

#include "CbmLitTrackParam.h"
#include "CbmLitFitNode.h"
#include "CbmLitTypes.h"

class CbmLitTrack
{
public:
	CbmLitTrack();
	virtual ~CbmLitTrack();
	
	CbmLitTrack& operator=(const CbmLitTrack& track);
	CbmLitTrack(const CbmLitTrack& track) { *this = track; };
	
	Int_t GetNofHits() const { return fHits.size(); }
	Int_t GetFlag() const { return fFlag; }
	Double_t GetChi2() const { return fChi2; }
	Int_t GetNDF() const { return fNDF; }
	Int_t GetPreviousTrackId() const { return fPreviousTrackId; }
	Int_t GetPDG() const {return fPDG;};
	const CbmLitTrackParam* GetParamFirst() const { return &fParamFirst; }
	const CbmLitTrackParam* GetParamLast() const { return &fParamLast; }
	Int_t GetLastPlaneId() const { return fLastPlaneId; };
	const CbmLitHit* GetHit(Int_t index) const { return fHits[index];}
	const HitVector& GetHits() const { return fHits;}
	const CbmLitFitNode* GetFitNode(Int_t index) const {return &fFitNodes[index];}
	const FitNodeVector& GetFitNodes() const {return fFitNodes;}
	HitIteratorPair GetHits(Int_t planeId);
	std::vector<HitIteratorPair> GetHitBounds();
	
	void SetFlag(Int_t flag) { fFlag = flag; }
	void SetChi2(Double_t chi2) { fChi2 = chi2; }
	void SetNDF(Int_t ndf) { fNDF = ndf; }
	void SetPreviousTrackId(Int_t id) { fPreviousTrackId = id; }
	void SetPDG(Int_t pdg) { fPDG = pdg; }
	void SetParamFirst(const CbmLitTrackParam* par) { fParamFirst = *par; }
	void SetParamLast(const CbmLitTrackParam* par) { fParamLast = *par; }
	//TODO temporarily needed for equal_range algorithm 
	void SetNofHits(Int_t nofHits) { fHits.resize(nofHits); }
	void SetLastPlaneId(Int_t lastPlaneId) { fLastPlaneId = lastPlaneId; }
	void AddHit(const CbmLitHit* hit);
	void SetFitNodes(const FitNodeVector& nodes) {fFitNodes = nodes;}	
	void SortHits(Bool_t downstream = true);
	
	Bool_t CheckParams() const;
	void ClearHits();
	
	void Print() const;

private:

	HitVector fHits;
	CbmLitTrackParam fParamFirst;
	CbmLitTrackParam fParamLast;
	FitNodeVector fFitNodes;
	Int_t fFlag;
	Double_t fChi2;
	Int_t fNDF;
	Int_t fPreviousTrackId;
	Int_t fLastPlaneId;
	Int_t fPDG;
	
	ClassDef(CbmLitTrack, 1);
};

#endif /*CBMLITTRACK_H_*/
