#include "CbmLitRobustFitterAnalysis.h"

#include "CbmLitTrack.h"
#include "CbmLitEnvironment.h"
#include "CbmLitConverter.h"
#include "CbmLitToolFactory.h"
#include "CbmLitMemoryManagment.h"
#include "CbmLitTypes.h"
#include "CbmLitTrackFitter.h"

#include "CbmBaseHit.h"
#include "CbmPixelHit.h"
#include "CbmStripHit.h"
#include "CbmMuchTrack.h"
//#include "CbmMuchHit.h"
//#include "CbmTrdHit.h"
#include "CbmTrdTrack.h"
#include "CbmStsTrack.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "CbmTrackMatch.h"
//#include "CbmTrdTrackMatch.h"
#include "CbmGeoMuchPar.h"
#include "FairMCPoint.h"
#include "CbmMuchDigiMatch.h"

#ifdef NEWMUCH
#include "CbmMuchGeoScheme.h"
#endif

#include "TH1D.h"
#include "TH2D.h"
#include "TList.h"
#include "TClonesArray.h"

#include <algorithm>
#include <iostream>
#include <vector>

CbmLitRobustFitterAnalysis::CbmLitRobustFitterAnalysis(
		DetectorId detId)
{
	fDetId = detId;
	fVerbose = 1;
	fEvents = 0;
}

CbmLitRobustFitterAnalysis::~CbmLitRobustFitterAnalysis()
{
	fHistoList->Delete();
	delete fHistoList;
}

InitStatus CbmLitRobustFitterAnalysis::Init()
{
	ReadDataBranches();

#ifdef NEWMUCH
	CbmMuchGeoScheme* geoScheme = CbmMuchGeoScheme::Instance();
	FairRuntimeDb* db = FairRuntimeDb::instance();
	CbmGeoMuchPar* geoPar = (CbmGeoMuchPar*) db->getContainer("CbmGeoMuchPar");
	TObjArray* stations = (TObjArray*) geoPar->GetStations();
	geoScheme->Init(stations);
#endif

	// Create tools
	CbmLitToolFactory* factory = CbmLitToolFactory::Instance();
	fFitter = factory->CreateTrackFitter("kalman_robust");

	// Get the layout
	if (fDetId == kMUCH) fLayout = CbmLitEnvironment::Instance()->GetMuchLayout();
	if (fDetId == kTRD) fLayout = CbmLitEnvironment::Instance()->GetTrdLayout();
	fNofPlanes = fLayout.GetNofPlanes();
	fNofPlanes = 26;
	fNofVars = 2;
	fNofParams = 15;
//	fNof1DParams = 5;
//	fNof2DParams = 2;

	CreateHistograms();
}

void CbmLitRobustFitterAnalysis::SetParContainers()
{
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb = ana->GetRuntimeDb();
    rtdb->getContainer("FairBaseParSet");
    rtdb->getContainer("CbmGeoMuchPar");
    rtdb->getContainer("CbmFieldPar");
}

void CbmLitRobustFitterAnalysis::Exec(
		Option_t* opt)
{
	CreateTrackArrays();

	if(fDetId == kMUCH)	MatchMuchTracks(fLitTracks[0], fLitMuchTrackMatches[0]);
	if(fDetId == kTRD)	MatchTrdTracks(fLitTracks[0], fLitTrdTrackMatches[0]);
	std::cout << "MatchTracks finished" << std::endl;
	FillHistograms(0);
	std::cout << "FillHistograms(0) finished" << std::endl;

	TestFitter(1);
	if(fDetId == kMUCH)	MatchMuchTracks(fLitTracks[1], fLitMuchTrackMatches[1]);
	if(fDetId == kTRD)	MatchTrdTracks(fLitTracks[1], fLitTrdTrackMatches[1]);
	FillHistograms(1);

	DeleteTrackArrays();
	std::cout << "Event: " << fEvents++ << std::endl;
}

