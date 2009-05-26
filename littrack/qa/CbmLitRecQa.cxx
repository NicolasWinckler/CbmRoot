#include "CbmLitRecQa.h"

#include "CbmBaseHit.h"
#include "CbmPixelHit.h"
#include "CbmStripHit.h"
#include "CbmMuchTrack.h"
#include "CbmTrackMatch.h"
#include "CbmTrdTrack.h"
#include "CbmTrdHit.h"
#include "CbmStsTrack.h"
#include "FairTrackParam.h"
#include "FairMCPoint.h"
#include "CbmMCTrack.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"
#include "TDirectory.h"
#include "TH1F.h"
#include "TList.h"
#include "TVector3.h"

#include <iostream>
#include <map>
#include <cmath>

CbmLitRecQa::CbmLitRecQa(
		Int_t minPoints,
		Double_t quota,
		DetectorId detId,
		Int_t iVerbose):
  FairTask("LitRecQA", iVerbose),
  fMinPoints(minPoints),
  fQuota(quota),
  fDetId(detId),
  fNormType(1)
{
	ZeroGlobalCounters();
//	fNN.open("much_rec_qa_ghost_clones.txt");
}

CbmLitRecQa::~CbmLitRecQa()
{
  fHistoList->Delete();
  delete fHistoList;
//  fNN.close();
}

void CbmLitRecQa::SetParContainers()
{

}

InitStatus CbmLitRecQa::Init()
{
    FairRootManager* ioman = FairRootManager::Instance();
    if (NULL == ioman) Fatal("Init","CbmRootManager is not instantiated");

    fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
    if (NULL == fMCTracks) Fatal("Init","No MCTrack array!");

    if (fDetId == kMUCH) {
    	fMCPoints = (TClonesArray*) ioman->GetObject("MuchPoint");
    	if (NULL == fMCPoints ) Fatal("Init","No MuchPoint array!");
    	fRecHits = (TClonesArray*) ioman->GetObject("MuchPixelHit");
    	if (NULL == fRecHits) Fatal("Init", "No MuchPixelHit array!");
    	fRecTracks = (TClonesArray*) ioman->GetObject("MuchTrack");
    	if (NULL == fRecTracks) Fatal("Init","No MuchTrack array!");
    	fRecMatches = (TClonesArray*) ioman->GetObject("MuchTrackMatch");
    	if (NULL == fRecMatches) Fatal("Init","No MuchTrackMatch array!");
    } else
    if (fDetId == kTRD) {
    	fMCPoints = (TClonesArray*) ioman->GetObject("TRDPoint");
    	if (NULL == fMCPoints ) Fatal("Init","No TRDPoint array!");
    	fRecHits = (TClonesArray*) ioman->GetObject("TRDHit");
    	if (NULL == fRecHits) Fatal("Init", "No TRDHit array!");
    	fRecTracks = (TClonesArray*) ioman->GetObject("TRDTrack");
    	if (NULL == fRecTracks) Fatal("Init","No TRDTrack array!");
    	fRecMatches = (TClonesArray*) ioman->GetObject("TRDTrackMatch");
    	if (NULL == fRecMatches) Fatal("Init","No TRDTrackMatch array!");
    } else {
    	Fatal("Init","This detector type is not supported");
    }

    if (fNormType == 2) {
    	fStsTracks = (TClonesArray*) ioman->GetObject("STSTrack");
    	if (NULL == fStsTracks) Fatal("Init","No STStrack array!");
    	fStsMatches = (TClonesArray*) ioman->GetObject("STSTrackMatch");
    	if (NULL == fStsMatches) Fatal("Init",": No STSTrackMatch array!");
    }

    CreateHistos();

    return kSUCCESS;
}

InitStatus CbmLitRecQa::ReInit()
{
	return kSUCCESS;
}

void CbmLitRecQa::Exec(
		Option_t* opt)
{

  ZeroEventCounters();

  ProcessStsTracks();

  if (fDetId == kMUCH) ProcessMuchTracks(); else
  if (fDetId == kTRD) ProcessTrdTracks();

  ProcessMcTracks();

  CountClones();

  CalcEventEff();

  PrintEventStatistic();

  IncreaseCounters();
}

void CbmLitRecQa::FinishTask()
{
	CalcEffAndRates();

	PrintStatistic();

	WriteToFile();

//	std::cout << "number of true hits" << std::endl;
//	for (int i = 0; i < 15; i++)
//		std::cout << "i=" << i << " n=" << fNofTrue[i] << std::endl;
}


void CbmLitRecQa::ProcessStsTracks()
{
  fMcStsMap.clear();
  if (fNormType == 2) { // Normalization to number of Sts AND Much tracks
	fEvNofStsTracks = fStsTracks->GetEntriesFast();
    for(Int_t iStsTrack = 0; iStsTrack < fEvNofStsTracks; iStsTrack++) {
      CbmTrackMatch* stsTrackM = (CbmTrackMatch*) fStsMatches->At(iStsTrack);
      if (stsTrackM == NULL) continue;

      Int_t mcId = stsTrackM->GetMCTrackId();
      if(mcId == -1) continue;

      CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(mcId);
      if (mcTrack == NULL) continue;

      Int_t nofTrue = stsTrackM->GetNofTrueHits();
      Int_t nofWrong = stsTrackM->GetNofWrongHits();
      Int_t nofFake = stsTrackM->GetNofFakeHits();
      Int_t nofHits = nofTrue + nofWrong + nofFake;
      Double_t quali = Double_t(nofTrue) / Double_t(nofHits);
      if (quali < fQuota) {
    	  if (fVerbose > 2)std::cout << "Fake STS track, quali = " << quali << std::endl;
    	  continue;
      }

      fMcStsMap.insert(std::pair<Int_t, Int_t>(mcId, iStsTrack));
    }
  }
//  std::cout << "fMcStsMap.size()=" <<  fMcStsMap.size() << std::endl;
}

