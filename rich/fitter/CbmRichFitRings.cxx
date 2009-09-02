/******************************************************************************
*  $Id: CbmRichFitRings.cxx,v 1.1 2006/01/19 12:19:27 hoehne Exp $
*
*  Class  : CbmRichRingFitter
*  Description: Task class for ring fitting in the RICH.
*               Input: TClonesArray of CbmRichRing
*
*               Uses as ring fitting algorithm classes derived from
*               CbmRichRingFitter.
*
*  Author : Supriya Das
*  E-mail : S.Das@gsi.de
*
*******************************************************************************
*  $Log: CbmRichFitRings.cxx,v $
*  Revision 1.1  2006/01/19 12:19:27  hoehne
*  initial version
*
*
*******************************************************************************/

#include "CbmRichFitRings.h"

#include "CbmRichRingFitter.h"
#include "CbmRichRing.h"

#include "FairRootManager.h"

#include "TClonesArray.h"

#include <iostream>

using std::cout;
using std::endl;



// -----   Default constructor   -------------------------------------------
CbmRichFitRings::CbmRichFitRings() {
  fFitter        = NULL;
  fRingArray     = NULL;
  fNofRings      = 0;
  fVerbose       = 1;
}
// -------------------------------------------------------------------------


// -----   Standard constructor   ------------------------------------------
CbmRichFitRings::CbmRichFitRings(CbmRichRingFitter* fitter,
				   Int_t verbose)
  : FairTask("RICH Fit Rings") {
  fFitter         = fitter;
  fRingArray      = NULL;
  fNofRings       = 0;
  fVerbose        = verbose;
}
// -------------------------------------------------------------------------

// -----   Standard constructor with name and title   ----------------------
CbmRichFitRings::CbmRichFitRings(const char* name,
				 const char* title,
				 CbmRichRingFitter* fitter,
				 Int_t verbose)
  : FairTask(name) {
  fFitter        = fitter;
  fRingArray    = NULL;
  fNofRings     = 0;
  fVerbose      = verbose;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmRichFitRings::~CbmRichFitRings() { }
// -------------------------------------------------------------------------



// -----   Public method Init (abstract in base class)  --------------------
InitStatus CbmRichFitRings::Init() {
  // Check for Track fitter
  if (! fFitter) {
    cout << "-E- CbmRichFitRings: No ring fitter selected!" << endl;
    Fatal("CbmRichFitRings","No fitter selected");
    return kERROR;
  }

  // Get and check FairRootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if (! ioman) {
    cout << "-E- CbmRichFitRings::Init: "
	 << "RootManager not instantised!" << endl;
    return kFATAL;
  }

  // Get Rich Ring array
  fRingArray  = (TClonesArray*) ioman->GetObject("RichRing");
  if ( ! fRingArray) {
    cout << "-E- CbmRichFitRings::Init: No RichRing array!"
	 << endl;
    return kERROR;
  }

  // Set verbosity of ring fitter
  fFitter->SetVerbose(fVerbose);

  // Call the Init method of the ring fitter
  fFitter->Init();

  return kSUCCESS;

}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void CbmRichFitRings::Exec(Option_t* opt) {
  if ( !fRingArray ) {
    cout<<"-E- CbmRichFitRings::Exec: No Ring Array"<<endl;
  return;
  }
  Int_t nRings = fRingArray->GetEntriesFast();
  for (Int_t iRing=0; iRing<nRings; iRing++) {
   CbmRichRing* pRing = (CbmRichRing*)fRingArray->At(iRing);
    fFitter->DoFit(pRing);

    if (fVerbose >1) cout <<"-I- CbmRichFitRings: iRing, x, y, r :" <<
                            iRing << " " << pRing->GetCenterX() <<
			    " " << pRing->GetCenterY() << " " <<
			    pRing->GetRadius() << endl;
  }
}
// -------------------------------------------------------------------------



// -----   Public method Finish   ------------------------------------------
void CbmRichFitRings::Finish() { }
// -------------------------------------------------------------------------


ClassImp(CbmRichFitRings)
