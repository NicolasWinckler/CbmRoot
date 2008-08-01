// -------------------------------------------------------------------------
// -----                CbmLitMuchAna source file            -----
// -----                  Created 01/10/07  by A. Lebedev              -----
// -------------------------------------------------------------------------

#include "CbmLitMuchAna.h"

#include "CbmLitTrackUpdate.h"
#include "CbmLitTrackExtrapolator.h"
#include "CbmLitTrackPropagator.h"
#include "CbmLitKalmanFilter.h"
#include "CbmLitTrackPropagatorImp.h"
#include "CbmLitLineTrackExtrapolator.h"
#include "CbmLitRK4TrackExtrapolator.h"
#include "CbmTrackPropagatorGeane.h"
#include "CbmLitConverter.h"
#include "CbmLitTrack.h"
#include "CbmLitTrackFitter.h"
#include "CbmLitTrackFitterImp.h"
#include "CbmLitKalmanSmoother.h"

#include "CbmMuchHit.h"
#include "CbmMuchTrack.h"
#include "CbmMuchDigi.h"
#include "CbmMuchDigiMatch.h"
#include "CbmDetectorList.h"
#include "CbmMCPoint.h"
#include "CbmMCTrack.h"
#include "CbmMCTrack.h"
#include "CbmRootManager.h"
#include "CbmRunAna.h"
#include "CbmRuntimeDb.h"

#include "TClonesArray.h"
#include "TH1F.h"

#include <iostream>
#include <iomanip>
#include <map>


CbmLitMuchAna::CbmLitMuchAna() { 
  fMCTrackArray = NULL;
  fMCPointArray = NULL;
  fVerbose = 1;
  fEvents = 0;
}

CbmLitMuchAna::~CbmLitMuchAna() 
{ 

}

InitStatus CbmLitMuchAna::Init() {

   // Get and check CbmRootManager
   CbmRootManager* ioman = CbmRootManager::Instance();
   if ( ! ioman ) Fatal("Init", "No CbmRootManager");
   
   // Get MCTrack array
   fMCTrackArray  = (TClonesArray*) ioman->GetObject("MCTrack");
   if ( ! fMCTrackArray ) Fatal("Init", "No MCTrack array!");
   
   // Get MCPoint array
   fMCPointArray  = (TClonesArray*) ioman->GetObject("MuchPoint");
   if ( ! fMCPointArray ) Fatal("Init", "No MuchPoint array!");
   
   fMuchHits = (TClonesArray*) ioman->GetObject("MuchHit");
   if ( ! fMuchHits ) Fatal("Init", "No fMuchHits array!");
   
      // Get Much digi array
   fMuchDigis  = (TClonesArray*) ioman->GetObject("MuchDigi");
   if ( ! fMuchDigis ) Fatal("Init", "No MuchDigis array!");
   
   // Get Much digi match array
   fMuchDigiMatches  = (TClonesArray*) ioman->GetObject("MuchDigiMatch");
   if ( ! fMuchDigiMatches ) Fatal("Init", "No MuchDigiMatches array!");
      
   fMuchTracks = (TClonesArray*) ioman->GetObject("MuchTrack");
   if ( ! fMuchTracks ) Fatal("Init", "No trackArray!");
     
   fExtrapolator = new CbmLitRK4TrackExtrapolator();
   //fExtrapolator = new CbmLitLineTrackExtrapolator();
   fExtrapolator->Initialize();
   fPropagator = new CbmLitTrackPropagatorImp(fExtrapolator);
   // fPropagator = new CbmTrackPropagatorGeane();
    fPropagator->Initialize();
     
   fFilter = new CbmLitKalmanFilter();
   fFilter->Initialize();
   
   fFitter = new CbmLitTrackFitterImp(fPropagator, fFilter);
   
   fSmoother = new CbmLitKalmanSmoother();
   
   fNofLayers = 10;
   CreateHistograms();
   

   
}

void CbmLitMuchAna::SetParContainers()
{
}

