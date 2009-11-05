#include "CbmLitCheckMultipleScattering.h"

#include "FairRootManager.h"
#include "CbmMCTrack.h"
#include "FairMCPoint.h"
#include "CbmMuchPoint.h"

#include "TClonesArray.h"
#include "TH1F.h"

#include <iostream>
#include <cmath>

CbmLitCheckMultipleScattering::CbmLitCheckMultipleScattering()
{
	fNEvents = 0;
}

CbmLitCheckMultipleScattering::~CbmLitCheckMultipleScattering()
{

}

InitStatus CbmLitCheckMultipleScattering::Init()
{
	FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman) Fatal("Init", "No CbmRootManager");

    fMCTrackArray  = (TClonesArray*) ioman->ActivateBranch("MCTrack");
    if (!fMCTrackArray) Fatal("Init", "No MCTrack array!");

    fMCPointArray  = (TClonesArray*) ioman->ActivateBranch("MuchPoint");
    if (!fMCPointArray) Fatal("Init", "No MuchPoint array!");

    Int_t nBins = 1000;
    Double_t minTheta = -0.2;
    Double_t maxTheta =  0.2;

    fh_theta_mc = new TH1F("fh_theta_mc", "theta_mc", nBins, minTheta, maxTheta);

    return kSUCCESS;
}

void CbmLitCheckMultipleScattering::SetParContainers()
{
}

void CbmLitCheckMultipleScattering::Exec(
		Option_t* opt)
{
//	Double_t mass = 0.105;
//	Double_t Q;
	Int_t nofTracks = fMCTrackArray->GetEntriesFast();
	for (Int_t iTrack = 0; iTrack < nofTracks; iTrack++) {
        CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTrackArray->At(iTrack);
        Int_t motherId = mcTrack->GetMotherId();
        if (motherId != -1) continue;

        CbmMuchPoint* point = NULL;

    	Int_t nofPoints = fMCPointArray->GetEntriesFast();
        for (Int_t iPoint = 0; iPoint < nofPoints; iPoint++){
        	CbmMuchPoint* pnt = (CbmMuchPoint*) fMCPointArray->At(iPoint);
        	Int_t trackId = pnt->GetTrackID();
        	if (trackId != iTrack) continue;
        	point = pnt;
        	break;
        }
        if (point == NULL) continue;

        TVector3 momIn, momOut;
        point->Momentum(momIn);
        point->MomentumOut(momOut);

    	Double_t mc_theta = momOut.Theta() * std::cos(momOut.Phi());
        fh_theta_mc->Fill(mc_theta);
	} //loop over MC tracks

	std::cout << "Event no: " << fNEvents++ << std::endl;
}

void CbmLitCheckMultipleScattering::Finish()
{
    fh_theta_mc->Write();
}

ClassImp(CbmLitCheckMultipleScattering)
