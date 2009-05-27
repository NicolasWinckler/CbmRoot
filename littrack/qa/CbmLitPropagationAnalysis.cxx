#include "CbmLitPropagationAnalysis.h"

#include "CbmLitTrackUpdate.h"
#include "CbmLitTrackPropagator.h"
#include "CbmLitTrackFitter.h"
#include "CbmLitConverter.h"
#include "CbmLitEnvironment.h"
#include "CbmLitToolFactory.h"
#include "CbmLitMemoryManagment.h"
#include "CbmLitMath.h"
#include "CbmLitTrack.h"
#include "CbmLitHit.h"
#include "CbmLitPixelHit.h"
#include "CbmLitStripHit.h"
#include "CbmLitSimpleGeometryConstructor.h"

#include "CbmHit.h"
#include "CbmBaseHit.h"
#include "CbmPixelHit.h"
#include "CbmStripHit.h"
#include "CbmGlobalTrack.h"
#include "CbmStsTrack.h"
#include "CbmTrdTrack.h"
#include "CbmMuchTrack.h"
#include "CbmMuchDigi.h"
#include "CbmMuchDigiMatch.h"
#include "CbmTrackMatch.h"
#include "FairMCPoint.h"
#include "CbmMCTrack.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "CbmGeoMuchPar.h"

#ifdef NEWMUCH
#include "CbmMuchGeoScheme.h"
#endif

#include "TClonesArray.h"
#include "TH1F.h"

#include <iostream>
#include <iomanip>
#include <cmath>
#include <sstream>
#include <string>

CbmLitPropagationAnalysis::CbmLitPropagationAnalysis()
{
	fVerbose = 1;
	fEvents = 0;
	fNofParams = 12;
	fNofPlanes = 14;
	fNofTrdHits = 0;
	fNofMuchHits = 13;
	fNofTofHits = 1;
}

CbmLitPropagationAnalysis::~CbmLitPropagationAnalysis()
{
}

InitStatus CbmLitPropagationAnalysis::Init()
{
	DetermineSetup();
	ReadDataBranches();

//	fGeoConstructor.ConstructGeometry();
//	fGeoConstructor.Draw();

	// Create tools
	CbmLitToolFactory* factory = CbmLitToolFactory::Instance();
	fPropagator = factory->CreateTrackPropagator("lit");
	fFilter = factory->CreateTrackUpdate("kalman");
	fFitter = factory->CreateTrackFitter("lit_kalman");
	fSmoother = factory->CreateTrackFitter("kalman_smoother");

	CreateHistograms();
}

void CbmLitPropagationAnalysis::SetParContainers()
{
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb = ana->GetRuntimeDb();
    rtdb->getContainer("CbmGeoMuchPar");
}

void CbmLitPropagationAnalysis::Exec(
		Option_t* opt)
{
	CreateTrackArrays();
	RunTest();
	DeleteTrackArrays();

	std::cout << "Event: " << fEvents++ << std::endl;
}

void CbmLitPropagationAnalysis::Finish()
{
	for(Int_t i = 0; i < fNofPlanes; i++) {
		for(Int_t j = 0; j < fNofParams; j++) {
			fPropagationHistos[i][j]->Write();
			fFilterHistos[i][j]->Write();
			fSmootherHistos[i][j]->Write();
		}
	}

	PrintStopwatchStatistics();
}

void CbmLitPropagationAnalysis::DetermineSetup()
{
    CbmLitEnvironment* env = CbmLitEnvironment::Instance();
    fIsElectronSetup = env->IsElectronSetup();
	fIsSts = true;
    fIsTrd = env->IsTrd();
    fIsMuch = env->IsMuch();
    fIsTof = env->IsTof();

    if (fIsElectronSetup) std::cout << "-I- CBM electron setup detected" << std::endl;
    else std::cout << "-I- CBM muon setup detected" << std::endl;
    std::cout << "-I- The following detectors were found in the CBM setup:" << std::endl;
    if (fIsTrd) std::cout << "TRD" << std::endl;
    if (fIsMuch) std::cout << "MUCH" << std::endl;
    if (fIsTof) std::cout << "TOF" << std::endl;
}