void CbmLitRobustFitterAnalysis::FillHistograms(
		Int_t id)
{
	for(Int_t i = 0; i < fLitTracks[id].size(); i++) {
		CbmLitTrack* track = fLitTracks[id][i];
		if (track->GetQuality() == kLITBAD) continue;
		if (track->GetLastPlaneId() < 5) continue;
//		if (track->GetNofHits() < 10 && track->GetNofHits() > 20) continue;
		//if (track->GetChi2()/track->GetNDF() > 5) continue;
		Int_t nofTrue, nofFalse;
		if (fDetId == kMUCH) {
			CbmTrackMatch* trackMatch = fLitMuchTrackMatches[id][i];
			nofTrue = trackMatch->GetNofTrueHits();
			nofFalse = trackMatch->GetNofWrongHits() + trackMatch->GetNofFakeHits();
		} else {
			CbmTrackMatch* trackMatch = fLitTrdTrackMatches[id][i];
			nofTrue = trackMatch->GetNofTrueHits();
			nofFalse = trackMatch->GetNofWrongHits() + trackMatch->GetNofFakeHits();
		}
		Int_t nofHits = track->GetNofHits();
		Double_t ratio = (Double_t) nofTrue/ (Double_t)nofHits;
		Bool_t isGhost = (ratio < 0.7);
		std::vector<HitPtrIteratorPair> bounds;
		track->GetHitBounds(bounds);
		for (Int_t j = 0; j < bounds.size(); j++){
			Int_t nofHits = bounds[j].second - bounds[j].first;
			Int_t planeId = ((*bounds[j].first)->GetPlaneId());
			TH2* hist = (TH2*)fHistoList->FindObject("hPlaneNofHits_0");
			hist->Fill(planeId, nofHits);
			fhPlaneNofHits[id]->Fill(planeId, nofHits);
			fhNofHitsInPlane[id]->Fill(nofHits);
		}
		fhLastPlaneIdNofHits[id]->Fill(track->GetLastPlaneId(), track->GetNofHits());
		fhNofTrueHitsInTrack[id]->Fill(nofTrue);
		fhNofFalseHitsInTrack[id]->Fill(nofFalse);
		fhNofHitsInTrack[id]->Fill(nofHits);
		fhTrueHitsRatioInTrack[id]->Fill(ratio);

		Double_t chisq = track->GetChi2() / track->GetNDF();
		fhTrackChiSq[id]->Fill(chisq);
		fhTrackLastPlaneId[id]->Fill(track->GetLastPlaneId());
		if (isGhost) {
			fhGhostTrackChiSq[id]->Fill(chisq);
			fhGhostTrackLastPlaneId[id]->Fill(track->GetLastPlaneId());
			fhNofHitsInGhostTrack[id]->Fill(nofHits);
		} else {
			fhTrueTrackChiSq[id]->Fill(chisq);
			fhTrueTrackLastPlaneId[id]->Fill(track->GetLastPlaneId());
			fhNofHitsInTrueTrack[id]->Fill(nofHits);
		}
	}
}

void CbmLitRobustFitterAnalysis::TestFitter(
		Int_t id)
{
	for(TrackPtrIterator it = fLitTracks[id].begin(); it != fLitTracks[id].end(); it++){
		if (fFitter->Fit(*it) == kLITERROR) (*it)->SetQuality(kLITBAD);
	}
}

void CbmLitRobustFitterAnalysis::CreateTrackArrays()
{
	fLitTracks.resize(2);
	fLitMuchTrackMatches.resize(2);
	fLitTrdTrackMatches.resize(2);
	Int_t nofTracks = fTracks->GetEntriesFast();
	for (Int_t iTrack = 0; iTrack < nofTracks; iTrack++) {
		if (fDetId == kMUCH) {
			CbmMuchTrack* muchTrack = (CbmMuchTrack*) fTracks->At(iTrack);
			//if (muchTrack->GetNHits() == 0) continue;
			CbmLitTrack* litTrack = new CbmLitTrack();
			CbmLitConverter::MuchTrackToLitTrack(muchTrack, litTrack, fHits);
			InitTrackParamFromStsTrackParam(muchTrack->GetStsTrackID(), litTrack);
			litTrack->SetRefId(iTrack);
			fLitTracks[0].push_back(litTrack);
			fLitTracks[1].push_back(new CbmLitTrack(*litTrack));
			//CbmLitTrack* mcLitTrack = new CbmLitTrack();
			//McMuchTrackToLitTrack(muchTrack, mcLitTrack);
			//fLitMcTracks.push_back(mcLitTrack);
		}
		if (fDetId == kTRD) {
			CbmTrdTrack* trdTrack = (CbmTrdTrack*) fTracks->At(iTrack);
			//if (trdTrack->GetNofTrdHits() == 0) continue;
			CbmLitTrack* litTrack = new CbmLitTrack();
			CbmLitConverter::TrdTrackToLitTrack(trdTrack, litTrack, fHits);
			InitTrackParamFromStsTrackParam(trdTrack->GetStsTrackIndex(), litTrack);
			litTrack->SetRefId(iTrack);
			fLitTracks[0].push_back(litTrack);
			fLitTracks[1].push_back(new CbmLitTrack(*litTrack));
			//CbmLitTrack* mcLitTrack = new CbmLitTrack();
			//McTrdTrackToLitTrack(trdTrack, mcLitTrack);
			//fLitMcTracks.push_back(mcLitTrack);
		}
	}
}

