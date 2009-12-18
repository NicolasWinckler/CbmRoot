// ------------------------------------------------------------------
// -----                  CbmGlobalTrackFitterKF                -----
// -----              Created 07/03/2006 by D.Kresan            -----
// ------------------------------------------------------------------
#include "CbmGlobalTrackFitterKF.h"

#include "CbmKFStsHit.h"
#include "CbmKFTrdHit.h"
#include "CbmKFTrack.h"

#include "FairRootManager.h"
#include "CbmStsHit.h"
#include "CbmTrdHit.h"
#include "CbmTofHit.h"
#include "CbmStsTrack.h"
#include "CbmTrdTrack.h"
#include "CbmGlobalTrack.h"

#include "TClonesArray.h"
#include "TMath.h"

#include <iostream>

using std::cout;
using std::endl;
using std::vector;

//___________________________________________________________________
//
// CbmGlobalTrackFitterKF
//
// Concrete implementation of global track fitting based on the
// Kalman filter
//


// ------------------------------------------------------------------
CbmGlobalTrackFitterKF::CbmGlobalTrackFitterKF()
{
    // Default constructor
    fArrayStsHit = fArrayTrdHit = fArrayTofHit
	= fArrayStsTrack = fArrayTrdTrack = NULL;
    fKfTrack = new CbmKFTrack();
    // Set mass hypothesis
    fKfTrack->SetPID(211);
    fVerbose = 0;
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
CbmGlobalTrackFitterKF::~CbmGlobalTrackFitterKF()
{
    // Destructor
    delete fKfTrack;
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
void CbmGlobalTrackFitterKF::Init()
{
    // Initialisation

    // Get pointer to the ROOT I/O manager
    FairRootManager* rootMgr = FairRootManager::Instance();
    if(NULL == rootMgr) {
	cout << "-E- CbmGlobalTrackFitterKF::Init :"
	    << " ROOT manager is not instantiated" << endl;
        return;
    }
    // Get hit arrays
    fArrayStsHit = (TClonesArray*) rootMgr->GetObject("StsHit");
    if(NULL == fArrayStsHit) {
	cout << "-W- CbmGlobalTrackFitterKF::Init :"
            << " no Sts hit array" << endl;
    }
    fArrayTrdHit = (TClonesArray*) rootMgr->GetObject("TrdHit");
    if(NULL == fArrayTrdHit) {
	cout << "-W- CbmGlobalTrackFitterKF::Init :"
            << " no TRD hit array" << endl;
    }
    fArrayTofHit = (TClonesArray*) rootMgr->GetObject("TofHit");
    if(NULL == fArrayTofHit) {
	cout << "-W- CbmGlobalTrackFitterKF::Init :"
            << " no TOF hit array" << endl;
    }
    // Get track arrays
    fArrayStsTrack = (TClonesArray*) rootMgr->GetObject("StsTrack");
    if(NULL == fArrayStsTrack) {
	cout << "-W- CbmGlobalTrackFitterKF::Init : "
            << "no STS track array!" << endl;
    }
    fArrayTrdTrack = (TClonesArray*) rootMgr->GetObject("TrdTrack");
    if(NULL == fArrayTrdTrack) {
	cout << "-W- CbmGlobalTrackFitterKF::Init : "
            << "no TRD track array!" << endl;
    }
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
void CbmGlobalTrackFitterKF::DoFit(CbmGlobalTrack* glbTrack)
{
    // Implementation of the fitting algorithm
    if(NULL==glbTrack || NULL==fArrayStsTrack || NULL==fArrayTrdTrack ||
       NULL==fArrayStsHit ||  NULL==fArrayTrdHit) return;

    // Get STS track index
    Int_t stsTrackIndex = glbTrack->GetStsTrackIndex();
    if(stsTrackIndex >= 0) {
        // Get STS track
	CbmStsTrack* stsTrack = (CbmStsTrack*) fArrayStsTrack->
	    At(stsTrackIndex);
	if(NULL != stsTrack) {
	    Int_t stsHitIndex;
	    CbmStsHit* stsHit;
	    CbmKFStsHit* kfStsHit;
	    // Loop over hits of the STS track
	    for(Int_t iHit = 0; iHit < stsTrack->GetNStsHits(); iHit++) {
		// Get hit index
		stsHitIndex = stsTrack->GetStsHitIndex(iHit);
		// Get hit
		stsHit = (CbmStsHit*) fArrayStsHit->At(stsHitIndex);
		// Create KF hit
		kfStsHit = new CbmKFStsHit();
		kfStsHit->Create(stsHit);
		// Add hit to the kf track
                fKfTrack->fHits.push_back(kfStsHit);
	    }
	}
    }

    // Get TRD track index
    Int_t trdTrackIndex = glbTrack->GetTrdTrackIndex();
    if(trdTrackIndex >= 0) {
        // Get TRD track
	CbmTrdTrack* trdTrack = (CbmTrdTrack*) fArrayTrdTrack->
	    At(trdTrackIndex);
	if(NULL != trdTrack) {
	    Int_t trdHitIndex;
	    CbmTrdHit* trdHit;
	    CbmKFTrdHit* kfTrdHit;
	    // Loop over hits of the TRD track
	    for(Int_t iTrd = 0; iTrd < trdTrack->GetNofHits(); iTrd++) {
		// Get hit index
		trdHitIndex = trdTrack->GetHitIndex(iTrd);
		// Get hit
		trdHit = (CbmTrdHit*) fArrayTrdHit->At(trdHitIndex);
		// Create KF hit
		kfTrdHit = new CbmKFTrdHit();
		kfTrdHit->Create(trdHit);
		// Add hit to the kf track
                fKfTrack->fHits.push_back(kfTrdHit);
	    }
	}
    }

    if(fKfTrack->GetNOfHits() > 2) {
	fKfTrack->GetRefChi2() = 0.;
	fKfTrack->GetRefNDF() = 0;
        // No guess taken
	for(Int_t i = 0; i < 6; i++) {
	    fKfTrack->GetTrack()[i] = 0;
	}
        fKfTrack->Fit(1);
	fKfTrack->Fit(0);
        fKfTrack->Fit(1);
	// Store parameters at the last plane
    FairTrackParam parLast;
	fKfTrack->GetTrackParam(parLast);
	glbTrack->SetParamLast(&parLast);
	fKfTrack->Fit(1);
	// Store parameters at the first plane
	    FairTrackParam parFirst;
        fKfTrack->GetTrackParam(parFirst);
        glbTrack->SetParamFirst(&parFirst);
	glbTrack->SetChi2(fKfTrack->GetRefChi2());
	glbTrack->SetNDF(fKfTrack->GetRefNDF());
	if(fVerbose > 1) {
	    cout << "-I- CbmGlobalTrackFitterKF::DoFit : "
                << "global track fitted. chi2=" << glbTrack->GetChi2() << endl;
	}
	if(fVerbose > 2) {
            glbTrack->Print();
	}


    	// Calculate track length
	Int_t tofIndex = glbTrack->GetTofHitIndex();
	if(tofIndex > -1) {
	    CbmTofHit *tofHit = (CbmTofHit*)fArrayTofHit->At(tofIndex);
	    Double_t length = 0.;
	    Double_t z = 0.;
	    fKfTrack->Extrapolate(z);
	    while(z <= tofHit->GetZ()) {
		Double_t x_old = fKfTrack->GetTrack()[0];
		Double_t y_old = fKfTrack->GetTrack()[1];
		Double_t z_old = z;
		if(z <= 300) {
		    z += 0.1;
		} else {
		    z += 1.;
		}
		fKfTrack->Extrapolate(z);
		Double_t x_new = fKfTrack->GetTrack()[0];
		Double_t y_new = fKfTrack->GetTrack()[1];
		Double_t z_new = z;
		length += TMath::Sqrt(TMath::Power(x_new-x_old, 2)+
				      TMath::Power(y_new-y_old, 2)+
				      TMath::Power(z_new-z_old, 2));
	    }
	    Double_t x_old = fKfTrack->GetTrack()[0];
	    Double_t y_old = fKfTrack->GetTrack()[1];
	    Double_t z_old = z;
	    fKfTrack->Extrapolate(tofHit->GetZ());
	    Double_t x_new = fKfTrack->GetTrack()[0];
	    Double_t y_new = fKfTrack->GetTrack()[1];
	    Double_t z_new = tofHit->GetZ();
	    length += TMath::Sqrt(TMath::Power(x_new-x_old, 2)+
				  TMath::Power(y_new-y_old, 2)+
				  TMath::Power(z_new-z_old, 2));

	    glbTrack->SetLength(length);
	}
    }

    // Delete KF hits
    vector<CbmKFHit*>::iterator iter;
    for(iter = fKfTrack->fHits.begin(); iter != fKfTrack->fHits.end(); iter++) {
        delete (*iter);
    }
    fKfTrack->fHits.clear();
}
// ------------------------------------------------------------------


ClassImp(CbmGlobalTrackFitterKF);

