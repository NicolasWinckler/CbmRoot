
#include "CbmTrdElectronsQa.h"

#include "CbmTrdHit.h"
#include "CbmTrdTrack.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"
#include "FairDetector.h"
#include "FairMCPoint.h"
#include "CbmMCTrack.h"

#include "TClonesArray.h"
#include "TObjArray.h"

#include <iostream>
#include <map>
using std::cout;
using std::endl;
using std::map;


CbmTrdElectronsQa::CbmTrdElectronsQa()
{
  fVerbose = 1;
}

CbmTrdElectronsQa::~CbmTrdElectronsQa()
{

}

InitStatus CbmTrdElectronsQa::Init()
{

	FairRootManager* ioman = FairRootManager::Instance();
	if (! ioman) {
		cout << "-E- CbmTrdElectronsQa::Init: RootManager not instantised!" << endl;
		return kERROR;
	}

	fMCTracks  = (TClonesArray*) ioman->ActivateBranch("MCTrack");
	if ( ! fMCTracks) {
		cout << "-E- CbmTrdElectronsQa::Init: No MCTrack array!" << endl;
		return kERROR;
	}

	fMCPoints  = (TClonesArray*) ioman->ActivateBranch("TRDPoint");
	if ( ! fMCPoints) {
		cout << "-E- CbmTrdElectronsQa::Init: No MCPoint array!" << endl;
		return kERROR;
	}

	fTrdTracks  = (TClonesArray*) ioman->ActivateBranch("TRDTrack");
	if ( ! fTrdTracks) {
		cout << "-E- CbmTrdElectronsQa::Init: No TRDTrack array!" << endl;
		return kERROR;
	}

	fTrdTrackMatches  = (TClonesArray*) ioman->ActivateBranch("TRDTrackMatch");
	if ( ! fTrdTrackMatches) {
		cout << "-E- CbmTrdElectronsQa::Init: No TRDTrackMatch array!" << endl;
		return kERROR;
	}

	fTrdHits  = (TClonesArray*) ioman->ActivateBranch("TRDHit");
	if ( ! fTrdHits) {
		cout << "-E- CbmTrdElectronsQa::Init: No TRDHit array!" << endl;
		return kERROR;
	}

	return kSUCCESS;
}

void CbmTrdElectronsQa::Exec(Option_t* option)
{
	if (!fMCTracks) return;
	if (!fMCPoints) return;
	if (!fTrdTracks) return;
	if (!fTrdTrackMatches) return;
	if (!fTrdHits) return;



}

void CbmTrdElectronsQa::Finish()
{

}


ClassImp(CbmTrdElectronsQa)