void CbmLitRecQa::ProcessMuchTracks()
{
  fMcRecMap.clear();
  fEvNofRecTracks = fRecTracks->GetEntriesFast();
  for (Int_t iRec = 0; iRec < fEvNofRecTracks; iRec++) {

	  CbmMuchTrack* recTrack = (CbmMuchTrack*) fRecTracks->At(iRec);
	  CbmTrackMatch* recMatch = (CbmTrackMatch*) fRecMatches->At(iRec);
	  if (recTrack == NULL || recMatch == NULL) continue;
	  Int_t mcIdRec = recMatch->GetMCTrackId();

	  //std::cout << "mcIdRec=" << mcIdRec << " ";
	  if (mcIdRec == -1){
      	  fEvNofGhosts++;
      	  continue;
      }

	  CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(mcIdRec);
	  if (mcTrack == NULL) continue;
//	  std::cout << "mcTrack->GetPdgCode()=" << mcTrack->GetPdgCode() << std::endl;
	  Int_t nofPoints = mcTrack->GetNPoints(fDetId);
	  Int_t nofHits = recTrack->GetNHits();
	  Int_t nofTrue = recMatch->GetNofTrueHits();
	  Double_t qualiNofHits = Double_t(nofHits) / Double_t(nofPoints);
	  Double_t quali = Double_t(nofTrue) / Double_t(nofHits);
	  fhNofGoodHits->Fill(nofTrue);
	  fhNofBadHits->Fill(nofHits - nofTrue);
	  fhNofHits->Fill(nofHits);
	  if (quali >= fQuota && qualiNofHits >= 0.0) { //
		  fMcRecMap.insert(std::pair<Int_t, Int_t>(mcIdRec, iRec));
		  FillTrackParams(recTrack, "true");
	  } else {
		  fhNhGhosts->Fill(nofHits);
		  fEvNofGhosts++;
		  FillTrackParams(recTrack, "ghost");
	  }
  } // Loop over rec tracks
}

void CbmLitRecQa::ProcessTrdTracks()
{
  fMcRecMap.clear();
  fEvNofRecTracks = fRecTracks->GetEntriesFast();
  for (Int_t iRec = 0; iRec < fEvNofRecTracks; iRec++) {

	  CbmTrdTrack* recTrack = (CbmTrdTrack*) fRecTracks->At(iRec);
	  CbmTrackMatch* recMatch = (CbmTrackMatch*) fRecMatches->At(iRec);
	  if (recTrack == NULL || recMatch == NULL) continue;
	  Int_t mcIdRec = recMatch->GetMCTrackId();

	  if (mcIdRec == -1){
      	  fEvNofGhosts++;
      	  continue;
      }

	  CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(mcIdRec);
	  if (mcTrack == NULL) continue;
	  Int_t nofPoints = mcTrack->GetNPoints(fDetId);
	  Int_t nofHits = recTrack->GetNofTrdHits();
	  Int_t nofTrue = recMatch->GetNofTrueHits();
	  Double_t qualiNofHits = Double_t(nofHits) / Double_t(nofPoints);
	  Double_t quali = Double_t(nofTrue) / Double_t(nofHits);
	  fhNofGoodHits->Fill(nofTrue);
	  fhNofBadHits->Fill(nofHits - nofTrue);
	  fhNofHits->Fill(nofHits);
	  //fNofTrue[nofTrue]++;
	  //if (nofTrue > 8) { //
	  if (quali >= fQuota && qualiNofHits >= 0.0) { //
		  fMcRecMap.insert(std::pair<Int_t, Int_t>(mcIdRec, iRec));
		//  FillTrackParams(recTrack, "true");
	  } else {
		  fhNhGhosts->Fill(nofHits);
		  fEvNofGhosts++;
		//  FillTrackParams(recTrack, "ghost");
	  }
  } // Loop over rec tracks
}

void CbmLitRecQa::ProcessMcTracks()
{
  fEvNofMcTracks = fMCTracks->GetEntriesFast();
  for (Int_t iMCTrack = 0; iMCTrack < fEvNofMcTracks; iMCTrack++) {
    CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(iMCTrack);
    if (mcTrack == NULL) continue;

    Int_t nofPoints = mcTrack->GetNPoints(fDetId);
    if (nofPoints < fMinPoints) continue;

    Bool_t woSts = false;
    if (fNormType == 2) {
        if (fMcStsMap.find(iMCTrack) == fMcStsMap.end()) {
        	woSts = true;
        	//continue;
        }
    }

    TVector3 vertex;
    mcTrack->GetStartVertex(vertex);

    Bool_t isPrim = mcTrack->GetMotherId() == -1;
    Double_t mom = mcTrack->GetP();
    Bool_t isRef = (mom > 2. && nofPoints > 8);
    Bool_t isMuon = std::abs(mcTrack->GetPdgCode()) == 13;
    Bool_t isElectron = std::abs(mcTrack->GetPdgCode()) == 11;

    if (!woSts) {
	    fEvNofAccAll++;
	    if (isPrim && isMuon) {
	    	fEvNofAccMuons++;
	    	fhMomAccMuons->Fill(mom);
	    	fhNpAccMuons->Fill(Double_t(nofPoints));
	    }
	    if (isPrim && isElectron) {
	    	fEvNofAccElectrons++;
	    	fhMomAccElectrons->Fill(mom);
	    	fhNpAccElectrons->Fill(Double_t(nofPoints));
	    }
	    // Fill histograms for accepted tracks
	    fhMomAccAll->Fill(mom);
	    fhNpAccAll->Fill(Double_t(nofPoints));
	    fhMomNhAccAll->Fill(mom, Double_t(nofPoints));
	    if (isPrim) {
	      fEvNofAccPrim++;
	      fhMomAccPrim->Fill(mom);
	      fhNpAccPrim->Fill(Double_t(nofPoints));
	      if (isRef) {
	    	  fEvNofAccRef++;
	          fhMomAccRef->Fill(mom);
	          fhNpAccRef->Fill(Double_t(nofPoints));
	      }
	    } else {
	      fEvNofAccSec++;
	      fhMomAccSec->Fill(mom);
	      fhNpAccSec->Fill(Double_t(nofPoints));
	    }
    } else {

    }
    if (fMcRecMap.find(iMCTrack) != fMcRecMap.end() ) {

    	if (!woSts) {
	      if (IsMismatch(iMCTrack)) {
	    	  fEvNofMismatches++;
	   	      fhMomMismatches->Fill(mom);
	    	  fhNpMismatches->Fill(Double_t(nofPoints));
	    	//  continue;
	      }

	      // Fill histograms for reconstructed tracks
	      fEvNofRecAll++;
	      if (isPrim && isMuon) {
	    	  fEvNofRecMuons++;
	    	  fhMomRecMuons->Fill(mom);
	    	  fhNpRecMuons->Fill(Double_t(nofPoints));
	      }
	      if (isPrim && isElectron) {
	    	  fEvNofRecElectrons++;
	    	  fhMomRecElectrons->Fill(mom);
	    	  fhNpRecElectrons->Fill(Double_t(nofPoints));
	      }
	      fhMomRecAll->Fill(mom);
	      fhNpRecAll->Fill(Double_t(nofPoints));
	      fhMomNhRecAll->Fill(mom, Double_t(nofPoints));
	      if (isPrim) {
	         fEvNofRecPrim++;
	         fhMomRecPrim->Fill(mom);
	         fhNpRecPrim->Fill(Double_t(nofPoints));
	         if (isRef) {
	        	 fEvNofRecRef++;
	        	 fhMomRecRef->Fill(mom);
	        	 fhNpRecRef->Fill(Double_t(nofPoints));
	         }
	      } else {
	         fEvNofRecSec++;
	         fhMomRecSec->Fill(mom);
	         fhNpRecSec->Fill(Double_t(nofPoints));
	      }
    	} else {
    		fEvNofRecTracksNoSts++;
    	}
    }
  } // Loop over MCTracks

  if (fEvNofAccMuons == 2) fEvNofAccMuonPairs++;
  if (fEvNofRecMuons == 2) fEvNofRecMuonPairs++;

}

