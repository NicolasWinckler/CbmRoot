// --------------------------------------------------------------------------------------
// Implementation: Semen Lebedev (s.lebedev@gsi.de)

#include "tbb/blocked_range.h"
#include "tbb/parallel_for.h"

#include "CbmRichRingFinderHoughParallelImpl.h"

class ApplyHough {
	Int_t fIndmin;
	Int_t fIndmax;
	CbmRichRingFinderHoughParallelImpl* fHTImpl;
public:
	void operator()(const tbb::blocked_range<size_t>& r) const {
		for ( size_t i = r.begin(); i!= r.end(); ++i){
			fHTImpl->HoughTransformGroup(fIndmin, fIndmax, i);
		}
	}

	ApplyHough(CbmRichRingFinderHoughParallelImpl* ht,
			Int_t indmin, Int_t indmax){
		fHTImpl = ht;
		fIndmax = indmax;
		fIndmin = indmin;
	}

};


CbmRichRingFinderHoughParallelImpl::CbmRichRingFinderHoughParallelImpl  (TString geometry)
{
    cout << "-I- CbmRichRingFinderHoughImpl constructor for " << geometry << " RICH geometry"<<endl;
    if (geometry != "compact" && geometry != "large"){
        geometry = "compact";
        cout << "-E- CbmRichRingFinderHoughImpl::SetParameters UNKNOWN geometry,  " <<
        "Set default parameters for "<< geometry << " RICH geometry"<<endl;
    }
    fGeometryType = geometry;
}



void CbmRichRingFinderHoughParallelImpl::HoughTransform(unsigned short int indmin,
		unsigned short int indmax)
{
	tbb::parallel_for(tbb::blocked_range<size_t>(0, fNofParts,1), ApplyHough(this, indmin, indmax));
}

ClassImp(CbmRichRingFinderHoughImpl)