void CbmLitPropagationAnalysis::ReadDataBranches()
{
	FairRootManager* ioman = FairRootManager::Instance();
    if (NULL == ioman) Fatal("Init","CbmRootManager is not instantiated");

    fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
    if (NULL == fMCTracks) Fatal("Init","No MCTrack array!");

    fGlobalTracks = (TClonesArray*) ioman->GetObject("GlobalTrack");
    if (NULL == fGlobalTracks) Fatal("Init","No GlobalTrack array!");

   	if (fIsSts) {
   		fStsTracks = (TClonesArray*) ioman->GetObject("STSTrack");
   		if (NULL == fStsTracks) Fatal("Init", "No STSTrack array!");
//   		fStsTrackMatches = (TClonesArray*) ioman->GetObject("STSTrackMatch");
//   		if (NULL == fStsTrackMatches) Fatal("Init", "No STSTrackMatch array!");
   	}

   	if (fIsMuch) {
		fMuchTracks = (TClonesArray*) ioman->GetObject("MuchTrack");
		if (NULL == fMuchTracks) Fatal("Init", "No MuchTrack array!");
		fMuchPixelHits = (TClonesArray*) ioman->GetObject("MuchPixelHit");
		fMuchStrawHits = (TClonesArray*) ioman->GetObject("MuchStrawHit");
		if (NULL == fMuchPixelHits && NULL == fMuchStrawHits) Fatal("Init", "No MuchPixelHit AND/OR MuchStrawHit arrays!");
		fMuchTrackMatches = (TClonesArray*) ioman->GetObject("MuchTrackMatch");
		if (NULL == fMuchTrackMatches) Fatal("Init", "No MuchTrackMatch array!");
		fMuchDigiMatches = (TClonesArray*) ioman->GetObject("MuchDigiMatch");
		if (NULL == fMuchDigiMatches) Fatal("Init", "No MuchDigiMatches array!");
		fMuchPoints  = (TClonesArray*) ioman->GetObject("MuchPoint");
		if (NULL == fMuchPoints) Fatal("Init", "No MuchPoint array!");
   	}

   	if (fIsTrd) {
		fTrdTracks = (TClonesArray*) ioman->GetObject("TRDTrack");
		if (NULL == fTrdTracks) Fatal("Init", "No TRDTrack array!");
		fTrdHits  = (TClonesArray*) ioman->GetObject("TRDHit");
		if (NULL == fTrdHits) Fatal("Init", "No TRDHit array!");
		fTrdTrackMatches = (TClonesArray*) ioman->GetObject("TRDTrackMatch");
		if (!fTrdTrackMatches) Fatal("Init", "No TRDTrackMatch array!");
		fTrdPoints  = (TClonesArray*) ioman->GetObject("TRDPoint");
		if (NULL == fTrdPoints) Fatal("Init", "No TRDPoint array!");
   	}

   	if (fIsTof) {
		fTofPoints = (TClonesArray*) ioman->GetObject("TOFPoint");
		if (NULL == fTofPoints) Fatal("Init", "No TofPoint array!");
		fTofHits = (TClonesArray*) ioman->GetObject("TofHit");
		if (NULL == fTofHits) Fatal("Init", "No TofHit array!");
   	}
}

void CbmLitPropagationAnalysis::CreateHistograms()
{
	//0,1,2,3,4 - residuals (x,y,tx,ty,q/p)
	//5,6,7,8,9 - pulls (x,y,tx,ty,q/p)
	//11 - relative momentum error
	//12 - chi-square of the track param and hit

	fPropagationHistos.resize(fNofPlanes);
	fFilterHistos.resize(fNofPlanes);
	fSmootherHistos.resize(fNofPlanes);
	for(Int_t i = 0; i < fNofPlanes; i++) {
		fPropagationHistos[i].resize(fNofParams);
		fFilterHistos[i].resize(fNofParams);
		fSmootherHistos[i].resize(fNofParams);
	}

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
	Double_t boundL[] = {-10., -10., -.15, -.15, -.1, -5., -5., -5., -5., -7., -30., -1.};
	Double_t boundR[] = { 10.,  10.,  .15,  .15,  .1,  5.,  5.,  5.,  5.,  7.,  30.,  15.};
	std::string var[] = {"p", "f", "s"};
	std::string var2[] = {"[propagation]", "[filter]", "[smoother]"};

	for (Int_t v = 0; v < 3; v++) {
		for (Int_t i = 0; i < fNofPlanes; i++) {
			for (Int_t j = 0; j < fNofParams; j++) {
				std::stringstream histName;
				std::stringstream histTitle;
				histName << names[j] << "_" << var[v] << "_" << i;
				histTitle << var2[v] << " " << titles[j] << i << " station";
				TH1F* hist = new TH1F(histName.str().c_str(), histTitle.str().c_str(),
						bins[j], boundL[j], boundR[j]);
				if (v == 0) fPropagationHistos[i][j] = hist;
				if (v == 1) fFilterHistos[i][j] = hist;
				if (v == 2) fSmootherHistos[i][j] = hist;
//				std::cout << histName.str() << std::endl << histTitle.str() << std::endl;
			}
		}
	}
}

