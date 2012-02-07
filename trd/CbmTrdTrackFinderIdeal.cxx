// -------------------------------------------------------------------------
// -----                CbmTrdTrackFinderIdeal source file             -----
// -----                  Created 28/11/05  by V. Friese               -----
// -----              according to the CbmStsTrackFinderIdeal          -----
// -------------------------------------------------------------------------
// CBM includes
#include "CbmTrdTrackFinderIdeal.h"

#include "CbmTrdHit.h"
#include "CbmTrdTrack.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"
#include "FairDetector.h"
#include "FairMCPoint.h"
#include "CbmMCTrack.h"

// ROOT includes
#include "TClonesArray.h"
#include "TObjArray.h"

// C++ includes
#include <iostream>
#include <map>
using std::cout;
using std::endl;
using std::map;


// -----   Default constructor   -------------------------------------------
CbmTrdTrackFinderIdeal::CbmTrdTrackFinderIdeal()
  : CbmTrdTrackFinder(),
    fMCTrackArray(NULL),
    fMCPointArray(NULL),
    fNoTrdStations(-1),
    fNoTrdPerStation(-1),
    fVerbose(-1),
    fEventNum(-1)
{
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmTrdTrackFinderIdeal::~CbmTrdTrackFinderIdeal()
{
}
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
void CbmTrdTrackFinderIdeal::Init()
{
  // Get and check FairRootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if (! ioman) {
    cout << "-E- CbmTrdTrackFinderIdeal::Init: "
	 << "RootManager not instantised!" << endl;
    return;
  }

  // Get MCTrack array
  fMCTrackArray  = (TClonesArray*) ioman->GetObject("MCTrack");
  if ( ! fMCTrackArray) {
    cout << "-E- CbmTrdTrackFinderIdeal::Init: No MCTrack array!"
	 << endl;
    return;
  }

  // Get MCPoint array
  fMCPointArray  = (TClonesArray*) ioman->GetObject("TrdPoint");
  if ( ! fMCPointArray) {
    cout << "-E- CbmTrdTrackFinderIdeal::Init: No MCPoint array!"
	 << endl;
    return;
  }


  // Get the pointer to the singleton FairRunAna object
  FairRunAna* ana = FairRunAna::Instance();
  if(NULL == ana) {
      cout << "-E- CbmTrdTrackFinderIdeal::Init :"
          <<" no FairRunAna object!" << endl;
      return;
  }

  // Get the pointer to run-time data base
  FairRuntimeDb* rtdb = ana->GetRuntimeDb();
  if(NULL == rtdb) {
      cout << "-E- CbmTrdTrackFinderIdeal::Init :"
          <<" no runtime database!" << endl;
      return;
  }

  // Get the pointer to container of base parameters
  FairBaseParSet* baseParSet =
      (FairBaseParSet*) rtdb->getContainer("FairBaseParSet");
  if(NULL == baseParSet) {
      cout << "-E- CbmTrdTrackFinderIdeal::Init :"
          <<" no container of base parameters!" << endl;
      return;
  }

  // Get the pointer to detector list
  TObjArray *detList = baseParSet->GetDetList();
  if(NULL == detList) {
      cout << "-E- CbmTrdTrackFinderIdeal::Init :"
          << " no detector list!" << endl;
      return;
  }

  // Find TRD detector
  FairDetector* trd = (FairDetector*) detList->FindObject("TRD");
  if(NULL == trd) {
      cout << "-E- CbmTrdTrackFinderIdeal::Init :"
          << " no TRD detector!" << endl;
      return;
  }
  fEventNum = 0;

  /*
    CbmGeoTrdPar* par=(CbmGeoTrdPar*)(rtdb->getContainer("CbmGeoTrdPar"));
    TObjArray *fSensNodes = par->GetGeoSensitiveNodes();
    FairGeoNode *fm1= (FairGeoNode *)fSensNodes->FindObject("trd3gas#4");
    FairGeoNode *fm2= (FairGeoNode *)fSensNodes->FindObject("trd3gas#3");
    FairGeoNode *fm3= (FairGeoNode *)fSensNodes->FindObject("trd6gas#2");
    FairGeoNode *fm4= (FairGeoNode *)fSensNodes->FindObject("trd1gas#4");
    FairGeoNode *fm5= (FairGeoNode *)fSensNodes->FindObject("trd1mod1gas");

    if (fm1) {
      cout << " -I- CbmTrdTrackFinderIdeal: TRD version with 12 layers (3 stations x 4 layers each) " << endl;
      fNoTrdStations = 3;
      fNoTrdPerStation = 4;
    }
    else if (fm2 && fm4) {
      cout << " -I- CbmTrdTrackFinderIdeal: special TRD version with 10 layers (station 1 with 4 layers, station 2+3 with 3 layers each ) " << endl;
      cout << " -I Dont know how to handle this correctly" << endl;
      fNoTrdStations = 1;
      fNoTrdPerStation = 10;
    }
    else if (fm2) {
      cout << " -I- CbmTrdTrackFinderIdeal: TRD version with 9 layers (3 stations x 3 layers each) " << endl;
      fNoTrdStations = 3;
      fNoTrdPerStation = 3;
    }
    else if (fm3) {
      cout << " -I- CbmTrdTrackFinderIdeal: TRD version with 12 layers (6 stations x 2 layers each)" << endl;
      fNoTrdStations = 6;
      fNoTrdPerStation = 2;
    }
    else if (fm5) {
      cout << " -I- CbmTrdTrackFinderIdeal: TRD version with 12 layers (3 stations x 4 layers each) and new segmented geometry" << endl;
      fNoTrdStations = 3;
      fNoTrdPerStation = 4;
    }
    else {
      cout << "***************************************" <<endl;
      cout << "                                       " <<endl;
      cout << " - FATAL ERROR Unknown geometry version" <<endl;
      cout << "   in CbmTrdHitProducer                " <<endl;
      cout << "                                       " <<endl;
      cout << "***************************************" <<endl;
      Fatal("","");
    }

  */

}
// -------------------------------------------------------------------------



