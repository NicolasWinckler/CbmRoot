#include "CbmLitPropAna.h"

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
#include "CbmLitHit.h"
#include "CbmLitTrackFitter.h"
#include "CbmLitTrackFitterImp.h"
#include "CbmLitKalmanSmoother.h"

#include "CbmTrkHit.h"
#include "CbmTrdTrack.h"
#include "CbmMuchHit.h"
#include "CbmMuchTrack.h"
#include "CbmMuchDigi.h"
#include "CbmMuchDigiMatch.h"
#include "CbmDetectorList.h"
#include "CbmMCPoint.h"
#include "CbmMCTrack.h"
#include "CbmRootManager.h"
#include "CbmRunAna.h"
#include "CbmRuntimeDb.h"

#include "TClonesArray.h"
#include "TH1F.h"

#include <iostream>
#include <cmath>
#include <sstream>

CbmLitPropAna::CbmLitPropAna(
		DetectorId detId,
		Int_t nofStations) 
{ 
  fVerbose = 1;
  fEvents = 0;
  fDetId = detId;
  fNofLayers = nofStations;
}

CbmLitPropAna::~CbmLitPropAna() 
{ 

}

InitStatus CbmLitPropAna::Init() 
{
   CbmRootManager* ioman = CbmRootManager::Instance();
   if (!ioman ) Fatal("Init", "No CbmRootManager");

   fMCTracks  = (TClonesArray*) ioman->GetObject("MCTrack");
   if (!fMCTracks) Fatal("Init", "No MCTrack array!");
   
   if (fDetId == kMUCH) {
	   fMCPoints  = (TClonesArray*) ioman->GetObject("MuchPoint");
	   if (!fMCPoints) Fatal("Init", "No MuchPoint array!");
	   fHits = (TClonesArray*) ioman->GetObject("MuchHit");
	   if (!fHits) Fatal("Init", "No MuchHit array!");
	   fDigiMatches  = (TClonesArray*) ioman->GetObject("MuchDigiMatch");
	   if (!fDigiMatches) Fatal("Init", "No MuchDigiMatches array!");
	   fTracks = (TClonesArray*) ioman->GetObject("MuchTrack");
	   if (!fTracks) Fatal("Init", "No MuchTrack array!");
   } 
   else
   if (fDetId == kTRD) {
	   fMCPoints  = (TClonesArray*) ioman->GetObject("TRDPoint");
	   if (!fMCPoints) Fatal("Init", "No TRDPoint array!");
	   fHits = (TClonesArray*) ioman->GetObject("TRDHit");
	   if (!fHits) Fatal("Init", "No TRDHit array!");
	   fTracks = (TClonesArray*) ioman->GetObject("TRDTrack");
	   if (!fTracks) Fatal("Init", "No TRDTrack array!");
   }
   else {
	   Fatal("Init", "CbmLitPropAna works only for MUCH and TRD!");
   }
   
   if (fDetId == kTRD) {
	   fExtrapolatorDet = new CbmLitRK4TrackExtrapolator();
	   fExtrapolatorDet->Initialize();
	   fPropagatorDet = new CbmLitTrackPropagatorImp(fExtrapolatorDet);
	   fPropagatorDet->Initialize();
   }
   
   fExtrapolator = new CbmLitLineTrackExtrapolator();
   fExtrapolator->Initialize();
   fPropagator = new CbmLitTrackPropagatorImp(fExtrapolator);
//    fPropagator = new CbmTrackPropagatorGeane();
    fPropagator->Initialize();
     
   fFilter = new CbmLitKalmanFilter();
   fFilter->Initialize();
   
   fFitter = new CbmLitTrackFitterImp(fPropagator, fFilter);
   
   fSmoother = new CbmLitKalmanSmoother();
   
//   fNofLayers = 12;
   CreateHistograms();   
}

void CbmLitPropAna::SetParContainers()
{
}

