/** CbmRichRingFinderHough.cxx
 * @author Semen Lebedev <s.lebedev@gsi.de>
 * @since 2007
 * @version 2.0
 **/

#include "CbmRichRingFinderHough.h"
#include "CbmRichRingFinderHoughImpl.h"
//#include "CbmRichRingFinderHoughSimd.h"
//#include "../../littrack/utils/CbmLitMemoryManagment.h"
#include "CbmRichHit.h"
#include "CbmRichRing.h"
#include "FairTrackParam.h"
#include "CbmRichRingFitterCOPLight.h"
#include "CbmRichRingSelectNeuralNet.h"

#include "TString.h"
#include "TStopwatch.h"
#include "TSystem.h"
#include "TClonesArray.h"

#include <iostream>
#include <cmath>
#include <set>
#include <algorithm>
#include <fstream>

using std::cout;
using std::endl;
using std::vector;

CbmRichRingFinderHough::CbmRichRingFinderHough(
      Int_t verbose,
      TString geometry)
{
	//FIXME: remove geometry type from function parameters
    fRingCount = 0;
    fNEvent = 0;

#ifdef HOUGH_SERIAL
	fHTImpl = new CbmRichRingFinderHoughImpl();
#endif

#ifdef HOUGH_SIMD
	fHTImpl  = new CbmRichRingFinderHoughSimd();
#endif
}

void CbmRichRingFinderHough::Init()
{
	fHTImpl->Init();
}

CbmRichRingFinderHough::CbmRichRingFinderHough()
{
}

CbmRichRingFinderHough::~CbmRichRingFinderHough()
{
	delete fHTImpl;
}

Int_t CbmRichRingFinderHough::DoFind(
      const vector<CbmRichHoughHit>& data)
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

	fHTImpl->SetData(UpH);
	fHTImpl->DoFind();
	//if (rRingArray!=NULL) AddRingsToOutputArray(rRingArray, fHTImpl->GetFoundRings());
	UpH.clear();

	fHTImpl->SetData(DownH);
	fHTImpl->DoFind();
	//if (rRingArray!=NULL) AddRingsToOutputArray(rRingArray, fHTImpl->GetFoundRings());
	DownH.clear();
}

Int_t CbmRichRingFinderHough::DoFind(
      TClonesArray* rHitArray,
      TClonesArray* rProjArray,
      TClonesArray* rRingArray)
{
	TStopwatch timer;

	fNEvent++;
	if (fVerbose) cout << "-I- CbmRichRingFinderHough  Event no. " << fNEvent<< endl;

	std::vector<CbmRichHoughHit> UpH;
	std::vector<CbmRichHoughHit> DownH;
    fRingCount = 0;

	if (NULL == rHitArray) {
		cout << "-E- CbmRichRingFinderHough::DoFind: Hit array missing! "<< rHitArray << endl;
		return -1;
	}
	const Int_t nhits = rHitArray->GetEntriesFast();
	if (nhits <= 0) {
		cout << "-E- CbmRichRingFinderHough::DoFind: No hits in this event."	<< endl;
		return -1;
	}

	UpH.reserve(nhits/2);
	DownH.reserve(nhits/2);

	// convert CbmRichHit to CbmRichHoughHit and
	// sort hits according to the photodetector (up or down)
	for(Int_t iHit = 0; iHit < nhits; iHit++) {
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

	timer.Start();
	fHTImpl->SetData(UpH);
	fHTImpl->DoFind();
	timer.Stop();
	if (rRingArray!=NULL) AddRingsToOutputArray(rRingArray, fHTImpl->GetFoundRings());
	//for_each(UpH.begin(), UpH.end(), DeleteObject());
	UpH.clear();

	timer.Start(false);
	fHTImpl->SetData(DownH);
	fHTImpl->DoFind();
	timer.Stop();
	if (rRingArray!=NULL) AddRingsToOutputArray(rRingArray, fHTImpl->GetFoundRings());
	//for_each(DownH.begin(), DownH.end(), DeleteObject());
	DownH.clear();
	fExecTime += timer.CpuTime();

	cout << "CbmRichRingFinderHough: Number of output rings: "<<
	      rRingArray->GetEntriesFast() << endl;
	cout << "Exec time : " << fExecTime << ", per event " << 1000.*fExecTime/fNEvent <<
	      " ms" << endl;

	return 1;
}

void CbmRichRingFinderHough::AddRingsToOutputArray(
      TClonesArray *rRingArray,
		const vector<CbmRichRingLight*>& rings)
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

void CbmRichRingFinderHough::Finish()
{
}

ClassImp(CbmRichRingFinderHough)
