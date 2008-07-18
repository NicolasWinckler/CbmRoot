// -------------------------------------------------------------------------
// -----                  CbmMuchMatchTracks source file                -----
// -----                  Created 10/10/07  by A. Lebedev               -----
// -------------------------------------------------------------------------
#include "CbmMuchMatchTracks.h"

#include "CbmMuchHit.h"
#include "CbmMuchTrack.h"
#include "CbmMuchTrackMatch.h"
#include "CbmMuchDigiMatch.h"

#include "CbmMCPoint.h"
#include "CbmRootManager.h"

#include "TClonesArray.h"

#include <iostream>
#include <iomanip>
#include <map>

using std::cout;
using std::endl;
using std::left;
using std::right;
using std::setw;
using std::fixed;
using std::setprecision;
using std::map;

// -----   Default constructor   -------------------------------------------
CbmMuchMatchTracks::CbmMuchMatchTracks() 
  : CbmTask("MuchMatchTracks") {
  fTracks  = NULL;
  fPoints  = NULL;
  fHits    = NULL;
  fMatches = NULL;
  fTime    = 0.;
  fNTrackMatches = 0.;
  fNAllHits      = 0.;   
  fNTrueHits     = 0.;
  fNEvents        = 0;
  fNEventsFailed  = 0;
}
// -------------------------------------------------------------------------



// -----   Standard constructor  -------------------------------------------
CbmMuchMatchTracks::CbmMuchMatchTracks(Int_t iVerbose) 
  : CbmTask("MuchMatchTracks", iVerbose) {
  fTracks  = NULL;
  fPoints  = NULL;
  fHits    = NULL;
  fMatches = NULL;
  fTime    = 0.;
  fNTrackMatches = 0.;
  fNAllHits      = 0.;   
  fNTrueHits     = 0.;
  fNEvents        = 0;
  fNEventsFailed  = 0;
}
// -------------------------------------------------------------------------



