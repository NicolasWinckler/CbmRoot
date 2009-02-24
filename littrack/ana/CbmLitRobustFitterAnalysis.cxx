#include "CbmLitRobustFitterAnalysis.h"

#include "CbmLitTrack.h"
#include "CbmLitEnvironment.h"
#include "CbmLitConverter.h"
#include "CbmLitToolFactory.h"
#include "CbmLitMemoryManagment.h"
#include "CbmLitTypes.h"
#include "CbmLitTrackFitter.h"

#include "CbmMuchTrack.h"
#include "CbmTrdTrack.h"
#include "CbmStsTrack.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "CbmMuchTrackMatch.h"

#ifdef CbmMuchGeoScheme_H
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

#ifdef CbmMuchGeoScheme_H
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
	fNofPlanes = 13;
//	fNofParams = 12;

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
	AnalyseTracks();
	TestFitter();

	DeleteTrackArrays();
	std::cout << "Event: " << fEvents++ << std::endl;
}

void CbmLitRobustFitterAnalysis::AnalyseTracks()
{
	for(TrackPtrIterator it = fLitTracks.begin(); it != fLitTracks.end(); it++){
		CbmLitTrack* track = *it;
		if (track->GetLastPlaneId() < 5) continue;
		std::vector<HitPtrIteratorPair> bounds;
		track->GetHitBounds(bounds);
		for (Int_t i = 0; i < bounds.size(); i++){
			Int_t nofHits = bounds[i].second - bounds[i].first;
			Int_t planeId = ((*bounds[i].first)->GetPlaneId());
			fhPlaneNofHits->Fill(planeId, nofHits);
			fhNofHitsInPlane->Fill(nofHits);
		}
		fhLastPlaneIdNofHits->Fill(track->GetLastPlaneId(), track->GetNofHits());
		fhNofHitsInTrack->Fill(track->GetNofHits());
		fhLastPlaneIdInTrack->Fill(track->GetLastPlaneId());
		CbmMuchTrackMatch* trackMatch = (CbmMuchTrackMatch*) fTrackMatches->At(track->GetRefId());
		fhNofTrueHitsInTrack->Fill(trackMatch->GetNofTrueHits());
		fhNofFalseHitsInTrack->Fill(trackMatch->GetNofWrongHits() + trackMatch->GetNofFakeHits());
	}
}

void CbmLitRobustFitterAnalysis::TestFitter()
{
	for(TrackPtrIterator it = fLitTracks.begin(); it != fLitTracks.end(); it++){
		fFitter->Fit(*it);
	}
}

void CbmLitRobustFitterAnalysis::CreateTrackArrays()
{
	Int_t nofTracks = fTracks->GetEntriesFast();
	for (Int_t iTrack = 0; iTrack < nofTracks; iTrack++) {
		if (fDetId == kMUCH) {
			CbmMuchTrack* muchTrack = (CbmMuchTrack*) fTracks->At(iTrack);
			//if (muchTrack->GetNHits() == 0) continue;
			CbmLitTrack* litTrack = new CbmLitTrack();
			CbmLitConverter::MuchTrackToLitTrack(muchTrack, litTrack, fHits);
			InitTrackParamFromStsTrackParam(muchTrack->GetStsTrackID(), litTrack);
			litTrack->SetRefId(iTrack);
			fLitTracks.push_back(litTrack);
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
			fLitTracks.push_back(litTrack);
			//CbmLitTrack* mcLitTrack = new CbmLitTrack();
			//McTrdTrackToLitTrack(trdTrack, mcLitTrack);
			//fLitMcTracks.push_back(mcLitTrack);
		}
	}
}

void CbmLitRobustFitterAnalysis::DeleteTrackArrays()
{
	for_each(fLitTracks.begin(), fLitTracks.end(), DeleteObject());
//	for_each(fLitMcTracks.begin(), fLitMcTracks.end(), DeleteObject());
	fLitTracks.clear();
//	fLitMcTracks.clear();
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
	fHistoList = new TList();

	Int_t nofBinsPlane = 27;
	Int_t minPlane = 0;
	Int_t maxPlane = 27;
	Int_t nofBinsNofHits = 100;
	Int_t minNofHits = 0;
	Int_t maxNofHits = 100;
	Int_t nofBinsLastPlaneId = 7;
	Int_t minLastPlaneId = 0;
	Int_t maxLastPlaneId = 7;

	fhPlaneNofHits = new TH2D("hPlaneNofHits", "nof hits vs. plane",
			nofBinsPlane, minPlane, maxPlane, nofBinsNofHits, minNofHits, maxNofHits);
	fhLastPlaneIdNofHits = new TH2D("hLastPlaneIdNofHits", "nof hits vs. last plane id",
			nofBinsLastPlaneId, minLastPlaneId, maxLastPlaneId, nofBinsNofHits, minNofHits, maxNofHits);
	fhNofHitsInTrack = new TH1D("hNofHitsInTrack", "nof hits in track",
			nofBinsNofHits, minNofHits, maxNofHits);
	fhNofTrueHitsInTrack = new TH1D("hNofTrueHitsInTrack", "nof true hits in track",
			nofBinsNofHits, minNofHits, maxNofHits);
	fhNofFalseHitsInTrack = new TH1D("hNofFalseHitsInTrack", "nof false hits in track",
			nofBinsNofHits, minNofHits, maxNofHits);
	fhLastPlaneIdInTrack = new TH1D("hLastPlaneIdInTrack", "last plane id in track",
			nofBinsLastPlaneId, minLastPlaneId, maxLastPlaneId);
	fhNofHitsInPlane = new TH1D("hNofHitsInPlane", "nof hits in plane",
			nofBinsNofHits, minNofHits, maxNofHits);
	fHistoList->Add(fhPlaneNofHits);
	fHistoList->Add(fhLastPlaneIdNofHits);
	fHistoList->Add(fhNofHitsInTrack);
	fHistoList->Add(fhNofTrueHitsInTrack);
	fHistoList->Add(fhNofFalseHitsInTrack);
	fHistoList->Add(fhLastPlaneIdInTrack);
	fHistoList->Add(fhNofHitsInPlane);
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
//	   fMCPoints  = (TClonesArray*) ioman->GetObject("MuchPoint");
//	   if (!fMCPoints) Fatal("Init", "No MuchPoint array!");
	   fHits = (TClonesArray*) ioman->GetObject("MuchHit");
	   if (!fHits) Fatal("Init", "No MuchHit array!");
//	   fDigiMatches  = (TClonesArray*) ioman->GetObject("MuchDigiMatch");
//	   if (!fDigiMatches) Fatal("Init", "No MuchDigiMatches array!");
	   fTracks = (TClonesArray*) ioman->GetObject("MuchTrack");
	   if (!fTracks) Fatal("Init", "No MuchTrack array!");
	   fTrackMatches = (TClonesArray*) ioman->GetObject("MuchTrackMatch");
	   if (!fTrackMatches) Fatal("Init", "No MuchTrackMatch array!");
	} else
	if (fDetId == kTRD) {
//	   fMCPoints  = (TClonesArray*) ioman->GetObject("TRDPoint");
//	   if (!fMCPoints) Fatal("Init", "No TRDPoint array!");
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

void CbmLitRobustFitterAnalysis::Finish()
{
	TIter next(fHistoList);
	while ( TH1* histo = ((TH1*)next()) ) histo->Write();
}
ClassImp(CbmLitRobustFitterAnalysis);
