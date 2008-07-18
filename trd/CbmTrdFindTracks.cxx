// -------------------------------------------------------------------------
// -----                   CbmTrdFindTracks source file                -----
// -----                  Created 25/11/05  by D. Kresan               -----
// -----                 according to the CbmStsFindTracks             -----
// -------------------------------------------------------------------------

#include "CbmTrdFindTracks.h"

#include "CbmTrdTrack.h"
#include "CbmTrdTrackFinder.h"
#include "CbmTrdHit.h"

#include "CbmRootManager.h"
#include "CbmRunAna.h"
#include "CbmRuntimeDb.h"

#include "TClonesArray.h"

#include <iostream>
using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
CbmTrdFindTracks::CbmTrdFindTracks()
{
  fFinder         = NULL;
  fTrdHitArray    = NULL;
  fTrackArray     = NULL;
  fNofTracks      = 0;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmTrdFindTracks::CbmTrdFindTracks(const char* name, 
				   const char* title, 
				   CbmTrdTrackFinder* finder) 
: CbmTask(name)
{
  fFinder         = finder;
  fTrdHitArray    = NULL;
  fTrackArray     = NULL;
  fNofTracks      = 0;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmTrdFindTracks::~CbmTrdFindTracks()
{
  fTrackArray->Delete();
}
// -------------------------------------------------------------------------



// -----   Public method Init (abstract in base class)  --------------------
InitStatus CbmTrdFindTracks::Init()
{

  // Check for Track finder
  if (! fFinder) {
    cout << "-W- CbmTrdFindTracks::Init: No track finder selected!" << endl;
    return kERROR;
  }

  // Get and check CbmRootManager
  CbmRootManager* ioman = CbmRootManager::Instance();
  if (! ioman) {
    cout << "-E- CbmTrdFindTracks::Init: "
	 << "RootManager not instantised!" << endl;
    return kFATAL;
  }

  // Get TRD hit Array
  fTrdHitArray
    = (TClonesArray*) ioman->GetObject("TRDHit");
  if ( ! fTrdHitArray) {
    cout << "-W- CbmTrdFindTracks::Init: No TrdHit array!"
	 << endl;
    return kERROR;
  }

  // Create and register TrdTrack array
  fTrackArray = new TClonesArray("CbmTrdTrack",100);
  ioman->Register("TRDTrack", "TRD", fTrackArray, kTRUE);


  // Call the Init method of the track finder
  fFinder->Init();

  return kSUCCESS;

}
// -------------------------------------------------------------------------



// -----  SetParContainers -------------------------------------------------
void CbmTrdFindTracks::SetParContainers()
{
    CbmRunAna* ana = CbmRunAna::Instance();
    CbmRuntimeDb* rtdb = ana->GetRuntimeDb();
  //  rtdb->getContainer("CbmGeoPassivePar");
 //   rtdb->getContainer("CbmGeoStsPar");
 //   rtdb->getContainer("CbmGeoTrdPar");
    rtdb->getContainer("CbmBaseParSet");
    rtdb->getContainer("CbmGeoPassivePar");
    rtdb->getContainer("CbmGeoStsPar");
    rtdb->getContainer("CbmGeoRichPar");
    rtdb->getContainer("CbmGeoTrdPar");
    rtdb->getContainer("CbmFieldPar");
}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void CbmTrdFindTracks::Exec(Option_t* opt)
{
  fTrackArray->Delete();
  fNofTracks = fFinder->DoFind(fTrdHitArray, fTrackArray);
  for (Int_t iTrack=0; iTrack<fTrackArray->GetEntriesFast(); iTrack++) {
    CbmTrdTrack* track = (CbmTrdTrack*) fTrackArray->At(iTrack);
    track->SortHits();
    Double_t eloss = 0;
    for (Int_t iTRD=0; iTRD < track->GetNofTrdHits(); iTRD++){
      Int_t TRDindex = track->GetTrdHitIndex(iTRD);
      CbmTrdHit* trdHit = (CbmTrdHit*) fTrdHitArray->At(TRDindex);
      eloss += trdHit->GetELoss();
    } 
    track->SetELoss(eloss);
  }
}
// -------------------------------------------------------------------------



// -----   Public method Finish   ------------------------------------------
void CbmTrdFindTracks::Finish()
{
  fTrackArray->Clear();
}
// -------------------------------------------------------------------------




ClassImp(CbmTrdFindTracks)