void CbmLitRobustFitterAnalysis::MatchMuchTracks(
		TrackPtrVector& tracks,
		std::vector<CbmTrackMatch*>& matches)
{
	for (TrackPtrIterator it = tracks.begin(); it < tracks.end(); it++) {
		std::map<Int_t, Int_t> matchMap;
		Int_t nofHits = (*it)->GetNofHits();
		for (Int_t iHit = 0; iHit < nofHits; iHit++) {
			CbmPixelHit* pHit = (CbmPixelHit*) fHits->At((*it)->GetHit(iHit)->GetRefId());
			Int_t digiIndex = pHit->GetRefId();
			CbmMuchDigiMatch* pDigiMatch = (CbmMuchDigiMatch*) fDigiMatches->At(digiIndex);
			for (Int_t iDigi = 0; iDigi < pDigiMatch->GetNPoints(); iDigi++) {
			   Int_t pointIndex = pDigiMatch->GetRefIndex(iDigi);
			   if (pointIndex < 0) { // Fake or background hit
				  matchMap[-1]++;
				  continue;
			   }
			   FairMCPoint* pPoint = (FairMCPoint*) fPoints->At(pointIndex);
			   matchMap[pPoint->GetTrackID()]++;
			}
		} // loop over hits

		Int_t nofTrue = 0;
		Int_t bestMcTrackId = -1;
		for (std::map<Int_t, Int_t>::iterator it = matchMap.begin(); it!=matchMap.end(); it++) {
			if (it->first != -1 && it->second > nofTrue) {
			   bestMcTrackId = it->first;
			   nofTrue = it->second;
			}
		}
		Int_t nofFake = 0;
		Int_t nofWrong = nofHits - nofTrue - nofFake;
		Int_t nofMcTracks = matchMap.size() - 1;

		matches.push_back(new CbmTrackMatch(bestMcTrackId, nofTrue, nofWrong, nofFake, nofMcTracks));
//		std::cout << "match added" << std::endl;
	} // Track loop
}

void CbmLitRobustFitterAnalysis::MatchTrdTracks(
		TrackPtrVector& tracks,
		std::vector<CbmTrackMatch*>& matches)
{
	for (TrackPtrIterator it = tracks.begin(); it < tracks.end(); it++) {
		std::map<Int_t, Int_t> matchMap;
		Int_t nofHits = (*it)->GetNofHits();
		for (Int_t iHit = 0; iHit < nofHits; iHit++) {
			CbmPixelHit* hit = (CbmPixelHit*) fHits->At((*it)->GetHit(iHit)->GetRefId());
			Int_t iPoint = hit->GetRefId();
			if (iPoint < 0) {
				matchMap[-1]++;
				continue;
			}
			FairMCPoint* point = (FairMCPoint*) fPoints->At(iPoint);
			matchMap[point->GetTrackID()]++;
	    }
		Int_t nofTrue = 0;
		Int_t bestMcTrackId = -1;
		for (std::map<Int_t, Int_t>::iterator it = matchMap.begin(); it!=matchMap.end(); it++) {
			if (it->first != -1 && it->second > nofTrue) {
			   bestMcTrackId = it->first;
			   nofTrue = it->second;
			}
		}
		Int_t nofFake = 0;
		Int_t nofWrong = nofHits - nofTrue - nofFake;
		Int_t nofMcTracks = matchMap.size() - 1;

		matches.push_back(new CbmTrackMatch(bestMcTrackId, nofTrue, nofWrong, nofFake, nofMcTracks));

	} // Track loop
}

