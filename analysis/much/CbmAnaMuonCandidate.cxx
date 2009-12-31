#include "CbmAnaMuonCandidate.h"
#include "CbmStsTrack.h"
#include "CbmKFTrack.h"
#include "TLorentzVector.h"

CbmAnaMuonCandidate::CbmAnaMuonCandidate(Int_t iTrack, TLorentzVector pMC){
  
  fMCTrackId = iTrack;
  fpMC = pMC;
  
  for (Int_t i=0;i<NPLANES;i++){
    muchPoints[i]=-1;
    muchHits[i]=-1;
  }
  
}

ClassImp(CbmAnaMuonCandidate);
