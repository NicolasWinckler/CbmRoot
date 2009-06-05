//-------------------------------------------------------------------------
//-----                      CbmTrdFitTracksQa                        -----
//-----                 Created 29/11/05 by D. Kresan                 -----
//-------------------------------------------------------------------------
#include "CbmTrdFitTracksQa.h"

#include "CbmTrdPoint.h"
#include "CbmTrdHit.h"
#include "CbmTrdTrack.h"

#include "FairRootManager.h"

#include "TClonesArray.h"
#include "TH1F.h"
#include "TMath.h"

#include <iostream>
using std::cout;
using std::endl;


//____________________________________________________________________
//
// CbmTrdFitTracksQa
//
// Task for performance analysis of the TRD track fitting
//


// -------------------------------------------------------------------
CbmTrdFitTracksQa::CbmTrdFitTracksQa()
{
    // Default constructor
    fArrayTrdPoint = fArrayTrdHit = fArrayTrdTrack = NULL;
    CreateHistogramms();
}
// -------------------------------------------------------------------


// -------------------------------------------------------------------
CbmTrdFitTracksQa::CbmTrdFitTracksQa(const char* name, Int_t verbose)
: FairTask(name, verbose)
{
    // Standard constructor
    fArrayTrdPoint = fArrayTrdHit = fArrayTrdTrack = NULL;
    CreateHistogramms();
}
// -------------------------------------------------------------------


// -------------------------------------------------------------------
CbmTrdFitTracksQa::~CbmTrdFitTracksQa()
{
    // Destructor
}
// -------------------------------------------------------------------


// -------------------------------------------------------------------
InitStatus CbmTrdFitTracksQa::Init()
{
    // Task initialisation

    // Get the pointer to the ROOT IO manager
    FairRootManager* rootMgr = FairRootManager::Instance();
    if(NULL == rootMgr) {
        cout << "-E- CbmTrdFitTracksQa::Init : "
            << "root manager is not instantiated!" << endl;
        return kFATAL;
    }

    // Get pointer to array of TRD MC points
    fArrayTrdPoint =
        (TClonesArray*) rootMgr->GetObject("TRDPoint");
    if(NULL == fArrayTrdPoint) {
        cout << "-W- CbmTrdFitTracksQa::Init : "
            << "no TRDPoint array!" << endl;
    }

    // Get pointer to array of TRD hits
    fArrayTrdHit =
        (TClonesArray*) rootMgr->GetObject("TRDHit");
    if(NULL == fArrayTrdHit) {
        cout << "-W- CbmTrdFitTracksQa::Init : "
            << "no TRDHit array!" << endl;
    }

    // Get pointer to array of TRD tracks
    fArrayTrdTrack =
        (TClonesArray*) rootMgr->GetObject("TRDTrack");
    if(NULL == fArrayTrdTrack) {
        cout << "-W- CbmTrdFitTracksQa::Init : "
            << "no TRD tracks!" << endl;
    }

    return kSUCCESS;
}
// -------------------------------------------------------------------


