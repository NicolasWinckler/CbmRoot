#include "CbmRichRingSelectImpl.h"
#include "CbmRichHit.h"
#include "FairRootManager.h"

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

#include "TClonesArray.h"
#include "CbmRichRingLight.h"
#include "CbmRichRing.h"

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

int CbmRichRingSelectImpl::GetNofHitsOnRing(CbmRichRing* ring){
    int count = 0;
	int nHits = ring->GetNofHits();
	CbmRichHit* hit;
	double A = ring->GetAPar();
	double B = ring->GetBPar();
	double C = ring->GetCPar();
	double D = ring->GetDPar();
	double E = ring->GetEPar();
	double F = ring->GetFPar();
	double t1, t2;
	for(int iH = 0; iH < nHits; iH++){
		hit = (CbmRichHit*)fHitsArray->At(ring->GetHit(iH));
		if (!hit) continue;

		double x =hit->GetX();
		double y = hit->GetY();

        double d1 = fabs(A*x*x + B*x*y + C*y*y + D*x + E*y + F);
        t1 = 2*A*x + B*y + D;
        t2 = B*x + 2*C*y + E;
        double d2 = sqrt( t1 * t1 + t2*t2);

        double d = d1/d2;

        if (d < 0.3) count++;
	}
	return count;
}

int CbmRichRingSelectImpl::GetNofHitsOnRingCircle(CbmRichRing* ring)
{
    int count = 0;
	int nHits = ring->GetNofHits();
	CbmRichHit* hitRing;
	for (int iH = 0; iH < nHits; iH++) {
		hitRing = (CbmRichHit*) fHitsArray->At(ring->GetHit(iH));
		if (!hitRing)continue;
		float rx = hitRing->GetX() - ring->GetCenterX();
		float ry = hitRing->GetY() - ring->GetCenterY();
		float r = sqrt(rx * rx + ry * ry) - ring->GetRadius();
		if (r < 0.35f)	count++;
	}
	return count;
}

float CbmRichRingSelectImpl::GetAngle(CbmRichRing* ring)
{
    register int nHits = ring->GetNofHits();
	if (nHits > kMAX_NOF_HITS) return 0.2f;
	if (nHits < 4) return 999.f;

	register float Pi = 3.14159265;
	register float TwoPi = 2.*3.14159265;
    register float xRing = ring->GetCenterX();
    register float yRing = ring->GetCenterY();
    register float xHit, yHit;

    for(int iHit = 0; iHit < nHits; iHit++){
		CbmRichHit *hit = (CbmRichHit*)fHitsArray->At(ring->GetHit(iHit));
		xHit = hit->GetX();
		yHit = hit->GetY();

		if (!hit) continue;
		if (yHit-yRing == 0 || xHit-xRing == 0) continue;

		if( xHit > xRing){
			if (yHit > yRing){
				fAlpha[iHit] = atan(fabs((yHit-yRing)/(xHit-xRing)));
			} else{
				fAlpha[iHit] = TwoPi - atan(fabs((yHit-yRing)/(xHit-xRing)));
			}
		}else {
			if (yHit > yRing){
				fAlpha[iHit] = Pi - atan(fabs((yHit-yRing)/(xHit-xRing)));
			}else {
				fAlpha[iHit] = Pi + atan(fabs((yHit-yRing)/(xHit-xRing)));
			}
		}
    }

    sort(fAlpha.begin(),fAlpha.begin()+nHits);

    for(int i = 0; i < nHits-1; i++) fPhi[i] = fAlpha[i+1] - fAlpha[i];
    fPhi[nHits-1] = TwoPi - fAlpha[nHits-1] + fAlpha[0];
    sort(fPhi.begin(),fPhi.begin()+nHits);

    float angle = fPhi[nHits-1]+fPhi[nHits-2]+fPhi[nHits-3];

    return angle;
}