Bool_t CbmLitRecQa::IsMismatch(
		Int_t mcId)
{
    typedef std::multimap<Int_t, Int_t>::iterator Iterator;
    std::pair<Iterator, Iterator> bounds = fMcRecMap.equal_range(mcId);
    std::pair<Iterator, Iterator> boundsSts = fMcStsMap.equal_range(mcId);

    for (Iterator i = bounds.first; i != bounds.second; i++) {
    	Int_t stsId;
    	if (fDetId == kMUCH) {
	    	CbmMuchTrack* track = (CbmMuchTrack*) fRecTracks->At((*i).second);
	    	if (track == NULL) return true;
	    	stsId = track->GetStsTrackID();
    	} else
    	if (fDetId == kTRD) {
    		CbmTrdTrack* track = (CbmTrdTrack*) fRecTracks->At((*i).second);
    		if (track == NULL) return true;
    		stsId = track->GetStsTrackIndex();
    	}

    	for (Iterator iSts = boundsSts.first; iSts != boundsSts.second; iSts++) {
    		if (stsId == (*iSts).second) return false;
    	}
    }
    return true;
}

void CbmLitRecQa::CountClones()
{
	typedef std::multimap<Int_t, Int_t>::iterator Iterator;
	for (Iterator i = fMcRecMap.begin(); i != fMcRecMap.end(); ){
		Int_t count = fMcRecMap.count((*i).first);
		//std::cout << (*i).first << " " << (*i).second << " " << count << std::endl;
		if (count > 1) {
			fEvNofClones += count - 1;
			//fhNhClones->Fill();
		}
		for (Int_t j = 0; j < count; j++) i++;
	}
}

void CbmLitRecQa::FillTrackParams(
		CbmMuchTrack* track,
		const std::string& type)
{
	Double_t radialPos = std::sqrt(
	    	track->GetMuchTrack()->GetX()*track->GetMuchTrack()->GetX() +
			track->GetMuchTrack()->GetY()*track->GetMuchTrack()->GetY());
	Double_t chi2 = track->GetChi2() / track->GetNDF();
	Double_t p = std::abs(1. / track->GetMuchTrack()->GetQp());
	Int_t nofHits = track->GetNHits();
	Int_t lastPlaneId = track->GetFlag();
	if (type == "true") {
		fhChi2True->Fill(chi2);
		fhMomTrue->Fill(p);
		fhNofHitsTrue->Fill(nofHits);
		fhRadialPosTrue->Fill(radialPos);
		fhLastPlaneIdTrue->Fill(lastPlaneId);
		fhMomChi2True->Fill(p, chi2);
	} else if (type == "ghost"){
		fhChi2Ghost->Fill(chi2);
		fhMomGhost->Fill(p);
		fhNofHitsGhost->Fill(nofHits);
		fhRadialPosGhost->Fill(radialPos);
		fhLastPlaneIdGhost->Fill(lastPlaneId);
		fhMomChi2Ghost->Fill(p, chi2);
	}
//	Int_t type_int;
//	if (type == "ghost") type_int = -1; else type_int = 1;
//	fNN << chi2 << " " << p << " " << nofHits << " "
//	    << radialPos << " " << lastPlaneId << " "
//	    << type_int << std::endl;
}

void CbmLitRecQa::FillTrackParams(
		CbmTrdTrack* track,
		const std::string& type)
{
	Double_t radialPos = std::sqrt(
	    	track->GetParamLast()->GetX()*track->GetParamLast()->GetX() +
			track->GetParamLast()->GetY()*track->GetParamLast()->GetY());
	Double_t chi2 = track->GetChi2() / track->GetNDF();
	Double_t p = std::abs(1. / track->GetParamLast()->GetQp());
	Int_t nofHits = track->GetNofTrdHits();
	Int_t lastPlaneId = track->GetFlag();
	if (type == "true") {
		fhChi2True->Fill(chi2);
		fhMomTrue->Fill(p);
		fhNofHitsTrue->Fill(nofHits);
		fhRadialPosTrue->Fill(radialPos);
		fhLastPlaneIdTrue->Fill(lastPlaneId);
		fhMomChi2True->Fill(p, chi2);
	} else if (type == "ghost"){
		fhChi2Ghost->Fill(chi2);
		fhMomGhost->Fill(p);
		fhNofHitsGhost->Fill(nofHits);
		fhRadialPosGhost->Fill(radialPos);
		fhLastPlaneIdGhost->Fill(lastPlaneId);
		fhMomChi2Ghost->Fill(p, chi2);
	}
//	Int_t type_int;
//	if (type == "ghost") type_int = -1; else type_int = 1;
//	fNN << chi2 << " " << p << " " << nofHits << " "
//	    << radialPos << " " << lastPlaneId << " "
//	    << type_int << std::endl;
}

