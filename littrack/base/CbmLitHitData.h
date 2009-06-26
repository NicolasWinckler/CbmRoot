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
			int stationGroup,
			int station,
			int substation,
			CbmLitHit* hit);

	void AddHit(
			int planeId,
			CbmLitHit* hit);

	const CbmLitHit* GetHit(
			int stationGroup,
			int station,
			int substation,
			int hitId) const;

	const CbmLitHit* GetHit(
			int planeId,
			int hitId) const;

	HitPtrIteratorPair GetHits(
			int stationGroup,
			int station,
			int substation);

	HitPtrIteratorPair GetHits(
			int planeId);

	int GetNofHits(
			int stationGroup,
			int station,
			int substation) const;

	std::pair<double, Char_t> GetMaxErr(
			int stationGroup,
			int station,
			int substation) const;

	std::pair<double, Char_t> GetMaxErr(
			int planeId) const;

	void Clear();

	virtual std::string ToString() const;

private:
	void StationByPlaneId(
			int planeId,
			int& stationGroup,
			int& station,
			int& substation) const;

	std::vector<std::vector<std::vector<HitPtrVector> > > fHits;
	std::vector<std::vector<std::vector<std::pair<double, Char_t> > > > fMaxErr;
};

#endif /*CBMLITHITDATA_H_*/
