#include "CbmLitPropagationAnalysis.h"

#include "CbmLitTrackUpdate.h"
#include "CbmLitTrackPropagator.h"
#include "CbmLitTrackFitter.h"
#include "CbmLitTrackPropagatorGeane.h"
#include "CbmLitConverter.h"
#include "CbmLitEnvironment.h"
#include "CbmLitToolFactory.h"
#include "CbmLitMemoryManagment.h"
#include "CbmLitMath.h"
#include "CbmLitTrack.h"
#include "CbmLitHit.h"

#include "CbmTrkHit.h"
#include "CbmStsTrack.h"
#include "CbmTrdTrack.h"
#include "CbmMuchHit.h"
#include "CbmMuchTrack.h"
#include "CbmMuchDigi.h"
#include "CbmMuchDigiMatch.h"
#include "CbmMuchTrackMatch.h"
#include "CbmStsTrackMatch.h"
#include "CbmTrdTrackMatch.h"
#include "CbmDetectorList.h"
#include "FairMCPoint.h"
#include "CbmMCTrack.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "CbmGeoMuchPar.h"

#ifdef CbmMuchGeoScheme_H
#include "CbmMuchGeoScheme.h"
#endif

#include "TClonesArray.h"
#include "TH1F.h"

#include <iostream>
#include <iomanip>
#include <cmath>
#include <sstream>
#include <string>

CbmLitPropagationAnalysis::CbmLitPropagationAnalysis(
		DetectorId detId)
{
	fVerbose = 1;
	fEvents = 0;
	fDetId = detId;
}

CbmLitPropagationAnalysis::~CbmLitPropagationAnalysis()
{
}

InitStatus CbmLitPropagationAnalysis::Init()
{
	FairRootManager* ioman = FairRootManager::Instance();
	if (!ioman ) Fatal("Init", "No CbmRootManager");

	fMCTracks  = (TClonesArray*) ioman->GetObject("MCTrack");
	if (!fMCTracks) Fatal("Init", "No MCTrack array!");
	fStsTracks = (TClonesArray*) ioman->GetObject("STSTrack");
	if (!fStsTracks) Fatal("Init", "No STSTrack array!");
	fStsTrackMatches = (TClonesArray*) ioman->GetObject("STSTrackMatch");
	if (!fStsTrackMatches) Fatal("Init", "No STSTrackMatch array!");

	if (fDetId == kMUCH) {
	   fMCPoints  = (TClonesArray*) ioman->GetObject("MuchPoint");
	   if (!fMCPoints) Fatal("Init", "No MuchPoint array!");
	   fHits = (TClonesArray*) ioman->GetObject("MuchHit");
	   if (!fHits) Fatal("Init", "No MuchHit array!");
	   fDigiMatches  = (TClonesArray*) ioman->GetObject("MuchDigiMatch");
	   if (!fDigiMatches) Fatal("Init", "No MuchDigiMatches array!");
	   fTracks = (TClonesArray*) ioman->GetObject("MuchTrack");
	   if (!fTracks) Fatal("Init", "No MuchTrack array!");
	   fTrackMatches = (TClonesArray*) ioman->GetObject("MuchTrackMatch");
	   if (!fTrackMatches) Fatal("Init", "No MuchTrackMatch array!");
	} else
	if (fDetId == kTRD) {
	   fMCPoints  = (TClonesArray*) ioman->GetObject("TRDPoint");
	   if (!fMCPoints) Fatal("Init", "No TRDPoint array!");
	   fHits = (TClonesArray*) ioman->GetObject("TRDHit");
	   if (!fHits) Fatal("Init", "No TRDHit array!");
	   fTracks = (TClonesArray*) ioman->GetObject("TRDTrack");
	   if (!fTracks) Fatal("Init", "No TRDTrack array!");
	   fTrackMatches = (TClonesArray*) ioman->GetObject("TRDTrackMatch");
	   if (!fTrackMatches) Fatal("Init", "No TRDTrackMatch array!");
	} else {
	   Fatal("Init", "CbmLitPropagationAnalysis works only with MUCH and TRD!");
	}

#ifdef CbmMuchGeoScheme_H
	CbmMuchGeoScheme* geoScheme = CbmMuchGeoScheme::Instance();
	CbmRuntimeDb* db = CbmRuntimeDb::instance();
	CbmGeoMuchPar* geoPar = (CbmGeoMuchPar*) db->getContainer("CbmGeoMuchPar");
	TObjArray* stations = (TObjArray*) geoPar->GetStations();
	geoScheme->Init(stations);
#endif

	// Create tools
	CbmLitToolFactory* factory = CbmLitToolFactory::Instance();
	fPropagator = factory->CreateTrackPropagator("lit");
	fFilter = factory->CreateTrackUpdate("kalman");
	fFitter = factory->CreateTrackFitter("lit_kalman");
	fSmoother = factory->CreateTrackFitter("kalman_smoother");

	// Get the layout
	if (fDetId == kMUCH) fLayout = CbmLitEnvironment::Instance()->GetMuchLayout();
	if (fDetId == kTRD) fLayout = CbmLitEnvironment::Instance()->GetTrdLayout();
	fNofPlanes = fLayout.GetNofPlanes();
	fNofPlanes = 13;
	fNofHistos = 12;

	CreateHistograms();
}

