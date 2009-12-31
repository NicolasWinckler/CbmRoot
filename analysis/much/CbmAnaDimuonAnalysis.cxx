/** CbmAnaDimuonAnalysis.cxx
 *@author E.Kryshen <e.kryshen@gsi.de>
 *@since 2009-09-11
 **/

#include "CbmAnaDimuonAnalysis.h"
#include "CbmAnaMuch.h"
#include "CbmMCTrack.h"
#include "CbmStsTrack.h"
#include "CbmMuchTrack.h"
#include "CbmMuchPoint.h"
#include "CbmTrackMatch.h"
#include "CbmMuchPixelHit.h"
#include "CbmAnaMuonCandidate.h"
#include "CbmGlobalTrack.h"
#include "CbmMuchGeoScheme.h"
#include "CbmMuchPixelHit.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"
#include "TMath.h"
#include "TH1D.h"
#include "TLegend.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TLorentzVector.h"

// -----   Default constructor   -------------------------------------------
CbmAnaDimuonAnalysis::CbmAnaDimuonAnalysis(){
  fEvent=0;
  fStsPointsAccQuota=4;
  fStsTrueHitQuota=0.7;
  fMuchPointsAccQuota=10;
  fMuchTrueHitQuota=0.7;
  fHistoFileName="histo.root";
  fMuPlusCandidates  = new TClonesArray("CbmAnaMuonCandidate",1);
  fMuMinusCandidates = new TClonesArray("CbmAnaMuonCandidate",1);
}
// -------------------------------------------------------------------------


// -----   Standard constructor   ------------------------------------------
CbmAnaDimuonAnalysis::CbmAnaDimuonAnalysis(const char* name,TString digiFileName,TString histoFileName)
:FairTask(name){
  fEvent=0;
  fStsPointsAccQuota=4;
  fStsTrueHitQuota=0.7;
  fMuchPointsAccQuota=10;
  fMuchTrueHitQuota=0.7;
  fHistoFileName=histoFileName;
  fMuPlusCandidates  = new TClonesArray("CbmAnaMuonCandidate",1);
  fMuMinusCandidates = new TClonesArray("CbmAnaMuonCandidate",1);
  fDigiFileName = digiFileName;
}

// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
CbmAnaDimuonAnalysis::~CbmAnaDimuonAnalysis(){
}
// -------------------------------------------------------------------------


// -----  SetParContainers -------------------------------------------------
void CbmAnaDimuonAnalysis::SetParContainers(){
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb = ana->GetRuntimeDb();

  rtdb->getContainer("FairBaseParSet");
  rtdb->getContainer("CbmGeoPassivePar");
  rtdb->getContainer("CbmGeoStsPar");
  rtdb->getContainer("CbmGeoMuchPar");
  rtdb->getContainer("CbmFieldPar");
}
// -------------------------------------------------------------------------