void CbmLitPropAna::Exec(
		Option_t* opt) 
{
  Int_t nofTracks = fTracks->GetEntriesFast();
  for (Int_t iTrack = 0; iTrack < nofTracks; iTrack++) {
      
	  std::vector<CbmMCPoint*> points;
      CbmLitTrack track;
      
      if (fDetId == kMUCH) {
    	  if (!CreateTrackMuch(iTrack, &track, points)) continue;
      } else 
      if (fDetId == kTRD) {
    	  if (!CreateTrackTrd(iTrack, &track, points)) continue;
      }          

      CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(points[0]->GetTrackID());
      CbmLitTrackParam par = *track.GetParamLast();
      std::vector<Double_t> res(5), pull(5);
      Double_t resp;
    
      if (fDetId == kTRD) {
    	  Double_t zOut = track.GetHit(0)->GetZ(); 
    	  fPropagatorDet->Propagate(&par, zOut, track.GetPDG());
      }
      
      for (Int_t i = 0; i < track.GetNofHits(); i++){
    
         Double_t zOut = track.GetHit(i)->GetZ(); 
         if (fPropagator->Propagate(&par, zOut, track.GetPDG()) == kLITERROR) continue;
         //fFilter->Update(&par, track.GetHit(i));
         
         fh_srx[i]->Fill(par.GetX() - track.GetHit(i)->GetX());
         fh_sry[i]->Fill(par.GetY() - track.GetHit(i)->GetY());
         
         
         CalcResAndPull(points[i], mcTrack, &par, res, pull, resp);
         
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
	  CalcResAndPull(points.back(), mcTrack, track.GetParamLast(), res, pull, resp);
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

void CbmLitPropAna::CalcResAndPull(
		const CbmMCPoint* point,
		const CbmMCTrack* mcTrack,
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
    if (pdg == 13) q = -1.; else 
    if (pdg == -13) q = 1.; else
    if (pdg > 0) q = 1.; else q = -1;
    
    Double_t qp = q / std::sqrt(px * px + py * py + pz * pz);
    
    res[0] = par->GetX() - x;
    res[1] = par->GetY() - y;
    res[2] = par->GetTx() - tx;
    res[3] = par->GetTy() - ty;
    res[4] = par->GetQp() - qp;     
    
    resp = 100 * (((1. / std::fabs(par->GetQp())) - (1. / std::fabs(qp))) / (1. / qp));                
     
    if (par->GetCovariance(0) != 0) pull[0] = (res[0]) / (std::sqrt(par->GetCovariance(0)));
    if (par->GetCovariance(5) != 0) pull[1] = (res[1]) / (std::sqrt(par->GetCovariance(5)));
    if (par->GetCovariance(9) != 0) pull[2] = (res[2]) / (std::sqrt(par->GetCovariance(9)));
    if (par->GetCovariance(12) != 0) pull[3] = (res[3]) / (std::sqrt(par->GetCovariance(12)));
    if (par->GetCovariance(14) != 0) pull[4] = (res[4]) / (std::sqrt(par->GetCovariance(14))); 
}

void CbmLitPropAna::FillParam(
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
    CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(mcId);
    Int_t pdg = mcTrack->GetPdgCode();
    Double_t q;
    if (pdg > 0) q = 1.; else q = -1;
    if (pdg == 13) q = -1.;
    if (pdg == -13) q = 1.;
    Double_t qp = q / std::sqrt(px * px + py * py + pz * pz);
    
    par->SetX(x);
    par->SetY(y);
    par->SetZ(z);
    par->SetTx(tx);
    par->SetTy(ty);
    par->SetQp(qp);
}

Bool_t CbmLitPropAna::CreateTrackMuch(
		Int_t trackId,
		CbmLitTrack* track,
		std::vector<CbmMCPoint*>& points)
{
    CbmMuchTrack* pTrack = (CbmMuchTrack*) fTracks->At(trackId);
    Int_t nofHits = pTrack->GetNHits();
	if (nofHits != fNofLayers) return false;

    points.clear();
    
    for (Int_t iHit = 0; iHit < nofHits; iHit++){
       Int_t hitIndex = pTrack->GetHitIndex(iHit);
       CbmMuchHit* pHit = (CbmMuchHit*) fHits->At(hitIndex);
       CbmLitHit hit;
       CbmLitConverter::TrkHitToLitHit(pHit, 0, &hit);
       track->AddHit(&hit);
       Int_t digiIndex = pHit->GetDigi();
       CbmMuchDigiMatch* pDigiMatch = (CbmMuchDigiMatch*) fDigiMatches->At(digiIndex);    
       Int_t pointIndex = pDigiMatch->GetRefIndex(0);
       if (pointIndex < 0) Fatal("MUCH Ana", "Wrong PointIndex");
       CbmMCPoint* pPoint = (CbmMCPoint*) fMCPoints->At(pointIndex);
       if (!pPoint) Fatal("MUCH Ana", "Wrong Point pointer");
       points.push_back(pPoint);
    }
    CbmLitTrackParam par;
    FillParam(points[0], &par);
    //CbmLitConverter::TrackParamToLitTrackParam(&(*pTrack->GetMuchTrack()), &par);
          
    track->SetParamFirst(&par);
    track->SetParamLast(&par);
    track->SetPDG(13);
    
    return true;
}

Bool_t CbmLitPropAna::CreateTrackTrd(
		Int_t trackId,
		CbmLitTrack* track,
		std::vector<CbmMCPoint*>& points)
{
    CbmTrdTrack* pTrack = (CbmTrdTrack*) fTracks->At(trackId);
    Int_t nofHits = pTrack->GetNofTrdHits();
	if (nofHits != fNofLayers) return false;

    points.clear();
    
    for (Int_t iHit = 0; iHit < nofHits; iHit++){
       Int_t hitIndex = pTrack->GetTrdHitIndex(iHit);
       CbmTrkHit* pHit = (CbmTrkHit*) fHits->At(hitIndex);
       CbmLitHit hit;
       CbmLitConverter::TrkHitToLitHit(pHit, 0, &hit);
       track->AddHit(&hit);
       //Int_t digiIndex = pHit->GetDigi();
       //CbmMuchDigiMatch* pDigiMatch = (CbmMuchDigiMatch*) fDigiMatches->At(digiIndex);  
       //Int_t pointIndex = pDigiMatch->GetRefIndex(0);
       Int_t pointIndex = pHit->GetRefIndex();       
       if (pointIndex < 0) Fatal("MUCH Ana", "Wrong PointIndex");
       CbmMCPoint* pPoint = (CbmMCPoint*) fMCPoints->At(pointIndex);
       if (!pPoint) Fatal("MUCH Ana", "Wrong Point pointer");
       points.push_back(pPoint);
    }
    CbmLitTrackParam par;
    //CbmLitConverter::TrackParamToLitTrackParam(&(*pTrack->GetMuchTrack()), &par);
    FillParam(points[0], &par);      
    track->SetParamFirst(&par);
    track->SetParamLast(&par);
    track->SetPDG(11);
    
    return true;
}

void CbmLitPropAna::CreateHistograms()
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
   
   std::stringstream histName;
   std::stringstream histTitle;
   
   Int_t nBins = 200;
   Double_t minSr = -10;
   Double_t maxSr = 10;
   Double_t minPull = -5.;
   Double_t maxPull = 5.;
   Double_t minPullQp = -7;
   Double_t maxPullQp = 7;
   Double_t minResX = -10.;
   Double_t maxResX = 10.;
   Double_t minResTx = -0.15;
   Double_t maxResTx = 0.15;
   Double_t minResQp = -0.1;
   Double_t maxResQp = 0.1;   
      
   for (Int_t i = 0; i < fNofLayers; i++) {
	  
	  histName.str("");
	  histTitle.str("");
	  histName << "h_srx" << i;
	  histTitle << "Search region for X at " << i << " station";
      fh_srx[i] = new TH1F(histName.str().c_str(), histTitle.str().c_str(), nBins, minSr, maxSr);
         
	  histName.str("");
	  histTitle.str("");
	  histName << "h_sry" << i;
	  histTitle << "Search region for Y at " << i << " station";
      fh_sry[i] = new TH1F(histName.str().c_str(), histTitle.str().c_str(), nBins, minSr, maxSr); 
      
	  histName.str("");
	  histTitle.str("");
      histName << "h_resx" << i;
      histTitle << "Resolution for X at " << i << " station";
      fh_resx[i] = new TH1F(histName.str().c_str(), histTitle.str().c_str(), nBins, minResX, maxResX);
        
	  histName.str("");
	  histTitle.str("");
      histName << "h_resy" << i;
      histTitle << "Resolution for Y at " << i << " station";
      fh_resy[i] = new TH1F(histName.str().c_str(), histTitle.str().c_str(), nBins, minResX, maxResX);
      
	  histName.str("");
	  histTitle.str("");
      histName << "h_restx" << i;
      histTitle << "Resolution for Tx at " << i << " station";
      fh_restx[i] = new TH1F(histName.str().c_str(), histTitle.str().c_str(), nBins, minResTx, maxResTx);
            
	  histName.str("");
	  histTitle.str("");
      histName << "h_resty" << i;
      histTitle << "Resolution for Ty at " << i << " station";
      fh_resty[i] = new TH1F(histName.str().c_str(), histTitle.str().c_str(), nBins, minResTx, maxResTx);
      
	  histName.str("");
	  histTitle.str("");
      histName << "h_resqp" << i;
      histTitle << "Resolution for Qp at " << i << " station";
      fh_resqp[i] = new TH1F(histName.str().c_str(), histTitle.str().c_str(), nBins, minResQp, maxResQp);      
      
	  histName.str("");
	  histTitle.str("");
      histName << "h_pullx" << i;
      histTitle << "Pull for X at " << i << " station";      
      fh_pullx[i] = new TH1F(histName.str().c_str(), histTitle.str().c_str(), nBins, minPull, maxPull);
      
	  histName.str("");
	  histTitle.str("");
      histName << "h_pully" << i;
      histTitle << "Pull for Y at " << i << " station";  
      fh_pully[i] = new TH1F(histName.str().c_str(), histTitle.str().c_str(), nBins, minPull, maxPull);
       
	  histName.str("");
	  histTitle.str("");
      histName << "h_pulltx" << i;
      histTitle << "Pull for Tx at " << i << " station";  
      fh_pulltx[i] = new TH1F(histName.str().c_str(), histTitle.str().c_str(), nBins, minPull, maxPull);
      
	  histName.str("");
	  histTitle.str("");
      histName << "h_pullty" << i;
      histTitle << "Pull for Ty at " << i << " station";  
      fh_pullty[i] = new TH1F(histName.str().c_str(), histTitle.str().c_str(), nBins, minPull, maxPull);
      
	  histName.str("");
	  histTitle.str("");
      histName << "h_pullqp" << i;
      histTitle << "Pull for Qp at " << i << " station";  
      fh_pullqp[i] = new TH1F(histName.str().c_str(), histTitle.str().c_str(), nBins, minPullQp, maxPullQp);
      
	  histName.str("");
	  histTitle.str("");
      histName << "h_resp" << i;
      histTitle << "Relative resolution for momentum at " << i << " station";  
      fh_resp[i] = new TH1F(histName.str().c_str(), histTitle.str().c_str(), nBins, -30., 30.);
   }
   
   fh_pullx_last = new TH1F("h_pullx_last", "pull x of the last track param", nBins, minPull, maxPull);
   fh_pullx_first = new TH1F("h_pullx_first", "pull x of the first track param", nBins, minPull, maxPull);
   fh_pully_last = new TH1F("h_pully_last", "pull y of the last track param", nBins, minPull, maxPull);
   fh_pully_first = new TH1F("h_pully_first", "pull y of the first track param", nBins, minPull, maxPull);
   fh_pulltx_last = new TH1F("h_pulltx_last", "pull tx of the last track param", nBins, minPull, maxPull);
   fh_pulltx_first = new TH1F("h_pulltx_first", "pull tx of the first track param", nBins, minPull, maxPull);
   fh_pullty_last = new TH1F("h_pullty_last", "pull ty of the last track param", nBins, minPull, maxPull);
   fh_pullty_first = new TH1F("h_pullty_first", "pull ty of the first track param", nBins, minPull, maxPull);
   fh_pullqp_last = new TH1F("h_pullqp_last", "pull qp of the last track param", nBins, minPullQp, maxPullQp);
   fh_pullqp_first = new TH1F("h_pullqp_first", "pull qp of the first track param", nBins, minPullQp, maxPullQp);
   
   fh_resx_last = new TH1F("h_resx_last", "res x of the last track param", nBins, minResX, maxResX);
   fh_resx_first = new TH1F("h_resx_first", "res x of the first track param", nBins, minResX, maxResX);
   fh_resy_last = new TH1F("h_resy_last", "res y of the last track param", nBins, minResX, maxResX);
   fh_resy_first = new TH1F("h_resy_first", "res y of the first track param", nBins, minResX, maxResX);
   fh_restx_last = new TH1F("h_restx_last", "res tx of the last track param", nBins, minResTx, maxResTx);
   fh_restx_first = new TH1F("h_restx_first", "res tx of the first track param", nBins, minResTx, maxResTx);
   fh_resty_last = new TH1F("h_resty_last", "res ty of the last track param", nBins, minResTx, maxResTx);
   fh_resty_first = new TH1F("h_resty_first", "res ty of the first track param", nBins, minResTx, maxResTx);
   fh_resqp_last = new TH1F("h_resqp_last", "res qp of the last track param", nBins, minResQp, maxResQp);
   fh_resqp_first = new TH1F("h_resqp_first", "res qp of the first track param", nBins, minResQp, maxResQp);
   
   fh_resp_last = new TH1F("h_resp_last", "res qp of the last track param", nBins, -15., 15.);
   fh_resp_first = new TH1F("h_resp_first", "res qp of the first track param", nBins, -15, 15.);
}

void CbmLitPropAna::Finish()
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

ClassImp(CbmLitPropAna)