void CbmLitRecQa::CreateHistos()
{
  // Histogram list
  fHistoList = new TList();

  // Momentum distributions
  Double_t minMom   =  0.;
  Double_t maxMom   = 25.;
  Int_t    nBinsMom = 50;

  fhMomAccAll = new TH1F("hMomAccAll", "all accepted tracks",
                           nBinsMom, minMom, maxMom);
  fhMomRecAll = new TH1F("hMomRecAll", "all reconstructed tracks",
                           nBinsMom, minMom, maxMom);
  fhMomEffAll = new TH1F("hMomEffAll", "efficiency all tracks",
                           nBinsMom, minMom, maxMom);

  fhMomAccRef = new TH1F("hMomAccRef", "ref accepted tracks",
                           nBinsMom, minMom, maxMom);
  fhMomRecRef = new TH1F("hMomRecRef", "ref reconstructed tracks",
                           nBinsMom, minMom, maxMom);
  fhMomEffRef = new TH1F("hMomEffRef", "efficiency ref tracks",
                           nBinsMom, minMom, maxMom);

  fhMomAccPrim = new TH1F("hMomAccPrim", "accepted vertex tracks",
                           nBinsMom, minMom, maxMom);
  fhMomRecPrim = new TH1F("hMomRecPrim", "reconstructed vertex tracks",
                           nBinsMom, minMom, maxMom);
  fhMomEffPrim = new TH1F("hMomEffPrim", "efficiency vertex tracks",
                           nBinsMom, minMom, maxMom);

  fhMomAccSec  = new TH1F("hMomAccSec", "accepted non-vertex tracks",
                           nBinsMom, minMom, maxMom);
  fhMomRecSec  = new TH1F("hMomRecSec", "reconstructed non-vertex tracks",
                           nBinsMom, minMom, maxMom);
  fhMomEffSec  = new TH1F("hMomEffSec", "efficiency non-vertex tracks",
                           nBinsMom, minMom, maxMom);

  fhMomAccMuons  = new TH1F("hMomAccMuons", "accepted primary muon tracks",
                           nBinsMom, minMom, maxMom);
  fhMomRecMuons  = new TH1F("hMomRecMuons", "reconstructed primary muon tracks",
                           nBinsMom, minMom, maxMom);
  fhMomEffMuons  = new TH1F("hMomEffMuons", "efficiency primary muon tracks",
                           nBinsMom, minMom, maxMom);

  fhMomAccElectrons  = new TH1F("hMomAccElectrons", "accepted primary electron tracks",
                           nBinsMom, minMom, maxMom);
  fhMomRecElectrons  = new TH1F("hMomRecElectrons", "reconstructed primary electron tracks",
                           nBinsMom, minMom, maxMom);
  fhMomEffElectrons  = new TH1F("hMomEffElectrons", "efficiency primary electron tracks",
                           nBinsMom, minMom, maxMom);

  fHistoList->Add(fhMomAccAll);
  fHistoList->Add(fhMomRecAll);
  fHistoList->Add(fhMomEffAll);
  fHistoList->Add(fhMomAccRef);
  fHistoList->Add(fhMomRecRef);
  fHistoList->Add(fhMomEffRef);
  fHistoList->Add(fhMomAccPrim);
  fHistoList->Add(fhMomRecPrim);
  fHistoList->Add(fhMomEffPrim);
  fHistoList->Add(fhMomAccSec);
  fHistoList->Add(fhMomRecSec);
  fHistoList->Add(fhMomEffSec);
  fHistoList->Add(fhMomAccMuons);
  fHistoList->Add(fhMomRecMuons);
  fHistoList->Add(fhMomEffMuons);
  fHistoList->Add(fhMomAccElectrons);
  fHistoList->Add(fhMomRecElectrons);
  fHistoList->Add(fhMomEffElectrons);

  // Number-of-points distributions
  Double_t minNp   = -0.5;
  Double_t maxNp   = 18.5;
  Int_t    nBinsNp = 19;

  fhNpAccAll = new TH1F("hNpAccAll", "all accepted tracks",
                           nBinsNp, minNp, maxNp);
  fhNpRecAll = new TH1F("hNpRecAll", "all reconstructed tracks",
                           nBinsNp, minNp, maxNp);
  fhNpEffAll = new TH1F("hNpEffAll", "efficiency all tracks",
                           nBinsNp, minNp, maxNp);

  fhNpAccRef = new TH1F("hNpAccRef", "ref accepted tracks",
                           nBinsNp, minNp, maxNp);
  fhNpRecRef = new TH1F("hNpRecRef", "ref reconstructed tracks",
                           nBinsNp, minNp, maxNp);
  fhNpEffRef = new TH1F("hNpEffRef", "efficiency ref tracks",
                           nBinsNp, minNp, maxNp);

  fhNpAccPrim = new TH1F("hNpAccPrim", "accepted vertex tracks",
                           nBinsNp, minNp, maxNp);
  fhNpRecPrim = new TH1F("hNpRecPrim", "reconstructed vertex tracks",
                           nBinsNp, minNp, maxNp);
  fhNpEffPrim = new TH1F("hNpEffPrim", "efficiency vertex tracks",
                           nBinsNp, minNp, maxNp);

  fhNpAccSec  = new TH1F("hNpAccSec", "accepted non-vertex tracks",
                           nBinsNp, minNp, maxNp);
  fhNpRecSec  = new TH1F("hNpRecSec", "reconstructed non-vertex tracks",
                           nBinsNp, minNp, maxNp);
  fhNpEffSec  = new TH1F("hNpEffSec", "efficiency non-vertex tracks",
                           nBinsNp, minNp, maxNp);

  fhNpAccMuons  = new TH1F("hNpAccMuons", "accepted muon tracks",
                           nBinsNp, minNp, maxNp);
  fhNpRecMuons  = new TH1F("hNpRecMuons", "reconstructed muon tracks",
                           nBinsNp, minNp, maxNp);
  fhNpEffMuons  = new TH1F("hNpEffMuons", "efficiency muon tracks",
                           nBinsNp, minNp, maxNp);

  fhNpAccElectrons  = new TH1F("hNpAccElectrons", "accepted electron tracks",
                           nBinsNp, minNp, maxNp);
  fhNpRecElectrons  = new TH1F("hNpRecElectrons", "reconstructed electron tracks",
                           nBinsNp, minNp, maxNp);
  fhNpEffElectrons  = new TH1F("hNpEffElectron", "efficiency electron tracks",
                           nBinsNp, minNp, maxNp);

  fHistoList->Add(fhNpAccAll);
  fHistoList->Add(fhNpRecAll);
  fHistoList->Add(fhNpEffAll);
  fHistoList->Add(fhNpAccRef);
  fHistoList->Add(fhNpRecRef);
  fHistoList->Add(fhNpEffRef);
  fHistoList->Add(fhNpAccPrim);
  fHistoList->Add(fhNpRecPrim);
  fHistoList->Add(fhNpEffPrim);
  fHistoList->Add(fhNpAccSec);
  fHistoList->Add(fhNpRecSec);
  fHistoList->Add(fhNpEffSec);
  fHistoList->Add(fhNpAccMuons);
  fHistoList->Add(fhNpRecMuons);
  fHistoList->Add(fhNpEffMuons);
  fHistoList->Add(fhNpAccElectrons);
  fHistoList->Add(fhNpRecElectrons);
  fHistoList->Add(fhNpEffElectrons);

  // Number-of-hit distributions
  fhNhClones  = new TH1F("hNhClones", "number of hits for clones",
                           nBinsNp, minNp, maxNp);
  fhNhGhosts  = new TH1F("hNhGhosts", "number of hits for ghosts",
                           nBinsNp, minNp, maxNp);
  fHistoList->Add(fhNhClones);
  fHistoList->Add(fhNhGhosts);

  // mismatches
  fhMomMismatches = new TH1F("hMomMismatches", "mismatches",
          nBinsMom, minMom, maxMom);
  fhMomEffMismatches  = new TH1F("hMomEffMismatches", "efficiency mismatches",
                           nBinsMom, minMom, maxMom);
  fhNpMismatches = new TH1F("hNpMismatches", "mismatches",
          nBinsNp, minNp, maxNp);
  fhNpEffMismatches = new TH1F("hNpEffMismatches", "efficiency mismatches",
          nBinsNp, minNp, maxNp);

  fHistoList->Add(fhMomMismatches);
  fHistoList->Add(fhMomEffMismatches);
  fHistoList->Add(fhNpMismatches);
  fHistoList->Add(fhNpEffMismatches);

   // Number of hit and momentum distributions
  fhMomNhAccAll = new TH2D("hMomNhAccAll", "momentum vs. number of hits",
		               nBinsMom, minMom, maxMom, nBinsNp, minNp, maxNp);
  fhMomNhRecAll = new TH2D("hMomNhRecAll", "momentum vs. number of hits",
                       nBinsMom, minMom, maxMom, nBinsNp, minNp, maxNp);
  fhMomNhEffAll = new TH2D("hMomNhEffAll", "efficiency vs. momentum and number of hits",
                       nBinsMom, minMom, maxMom, nBinsNp, minNp, maxNp);
  fHistoList->Add(fhMomNhAccAll);
  fHistoList->Add(fhMomNhRecAll);
  fHistoList->Add(fhMomNhEffAll);

  //histograms for true and ghost tracks
  Double_t minChi2   =  0.;
  Double_t maxChi2   = 20.;
  Int_t nBinsChi2 = 100;
  Int_t minNofHits = 0;
  Int_t maxNofHits = 50;
  Int_t nBinsNofHits = 50;
  Int_t nBinsRP = 100;
  Double_t minRP = 0.;
  Double_t maxRP = 400;
  fhChi2True = new TH1F("hChi2True", "chi2 of true tracks",
           nBinsChi2, minChi2, maxChi2);
  fhChi2Ghost = new TH1F("hChi2Ghost", "chi2 of ghost tracks",
           nBinsChi2, minChi2, maxChi2);
  fhMomTrue = new TH1F("hMomTrue", "momentum of true tracks",
          100, 0., 10.);
  fhMomGhost = new TH1F("hMomGhost", "momentum of ghost tracks",
          100, 0., 10);
  fhNofHitsTrue = new TH1F("hNofHitsTrue", "nof hits for true tracks",
          nBinsNofHits, minNofHits, maxNofHits);
  fhNofHitsGhost= new TH1F("hNofHitsGhost", "nof hits for ghost tracks",
          nBinsNofHits, minNofHits, maxNofHits);
  fhRadialPosTrue = new TH1F("hRadialPosTrue", "radial position for true tracks",
          nBinsRP, minRP, maxRP);
  fhRadialPosGhost = new TH1F("hRadialPosGhost", "radial position for ghost tracks",
          nBinsRP, minRP, maxRP);

  fhLastPlaneIdTrue = new TH1F("hLastPlaneIdTrue", "last plane id for true tracks",
		  nBinsNofHits, minNofHits, maxNofHits);
  fhLastPlaneIdGhost = new TH1F("hLastPlaneIdGhost", "last plane id for ghost tracks",
		  nBinsNofHits, minNofHits, maxNofHits);

  fhMomChi2True = new TH2D("hMomChi2True", "momentum vs. chi2 for true tracks",
          nBinsMom, minMom, maxMom, nBinsChi2, minChi2, maxChi2);
  fhMomChi2Ghost = new TH2D("hMomChi2Ghost", "momentum vs. chi2 for ghost tracks",
          nBinsMom, minMom, maxMom, nBinsChi2, minChi2, maxChi2);

  fHistoList->Add(fhChi2True);
  fHistoList->Add(fhChi2Ghost);
  fHistoList->Add(fhMomTrue);
  fHistoList->Add(fhMomGhost);
  fHistoList->Add(fhNofHitsTrue);
  fHistoList->Add(fhNofHitsGhost);
  fHistoList->Add(fhRadialPosTrue);
  fHistoList->Add(fhRadialPosGhost);
  fHistoList->Add(fhLastPlaneIdTrue);
  fHistoList->Add(fhLastPlaneIdGhost);
  fHistoList->Add(fhMomChi2True);
  fHistoList->Add(fhMomChi2Ghost);

  fhNofGoodHits = new TH1F("hNofGoodHits", "nof good hits in track",
          nBinsNofHits, minNofHits, maxNofHits);
  fhNofBadHits = new TH1F("hNofBadHits", "nof bad hits in track",
          nBinsNofHits, minNofHits, maxNofHits);
  fhNofHits = new TH1F("hNofHits", "nof hits in track",
          nBinsNofHits, minNofHits, maxNofHits);

  fHistoList->Add(fhNofGoodHits);
  fHistoList->Add(fhNofBadHits);
  fHistoList->Add(fhNofHits);
}

