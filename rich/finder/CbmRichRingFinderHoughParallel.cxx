// --------------------------------------------------------------------------------------
// CbmRichRingFinderHoughBase source file
// Base class for ring finders based on on HT method
// Implementation: Semen Lebedev (s.lebedev@gsi.de)

#include "tbb/task_scheduler_init.h"
#include "tbb/task.h"
#include "tbb/tick_count.h"
#include "tbb/parallel_invoke.h"

#include "CbmRichRingFinderHoughParallel.h"
#include "CbmRichRingFinderHoughImpl.h"

#include "TString.h"
#include "TStopwatch.h"
#include "TSystem.h"

#include <iostream>
#include <cmath>
#include <set>
#include <algorithm>
#include <iostream>

using std::cout;
using std::endl;
using std::vector;

#ifdef HOUGH_IMPL
class FinderTask{
	CbmRichRingFinderHoughImpl* fHTImpl;
public:
	FinderTask(CbmRichRingFinderHoughImpl* HTImpl) {
		fHTImpl = HTImpl;
	}
	void operator()() const {
		fHTImpl->DoFind();
	}
};
#endif

#ifdef HOUGH_IMPL_PARALLEL
class FinderTask{
	CbmRichRingFinderHoughParallelImpl* fHTImpl;
public:
	FinderTask(CbmRichRingFinderHoughParallelImpl* HTImpl) {
		fHTImpl = HTImpl;
	}
	void operator()() const {
		fHTImpl->DoFind();
	}
};
#endif

// -----   Standard constructor   ------------------------------------------
CbmRichRingFinderHoughParallel::CbmRichRingFinderHoughParallel  ( Int_t verbose, TString geometry )
{
    cout << "-I- CbmRichRingFinderHoughParallel constructor for " << geometry << " RICH geometry"<<endl;
    if (geometry != "compact" && geometry != "large"){
        geometry = "compact";
        cout << "-E- CbmRichRingFinderHoughParallel::SetParameters UNKNOWN geometry,  " <<
        "Set default parameters for "<< geometry << " RICH geometry"<<endl;
    }
    fGeometryType = geometry;
}

void CbmRichRingFinderHoughParallel::Init()
{
    fNEvent = 0;
    fRingCount = 0;
#ifdef HOUGH_IMPL_PARALLEL
	fHTImpl1 = new CbmRichRingFinderHoughParallelImpl(fGeometryType);
	fHTImpl2 = new CbmRichRingFinderHoughParallelImpl(fGeometryType);
#endif
#ifdef HOUGH_IMPL
	fHTImpl1 = new CbmRichRingFinderHoughImpl(fGeometryType);
	fHTImpl2 = new CbmRichRingFinderHoughImpl(fGeometryType);
#endif

	fHTImpl2->Init();
	fHTImpl1->Init();
	tbb::task_scheduler_init init;
}

CbmRichRingFinderHoughParallel::CbmRichRingFinderHoughParallel()
{


}
// -----   Destructor   ----------------------------------------------------
CbmRichRingFinderHoughParallel::~CbmRichRingFinderHoughParallel()
{

}


Int_t CbmRichRingFinderHoughParallel::DoFind(const vector<CbmRichHoughHit>& data)
{
	fNEvent++;
	if (fVerbose) cout << "-I- CbmRichRingFinderHough  Event no. " << fNEvent<< endl;

	std::vector<CbmRichHoughHit> UpH;
	std::vector<CbmRichHoughHit> DownH;
    fRingCount = 0;

	UpH.reserve(data.size()/2);
	DownH.reserve(data.size()/2);

	for(Int_t iHit = 0; iHit < data.size(); iHit++) {
		CbmRichHoughHit hit = data[iHit];
		if (hit.fHit.fY >= 0)
			UpH.push_back(hit);
		else
			DownH.push_back(hit);
	}
	fHTImpl1->SetData(UpH);
	fHTImpl2->SetData(DownH);

	fHTImpl1->DoFind();
	fHTImpl2->DoFind();

	FinderTask a(fHTImpl1);
	FinderTask b(fHTImpl2);
	tbb::parallel_invoke(a, b);

	UpH.clear();
	DownH.clear();
}