// -------------------------------------------------------------------
void CbmTrdFitTracksQa::Exec(Option_t* option)
{
    // Task execution

    if(NULL==fArrayTrdTrack || NULL==fArrayTrdHit || NULL==fArrayTrdPoint)
        return;

    TVector3 firstPos, firstMom;
    TVector3 lastPos, lastMom;
    CbmTrdPoint* pFirstPoint = NULL;
    CbmTrdPoint* pLastPoint = NULL;
    CbmTrdHit* pFirstHit = NULL;
    CbmTrdHit* pLastHit = NULL;
    const FairTrackParam* pFirstParam = NULL;
    const FairTrackParam* pLastParam = NULL;
    CbmTrdTrack* pTrack = NULL;

    // Loop over TRD tracks
    for(Int_t iTrack = 0; iTrack < fArrayTrdTrack->GetEntriesFast();
        iTrack++) {

        // Get the pointer to the track
        pTrack = (CbmTrdTrack*) fArrayTrdTrack->At(iTrack);
        if(NULL == pTrack) {
            cout << "-E- CbmTrdFitTracksQa::Exec : "
                << "empty TRDTrack slot : " << iTrack << ", "
                << pTrack << " !" << endl;
            return;
        }

	if(pTrack->GetFlag()) continue;

	// Fill chi2/NDF histogramms
        Int_t ndf = pTrack->GetNDF();
	if(ndf > 0) {
	    fh_chi2ndf->Fill( pTrack->GetChiSq()/(Double_t)ndf );
	}

	if(pTrack->GetNofHits() < 2) continue;

        // Get parameters at first and last TRD layer
        pFirstParam = pTrack->GetParamFirst();
        pLastParam = pTrack->GetParamLast();
        if(NULL==pFirstParam || NULL==pLastParam) {
            cout << "-E- CbmTrdFitTracksQa::Exec : "
                << "no track parameters : " << pFirstParam
                << ", " << pLastParam << " !" << endl;
            return;
        }

        // Get the pointer to TRD point at first and last layer
        Int_t firstHitIndex = pTrack->GetHitIndex(0);
        Int_t lastHitIndex = pTrack->GetHitIndex( pTrack->GetNofHits()-1 );
        pFirstHit = (CbmTrdHit*) fArrayTrdHit->At(firstHitIndex);
        pLastHit = (CbmTrdHit*) fArrayTrdHit->At(lastHitIndex);
        if(NULL==pFirstHit || NULL==pLastHit) {
            cout << "-E- CbmTrdFitTracksQa::Exec : "
                << "no hit : " << pFirstHit << ", " << pLastHit
                << " !" << endl;
        }
        Int_t firstPointIndex = pFirstHit->GetRefId();
        Int_t lastPointIndex = pLastHit->GetRefId();
        pFirstPoint = (CbmTrdPoint*) fArrayTrdPoint->At(firstPointIndex);
        pLastPoint = (CbmTrdPoint*) fArrayTrdPoint->At(lastPointIndex);
        if(NULL==pFirstPoint || NULL==pLastPoint) {
            cout << "-E- CbmTrdFitTracksQa::Exec : "
                << "no hit : " << pFirstPoint << ", " << pLastPoint
                << " !" << endl;
        }

        // Get position and momentum of first and last points
        pFirstPoint->Position(firstPos);
        pFirstPoint->Momentum(firstMom);
        pLastPoint->Position(lastPos);
        pLastPoint->Momentum(lastMom);

        // Calculate tx and ty for first and last points
        Double_t first_tx = firstMom.X() / firstMom.Z();
        Double_t first_ty = firstMom.Y() / firstMom.Z();
        Double_t last_tx = lastMom.X() / lastMom.Z();
        Double_t last_ty = lastMom.Y() / lastMom.Z();

        // Fill the residuals histogramms
        fh_first_resx->Fill( pFirstParam->GetX() - firstPos.X() );
        fh_first_resy->Fill( pFirstParam->GetY() - firstPos.Y() );
        fh_first_restx->Fill( pFirstParam->GetTx() - first_tx );
        fh_first_resty->Fill( pFirstParam->GetTy() - first_ty );
        fh_first_resz->Fill( pFirstParam->GetZ() - firstPos.Z() );
        fh_last_resx->Fill( pLastParam->GetX() - lastPos.X() );
        fh_last_resy->Fill( pLastParam->GetY() - lastPos.Y() );
        fh_last_restx->Fill( pLastParam->GetTx() - last_tx );
        fh_last_resty->Fill( pLastParam->GetTy() - last_ty );
        fh_last_resz->Fill( pLastParam->GetZ() - lastPos.Z() );

        // Fill the pulls histogramms
        fh_first_pullx->Fill( (pFirstParam->GetX()-firstPos.X())/
                              TMath::Sqrt(pFirstParam->GetCovariance(0,0)) );
        fh_first_pully->Fill( (pFirstParam->GetY()-firstPos.Y())/
                              TMath::Sqrt(pFirstParam->GetCovariance(1,1)) );
        fh_first_pulltx->Fill( (pFirstParam->GetTx()-first_tx)/
                               TMath::Sqrt(pFirstParam->GetCovariance(2,2)) );
        fh_first_pullty->Fill( (pFirstParam->GetTy()-first_ty)/
                               TMath::Sqrt(pFirstParam->GetCovariance(3,3)) );
        fh_last_pullx->Fill( (pLastParam->GetX()-lastPos.X())/
                             TMath::Sqrt(pLastParam->GetCovariance(0,0)) );
        fh_last_pully->Fill( (pLastParam->GetY()-lastPos.Y())/
                             TMath::Sqrt(pLastParam->GetCovariance(1,1)) );
        fh_last_pulltx->Fill( (pLastParam->GetTx()-last_tx)/
                              TMath::Sqrt(pLastParam->GetCovariance(2,2)) );
        fh_last_pullty->Fill( (pLastParam->GetTy()-last_ty)/
                              TMath::Sqrt(pLastParam->GetCovariance(3,3)) );
    }
    // Increment number of processed events
    fNoEvents += 1;
}
// -------------------------------------------------------------------


// -------------------------------------------------------------------
void CbmTrdFitTracksQa::Finish()
{
    // Finish of task execution
    WriteHistogramms();
}
// -------------------------------------------------------------------


