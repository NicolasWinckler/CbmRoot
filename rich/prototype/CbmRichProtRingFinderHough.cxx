// --------------------------------------------------------------------------------------
// CbmRichProtRingFinderHough source file
// Base class for ring finders based on on HT method
// Implementation: Semen Lebedev (s.lebedev@gsi.de)

#include "CbmRichProtRingFinderHough.h"
#include "CbmRichProtRingFinderHoughImpl.h"
//#include "CbmRichRingFinderHoughSimd.h"
//#include "../../littrack/utils/CbmLitMemoryManagment.h"

#include "CbmRichHit.h"
#include "CbmRichRing.h"
#include "FairTrackParam.h"
#include "CbmRichRingFitter.h"

#include "TString.h"
#include "TStopwatch.h"
#include "TSystem.h"
#include "TMath.h"

#include <iostream>
#include <cmath>
#include <set>
#include <algorithm>
#include <fstream>


using std::cout;
using std::endl;
using std::vector;

// -----   Standard constructor   ------------------------------------------
CbmRichProtRingFinderHough::CbmRichProtRingFinderHough  ( Int_t verbose, TString geometry )
{
    cout << "-I- CbmRichProtRingFinderHough constructor for " << geometry << " RICH geometry"<<endl;
    if (geometry != "compact" && geometry != "large"){
        geometry = "compact";
        cout << "-E- CbmRichProtRingFinderHough::SetParameters UNKNOWN geometry,  " <<
        "Set default parameters for "<< geometry << " RICH geometry"<<endl;
    }

    fGeometryType = geometry;
    fIsFindOptPar = false;
    fRingCount = 0;
    fNEvent = 0;

#ifdef HOUGH_SERIAL
	fHTImpl = new CbmRichProtRingFinderHoughImpl(fGeometryType);;
#endif

#ifdef HOUGH_SIMD
	fHTImpl  = new CbmRichProtRingFinderHoughSimd(fGeometryType);;
#endif

    rNewArray = new TClonesArray("CbmRichRing");

}

void CbmRichProtRingFinderHough::Init()
{
	fHTImpl->Init();
}

CbmRichProtRingFinderHough::CbmRichProtRingFinderHough()
{
    fExecTime = 0;
    rNewArray = new TClonesArray("CbmRichRing");
}

// -----   Destructor   ----------------------------------------------------
CbmRichProtRingFinderHough::~CbmRichProtRingFinderHough()
{
	delete fHTImpl;
}

Int_t CbmRichProtRingFinderHough::DoFind(const vector<CbmRichProtHoughHit>& data)
{
	fNEvent++;
	if (fVerbose) cout << "-I- CbmRichProtRingFinderHough  Event no. " << fNEvent<< endl;

	std::vector<CbmRichProtHoughHit> UpH;
	std::vector<CbmRichProtHoughHit> DownH;
    fRingCount = 0;

	UpH.reserve(data.size()/2);
	DownH.reserve(data.size()/2);

	for(Int_t iHit = 0; iHit < data.size(); iHit++) {
		CbmRichProtHoughHit hit = data[iHit];
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



Int_t CbmRichProtRingFinderHough::DoFind(TClonesArray* rHitArray,
                                         TClonesArray* rProjArray,
                                         TClonesArray* rRingArray)
{
	TStopwatch timer;

	fNEvent++;
	if (fVerbose) cout << "-I- CbmRichProtRingFinderHough  Event no. " << fNEvent<< endl;

	std::vector<CbmRichProtHoughHit> UpH;
	std::vector<CbmRichProtHoughHit> DownH;
	if(0 == rRingArray->GetEntriesFast()) {
	    fRingCount = 0;
	}

	if (!rHitArray) {
		cout << "-E- CbmRichProtRingFinderHough::DoFind: Hit array missing! "<< rHitArray << endl;
		return -1;
	}
	const Int_t nhits = rHitArray->GetEntriesFast();
	if (!nhits) {
		cout << "-E- CbmRichProtRingFinderHough::DoFind:No hits in this event."	<< endl;
		return -1;
	}
	UpH.reserve(nhits/2);
	DownH.reserve(nhits/2);

//	std::ofstream outEvents;
//	outEvents.open("events.txt",std::ios_base::app);
//	outEvents << fNEvent << " " << nhits << endl;


	FairTrackParam *proj = (FairTrackParam*) rProjArray->At(0);


        Double_t dist;


	if(rRingArray->GetEntriesFast()==0) {
	    for(Int_t iHit = 0; iHit < nhits; iHit++) {
		CbmRichHit * hit = (CbmRichHit*) rHitArray->At(iHit);
		if (hit) {
		    dist = TMath::Sqrt(TMath::Power(hit->GetX()-proj->GetX(),2)+
				       TMath::Power(hit->GetY()-proj->GetY(),2));
		    //if((dist/0.6125) > 8.16 || (dist/0.6125) < 3.27) {
		    if(dist > 6. || dist < 2.) {
			continue;
		    }
		    CbmRichProtHoughHit tempPoint;
		    tempPoint.fHit.fX = hit->GetX();
		    tempPoint.fHit.fY = hit->GetY();
		    //outEvents << hit->GetX() << " " << hit->GetY() << endl;

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

	    fExecTime += timer.CpuTime();
	}// rRingArray->GetEntriesFast()==0


        CbmRichRing *oldRing;
	CbmRichRing *ring;
	Float_t r;
	if(rRingArray->GetEntriesFast() > 0) {

	    rNewArray->Clear();
	    for(Int_t ir = 0; ir < rRingArray->GetEntriesFast(); ir++) {
		oldRing = (CbmRichRing*) rRingArray->At(ir);
		new ((*rNewArray)[ir]) CbmRichRing(*oldRing);
	    }

	    rRingArray->Clear();
	    fRingCount = 0;
	    for(Int_t ir = 0; ir < rNewArray->GetEntriesFast(); ir++) {

		ring = (CbmRichRing*) rNewArray->At(ir);

		fFitter->DoFit(ring);
		r = ring->GetRadius();

		for(Int_t iHit = 0; iHit < nhits; iHit++) {
		    CbmRichHit * hit = (CbmRichHit*) rHitArray->At(iHit);
		    if (hit) {
			dist = TMath::Sqrt(TMath::Power(hit->GetX()-ring->GetCenterX(),2)+
					   TMath::Power(hit->GetY()-ring->GetCenterY(),2));
			if((dist<(r-0.62)) || (dist>(r+0.62))) {
			    continue;
			}
			CbmRichProtHoughHit tempPoint;
			tempPoint.fHit.fX = hit->GetX();
			tempPoint.fHit.fY = hit->GetY();
			//outEvents << hit->GetX() << " " << hit->GetY() << endl;

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

		fExecTime += timer.CpuTime();
	    }
	}// rRingArray->GetEntriesFast()==0


	cout << "CbmRichProtRingFinderHough: Number of output rings: "<< rRingArray->GetEntriesFast() << endl;

	cout << "Exec time : " << fExecTime << ", per event " << 1000.*fExecTime/fNEvent << " ms" << endl;

	if (fIsFindOptPar == true ){
		std::ofstream fout;
		fout.open("opt_param_ht.txt",std::ios_base::app);
		fout << fExecTime/fNEvent * 1000. << " ";
	}

	return 1;
}

void CbmRichProtRingFinderHough::Finish()
{

}

void CbmRichProtRingFinderHough::SetParameters( Int_t nofParts,
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

void CbmRichProtRingFinderHough::AddRingsToOutputArray(TClonesArray *rRingArray,
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

ClassImp(CbmRichProtRingFinderHough)