void CbmLitRecQa::DivideHistos(
		TH1* histo1,
		TH1* histo2,
		TH1* histo3)
{
	histo1->Sumw2();
	histo2->Sumw2();
	histo3->Divide(histo1, histo2, 1, 1, "B");
}

void CbmLitRecQa::ZeroGlobalCounters()
{
	fNofMcTracks = 0;
	fNofRecTracks = 0;
	fNofStsTracks = 0;
	fNofRecTracksNoSts = 0;
	fNofAccAll = 0;
	fNofAccRef = 0;
	fNofAccPrim = 0;
	fNofAccSec = 0;
	fNofAccMuons = 0;
	fNofAccMuonPairs = 0;
	fNofAccElectrons = 0;
	fNofRecAll = 0;
	fNofRecRef = 0;
	fNofRecPrim = 0;
	fNofRecSec = 0;
	fNofRecMuons = 0;
	fNofRecMuonPairs = 0;
	fNofRecElectrons = 0;
	fNofGhosts = 0;
	fNofClones = 0;
	fNofMismatches = 0;
	fNEvents = 0;
	//rates
    fRateMcTracks = 0;
    fRateStsTracks = 0;
    fRateRecTracks = 0;
    fRateRecTracksNoSts = 0;
    fRateRecAll = 0;
    fRateRecRef = 0;
    fRateRecPrim = 0;
    fRateRecSec = 0;
    fRateRecMuons = 0;
    fRateRecElectrons = 0;
    fRateAccAll = 0;
    fRateAccRef = 0;
    fRateAccPrim = 0;
    fRateAccSec = 0;
    fRateAccMuons = 0;
    fRateAccElectrons = 0;
    fRateGhosts = 0;
    fRateClones = 0;
    fRateMismatches = 0;
    //eff
    fEffAll = 0;
    fEffRef = 0;
    fEffPrim = 0;
    fEffSec = 0;
    fEffMuons = 0;
    fEffElectrons = 0;
    fEffGhosts = 0;
    fEffClones = 0;
    fEffMismatches = 0;
}