void CbmLitPropagationAnalysis::CreateTrackArrays()
{
	Int_t nofGlobalTracks = fGlobalTracks->GetEntriesFast();
	for (Int_t iTrack = 0; iTrack < nofGlobalTracks; iTrack++) {
		CbmGlobalTrack* globalTrack = (CbmGlobalTrack*) fGlobalTracks->At(iTrack);
		CbmLitTrack* litTrack = new CbmLitTrack();
		CbmLitTrack* mcLitTrack = new CbmLitTrack();

		CheckAcceptance(globalTrack);

		GlobalTrackToLitTrack(globalTrack, litTrack);
		if (fIsElectronSetup) litTrack->SetPDG(11); else litTrack->SetPDG(13);

		GlobalTrackToMCLitTrack(globalTrack, mcLitTrack);

		fLitTracks.push_back(litTrack);
		fLitMcTracks.push_back(mcLitTrack);
	}

	if (fLitTracks.size() != fLitMcTracks.size())
		Fatal("CbmLitpropagationAnalysis", "LitTrack vector size != LitMcVector size");
}

Bool_t CbmLitPropagationAnalysis::CheckAcceptance(
		const CbmGlobalTrack* globalTrack)
{
	Int_t trdId = globalTrack->GetTrdTrackIndex();
	Int_t muchId = globalTrack->GetRichRingIndex(); //TODO change to MUCH
	Int_t tofId = globalTrack->GetTofHitIndex();
	if (fIsTrd && trdId > -1) {
		CbmTrdTrack* trdTrack = (CbmTrdTrack*) fTrdTracks->At(trdId);
//		std::cout << "CHECK ACC: trd hits " << trdTrack->GetNofTrdHits() <<  std::endl;
		if (trdTrack->GetNofTrdHits() != fNofTrdHits) return false;
	}
	if (fIsMuch && muchId > -1) {
		CbmMuchTrack* muchTrack = (CbmMuchTrack*) fMuchTracks->At(muchId);
//		std::cout << "CHECK ACC: much hits " << muchTrack->GetNHits() <<  std::endl;
		if (muchTrack->GetNHits() != fNofMuchHits) return false;
	}

	if (tofId > -1) return true;
	return true;
}

void CbmLitPropagationAnalysis::GlobalTrackToLitTrack(
		const CbmGlobalTrack* globalTrack,
		CbmLitTrack* litTrack)
{
	Int_t stsId = globalTrack->GetStsTrackIndex();
	Int_t trdId = globalTrack->GetTrdTrackIndex();
	Int_t muchId = globalTrack->GetRichRingIndex(); //TODO change to MUCH
	Int_t tofId = globalTrack->GetTofHitIndex();

	// STS: set initial track parameters from STS track
	if (stsId > -1) {
		CbmStsTrack* stsTrack = (CbmStsTrack*) fStsTracks->At(stsId);
		CbmLitTrackParam par;
		CbmLitConverter::TrackParamToLitTrackParam((const_cast<CbmStsTrack*> (stsTrack))->GetParamLast(), &par);
		litTrack->SetParamLast(&par);
		litTrack->SetParamFirst(&par);
	}
	//MUCH: attach hits from MUCH track
	if (muchId > -1) {
		CbmMuchTrack* muchTrack = (CbmMuchTrack*) fMuchTracks->At(muchId);
		for (int iHit = 0; iHit < muchTrack->GetNHits(); iHit++) {
			Int_t index = muchTrack->GetHitIndex(iHit);
			//TODO: add straw hits here!!!!!
			CbmPixelHit* hit = (CbmPixelHit*) fMuchPixelHits->At(index);
//			if (hit->GetTime(2) == -77777) {
//				CbmLitStripHit litHit;
//				CbmLitConverter::MuchHitToLitStripHit(hit, index, &litHit);
//				litTrack->AddHit(&litHit);
//			} else {
				CbmLitPixelHit litHit;
				CbmLitConverter::PixelHitToLitPixelHit(hit, index, &litHit);
				litTrack->AddHit(&litHit);
//			}
		}
	}
	//TRD: attach hits from TRD track
	if (trdId > -1) {
		CbmTrdTrack* trdTrack = (CbmTrdTrack*) fTrdTracks->At(trdId);
		for (int iHit = 0; iHit < trdTrack->GetNofTrdHits(); iHit++) {
			Int_t index = trdTrack->GetTrdHitIndex(iHit);
			CbmPixelHit* hit = (CbmPixelHit*) fTrdHits->At(index);
			CbmLitPixelHit litHit;
			CbmLitConverter::PixelHitToLitPixelHit(hit, index, &litHit);
			litTrack->AddHit(&litHit);
		}
	}
	//TOF: attach TOF hit
	if (tofId > -1) {
		CbmPixelHit* tofHit = (CbmPixelHit*) fTofHits->At(tofId);
		CbmLitPixelHit litHit;
		CbmLitConverter::PixelHitToLitPixelHit(tofHit, tofId, &litHit);
		litTrack->AddHit(&litHit);
	}
}

