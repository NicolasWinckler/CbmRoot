// -------------------------------------------------------------------------
// -----                   CbmMuchFindTracks source file                -----
// -----                  Created 01/10/07  by A. Lebedev               -----
// -----                                                               -----
// -------------------------------------------------------------------------

#include "CbmMuchFindTracks.h"

#include "CbmMuchTrackFinder.h"
#include "CbmMuchHit.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"

#include <iostream>
using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
CbmMuchFindTracks::CbmMuchFindTracks()
{
  fFinder         = NULL;
  fMuchHitArray    = NULL;
  fTrackArray     = NULL;
  fNofTracks      = 0;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmMuchFindTracks::CbmMuchFindTracks(const char* name, 
				   const char* title, 
				   CbmMuchTrackFinder* finder) 
: FairTask(name)
{
  fFinder         = finder;
  fMuchHitArray    = NULL;
  fTrackArray     = NULL;
  fNofTracks      = 0;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmMuchFindTracks::~CbmMuchFindTracks()
{
  fTrackArray->Delete();
}
// -------------------------------------------------------------------------



// -----   Public method Init (abstract in base class)  --------------------
InitStatus CbmMuchFindTracks::Init()
{

  // Check for Track finder
  if (! fFinder) {
    cout << "-W- CbmMuchFindTracks::Init: No track finder selected!" << endl;
    return kERROR;
  }

  // Get and check FairRootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if (! ioman) {
    cout << "-E- CbmMuchFindTracks::Init: "
	 << "RootManager not instantised!" << endl;
    return kFATAL;
  }

  // Get TRD hit Array
  fMuchHitArray
    = (TClonesArray*) ioman->GetObject("MuchHit");
  if ( ! fMuchHitArray) {
    cout << "-W- CbmMuchFindTracks::Init: No MuchHit array!"
	 << endl;
    return kERROR;
  }

  // Create and register MuchTrack array
  fTrackArray = new TClonesArray("CbmMuchTrack",100);
  ioman->Register("MuchTrack", "Much", fTrackArray, kTRUE);


  // Call the Init method of the track finder
  fFinder->Init();

  return kSUCCESS;

}
// -------------------------------------------------------------------------



// -----  SetParContainers -------------------------------------------------
void CbmMuchFindTracks::SetParContainers()
{
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb = ana->GetRuntimeDb();

    rtdb->getContainer("FairBaseParSet");
    rtdb->getContainer("CbmGeoPassivePar");
    rtdb->getContainer("CbmGeoStsPar");
    rtdb->getContainer("CbmGeoRichPar");
    rtdb->getContainer("CbmGeoMuchPar");
    rtdb->getContainer("CbmGeoTrdPar");
    rtdb->getContainer("CbmFieldPar");
}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void CbmMuchFindTracks::Exec(Option_t* opt)
{
  fTrackArray->Delete();
  
  fNofTracks = fFinder->DoFind(fMuchHitArray, fTrackArray);
  
}
// -------------------------------------------------------------------------



// -----   Public method Finish   ------------------------------------------
void CbmMuchFindTracks::Finish()
{
  fTrackArray->Clear();
}
// -------------------------------------------------------------------------




ClassImp(CbmMuchFindTracks)
