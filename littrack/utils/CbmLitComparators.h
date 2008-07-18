
#ifndef CBMLITCOMPARATORS_H_
#define CBMLITCOMPARATORS_H_

#include "CbmLitHit.h"
#include "CbmLitTrack.h"
#include "CbmLitMaterial.h"
#include "CbmLitTrackParam.h"


#include <functional>



class CompareHitPtrZLess: 
	public std::binary_function<
		const CbmLitHit*,
	    const CbmLitHit*,
	    bool> 
{
public:
	bool operator()(const CbmLitHit* hit1, const CbmLitHit* hit2) const {
		return hit1->GetZ() < hit2->GetZ();
	}
};



class CompareHitPtrWLess: 
	public std::binary_function<
		const CbmLitHit*,
	    const CbmLitHit*,
	    bool> 
{
public:
	bool operator()(const CbmLitHit* hit1, const CbmLitHit* hit2) const {
		return hit1->GetW() < hit2->GetW();
	}
};



class CompareTrackParamZLess: 
	public std::binary_function<
		const CbmLitTrackParam&,
	    const CbmLitTrackParam&,
	    bool> 
{
public:
	bool operator()(const CbmLitTrackParam& par1, const CbmLitTrackParam& par2) const {
		return par1.GetZ() < par2.GetZ();
	}
};
//
//class CompareHitPtrPlaneIdLess: 
//	public std::binary_function<
//		const CbmLitHit*,
//	    const CbmLitHit*,
//	    bool> 
//{
//public:
//	bool operator()(const CbmLitHit* hit1, const CbmLitHit* hit2) const {
//		return hit1->GetPlaneId() < hit2->GetPlaneId();
//	}
//};
//
//
//
//class CompareHitPtrPlaneIdMore: 
//	public std::binary_function<
//		const CbmLitHit*,
//	    const CbmLitHit*,
//	    bool> 
//{
//public:
//	bool operator()(const CbmLitHit* hit1, const CbmLitHit* hit2) const {
//		return hit1->GetPlaneId() > hit2->GetPlaneId();
//	}
//};



class CompareHitPtrPlaneIdLess: 
	public std::binary_function<
		const CbmLitHit*,
	    const CbmLitHit*,
	    bool> 
{
public:
	bool operator()(const CbmLitHit* hit1, const CbmLitHit* hit2) const {
		return hit1->GetPlaneId() < hit2->GetPlaneId();
	}
};



class CompareHitPtrPlaneIdMore: 
	public std::binary_function<
		const CbmLitHit*,
	    const CbmLitHit*,
	    bool> 
{
public:
	bool operator()(const CbmLitHit* hit1, const CbmLitHit* hit2) const {
		return hit1->GetPlaneId() > hit2->GetPlaneId();
	}
};



class CompareHitPtrYLess :
	public std::binary_function<
			const CbmLitHit*,
		    const CbmLitHit*,
		    bool> 
{
public:
	bool operator()(const CbmLitHit* hit1, const CbmLitHit* hit2) const {
		return hit1->GetY() < hit2->GetY();
	}
};



class CompareHitPtrXLess :
	public std::binary_function<
			const CbmLitHit*,
		    const CbmLitHit*,
		    bool> 
{
public:
	bool operator()(const CbmLitHit* hit1, const CbmLitHit* hit2) const {
		return hit1->GetX() < hit2->GetX();
	}
};



class CompareTrackPtrChi2OverNdfLess :
	public std::binary_function<
			const CbmLitTrack*,
		    const CbmLitTrack*,
		    bool> 
{
public:
	bool operator()(const CbmLitTrack* track1, const CbmLitTrack* track2) const {
		return ( (track1->GetChi2() / track1->GetNDF()) < (track2->GetChi2() / track2->GetNDF()) );
	}
};



class CompareTrackPtrPrevTrackIdLess :
	public std::binary_function<
			const CbmLitTrack*,
		    const CbmLitTrack*,
		    bool> 
{
public:
	bool operator()(const CbmLitTrack* track1, const CbmLitTrack* track2) const {
		return track1->GetPreviousTrackId() < track2->GetPreviousTrackId();
	}
};



class CompareTrackPtrNofHitsMore :
	public std::binary_function<
			const CbmLitTrack*,
		    const CbmLitTrack*,
		    bool> 
{
public:
	bool operator()(const CbmLitTrack* track1, const CbmLitTrack* track2) const {
		return track1->GetNofHits() > track2->GetNofHits();
	}
};


class CompareTrackPtrNofHitsLess :
	public std::binary_function<
			const CbmLitTrack*,
		    const CbmLitTrack*,
		    bool> 
{
public:
	bool operator()(const CbmLitTrack* track1, const CbmLitTrack* track2) const {
		return track1->GetNofHits() < track2->GetNofHits();
	}
};


class CompareTrackPtrLastPlaneIdMore :
	public std::binary_function<
			const CbmLitTrack*,
		    const CbmLitTrack*,
		    bool> 
{
public:
	bool operator()(const CbmLitTrack* track1, const CbmLitTrack* track2) const {
		return track1->GetLastPlaneId() > track2->GetLastPlaneId();
	}
};



class CompareMaterialPtrZLess :
	public std::binary_function<
			const CbmLitMaterial*,
		    const CbmLitMaterial*,
		    bool> 
{
public:
	bool operator()(const CbmLitMaterial* mat1, const CbmLitMaterial* mat2) const {
		return mat1->GetZ() < mat2->GetZ();
	}
};


#endif /*CBMLITCOMPARATORS_H_*/