// -----   Public method Init (abstract in base class)  --------------------
InitStatus CbmAnaDimuonAnalysis::Init()
{
  // Get and check FairRootManager
  FairRootManager* fManager = FairRootManager::Instance();
  fMCTracks         = (TClonesArray*) fManager->GetObject("MCTrack");
  fStsPoints        = (TClonesArray*) fManager->GetObject("StsPoint");
  fMuchPoints       = (TClonesArray*) fManager->GetObject("MuchPoint");
  fMuchHits         = (TClonesArray*) fManager->GetObject("MuchPixelHit");
  fStsTracks        = (TClonesArray*) fManager->GetObject("StsTrack");
  fMuchTracks       = (TClonesArray*) fManager->GetObject("MuchTrack");
  fMuchTrackMatches = (TClonesArray*) fManager->GetObject("MuchTrackMatch");
  fStsTrackMatches  = (TClonesArray*) fManager->GetObject("StsTrackMatch");
  fGlobalTracks     = (TClonesArray*) fManager->GetObject("GlobalTrack");
  fEvent=0;
  
  if (!(fMCTracks&&fStsPoints&&fMuchPoints&&fMuchHits&&fStsTracks&&fMuchTracks&&fMuchTrackMatches&&fStsTrackMatches)){
    printf(" %i",fStsPoints);
    printf(" %i",fMuchPoints);
    printf(" %i",fStsTracks);
    printf(" %i",fMuchHits);
    printf(" %i",fMuchTracks);
    printf(" %i",fMuchTrackMatches);
    printf("\n");
    Fatal("Init","One of TCloneArrays not available");
  }  
  
  fManager->Register("MuPlusCandidates" ,"Much",fMuPlusCandidates,kTRUE);
  fManager->Register("MuMinusCandidates","Much",fMuMinusCandidates,kTRUE);
  
  fGeoScheme = CbmMuchGeoScheme::Instance();
  fGeoScheme->Init(fDigiFileName);

  return kSUCCESS;
}
// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void CbmAnaDimuonAnalysis::Exec(Option_t* opt){
  Int_t nSignalPairs   = 15;
  
  Int_t nMCTracks      = fMCTracks->GetEntriesFast();
  Int_t nStsTracks     = fStsTracks->GetEntriesFast();
  Int_t nMuchTracks    = fMuchTracks->GetEntriesFast();
  Int_t nStsPoints     = fStsPoints->GetEntriesFast();
  Int_t nMuchPoints    = fMuchPoints->GetEntriesFast();
  Int_t nMuchHits      = fMuchHits->GetEntriesFast();
  Int_t nGlobalTracks  = fGlobalTracks->GetEntriesFast();
  if (fVerbose>0) printf(" Event: %4i",fEvent++);
  if (fVerbose>0) printf(" MCtracks: %4i",nMCTracks);
  if (fVerbose>0) printf(" GlobalTracks: %4i",nGlobalTracks);
  if (fVerbose>0) printf(" StsTracks: %4i",nStsTracks);
  if (fVerbose>0) printf(" MuchTracks: %4i",nMuchTracks);
  if (fVerbose>0) printf(" MCtracks: %4i",nMCTracks);
  if (fVerbose>0) printf(" StsPoints: %4i",nStsPoints);
  if (fVerbose>0) printf(" MuchPoints: %4i",nMuchPoints);
  if (fVerbose>0) printf(" MuchHits: %4i",nMuchHits);
  if (fVerbose>0) printf("\n");

  Int_t iMuPlusCandidates = 0;
  Int_t iMuMinusCandidates = 0;
  
  Int_t muPlusAccepted = -1;
  Int_t muMinusAccepted = -1;
  
  TLorentzVector pMC;
  TVector3 pRCmuPlus;
  TVector3 pRCmuMinus;
  
  for (Int_t iTrack=0;iTrack<nMCTracks;iTrack++){
    CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(iTrack);
    if (mcTrack->GetMotherId()>=0) continue;
    Int_t nAccStsPoints  = mcTrack->GetNPoints(kSTS);
    Int_t nAccMuchPoints = mcTrack->GetNPoints(kMUCH);
    Int_t pdgCode = mcTrack->GetPdgCode();
    mcTrack->Get4Momentum(pMC);
    // TODO add variables
    if (pdgCode== 13) new((*fMuPlusCandidates)[iMuPlusCandidates++]) CbmAnaMuonCandidate(iTrack,pMC);
    if (pdgCode==-13) new((*fMuMinusCandidates)[iMuMinusCandidates++]) CbmAnaMuonCandidate(iTrack,pMC);
  }

  for (Int_t iPoint=0;iPoint<nMuchPoints;iPoint++){
    CbmMuchPoint* point = (CbmMuchPoint*) fMuchPoints->At(iPoint);
    Int_t trackId = point->GetTrackID();
    if (trackId>=2*nSignalPairs) continue;
    Int_t iJpsi = trackId/2;
    Int_t iSign = trackId%2; // 0 - muPlus, 1 - muMinus
    CbmAnaMuonCandidate* mu = (CbmAnaMuonCandidate*) (!iSign ? (*fMuPlusCandidates)[iJpsi] : (*fMuMinusCandidates)[iJpsi]); 
    Int_t planeId = fGeoScheme->GetLayerSideNr(point->GetDetectorId());
    mu->SetMuchPoint(planeId,iPoint);
  }
  
  for (Int_t iTrack=0;iTrack<nGlobalTracks;iTrack++){
    CbmGlobalTrack* globalTrack = (CbmGlobalTrack*) fGlobalTracks->At(iTrack);
    Int_t iMuchTrack = globalTrack->GetMuchTrackIndex();
    Int_t iStsTrack  = globalTrack->GetStsTrackIndex();
    printf("iSts=%4i iMuch=%4i\n", iStsTrack, iMuchTrack);
    // Check much track
    if (iMuchTrack<0) continue;
    CbmMuchTrack*  muchTrack = (CbmMuchTrack*)  fMuchTracks->At(iMuchTrack);
    CbmTrackMatch* muchTrackMatch = (CbmTrackMatch*) fMuchTrackMatches->At(iMuchTrack);
    Int_t mcMuchTrackId = CbmAnaMuch::GetTrackId(muchTrackMatch,fMuchTrueHitQuota);
    Int_t iJpsi = mcMuchTrackId/2;
    Int_t iSign = mcMuchTrackId%2; // 0 - muPlus, 1 - muMinus
    CbmAnaMuonCandidate* mu = (CbmAnaMuonCandidate*) (!iSign ? (*fMuPlusCandidates)[iJpsi] : (*fMuMinusCandidates)[iJpsi]); 
    for (Int_t i=0;i<muchTrack->GetNofHits();i++){
      Int_t hitIndex = muchTrack->GetHitIndex(i);
      CbmMuchPixelHit* hit = (CbmMuchPixelHit*) fMuchHits->At(hitIndex);
      Int_t planeId = hit->GetPlaneId();
      mu->SetMuchHit(planeId,hitIndex);
    }
    
//    Bool_t trigger = Trigger(muchTrack);
//    printf("%i\n", trigger);
//    if (!trigger) continue;
//    // Check sts track
//    if (iStsTrack<0) continue;
//    CbmStsTrack*   stsTrack      = (CbmStsTrack*) fStsTracks->At(iStsTrack);
//    CbmTrackMatch* stsTrackMatch = (CbmTrackMatch*) fStsTrackMatches->At(iStsTrack);
//    Int_t mcStsTrackId = CbmAnaMuch::GetTrackId(stsTrackMatch,fStsTrueHitQuota);
  }
  // reconstructed dimuon