void CbmLitPropagationAnalysis::SetParContainers()
{
}

void CbmLitPropagationAnalysis::Exec(
		Option_t* opt)
{
	CreateTrackArrays();
	RunTest();
	DeleteTrackArrays();

	std::cout << "Event: " << fEvents++ << std::endl;
}

void CbmLitPropagationAnalysis::RunTest()
{
	for(Int_t i = 0; i < fLitTracks.size(); i++){
		if (fLitTracks[i]->GetNofHits() != fNofPlanes) continue;
		TestPropagation(fLitTracks[i], fLitMcTracks[i]);
		TestFitter(fLitTracks[i], fLitMcTracks[i]);
	}
}

void CbmLitPropagationAnalysis::TestPropagation(
		CbmLitTrack* track,
		CbmLitTrack* mcTrack)
{
//	std::cout << "------------------------------------" << std::endl;
	CbmLitTrackParam par(*track->GetParamLast());
	for (Int_t i = 0; i < track->GetNofHits(); i++){
//		 std::cout << "before " << par.ToString();
		Double_t zOut = track->GetHit(i)->GetZ();
         if (fPropagator->Propagate(&par, zOut, track->GetPDG()) == kLITERROR) continue;
//         if (fFilter->Update(&par, track->GetHit(i)) == kLITERROR) continue;
         FillHistosPropagation(&par, mcTrack->GetFitNode(i)->GetPredictedParam(), track->GetHit(i), i);
//         if (fFilter->Update(&par, track->GetHit(i)) == kLITERROR) continue;
//         std::cout << "after " << par.ToString();
	}
}

void CbmLitPropagationAnalysis::TestFitter(
		CbmLitTrack* track,
		CbmLitTrack* mcTrack)
{
	LitStatus fitStatus, smoothStatus;
	fitStatus = fFitter->Fit(track);
	if (fitStatus == kLITSUCCESS) smoothStatus = fSmoother->Fit(track);
	if (fitStatus == kLITSUCCESS && smoothStatus == kLITSUCCESS)
		FillHistosFitter(track, mcTrack);
}

void CbmLitPropagationAnalysis::FillHistosPropagation(
		const CbmLitTrackParam* par,
		const CbmLitTrackParam* mcPar,
		const CbmLitHit* hit,
		Int_t plane)
{
	 std::vector<Double_t> r = CalcResidualsAndPulls(par, mcPar);
	 for (Int_t i = 0; i < 11; i++){
		 fHistos[plane][i]->Fill(r[i]);
	 }
	 fHistos[plane][11]->Fill(ChiSq(par, hit));
}