// -------------------------------------------------------------------
void CbmTrdFitTracksQa::CreateHistogramms()
{
    // Create control histogramms

    // Residuals at first TRD layer
    fh_first_resx = new TH1F("h_first_resx",
                             "x residual at first TRD layer",
                             200, -1., 1.);
    fh_first_resy = new TH1F("h_first_resy",
                             "y residual at first TRD layer",
                             200, -1., 1.);
    fh_first_restx = new TH1F("h_first_restx",
                              "t_{x} residual at first TRD layer",
                              200, -0.01, 0.01);
    fh_first_resty = new TH1F("h_first_resty",
                              "t_{y} residual at first TRD layer",
                              200, -0.01, 0.01);
    fh_first_resz = new TH1F("h_first_resz",
                             "z residual at first TRD layer",
                             100, -0.5, 0.5);

    // Residuals at last TRD layer
    fh_last_resx = new TH1F("h_last_resx",
                            "x residual at last TRD layer",
                            200, -1., 1.);
    fh_last_resy = new TH1F("h_last_resy",
                            "y residual at last TRD layer",
                            200, -1., 1.);
    fh_last_restx = new TH1F("h_last_restx",
                             "t_{x} residual at last TRD layer",
                             200, -0.01, 0.01);
    fh_last_resty = new TH1F("h_last_resty",
                             "t_{y} residual at last TRD layer",
                             200, -0.01, 0.01);
    fh_last_resz = new TH1F("h_last_resz",
                            "z residual at last TRD layer",
                            100, -0.5, 0.5);

    // Pulls at first TRD layer
    fh_first_pullx = new TH1F("h_first_pullx",
                              "x pull at first TRD layer",
                              200, -10., 10.);
    fh_first_pully = new TH1F("h_first_pully",
                              "y pull at first TRD layer",
                              200, -10., 10.);
    fh_first_pulltx = new TH1F("h_first_pulltx",
                               "t_{x} pull at first TRD layer",
                               200, -10., 10.);
    fh_first_pullty = new TH1F("h_first_pullty",
                               "t_{y} pull at first TRD layer",
                               200, -10., 10.);

    // Pulls at last TRD layer
    fh_last_pullx = new TH1F("h_last_pullx",
                             "x pull at last TRD layer",
                             200, -10., 10.);
    fh_last_pully = new TH1F("h_last_pully",
                             "y pull at last TRD layer",
                             200, -10., 10.);
    fh_last_pulltx = new TH1F("h_last_pulltx",
                              "t_{x} pull at last TRD layer",
                             200, -10., 10.);
    fh_last_pullty = new TH1F("h_last_pullty",
                              "t_{y} pull at last TRD layer",
                              200, -10., 10.);

    // Chi2/NDF of track fit
    fh_chi2ndf = new TH1F("h_chi2ndf",
                          "#chi^{2}/NDF of track fit",
                          500, 0., 50.);
}
// -------------------------------------------------------------------


// -------------------------------------------------------------------
void CbmTrdFitTracksQa::WriteHistogramms()
{
    // Write control histogramms to file

    // Residuals at first and last TRD layer
    fh_first_resx->Scale(1./fNoEvents);
    fh_first_resy->Scale(1./fNoEvents);
    fh_first_restx->Scale(1./fNoEvents);
    fh_first_resty->Scale(1./fNoEvents);
    fh_last_resx->Scale(1./fNoEvents);
    fh_last_resy->Scale(1./fNoEvents);
    fh_last_restx->Scale(1./fNoEvents);
    fh_last_resty->Scale(1./fNoEvents);
    fh_first_resx->Write();
    fh_first_resy->Write();
    fh_first_restx->Write();
    fh_first_resty->Write();
    fh_first_resz->Write();
    fh_last_resx->Write();
    fh_last_resy->Write();
    fh_last_restx->Write();
    fh_last_resty->Write();
    fh_last_resz->Write();

    // Pulls at first and last TRD layer
    fh_first_pullx->Scale(1./fNoEvents);
    fh_first_pully->Scale(1./fNoEvents);
    fh_first_pulltx->Scale(1./fNoEvents);
    fh_first_pullty->Scale(1./fNoEvents);
    fh_last_pullx->Scale(1./fNoEvents);
    fh_last_pully->Scale(1./fNoEvents);
    fh_last_pulltx->Scale(1./fNoEvents);
    fh_last_pullty->Scale(1./fNoEvents);
    fh_first_pullx->Write();
    fh_first_pully->Write();
    fh_first_pulltx->Write();
    fh_first_pullty->Write();
    fh_last_pullx->Write();
    fh_last_pully->Write();
    fh_last_pulltx->Write();
    fh_last_pullty->Write();

    // Chi2/NDF of fit
    fh_chi2ndf->Scale(1./fNoEvents);
    fh_chi2ndf->Write();
}
// -------------------------------------------------------------------


ClassImp(CbmTrdFitTracksQa);