// -----   Constructor with task name   ------------------------------------
CbmMuchMatchTracks::CbmMuchMatchTracks(const char* name, Int_t iVerbose) 
  : CbmTask(name, iVerbose) {
  fTracks  = NULL;
  fPoints  = NULL;
  fHits    = NULL;
  fMatches = NULL;
  fTime    = 0.;
  fNTrackMatches = 0.;
  fNAllHits      = 0.;   
  fNTrueHits     = 0.;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmMuchMatchTracks::~CbmMuchMatchTracks() { }
// -------------------------------------------------------------------------



// -----   Virtual public method Exec   ------------------------------------
void CbmMuchMatchTracks::Exec(Option_t* opt) {

  // Timer
  fTimer.Start();
  
   // Clear output array
  fMatches->Clear();
 
  Bool_t warn = kFALSE;

  // Create some pointers and variables
  //CbmMuchTrack* track = NULL;
  //CbmMuchHit*   hit  = NULL;
  //CbmMCPoint*  point = NULL;
  
  
 // Int_t nHits    = 0;
  Int_t nMCTracks = 0;
 // Int_t iPoint    = 0;
 // Int_t iMCTrack  = 0;
  Int_t nAll      = 0;
  Int_t nTrue     = 0;
  Int_t nWrong    = 0;
  Int_t nFake     = 0;
  Int_t nHitSum     = 0;
  Int_t nTrueSum    = 0;
  Int_t nWrongSum   = 0;
  Int_t nFakeSum    = 0;
  Int_t nMCTrackSum = 0;
  
  
  map<Int_t, Int_t>::iterator it;

  // Loop over MuchTracks
  Int_t nofTracks = fTracks->GetEntriesFast();
  for (Int_t iTrack = 0; iTrack < nofTracks; iTrack++) {
    
    CbmMuchTrack* pTrack = (CbmMuchTrack*) fTracks->At(iTrack);
    
    if ( ! pTrack) {
      cout << "-W- CbmMuchMatchTracks::Exec: Empty MuchTrack at " 
           << iTrack << endl;
      warn = kTRUE;
      continue;
    }
    
    Int_t nofHits = pTrack->GetNHits();
       
    fMatchMap.clear();
    
    Int_t nofMultyPoints = 0;    
    // Loop over MuchHits on track
    for (Int_t iHit=0; iHit < nofHits; iHit++) {
      
      CbmMuchHit* pHit = (CbmMuchHit*) fHits->At(pTrack->GetHitIndex(iHit));
      
      if ( ! pHit ) {
         cout << "-E- CbmMuchMatchTracks::Exec: "
               << "No MuchHit " << iHit << " for track " << iTrack << endl;
         warn = kTRUE;
         continue;
      }
                
      Int_t digiIndex = pHit->GetDigi();
      CbmMuchDigiMatch* pDigiMatch = (CbmMuchDigiMatch*) fDigiMatch->At(digiIndex);    
      
      Int_t nofGoodPoints = 0;  
      Int_t nofFakePoints = 0;    
      for (Int_t iDigi = 0; iDigi < pDigiMatch->GetNPoints(); iDigi++) {
    
         Int_t pointIndex = pDigiMatch->GetRefIndex(iDigi);

         if ( pointIndex < 0 ) { // Fake or background hit
            nofFakePoints++;
            //nFake++;
            continue;
         } else {
            nofGoodPoints++;
         }
         
         CbmMCPoint* pPoint = (CbmMCPoint*) fPoints->At(pointIndex);
         
         if ( ! pPoint ) {
            cout << "-E- CbmMuchMatchTracks::Exec: "
                 << "Empty MCPoint " << pointIndex << " from MapsHit " << iHit
                 << " (track " << iTrack << ")" << endl;
            warn = kTRUE;
            continue;
         }
         
         Int_t mcTrackIndex = pPoint->GetTrackID();
               
         fMatchMap[mcTrackIndex]++;
         
      } // loop over digis    
      
      nofMultyPoints += nofGoodPoints - 1;  
      if (nofFakePoints > 0) nFake++;
      
    } // loop over hits

 
    // Search for best matching MCTrack
    nAll = nTrue = nWrong = nFake = nMCTracks = 0;
    Int_t bestMCTrackIndex = -1;
    for (it = fMatchMap.begin(); it!=fMatchMap.end(); it++) {
      if (fVerbose > 2) 
         cout << it->second 
              << " common points wth MCtrack " 
              << it->first << endl;
      
      nMCTracks++;
      nAll += it->second;
      if ( it->second > nTrue ) {
         bestMCTrackIndex = it->first;
         nTrue    = it->second;	
      }
    }
    nWrong = nAll - nTrue - nofMultyPoints;
    
    if (fVerbose > 1) cout << "-I- CbmMuchMatchTracks: MuchTrack " << iTrack 
                           << ", MCTrack " << bestMCTrackIndex << ", true " 
                           << nTrue << ", wrong " << nWrong << ", fake " 
                           << nFake << ", nofMultyPoints " 
                           << nofMultyPoints
                           << " ,#MCTracks " << nMCTracks << endl;

    // Create MuchTrackMatch
    new ((*fMatches)[iTrack]) CbmMuchTrackMatch(bestMCTrackIndex, nTrue, 
                                                nWrong, nFake, 
                                                nMCTracks);

    // Some statistics
    nHitSum     += nofHits;
    nTrueSum    += nTrue;
    nWrongSum   += nWrong;
    nFakeSum    += nFake;
    nMCTrackSum += nMCTracks;

  } // Track loop

  // Event statistics
  fTimer.Stop();
  Double_t qTrue = 0.;
  if ( nHitSum) qTrue  = Double_t(nTrueSum)  / Double_t(nHitSum) * 100.;
  if (fVerbose && nHitSum && nofTracks) {
    
    Double_t qWrong = Double_t(nWrongSum) / Double_t(nHitSum) * 100.;
    Double_t qFake  = Double_t(nFakeSum)  / Double_t(nHitSum) * 100.;
    Double_t qMC    = Double_t(nMCTrackSum) / Double_t(nofTracks);
    cout << endl;
    cout << "-------------------------------------------------------" 
	 << endl;
    cout << "-I-              Much Track Matching                 -I-"
	 << endl;
    cout << "Reconstructed MuchTracks : " << nofTracks << endl;;
    cout << "True  hit assignments   : " << qTrue  << " %" << endl;
    cout << "Wrong hit assignments   : " << qWrong << " %" << endl;
    cout << "Fake  hit assignments   : " << qFake  << " %" << endl;
    cout << "MCTracks per MuchTrack   : " << qMC << endl;
    cout << "--------------------------------------------------------" 
	 << endl;
  
  }
  else {
    if ( warn) cout << "- ";
    else       cout << "+ ";
    cout << setw(15) << left << fName << ": " << setprecision(4) << setw(8)
         << fixed << right << fTimer.RealTime() << " s, matches " 
         << nofTracks << ", hit quota " << qTrue << " %" << endl;
  }

  // Run statistics
  if ( warn ) fNEventsFailed++;
  else {
    fNEvents++;
    fTime          += fTimer.RealTime();
    fNTrackMatches += Double_t(nofTracks);
    fNAllHits      += Double_t(nHitSum);
    fNTrueHits     += Double_t(nTrueSum);
  }
  
}
// -------------------------------------------------------------------------



// -----   Virtual private method Init   -----------------------------------
InitStatus CbmMuchMatchTracks::Init() {
  
  // Get CbmRootManager
  CbmRootManager* ioman = CbmRootManager::Instance();
  if (! ioman) {
    cout << "-E- CbmMuchMatchTracks::Init: "
	 << "RootManager not instantised!" << endl;
    return kFATAL;
  }

  // Get Much hit Array
  fHits = (TClonesArray*) ioman->GetObject("MuchHit");
  if ( ! fHits) {
    cout << "-W- CbmMuchMatchTracks::Init: No MuchHit array!"
	 << endl;
  }

  // Get MuchTrack Array
  fTracks = (TClonesArray*) ioman->GetObject("MuchTrack");
  if ( ! fTracks ) {
    cout << "-E- CbmMuchMatchTracks::Init: No MuchTrack array!" << endl;
    return kERROR;
  }

  // Get MuchPoint array
  fPoints = (TClonesArray*) ioman->GetObject("MuchPoint");
  if ( ! fPoints ) {
    cout << "-E- CbmMuchMatchTracks::Init: No MuchPoint array!" << endl;
    return kERROR;
  }
  
  fDigiMatch = (TClonesArray*) ioman->GetObject("MuchDigiMatch");
  if ( ! fDigiMatch ) {
    cout << "-E- CbmMuchMatchTracks::Init: No MuchDigiMatch array!" << endl;
    return kERROR;
  }
  
  // Get Much digi array
  fMuchDigis  = (TClonesArray*) ioman->GetObject("MuchDigi");
  if ( ! fMuchDigis ) Fatal("Init", "No MuchDigis array!");
  
  // Create and register MuchTrackMatch array
  fMatches = new TClonesArray("CbmMuchTrackMatch",100);
  ioman->Register("MuchTrackMatch", "MUCH", fMatches, kTRUE);

  return kSUCCESS;

}
// -------------------------------------------------------------------------



    
// -----   Virtual private method Finish   ---------------------------------
void CbmMuchMatchTracks::Finish() {

  cout << endl;
  cout << "============================================================"
       << endl;
  cout << "=====   " << GetName() << ": Run summary " << endl;
  cout << "===== " << endl;
  cout << "===== Good events   : " << setw(6) << fNEvents << endl;
  cout << "===== Failed events : " << setw(6) << fNEventsFailed << endl;
  cout << "===== Average time  : " << setprecision(4) << setw(8) << right
       << fTime / Double_t(fNEvents)  << " s" << endl;
  cout << "===== " << endl;
  cout << "===== Tracks per event  : " << fixed << setprecision(0)
       << fNTrackMatches / Double_t(fNEvents) << endl;
  cout << setprecision(2);
  cout << "===== True hits         : " << fixed << setw(6) << right
       << fNTrueHits / fNAllHits * 100. << " %" << endl;
  cout << "============================================================"
       << endl;

}
// -------------------------------------------------------------------------


    
ClassImp(CbmMuchMatchTracks)


      

      
