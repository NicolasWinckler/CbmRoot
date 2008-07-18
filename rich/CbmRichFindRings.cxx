/******************************************************************************
*  $Id: CbmRichFindRings.cxx,v 1.2 2006/01/26 09:49:07 hoehne Exp $
*
*  Class  : CbmRichFindRings
*  Description: This is the implementation of the FindRings class. This
*               takes a particular Ring Finder to find the rings.
*
*  Author : Supriya Das
*  E-mail : S.Das@gsi.de
*
*******************************************************************************
*  $Log: CbmRichFindRings.cxx,v $
*  Revision 1.2  2006/01/26 09:49:07  hoehne
*  Array of projected tracks added for track-based ring finders
*
*  Revision 1.1  2006/01/19 11:24:39  hoehne
*  initial revision
*
*
*******************************************************************************/
#include "CbmRichFindRings.h"

#include "CbmRichRing.h"
#include "CbmRichRingFinder.h"

#include "CbmHit.h"
#include "CbmRootManager.h"

#include "TClonesArray.h"

#include <iostream>

using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
CbmRichFindRings::CbmRichFindRings()
  : CbmTask("RICH Find Rings") {
  fFinder         = NULL;
  fRichHitArray   = NULL;
  fProjArray      = NULL;
  fRingArray      = NULL;
  fNofRings       = 0;
  fVerbose        = 1;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmRichFindRings::CbmRichFindRings(CbmRichRingFinder* finder,
				   Int_t verbose)
  : CbmTask("RICH Find Rings") {
  fFinder         = finder;
  fRichHitArray   = NULL;
  fProjArray      = NULL;
  fRingArray      = NULL;
  fNofRings       = 0;
  fVerbose        = verbose;
}
// -------------------------------------------------------------------------



// -----   Constructor with name and title   -------------------------------
CbmRichFindRings::CbmRichFindRings(const char* name, const char* title,
				   CbmRichRingFinder* finder,
				   Int_t verbose)
  : CbmTask(name) {
  fFinder         = finder;
  fRichHitArray   = NULL;
  fProjArray      = NULL;
  fRingArray      = NULL;
  fNofRings       = 0;
  fVerbose        = verbose;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmRichFindRings::~CbmRichFindRings() {
  fRingArray->Delete();
}
// -------------------------------------------------------------------------



// -----   Public method Init (abstract in base class)  --------------------
InitStatus CbmRichFindRings::Init() {

  // Check for Ring finder
  if (! fFinder) {
    cout << "-E- CbmRichFindRings::Init: No ring finder selected!" << endl;
    return kERROR;
  }

  // Get and check CbmRootManager
  CbmRootManager* ioman = CbmRootManager::Instance();
  if (! ioman) {
    cout << "-E- CbmRichFindRings::Init: "
	 << "RootManager not instantised!" << endl;
    return kFATAL;
  }

  // Get RICH hit Array
  fRichHitArray
    = (TClonesArray*) ioman->GetObject("RichHit");
  if ( ! fRichHitArray) {
    cout << "-W- CbmRichFindRings::Init: No Rich Hit array!"
	 << endl;
  }

  // Get RICH Ring Array of projected tracks
  fProjArray
    = (TClonesArray*) ioman->GetObject("RichProjection");
  if ( ! fProjArray) {
    cout << "-W- CbmRichFindRings::Init: No Rich Ring Array of projected tracks!"
	 << endl;
  }

  // Create and register RichRing array
  fRingArray = new TClonesArray("CbmRichRing",100);
  ioman->Register("RichRing", "RICH", fRingArray, kTRUE);

  // Set verbosity of ring finder
  fFinder->SetVerbose(fVerbose);

  // Call the Init method of the ring finder
  fFinder->Init();

  return kSUCCESS;

}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void CbmRichFindRings::Exec(Option_t* opt) {
  fRingArray->Clear();
  fNofRings = fFinder->DoFind(fRichHitArray, fProjArray, fRingArray);
  /*
  for (Int_t iRing=0; iRing<fRingArray->GetEntriesFast(); iRing++) {
    CbmRichRing* ring = (CbmRichRing*) fRingArray->At(iRing);
    ring->SortRings();
  }
  */
}
// -------------------------------------------------------------------------



// -----   Public method Finish   ------------------------------------------
void CbmRichFindRings::Finish() {
  fRingArray->Clear();
}
// -------------------------------------------------------------------------




ClassImp(CbmRichFindRings)