void CbmLitRecQa::ZeroEventCounters()
{
	fEvNofMcTracks = 0;
	fEvNofRecTracks = 0;
	fEvNofStsTracks = 0;
	fEvNofRecTracksNoSts = 0;
	fEvNofAccAll = 0;
	fEvNofAccRef = 0;
	fEvNofAccPrim = 0;
	fEvNofAccSec = 0;
	fEvNofAccMuons = 0;
	fEvNofAccMuonPairs = 0;
	fEvNofAccElectrons = 0;
	fEvNofRecAll = 0;
	fEvNofRecRef = 0;
	fEvNofRecPrim = 0;
	fEvNofRecSec = 0;
	fEvNofRecMuons = 0;
	fEvNofRecMuonPairs = 0;
	fEvNofRecElectrons = 0;
	fEvNofGhosts = 0;
	fEvNofClones = 0;
	fEvNofMismatches = 0;

	fEvEffAll = 0.;
	fEvEffRef = 0.;
	fEvEffPrim = 0.;
	fEvEffSec = 0.;
	fEvEffMuons = 0.;
	fEvEffElectrons = 0.;
	fEvEffGhosts = 0.;
	fEvEffClones = 0.;
	fEvEffMismatches = 0.;
}

void CbmLitRecQa::IncreaseCounters()
{
  fNofRecTracks += fEvNofRecTracks;
  fNofStsTracks += fEvNofStsTracks;
  fNofRecTracksNoSts += fEvNofRecTracksNoSts;
  fNofMcTracks += fEvNofMcTracks;
  fNofAccAll += fEvNofAccAll;
  fNofAccRef += fEvNofAccRef;
  fNofAccPrim += fEvNofAccPrim;
  fNofAccSec += fEvNofAccSec;
  fNofAccMuons += fEvNofAccMuons;
  fNofAccMuonPairs += fEvNofAccMuonPairs;
  fNofAccElectrons += fEvNofAccElectrons;
  fNofRecAll += fEvNofRecAll;
  fNofRecRef += fEvNofRecRef;
  fNofRecPrim += fEvNofRecPrim;
  fNofRecSec += fEvNofRecSec;
  fNofRecMuons += fEvNofRecMuons;
  fNofRecMuonPairs += fEvNofRecMuonPairs;
  fNofRecElectrons += fEvNofRecElectrons;
  fNofGhosts += fEvNofGhosts;
  fNofClones += fEvNofClones;
  fNofMismatches += fEvNofMismatches;

  fNEvents++;
}

