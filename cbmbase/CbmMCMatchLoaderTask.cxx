// -------------------------------------------------------------------------
// -----                CbmMCMatchLoaderTask source file             -----
// -----                  Created 18/07/08  by T.Stockmanns        -----
// -------------------------------------------------------------------------
// libc includes
#include <iostream>

// Root includes
#include "TROOT.h"
#include "TClonesArray.h"
#include "TVector3.h"


// framework includes
#include "FairRootManager.h"
#include "CbmMCMatchLoaderTask.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairHit.h"
#include "FairLinkedData.h"
#include "CbmMCTrack.h"
// CbmMvd includes
#include "CbmStsPoint.h"
#include "CbmStsDigi.h"
#include "CbmStsCluster.h"
#include "CbmStsHit.h"


#include "CbmDetectorList.h"

#include <vector>
#include <map>


// -----   Default constructor   -------------------------------------------
CbmMCMatchLoaderTask::CbmMCMatchLoaderTask() : FairTask("Creates CbmMCMatch"), fEventNr(0)
{
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
CbmMCMatchLoaderTask::~CbmMCMatchLoaderTask()
{
}

// -----   Public method Init   --------------------------------------------
InitStatus CbmMCMatchLoaderTask::Init()
{
	fMCMatch = new CbmMCMatch("CbmMCMatch", "CbmMCMatch");

//  fMCMatch->InitStage(kMCTrack, "", "MCTrack");
  fMCMatch->InitStage(kStsPoint, "", "StsPoint");
  fMCMatch->InitStage(kStsDigi, "", "StsDigi");
  fMCMatch->InitStage(kStsCluster, "", "StsCluster");
  fMCMatch->InitStage(kStsHit, "", "StsHit");


  FairRootManager* ioman = FairRootManager::Instance();
  	if (!ioman) {
  		std::cout << "-E- CbmMCMatchLoaderTask::Init: "
  				<< "RootManager not instantiated!" << std::endl;
  		return kFATAL;
  	}

	fMCLink = (TClonesArray*)ioman->GetObject("MCLink");
	ioman->Register("MCMatch", "MCMatch", fMCMatch, kFALSE);

  return kSUCCESS;
}


// -------------------------------------------------------------------------
void CbmMCMatchLoaderTask::SetParContainers()
{
  // Get Base Container
//  FairRun* ana = FairRun::Instance();
//  FairRuntimeDb* rtdb=ana->GetRuntimeDb();

}


// -----   Public method Exec   --------------------------------------------
void CbmMCMatchLoaderTask::Exec(Option_t* opt)
{
/*	for (int trackIndex = 0; trackIndex < fTrack->GetEntriesFast(); trackIndex++){
		CbmTrack* myTrack = (CbmTrack*)fTrack->At(trackIndex);
		fMCMatch->AddElement(kTrack, trackIndex, kTrackCand, myTrack->GetRefIndex());
	}
*/

	if (!fMCLink) Fatal("Exec", "No fMCLink");
//	fMCLinkDet->Delete();
//	fMCLinkHit->Delete();
	fMCMatch->ClearMCList();

	fMCMatch->LoadInMCLists(fMCLink);
	fMCMatch->CreateArtificialStage(kMCTrack, "", "");

	fMCMatch->Print();
	std::cout << std::endl;
}

void CbmMCMatchLoaderTask::Finish()
{
}


ClassImp(CbmMCMatchLoaderTask);
