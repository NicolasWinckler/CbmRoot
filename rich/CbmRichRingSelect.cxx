/******************************************************************************
*  $Id: CbmRichRingSelect.cxx,v 1.1 2006/09/13 14:53:31 hoehne Exp $
*
*  Class  : CbmRichRingSelect
*  Description : Abstract base class for concrete RICH ring selection algorithm:
*                to be run after ring-track assign for fake-ring rejection
*
*  Author : Simeon Lebedev
*  E-mail : salebedev@jinr.ru
*
*******************************************************************************
*  $Log: CbmRichRingSelect.cxx,v $
*  Revision 1.1  2006/09/13 14:53:31  hoehne
*  initial version
*
*  
*
*******************************************************************************/
#include "CbmRichRingSelect.h"

#include "CbmRootManager.h"

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

void CbmRichRingSelect::Init()
{
	cout << "CbmRichRingSelect::Init()"<<endl;
	// Get and check CbmRootManager
	CbmRootManager* ioman = CbmRootManager::Instance();
	if (! ioman) {
		cout << "-E- CbmRichRingSelect::Init()"
		<< "RootManager not instantised!" << endl;
		return;
	}
	
	// Get hit Array
	fHitsArray = (TClonesArray*) ioman->GetObject("RichHit");
	if ( ! fHitsArray) {
		cout << "-W- CbmRichRingSelect::Init(): No RichHit array!"
		<< endl;
	}

}

Int_t CbmRichRingSelect::GetNofHitsOnRing(CbmRichRing* ring){
    Int_t count = 0;
	Int_t nHits = ring->GetNofHits();
	CbmRichHit* hitRing;
	for(Int_t iH = 0; iH < nHits; iH++){
		hitRing = (CbmRichHit*)fHitsArray->At(ring->GetHit(iH));
		if (!hitRing) {
			cout << "-E- Double_t CbmRichRing::GetNofHitsOnRing()"<< iH <<endl;
			continue;
		}		
		Double_t rx = hitRing->X() - ring->GetCenterX();
		Double_t ry = hitRing->Y() - ring->GetCenterY();
		Double_t r = sqrt(rx*rx + ry*ry) - ring->GetRadius();
		if ( r < 0.35) count++;
	}
	return count;
}

Double_t CbmRichRingSelect::GetAngle(CbmRichRing* ring){
    vector<Double_t> alpha;
    vector<Double_t> phi;
    phi.clear();
    alpha.clear();
    Double_t Pi = TMath::Pi();
    CbmRichHit * hitRing;
    Int_t nHits = ring->GetNofHits();
    Double_t xRing = ring->GetCenterX();
    Double_t yRing = ring->GetCenterY();

    if (nHits < 2) return 999.;
    for(Int_t iHit = 0; iHit < nHits; iHit++){
		hitRing = (CbmRichHit*)fHitsArray->At(ring->GetHit(iHit));
		if (!hitRing) {
			cout << "-E- Double_t CbmRichRing::GetAngle()"<< iHit <<endl;
			continue;
		}
	
		if ((hitRing->Y()-yRing) == 0) {
			cout << " -W- CbmRichRing 0 in angle determination, y " << endl;
			return 999.;
		}
		if ((hitRing->X()-xRing) == 0) {
			cout << " -W- CbmRichRing 0 in angle determination, x " << endl;
			return 999.;
		}
	
		if( hitRing->X() > xRing && hitRing->Y() > yRing ){
			alpha.push_back(atan(fabs((hitRing->Y()-yRing)/
					(hitRing->X()-xRing))));
		}
		if( hitRing->X() < xRing && hitRing->Y() > yRing ){
			alpha.push_back(Pi - atan(fabs((hitRing->Y()-yRing)/
					(hitRing->X()-xRing))));
		}
		if( hitRing->X() < xRing && hitRing->Y() < yRing ){
			alpha.push_back(Pi + atan(fabs((hitRing->Y()-yRing)/
					(hitRing->X()-xRing))));
		}
		if( hitRing->X() > xRing && hitRing->Y() < yRing ){
			alpha.push_back(2*Pi - atan(fabs((hitRing->Y()-yRing)/
					(hitRing->X()-xRing))));
		}
    }

    sort(alpha.begin(),alpha.end());

    for(Int_t i = 0; i < nHits-1; i++) phi.push_back(alpha[i+1] - alpha[i]);
    phi.push_back(2*Pi - alpha[nHits-1] + alpha[0]);
    sort(phi.begin(),phi.end());

    Double_t bigestAngle = phi[nHits-1]+phi[nHits-2]+phi[nHits-3];

    return bigestAngle;
}



//------------------------------------------------------------------
ClassImp(CbmRichRingSelect)