void CbmLitPropagationAnalysis::FillHistosFitter(
		const CbmLitTrack* track,
		const CbmLitTrack* mcTrack)
{
	std::vector<Double_t> rLast = CalcResidualsAndPulls(track->GetParamLast(), mcTrack->GetParamLast());
	std::vector<Double_t> rFirst = CalcResidualsAndPulls(track->GetParamFirst(), mcTrack->GetParamFirst());
	for (Int_t i = 0; i < 11; i++){
		fFitHistosFirst[i]->Fill(rFirst[i]);
		fFitHistosLast[i]->Fill(rLast[i]);
	}
}

void CbmLitPropagationAnalysis::CreateTrackArrays()
{
	Int_t nofTracks = fTracks->GetEntriesFast();
	for (Int_t iTrack = 0; iTrack < nofTracks; iTrack++) {
		if (fDetId == kMUCH) {
			CbmMuchTrack* muchTrack = (CbmMuchTrack*) fTracks->At(iTrack);
			if (muchTrack->GetNHits() == 0) continue;
			CbmLitTrack* litTrack = new CbmLitTrack();
			CbmLitConverter::MuchTrackToLitTrack(muchTrack, litTrack, fHits);
			if (!InitTrackParamFromStsTrackParam(iTrack, litTrack)) continue;
			fLitTracks.push_back(litTrack);
			CbmLitTrack* mcLitTrack = new CbmLitTrack();
			McMuchTrackToLitTrack(muchTrack, mcLitTrack);
			fLitMcTracks.push_back(mcLitTrack);
		}
		if (fDetId == kTRD) {
			CbmTrdTrack* trdTrack = (CbmTrdTrack*) fTracks->At(iTrack);
			if (trdTrack->GetNofTrdHits() == 0) continue;
			CbmLitTrack* litTrack = new CbmLitTrack();
			CbmLitConverter::TrdTrackToLitTrack(trdTrack, litTrack, fHits);
			if (!InitTrackParamFromStsTrackParam(iTrack, litTrack)) continue;
			fLitTracks.push_back(litTrack);
			CbmLitTrack* mcLitTrack = new CbmLitTrack();
			McTrdTrackToLitTrack(trdTrack, mcLitTrack);
			fLitMcTracks.push_back(mcLitTrack);
		}
	}
}

void CbmLitPropagationAnalysis::DeleteTrackArrays()
{
	for_each(fLitTracks.begin(), fLitTracks.end(), DeleteObject());
	for_each(fLitMcTracks.begin(), fLitMcTracks.end(), DeleteObject());
	fLitTracks.clear();
	fLitMcTracks.clear();
}

void CbmLitPropagationAnalysis::McMuchTrackToLitTrack(
		CbmMuchTrack* muchTrack,
		CbmLitTrack* track)
{
	Int_t nofHits = muchTrack->GetNHits();
    FitNodeVector nodes(nofHits);
	for (Int_t i = 0; i < nofHits; i++){
		Int_t hitIndex = muchTrack->GetHitIndex(i);
		CbmMuchHit* hit = (CbmMuchHit*) fHits->At(hitIndex);
		Int_t digiIndex = hit->GetDigi();
		CbmMuchDigiMatch* digiMatch = (CbmMuchDigiMatch*) fDigiMatches->At(digiIndex);
		Int_t pointIndex = digiMatch->GetRefIndex(0);
		if (pointIndex < 0) Fatal("CbmLitPropagationAnalysis", "Wrong point index");
		FairMCPoint* point = (FairMCPoint*) fMCPoints->At(pointIndex);
		if (point == NULL) Fatal("CbmLitPropagationAnalysis", "NULL pointer");
		McPointToLitFitNode(point, &nodes[i]);
	}
    track->SetFitNodes(nodes);
    track->SetParamFirst(nodes.front().GetPredictedParam());
    track->SetParamLast(nodes.back().GetPredictedParam());
}

