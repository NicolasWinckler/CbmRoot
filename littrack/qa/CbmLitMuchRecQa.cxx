// -------------------------------------------------------------------------
// -----                  CbmLitMuchRecQa source file               -----
// -----                  Created 15/10/07  by A. Lebedev               -----
// -------------------------------------------------------------------------

#include "CbmLitMuchRecQa.h"

#include "CbmMuchTrack.h"
#include "CbmMuchTrackMatch.h"
#include "CbmMuchHit.h"

#include "CbmStsTrack.h"
#include "CbmStsTrackMatch.h"
#include "CbmTrackParam.h"
#include "CbmMCPoint.h"
#include "CbmMCTrack.h"
#include "CbmRootManager.h"
#include "CbmRunAna.h"
#include "CbmRuntimeDb.h"

#include "TClonesArray.h"
#include "TDirectory.h"
#include "TH1F.h"
#include "TList.h"
#include "TVector3.h"

#include <iostream>
#include <map>


CbmLitMuchRecQa::CbmLitMuchRecQa():
  CbmTask("MuchRecQA", 1),
  fMinPoints(15),
  fQuota(0.7),
  fNormType(1)
{
	ZeroGlobalCounters();
}

CbmLitMuchRecQa::CbmLitMuchRecQa(
		Int_t minPoints, 
		Double_t quota,
		Int_t iVerbose):
  CbmTask("MuchRecQA", iVerbose),
  fMinPoints(minPoints),
  fQuota(quota),
  fNormType(1) 
{
	ZeroGlobalCounters();
}

CbmLitMuchRecQa::~CbmLitMuchRecQa() 
{ 
  fHistoList->Delete();
  delete fHistoList;
}

void CbmLitMuchRecQa::SetParContainers() 
{

}

InitStatus CbmLitMuchRecQa::Init() 
{
    CbmRootManager* ioman = CbmRootManager::Instance();
    if (NULL == ioman) 
    	Fatal("CbmLitMuchRecQa::Init","CbmRootManager is not instantiated");

    fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
    if (NULL == fMCTracks ) 
    	Fatal("CbmLitMuchRecQa::Init","No MCTrack array!");
    
    fMCPoints = (TClonesArray*) ioman->GetObject("MuchPoint");
    if (NULL == fMCTracks ) 
    	Fatal("CbmLitMuchRecQa::Init","No MCTrack array!");
    
    fMuchHits = (TClonesArray*) ioman->GetObject("MuchHit");
    if (NULL == fMuchHits) 
    	Fatal("CbmLitMuchRecQa::Init", "No fMuchHits array!");
    
    if (fNormType == 2) {
    	fStsTracks = (TClonesArray*) ioman->GetObject("STSTrack");
    	if (NULL == fStsTracks)
    		Fatal("CbmLitMuchRecQa::Init","No STStrack array!");

    	fStsMatches = (TClonesArray*) ioman->GetObject("STSTrackMatch");
    	if (NULL == fStsMatches)
    		Fatal("CbmLitMuchRecQa::Init",": No STSTrackMatch array!");
    }

    fMuchTracks = (TClonesArray*) ioman->GetObject("MuchTrack");
    if (NULL == fMuchTracks )
    	Fatal("CbmLitMuchRecQa::Init","No MUCHTrack array!");
	  
    fMuchMatches = (TClonesArray*) ioman->GetObject("MuchTrackMatch");
    if (NULL == fMuchMatches )
    	Fatal("CbmLitMuchRecQa::Init","No MUCHTrackMatch array!");
	  
    CreateHistos();

    std::cout << "Minimum number of Much points : " << fMinPoints << std::endl;
    std::cout << "Matching quota : " << fQuota << std::endl;
    std::cout << "-----------------------------------------------" << std::endl;

    return kSUCCESS;
}

InitStatus CbmLitMuchRecQa::ReInit() 
{
	return kSUCCESS;
}

void CbmLitMuchRecQa::Exec(Option_t* opt) 
{

  ZeroEventCounters();
	
  ProcessStsTracks();
  
  ProcessMuchTracks();
  
  ProcessMcTracks();
  
  CountClones();
  
  CalcEventEff();
	  
  PrintEventStatistic();

  IncreaseCounters();
}

