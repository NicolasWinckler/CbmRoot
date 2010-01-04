#include "CbmAnaMuonCandidate.h"
#include "CbmStsTrack.h"
#include "CbmKFTrack.h"
#include "TLorentzVector.h"

CbmAnaMuonCandidate::CbmAnaMuonCandidate(){
  fIsReconstructed = kFALSE;
  for (Int_t i=0;i<NPLANES;i++){
    muchPoints[i]=-1;
    muchHits[i]=-1;
  }
  
}


CbmAnaMuonCandidate::CbmAnaMuonCandidate(Int_t iTrack, TLorentzVector pMC){
  fIsReconstructed = kFALSE;
  
  fMCTrackId = iTrack;
  fpMC = pMC;
  
  for (Int_t i=0;i<NPLANES;i++){
    muchPoints[i]=-1;
    muchHits[i]=-1;
  }
  
}

void CbmAnaMuonCandidate::SetMomentumRC(Double_t* T){
  Double_t tx = T[2];
  Double_t ty = T[3];
  Double_t qp = T[4];
  Double_t p  = ( TMath::Abs(qp)>1.e-4 ) ? 1./TMath::Abs(qp) :1.e4;
  Double_t pz = TMath::Sqrt( p*p / ( tx*tx + ty*ty + 1 ) );
  Double_t px = tx * pz;
  Double_t py = ty * pz;
  fpRC.SetXYZM(px, py,pz,0.105658);

}

Bool_t CbmAnaMuonCandidate::IsReconstructed(Int_t nStsHitsCut, Double_t chiToVertexCut){
  if (fNStsHits<nStsHitsCut) return kFALSE;
  if (fChiToVertex>chiToVertexCut) return kFALSE;
  return kTRUE;
}

ClassImp(CbmAnaMuonCandidate);