void CbmLitPropagationAnalysis::GlobalTrackToMCLitTrack(
		const CbmGlobalTrack* globalTrack,
		CbmLitTrack* litTrack)
{
	Int_t trdId = globalTrack->GetTrdTrackIndex();
	Int_t muchId = globalTrack->GetRichRingIndex(); //TODO change to MUCH
	Int_t tofId = globalTrack->GetTofHitIndex();

	CbmTrdTrack* trdTrack;
	CbmMuchTrack* muchTrack;
	Int_t nofTrdHits = 0, nofMuchHits = 0, nofTofHits = 0;

	if (muchId > -1){
		muchTrack = (CbmMuchTrack*) fMuchTracks->At(muchId);
		nofMuchHits = muchTrack->GetNHits();
	}
	if (trdId > -1) {
		trdTrack = (CbmTrdTrack*) fTrdTracks->At(trdId);
		nofTrdHits = trdTrack->GetNofTrdHits();
	}
	if (tofId > -1) nofTofHits = 1;

	FitNodeVector nodes(nofTrdHits + nofMuchHits + nofTofHits);
	Int_t counter = 0;

	//MUCH
	if (muchId > -1) {
		for (Int_t i = 0; i < nofMuchHits; i++){
			Int_t hitIndex = muchTrack->GetHitIndex(i);
			//TODO: add strip hit
			CbmPixelHit* hit = (CbmPixelHit*) fMuchPixelHits->At(hitIndex);
			Int_t digiIndex = hit->GetRefId();
			CbmMuchDigiMatch* digiMatch = (CbmMuchDigiMatch*) fMuchDigiMatches->At(digiIndex);
			Int_t pointIndex = digiMatch->GetRefIndex(0);
			if (pointIndex < 0) Fatal("CbmLitPropagationAnalysis", "Wrong point index");
			FairMCPoint* point = (FairMCPoint*) fMuchPoints->At(pointIndex);
			if (point == NULL) Fatal("CbmLitPropagationAnalysis", "NULL pointer");
			McPointToLitFitNode(point, &nodes[counter++]);
		}
	}

	//TRD
	if (trdId > -1) {
	    for (Int_t i = 0; i < nofTrdHits; i++){
			Int_t hitIndex = trdTrack->GetTrdHitIndex(i);
			CbmHit* hit = (CbmHit*) fTrdHits->At(hitIndex);
			Int_t pointIndex = hit->GetRefIndex();
			if (pointIndex < 0) Fatal("CbmLitPropagationAnalysis", "Wrong point index");
			FairMCPoint* point = (FairMCPoint*) fTrdPoints->At(pointIndex);
			McPointToLitFitNode(point, &nodes[counter++]);
	    }
	}

	//TOF
	if (tofId > -1) {
		CbmHit* hit = (CbmHit*) fTofHits->At(tofId);
		Int_t pointIndex = hit->GetRefIndex();
		if (pointIndex < 0) Fatal("CbmLitPropagationAnalysis", "Wrong point index");
		FairMCPoint* point = (FairMCPoint*) fTofPoints->At(pointIndex);
		McPointToLitFitNode(point, &nodes[counter++]);
	}

	litTrack->SetFitNodes(nodes);
}

