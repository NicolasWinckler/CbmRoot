/** CbmAnaDimuonAnalysis.cxx
 *@author E.Kryshen <e.kryshen@gsi.de>
 *@since 2010-01-02
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
#include "CbmAnaDimuonCandidate.h"
#include "CbmKFTrack.h"
#include "CbmStsKFTrackFitter.h"
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
  fMuCandidates  = new TClonesArray("CbmAnaMuonCandidate",1);
}
// -------------------------------------------------------------------------


// -----   Standard constructor   ------------------------------------------
CbmAnaDimuonAnalysis::CbmAnaDimuonAnalysis(const char* name,
    TString digiFileName,Int_t nSignalPairs)
:FairTask(name){
  fEvent=0;
  fStsPointsAccQuota=4;
  fStsTrueHitQuota=0.7;
  fMuchPointsAccQuota=10;
  fMuchTrueHitQuota=0.7;
  fMuCandidates  = new TClonesArray("CbmAnaMuonCandidate",1);
  fDimuonCandidates  = new TClonesArray("CbmAnaDimuonCandidate",nSignalPairs);
  fSignalPairs  = nSignalPairs;
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
  fStsPoints        = (TClonesArray*) fManager->GetObject("STSPoint");
  fMuchPoints       = (TClonesArray*) fManager->GetObject("MuchPoint");
  fMuchHits         = (TClonesArray*) fManager->GetObject("MuchPixelHit");
  fStsTracks        = (TClonesArray*) fManager->GetObject("STSTrack");
  fMuchTracks       = (TClonesArray*) fManager->GetObject("MuchTrack");
  fMuchTrackMatches = (TClonesArray*) fManager->GetObject("MuchTrackMatch");
  fStsTrackMatches  = (TClonesArray*) fManager->GetObject("STSTrackMatch");
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
  
  fManager->Register("DimuonCandidates" ,"Much",fDimuonCandidates,kTRUE);
  fManager->Register("MuCandidates" ,"Much",fMuCandidates,kTRUE);
  
  fGeoScheme = CbmMuchGeoScheme::Instance();
  fGeoScheme->Init(fDigiFileName);
  fLastStationIndex = fGeoScheme->GetNStations()-1;
  fNLayers = 0;
  for (Int_t i=0;i<=fLastStationIndex;i++){
    fNLayers+=fGeoScheme->GetLayerSides(i).size()/2;
  }
  fFitter = new CbmStsKFTrackFitter();
  fFitter->Init();

  return kSUCCESS;
}
// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void CbmAnaDimuonAnalysis::Exec(Option_t* opt){
  Int_t nMCTracks      = fMCTracks->GetEntriesFast();
  Int_t nStsTracks     = fStsTracks->GetEntriesFast();
  Int_t nMuchTracks    = fMuchTracks->GetEntriesFast();
  Int_t nStsPoints     = fStsPoints->GetEntriesFast();
  Int_t nMuchPoints    = fMuchPoints->GetEntriesFast();
  Int_t nMuchHits      = fMuchHits->GetEntriesFast();
  Int_t nGlobalTracks  = fGlobalTracks->GetEntriesFast();
  if (fVerbose>-1) printf(" Event: %4i",fEvent++);
  if (fVerbose>0) printf(" MCtracks: %4i",nMCTracks);
  if (fVerbose>0) printf(" GlobalTracks: %4i",nGlobalTracks);
  if (fVerbose>0) printf(" StsTracks: %4i",nStsTracks);
  if (fVerbose>0) printf(" MuchTracks: %4i",nMuchTracks);
  if (fVerbose>0) printf(" MCtracks: %4i",nMCTracks);
  if (fVerbose>0) printf(" StsPoints: %4i",nStsPoints);
  if (fVerbose>0) printf(" MuchPoints: %4i",nMuchPoints);
  if (fVerbose>0) printf(" MuchHits: %4i",nMuchHits);
  if (fVerbose>-1) printf("\n");

  fMuCandidates->Clear();
  fDimuonCandidates->Clear();
  
  for (Int_t iDimuon=0;iDimuon<fSignalPairs;iDimuon++){
    new((*fDimuonCandidates)[iDimuon]) CbmAnaDimuonCandidate();
  }
  
  TLorentzVector pMC;

  for (Int_t iMCTrack=0;iMCTrack<2*fSignalPairs;iMCTrack++){
    CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(iMCTrack);
    CbmAnaMuonCandidate* mu = GetMu(iMCTrack);
    mcTrack->Get4Momentum(pMC);
    mu->SetMomentumMC(pMC);
    mu->SetMCTrackId(iMCTrack);
  }
  
  Int_t iMuCandidates = 0;
  
  Int_t muPlusAccepted = -1;
  Int_t muMinusAccepted = -1;
  
  TVector3 pRCmuPlus;
  TVector3 pRCmuMinus;
  
  for (Int_t iTrack=0;iTrack<nMCTracks;iTrack++){
    CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(iTrack);
    if (mcTrack->GetMotherId()>=0) continue;
    Int_t nAccStsPoints  = mcTrack->GetNPoints(kSTS);
    Int_t nAccMuchPoints = mcTrack->GetNPoints(kMUCH);
    Int_t pdgCode = mcTrack->GetPdgCode();
    if (pdgCode==223) printf("%i\n",iTrack);
    mcTrack->Get4Momentum(pMC);
    // TODO add variables
  }

  for (Int_t iPoint=0;iPoint<nMuchPoints;iPoint++){
    CbmMuchPoint* point = (CbmMuchPoint*) fMuchPoints->At(iPoint);
    Int_t trackId = point->GetTrackID();
    if (trackId>=2*fSignalPairs) continue;
    CbmAnaMuonCandidate* mu = GetMu(trackId);
    Int_t planeId = fGeoScheme->GetLayerSideNr(point->GetDetectorId());
    mu->SetMuchPoint(planeId,iPoint);
  }

  
  for (Int_t iTrack=0;iTrack<nGlobalTracks;iTrack++){
    CbmGlobalTrack* globalTrack = (CbmGlobalTrack*) fGlobalTracks->At(iTrack);
    Int_t iMuchTrack = globalTrack->GetMuchTrackIndex();
    Int_t iStsTrack  = globalTrack->GetStsTrackIndex();
    //printf("iSts=%4i iMuch=%4i\n", iStsTrack, iMuchTrack);
    // Check much track
    if (iMuchTrack<0) continue;
    CbmMuchTrack*  muchTrack = (CbmMuchTrack*)  fMuchTracks->At(iMuchTrack);
    CbmTrackMatch* muchTrackMatch = (CbmTrackMatch*) fMuchTrackMatches->At(iMuchTrack);
    Int_t mcMuchTrackId = CbmAnaMuch::GetTrackId(muchTrackMatch,fMuchTrueHitQuota);
    //printf("\n");
    Int_t nTriggerHits=0;
    for (Int_t i=0;i<muchTrack->GetNofHits();i++){
      Int_t hitIndex = muchTrack->GetHitIndex(i);
      CbmMuchPixelHit* hit = (CbmMuchPixelHit*) fMuchHits->At(hitIndex);
      //printf(" %i",hitIndex);
//      Int_t planeId = hit->GetPlaneId();
//      mu->SetMuchHit(planeId,hitIndex);
      Int_t stationIndex = fGeoScheme->GetStationIndex(hit->GetDetectorId());
      if (stationIndex==fLastStationIndex) nTriggerHits++;
    }
    
    CbmAnaMuonCandidate* mu;
    if (mcMuchTrackId<0 || mcMuchTrackId>=2*fSignalPairs) {
      if (nTriggerHits<3) continue;
      if (muchTrack->GetNofHits()<fNLayers-3) continue;
      new((*fMuCandidates)[iMuCandidates++]) CbmAnaMuonCandidate();
      mu = (CbmAnaMuonCandidate*) (*fMuCandidates)[iMuCandidates-1];
    } else {
      mu = GetMu(mcMuchTrackId); 
    }
    mu->SetNTriggerHits(nTriggerHits);
    if (iStsTrack<0) continue;
    CbmStsTrack* stsTrack = (CbmStsTrack*) fStsTracks->At(iStsTrack);
    Int_t nStsHits = stsTrack->GetNStsHits();
    mu->SetNStsHits(nStsHits);
    Double_t chi = fFitter->GetChiToVertex(stsTrack);
    mu->SetChiToVertex(chi);
//    printf("n=%i chi=%f\n",nStsHits,chi);
    CbmKFTrack kfTrack = CbmKFTrack(*stsTrack);
    
    kfTrack.Extrapolate(0); // TODO change to primary vertex z
    Double_t* T=kfTrack.GetTrack();
    mu->SetMomentumRC(T);
    mu->SetReconstructed(kTRUE);
    mu->SetSign(T[4]>0 ? 1. : -1.);

//    // Check sts track
//    if (iStsTrack<0) continue;
//    CbmStsTrack*   stsTrack      = (CbmStsTrack*) fStsTracks->At(iStsTrack);
//    CbmTrackMatch* stsTrackMatch = (CbmTrackMatch*) fStsTrackMatches->At(iStsTrack);
//    Int_t mcStsTrackId = CbmAnaMuch::GetTrackId(stsTrackMatch,fStsTrueHitQuota);
  }

  for (Int_t iDimuon=0;iDimuon<fDimuonCandidates->GetEntriesFast();iDimuon++){
    CbmAnaDimuonCandidate* dimuon = (CbmAnaDimuonCandidate*) fDimuonCandidates->At(iDimuon);
    for (Int_t sign=0;sign<2;sign++){
      CbmAnaMuonCandidate* mu = dimuon->GetMu(sign);
//      printf("mu(%2i,%i) planes: ",iDimuon,sign);
      Int_t* points = mu->GetMuchPoints();
      Int_t* hits   = mu->GetMuchHits();
//      for (Int_t iPlane=0;iPlane<NPLANES;iPlane++){
//        if (points[iPlane]>=0) printf("%2i",iPlane); else printf("  ");
//        if (hits[iPlane]>=0)  printf("!",iPlane); else printf(" ");
//      }
//      printf("\n");
    }//sign
  }//dimuon

}
// -------------------------------------------------------------------------


// -----   Public method Finish   ------------------------------------------
void CbmAnaDimuonAnalysis::Finish(){
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
CbmAnaMuonCandidate* CbmAnaDimuonAnalysis::GetMu(Int_t trackId){
  if (trackId<0) Fatal("GetMu","MCtrackId for signal muon < 0");
  if (trackId>=2*fSignalPairs) Fatal("GetMu","MCtrackId for signal muon > 2*dimuons");
  Int_t iDimuon = trackId/2;
  Int_t sign = trackId%2; // 0 - muPlus, 1 - muMinus
  return ((CbmAnaDimuonCandidate*) fDimuonCandidates->At(iDimuon))->GetMu(sign); 
}
// -------------------------------------------------------------------------


ClassImp(CbmAnaDimuonAnalysis);
