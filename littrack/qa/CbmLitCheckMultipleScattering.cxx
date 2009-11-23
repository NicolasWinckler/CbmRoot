#include "CbmLitCheckMultipleScattering.h"

#include "FairRootManager.h"
#include "CbmMCTrack.h"
#include "FairMCPoint.h"
#include "CbmLitDetPoint.h"

#include "TClonesArray.h"
#include "TH1F.h"
#include "TPad.h"

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

    fMCPointArray  = (TClonesArray*) ioman->ActivateBranch("LitDetPoint");
    if (!fMCPointArray) Fatal("Init", "No LitPoint array!");

    Int_t nBins = 1000;
    Double_t minTheta = -0.2;
    Double_t maxTheta =  0.2;
    Double_t minEloss = -0.001;
    Double_t maxEloss = 0.1;

    fh_theta_mc = new TH1F("fh_theta_mc", "theta_mc", nBins, minTheta, maxTheta);
    fh_eloss_mc = new TH1F("fh_eloss_mc", "eloss_mc", nBins, minEloss, maxEloss);
    fh_eloss_rec = new TH1F("fh_eloss_rec", "eloss_rec", nBins, minEloss, maxEloss);;
    return kSUCCESS;
}

void CbmLitCheckMultipleScattering::SetParContainers()
{
}

void CbmLitCheckMultipleScattering::Exec(
		Option_t* opt)
{
	Int_t nofTracks = fMCTrackArray->GetEntriesFast();
	std::cout << "nofTracks=" << nofTracks << std::endl;
	for (Int_t iTrack = 0; iTrack < nofTracks; iTrack++) {
        CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTrackArray->At(iTrack);
        Int_t motherId = mcTrack->GetMotherId();
        if (motherId != -1) continue;

        CbmLitDetPoint* point = NULL;

    	Int_t nofPoints = fMCPointArray->GetEntriesFast();
        for (Int_t iPoint = 0; iPoint < nofPoints; iPoint++){
        	CbmLitDetPoint* pnt = (CbmLitDetPoint*) fMCPointArray->At(iPoint);
        	Int_t trackId = pnt->GetTrackID();
        	if (trackId != iTrack) continue;
        	point = pnt;
        	break;
        }
        if (point == NULL) continue;

        TVector3 mom, momOut;
        point->Momentum(mom);
        point->MomentumOut(momOut);

    	Double_t mc_theta = momOut.Theta() * std::cos(momOut.Phi());
        fh_theta_mc->Fill(mc_theta);

        Double_t mc_eloss = mom.Mag() - momOut.Mag();
        std::cout << "mc_eloss=" << mc_eloss << std::endl;
        fh_eloss_mc->Fill(mc_eloss);

        Double_t length = 0.1;
        Double_t p = mom.Mag();
        Double_t m = 0.000511;
        Double_t initialE = std::sqrt(p*p+m*m);
        Double_t X0 = 9.34961;
        Double_t dEdX = initialE / X0;
        Double_t rec_eloss = dEdX * length;

        std::cout << "rec_eloss=" << rec_eloss << std::endl;
        fh_eloss_rec->Fill(rec_eloss);
	} //loop over MC tracks

	std::cout << "Event no: " << fNEvents++ << std::endl;
}

void CbmLitCheckMultipleScattering::Finish()
{
    fh_theta_mc->Draw();
    gPad->SetLogy();
    fh_eloss_mc->Draw();
    fh_eloss_rec->Draw("same");
}

ClassImp(CbmLitCheckMultipleScattering)
