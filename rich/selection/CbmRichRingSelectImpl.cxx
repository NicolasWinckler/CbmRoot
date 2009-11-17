/******************************************************************************
*  $Id: CbmRichRingSelectImpl.cxx,v 1.1 2006/09/13 14:53:31 hoehne Exp $
*
*  Class  : CbmRichRingSelectImpl
*  Description : Abstract base class for concrete RICH ring selection algorithm:
*                to be run after ring-track assign for fake-ring rejection
*
*  Author : Simeon Lebedev
*  E-mail : salebedev@jinr.ru
*
*******************************************************************************
*  $Log: CbmRichRingSelectImpl.cxx,v $
*  Revision 1.1  2006/09/13 14:53:31  hoehne
*  initial version
*
*
*
*******************************************************************************/
#include "CbmRichRingSelectImpl.h"

#include "CbmRichRing.h"

#include "FairRootManager.h"

#include "TMath.h"

#include <iostream>
#include <vector>
#include <cmath>

using std::vector;
using std::cout;
using std::endl;
using std::sqrt;
using std::fabs;
using std::atan;

void CbmRichRingSelectImpl::Init()
{
	cout << "CbmRichRingSelectImpl::Init()"<<endl;
	// Get and check CbmRootManager
	FairRootManager* ioman = FairRootManager::Instance();
	if (! ioman) {
		cout << "-E- CbmRichRingSelectImpl::Init()"
		<< "RootManager not instantised!" << endl;
		return;
	}

	// Get hit Array
	fHitsArray = (TClonesArray*) ioman->GetObject("RichHit");
	if ( ! fHitsArray) {
		cout << "-W- CbmRichRingSelectImpl::Init(): No RichHit array!"
		<< endl;
	}
	kMAX_NOF_HITS = 100;
	fAlpha.resize(kMAX_NOF_HITS);
	fPhi.resize(kMAX_NOF_HITS);
}

Int_t CbmRichRingSelectImpl::GetNofHitsOnRing(CbmRichRing* ring){
    Int_t count = 0;
	Int_t nHits = ring->GetNofHits();
	CbmRichHit* hit;
	Double_t A = ring->GetAPar();
	Double_t B = ring->GetBPar();
	Double_t C = ring->GetCPar();
	Double_t D = ring->GetDPar();
	Double_t E = ring->GetEPar();
	Double_t F = ring->GetFPar();
	Double_t t1, t2;
	for(Int_t iH = 0; iH < nHits; iH++){
		hit = (CbmRichHit*)fHitsArray->At(ring->GetHit(iH));
		if (!hit) continue;

		Double_t x =hit->GetX();
		Double_t y = hit->GetY();

        Double_t d1 = TMath::Abs(A*x*x + B*x*y + C*y*y + D*x + E*y + F);
        t1 = 2*A*x + B*y + D;
        t2 = B*x + 2*C*y + E;
        Double_t d2 = sqrt( t1 * t1 + t2*t2);

        Double_t d = d1/d2;

        if (d < 0.3) count++;
	}


	return count;
}

Int_t CbmRichRingSelectImpl::GetNofHitsOnRingCircle(CbmRichRing* ring)
{
    Int_t count = 0;
	Int_t nHits = ring->GetNofHits();
	CbmRichHit* hitRing;
	for (Int_t iH = 0; iH < nHits; iH++) {
		hitRing = (CbmRichHit*) fHitsArray->At(ring->GetHit(iH));
		if (!hitRing)continue;
		Double_t rx = hitRing->GetX() - ring->GetCenterX();
		Double_t ry = hitRing->GetY() - ring->GetCenterY();
		Double_t r = sqrt(rx * rx + ry * ry) - ring->GetRadius();
		if (r < 0.35)	count++;
	}
	return count;
}

Double_t CbmRichRingSelectImpl::GetAngle(CbmRichRing* ring)
{
    register Int_t nHits = ring->GetNofHits();
	if (nHits > kMAX_NOF_HITS) return 0.2;
	if (nHits < 4) return 999.;

	register Double_t Pi = TMath::Pi();
	register Double_t TwoPi = 2.*TMath::Pi();
    CbmRichHit * hit;
    register Double_t xRing = ring->GetCenterX();
    register Double_t yRing = ring->GetCenterY();
    register Double_t xHit, yHit;

    for(Int_t iHit = 0; iHit < nHits; iHit++){
		hit = (CbmRichHit*)fHitsArray->At(ring->GetHit(iHit));
		xHit = hit->GetX();
		yHit = hit->GetY();

		if (!hit) continue;
		if (yHit-yRing == 0 || xHit-xRing == 0) return 999.;

		if( xHit > xRing && yHit > yRing ){
			fAlpha[iHit] = atan(fabs((yHit-yRing)/(xHit-xRing)));
		}else if( xHit < xRing && yHit > yRing ){
			fAlpha[iHit] = Pi - atan(fabs((yHit-yRing)/(xHit-xRing)));
		}else if( xHit < xRing && yHit < yRing ){
			fAlpha[iHit] = Pi + atan(fabs((yHit-yRing)/(xHit-xRing)));
		} else if( xHit > xRing && yHit < yRing ){
			fAlpha[iHit] = TwoPi - atan(fabs((yHit-yRing)/(xHit-xRing)));
		}
    }

    sort(fAlpha.begin(),fAlpha.begin()+nHits);

    for(Int_t i = 0; i < nHits-1; i++) fPhi[i] = fAlpha[i+1] - fAlpha[i];
    fPhi[nHits-1] = 2*Pi - fAlpha[nHits-1] + fAlpha[0];
    sort(fPhi.begin(),fPhi.begin()+nHits);

    Double_t angle = fPhi[nHits-1]+fPhi[nHits-2]+fPhi[nHits-3];

    return angle;
}



//------------------------------------------------------------------
ClassImp(CbmRichRingSelectImpl)
