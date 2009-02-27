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


}
// -----   Protected method CalcChi2   ----------------------------------------
void CbmRichRingFitterImpl::CalcChi2(CbmRichRing* pRing)
{
    Int_t fNhits=pRing->GetNofHits();

    if (fNhits < 4) {
      pRing->SetChi2(-1.);
      return;
    }

    Double_t Xd2, Yd2;
    Double_t ChiSquare = 0.;
    Double_t Xcenter, Ycenter, Radius;

    Radius  = pRing->GetRadius();
    Xcenter = pRing->GetCenterX();
    Ycenter = pRing->GetCenterY();

    for (int i = 0; i < fNhits; i++) {

        CbmRichHit *hit = (CbmRichHit*)fHitsArray->At(pRing->GetHit(i));

		Xd2 = Xcenter - hit->X();
		Yd2 = Ycenter - hit->Y();

		Xd2 *= Xd2;
		Yd2 *= Yd2;

		Double_t d = TMath::Sqrt( Xd2 + Yd2 ) - Radius;
        ChiSquare += d*d;
    }

    if (fNhits == 3) cout << " -W- CbmRichRingFitterImpl: division by 0 " << endl;
    Double_t chi2 = ChiSquare / (fNhits - 3);

    pRing->SetChi2(chi2);

}

ClassImp(CbmRichRingFitterImpl)
