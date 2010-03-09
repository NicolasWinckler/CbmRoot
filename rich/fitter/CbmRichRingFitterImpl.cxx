/******************************************************************************
*  $Id: CbmRichRingFitterImpl.cxx,v 1.2 2006/09/13 14:57:15 hoehne Exp $
*
*  Class  : CbmRichRingFitterImpl
*  Description: Empty file, just there to please CINT, DoFit is implemented
*               in the actual algorithm.
*
*  Author : Supriya Das
*  E-mail : S.Das@gsi.de
*
*******************************************************************************
*  $Log: CbmRichRingFitterImpl.cxx,v $
*  Revision 1.2  2006/09/13 14:57:15  hoehne
*  task for calculating Chi2 of ring fit added
*
*  Revision 1.1  2006/01/19 11:33:12  hoehne
*  initial version: base class for RingFitters
*
*
*******************************************************************************/
#include "CbmRichRingFitterImpl.h"

#include "CbmRichRing.h"
#include "CbmRichHit.h"

#include "FairRootManager.h"

#include "TMath.h"

#include <iostream>

using std::cout;
using std::endl;


void CbmRichRingFitterImpl::Init()
{
	cout << "CbmRichRingFitterImpl::Init()"<<endl;
	// Get and check FairRootManager
	FairRootManager* ioman = FairRootManager::Instance();
	if (! ioman) {
		cout << "-E- CbmRichRingFitterImpl::Init()"
		<< "RootManager not instantised!" << endl;
		return;
	}

	// Get hit Array
	fHitsArray = (TClonesArray*) ioman->GetObject("RichHit");
	if ( ! fHitsArray) {
		cout << "-W- CbmRichRingFitterImpl::Init(): No RichHit array!"
		<< endl;
	}
	fHitX.resize(MAX_NOF_HITS_IN_RING);
	fHitY.resize(MAX_NOF_HITS_IN_RING);
}

// -----   Protected method CalcChi2   ----------------------------------------
void CbmRichRingFitterImpl::CalcChi2(CbmRichRingLight* pRing)
{
    Int_t fNhits=pRing->GetNofHits();

    if (fNhits < 4) {
      pRing->SetChi2(-1.);
      return;
    }

    Float_t Xd2, Yd2;
    Float_t chi2 = 0.;

    Float_t Radius  = pRing->GetRadius();
    Float_t Xcenter = pRing->GetCenterX();
    Float_t Ycenter = pRing->GetCenterY();

    for (Int_t i = 0; i < fNhits; i++) {

        //CbmRichHit *hit = (CbmRichHit*)fHitsArray->At(pRing->GetHit(i));
		//Xd2 = Xcenter - hit->GetX();
		//Yd2 = Ycenter - hit->GetY();

    	Xd2 = Xcenter - fHitX[i];
		Yd2 = Ycenter - fHitY[i];
		Xd2 *= Xd2;
		Yd2 *= Yd2;

		Float_t d = TMath::Sqrt( Xd2 + Yd2 ) - Radius;
        chi2 += d*d;
    }

    pRing->SetChi2(chi2);
}

ClassImp(CbmRichRingFitterImpl)