void CbmLitRobustFitterAnalysis::DeleteTrackArrays()
{
	for (Int_t i = 0; i < fNofVars; i++) {
		for_each(fLitTracks[i].begin(), fLitTracks[i].end(), DeleteObject());
		for_each(fLitMuchTrackMatches[i].begin(), fLitMuchTrackMatches[i].end(), DeleteObject());
	//	for_each(fLitMcTracks.begin(), fLitMcTracks.end(), DeleteObject());
		fLitTracks[i].clear();
		fLitMuchTrackMatches[i].clear();
	//	fLitMcTracks.clear();
	}
}

void CbmLitRobustFitterAnalysis::InitTrackParamFromStsTrackParam(
		Int_t trackIndex,
		CbmLitTrack* track)
{
	CbmStsTrack* stsTrack = (CbmStsTrack*)fStsTracks->At(trackIndex);
//	if (stsTrack == NULL) return false;
	CbmLitTrackParam par;
	CbmLitConverter::TrackParamToLitTrackParam(stsTrack->GetParamLast(), &par);
	track->SetParamLast(&par);
	track->SetParamFirst(&par);
//	return true;
}

void CbmLitRobustFitterAnalysis::CreateHistograms()
{
//	std::string names1D[] = {"hNofHitsInTrack", "hNofTrueHitsInTrack", "hNofFalseHitsInTrack",
//			"hLastPlaneIdInTrack", "hNofHitsInPlane"};
//	std::string titles1D[] = { "nof hits in track", "nof true hits in track", "nof false hits in track",
//			"last plane id in track", "nof hits in plane"};
//	Int_t bins1D[] =      {100, 100, 100, 30, 100};
//	Double_t boundL1D[] = {0  ,   0,   0,  0,   0};
//	Double_t boundR1D[] = {100, 100, 100, 30, 100};
//
//	std::string names2D[] = { "hPlaneNofHits", "hLastPlaneIdNofHits"};
//	std::string titles2D[] = { "nof hits vs. plane", "nof hits vs. last plane id"};
//	Int_t bins2D1[] =      {30, 30};
//	Double_t boundL2D1[] = {0 ,  0};
//	Double_t boundR2D1[] = {30, 30};
//	Int_t bins2D2[] =      {100, 100};
//	Double_t boundL2D2[] = {0  ,   0};
//	Double_t boundR2D2[] = {100, 100};

	fHistoList = new TList();

	Int_t nofBinsNofHits = 100;
	Int_t minNofHits = 0;
	Int_t maxNofHits = 100;
	Int_t nofBinsNofPlanes = 30;
	Int_t minNofPlanes = 0;
	Int_t maxNofPlanes = 30;
	Int_t nofBinsLastPlaneId = 30;
	Int_t minLastPlaneId = 0;
	Int_t maxLastPlaneId = 30;
	Int_t nofBinsChiSq= 100;
	Int_t minChiSq = -10;
	Int_t maxChiSq = 100;

	std::string names[] = {
			"hPlaneNofHits", "hLastPlaneIdNofHits", "hNofHitsInPlane", "hTrueHitsRatioInTrack",
			"hNofHitsInTrack", "hNofTrueHitsInTrack", "hNofFalseHitsInTrack",
			"hNofHitsInTrueTrack", "hNofHitsInGhostTrack",
			"hTrackLastPlaneId", "hTrueTrackLastPlaneId", "hGhostTrackLastPlaneId",
			"hTrackChiSq", "hTrueTrackChiSq", "hGhostTrackChiSq"};
	std::string titles[] = {
			"nof hits vs. plane", "nof hits vs. last plane id", "nof hits in plane", "true hits ration in track",
			"nof hits in track", "nof true hits in track", "nof false hits in track",
			"nof hits in true track", "number of hits in false track",
			"last plane id for track", "last plane id for true track", "last plane id for ghost track",
			"chi square of the track", "chi square of the true track", "chi square of the ghost track"};
	fhPlaneNofHits.resize(fNofVars);
	fhLastPlaneIdNofHits.resize(fNofVars);
	fhNofHitsInPlane.resize(fNofVars);
	fhTrueHitsRatioInTrack.resize(fNofVars);
	fhNofHitsInTrack.resize(fNofVars);
	fhNofTrueHitsInTrack.resize(fNofVars);
	fhNofFalseHitsInTrack.resize(fNofVars);
	fhNofHitsInTrueTrack.resize(fNofVars);
	fhNofHitsInGhostTrack.resize(fNofVars);
	fhTrackLastPlaneId.resize(fNofVars);
	fhTrueTrackLastPlaneId.resize(fNofVars);
	fhGhostTrackLastPlaneId.resize(fNofVars);
	fhTrackChiSq.resize(fNofVars);
	fhTrueTrackChiSq.resize(fNofVars);
	fhGhostTrackChiSq.resize(fNofVars);

//	fh1DHistos.resize(fNofVars);
//	fh2DHistos.resize(fNofVars);

	for (Int_t v = 0; v < fNofVars; v++) {
		std::stringstream histName[fNofParams];
		for (Int_t j = 0; j < fNofParams; j++) {
			histName[j] << names[j] << "_" << v;
			std::cout << titles[j] << std::endl;
		}

		fhPlaneNofHits[v] = new TH2D(histName[0].str().c_str(), titles[0].c_str(),
				nofBinsNofPlanes, minNofPlanes, maxNofPlanes, nofBinsNofHits, minNofHits, maxNofHits);
		fhLastPlaneIdNofHits[v] = new TH2D(histName[1].str().c_str(), titles[1].c_str(),
				nofBinsLastPlaneId, minLastPlaneId, maxLastPlaneId, nofBinsNofHits, minNofHits, maxNofHits);;
		fhNofHitsInPlane[v] = new TH1D(histName[2].str().c_str(), titles[2].c_str(),
				nofBinsNofPlanes, minNofPlanes, maxNofPlanes);
		fhTrueHitsRatioInTrack[v] = new TH1D(histName[3].str().c_str(), titles[3].c_str(),
				20, 0., 1.);
		fhNofHitsInTrack[v] = new TH1D(histName[4].str().c_str(), titles[4].c_str(),
				nofBinsNofHits, minNofHits, maxNofHits);
		fhNofTrueHitsInTrack[v] = new TH1D(histName[5].str().c_str(), titles[5].c_str(),
				nofBinsNofHits, minNofHits, maxNofHits);
		fhNofFalseHitsInTrack[v] = new TH1D(histName[6].str().c_str(), titles[6].c_str(),
				nofBinsNofHits, minNofHits, maxNofHits);
		fhNofHitsInTrueTrack[v] = new TH1D(histName[7].str().c_str(), titles[7].c_str(),
				nofBinsNofHits, minNofHits, maxNofHits);
		fhNofHitsInGhostTrack[v] = new TH1D(histName[8].str().c_str(), titles[8].c_str(),
				nofBinsNofHits, minNofHits, maxNofHits);
		fhTrackLastPlaneId[v] = new TH1D(histName[9].str().c_str(), titles[9].c_str(),
				nofBinsLastPlaneId, minLastPlaneId, maxLastPlaneId);
		fhTrueTrackLastPlaneId[v] = new TH1D(histName[10].str().c_str(), titles[10].c_str(),
						nofBinsLastPlaneId, minLastPlaneId, maxLastPlaneId);
		fhGhostTrackLastPlaneId[v] = new TH1D(histName[11].str().c_str(), titles[11].c_str(),
						nofBinsLastPlaneId, minLastPlaneId, maxLastPlaneId);
		fhTrackChiSq[v] = new TH1D(histName[12].str().c_str(), titles[12].c_str(),
				nofBinsChiSq, minChiSq, maxChiSq);
		fhTrueTrackChiSq[v] = new TH1D(histName[13].str().c_str(), titles[13].c_str(),
				nofBinsChiSq, minChiSq, maxChiSq);
		fhGhostTrackChiSq[v] = new TH1D(histName[14].str().c_str(), titles[14].c_str(),
				nofBinsChiSq, minChiSq, maxChiSq);
		fHistoList->Add(fhPlaneNofHits[v]);
		fHistoList->Add(fhLastPlaneIdNofHits[v]);
		fHistoList->Add(fhNofHitsInPlane[v]);
		fHistoList->Add(fhTrueHitsRatioInTrack[v]);
		fHistoList->Add(fhNofHitsInTrack[v]);
		fHistoList->Add(fhNofTrueHitsInTrack[v]);
		fHistoList->Add(fhNofFalseHitsInTrack[v]);
		fHistoList->Add(fhNofHitsInTrueTrack[v]);
		fHistoList->Add(fhNofHitsInGhostTrack[v]);
		fHistoList->Add(fhTrackLastPlaneId[v]);
		fHistoList->Add(fhTrueTrackLastPlaneId[v]);
		fHistoList->Add(fhGhostTrackLastPlaneId[v]);
		fHistoList->Add(fhTrackChiSq[v]);
		fHistoList->Add(fhTrueTrackChiSq[v]);
		fHistoList->Add(fhGhostTrackChiSq[v]);

//		fh1DHistos[v].resize(fNof1DParams);
//		for (Int_t j = 0; j < fNof1DParams; j++){
//			std::stringstream histName;
//			histName << names1D[j] << "_" << v;
//			fh1DHistos[v][j] = new TH1D(histName.str().c_str(), titles1D[j].c_str(),
//					bins1D[j], boundL1D[j], boundR1D[j]);
//		}
//
//		fh2DHistos[v].resize(fNof2DParams);
//		for (Int_t j = 0; j < fNof2DParams; j++){
//			std::stringstream histName;
//			histName << names2D[j] << "_" << v;
//			fh2DHistos[v][j] = new TH2D(histName.str().c_str(), titles2D[j].c_str(),
//					bins2D1[j], boundL2D1[j], boundR2D1[j], bins2D2[j], boundL2D2[j], boundR2D2[j]);
//		}
	}
}

