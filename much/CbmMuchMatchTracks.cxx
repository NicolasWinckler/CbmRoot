#include "CbmMuchMatchTracks.h"

#include "CbmMuchHit.h"
#include "CbmMuchTrack.h"
#include "CbmMuchTrackMatch.h"
#include "CbmMuchDigiMatch.h"
#include "CbmMuchCluster.h"

#include "FairMCPoint.h"
#include "FairRootManager.h"

#include "TClonesArray.h"

#include <iostream>
#include <iomanip>
#include <map>

CbmMuchMatchTracks::CbmMuchMatchTracks()
  : FairTask("CbmMuchMatchTracks"),
  fNofHits(0),
  fNofTrueHits(0),
  fNofWrongHits(0),
  fNofFakeHits(0)
{
}

CbmMuchMatchTracks::~CbmMuchMatchTracks()
{
}

void CbmMuchMatchTracks::Exec(Option_t* opt)
{
  fMatches->Clear();

  // Loop over MuchTracks
  Int_t nofTracks = fTracks->GetEntriesFast();
  for (Int_t iTrack = 0; iTrack < nofTracks; iTrack++) {

    std::map<Int_t, Int_t> matchMap;

    CbmMuchTrack* pTrack = (CbmMuchTrack*) fTracks->At(iTrack);
    if (!pTrack) continue;

    Int_t nofHits = pTrack->GetNHits();
    for (Int_t iHit = 0; iHit < nofHits; iHit++) {
      CbmMuchHit* pHit = (CbmMuchHit*) fHits->At(pTrack->GetHitIndex(iHit));
      if (!pHit) continue;

      if (fClusters==NULL){
        // No Cluster array = Simple hit-production algorithm was used
        // use simplified hit-to-digi-to-point-to-mctrack matching
        Int_t digiIndex = pHit->GetDigi();
        DigiToTrackMatch(digiIndex,matchMap);
      } else {
        // Cluster array present
        // use hit-to-cluster-to-digi-to-point-to-mctrack matching
        Int_t clusterId = pHit->GetCluster();
        CbmMuchCluster* cluster = (CbmMuchCluster*) fClusters->At(clusterId);
        for (Int_t iDigi = 0; iDigi < cluster->GetNDigis(); iDigi++){
          Int_t digiIndex = cluster->GetDigiIndex(iDigi);
          DigiToTrackMatch(digiIndex,matchMap);
        }
      }
    } // loop over hits

    Int_t nofTrue = 0;
    Int_t bestMcTrackId = -1;
    for (std::map<Int_t, Int_t>::iterator it = matchMap.begin();
          it!=matchMap.end(); it++) {
      if (it->first != -1 && it->second > nofTrue) {
         bestMcTrackId = it->first;
         nofTrue = it->second;
      }
    }
    Int_t nofFake = 0;//matchMap[-1];
    Int_t nofWrong = nofHits - nofTrue - nofFake;
    Int_t nofMcTracks = matchMap.size() - 1;

    new ((*fMatches)[iTrack]) CbmMuchTrackMatch(
            bestMcTrackId, nofTrue, nofWrong, nofFake, nofMcTracks);

    fNofHits += nofHits;
    fNofTrueHits += nofTrue;
    fNofWrongHits += nofWrong;
    fNofFakeHits += nofFake;

    if (fVerbose > 1)
    std::cout << "iTrack=" << iTrack << " mcTrack=" << bestMcTrackId
        << " nofHits=" << nofHits << " nofTrue=" << nofTrue
        << " nofWrong=" << nofWrong << " nofFake=" << nofFake
        << " nofMcTracks=" << nofMcTracks << std::endl;
  } // Track loop

  fNEvents++;
}

InitStatus CbmMuchMatchTracks::Init()
{
  FairRootManager* ioman = FairRootManager::Instance();
  if (ioman == NULL) Fatal("Init", "RootManager not instantised!");

  fHits = (TClonesArray*) ioman->GetObject("MuchHit");
  if (fHits == NULL) Fatal("Init", "No MuchHit array!");

  fTracks = (TClonesArray*) ioman->GetObject("MuchTrack");
  if (fTracks == NULL) Fatal("Init", "No MuchTrack array!");

  fPoints = (TClonesArray*) ioman->GetObject("MuchPoint");
  if (fPoints == NULL) Fatal("Init", "No MuchPoint array!");

  fDigiMatch = (TClonesArray*) ioman->GetObject("MuchDigiMatch");
  if (fDigiMatch == NULL) Fatal("Init", "No MuchDigiMatch array!");

  fClusters = (TClonesArray*) ioman->GetObject("MuchCluster");
  if (fClusters == NULL) Info("Init", "No cluster array -- simple hit to digi matching will be used");

  fMatches = new TClonesArray("CbmMuchTrackMatch",100);
  ioman->Register("MuchTrackMatch", "MUCH", fMatches, kTRUE);

  return kSUCCESS;
}

void CbmMuchMatchTracks::Finish()
{
  Double_t trueHits = 100. * Double_t(fNofTrueHits)/Double_t(fNofHits);
  Double_t wrongHits = 100. * Double_t(fNofWrongHits)/Double_t(fNofHits);
  Double_t fakeHits = 100. * Double_t(fNofFakeHits)/Double_t(fNofHits);
  std::cout << "=================================================" << std::endl;
  std::cout << "=====   " << GetName() << ": Run summary " << std::endl;
  std::cout << "True hits: " << trueHits << "%" << std::endl;
  std::cout << "Wrong hits: " << wrongHits << "%" << std::endl;
  std::cout << "Fake hits: " << fakeHits << "%" << std::endl;
  std::cout << "=================================================" << std::endl;
}



void CbmMuchMatchTracks::DigiToTrackMatch(Int_t digiIndex, std::map<Int_t, Int_t> &matchMap){
  CbmMuchDigiMatch* pDigiMatch = (CbmMuchDigiMatch*) fDigiMatch->At(digiIndex);
  if (!pDigiMatch) return;

  for (Int_t iPoint = 0; iPoint < pDigiMatch->GetNPoints(); iPoint++) {
    Int_t pointIndex = pDigiMatch->GetRefIndex(iPoint);

    if (pointIndex < 0) { // Fake or background hit
      matchMap[-1]++;
      continue;
    }

    FairMCPoint* pPoint = (FairMCPoint*) fPoints->At(pointIndex);
    if (!pPoint) continue;

    matchMap[pPoint->GetTrackID()]++;
  }
}


ClassImp(CbmMuchMatchTracks)