void CbmLitPropagationAnalysis::DeleteTrackArrays()
{
	for_each(fLitTracks.begin(), fLitTracks.end(), DeleteObject());
	for_each(fLitMcTracks.begin(), fLitMcTracks.end(), DeleteObject());
	fLitTracks.clear();
	fLitMcTracks.clear();
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
	CbmLitTrackParam par(*track->GetParamLast());
	fPropagationWatch.Start(kFALSE);
	for (Int_t i = 0; i < track->GetNofHits(); i++){
		Double_t zOut = track->GetHit(i)->GetZ();
         if (fPropagator->Propagate(&par, zOut, track->GetPDG()) == kLITERROR) continue;
//         if (fFilter->Update(&par, track->GetHit(i)) == kLITERROR) continue;
         FillHistosPropagation(&par, mcTrack->GetFitNode(i)->GetPredictedParam(), track->GetHit(i), i);
//         if (fFilter->Update(&par, track->GetHit(i)) == kLITERROR) continue;
	}
	fPropagationWatch.Stop();
}

void CbmLitPropagationAnalysis::TestFitter(
		CbmLitTrack* track,
		CbmLitTrack* mcTrack)
{
	LitStatus fitStatus, smoothStatus;
	fFitterWatch.Start(kFALSE);
	fitStatus = fFitter->Fit(track);
	fFitterWatch.Stop();
	fSmootherWatch.Start(kFALSE);
	if (fitStatus == kLITSUCCESS) smoothStatus = fSmoother->Fit(track);
	fSmootherWatch.Stop();
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
		 fPropagationHistos[plane][i]->Fill(r[i]);
	 }
	 fPropagationHistos[plane][11]->Fill(ChiSq(par, hit));
}

void CbmLitPropagationAnalysis::FillHistosFitter(
		const CbmLitTrack* track,
		const CbmLitTrack* mcTrack)
{
	Int_t nofHits = track->GetNofHits();
	for (Int_t i = 0; i < nofHits; i++){
		const CbmLitFitNode* node = track->GetFitNode(i);
		std::vector<Double_t> rFilter = CalcResidualsAndPulls(node->GetUpdatedParam(), mcTrack->GetFitNode(i)->GetPredictedParam());
		std::vector<Double_t> rSmoother = CalcResidualsAndPulls(node->GetSmoothedParam(), mcTrack->GetFitNode(i)->GetPredictedParam());
		for (Int_t j = 0; j < 11; j++){
			fFilterHistos[i][j]->Fill(rFilter[j]);
			fSmootherHistos[i][j]->Fill(rSmoother[j]);
		}
		fFilterHistos[i][11]->Fill(node->GetChiSqFiltered());
		fSmootherHistos[i][11]->Fill(node->GetChiSqSmoothed());
	}
}

void CbmLitPropagationAnalysis::McPointToLitFitNode(
		FairMCPoint* point,
		CbmLitFitNode* node)
{
	Int_t mcId = point->GetTrackID();
    CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(mcId);
    Int_t pdg = mcTrack->GetPdgCode();
    Double_t q;
    if (pdg > 0) q = -1.; else q = 1;
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

void CbmLitPropagationAnalysis::PrintStopwatchStatistics()
{
	std::cout << "Stopwatch: " << std::endl;
	std::cout << "propagation: counts=" << fPropagationWatch.Counter()
		<< ", real=" << fPropagationWatch.RealTime()/fPropagationWatch.Counter()
		<< "/" << fPropagationWatch.RealTime()
		<< " s, cpu=" << fPropagationWatch.CpuTime()/fPropagationWatch.Counter()
		<< "/" << fPropagationWatch.CpuTime() << std::endl;
	std::cout << "fitter: real=" << fFitterWatch.Counter()
		<< ", real=" << fFitterWatch.RealTime()/fFitterWatch.Counter()
		<< "/" << fFitterWatch.RealTime()
		<< " s, cpu=" << fFitterWatch.CpuTime()/fFitterWatch.Counter()
		<< "/" << fFitterWatch.CpuTime() << std::endl;
	std::cout << "smoother: real=" << fSmootherWatch.Counter()
		<< ", real=" << fSmootherWatch.RealTime()/fSmootherWatch.Counter()
		<< "/" << fSmootherWatch.RealTime()
		<< " s, cpu=" << fSmootherWatch.CpuTime()/fSmootherWatch.Counter()
		<< "/" << fSmootherWatch.CpuTime() << std::endl;
}

ClassImp(CbmLitPropagationAnalysis)
