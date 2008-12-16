#ifndef CBMLITHITDATA_H_
#define CBMLITHITDATA_H_

#include "CbmLitTypes.h"

#include <vector>
#include <string>
#include <utility>

class CbmLitDetectorLayout;
class CbmLitHit;

class CbmLitHitData
{
public:
	CbmLitHitData();
	virtual ~CbmLitHitData();
	
	void SetDetectorLayout(
			const CbmLitDetectorLayout& layout);
	
	void AddHit(
			Int_t stationGroup,
			Int_t station,
			Int_t substation,
			CbmLitHit* hit);
	
	void AddHit(
			Int_t planeId,
			CbmLitHit* hit);
	
	const CbmLitHit* GetHit(
			Int_t stationGroup,
			Int_t station,
			Int_t substation,
			Int_t hitId) const;
	
	const CbmLitHit* GetHit(
			Int_t planeId,
			Int_t hitId) const;
	
	HitPtrIteratorPair GetHits(
			Int_t stationGroup,
			Int_t station,
			Int_t substation);
	
	HitPtrIteratorPair GetHits(
			Int_t planeId);
	
	Int_t GetNofHits(
			Int_t stationGroup,
			Int_t station,
			Int_t substation) const;
	
	std::pair<Double_t, Char_t> GetMaxErr(
			Int_t stationGroup,
			Int_t station,
			Int_t substation) const;
	
	std::pair<Double_t, Char_t> GetMaxErr(
			Int_t planeId) const;
	
	void Clear();
	
	virtual std::string ToString() const;
	
private:
	void StationByPlaneId(
			Int_t planeId,
			Int_t& stationGroup,
			Int_t& station,
			Int_t& substation) const;
	
	std::vector<std::vector<std::vector<HitPtrVector> > > fHits;
	std::vector<std::vector<std::vector<std::pair<Double_t, Char_t> > > > fMaxErr;
		
	ClassDef(CbmLitHitData, 1);
};

#endif /*CBMLITHITDATA_H_*/
