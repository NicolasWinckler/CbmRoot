/**
*  $Id: CbmRichRingFinderIdeal.cxx,v 1.5 2006/02/01 13:16:58 hoehne Exp $
*
*  Class  : CbmRichRingFinderIdeal
*  Description: This is the implementation of the Ideal RichRingFinder. This
*               takes the Rich hits and associates them with the MC Track.
*
*  Author : Supriya Das
*  E-mail : S.Das@gsi.de
*
*/
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


CbmRichRingFinderIdeal::CbmRichRingFinderIdeal()
{
   fMCRichPointArray = NULL;
   fVerbose = 1;
}

CbmRichRingFinderIdeal::CbmRichRingFinderIdeal(
      Int_t verbose)
{
   fMCRichPointArray = NULL;
   fVerbose = verbose;
}

CbmRichRingFinderIdeal::~CbmRichRingFinderIdeal()
{
}

void CbmRichRingFinderIdeal::Init()
{
   FairRootManager* ioman = FairRootManager::Instance();
   if (! ioman) {
      cout << "-E- CbmRichRingFinderIdeal::Init: " << "RootManager not instantised!" << endl;
      return;
   }

   fMCTrackArray  = (TClonesArray*) ioman->GetObject("MCTrack");
   if (NULL == fMCTrackArray) {
      cout << "-E- CbmRichRingFinderIdeal::Init: No MCTrack array!" << endl;
      return;
   }

   fMCRichPointArray  = (TClonesArray*) ioman->GetObject("RichPoint");
   if (NULL == fMCRichPointArray) {
      cout << "-E- CbmRichRingFinderIdeal::Init: No MCPoint array!" << endl;
      return;
   }
}

Int_t CbmRichRingFinderIdeal::DoFind(
      TClonesArray* hitArray,
      TClonesArray* projArray,
		TClonesArray* ringArray)
{
   // Check pointers
   if ( NULL == fMCTrackArray ) {
      cout << "-E- CbmRichRingFinderIdeal::DoFind: " << "MCTrack array missing! " << endl;
      return -1;
   }
   if ( NULL == fMCRichPointArray ) {
      cout << "-E- CbmRichRingFinderIdeal::DoFind: " << "MCPoint array missing! " << endl;
      return -1;
   }

   if ( NULL == hitArray) {
      cout << "-E- CbmRichRingFinderIdeal::DoFind: " << "Hit array missing! " << hitArray << endl;
      return -1;
   }

   if ( NULL == projArray) {
      cout << "-E- CbmRichRingFinderIdeal::DoFind: "
            << "Track Projection array missing! --- not needed anyhow for Ideal Finder! "
            << projArray << endl;
   }

   if ( NULL == ringArray ) {
      cout << "-E- CbmRichRingFinderIdeal::DoFind: " << "Ring array missing! " << endl;
      return -1;
   }

   // Initialise control counters
   Int_t nNoMCTrack = 0;
   Int_t nNoRichPoint = 0;
   Int_t nNoRichHit = 0;
   Int_t nNoRichRing = 0;

   // Create STL map from MCtrack index to number of valid RichHits
   map<Int_t, Int_t> hitMap;
   Int_t nRichHits   = hitArray->GetEntriesFast();
   for (Int_t iHit = 0; iHit<nRichHits; iHit++) {
      CbmRichHit* pRhit = (CbmRichHit*) hitArray->At(iHit);
      if ( NULL == pRhit ) continue;
      Int_t ptIndex = pRhit->GetRefId();
      if (ptIndex < 0) continue; // fake or background hit
      CbmRichPoint* pMCpt = (CbmRichPoint*) (fMCRichPointArray->At(ptIndex));
      if ( NULL == pMCpt ) continue;
      Int_t mcTrackIndex = pMCpt->GetTrackID();
      if ( mcTrackIndex < 0 ) continue;
      CbmMCTrack* pMCtr = (CbmMCTrack*)fMCTrackArray->At(mcTrackIndex);
      if ( NULL == pMCtr ) continue;
      if ( pMCtr->GetPdgCode() != 50000050) continue; // select only Cherenkov photons
      Int_t motherId = pMCtr->GetMotherId();
      hitMap[motherId]++;
   }

   // Create STL map from MCTrack index to RichRing index
   map<Int_t, Int_t> ringMap;

   // Create RICHRings for MCTracks
   Int_t nRings  = 0; // number of RichRings
   Int_t nMCTracks = fMCTrackArray->GetEntriesFast();
   for (Int_t iMCTrack = 0; iMCTrack < nMCTracks; iMCTrack++) {
      CbmMCTrack* pMCtr = (CbmMCTrack*) fMCTrackArray->At(iMCTrack);
      if ( NULL == pMCtr ) continue;

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

   if (fVerbose > 2) cout << "-I- CbmRichRingIdeal: " << endl;

   // Loop over RichHits. Get corresponding MCPoint and MCTrack index
   for (Int_t iHit = 0; iHit<nRichHits; iHit++) {
      CbmRichHit* pRhit = (CbmRichHit*) hitArray->At(iHit);
      if ( NULL == pRhit ) {
         cout << "-E- CbmRichRingFinderIdeal::DoFind: Empty slot "
               << "in RichHitArray at position " << iHit << endl;
         nNoRichHit++;
         continue;
      }
      Int_t ptIndex = pRhit->GetRefId();

      if (ptIndex < 0) continue;// fake or background hit
      CbmRichPoint* pMCpt = (CbmRichPoint*) (fMCRichPointArray->At(ptIndex));
      if ( NULL == pMCpt ) {
         nNoRichPoint++;
         continue;
      }
      Int_t mcTrackIndex = pMCpt->GetTrackID();
      if ( mcTrackIndex < 0) continue;
      CbmMCTrack* pMCtr = (CbmMCTrack*)fMCTrackArray->At(mcTrackIndex);
      if ( NULL == pMCtr ) continue;
      if ( pMCtr->GetPdgCode() != 50000050) continue;
      Int_t motherId = pMCtr->GetMotherId();

      if (motherId < 0 || motherId > nMCTracks) {
         cout << "-E- CbmRichRingFinderIdeal::DoFind: "
               << "MCTrack index out of range. " << motherId << " "
               << nMCTracks << endl;
         nNoMCTrack++;
         continue;
      }

      if (ringMap.find(motherId) == ringMap.end()) continue;

      Int_t ringIndex = ringMap[motherId];

      CbmRichRing* pRing = (CbmRichRing*) ringArray->At(ringIndex);
      if ( NULL == pRing ) {
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
      cout << "-------------------------------------------------------" << endl;
      cout << "-I-           Ideal RICH Ring finding               -I-" << endl;
      cout << "RICH hits: " << nRichHits << endl;
      cout << "MCTracks: total " << nMCTracks << ", reconstructable Rich Rings : " << nRings << endl;
      if (nNoRichHit)   cout << "Rich Hit not found   : " << nNoRichHit   << endl;
      if (nNoRichPoint) cout << "RichPoints not found : " << nNoRichPoint << endl;
      if (nNoMCTrack)  cout << "MCTracks not found  : " << nNoMCTrack  << endl;
      if (nNoRichRing)    cout << "RichRings not found : " << nNoRichRing    << endl;
      cout << "-------------------------------------------------------" << endl;
   } else {
      cout << "-I- CbmRichRingFinderIdeal: all " << nMCTracks << ", rec. " << nRings << endl;
   }
   return nRings;
}

ClassImp(CbmRichRingFinderIdeal)