void CbmLitRecQa::CalcEventEff()
{
  if (fEvNofAccAll > 0) {
	  fEvEffAll  = Double_t(fEvNofRecAll)  / Double_t(fEvNofAccAll);
	  fEvEffGhosts = Double_t(fEvNofGhosts) / Double_t(fEvNofAccAll);
  	  fEvEffClones = Double_t(fEvNofClones) / Double_t(fEvNofAccAll);
  }
  if (fEvNofAccRef > 0) fEvEffRef  = Double_t(fEvNofRecRef)  / Double_t(fEvNofAccRef);
  if (fEvNofAccPrim > 0) fEvEffPrim = Double_t(fEvNofRecPrim) / Double_t(fEvNofAccPrim);
  if (fEvNofAccSec > 0) fEvEffSec  = Double_t(fEvNofRecSec)  / Double_t(fEvNofAccSec);
  if (fEvNofAccMuons > 0) fEvEffMuons  = Double_t(fEvNofRecMuons)  / Double_t(fEvNofAccMuons);
  if (fEvNofAccElectrons > 0) fEvEffElectrons  = Double_t(fEvNofRecElectrons)  / Double_t(fEvNofAccElectrons);
  if (fEvNofRecAll > 0)fEvEffMismatches = Double_t(fEvNofMismatches) / Double_t(fEvNofRecAll*fEvEffAll);
}

void CbmLitRecQa::CalcEffAndRates()
{
  // Divide histograms for efficiency calculation
  DivideHistos(fhMomRecAll, fhMomAccAll, fhMomEffAll);
  DivideHistos(fhMomRecRef, fhMomAccRef, fhMomEffRef);
  DivideHistos(fhMomRecPrim, fhMomAccPrim, fhMomEffPrim);
  DivideHistos(fhMomRecSec, fhMomAccSec, fhMomEffSec);
  DivideHistos(fhMomRecMuons, fhMomAccMuons, fhMomEffMuons);
  DivideHistos(fhMomRecElectrons, fhMomAccElectrons, fhMomEffElectrons);
  DivideHistos(fhNpRecAll, fhNpAccAll, fhNpEffAll);
  DivideHistos(fhNpRecRef, fhNpAccRef, fhNpEffRef);
  DivideHistos(fhNpRecPrim, fhNpAccPrim, fhNpEffPrim);
  DivideHistos(fhNpRecSec, fhNpAccSec, fhNpEffSec);
  DivideHistos(fhNpRecMuons, fhNpAccMuons, fhNpEffMuons);
  DivideHistos(fhNpRecElectrons, fhNpAccElectrons, fhNpEffElectrons);
  DivideHistos(fhMomNhRecAll,fhMomNhAccAll,fhMomNhEffAll);
  DivideHistos(fhMomMismatches,fhMomRecAll,fhMomEffMismatches);
  DivideHistos(fhNpMismatches,fhNpRecAll,fhNpEffMismatches);

  // Normalise histos for clones and ghosts to one event
  if (fNEvents != 0) {
    fhNhClones->Scale(1./Double_t(fNEvents));
    fhNhGhosts->Scale(1./Double_t(fNEvents));
  }

  if (fNofAccAll != 0) {
	  fEffAll = Double_t(fNofRecAll)  / Double_t(fNofAccAll);
	  fEffGhosts = Double_t(fNofGhosts) / Double_t(fNofAccAll);
	  fEffClones = Double_t(fNofClones) / Double_t(fNofAccAll);
  }
  if (fNofAccRef != 0) fEffRef = Double_t(fNofRecRef)  / Double_t(fNofAccRef);
  if (fNofAccPrim != 0) fEffPrim = Double_t(fNofRecPrim) / Double_t(fNofAccPrim);
  if (fNofAccSec != 0) fEffSec = Double_t(fNofRecSec)  / Double_t(fNofAccSec);
  if (fNofAccMuons != 0) fEffMuons = Double_t(fNofRecMuons)  / Double_t(fNofAccMuons);
  if (fNofAccElectrons != 0) fEffElectrons = Double_t(fNofRecElectrons)  / Double_t(fNofAccElectrons);
  if (fNofRecAll != 0) fEffMismatches = Double_t(fNofMismatches) / Double_t(fNofRecAll*fEffAll);

  fRateMcTracks = Double_t(fNofMcTracks) / Double_t(fNEvents);
  fRateStsTracks = Double_t(fNofStsTracks) / Double_t(fNEvents);
  fRateRecTracks = Double_t(fNofRecTracks) / Double_t(fNEvents);
  fRateRecTracksNoSts = Double_t(fNofRecTracksNoSts) / Double_t(fNEvents);
  fRateRecAll = Double_t(fNofRecAll)  / Double_t(fNEvents);
  fRateRecRef = Double_t(fNofRecRef)  / Double_t(fNEvents);
  fRateRecPrim = Double_t(fNofRecPrim) / Double_t(fNEvents);
  fRateRecSec = Double_t(fNofRecSec)  / Double_t(fNEvents);
  fRateRecMuons = Double_t(fNofRecMuons) / Double_t(fNEvents);
  fRateRecElectrons = Double_t(fNofRecElectrons) / Double_t(fNEvents);
  fRateAccAll = Double_t(fNofAccAll)  / Double_t(fNEvents);
  fRateAccRef = Double_t(fNofAccRef)  / Double_t(fNEvents);
  fRateAccPrim = Double_t(fNofAccPrim) / Double_t(fNEvents);
  fRateAccSec = Double_t(fNofAccSec)  / Double_t(fNEvents);
  fRateAccMuons = Double_t(fNofAccMuons) / Double_t(fNEvents);
  fRateAccElectrons = Double_t(fNofAccElectrons) / Double_t(fNEvents);
  fRateGhosts = Double_t(fNofGhosts) / Double_t(fNEvents);
  fRateClones = Double_t(fNofClones) / Double_t(fNEvents);
  fRateMismatches = Double_t(fNofMismatches) / Double_t(fNEvents);
}

