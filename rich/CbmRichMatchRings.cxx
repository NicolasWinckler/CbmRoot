/******************************************************************************
*  $Id: CbmRichMatchRings.cxx,v 1.5 2006/09/13 15:05:38 hoehne Exp $
*
*  Class      : CbmRichMatchRings
*  Description: Implementation of CbmMatchRings Task
*  Author     : Supriya Das (following CbmStsMatchTracks)
*  E-mail     : S.Das@gsi.de
*
*******************************************************************************
*  $Log: CbmRichMatchRings.cxx,v $
*  Revision 1.5  2006/09/13 15:05:38  hoehne
*  reject selection line for fake hits -> to be done in analysis
*
*  Revision 1.4  2006/08/04 13:46:17  hoehne
*  counting of MC hits corrected, additional requirement implemented: correctly reconstructed ring should have at
*  least 50% of the MC hits (S. Das)
*
*  Revision 1.3  2006/06/19 11:19:52  turany
*  map<Int_t, Int_t>iterator it do not need to be set to zero, it will be reseted by calling it.begin(), putting it=0 gives a compiler error with the new compilers
*
*  Revision 1.2  2006/02/22 09:15:46  hoehne
*  store also number of MC hits for each ring (Simeon Lebedev)
*
*  Revision 1.1  2006/01/23 11:42:44  hoehne
*  initial version: task to match reconstructed to MC rings
*
*
*******************************************************************************/
#include "CbmRichMatchRings.h"

#include "CbmRichHit.h"
#include "CbmRichRing.h"
#include "CbmRichRingMatch.h"

#include "CbmMCPoint.h"
#include "CbmMCTrack.h"
#include "CbmRootManager.h"

#include "TClonesArray.h"

#include <iostream>
#include <map>

using std::cout;
using std::endl;
using std::map;


// -----   Default constructor   -------------------------------------------
CbmRichMatchRings::CbmRichMatchRings()
  : CbmTask("RICH Ring match") {
  fRings      = NULL;
  fPoints     = NULL;
  fHits       = NULL;
  fMatches    = NULL;
  fVerbose    = 1;
}
// -------------------------------------------------------------------------



// -----   Constructor with verbosity level   ------------------------------
CbmRichMatchRings::CbmRichMatchRings(Int_t verbose)
  : CbmTask("RICH Ring match") {
  fRings      = NULL;
  fPoints     = NULL;
  fHits       = NULL;
  fMatches    = NULL;
  fVerbose    = verbose;
}
// -------------------------------------------------------------------------



