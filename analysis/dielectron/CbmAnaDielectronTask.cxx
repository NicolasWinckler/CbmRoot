#include <iostream>
#include <map>
using namespace std;

#include "CbmAnaDielectronTask.h"

#include "FairTask.h"
#include "FairRootManager.h"
#include "CbmVertex.h"
#include "CbmGlobalTrack.h"
#include "CbmStsKFTrackFitter.h"
#include "CbmStsTrack.h"
#include "CbmRichRing.h"
#include "CbmRichRingMatch.h"
#include "CbmTrdTrack.h"
#include "CbmTrdHit.h"
#include "CbmTofHit.h"

#include "FairTrackParam.h"
#include "CbmTrackMatch.h"
#include "CbmKF.h"

#include "CbmRichPoint.h"

#include "TClonesArray.h"
#include "TObjArray.h"
#include "TGraph.h"
#include "TVector3.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TF1.h"
#include "TMath.h"
#include "TLorentzVector.h"
#include "TProfile.h"
#include "TObject.h"

#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"
#include "FairGeoVolume.h"
#include "FairGeoTransform.h"
#include "FairGeoVector.h"
#include "FairGeoMedium.h"
#include "FairGeoNode.h"
#include "TDatabasePDG.h"

#include "CbmStsHit.h"
#include "CbmMvdHit.h"

#include "TString.h"
#include "TSystem.h"

#define M2E 2.6112004954086e-7

ClassImp(CbmAnaDielectronTask);

CbmAnaDielectronTask::CbmAnaDielectronTask(const char *name, const char *title)
: FairTask(name)
{
    fEvents = 0;
    fUseRich = true;
    fUseTrd = true;
    fUseTof = true;
}

CbmAnaDielectronTask::~CbmAnaDielectronTask()
{

}

InitStatus CbmAnaDielectronTask::Init()
{
	cout << "InitStatus CbmRichElectronsQa::Init()"<<endl;

	FairRootManager* ioman = FairRootManager::Instance();
	if (!ioman) {
		cout << "-E- CbmAnaDielectronTask::Init: " << "RootManager not instantised!"<< endl;
		return kERROR;
	}

	fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
	if ( !fMCTracks) {
		cout << "-E- CbmAnaDielectronTask::Init: No MCTrack array!" << endl;
		return kERROR;
	}

	if (fUseRich == true){
		fRichHits = (TClonesArray*) ioman->GetObject("RichHit");
		if ( !fRichHits) {
			cout << "-W- CbmAnaDielectronTask::Init: No RichHit array!" << endl;
		}

		fRichRings = (TClonesArray*) ioman->GetObject("RichRing");
		if ( !fRichRings) {
			cout << "-E- CbmAnaDielectronTask::Init: No RichRing array!" << endl;
			return kERROR;
		}

		fRichPoints = (TClonesArray*) ioman->GetObject("RichPoint");
		if ( !fRichPoints) {
			cout << "-E- CbmAnaDielectronTask::Init: No RichPoint array!" << endl;
			return kERROR;
		}

		fRichRingMatches = (TClonesArray*) ioman->GetObject("RichRingMatch");
		if ( !fRichRingMatches) {
			cout << "-E- CbmAnaDielectronTask::Init: No RichRingMatch array!" << endl;
			return kERROR;
		}

		fRichProj = (TClonesArray*) ioman->GetObject("RichProjection");
		if ( !fRichProj) {
			cout << "-E- CbmAnaDielectronTask::Init: No RichProjection array!" << endl;
			return kERROR;
		}
	}//fUseRich

	fStsTrackMatches = (TClonesArray*) ioman->GetObject("StsTrackMatch");
	if ( !fStsTrackMatches) {
		cout << "-E- CbmAnaDielectronTask::Init: No track match array!"<< endl;
		return kERROR;
	}

	fStsTracks = (TClonesArray*) ioman->GetObject("StsTrack");
	if ( !fStsTracks) {
		cout << "-E- CbmAnaDielectronTask::Init: No  STSTrack array!"<< endl;
		return kERROR;
	}

    fStsHits = (TClonesArray*) ioman->GetObject("StsHit");
    if (! fStsHits) {
		cout << "-E- CbmAnaDielectronTask::Init: No STSHit array!" << endl;
		return kERROR;
    }

    fMvdHits = (TClonesArray*) ioman->GetObject("MvdHit");
    if (! fMvdHits) {
		cout << "-E- CbmAnaDielectronTask::Init: No MvdHit array!" << endl;
		return kERROR;
    }

	fGlobalTracks = (TClonesArray*) ioman->GetObject("GlobalTrack");
	if ( !fGlobalTracks) {
		cout << "-W- CbmAnaDielectronTask::Init: No global track array!" << endl;
		return kERROR;
	}

	if (fUseTrd == true){
		fTrdTracks = (TClonesArray*) ioman->GetObject("TrdTrack");
		if ( !fTrdTracks) {
			cout << "-W- CbmAnaDielectronTask::Init: No TrdTrack array!" << endl;
			return kERROR;
		}
		fTrdTrackMatches = (TClonesArray*) ioman->GetObject("TrdTrackMatch");
		if ( !fTrdTrackMatches) {
			cout << "-W- CbmAnaDielectronTask::Init: No TrdTrackMatch array!" << endl;
			return kERROR;
		}
	}//fUseTrd


	if (fUseTof == true){
		fTofPoints = (TClonesArray*) ioman->GetObject("TofPoint");
		if ( !fTofPoints) {
			cout << "-W- CbmAnaDielectronTask::Init: No TOFPoint array!" << endl;
			return kERROR;
		}

		fTofHits = (TClonesArray*) ioman->GetObject("TofHit");
		if ( !fTofHits) {
			cout << "-W- CbmAnaDielectronTask::Init: No TOFHit array!" << endl;
			return kERROR;
		}
	}//fUseTof

    fPrimVertex = (CbmVertex*) ioman->GetObject("PrimaryVertex");
    if (! fPrimVertex) {
		cout << "-E- CbmAnaDielectronTask::Init: No Primary Vertex!" << endl;
		return kERROR;
    }

	return kSUCCESS;
}


void CbmAnaDielectronTask::Exec(Option_t *option)
{

    fEvents++;
    cout << "-I- CbmAnaDielectronTask event number " << fEvents << endl;

}// Exec

void CbmAnaDielectronTask::MCPairs()
{
    Int_t nMCTracks = fMCTracks->GetEntries();


} //MC Pairs

void CbmAnaDielectronTask::SingleParticleAcceptance()
{
    Int_t nGlobalTracks = fGlobalTracks->GetEntriesFast();

	for(Int_t i = 0; i < nGlobalTracks; i++){

	}
}

void CbmAnaDielectronTask::PairAcceptance()
{
    Int_t nGlobalTracks = fGlobalTracks->GetEntriesFast();


} // PairsAcceptance

void CbmAnaDielectronTask::Finish()
{

}

void CbmAnaDielectronTask::WriteOutput()
{
    // --- Normalize histogramms ---
    Double_t scale = 1./(Double_t)fEvents;

}