// -----   Public method DoFind   ------------------------------------------
Int_t CbmTrdTrackFinderIdeal::DoFind(TClonesArray* hitArray,
                                     TClonesArray* trackArray)
{

  // Check pointers
  if ( !fMCTrackArray ) {
    cout << "-E- CbmTrdTrackFinderIdeal::DoFind: "
	 << "MCTrack array missing! " << endl;
    return -1;
  }

  if ( !fMCPointArray ) {
    cout << "-E- CbmTrdTrackFinderIdeal::DoFind: "
	 << "MCPoint array missing! " << endl;
    return -1;
  }

  if ( !hitArray ) {
    cout << "-E- CbmTrdTrackFinderIdeal::DoFind: "
	 << "Hit arrays missing! "
	 << hitArray << endl;
    return -1;
  }

  if ( !trackArray ) {
    cout << "-E- CbmTrdTrackFinderIdeal::DoFind: "
	 << "Track array missing! " << endl;
    return -1;
  }

  // Initialise control counters
  Int_t nNoMCTrack   = 0;
  Int_t nNoTrack     = 0;
  Int_t nNoTrdPoint  = 0;
  Int_t nNoTrdHit    = 0;

  // Create pointers to TRD hit and TrdPoint
  CbmTrdHit*       pHit = NULL;
  FairMCPoint*      pMCpt = NULL;
  CbmMCTrack*      pMCtr = NULL;
  CbmTrdTrack*     pTrck = NULL;

  // Number of TRD hits
  Int_t nTrdHit = hitArray->GetEntriesFast();

  // Declare variables outside the loop
  Int_t ptIndex = 0;       // MC point index
  Int_t mcTrackIndex = 0;  // MC track index
  Int_t trackIndex = 0;    // TRD track index

  // Create STL map from MCTrack index to number of hits
  map<Int_t, Int_t> hitMap;

  // Loop over TRD hits
  for(Int_t iHit = 0; iHit < nTrdHit; iHit++) {
      // Get the pointer to TRD hit
      pHit = (CbmTrdHit*) hitArray->At(iHit);
      if(NULL == pHit) continue;
      // if(pHit->GetFlag()) continue; // lost hit
      // Get point index
      ptIndex = pHit->GetRefId();
      if(ptIndex < 0) continue; // fake or background hit
      // Get pointer to MC point
      pMCpt = (FairMCPoint*) fMCPointArray->At(ptIndex);
      if(NULL == pMCpt) continue;
      // Get MC track index
      mcTrackIndex = pMCpt->GetTrackID();
      // Increment the number of hits
      hitMap[mcTrackIndex] += 1;
  }

  // Create STL map from MCTrack index to TrdTrack index
  map<Int_t, Int_t> trackMap;

  // Loop over reconstructable MCTracks and create corresponding TrdTrack
  Int_t nMCacc = 0;  // Accepted MC tracks
  Int_t nTracks = 0; // Reconstructable MC tracks
  Int_t nMCTracks = fMCTrackArray->GetEntriesFast();
  for (Int_t iMCTrack=0; iMCTrack<nMCTracks; iMCTrack++) {
    pMCtr = (CbmMCTrack*) fMCTrackArray->At(iMCTrack);
    if ( ! pMCtr ) continue;

    /*
    if ( pMCtr->GetTrdPoints() < (fNoTrdStations*fNoTrdPerStation) ) {
        continue;
    }
    */

    // Tracks in the TRD are straight lines, so two POINTS should be enough
    // to construct a track.
    if ( pMCtr->GetNPoints(kTRD) < 2) continue;

    nMCacc += 1;

    // Tracks in the TRD are straight lines, so two HITS should be enough
    // to construct a track.
//    if( hitMap[iMCTrack] < (fNoTrdStations*fNoTrdPerStation) ) {
    if( hitMap[iMCTrack] < 2 ) {
        continue;
    }
    new((*trackArray)[nTracks]) CbmTrdTrack();

    pTrck = (CbmTrdTrack*) trackArray->At(nTracks);
    Double_t mom = pMCtr->GetP();
    FairTrackParam par(*pTrck->GetParamLast());
    par.SetQp(1./mom);
    pTrck->SetParamLast(&par);

    if(fVerbose > 1) {
        cout << "-I- CbmTrdTrackFinderIdeal : "
            << "TrdTrack " << nTracks << " created "
            << "from MCTrack " << iMCTrack << " ("
            << pMCtr->GetNPoints(kTRD) << ")" << endl;
    }
    trackMap[iMCTrack] = nTracks;
    nTracks += 1;
  }

  if(fVerbose > 2) {
      cout << "-I- CbmTrdTrackFinderIdeal : " << endl;
  }

  //  Double_t eloss;

  // Loop over hits. Get corresponding MCPoint and MCTrack index
  for (Int_t iHit = 0; iHit<nTrdHit; iHit++) {
    pHit = (CbmTrdHit*) hitArray->At(iHit);
    if ( ! pHit ) {
      cout << "-E- CbmTrdTrackFinderIdeal::DoFind: Empty slot "
	   << "in HitArray at position " << iHit << endl;
      nNoTrdHit++;
      continue;
    }
//    if ( pHit->GetFlag() ) continue;    // lost hit
    ptIndex = pHit->GetRefId();
    if (ptIndex < 0) continue;           // fake or background hit
    pMCpt = (FairMCPoint*) (fMCPointArray->At(ptIndex));
    if ( ! pMCpt ) {
      nNoTrdPoint++;
      continue;
    }
    mcTrackIndex = pMCpt->GetTrackID();
    if (mcTrackIndex<0 || mcTrackIndex>nMCTracks) {
      cout << "-E- CbmTrdTrackFinderIdeal::DoFind: "
	   << "MCTrack index out of range. " << mcTrackIndex << " "
	   << nMCTracks << endl;
      nNoMCTrack++;
      continue;
    }
    if (trackMap.find(mcTrackIndex) == trackMap.end()) continue;
    trackIndex = trackMap[mcTrackIndex];
    pTrck = (CbmTrdTrack*) trackArray->At(trackIndex);
    if ( ! pTrck ) {
      cout << "-E- CbmTrdTrackFinderIdeal::DoFind: "
	   << "No TrdTrack pointer. " << iHit << " " << ptIndex
	   << " " << mcTrackIndex << " " << trackIndex << endl;
      nNoTrack++;
      continue;
    }
    pTrck->AddHit(iHit, kTRDHIT);

    //    eloss = pHit->GetELoss();
    //pTrck->SetELoss(pTrck->GetELoss()+eloss);
    if(fVerbose > 2) {
        cout << "TRD hit " << iHit << " from TRD point "
            << ptIndex << " (" << mcTrackIndex << ") "
            << "added to TRD track " << trackIndex << endl;
    }
  }
  fEventNum++;
  if(fVerbose) {
      cout << endl;
      cout << "------------------------------------------------------" << endl;
      cout << "         CbmTrdTrackFinderIdeal: Event summary # " << fEventNum <<  endl;
      cout << "TRD hits: " << nTrdHit << endl;
      cout << "MC tracks total: " << nMCTracks << ", accepted: "
          << nMCacc << ", reconstructable: " << nTracks << endl;
      if (nNoTrdHit)   cout << "TrdHits not found   : " << nNoTrdHit   << endl;
      if (nNoTrdPoint) cout << "TrdPoints not found : " << nNoTrdPoint << endl;
      if (nNoMCTrack)  cout << "MCTracks not found  : " << nNoMCTrack  << endl;
      if (nNoTrack)    cout << "TrdTracks not found : " << nNoTrack    << endl;
      cout << "------------------------------------------------------" << endl;
      cout << endl;
  } else {
      cout << "CbmTrdTrackFinderIdeal: all: " << nMCTracks
          << ", acc.: " << nMCacc << ", rec.:" << nTracks << endl;
  }

  return nTracks;

}
// -------------------------------------------------------------------------


ClassImp(CbmTrdTrackFinderIdeal)




