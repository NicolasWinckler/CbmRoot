
// -------------------------------------------------------------------------
// -----                  CbmTrdFindTracksQa source file               -----
// -----                  Created 06/07/06  by A. Lebedev               -----
// -------------------------------------------------------------------------

#include "CbmTrdFindTracksQa.h"

#include "CbmTrdTrack.h"
#include "CbmTrdTrackMatch.h"

#include "CbmGeoNode.h"
#include "CbmGeoPassivePar.h"
#include "CbmGeoVector.h"
#include "CbmMCTrack.h"
#include "CbmRootManager.h"
#include "CbmRunAna.h"
#include "CbmRuntimeDb.h"
#include "CbmStsTrack.h"
#include "CbmStsTrackMatch.h"

#include "TClonesArray.h"
#include "TDirectory.h"
#include "TH1F.h"
#include "TList.h"
#include "TVector3.h"

#include <iostream>
#include <map>
using std::cout;
using std::endl;
using std::map;

// -----   Default constructor   -------------------------------------------
CbmTrdFindTracksQa::CbmTrdFindTracksQa()
  : CbmTask("STS Track Finder QA", 1) { 
  fMinPoints = 12;
  fQuota     = 0.7;
  fNormType = 1;
  fVerbose = 1;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmTrdFindTracksQa::CbmTrdFindTracksQa(Int_t minPoints, Double_t quota,
                                          Int_t iVerbose) 
  : CbmTask("STS Track Finder QA", iVerbose) {
  fMinPoints = minPoints;
  fQuota     = quota;
    fNormType = 1;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmTrdFindTracksQa::~CbmTrdFindTracksQa() { 

  fHistoList->Delete();
  delete fHistoList;
}
// -------------------------------------------------------------------------



// -----   Public method SetParContainers   --------------------------------
void CbmTrdFindTracksQa::SetParContainers() {

  // Get Run
  CbmRunAna* run = CbmRunAna::Instance();
  if ( ! run ) {
    cout << "-E- " << GetName() << "::SetParContainers: No CbmRunAna!"
         << endl;
    return;
  }

  // Get Runtime Database
  CbmRuntimeDb* runDb = run->GetRuntimeDb();
  if ( ! run ) {
    cout << "-E- " << GetName() << "::SetParContainers: No runtime database!"
         << endl;
    return;
  }

  // Get passive geometry parameters
  fPassGeo = (CbmGeoPassivePar*) runDb->getContainer("CbmGeoPassivePar");
  if ( ! fPassGeo ) {
    cout << "-E- " << GetName() << "::SetParContainers: "
         << "No passive geometry parameters!" << endl;
    return;
  }
}
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
InitStatus CbmTrdFindTracksQa::Init() {

  cout << "==========================================================="
       << endl;;
  cout << GetName() << ": Initialising..." << endl;

  // Get RootManager
  CbmRootManager* ioman = CbmRootManager::Instance();
  if (! ioman) {
    cout << "-E- " << GetName() << "::Init: "
         << "RootManager not instantised!" << endl;
    return kFATAL;
  }

  // Get MCTrack array
  fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
  if ( ! fMCTracks ) {
    cout << "-E- " << GetName() << "::Init: No MCTrack array!" << endl;
    return kFATAL;
  }

  if (fNormType == 2) {
    // Get StsTrack array
    fStsTracks = (TClonesArray*) ioman->GetObject("STSTrack");
    if ( ! fStsTracks ) {
      cout << "-E- " << GetName() << "::Init: No StsTrack array!" << endl;
      return kERROR;
    }

    // Get StsTrackMatch array
    fStsMatches = (TClonesArray*) ioman->GetObject("STSTrackMatch");
    if ( ! fStsMatches ) {
      cout << "-E- " << GetName() << "::Init: No StsTrackMatch array!"
           << endl;
      return kERROR;
    }
  }

  // Get TrdTrack array
  fTrdTracks = (TClonesArray*) ioman->GetObject("TRDTrack");
  if ( ! fTrdTracks ) {
    cout << "-E- " << GetName() << "::Init: No TrdTrack array!" << endl;
    return kERROR;
  }

  // Get TrdTrackMatch array
  fTrdMatches = (TClonesArray*) ioman->GetObject("TRDTrackMatch");
  if ( ! fTrdMatches ) {
    cout << "-E- " << GetName() << "::Init: No TrdTrackMatch array!"
         << endl;
    return kERROR;
  }


  // Get the geometry of target
  InitStatus geoStatus = GetGeometry();
  if ( geoStatus != kSUCCESS ) {
    cout << "-E- " << GetName() << "::Init: Error in reading geometry!"
         << endl;
    return geoStatus;
  }

  // Create histograms
  CreateHistos();
  Reset();

  // Output
  cout << "   Minimum number of TRD points : " << fMinPoints << endl;
  cout << "   Matching quota               : " << fQuota << endl;
  if (fActive) cout << "   *****   Task is ACTIVE   *****" << endl;
  cout << "==========================================================="
       << endl << endl;

  return geoStatus;

}
// -------------------------------------------------------------------------



// -----   Public method ReInit   ------------------------------------------
InitStatus CbmTrdFindTracksQa::ReInit() {

  cout << "==========================================================="
       << endl;;
  cout << GetName() << ": Reinitialising..." << endl;

  // Get the geometry of target
  InitStatus geoStatus = GetGeometry();
  if ( geoStatus != kSUCCESS ) {
    cout << "-E- " << GetName() << "::ReInit: Error in reading geometry!"
         << endl;
    return geoStatus;
  }

  // Output
  cout << "   Target position ( " << fTargetPos.X() << ", "
       << fTargetPos.Y() << ", " << fTargetPos.Z() << ") " << endl;
  cout << "==========================================================="
       << endl << endl;

  return geoStatus;
}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void CbmTrdFindTracksQa::Exec(Option_t* opt) {

  fMatchSetSts.clear();

  if (fNormType == 2) { // Normalization to number of Sts tracks
    Int_t nStsTrack = fStsTracks->GetEntriesFast();
    for(Int_t iStsTrack = 0; iStsTrack < nStsTrack; iStsTrack++) {

      CbmStsTrack* stsTrack = (CbmStsTrack*) fStsTracks->At(iStsTrack);
      if ( ! stsTrack ) {
        cout << "-E- " << GetName() << "::Exec: "
             << "No StsTrack at index " << iStsTrack << endl;
        Fatal("Exec", "No StsTrack in array");
      }
      CbmStsTrackMatch* stsTrackM = (CbmStsTrackMatch*) fStsMatches->
                        At(iStsTrack);
      if ( ! stsTrackM ) {
        cout << "-E- " << GetName() << "::Exec: "
	     << "No StsTrackMatch at index " << iStsTrack << endl;
        Fatal("Exec", "No StsTrackMatch in array");
      }

      Int_t iMC = stsTrackM->GetMCTrackId();
      if(iMC == -1) continue;

      fMatchSetSts.insert(iMC);
    }

  }

  // Clear matching map and reset eventwise counters
  fMatchMap.clear();
  fQualiMap.clear();

  // Loop over TrdTracks. Check matched MCtrack and fill maps.
  Int_t nGhosts = 0;
  Int_t nClones = 0;
  Int_t nRec = fTrdTracks->GetEntriesFast();
  Int_t nMtc = fTrdMatches->GetEntriesFast();
  if ( nMtc != nRec ) {
    cout << "-E- " << GetName() << "::Exec: Number of TrdMatches ("
         << nMtc << ") does not equal number of TrdTracks ("
         << nRec << ")" << endl;
    Fatal("Exec", "Inequal number of TrdTrack and TrdTrackMatch");
  }
  for (Int_t iRec=0; iRec<nRec; iRec++) {

    CbmTrdTrack* trdTrack = (CbmTrdTrack*) fTrdTracks->At(iRec);
    if ( ! trdTrack ) {
      cout << "-E- " << GetName() << "::Exec: "
           << "No TrdTrack at index " << iRec << endl;
      Fatal("Exec", "No TrdTrack in array");
      }
    Int_t nHits = trdTrack->GetNofTrdHits();

    CbmTrdTrackMatch* match = (CbmTrdTrackMatch*) fTrdMatches->At(iRec);
    if ( ! match ) {
      cout << "-E- " << GetName() << "::Exec: "
            << "No TrdTrackMatch at index " << iRec << endl;
      Fatal("Exec", "No TrdTrackMatch in array");
    }
    Int_t nTrue = match->GetNofTrueHits();

    Int_t iMC = match->GetMCTrackID();
    if (iMC == -1 ) {       // no common point with MC, really ghastly!
      if ( fVerbose > 1 )
      cout << "-I- " << GetName() << ":"
           << "No MC match for TrdTrack " << iRec << endl;
      fhNhGhosts->Fill(nHits);
      nGhosts++;
      continue;
    }

    // Check matching criterion (quota)
    Double_t quali = Double_t(nTrue) / Double_t(nHits);
    if ( quali >= fQuota ) {

      // No previous match for this MCTrack
      if ( fMatchMap.find(iMC) == fMatchMap.end() ) {
         fMatchMap[iMC] = iRec;
         fQualiMap[iMC] = quali;
      }

      // Previous match; take the better one
      else {
      if ( fVerbose > 1 )
          cout << "-I- " << GetName() << ": "
               << "MCTrack " << iMC << " doubly matched."
               << "Current match " << iRec
               << ", previous match " << fMatchMap[iMC]
               << endl;
         if ( fQualiMap[iMC] < quali ) {
            CbmTrdTrack* oldTrack
                  = (CbmTrdTrack*) fTrdTracks->At(fMatchMap[iMC]);
            fhNhClones->Fill(Double_t(oldTrack->GetNofTrdHits()));
            fMatchMap[iMC] = iRec;
            fQualiMap[iMC] = quali;
         }
         else fhNhClones->Fill(nHits);
         nClones++;
      }

    }

    // If not matched, it's a ghost
    else {
      if ( fVerbose > 1 )
      cout << "-I- " << GetName() << ":"
           << "TrdTrack " << iRec << " below matching criterion "
           << "(" << quali << ")" << endl;
      fhNhGhosts->Fill(nHits);
      nGhosts++;
    }

  }   // Loop over TrdTracks


  // Loop over MCTracks
  Int_t nAll     = 0;
  Int_t nAcc     = 0;
  Int_t nRecAll  = 0;
  Int_t nPrim    = 0;
  Int_t nRecPrim = 0;
  Int_t nRef     = 0;
  Int_t nRecRef  = 0;
  Int_t nSec     = 0;
  Int_t nRecSec  = 0;

  TVector3 vertex;
  Int_t nMC = fMCTracks->GetEntriesFast();
  for (Int_t iMC=0; iMC<nMC; iMC++) {
    CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(iMC);
    if ( ! mcTrack ) {
      cout << "-E- " << GetName() << "::Exec: "
           << "No MCTrack at index " << iMC
           << endl;
      Fatal("Exec", "No MCTrack in array");
    }

    // Check geometrical acceptance; continue only for accepted tracks
    nAll++;
    Int_t nPoints = mcTrack->GetNPoints(kTRD);
    if ( nPoints < fMinPoints ) continue;

    if ( fNormType == 2 &&
        (fMatchSetSts.find(iMC) == fMatchSetSts.end()) ) continue;

    nAcc++;

    // Check origin of MCTrack
    mcTrack->GetStartVertex(vertex);
    Bool_t isPrim = kFALSE;
    if ( (vertex-fTargetPos).Mag() < 1. ) {
      isPrim = kTRUE;
      nPrim++;
    }
    else nSec++;

    // Get momentum
    Double_t mom = mcTrack->GetP();
    Bool_t isRef = kFALSE;
    if ( mom > 1. && isPrim) {
      isRef = kTRUE;
      nRef++;
    }

    // Fill histograms for accepted tracks
    fhMomAccAll->Fill(mom);
    fhNpAccAll->Fill(Double_t(nPoints));
    if ( isPrim) {
      fhMomAccPrim->Fill(mom);
      fhNpAccPrim->Fill(Double_t(nPoints));
    }
    else {
      fhMomAccSec->Fill(mom);
      fhNpAccSec->Fill(Double_t(nPoints));
      fhZAccSec->Fill(vertex.Z());
    }

    // Get matched TrdTrack
    Int_t    iRec  = -1;
    Double_t quali =  0.;
    Bool_t   isRec = kFALSE;
    if (fMatchMap.find(iMC) != fMatchMap.end() ) {
      iRec  = fMatchMap[iMC];
      isRec = kTRUE;
      CbmTrdTrack* trdTrack = (CbmTrdTrack*) fTrdTracks->At(iRec);
      if ( ! trdTrack ) {
         cout << "-E- " << GetName() << "::Exec: "
              << "No TrdTrack for matched MCTrack " << iMC << endl;
         Fatal("Exec", "No TrdTrack for matched MCTrack");
      }
      quali = fQualiMap[iMC];
      if ( quali < fQuota ) {
         cout << "-E- " << GetName() << "::Exec: "
              << "Matched TrdTrack " << iRec << " is below matching "
              << "criterion ( " << quali << ")" << endl;
         Fatal("Exec", "Match below matching quota");
      }
      CbmTrdTrackMatch* match = (CbmTrdTrackMatch*) fTrdMatches->At(iRec);
      if ( ! match ) {
         cout << "-E- " << GetName() << "::Exec: "
              << "No TrdTrackMatch for matched MCTrack " << iMC << endl;
         Fatal("Exec", "No TrdTrackMatch for matched MCTrack");
      }
      Int_t nTrue  = match->GetNofTrueHits();
      Int_t nWrong = match->GetNofWrongHits();
      Int_t nFake  = match->GetNofFakeHits();
      Int_t nHits  = trdTrack->GetNofTrdHits();
      if ( nTrue + nWrong + nFake != nHits ) {
         cout << "True " << nTrue << " wrong " << nWrong << " Fake "
              << nFake << " Hits " << nHits << endl;
         Fatal("Exec", "Wrong number of hits");
      }

      // Verbose output
      if ( fVerbose > 1 )
         cout << "-I- " << GetName() << ": "
              << "MCTrack " << iMC << ", points "
              << nPoints << ", TrdTrack " << iRec << ", hits " << nHits
              << ", true hits " << nTrue << endl;

      // Fill histograms for reconstructed tracks
      nRecAll++;
      fhMomRecAll->Fill(mom);
      fhNpRecAll->Fill(Double_t(nPoints));
      if ( isPrim ) {
         nRecPrim++;
         fhMomRecPrim->Fill(mom);
         fhNpRecPrim->Fill(Double_t(nPoints));
         if ( isRef ) nRecRef++;
      }
      else {
         nRecSec++;
         fhMomRecSec->Fill(mom);
         fhNpRecSec->Fill(Double_t(nPoints));
         fhZRecSec->Fill(vertex.Z());
      }

    }  // Match found in map?

  } // Loop over MCTracks


  // Calculate efficiencies
  Double_t effAll  = Double_t(nRecAll)  / Double_t(nAcc);
  Double_t effPrim = Double_t(nRecPrim) / Double_t(nPrim);
  Double_t effRef  = Double_t(nRecRef)  / Double_t(nRef);
  Double_t effSec  = Double_t(nRecSec)  / Double_t(nSec);
  Double_t effGhosts = Double_t(nGhosts) / Double_t(nAcc);
  Double_t effClones = Double_t(nClones) / Double_t(nAcc);


  // Event summary
  if ( fVerbose > 0 ) {
    cout << "----------   TrdFindTracksQaA : Event summary   ------------"
         << endl;
    cout << "MCTracks   : " << nAll << ", accepted: " << nAcc
         << ", reconstructed: " << nRecAll << endl;
    cout << "All        : accepted: " << nAcc << ", reconstructed: "
         << nRecAll << ", efficiency " << effAll*100. << "%" << endl;
    cout << "Vertex     : accepted: " << nPrim << ", reconstructed: "
         << nRecPrim << ", efficiency " << effPrim*100. << "%" << endl;
    cout << "Reference  : accepted: " << nRef  << ", reconstructed: "
         << nRecRef  << ", efficiency " << effRef*100. << "%" << endl;
    cout << "Non-vertex : accepted: " << nSec << ", reconstructed: "
         << nRecSec << ", efficiency " << effSec*100. << "%" << endl;
    cout << "Ghosts : " << nGhosts << ", ghosts/accepted MC tracks: "
         << effGhosts*100. << "%" << endl;
    cout << "Clones : " << nClones << ", clones/accepted MC tracks: "
         << effClones*100. << "%" << endl;
    cout << "TRDTracks " << nRec << ", ghosts " << nGhosts
         << ", clones " << nClones << endl;
    cout << "-----------------------------------------------------------"
         << endl;
    cout << endl;
  }
  else cout << "-I- " << GetName() << ": all " << effAll*100. << " %, prim. "
      << effPrim*100. << " %, ref. " << effRef*100. << " %" << endl;

  // Increase counters
  fNAccAll  += nAcc;
  fNAccPrim += nPrim;
  fNAccRef  += nRef;
  fNAccSec  += nSec;
  fNRecAll  += nRecAll;
  fNRecPrim += nRecPrim;
  fNRecRef  += nRecRef;
  fNRecSec  += nRecSec;
  fNGhosts  += nGhosts;
  fNClones  += nClones;
  fNEvents++;

}
// -------------------------------------------------------------------------



// -----   Private method Finish   -----------------------------------------
void CbmTrdFindTracksQa::Finish() {

  // Divide histograms for efficiency calculation
  DivideHistos(fhMomRecAll,  fhMomAccAll,  fhMomEffAll);
  DivideHistos(fhMomRecPrim, fhMomAccPrim, fhMomEffPrim);
  DivideHistos(fhMomRecSec,  fhMomAccSec,  fhMomEffSec);
  DivideHistos(fhNpRecAll,   fhNpAccAll,   fhNpEffAll);
  DivideHistos(fhNpRecPrim,  fhNpAccPrim,  fhNpEffPrim);
  DivideHistos(fhNpRecSec,   fhNpAccSec,   fhNpEffSec);
  DivideHistos(fhZRecSec,    fhZAccSec,    fhZEffSec);

  // Normalise histos for clones and ghosts to one event
  if ( fNEvents ) {
    fhNhClones->Scale(1./Double_t(fNEvents));
    fhNhGhosts->Scale(1./Double_t(fNEvents));
  }

  // Calculate integrated efficiencies and rates
  Double_t effAll  = Double_t(fNRecAll)  / Double_t(fNAccAll);
  Double_t effPrim = Double_t(fNRecPrim) / Double_t(fNAccPrim);
  Double_t effRef  = Double_t(fNRecRef)  / Double_t(fNAccRef);
  Double_t effSec  = Double_t(fNRecSec)  / Double_t(fNAccSec);
  Double_t rateGhosts = Double_t(fNGhosts) / Double_t(fNEvents);
  Double_t rateClones = Double_t(fNClones) / Double_t(fNEvents);
  Double_t effGhosts = Double_t(fNGhosts) / Double_t(fNAccAll);
  Double_t effClones = Double_t(fNClones) / Double_t(fNAccAll);

  // Run summary to screen
  cout << endl << endl;
  cout << "=======================================================" 
       << endl;
  cout << "            TrdFindTracksQaA: Run summary" << endl << endl;
  cout << "Events analysed : " << fNEvents << endl;
  cout << "Efficiency all tracks       : " << effAll*100 << " % (" 
       << fNRecAll << "/" << fNAccAll <<")" << endl;
  cout << "Efficiency vertex tracks    : " << effPrim*100 << " % (" 
       << fNRecPrim << "/" << fNAccPrim <<")" << endl;
  cout << "Efficiency reference tracks : " << effRef*100 << " % (" 
       << fNRecRef << "/" << fNAccRef <<")" << endl;
  cout << "Efficiency secondary tracks : " << effSec*100 << " % (" 
       << fNRecSec << "/" << fNAccSec <<")" << endl;
  cout << "Ghost rate " << rateGhosts << " per event, "
       << "ghosts/accepted MC tracks :" << effGhosts*100 << "% ("
       << fNGhosts << "/" << fNAccAll << ")" << endl;
  cout << "Clone rate " << rateClones << " per event, "
       << "clones/accepted MC tracks :" << effClones*100 << "% ("
       << fNClones << "/" << fNAccAll << ")" << endl;
  cout << "=======================================================" 
       << endl;
  cout << endl << endl;

  TDirectory *olddir = gDirectory;
  TDirectory *hdir = new TDirectory("TrdFinderQa",
				    "Performance of the track finding in TRD");
  hdir->cd();
  TIter next(fHistoList);
  while ( TH1* histo = ((TH1*)next()) ) histo->Write();
  olddir->cd();

}
// -------------------------------------------------------------------------



// -----   Private method GetGeometry   ------------------------------------
InitStatus CbmTrdFindTracksQa::GetGeometry() {

  // Get target geometry
  if ( ! fPassGeo ) {
    cout << "-W- " << GetName() << "::GetGeometry: No passive geometry!"
         <<endl;
    fTargetPos.SetXYZ(0., 0., 0.);
    return kERROR;
  }
  TObjArray* passNodes = fPassGeo->GetGeoPassiveNodes();
  if ( ! passNodes ) {
    cout << "-W- " << GetName() << "::GetGeometry: No passive node array" 
         << endl;
    fTargetPos.SetXYZ(0., 0., 0.);
    return kERROR;
  }
  CbmGeoNode* target = (CbmGeoNode*) passNodes->FindObject("targ");
  if ( ! target ) {
    cout << "-E- " << GetName() << "::GetGeometry: No target node" 
         << endl;
    fTargetPos.SetXYZ(0., 0., 0.);
    return kERROR;
  }
  CbmGeoVector targetPos = target->getLabTransform()->getTranslation();
  CbmGeoVector centerPos = target->getCenterPosition().getTranslation();
  Double_t targetX = targetPos.X() + centerPos.X();
  Double_t targetY = targetPos.Y() + centerPos.Y();
  Double_t targetZ = targetPos.Z() + centerPos.Z();
  fTargetPos.SetXYZ(targetX, targetY, targetZ);
  
  return kSUCCESS;

}
// -------------------------------------------------------------------------



// -----   Private method CreateHistos   -----------------------------------
void CbmTrdFindTracksQa::CreateHistos() {

  // Histogram list
  fHistoList = new TList();

  // Momentum distributions
  Double_t minMom   =  0.;
  Double_t maxMom   = 10.;
  Int_t    nBinsMom = 40;
  fhMomAccAll  = new TH1F("hMomAccAll", "all accepted tracks",
                           nBinsMom, minMom, maxMom);
  fhMomRecAll  = new TH1F("hMomRecAll", "all reconstructed tracks",
                           nBinsMom, minMom, maxMom);
  fhMomEffAll  = new TH1F("hMomEffAll", "efficiency all tracks",
                           nBinsMom, minMom, maxMom);
  fhMomAccPrim = new TH1F("hMomAccPrim", "accepted vertex tracks",
                           nBinsMom, minMom, maxMom);
  fhMomRecPrim = new TH1F("hMomRecPrim", "reconstructed vertex tracks",
                           nBinsMom, minMom, maxMom);
  fhMomEffPrim = new TH1F("hMomEffPrim", "efficiency vertex tracks",
                           nBinsMom, minMom, maxMom);
  fhMomAccSec  = new TH1F("hMomAccSec", "accepted non-vertex tracks",
                           nBinsMom, minMom, maxMom);
  fhMomRecSec  = new TH1F("hMomRecSec", "reconstructed non-vertex tracks",
                           nBinsMom, minMom, maxMom);
  fhMomEffSec  = new TH1F("hMomEffSec", "efficiency non-vertex tracks",
                           nBinsMom, minMom, maxMom);
  fHistoList->Add(fhMomAccAll);
  fHistoList->Add(fhMomRecAll);
  fHistoList->Add(fhMomEffAll);
  fHistoList->Add(fhMomAccPrim);
  fHistoList->Add(fhMomRecPrim);
  fHistoList->Add(fhMomEffPrim);
  fHistoList->Add(fhMomAccSec);
  fHistoList->Add(fhMomRecSec);
  fHistoList->Add(fhMomEffSec);

  // Number-of-points distributions
  Double_t minNp   = -0.5;
  Double_t maxNp   = 15.5;
  Int_t    nBinsNp = 16;
  fhNpAccAll  = new TH1F("hNpAccAll", "all accepted tracks",
                           nBinsNp, minNp, maxNp);
  fhNpRecAll  = new TH1F("hNpRecAll", "all reconstructed tracks",
                           nBinsNp, minNp, maxNp);
  fhNpEffAll  = new TH1F("hNpEffAll", "efficiency all tracks",
                           nBinsNp, minNp, maxNp);
  fhNpAccPrim = new TH1F("hNpAccPrim", "accepted vertex tracks",
                           nBinsNp, minNp, maxNp);
  fhNpRecPrim = new TH1F("hNpRecPrim", "reconstructed vertex tracks",
                           nBinsNp, minNp, maxNp);
  fhNpEffPrim = new TH1F("hNpEffPrim", "efficiency vertex tracks",
                           nBinsNp, minNp, maxNp);
  fhNpAccSec  = new TH1F("hNpAccSec", "accepted non-vertex tracks",
                           nBinsNp, minNp, maxNp);
  fhNpRecSec  = new TH1F("hNpRecSec", "reconstructed non-vertex tracks",
                           nBinsNp, minNp, maxNp);
  fhNpEffSec  = new TH1F("hNpEffSec", "efficiency non-vertex tracks",
                           nBinsNp, minNp, maxNp);
  fHistoList->Add(fhNpAccAll);
  fHistoList->Add(fhNpRecAll);
  fHistoList->Add(fhNpEffAll);
  fHistoList->Add(fhNpAccPrim);
  fHistoList->Add(fhNpRecPrim);
  fHistoList->Add(fhNpEffPrim);
  fHistoList->Add(fhNpAccSec);
  fHistoList->Add(fhNpRecSec);
  fHistoList->Add(fhNpEffSec);

  // z(vertex) distributions
  Double_t minZ    =  0.;
  Double_t maxZ    = 50.;
  Int_t    nBinsZ  = 50;
  fhZAccSec = new TH1F("hZAccSec", "accepted non-vertex tracks",
                        nBinsZ, minZ, maxZ);
  fhZRecSec = new TH1F("hZRecSecl", "reconstructed non-vertex tracks",
                        nBinsZ, minZ, maxZ);
  fhZEffSec = new TH1F("hZEffRec", "efficiency non-vertex tracks",
                        nBinsZ, minZ, maxZ);
  fHistoList->Add(fhZAccSec);
  fHistoList->Add(fhZRecSec);
  fHistoList->Add(fhZEffSec);

  // Number-of-hit distributions
  fhNhClones  = new TH1F("hNhClones", "number of hits for clones",
                           nBinsNp, minNp, maxNp);
  fhNhGhosts  = new TH1F("hNhGhosts", "number of hits for ghosts",
                           nBinsNp, minNp, maxNp);
  fHistoList->Add(fhNhClones);
  fHistoList->Add(fhNhGhosts);

}
// -------------------------------------------------------------------------



// -----   Private method Reset   ------------------------------------------
void CbmTrdFindTracksQa::Reset() {

  TIter next(fHistoList);
  while ( TH1* histo = ((TH1*)next()) ) histo->Reset();

  fNAccAll = fNAccPrim = fNAccRef = fNAccSec = 0;
  fNRecAll = fNRecPrim = fNRecRef = fNRecSec = 0;
  fNGhosts = fNClones = fNEvents = 0;

}
// -------------------------------------------------------------------------




// -----   Private method DivideHistos   -----------------------------------
void CbmTrdFindTracksQa::DivideHistos(TH1* histo1, TH1* histo2,
                                        TH1* histo3) {
  
  if ( !histo1 || !histo2 || !histo3 ) {
    cout << "-E- " << GetName() << "::DivideHistos: "
         << "NULL histogram pointer" << endl;
    Fatal("DivideHistos", "Null histo pointer");
  }

  Int_t nBins = histo1->GetNbinsX();
  if ( histo2->GetNbinsX() != nBins || histo3->GetNbinsX() != nBins ) {
    cout << "-E- " << GetName() << "::DivideHistos: "
         << "Different bin numbers in histos" << endl;
    cout << histo1->GetName() << " " << histo1->GetNbinsX() << endl;
    cout << histo2->GetName() << " " << histo2->GetNbinsX() << endl;
    cout << histo3->GetName() << " " << histo3->GetNbinsX() << endl;
   return;
  }

  Double_t c1, c2, c3, ce;
  for (Int_t iBin=0; iBin<nBins; iBin++) {
    c1 = histo1->GetBinContent(iBin);
    c2 = histo2->GetBinContent(iBin);
    if ( c2 ) {
      c3 = c1 / c2;
      ce = TMath::Sqrt( c3 * ( 1. - c3 ) / c2 );
    }
    else {
      c3 = 0.;
      ce = 0.;
    }
    histo3->SetBinContent(iBin, c3);
    histo3->SetBinError(iBin, ce);
  }

}
// -------------------------------------------------------------------------



ClassImp(CbmTrdFindTracksQa)