void CbmLitMuchAna::Exec(Option_t* opt) 
{
  Int_t nofTracks = fMuchTracks->GetEntriesFast();
  for (Int_t iTrack = 0; iTrack < nofTracks; iTrack++) {
      CbmMuchTrack* pTrack = (CbmMuchTrack*) fMuchTracks->At(iTrack);
      Int_t nofHits = pTrack->GetNHits();
      if(nofHits != fNofLayers) continue;

      std::vector<CbmMCPoint*> points;
      CbmLitTrack track;
      
      for (Int_t iHit = 0; iHit < nofHits; iHit++){
         Int_t hitIndex = pTrack->GetHitIndex(iHit);
         CbmMuchHit* pHit = (CbmMuchHit*) fMuchHits->At(hitIndex);
         CbmLitHit hit;
         CbmLitConverter::TrkHitToLitHit(pHit, 0, &hit);
         track.AddHit(&hit);
         
         Int_t digiIndex = pHit->GetDigi();
         CbmMuchDigiMatch* pDigiMatch = (CbmMuchDigiMatch*) fMuchDigiMatches->At(digiIndex);    
         Int_t pointIndex = pDigiMatch->GetRefIndex(0);
         
         if (pointIndex < 0) Fatal("MUCH Ana", "Wrong PointIndex");
                  
         CbmMCPoint* pPoint = (CbmMCPoint*) fMCPointArray->At(pointIndex);
         
         if (!pPoint) Fatal("MUCH Ana", "Wrong Point pointer");
         
         points.push_back(pPoint);
      }
      
      CbmLitTrackParam par;
      //FillParam(points[0], &par);
      CbmLitConverter::TrackParamToLitTrackParam(&(*pTrack->GetMuchTrack()), &par);
            
      track.SetParamFirst(&par);
      track.SetParamLast(&par);
      track.SetPDG(13);
          
      std::vector<Double_t> res(5), pull(5);
      Double_t resp;
      Int_t mcId = points[0]->GetTrackID();
      CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTrackArray->At(mcId);
      
      for (Int_t i = 0; i < nofHits; i++){
      	//Int_t i = 9;
      
         Double_t zOut = track.GetHit(i)->GetZ(); 
         if (fPropagator->Propagate(&par, zOut, 13) == kLITERROR) continue;
         //fFilter->Update(&par, track.GetHit(i));
         
         fh_srx[i]->Fill(par.GetX() - track.GetHit(i)->GetX());
         fh_sry[i]->Fill(par.GetY() - track.GetHit(i)->GetY());
         
         CalcResAndPull(mcTrack, points[i], &par, res, pull, resp);
         
         fh_resx[i]->Fill(res[0]);
         fh_resy[i]->Fill(res[1]); 
         fh_restx[i]->Fill(res[2]);
         fh_resty[i]->Fill(res[3]);
         fh_resqp[i]->Fill(res[4]);
         fh_resp[i]->Fill(resp);
       	 fh_pullx[i]->Fill(pull[0]); 
       	 fh_pully[i]->Fill(pull[1]); 
       	 fh_pulltx[i]->Fill(pull[2]);
       	 fh_pullty[i]->Fill(pull[3]);
       	 fh_pullqp[i]->Fill(pull[4]);
       	 
       	 fFilter->Update(&par, track.GetHit(i));
      }  
      
	  //fFitter->Properties().SetProperty("fDownstream", true);
	  if (fFitter->Fit(&track) == kLITERROR) continue;
	  CalcResAndPull(mcTrack, points[nofHits-1], track.GetParamLast(), res, pull, resp);
      fh_resx_last->Fill(res[0]);
      fh_resy_last->Fill(res[1]); 
      fh_restx_last->Fill(res[2]);
      fh_resty_last->Fill(res[3]);
      fh_resqp_last->Fill(res[4]);
      fh_resp_last->Fill(resp);
      fh_pullx_last->Fill(pull[0]);
      fh_pully_last->Fill(pull[1]); 
      fh_pulltx_last->Fill(pull[2]);
      fh_pullty_last->Fill(pull[3]);
      fh_pullqp_last->Fill(pull[4]);

      //fFitter->Properties().SetProperty("fDownstream", false);
	  //fFitter->Fit(&track);
//      fSmoother->Fit(&track);
//	  CalcResAndPull(mcTrack, points[0], track.GetParamFirst(), res, pull, resp);
//      fh_resx_first->Fill(res[0]);
//      fh_resy_first->Fill(res[1]); 
//      fh_restx_first->Fill(res[2]);
//      fh_resty_first->Fill(res[3]);
//      fh_resqp_first->Fill(res[4]);
//      fh_resp_first->Fill(resp);
//      fh_pullx_first->Fill(pull[0]);
//      fh_pully_first->Fill(pull[1]); 
//      fh_pulltx_first->Fill(pull[2]);
//      fh_pullty_first->Fill(pull[3]);
//      fh_pullqp_first->Fill(pull[4]);
  }
            
  fEvents++;
  std::cout << "Event number: " << fEvents << std::endl;
  std::cout << "---------------------------------------------" << std::endl;
}