void CbmLitRobustFitterAnalysis::ReadDataBranches()
{
	FairRootManager* ioman = FairRootManager::Instance();
	if (!ioman ) Fatal("Init", "No CbmRootManager");

//	fMCTracks  = (TClonesArray*) ioman->GetObject("MCTrack");
//	if (!fMCTracks) Fatal("Init", "No MCTrack array!");
	fStsTracks = (TClonesArray*) ioman->GetObject("STSTrack");
	if (!fStsTracks) Fatal("Init", "No STSTrack array!");
//	fStsTrackMatches = (TClonesArray*) ioman->GetObject("STSTrackMatch");
//	if (!fStsTrackMatches) Fatal("Init", "No STSTrackMatch array!");

	if (fDetId == kMUCH) {
	   fPoints  = (TClonesArray*) ioman->GetObject("MuchPoint");
	   if (!fPoints) Fatal("Init", "No MuchPoint array!");
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
	   fPoints  = (TClonesArray*) ioman->GetObject("TRDPoint");
	   if (!fPoints) Fatal("Init", "No TRDPoint array!");
	   fHits = (TClonesArray*) ioman->GetObject("TRDHit");
	   if (!fHits) Fatal("Init", "No TRDHit array!");
	   fTracks = (TClonesArray*) ioman->GetObject("TRDTrack");
	   if (!fTracks) Fatal("Init", "No TRDTrack array!");
	   fTrackMatches = (TClonesArray*) ioman->GetObject("TRDTrackMatch");
	   if (!fTrackMatches) Fatal("Init", "No TRDTrackMatch array!");
	} else {
	   Fatal("Init", "CbmLitRobustFitterAnalysis works only with MUCH and TRD!");
	}
}

void CbmLitRobustFitterAnalysis::FinishTask()
{
//	for (Int_t v = 0; v < fNofVars; v++) {
//		for (Int_t j = 0; j < fNof1DParams; j++)
//			fh1DHistos[v][j]->Write();
//		for (Int_t j = 0; j < fNof2DParams; j++)
//			fh2DHistos[v][j]->Write();
//	}
	TIter next(fHistoList);
	while ( TH1* histo = ((TH1*)next()) ) histo->Write();
}
ClassImp(CbmLitRobustFitterAnalysis);