// -----   Constructor with name, title and verbosity  ---------------------
CbmRichMatchRings::CbmRichMatchRings(const char* name, const char* title,
				     Int_t verbose)
  : CbmTask(name) {
  fRings      = NULL;
  fPoints     = NULL;
  fHits       = NULL;
  fMatches    = NULL;
  fVerbose    = verbose;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmRichMatchRings::~CbmRichMatchRings() { }
// -------------------------------------------------------------------------




// -----   Public method Init   --------------------------------------------
InitStatus CbmRichMatchRings::Init() {
  
  // Get CbmRootManager
  CbmRootManager* ioman = CbmRootManager::Instance();
  if (! ioman) {
    cout << "-E- CbmRichMatchRings::Init: "
	 << "RootManager not instantised!" << endl;
    return kFATAL;
  }
  
  // Get hit Array
  fHits
    = (TClonesArray*) ioman->GetObject("RichHit");
  if ( ! fHits) {
    cout << "-W- CbmRichMatchRings::Init: No RichHit array!"
	 << endl;
  }
  
  // Get RichRing Array
  fRings = (TClonesArray*) ioman->GetObject("RichRing");
  if ( ! fRings ) {
    cout << "-E- CbmRichMatchRings::Init: No RichRing array!" << endl;
    return kERROR;
  }
  
  // Get MC Point array
  fPoints = (TClonesArray*) ioman->GetObject("RichPoint");
  if ( ! fPoints ) {
    cout << "-E- CbmRichMatchRings::Init: No RichPoint array!" << endl;
    return kERROR;
  }
  
  // Get MC Point array
  fTracks = (TClonesArray*) ioman->GetObject("MCTrack");
  if ( ! fTracks ) {
    cout << "-E- CbmRichMatchRings::Init: No MCTrack array!" << endl;
    return kERROR;
  }
  
  // Create and register RichRingMatch array
  fMatches = new TClonesArray("CbmRichRingMatch",100);
  ioman->Register("RichRingMatch", "RICH", fMatches, kTRUE);
  
  return kSUCCESS;
  
}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void CbmRichMatchRings::Exec(Option_t* opt) {

  // Clear output array
  fMatches->Clear();

  // Create some pointers and variables
  CbmRichRing* ring  = NULL;
  CbmRichHit* hit    = NULL;
  CbmMCPoint* point  = NULL;
  CbmMCTrack* track  = NULL;

  Int_t nHits        = 0;
  Int_t nMCHits      = 0;
  Int_t nMCTracks    = 0;
  Int_t iPoint       = 0;
//  Int_t iFlag        = 0;
  Int_t iMCTrack     = 0;
  Int_t iMother      = 0;
  Int_t nAll         = 0;
  Int_t nTrue        = 0;
  Int_t nWrong       = 0;
  Int_t nFake        = 0;
  Int_t nHitSum      = 0;
  Int_t nTrueSum     = 0;
  Int_t nWrongSum    = 0;
  Int_t nFakeSum     = 0;
  Int_t nMCTrackSum  = 0;
  map<Int_t, Int_t>::iterator it ;
  //-------S.Lebedev-------------
  //calculate number of hits in MC ring to
  //compare its with number of hits in found ring
  
  fMatchMCMap.clear();
  
  // Loop over Rich hits
  Int_t nRichHits = fHits->GetEntriesFast();
  for (Int_t iHit=0; iHit < nRichHits; iHit++) {
    hit = (CbmRichHit*) fHits->At(iHit);
    if ( ! hit ) {
      cout << "-E- CbmRichMatchRings::Exec: "
	   << "No Hit " << iHit << endl;
      continue;
    }
    iPoint = hit->GetRefIndex();
    
    if ( iPoint < 0 ) {        // Fake or background hit
      nFake++;
      continue;
    }
    
    //Get the MC Point corresponding to the hit
    
    point = (CbmMCPoint*) fPoints->At(iPoint);
    if ( ! point ) {
      cout << "-E- CbmRichMatchRings::Exec: "
	   << "Empty MCPoint " << iPoint << " from Hit " << iHit
	   << endl;
      continue;
    }
    //Get the MC Track ID corresponding to the MC Point
    iMCTrack = point->GetTrackID();
    // Get the MC Track corresponding to the ID
    track   = (CbmMCTrack*)fTracks->At(iMCTrack);
    iMother = track->GetMotherId();
    fMatchMCMap[iMother]++;
  }
  
  
  // Loop over RichRings
  Int_t nRings = fRings->GetEntriesFast();
  for (Int_t iRing=0; iRing<nRings; iRing++) {
    ring = (CbmRichRing*) fRings->At(iRing);
    
    if ( ! ring) {
      cout << "-W- CbmRichMatchRings::Exec: Empty RichRing at "
	   << iRing << endl;
      continue;
    }
    
    nHits = ring->GetNofHits();
    nAll = nTrue = nWrong = nFake = nMCTracks = 0;
    fMatchMap.clear();
    
    if (fVerbose > 2) cout << endl << "Ring " << iRing << ", Hits "<<nHits << endl;
    
    // Loop over Hits of ring
    for (Int_t iHit=0; iHit<nHits; iHit++) {
      hit = (CbmRichHit*) fHits->At(ring->GetHit(iHit));
      
      if ( ! hit ) {
	cout << "-E- CbmRichMatchRings::Exec: "
	     << "No Hit " << iHit << " for ring " << iRing << endl;
	continue;
      }
      
      iPoint = hit->GetRefIndex();
      /*
	iFlag  = mHit->GetFlag();
	if ( iFlag ) {
	cout << "-W- CbmStsMatchTracks::Exec: "
	<< "Invalid MapsHit was attached to track " << iTrack << endl;
	continue;
	}
      */
      if ( iPoint < 0 ) {        // Fake or background hit
	nFake++;
	continue;
      }
      
      //Get the MC Point corresponding to the hit
      
      point = (CbmMCPoint*) fPoints->At(iPoint);
      if ( ! point ) {
	cout << "-E- CbmRichMatchRings::Exec: "
	     << "Empty MCPoint " << iPoint << " from Hit " << iHit
	     << " (ring " << iRing << ")" << endl;
	continue;
      }
      
      //Get the MC Track ID corresponding to the MC Point
      
      iMCTrack = point->GetTrackID();
      if ( fVerbose > 2 ) cout << "Ring " << iRing << ", hit "
			       << ring->GetHit(iHit)
			       << ", RichPoint " << iPoint << ", MCTrack "
			       << iMCTrack << endl;
      
      // Get the MC Track corresponding to the ID
      
      track   = (CbmMCTrack*)fTracks->At(iMCTrack);
      iMother = track->GetMotherId();
      fMatchMap[iMother]++;
      
    } //Hit loop

    // Search for best matching MCTrack
    iMCTrack = -1;
    for (it=fMatchMap.begin(); it!=fMatchMap.end(); it++) {
      if (fVerbose > 2) cout << it->second
			     << " common points wth MCtrack "
			     << it->first << endl;
      nMCTracks++;
      nAll += it->second;
      if ( it->second > nTrue ) {
	iMCTrack = it->first;
	nTrue    = it->second;
      }
    }
    nMCHits = fMatchMCMap[iMCTrack];//number of hits in MC ring
//    if(Float_t(nTrue)/Float_t(nMCHits) < 0.5) iMCTrack =-1;
    
    nWrong = nAll - nTrue;
    if (fVerbose>1)
      cout<< "-I- CbmRingMatchRings: RichRing " << iRing
	  << ", MCTrack " << iMCTrack <<", nMCHits "<<nMCHits
	  << ", true " << nTrue << ", wrong " << nWrong
	  << ", fake " << nFake << ", #MCTracks "
	  <<  nMCTracks << endl;
    
    // Create RichRingMatch
    new ((*fMatches)[iRing]) CbmRichRingMatch(iMCTrack, nTrue,
					      nWrong, nFake,
					      nMCHits, nMCTracks);
    
    // Set the MCMotherID for the ring
    ring->SetMCMotherID(iMCTrack);
    
    // Some statistics
    nHitSum     += nHits;
    nTrueSum    += nTrue;
    nWrongSum   += nWrong;
    nFakeSum    += nFake;
    nMCTrackSum += nMCTracks;
    
  } // Ring loop

  // Event statistics
  Double_t qTrue = 0.;
  if ( nHitSum) qTrue  = Double_t(nTrueSum)  / Double_t(nHitSum) * 100.;
  if (fVerbose) {
    Double_t qWrong = Double_t(nWrongSum) / Double_t(nHitSum) * 100.;
    Double_t qFake  = Double_t(nFakeSum)  / Double_t(nHitSum) * 100.;
    Double_t qMC    = Double_t(nMCTrackSum) / Double_t(nRings);
    cout << endl;
    cout << "-------------------------------------------------------"
	 << endl;
    cout << "-I-              RICH Ring Matching                 -I-"
	 << endl;
    cout << "Reconstructed Rich Rings : " << nRings << endl;;
    cout << "True  hit assignments    : " << qTrue  << " %" << endl;
    cout << "Wrong hit assignments    : " << qWrong << " %" << endl;
    cout << "Fake  hit assignments    : " << qFake  << " %" << endl;
    cout << "MCTracks per RichRing    : " << qMC << endl;
    cout << "--------------------------------------------------------"
	 << endl;
  }
  else cout << "-I- CbmRichMatchRings: rec. " << nRings << ", quota "
	    << qTrue << " % " << endl;

}
// -------------------------------------------------------------------------



// -----   Public method Finish   ------------------------------------------
void CbmRichMatchRings::Finish() { }
// -------------------------------------------------------------------------


    
ClassImp(CbmRichMatchRings)


      

      