void CbmLitRecQa::PrintEventStatistic()
{
  // Event summary
  if (fVerbose > 0) {
    std::cout << "-------CbmLitRecQa : Event summary-------" << std::endl;
    std::cout << "MCTracks   : " << fEvNofMcTracks << ", acc: " << fEvNofAccAll
    	<< ", rec: " << fEvNofRecAll << std::endl;
    std::cout << "Reconstructed STS tracks: " << fEvNofStsTracks << std::endl;
    std::cout << "Reconstructed tracks: " << fEvNofRecTracks << std::endl;
    std::cout << "Efficiency: " << std::endl;
    std::cout << "all: acc: " << fEvNofAccAll << ", rec: "
    	<< fEvNofRecAll << ", efficiency " << fEvEffAll*100. << "%" << std::endl;
    std::cout << "ref: acc: " << fEvNofAccRef << ", rec: "
    	<< fEvNofRecRef << ", efficiency " << fEvEffRef*100. << "%" << std::endl;
    std::cout << "prim: acc: " << fEvNofAccPrim << ", rec: "
         << fEvNofRecPrim << ", efficiency " << fEvEffPrim*100. << "%" << std::endl;
    std::cout << "sec: acc: " << fEvNofAccSec << ", rec: "
    	<< fEvNofRecSec << ", efficiency " << fEvEffSec*100. << "%" << std::endl;
    std::cout << "muons: acc: " << fEvNofAccMuons << ", rec: "
        << fEvNofRecMuons << ", efficiency " << fEvEffMuons*100. << "%" << std::endl;
    std::cout << "electrons: acc: " << fEvNofAccElectrons << ", rec: "
        << fEvNofRecElectrons << ", efficiency " << fEvEffElectrons*100. << "%" << std::endl;
    std::cout << "mismatches: " << fEvNofMismatches
    	<< ", efficiency " << fEvEffMismatches*100. << "%" << std::endl;
    std::cout << "ghosts: " << fEvNofGhosts << ", ghosts/acc MC tracks: "
    	<< fEvEffGhosts*100. << "%" << std::endl;
    std::cout << "clones: " << fEvNofClones << ", clones/accepted MC tracks: "
    	<< fEvEffClones*100. << "%" << std::endl;
    std::cout << "rec track w/o rec STS track: " << fEvNofRecTracksNoSts << std::endl;
    std::cout << "muon pairs: acc:" << fEvNofAccMuonPairs
    	<< ", rec:" << fEvNofRecMuonPairs << std::endl;
    std::cout << "-----------------------------------------------" << std::endl;
  }
}

void CbmLitRecQa::PrintStatistic()
{
	std::cout.precision(4);
  // Run summary to screen
  std::cout << "-------CbmLitRecQa: Run summary-------" << std::endl;
  std::cout << "MCTracks   : " << fNofMcTracks << ", acc: " << fNofAccAll
    	<< ", rec: " << fNofRecAll << std::endl;
  std::cout << "Rec STS tracks: " << fNofStsTracks << ", per event: " << fRateStsTracks << std::endl;
  std::cout << "Rec tracks: " << fNofRecTracks << ", per event: " << fRateRecTracks << std::endl;
  std::cout << "Efficiency: " << std::endl;
  std::cout << "all: " << fEffAll*100 << " % (" << fNofRecAll << "/" << fNofAccAll
  	<<"), per event (" << fRateRecAll << "/" << fRateAccAll << ")" << std::endl;
  std::cout << "ref: " << fEffRef*100 << " % (" << fNofRecRef << "/" << fNofAccRef
   	<<"), per event (" << fRateRecRef << "/" << fRateAccRef << ")" << std::endl;
  std::cout << "prim: " << fEffPrim*100 << " % (" << fNofRecPrim << "/" << fNofAccPrim
  	<<"), per event (" << fRateRecPrim << "/" << fRateAccPrim << ")" << std::endl;
  std::cout << "sec: " << fEffSec*100 << " % (" << fNofRecSec << "/" << fNofAccSec
  	<<"), per event (" << fRateRecSec << "/" << fRateAccSec << ")" << std::endl;
  std::cout << "muons: " << fEffMuons*100 << " % (" << fNofRecMuons << "/" << fNofAccMuons
   	<<"), per event (" << fRateRecMuons << "/" << fRateAccMuons << ")" << std::endl;
  std::cout << "electrons: " << fEffElectrons*100 << " % (" << fNofRecElectrons << "/" << fNofAccElectrons
   	<<"), per event (" << fRateRecElectrons << "/" << fRateAccElectrons << ")" << std::endl;
  std::cout << "mismatches: " << fEffMismatches*100 << " % (" << fNofMismatches << "/" << fEffAll*fNofRecAll
     	<<"), per event (" << fRateMismatches << "/" << fEffAll * fRateRecAll << ")" << std::endl;
  std::cout << "ghosts: " << fRateGhosts << " per event, " << "ghosts/accepted MC tracks :"
  	<< fEffGhosts*100 << "% (" << fNofGhosts << "/" << fNofAccAll << ")" << std::endl;
  std::cout << "clone: " << fRateClones << " per event, " << "clones/accepted MC tracks :"
  	<< fEffClones*100 << "% (" << fNofClones << "/" << fNofAccAll << ")" << std::endl;
  std::cout << "rec track w/o rec STS track: " << fRateRecTracksNoSts << " per event, "
  	<< "total = " << fNofRecTracksNoSts << std::endl;
  std::cout << "muon pairs: acc:" << fNofAccMuonPairs
  	<< ", rec:" << fNofRecMuonPairs << std::endl;
  std::cout << "Events analysed : " << fNEvents << std::endl;
  std::cout << "-------------------------------------------------" << std::endl;
}

void CbmLitRecQa::WriteToFile()
{
	//  TDirectory *olddir = gDirectory;
	//  TDirectory *hdir = new TDirectory("MuchFinderQa",
	//				    "Performance of the track finding in Much");
	//  hdir->cd();
	TIter next(fHistoList);
	while ( TH1* histo = ((TH1*)next()) ) histo->Write();
	//  olddir->cd();
}



ClassImp(CbmLitRecQa)