void CbmLitMuchRecQa::Finish() 
{
	CalcEffAndRates();

	PrintStatistic();

	WriteToFile();
}


void CbmLitMuchRecQa::ProcessStsTracks() 
{
  fMcStsMap.clear();
  if (fNormType == 2) { // Normalization to number of Sts AND Much tracks
	fEvNofStsTracks = fStsTracks->GetEntriesFast();
    for(Int_t iStsTrack = 0; iStsTrack < fEvNofStsTracks; iStsTrack++) {
      CbmStsTrackMatch* stsTrackM = (CbmStsTrackMatch*) fStsMatches->At(iStsTrack);
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
}

void CbmLitMuchRecQa::ProcessMuchTracks()
{
  fMcMuchMap.clear();
  fEvNofMuchTracks = fMuchTracks->GetEntriesFast();
  for (Int_t iRec = 0; iRec < fEvNofMuchTracks; iRec++) {
    CbmMuchTrack* muchTrack = (CbmMuchTrack*) fMuchTracks->At(iRec);
    if (muchTrack == NULL) continue;
    CbmMuchTrackMatch* muchMatch = (CbmMuchTrackMatch*) fMuchMatches->At(iRec);
    if (muchMatch == NULL) continue;
	Int_t mcIdMuch = muchMatch->GetMCTrackId();  
    
	if (mcIdMuch == -1){      
      fEvNofGhosts++;
      continue;
    } 

    CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(mcIdMuch);
	if (mcTrack == NULL) continue;
	
    Int_t nofPoints = mcTrack->GetNPoints(kMUCH);
	Int_t nofHits = muchTrack->GetNHits();  
    Int_t nofTrue = muchMatch->GetNofTrueHits();
    Double_t qualiNofHits = Double_t(nofHits) / Double_t(nofPoints);
    Double_t quali = Double_t(nofTrue) / Double_t(nofHits);
    if (quali >= fQuota && qualiNofHits >= 0.0) { // 
      fMcMuchMap.insert(std::pair<Int_t, Int_t>(mcIdMuch, iRec));      
    } else { 
      fhNhGhosts->Fill(nofHits);
	  fEvNofGhosts++;      
    }    
  } // Loop over MuchTracks
}

void CbmLitMuchRecQa::ProcessMcTracks()
{
  fEvNofMcTracks = fMCTracks->GetEntriesFast();
  for (Int_t iMCTrack = 0; iMCTrack < fEvNofMcTracks; iMCTrack++) {
    CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(iMCTrack);
    if (mcTrack == NULL) continue;
    
    Int_t nofPoints = mcTrack->GetNPoints(kMUCH);
    if (nofPoints < fMinPoints) continue; 

    if (fNormType == 2) {
        if (fMcStsMap.find(iMCTrack) == fMcStsMap.end()) continue;
    }
    
    TVector3 vertex;
    mcTrack->GetStartVertex(vertex);
        
    Bool_t isPrim = mcTrack->GetMotherId() == -1;
    Double_t mom = mcTrack->GetP();
    Bool_t isRef = (mom > 2. && nofPoints > 8);
    Bool_t isMuon = std::abs(mcTrack->GetPdgCode()) == 13;
     
    fEvNofAccAll++;
    if (isPrim && isMuon) {
    	fEvNofAccMuons++;
    	fhMomAccMuons->Fill(mom);
    	fhNpAccMuons->Fill(Double_t(nofPoints));
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

    if (fMcMuchMap.find(iMCTrack) != fMcMuchMap.end() ) {
      
      if (IsMismatch(iMCTrack)) {
    	  fEvNofMismatches++;
   	      fhMomMismatches->Fill(mom);
    	  fhNpMismatches->Fill(Double_t(nofPoints));
    	  //continue;
      }
      
      // Fill histograms for reconstructed tracks
      fEvNofRecAll++;
      if (isPrim && isMuon) {
    	  fEvNofRecMuons++; 
    	  fhMomRecMuons->Fill(mom);
    	  fhNpRecMuons->Fill(Double_t(nofPoints));
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
    }  
  } // Loop over MCTracks
}

Bool_t CbmLitMuchRecQa::IsMismatch(Int_t mcId)
{
    typedef std::multimap<Int_t, Int_t>::iterator Iterator;
    std::pair<Iterator, Iterator> bounds = fMcMuchMap.equal_range(mcId);
    std::pair<Iterator, Iterator> boundsSts = fMcStsMap.equal_range(mcId);

    for (Iterator i = bounds.first; i != bounds.second; i++) {
    	CbmMuchTrack* track = (CbmMuchTrack*) fMuchTracks->At((*i).second);
    	if (track == NULL) return true;
    	Int_t stsId = track->GetStsTrackID();
    	for (Iterator iSts = boundsSts.first; iSts != boundsSts.second; iSts++) {
    		if (stsId == (*iSts).second) return false;
    	}
    }
    return true;
}

void CbmLitMuchRecQa::CountClones()
{
	typedef std::multimap<Int_t, Int_t>::iterator Iterator;
	for (Iterator i = fMcMuchMap.begin(); i!=fMcMuchMap.end(); ){
		Int_t count = fMcMuchMap.count((*i).first);
		//std::cout << (*i).first << " " << (*i).second << " " << count << std::endl;
		if (count > 1) {
			fEvNofClones += count - 1;
			//fhNhClones->Fill();
		}
		for (Int_t j = 0; j < count; j++) i++;
	}
}

void CbmLitMuchRecQa::CreateHistos() 
{
  // Histogram list
  fHistoList = new TList();

  // Momentum distributions
  Double_t minMom   =  0.;
  Double_t maxMom   = 25.;
  Int_t    nBinsMom = 10;
  
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

  // Number-of-points distributions
  Double_t minNp   = -0.5;
  Double_t maxNp   = 15.5;
  Int_t    nBinsNp = 16;
  
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
                           nBinsNp, minNp, maxNp);;
  
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
}

void CbmLitMuchRecQa::DivideHistos(
		TH1* histo1,
		TH1* histo2,
		TH1* histo3) 
{ 
	histo1->Sumw2();
	histo2->Sumw2();
	histo3->Divide(histo1, histo2);
}

void CbmLitMuchRecQa::ZeroGlobalCounters()
{
	fNofMcTracks = 0;
	fNofMuchTracks = 0; 
	fNofStsTracks = 0; 
	fNofAccAll = 0;
	fNofAccRef = 0;
	fNofAccPrim = 0;
	fNofAccSec = 0;
	fNofAccMuons = 0;
	fNofRecAll = 0;
	fNofRecRef = 0;
	fNofRecPrim = 0;
	fNofRecSec = 0;
	fNofRecMuons = 0;
	fNofGhosts = 0;
	fNofClones = 0;
	fNofMismatches = 0;
	fNEvents = 0; 
	//rates
    fRateMcTracks = 0;
    fRateStsTracks = 0;
    fRateMuchTracks = 0;
    fRateRecAll = 0;
    fRateRecRef = 0;
    fRateRecPrim = 0;
    fRateRecSec = 0;
    fRateRecMuons = 0;
    fRateAccAll = 0;
    fRateAccRef = 0;
    fRateAccPrim = 0;
    fRateAccSec = 0;
    fRateAccMuons = 0;
    fRateGhosts = 0;
    fRateClones = 0;
    fRateMismatches = 0;
    //eff
    fEffAll = 0;
    fEffRef = 0;
    fEffPrim = 0;
    fEffSec = 0;
    fEffMuons = 0;
    fEffGhosts = 0;
    fEffClones = 0;
    fEffMismatches = 0;
}

void CbmLitMuchRecQa::ZeroEventCounters()
{
	fEvNofMcTracks = 0;
	fEvNofMuchTracks = 0; 
	fEvNofStsTracks = 0; 
	fEvNofAccAll = 0;
	fEvNofAccRef = 0;
	fEvNofAccPrim = 0;
	fEvNofAccSec = 0;
	fEvNofAccMuons = 0;
	fEvNofRecAll = 0;
	fEvNofRecRef = 0;
	fEvNofRecPrim = 0;
	fEvNofRecSec = 0;
	fEvNofRecMuons = 0;
	fEvNofGhosts = 0;
	fEvNofClones = 0;
	fEvNofMismatches = 0;
	
	fEvEffAll = 0.;
	fEvEffRef = 0.;
	fEvEffPrim = 0.;
	fEvEffSec = 0.;
	fEvEffMuons = 0.;
	fEvEffGhosts = 0.;
	fEvEffClones = 0.;
	fEvEffMismatches = 0.;
}

void CbmLitMuchRecQa::IncreaseCounters()
{
  fNofMuchTracks += fEvNofMuchTracks;
  fNofStsTracks += fEvNofStsTracks; 
  fNofMcTracks += fEvNofMcTracks;
  fNofAccAll += fEvNofAccAll; 
  fNofAccRef += fEvNofAccRef;
  fNofAccPrim += fEvNofAccPrim;
  fNofAccSec += fEvNofAccSec;
  fNofAccMuons += fEvNofAccMuons;
  fNofRecAll += fEvNofRecAll;
  fNofRecRef += fEvNofRecRef;
  fNofRecPrim += fEvNofRecPrim;
  fNofRecSec += fEvNofRecSec;
  fNofRecMuons += fEvNofRecMuons;
  fNofGhosts += fEvNofGhosts;
  fNofClones += fEvNofClones;
  fNofMismatches += fEvNofMismatches;
  fNEvents++;
}

void CbmLitMuchRecQa::CalcEventEff()
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
  if (fEvNofRecAll > 0)fEvEffMismatches = Double_t(fEvNofMismatches) / Double_t(fEvNofRecAll*fEvEffAll); 
}

void CbmLitMuchRecQa::CalcEffAndRates()
{
  // Divide histograms for efficiency calculation
  DivideHistos(fhMomRecAll, fhMomAccAll, fhMomEffAll);
  DivideHistos(fhMomRecRef, fhMomAccRef, fhMomEffRef);  
  DivideHistos(fhMomRecPrim, fhMomAccPrim, fhMomEffPrim);
  DivideHistos(fhMomRecSec, fhMomAccSec, fhMomEffSec);
  DivideHistos(fhMomRecMuons, fhMomAccMuons, fhMomEffMuons);  
  DivideHistos(fhNpRecAll, fhNpAccAll, fhNpEffAll);
  DivideHistos(fhNpRecRef, fhNpAccRef, fhNpEffRef);
  DivideHistos(fhNpRecPrim, fhNpAccPrim, fhNpEffPrim);
  DivideHistos(fhNpRecSec, fhNpAccSec, fhNpEffSec);
  DivideHistos(fhNpRecMuons, fhNpAccMuons, fhNpEffMuons);
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
  if (fNofRecAll != 0) fEffMismatches = Double_t(fNofMismatches) / Double_t(fNofRecAll*fEffAll);

  fRateMcTracks = Double_t(fNofMcTracks) / Double_t(fNEvents);
  fRateStsTracks = Double_t(fNofStsTracks) / Double_t(fNEvents);
  fRateMuchTracks = Double_t(fNofMuchTracks) / Double_t(fNEvents);
  fRateRecAll = Double_t(fNofRecAll)  / Double_t(fNEvents);
  fRateRecRef = Double_t(fNofRecRef)  / Double_t(fNEvents);
  fRateRecPrim = Double_t(fNofRecPrim) / Double_t(fNEvents);
  fRateRecSec = Double_t(fNofRecSec)  / Double_t(fNEvents);
  fRateRecMuons = Double_t(fNofRecMuons) / Double_t(fNEvents);
  fRateAccAll = Double_t(fNofAccAll)  / Double_t(fNEvents);
  fRateAccRef = Double_t(fNofAccRef)  / Double_t(fNEvents);
  fRateAccPrim = Double_t(fNofAccPrim) / Double_t(fNEvents);
  fRateAccSec = Double_t(fNofAccSec)  / Double_t(fNEvents);
  fRateAccMuons = Double_t(fNofAccMuons) / Double_t(fNEvents);
  fRateGhosts = Double_t(fNofGhosts) / Double_t(fNEvents);
  fRateClones = Double_t(fNofClones) / Double_t(fNEvents);
  fRateMismatches = Double_t(fNofMismatches) / Double_t(fNEvents); 
}

void CbmLitMuchRecQa::PrintEventStatistic()
{
  // Event summary
  if (fVerbose > 0) {
    std::cout << "-------CbmLitMuchRecQa : Event summary-------" << std::endl;
    std::cout << "MCTracks   : " << fEvNofMcTracks << ", acc: " << fEvNofAccAll
    	<< ", rec: " << fEvNofRecAll << std::endl;
    std::cout << "Reconstructed STS tracks: " << fEvNofStsTracks << std::endl;
    std::cout << "Reconstructed MuCh tracks: " << fEvNofMuchTracks << std::endl;
    std::cout << "Efficiency: " << std::endl;
    std::cout << "All: acc: " << fEvNofAccAll << ", rec: "
    	<< fEvNofRecAll << ", efficiency " << fEvEffAll*100. << "%" << std::endl;
    std::cout << "Ref: acc: " << fEvNofAccRef << ", rec: "
    	<< fEvNofRecRef << ", efficiency " << fEvEffRef*100. << "%" << std::endl;
    std::cout << "Prim: acc: " << fEvNofAccPrim << ", rec: "
         << fEvNofRecPrim << ", efficiency " << fEvEffPrim*100. << "%" << std::endl;
    std::cout << "Sec: acc: " << fEvNofAccSec << ", rec: "
    	<< fEvNofRecSec << ", efficiency " << fEvEffSec*100. << "%" << std::endl;
    std::cout << "Muons: acc: " << fEvNofAccMuons << ", rec: "
        << fEvNofRecMuons << ", efficiency " << fEvEffMuons*100. << "%" << std::endl;
    std::cout << "Mismatches: " << fEvNofMismatches 
    	<< ", efficiency " << fEvEffMismatches*100. << "%" << std::endl;
    std::cout << "Ghosts: " << fEvNofGhosts << ", ghosts/acc MC tracks: "
    	<< fEvEffGhosts*100. << "%" << std::endl;
    std::cout << "Clones: " << fEvNofClones << ", clones/accepted MC tracks: "
    	<< fEvEffClones*100. << "%" << std::endl;
    std::cout << "-----------------------------------------------" << std::endl;
  }
}

void CbmLitMuchRecQa::PrintStatistic()
{
  // Run summary to screen
  std::cout << "-------CbmLitMuchRecQa: Run summary-------" << std::endl;
  std::cout << "MCTracks   : " << fNofMcTracks << ", acc: " << fNofAccAll
    	<< ", rec: " << fNofRecAll << std::endl;
  std::cout << "Rec STS tracks: " << fNofStsTracks << ", per event: " << fRateStsTracks << std::endl;
  std::cout << "Rec MuCh tracks: " << fNofMuchTracks << ", per event: " << fRateMuchTracks << std::endl;
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
  std::cout << "mismatches: " << fEffMismatches*100 << " % (" << fNofMismatches << "/" << fEffAll*fNofRecAll 
     	<<"), per event (" << fRateMismatches << "/" << fEffAll * fRateRecAll << ")" << std::endl;
  std::cout << "ghosts: " << fRateGhosts << " per event, " << "ghosts/accepted MC tracks :" 
  	<< fEffGhosts*100 << "% (" << fNofGhosts << "/" << fNofAccAll << ")" << std::endl;
  std::cout << "clone: " << fRateClones << " per event, " << "clones/accepted MC tracks :" 
  	<< fEffClones*100 << "% (" << fNofClones << "/" << fNofAccAll << ")" << std::endl;
  std::cout << "Events analysed : " << fNEvents << std::endl;
  std::cout << "-------------------------------------------------" << std::endl;
}

void CbmLitMuchRecQa::WriteToFile()
{
	//  TDirectory *olddir = gDirectory;
	//  TDirectory *hdir = new TDirectory("MuchFinderQa",
	//				    "Performance of the track finding in Much");
	//  hdir->cd();
	TIter next(fHistoList);
	while ( TH1* histo = ((TH1*)next()) ) histo->Write();
	//  olddir->cd();
}

ClassImp(CbmLitMuchRecQa)