void CbmLitMuchAna::Finish()
{
   for (Int_t i = 0; i < fNofLayers; i++) {
      fh_srx[i]->Write();
      fh_sry[i]->Write();
      
      fh_resy[i]->Write();
      fh_resx[i]->Write();
      fh_resty[i]->Write();
      fh_restx[i]->Write();      
      fh_resqp[i]->Write(); 
      
      fh_pully[i]->Write();
      fh_pullx[i]->Write();
      fh_pullty[i]->Write();
      fh_pulltx[i]->Write();      
      fh_pullqp[i]->Write();     
      
      fh_resp[i]->Write();  
   }
   
   fh_pullx_last->Write();
   fh_pullx_first->Write();
   fh_pully_last->Write();
   fh_pully_first->Write();
   fh_pulltx_last->Write();
   fh_pulltx_first->Write();
   fh_pullty_last->Write();
   fh_pullty_first->Write();
   fh_pullqp_last->Write();
   fh_pullqp_first->Write();
   
   fh_resx_last->Write();
   fh_resx_first->Write();
   fh_resy_last->Write();
   fh_resy_first->Write();
   fh_restx_last->Write();
   fh_restx_first->Write();
   fh_resty_last->Write();
   fh_resty_first->Write();
   fh_resqp_last->Write();
   fh_resqp_first->Write();
   
   fh_resp_last->Write();
   fh_resp_first->Write();
}

void CbmLitMuchAna::CalcResAndPull(
		const CbmMCTrack* mcTrack,
		const CbmMCPoint* point,
		const CbmLitTrackParam* par,
		std::vector<Double_t>& res,
		std::vector<Double_t>& pull,
		Double_t& resp)
{
    Double_t x = point->GetX();
    Double_t y = point->GetY();
    Double_t px = point->GetPx();
    Double_t py = point->GetPy();
    Double_t pz = point->GetPz();
    Double_t tx = px / pz;
    Double_t ty = py / pz;

    Int_t pdg = mcTrack->GetPdgCode();
    Double_t q;
    if (pdg > 0) q = -1.; else q = 1.;         
    Double_t qp = q / sqrt(px * px + py * py + pz * pz);
     
    res[0] = par->GetX() - x;
    res[1] = par->GetY() - y;
    res[2] = par->GetTx() - tx;
    res[3] = par->GetTy() - ty;
    res[4] = par->GetQp() - qp;     
    
    resp = 100 * (((1. / fabs(par->GetQp())) - (1. / fabs(qp))) / (1. / qp));                
     
    if (par->GetCovariance(0) != 0) pull[0] = (res[0]) / (sqrt(par->GetCovariance(0)));
    if (par->GetCovariance(5) != 0) pull[1] = (res[1]) / (sqrt(par->GetCovariance(5)));
    if (par->GetCovariance(9) != 0) pull[2] = (res[2]) / (sqrt(par->GetCovariance(9)));
    if (par->GetCovariance(12) != 0) pull[3] = (res[3]) / (sqrt(par->GetCovariance(12)));
    if (par->GetCovariance(14) != 0) pull[4] = (res[4]) / (sqrt(par->GetCovariance(14)));
  
}