void CbmLitPropagationAnalysis::McTrdTrackToLitTrack(
		const CbmTrdTrack* trdTrack,
		CbmLitTrack* track)
{
    Int_t nofHits = trdTrack->GetNofTrdHits();
    FitNodeVector nodes(nofHits);
    for (Int_t i = 0; i < nofHits; i++){
		Int_t hitIndex = trdTrack->GetTrdHitIndex(i);
		CbmTrkHit* hit = (CbmTrkHit*) fHits->At(hitIndex);
		Int_t pointIndex = hit->GetRefIndex();
		if (pointIndex < 0) Fatal("CbmLitPropagationAnalysis", "Wrong point index");
		FairMCPoint* point = (FairMCPoint*) fMCPoints->At(pointIndex);
		McPointToLitFitNode(point, &nodes[i]);
    }
    track->SetFitNodes(nodes);
}

void CbmLitPropagationAnalysis::McPointToLitFitNode(
		FairMCPoint* point,
		CbmLitFitNode* node)
{
	Int_t mcId = point->GetTrackID();
    CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(mcId);
    Int_t pdg = mcTrack->GetPdgCode();
    Double_t q;
    if (pdg > 0) q = 1.; else q = -1;
    if (pdg == 13) q = -1.;
    if (pdg == -13) q = 1.;
    TVector3 mom;
    point->Momentum(mom);
    Double_t qp = q / mom.Mag();

    CbmLitTrackParam par;
    par.SetX(point->GetX());
    par.SetY(point->GetY());
    par.SetZ(point->GetZ());
    par.SetTx(point->GetPx()/point->GetPz());
    par.SetTy(point->GetPy()/point->GetPz());
    par.SetQp(qp);

    node->SetPredictedParam(&par);
}

Bool_t CbmLitPropagationAnalysis::InitTrackParamFromStsTrackParam(
		Int_t trackIndex,
		CbmLitTrack* track)
{
	Int_t mcId;
	if (fDetId = kMUCH) {
		CbmMuchTrackMatch* trackMatch = (CbmMuchTrackMatch*) fTrackMatches->At(trackIndex);
		mcId = trackMatch->GetMCTrackId();
	} else
	if (fDetId = kTRD) {
		CbmTrdTrackMatch* trackMatch = (CbmTrdTrackMatch*) fTrackMatches->At(trackIndex);
		mcId = trackMatch->GetMCTrackID();
	}
	CbmStsTrack* stsTrack = FindStsTrackByMc(mcId);
	if (stsTrack == NULL) return false;
	CbmLitTrackParam par;
	CbmLitConverter::TrackParamToLitTrackParam(stsTrack->GetParamLast(), &par);
	track->SetParamLast(&par);
	track->SetParamFirst(&par);
	return true;
}

CbmStsTrack* CbmLitPropagationAnalysis::FindStsTrackByMc(
		Int_t mcId)
{
	Int_t nofStsTracks = fStsTrackMatches->GetEntriesFast();
	for (Int_t i = 0; i < nofStsTracks; i++){
		CbmStsTrackMatch* trackMatch = (CbmStsTrackMatch*) fStsTrackMatches->At(i);
		Int_t stsMcId = trackMatch->GetMCTrackId();
		if (mcId == stsMcId) return (CbmStsTrack*)fStsTracks->At(i);
	}
	return NULL;
}