//  if (mapRecSts[0]>=0 && mapRecSts[1]>=0 && mapRecMuch[0]>=0 && mapRecMuch[1]>=0){
//    new((*fMuPlusCandidates)[iMuPlusCandidates++]) CbmAnaMuonCandidate();

//  }

  for (Int_t iMuon=0;iMuon<fMuPlusCandidates->GetEntriesFast();iMuon++){
    CbmAnaMuonCandidate* mu = (CbmAnaMuonCandidate*) fMuMinusCandidates->At(iMuon);
    printf("muon %4i planes: ",iMuon);
    Int_t* points = mu->GetMuchPoints();
    Int_t* hits   = mu->GetMuchHits();
    for (Int_t iPlane=0;iPlane<NPLANES;iPlane++){
      if (points[iPlane]>=0) printf(" %2i",iPlane); else printf("   ");
      if (hits[iPlane]>=0)  printf("!",iPlane); else printf(" ");
    }
    printf("\n");
  }

}
// -------------------------------------------------------------------------


// -----   Public method Finish   ------------------------------------------
void CbmAnaDimuonAnalysis::Finish(){
  TFile* f = new TFile(fHistoFileName,"recreate");
  f->Close();
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
Bool_t CbmAnaDimuonAnalysis::Trigger(CbmMuchTrack* track){
  Int_t nHits = track->GetNofHits();
  Int_t triggerPlaneId[3]={16,17,18};
  Bool_t triggeredPlanes[3]={0,0,0};
  for (Int_t i=0;i<nHits;i++){
    CbmMuchPixelHit* hit = (CbmMuchPixelHit*) fMuchHits->At(track->GetHitIndex(i));
    Int_t planeId = hit->GetPlaneId();
    printf("%i ",planeId);
    for (Int_t t=0;t<3;t++) if (planeId==triggerPlaneId[t]) triggeredPlanes[t]=1;
//    printf("%i\n",planeId);
  }
//  printf("\n");
  Bool_t trigger = 1;
  for (Int_t t=0;t<3;t++) trigger*=triggeredPlanes[t];
//  printf("trigger=%i\n",trigger);
  return trigger;
}
// -------------------------------------------------------------------------

ClassImp(CbmAnaDimuonAnalysis);