void CbmLitMuchAna::FillParam(
		   const CbmMCPoint* point,
		   CbmLitTrackParam* par)
{
    
    Double_t x = point->GetX();
    Double_t y = point->GetY();
    Double_t z = point->GetZ();
    Double_t px = point->GetPx();
    Double_t py = point->GetPy();
    Double_t pz = point->GetPz();
    Double_t tx = px / pz;
    Double_t ty = py / pz;
    
	Int_t mcId = point->GetTrackID();
    CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTrackArray->At(mcId);
    Int_t pdg = mcTrack->GetPdgCode();
    Double_t q;
    if (pdg > 0) q = -1.; else q = 1.;         
    Double_t qp = q / sqrt(px * px + py * py + pz * pz);
    
    par->SetX(x);
    par->SetY(y);
    par->SetZ(z);
    par->SetTx(tx);
    par->SetTy(ty);
    par->SetQp(qp);
}

void CbmLitMuchAna::CreateHistograms()
{
   fh_srx.resize(fNofLayers);
   fh_sry.resize(fNofLayers);
   
   fh_resx.resize(fNofLayers);
   fh_resy.resize(fNofLayers);
   fh_restx.resize(fNofLayers);
   fh_resty.resize(fNofLayers);
   fh_resqp.resize(fNofLayers);
   
   fh_pullx.resize(fNofLayers);
   fh_pully.resize(fNofLayers);
   fh_pulltx.resize(fNofLayers);
   fh_pullty.resize(fNofLayers);
   fh_pullqp.resize(fNofLayers);
   
   fh_resp.resize(fNofLayers);
   
   char histName[100];
   char histTitle[100];
   
   Int_t nBins = 200;
   Double_t minPull = -15.;
   Double_t maxPull = 15.;
   Double_t minRes = -10.;
   Double_t maxRes = 10.;
   
   for (Int_t i = 0; i < fNofLayers; i++) {
       
      sprintf(histName, "fh_srx%i",i);
      sprintf(histTitle, "Search Region X. X_pred - X_hit at %i MUCH layer",i);
      fh_srx[i] = new TH1F(histName, histTitle, nBins, minPull, maxPull);
                                   
      sprintf(histName, "fh_sry%i",i);
      sprintf(histTitle, "Search Region Y. Y_pred - Y_hit at %i MUCH layer",i);
      fh_sry[i] = new TH1F(histName, histTitle, nBins, minPull, maxPull); 
   
   
      sprintf(histName, "fh_resx%i",i);
      sprintf(histTitle, "Resolution X. X_pred - X_point at %i MUCH layer",i);
      fh_resx[i] = new TH1F(histName, histTitle, nBins, minRes, maxRes);
                                   
      sprintf(histName, "fh_resy%i",i);
      sprintf(histTitle, "Resolution Y. Y_pred - Y_point at %i MUCH layer",i);
      fh_resy[i] = new TH1F(histName, histTitle, nBins, minRes, maxRes);
      
      sprintf(histName, "fh_restx%i",i);
      sprintf(histTitle, "Resolution Tx. Tx_pred - Px/Pz at %i MUCH layer",i);
      fh_restx[i] = new TH1F(histName, histTitle, nBins, minRes, maxRes);
                                   
      sprintf(histName, "fh_resty%i",i);
      sprintf(histTitle, "Resolution Ty. Ty_pred - Py/Pz at %i MUCH layer",i);
      fh_resty[i] = new TH1F(histName, histTitle, nBins, minRes, maxRes);
      
      sprintf(histName, "fh_resqp%i",i);
      sprintf(histTitle, "Resolution Q/P. qp_pred - qp_point at %i MUCH layer",i);
      fh_resqp[i] = new TH1F(histName, histTitle, nBins, minRes, maxRes);      
      
            
      sprintf(histName, "fh_pullx%i",i);
      sprintf(histTitle, "Pull X at%i MUCH layer",i);
      fh_pullx[i] = new TH1F(histName, histTitle, nBins, minPull, maxPull);
      
      sprintf(histName, "fh_pully%i",i);
      sprintf(histTitle, "Pull Y at %i MUCH layer",i);
      fh_pully[i] = new TH1F(histName, histTitle, nBins, minPull, maxPull);
       
      sprintf(histName, "fh_pulltx%i",i);
      sprintf(histTitle, "Pull Tx at %i MUCH layer",i);
      fh_pulltx[i] = new TH1F(histName, histTitle, nBins, minPull, maxPull);
      
      sprintf(histName, "fh_pullty%i",i);
      sprintf(histTitle, "Pull Ty at %i MUCH layer",i);
      fh_pullty[i] = new TH1F(histName, histTitle, nBins, minPull, maxPull);
      
      sprintf(histName, "fh_pullqp%i",i);
      sprintf(histTitle, "Pull q/p at %i MUCH layer",i);
      fh_pullqp[i] = new TH1F(histName, histTitle, nBins, minPull, maxPull);
      
      sprintf(histName, "fh_resp%i",i);
      sprintf(histTitle, "Res % p at %i MUCH layer",i);
      fh_resp[i] = new TH1F(histName, histTitle, nBins, -35., 35.);
   }
   
   fh_pullx_last = new TH1F("fh_pullx_last", "pull x of the last track param", nBins, minPull, maxPull);
   fh_pullx_first = new TH1F("fh_pullx_first", "pull x of the first track param", nBins, minPull, maxPull);
   fh_pully_last = new TH1F("fh_pully_last", "pull y of the last track param", nBins, minPull, maxPull);
   fh_pully_first = new TH1F("fh_pully_first", "pull y of the first track param", nBins, minPull, maxPull);
   fh_pulltx_last = new TH1F("fh_pulltx_last", "pull tx of the last track param", nBins, minPull, maxPull);
   fh_pulltx_first = new TH1F("fh_pulltx_first", "pull tx of the first track param", nBins, minPull, maxPull);
   fh_pullty_last = new TH1F("fh_pullty_last", "pull ty of the last track param", nBins, minPull, maxPull);
   fh_pullty_first = new TH1F("fh_pullty_first", "pull ty of the first track param", nBins, minPull, maxPull);
   fh_pullqp_last = new TH1F("fh_pullqp_last", "pull qp of the last track param", nBins, minPull, maxPull);
   fh_pullqp_first = new TH1F("fh_pullqp_first", "pull qp of the first track param", nBins, minPull, maxPull);
   
   fh_resx_last = new TH1F("fh_resx_last", "res x of the last track param", nBins, minRes, maxRes);
   fh_resx_first = new TH1F("fh_resx_first", "res x of the first track param", nBins, minRes, maxRes);
   fh_resy_last = new TH1F("fh_resy_last", "res y of the last track param", nBins, minRes, maxRes);
   fh_resy_first = new TH1F("fh_resy_first", "res y of the first track param", nBins, minRes, maxRes);
   fh_restx_last = new TH1F("fh_restx_last", "res tx of the last track param", nBins, 0.1*minRes, 0.1*maxRes);
   fh_restx_first = new TH1F("fh_restx_first", "res tx of the first track param", nBins, 0.1*minRes, 0.1*maxRes);
   fh_resty_last = new TH1F("fh_resty_last", "res ty of the last track param", nBins, 0.1*minRes, 0.1*maxRes);
   fh_resty_first = new TH1F("fh_resty_first", "res ty of the first track param", nBins, 0.1*minRes, 0.1*maxRes);
   fh_resqp_last = new TH1F("fh_resqp_last", "res qp of the last track param", nBins, minRes, maxRes);
   fh_resqp_first = new TH1F("fh_resqp_first", "res qp of the first track param", nBins, minRes, maxRes);
   
   fh_resp_last = new TH1F("fh_resp_last", "res qp of the last track param", nBins, -15., 15.);
   fh_resp_first = new TH1F("fh_resp_first", "res qp of the first track param", nBins, -15, 15.);
}

ClassImp(CbmLitMuchAna)

  