std::vector<Double_t> CbmLitPropagationAnalysis::CalcResidualsAndPulls(
		const CbmLitTrackParam* par,
		const CbmLitTrackParam* mcPar)
{
    std::vector<Double_t> r(11);
    r[0] = par->GetX() - mcPar->GetX();
    r[1] = par->GetY() - mcPar->GetY();
    r[2] = par->GetTx() - mcPar->GetTx();
    r[3] = par->GetTy() - mcPar->GetTy();
    r[4] = par->GetQp() - mcPar->GetQp();
    if (par->GetCovariance(0) > 0.) r[5] = (r[0]) / (std::sqrt(par->GetCovariance(0)));
    if (par->GetCovariance(5) > 0.) r[6] = (r[1]) / (std::sqrt(par->GetCovariance(5)));
    if (par->GetCovariance(9) > 0.) r[7] = (r[2]) / (std::sqrt(par->GetCovariance(9)));
    if (par->GetCovariance(12) > 0.) r[8] = (r[3]) / (std::sqrt(par->GetCovariance(12)));
    if (par->GetCovariance(14) > 0.) r[9] = (r[4]) / (std::sqrt(par->GetCovariance(14)));
    r[10] = 100 * ((1./par->GetQp() - 1./mcPar->GetQp()) / (std::abs(1./mcPar->GetQp())));
    return r;
}

void CbmLitPropagationAnalysis::CreateHistograms()
{
	//0,1,2,3,4 - residuals (x,y,tx,ty,q/p)
	//5,6,7,8,9 - pulls (x,y,tx,ty,q/p)
	//11 - relative momentum error
	//12 - chi-square of the track param and hit

	fHistos.resize(fNofPlanes);
	for(Int_t i = 0; i < fNofPlanes; i++)
		fHistos[i].resize(fNofHistos);
	fFitHistosFirst.resize(fNofHistos);
	fFitHistosLast.resize(fNofHistos);

	std::stringstream histName;
	std::stringstream histTitle;
	std::string names[] = { "h_resx", "h_resy", "h_restx", "h_resty", "h_resqp",
			           "h_pullx", "h_pully", "h_pulltx", "h_pullty", "h_pullqp",
			           "h_resp", "h_chisq" };
	std::string titles[] = { "resolution X at ", "resolution Y at ", "resolution Tx at ",
			            "resolution Ty at ", "resolution Q/p at ", "pull X at ",
			            "pull Y at ", "pull Tx at ", "pull Ty at ", "pull Q/p at ",
			            "relative momentum resolution at ", "chi-square at" };
	Int_t bins[] = {200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200};
	Double_t boundL[] = {-10., -10., -.15, -.15, -.1, -5., -5., -5., -5., -7., -30., -10.};
	Double_t boundR[] = { 10.,  10.,  .15,  .15,  .1,  5.,  5.,  5.,  5.,  7.,  30.,  10.};

	for (Int_t i = 0; i < fNofPlanes; i++) {
		for (Int_t j = 0; j < fNofHistos; j++) {
			std::stringstream histName;
			std::stringstream histTitle;
			histName << names[j] << i;
			histTitle << titles[j] << i << " station";
			std::cout << histName.str() << " | " << histTitle.str() << std::endl;
			fHistos[i][j] = new TH1F(histName.str().c_str(), histTitle.str().c_str(),
					bins[j], boundL[j], boundR[j]);
		}
	}

	for (Int_t j = 0; j < fNofHistos; j++) {
		std::stringstream histNameFirst, histNameLast;
		std::stringstream histTitleFirst, histTitleLast;
		histNameFirst << names[j] << "_first";
		histNameLast << names[j] << "_last";
		histTitleFirst << titles[j] << " the first station";
		histTitleLast << titles[j] << " the last station";
		fFitHistosFirst[j] = new TH1F(histNameFirst.str().c_str(), histTitleFirst.str().c_str(),
				bins[j], boundL[j], boundR[j]);
		fFitHistosLast[j] = new TH1F(histNameLast.str().c_str(), histTitleLast.str().c_str(),
				bins[j], boundL[j], boundR[j]);
	}
}

void CbmLitPropagationAnalysis::Finish()
{
	for(Int_t i = 0; i < fNofPlanes; i++) {
		for(Int_t j = 0; j < fNofHistos; j++) {
			fHistos[i][j]->Write();
		}
	}
	for(Int_t j = 0; j < fNofHistos; j++) {
		fFitHistosFirst[j]->Write();
		fFitHistosLast[j]->Write();
	}
}

ClassImp(CbmLitPropagationAnalysis)
