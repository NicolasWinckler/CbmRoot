// --------------------------------------------------------------------------------------
// Implementation: Semen Lebedev (s.lebedev@gsi.de)

//#include "tbb/blocked_range.h"
//#include "tbb/parallel_for.h"
#include "tbb/parallel_invoke.h"
#include "CbmRichRingFinderHoughParallelImpl.h"
#include <iostream>
//class ApplyHough {
//	Int_t fIndmin;
//	Int_t fIndmax;
//	CbmRichRingFinderHoughParallelImpl* fHTImpl;
//public:
//	void operator()(const tbb::blocked_range<size_t>& r) const {
//		for ( size_t i = r.begin(); i!= r.end(); ++i){
//			fHTImpl->HoughTransformGroup(fIndmin, fIndmax, i);
//		}
//	}
//
//	ApplyHough(CbmRichRingFinderHoughParallelImpl* ht,
//			Int_t indmin, Int_t indmax){
//		fHTImpl = ht;
//		fIndmax = indmax;
//		fIndmin = indmin;
//	}
//
//};

class ApplyHough {
	Int_t fIndmin;
	Int_t fIndmax;
	Int_t fGroup;
	CbmRichRingFinderHoughParallelImpl* fHTImpl;
public:
	void operator()() const {
		fHTImpl->HoughTransformGroup(fIndmin, fIndmax, fGroup);
	}

	ApplyHough(CbmRichRingFinderHoughParallelImpl* ht,
			Int_t indmin, Int_t indmax, Int_t group){
		fHTImpl = ht;
		fIndmax = indmax;
		fIndmin = indmin;
		fGroup = group;
	}
};

CbmRichRingFinderHoughParallelImpl::CbmRichRingFinderHoughParallelImpl(
		TString geometry)
{
    std::cout << "-I- CbmRichRingFinderHoughImpl constructor for " << geometry << " RICH geometry"<<std::endl;
    if (geometry != "compact" && geometry != "large"){
        geometry = "compact";
        std::cout << "-E- CbmRichRingFinderHoughImpl::SetParameters UNKNOWN geometry,  " <<
        "Set default parameters for "<< geometry << " RICH geometry"<<std::endl;
    }
    fGeometryType = geometry;
}

void CbmRichRingFinderHoughParallelImpl::HoughTransform(
		unsigned short int indmin,
		unsigned short int indmax)
{
	//tbb::parallel_for(tbb::blocked_range<size_t>(0, fNofParts,1), ApplyHough(this, indmin, indmax));

        //if (fHitInd[0].size() <= fMinNofHitsInArea)
        //ApplyHough a(this,indmin, indmax, 0);
        //if (fHitInd[1].size() <= fMinNofHitsInArea)
        //ApplyHough b(this,indmin, indmax, 1);
		//tbb::parallel_invoke(a,b);

		HoughTransformGroup(indmin, indmax, 0);
		HoughTransformGroup(indmin, indmax, 1);
}
