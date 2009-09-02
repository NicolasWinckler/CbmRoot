 /******************************************************************************
*  $Id: CbmRichSelectRings.cxx,v 1.1 2006/09/13 14:53:31 hoehne Exp $
*
*  Class  : CbmRichSelectRings
*  Description: Implementation of the SelectRings class. This
*               takes a particular Ring Selection to distinguish good and fake rings
*
*  Author : Simeon Lebedev
*  E-mail : salebedev@jinr.ru
*
*******************************************************************************
*  $Log: CbmRichSelectRings.cxx,v $
*  Revision 1.1  2006/09/13 14:53:31  hoehne
*  initial version
*
*  
*
*******************************************************************************/
#include "CbmRichSelectRings.h"

#include "CbmRichRing.h"
#include "CbmRichRingSelect.h"

#include "FairHit.h"
#include "FairRootManager.h"

#include "TClonesArray.h"

#include <iostream>

using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
CbmRichSelectRings::CbmRichSelectRings()
  : FairTask("RICH Select Rings") {
  fSelect         = NULL;
  fRichHitArray   = NULL;
  fProjArray      = NULL;
  fRingArray      = NULL;
  fVerbose        = 1;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmRichSelectRings::CbmRichSelectRings(CbmRichRingSelect* select,
				   Int_t verbose)
  : FairTask("RICH Select Rings") {
  fSelect         = select;
  fRichHitArray   = NULL;
  fProjArray      = NULL;
  fRingArray      = NULL;
  fVerbose        = verbose;
}
// -------------------------------------------------------------------------



// -----   Constructor with name and title   -------------------------------
CbmRichSelectRings::CbmRichSelectRings(const char* name, const char* title,
				   CbmRichRingSelect* select,
				   Int_t verbose)
  : FairTask(name) {
  fSelect         = select;
  fRichHitArray   = NULL;
  fProjArray      = NULL;
  fRingArray      = NULL;
  fVerbose        = verbose;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmRichSelectRings::~CbmRichSelectRings() {

}
// -------------------------------------------------------------------------



// -----   Public method Init (abstract in base class)  --------------------
InitStatus CbmRichSelectRings::Init() {

  // Check for Ring Selection
  if (! fSelect) {
    cout << "-E- CbmRichSelectRings::Init: No Selection Routine  selected!" << endl;
    return kERROR;
  }

  // Get and check FairRootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if (! ioman) {
    cout << "-E- CbmRichSelectRings::Init: "
	 << "RootManager not instantised!" << endl;
    return kFATAL;
  }

  // Get RICH hit Array
  fRichHitArray
    = (TClonesArray*) ioman->GetObject("RichHit");
  if ( ! fRichHitArray) {
    cout << "-W- CbmRichSelectRings::Init: No Rich Hit array!"
	 << endl;
  }
  // Get RICH Ring Array of projected tracks
  fRingArray
    = (TClonesArray*) ioman->GetObject("RichRing");
  if ( ! fRingArray) {
    cout << "-W- CbmRichSelectRings::Init: No Rich Ring Array!"
	 << endl;
  }

  // Set verbosity of ring finder
  fSelect->SetVerbose(fVerbose);

  // Call the Init method of the ring finder
  fSelect->Init();

  return kSUCCESS;

}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void CbmRichSelectRings::Exec(Option_t* opt) {

if ( !fRingArray ) {
    cout<<"-E- CbmRichSelectRings::ExecC: No Ring Array"<<endl;
  return;
  }
  Int_t nRings = fRingArray->GetEntriesFast();
  for (Int_t iRing=0; iRing<nRings; iRing++) {
      CbmRichRing* pRing = (CbmRichRing*)fRingArray->At(iRing);
      fSelect->DoSelect(pRing);
  }

}
// -------------------------------------------------------------------------



// -----   Public method Finish   ------------------------------------------
void CbmRichSelectRings::Finish() {
}
// -------------------------------------------------------------------------

ClassImp(CbmRichSelectRings)
