// --------------------------------------------------------------------------------------
// CbmRichRingFinderHough source file
// Base class for ring finders based on on HT method
// Implementation: Semen Lebedev (s.lebedev@gsi.de)

#include "CbmRichRingFinderHough.h"
//#include "CbmRichFuzzyKE.h"

#include "CbmRichHit.h"
#include "CbmRichRing.h"
#include "FairTrackParam.h"

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

// -----   Standard constructor   ------------------------------------------
CbmRichRingFinderHough::CbmRichRingFinderHough  ( Int_t verbose, TString geometry )
{
    cout << "-I- CbmRichRingFinderHough constructor for " << geometry << " RICH geometry"<<endl;
    if (geometry != "compact" && geometry != "large"){
        geometry = "compact";
        cout << "-E- CbmRichRingFinderHough::SetParameters UNKNOWN geometry,  " <<
        "Set default parameters for "<< geometry << " RICH geometry"<<endl;
    }

    fGeometryType = geometry;
    fIsFindOptPar = false;
    fRingCount = 0;
    fNEvent = 0;

	fHTImpl = new CbmRichRingFinderHoughImpl(fGeometryType);
	//fHTImpl = new CbmRichRingFinderHoughSimd(fGeometryType);
}

void CbmRichRingFinderHough::Init()
{
	fHTImpl->Init();
}

CbmRichRingFinderHough::CbmRichRingFinderHough()
{


}

// -----   Destructor   ----------------------------------------------------
CbmRichRingFinderHough::~CbmRichRingFinderHough()
{
	delete fHTImpl;
}

Int_t CbmRichRingFinderHough::DoFind(TClonesArray* rHitArray,
                                         TClonesArray* rProjArray,
                                         TClonesArray* rRingArray)
{


	TStopwatch timer;
	timer.Start();

	fNEvent++;
	if (fVerbose) cout << "-I- CbmRichRingFinderHough  Event no. " << fNEvent<< endl;

	std::vector<CbmRichHoughHit> UpH;
	std::vector<CbmRichHoughHit> DownH;
    fRingCount = 0;

	if (!rHitArray) {
		cout << "-E- CbmRichRingFinderHough::DoFind: Hit array missing! "<< rHitArray << endl;
		return -1;
	}
	const Int_t nhits = rHitArray->GetEntriesFast();
	if (!nhits) {
		cout << "-E- CbmRichRingFinderHough::DoFind:No hits in this event."	<< endl;
		return -1;
	}
	UpH.reserve(nhits/2);
	DownH.reserve(nhits/2);

	for(Int_t iHit = 0; iHit < nhits; iHit++) {
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

	fHTImpl->SetData(UpH);
	fHTImpl->DoFind();
	AddRingsToOutputArray(rRingArray, fHTImpl->GetFoundRings());
	UpH.clear();
	fHTImpl->SetData(DownH);
	fHTImpl->DoFind();
	AddRingsToOutputArray(rRingArray, fHTImpl->GetFoundRings());
	DownH.clear();
	timer.Stop();
	fExecTime += timer.CpuTime();

	if (fVerbose)cout << "CbmRichRingFinderHough: Number of output rings: "<< rRingArray->GetEntriesFast() << endl;

	cout << "Exec time : " << fExecTime << ", per event " << 1000.*fExecTime/fNEvent << " ms" << endl;

	if (fIsFindOptPar == true ){
		ofstream fout;
		fout.open("opt_param_ht.txt",std::ios_base::app);
		fout << fExecTime/fNEvent * 1000. << " ";
	}

	return 1;
}

void CbmRichRingFinderHough::Finish()
{

}

void CbmRichRingFinderHough::SetParameters( Int_t nofParts,
		Float_t maxDistance, Float_t minDistance,
		Float_t minRadius, Float_t maxRadius,
		Int_t HTCut, Int_t hitCut,
		Int_t HTCutR, Int_t hitCutR,
		Int_t nofBinsX, Int_t nofBinsY,
		Int_t nofBinsR, Float_t annCut,
		Float_t usedHitsCut, Float_t usedHitsAllCut,
		Float_t rmsCoeffEl, Float_t maxCutEl,
		Float_t rmsCoeffCOP, Float_t maxCutCOP)
{
	fHTImpl->SetParameters(nofParts,
			maxDistance, minDistance,
			minRadius, maxRadius,
			HTCut, hitCut,
			HTCutR, hitCutR,
			nofBinsX, nofBinsY,
			nofBinsR, annCut,
			usedHitsCut, usedHitsAllCut,
			rmsCoeffEl, maxCutEl,
			rmsCoeffCOP, maxCutCOP);

    ofstream fout;
    fout.open("opt_param_ht.txt",std::ios_base::app);
    fout << annCut << " " <<  rmsCoeffEl << " "<<
    maxCutEl << " " <<  usedHitsCut<< " " << usedHitsAllCut << endl;
}

void CbmRichRingFinderHough::AddRingsToOutputArray(TClonesArray *rRingArray,
		std::vector<CbmRichRing*>& rings)
{
	for (UInt_t iRing = 0; iRing < rings.size(); iRing++) {
		if (rings[iRing]->GetRecFlag() == -1)	continue;
		new ((*rRingArray)[fRingCount]) CbmRichRing(*rings[iRing]);
		fRingCount++;
	}
}

ClassImp(CbmRichRingFinderHough)
