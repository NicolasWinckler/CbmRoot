/******************************************************************************
*  $Id: CbmRichRingSelect2DCuts.cxx,v 1.1 2006/09/13 14:53:31 hoehne Exp $
*
*  Class  : CbmRichRingSelect2DCuts
*  Description : Implementation for concrete RICH ring selection algorithm:
*                reject rings with a set of certain 2-dimensional cuts
*                store result in "Selection2D":
*                0 = good rings
*                1 = rings to be rejected
*
*  Author : Simeon Lebedev
*  E-mail : salebedev@jinr.ru
*
*******************************************************************************
*  $Log: CbmRichRingSelect2DCuts.cxx,v $
*  Revision 1.1  2006/09/13 14:53:31  hoehne
*  initial version
*
*  
*
*******************************************************************************/
#include "CbmRichRingSelect2DCuts.h"

#include "CbmRichHit.h"
#include "CbmRichRing.h"

#include <iostream>

using std::cout;
using std::endl;


//--------------------------------------------------------------------------
// -----   Default constructor   -------------------------------------------
CbmRichRingSelect2DCuts::CbmRichRingSelect2DCuts()
{
	fVerbose = 1;
}

// -----   Standard constructor   ------------------------------------------
CbmRichRingSelect2DCuts::CbmRichRingSelect2DCuts ( Int_t verbose )
{
	fVerbose = verbose;
}

// -----   Destructor   ----------------------------------------------------
CbmRichRingSelect2DCuts::~CbmRichRingSelect2DCuts()
{}


void CbmRichRingSelect2DCuts::DoSelect(CbmRichRing* ring)
 {
	ring->SetAngle(GetAngle(ring));
	ring->SetNofHitsOnRing(GetNofHitsOnRing(ring));    
 
     Double_t lChi2 = ring->GetChi2();
     UInt_t   lTBSum = ring->GetNofHitsOnRing();
     Double_t lAngle = ring->GetAngle();
     Double_t lRadPos = ring->GetRadialPosition();
     Double_t lTrackD = ring->GetDistance();
     Int_t    lNofHits = ring->GetNofHits();
     //Double_t lRadius = ring->GetRadius();


     if (fVerbose > 1) ring->Print();

     Bool_t isCut = false;
     if (lTBSum <=12 && lRadPos <36) isCut = true;
     if (lAngle > 2.5 && lRadPos <36) isCut = true;
     if (lChi2 > 0.4 && lRadPos <40) isCut = true;
     if (lChi2 > 0.6 && (Double_t)lTBSum/(Double_t)lNofHits < 0.42) isCut = true;
     if (lTBSum <=10) isCut = true;
     if (lTrackD > 1.0 && lRadPos < 40) isCut = true;

     if (isCut == true) ring->SetSelection2D(-1);
     else  ring->SetSelection2D(1);

     if (fVerbose > 1) cout << " 2D cut in RingSelect: " << ring->GetSelection2D() << endl;

 }

// -------------------------------------------------------------------------
ClassImp(CbmRichRingSelect2DCuts)
