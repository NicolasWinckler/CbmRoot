/******************************************************************************
*  $Id: CbmRichRingFinderIdeal.cxx,v 1.5 2006/02/01 13:16:58 hoehne Exp $
*
*  Class  : CbmRichRingFinderIdeal
*  Description: This is the implementation of the Ideal RichRingFinder. This
*               takes the Rich hits and associates them with the MC Track.
*
*  Author : Supriya Das
*  E-mail : S.Das@gsi.de
*
*******************************************************************************
*  $Log: CbmRichRingFinderIdeal.cxx,v $
*  Revision 1.5  2006/02/01 13:16:58  hoehne
*  definition of reconstructable ring slightly changed
*
*  Revision 1.4  2006/01/30 14:34:35  hoehne
*  correct FairMCPoint to CbmRichPoint in line 141
*
*  Revision 1.3  2006/01/27 11:37:08  hoehne
*  some "return -1" removed to allow for standalone running without track information
*
*  Revision 1.2  2006/01/26 09:49:40  hoehne
*  array of projected tracks added for track-based ring finders
*
*  Revision 1.1  2006/01/19 11:20:45  hoehne
*  initial revision: ideal RingFinder
*
*
*******************************************************************************/
#include "CbmRichRingFinderIdeal.h"

#include "CbmRichHit.h"
#include "CbmRichRing.h"
#include "CbmRichPoint.h"

// CBM includes
#include "CbmMCTrack.h"
#include "FairRootManager.h"

// ROOT includes
#include "TClonesArray.h"

// C++ includes
#include <iostream>
#include <map>

using std::cout;
using std::endl;
using std::map;