Int_t CbmRichRingFinderHoughParallel::DoFind(TClonesArray* rHitArray,
                                            TClonesArray* rProjArray,
                                         TClonesArray* rRingArray) {

	//TStopwatch timer;
	//timer.Start();

	tbb::tick_count t0 = tbb::tick_count::now();

    fRingCount = 0;

	fNEvent++;
	if (fVerbose)
		cout << "-------------------------    Event no. " << fNEvent<< "   -------------------------" << endl;

	std::vector<CbmRichHoughHit> UpH;
	std::vector<CbmRichHoughHit> DownH;

	if (!rHitArray) {
		cout << "-E- CbmRichRingFinderHoughParallel::DoFind: Hit array missing! "<< rHitArray << endl;
		return -1;
	}
	const Int_t nhits = rHitArray->GetEntriesFast();
	if (!nhits) {
		cout << "-E- CbmRichRingFinderHoughParallel::DoFind:No hits in this event."	<< endl;
		return -1;
	}
	for (Int_t iHit = 0; iHit < nhits; iHit++) {
		CbmRichHit * hit = (CbmRichHit*) rHitArray->At(iHit);
		if (hit) {
			CbmRichHoughHit tempPoint;
			tempPoint.fHit.fX = hit->GetX();
			tempPoint.fHit.fY = hit->GetY();
			tempPoint.fX2plusY2 = hit->GetX() * hit->GetX() + hit->GetY() * hit->GetY();
			tempPoint.fId = iHit;
			tempPoint.fIsUsed = false;
			if (hit->GetY() >= 0)
				UpH.push_back(tempPoint);
			else
				DownH.push_back(tempPoint);
		}
	}

	fHTImpl1->SetData(UpH);
	fHTImpl2->SetData(DownH);

	//fHTImpl1->DoFind();
	//fHTImpl2->DoFind();

	FinderTask a(fHTImpl1);
	FinderTask b(fHTImpl2);
	tbb::parallel_invoke(a, b);

	AddRingsToOutputArray(rRingArray, fHTImpl2->GetFoundRings());
	AddRingsToOutputArray(rRingArray, fHTImpl1->GetFoundRings());

	//timer.Stop();
	//fExecTime += timer.CpuTime();

	tbb::tick_count t1 = tbb::tick_count::now();

	fExecTime += (t1-t0).seconds();
	//for_each(UpH.begin(), UpH.end(), DeleteObject());
	UpH.clear();
	//for_each(DownH.begin(), DownH.end(), DeleteObject());
	DownH.clear();

	//FuzzyKE(rHitArray);
	if (fVerbose)
		cout << "CbmRichRingFinderHough: Number of output rings: "
				<< rRingArray->GetEntriesFast() << endl;

	cout << "Exec time : " << fExecTime << ", per event " << 1000.*fExecTime/fNEvent << " ms" << endl;

	return 1;
}

void CbmRichRingFinderHoughParallel::AddRingsToOutputArray(TClonesArray *rRingArray,
		std::vector<CbmRichRingLight*>& rings)
{
	for (UInt_t iRing = 0; iRing < rings.size(); iRing++) {
		if (rings[iRing]->GetRecFlag() == -1)	continue;
		CbmRichRing* r = new CbmRichRing();
		for (Int_t i = 0; i < rings[iRing]->GetNofHits(); i++){
			r->AddHit(rings[iRing]->GetHitId(i));
		}
		new ((*rRingArray)[fRingCount]) CbmRichRing(*r);
		fRingCount++;
	}
}

ClassImp(CbmRichRingFinderHoughParallel);
