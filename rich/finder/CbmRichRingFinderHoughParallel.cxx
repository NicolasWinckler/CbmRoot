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

	fHTImpl1 = new CbmRichRingFinderHoughParallelImpl(fGeometryType);
	fHTImpl1->Init();
	fHTImpl2 = new CbmRichRingFinderHoughParallelImpl(fGeometryType);
	fHTImpl2->Init();
	tbb::task_scheduler_init init;
}

CbmRichRingFinderHoughParallel::CbmRichRingFinderHoughParallel()
{


}
// -----   Destructor   ----------------------------------------------------
CbmRichRingFinderHoughParallel::~CbmRichRingFinderHoughParallel()
{

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
			tempPoint.fX = hit->GetX();
			tempPoint.fY = hit->GetY();
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

	FinderTask a(fHTImpl1);
	FinderTask b(fHTImpl2);
	tbb::parallel_invoke(a, b);

	AddRingsToOutputArray(rRingArray, fHTImpl2->GetFoundRings());
	AddRingsToOutputArray(rRingArray, fHTImpl1->GetFoundRings());

	//timer.Stop();
	//fExecTime += timer.CpuTime();

	tbb::tick_count t1 = tbb::tick_count::now();
	fExecTime += (t1-t0).seconds();

	//FuzzyKE(rHitArray);
	if (fVerbose)
		cout << "CbmRichRingFinderHough: Number of output rings: "
				<< rRingArray->GetEntriesFast() << endl;

	cout << "Exec time : " << fExecTime << ", per event " << 1000.*fExecTime/fNEvent << " ms" << endl;

	return 1;
}

void CbmRichRingFinderHoughParallel::AddRingsToOutputArray(TClonesArray *rRingArray,
		vector<CbmRichRing*>& rings)
{
	for (UInt_t iRing = 0; iRing < rings.size(); iRing++) {
		if (rings[iRing]->GetRecFlag() == -1)	continue;
		new ((*rRingArray)[fRingCount]) CbmRichRing(*rings[iRing]);
		fRingCount++;
	}
}

ClassImp(CbmRichRingFinderHoughParallel);