// -----   Default constructor   -------------------------------------------
CbmRichRingFinderIdeal::CbmRichRingFinderIdeal() {
  fMCRichPointArray = NULL;
  fVerbose      = 1;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmRichRingFinderIdeal::CbmRichRingFinderIdeal(Int_t verbose) {
  fMCRichPointArray = NULL;
  fVerbose          = verbose;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmRichRingFinderIdeal::~CbmRichRingFinderIdeal() { }
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
void CbmRichRingFinderIdeal::Init() {

  // Get and check FairRootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if (! ioman) {
    cout << "-E- CbmRichRingFinderIdeal::Init: "
	 << "RootManager not instantised!" << endl;
    return;
  }

  // Get MCTrack array
  fMCTrackArray  = (TClonesArray*) ioman->GetObject("MCTrack");
  if ( ! fMCTrackArray) {
    cout << "-E- CbmRichRingFinderIdeal::Init: No MCTrack array!"
	 << endl;
    return;
  }

  // Get MCPoint array
  fMCRichPointArray  = (TClonesArray*) ioman->GetObject("RichPoint");
  if ( ! fMCRichPointArray) {
    cout << "-E- CbmRichRingFinderIdeal::Init: No MCPoint array!"
	 << endl;
    return;
  }

}
// -------------------------------------------------------------------------



// -----   Public method DoFind   ------------------------------------------
Int_t CbmRichRingFinderIdeal::DoFind(TClonesArray* hitArray, TClonesArray* projArray,
				     TClonesArray* ringArray) {

  // Check pointers
  if ( !fMCTrackArray ) {
    cout << "-E- CbmRichRingFinderIdeal::DoFind: "
	 << "MCTrack array missing! " << endl;
    return -1;
  }
  if ( !fMCRichPointArray ) {
    cout << "-E- CbmRichRingFinderIdeal::DoFind: "
	 << "MCPoint array missing! " << endl;
    return -1;
  }

  if ( !hitArray) {
    cout << "-E- CbmRichRingFinderIdeal::DoFind: "
	 << "Hit array missing! "
	 << hitArray << endl;
    return -1;
  }

  if ( !projArray) {
    cout << "-E- CbmRichRingFinderIdeal::DoFind: "
	 << "Track Projection array missing! --- not needed anyhow for Ideal Finder! "
	 << projArray << endl;
  }

  if ( !ringArray ) {
    cout << "-E- CbmRichRingFinderIdeal::DoFind: "
	 << "Ring array missing! " << endl;
    return -1;
  }

  // Initialise control counters
  Int_t nNoMCTrack   = 0;
  Int_t nNoRichPoint = 0;
  Int_t nNoRichHit   = 0;
  Int_t nNoRichRing  = 0;

  // Create pointers to Rich Hit and Rich Point

  CbmRichHit*      pRhit = NULL;
  CbmRichPoint*      pMCpt = NULL;
  CbmMCTrack*      pMCtr = NULL;
  CbmRichRing*     pRing = NULL;

  // Number of RICH hits
  Int_t nRichHits   = hitArray->GetEntriesFast();

  // Declare some variables outside the loops
  Int_t ptIndex      = 0;     //! MCPoint index
  Int_t mcTrackIndex = 0;     //! MCTrack index
  Int_t ringIndex    = 0;     //! RichRing index
  Int_t motherId     = 0;     //! Mother Id of the MC Track


  // Create STL map from MCtrack index to number of valid RichHits
  map<Int_t, Int_t> hitMap;

  // Loop over rich hits
  for (Int_t iHit = 0; iHit<nRichHits; iHit++) {
    pRhit = (CbmRichHit*) hitArray->At(iHit);
    if ( ! pRhit ) continue;
    ptIndex = pRhit->GetRefId();
    if (ptIndex < 0) continue;           // fake or background hit
    pMCpt = (CbmRichPoint*) (fMCRichPointArray->At(ptIndex));
    if ( ! pMCpt ) continue;
    mcTrackIndex = pMCpt->GetTrackID();
    if ( ! mcTrackIndex ) continue;
    pMCtr = (CbmMCTrack*)fMCTrackArray->At(mcTrackIndex);
    if ( ! pMCtr ) continue;
    if ( pMCtr->GetPdgCode() != 50000050) continue; // select only Cherenkov photons
    motherId = pMCtr->GetMotherId();
    hitMap[motherId]++;
 }

  // Create STL map from MCTrack index to RichRing index
  map<Int_t, Int_t> ringMap;

  // Create RICHRings for reconstructable MCTracks
  Int_t nMCacc  = 0;         // accepted MCTracks
  Int_t nRings  = 0;         // reconstructable RichRings
  Int_t nMCTracks = fMCTrackArray->GetEntriesFast();
  for (Int_t iMCTrack=0; iMCTrack<nMCTracks; iMCTrack++) {
    pMCtr = (CbmMCTrack*) fMCTrackArray->At(iMCTrack);
    if ( ! pMCtr ) continue;
//    if ( pMCtr->GetRichPoints() < 1 ) continue; // to ensure that there is a point corresponding to the track

//    nMCacc++;
    if ( hitMap[iMCTrack] < 3 ){
    continue;
    } // needs at least 3 hits to make a ring
    nMCacc++;

    new((*ringArray)[nRings]) CbmRichRing();
    ringMap[iMCTrack] = nRings++;
    if (fVerbose>1) cout << "-I- CbmRichRingFinderIdeal: RichRing "
			 << nRings << " created from MCTrack "
			 << iMCTrack << " with PdgCode "
			 << pMCtr->GetPdgCode() << " (" << pMCtr->GetNPoints(kRICH)
			 << " RICHPoints, max=15) and ("
			 << hitMap[iMCTrack] << " RichHits) ringMap: "
			 << ringMap[iMCTrack] << endl;
  }

  if (fVerbose>2) cout << "-I- CbmRichRingIdeal: " << endl;

  // Loop over RichHits. Get corresponding MCPoint and MCTrack index
  for (Int_t iHit = 0; iHit<nRichHits; iHit++) {
    pRhit = (CbmRichHit*) hitArray->At(iHit);
    if ( ! pRhit ) {
      cout << "-E- CbmRichRingFinderIdeal::DoFind: Empty slot "
	   << "in RichHitArray at position " << iHit << endl;
      nNoRichHit++;
      continue;
    }
    ptIndex = pRhit->GetRefId();

    if (ptIndex < 0) continue;       // fake or background hit
    pMCpt = (CbmRichPoint*) (fMCRichPointArray->At(ptIndex));
    if ( ! pMCpt ) {
      nNoRichPoint++;
      continue;
    }
    mcTrackIndex = pMCpt->GetTrackID();
    if ( ! mcTrackIndex ) continue;
    pMCtr = (CbmMCTrack*)fMCTrackArray->At(mcTrackIndex);
    if ( ! pMCtr ) continue;
    if ( pMCtr->GetPdgCode() != 50000050) continue;
    motherId = pMCtr->GetMotherId();

    if (motherId<0 || motherId>nMCTracks) {
      cout << "-E- CbmRichRingFinderIdeal::DoFind: "
	   << "MCTrack index out of range. " << motherId << " "
	   << nMCTracks << endl;
      nNoMCTrack++;
      continue;
    }

    if (ringMap.find(motherId) == ringMap.end()) continue;

    ringIndex = ringMap[motherId];

    pRing = (CbmRichRing*) ringArray->At(ringIndex);
    if ( ! pRing ) {
      cout << "-E- CbmRichRingFinderIdeal::DoFind: "
	   << "No RichRing pointer. " << iHit << " " << ptIndex
	   << " " << mcTrackIndex << " " << ringIndex << endl;
      nNoRichRing++;
      continue;
    }

  pRing->AddHit(iHit); // attach the hit to the ring

  if (fVerbose>2)
     cout << "Rich Hit " << iHit << " from RICHPoint "
			 << ptIndex << " (MCTrack "
			 << motherId << ") added to RICHRing "
			 << ringIndex << endl;
  }
  if (fVerbose) {
    cout << endl;
    cout << "-------------------------------------------------------"
	 << endl;
    cout << "-I-           Ideal RICH Ring finding               -I-"
	 << endl;
    cout << "RICH hits: " << nRichHits << endl;
    cout << "MCTracks: total " << nMCTracks << ", accepted " << nMCacc
	 << ", reconstructable Rich Rings : " << nRings << endl;
    if (nNoRichHit)   cout << "Rich Hit not found   : "
			  << nNoRichHit   << endl;
    if (nNoRichPoint) cout << "RichPoints not found : "
			  << nNoRichPoint << endl;
    if (nNoMCTrack)  cout << "MCTracks not found  : "
			  << nNoMCTrack  << endl;
    if (nNoRichRing)    cout << "RichRings not found : "
			  << nNoRichRing    << endl;
    cout << "-------------------------------------------------------"
	 << endl;
  }
  else cout << "-I- CbmRichRingFinderIdeal: all " << nMCTracks
	    << ", acc. " << nMCacc << ", rec. " << nRings << endl;

  return nRings;

}
// -------------------------------------------------------------------------

ClassImp(CbmRichRingFinderIdeal)
