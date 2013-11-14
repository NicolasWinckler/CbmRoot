/*
 * CbmMatchRecoToMC.cxx
 *
 *  Created on: Oct 17, 2013
 *      Author: andrey
 */

#include "CbmMatchRecoToMC.h"
#include "CbmMatch.h"
#include "CbmTrackMatchNew.h"
#include "CbmCluster.h"
#include "CbmBaseHit.h"
#include "CbmTrack.h"
#include "FairMCPoint.h"
#include "FairLogger.h"
#include "TClonesArray.h"

CbmMatchRecoToMC::CbmMatchRecoToMC() :
   FairTask(),
   fMCTracks(NULL),
   fTrdPoints(NULL),
   fTrdDigis(NULL),
   fTrdClusters(NULL),
   fTrdHits(NULL),
   fTrdTracks(NULL),
   fTrdDigiMatches(NULL),
   fTrdClusterMatches(NULL),
   fTrdHitMatches(NULL),
   fTrdTrackMatches(NULL)
{

}

CbmMatchRecoToMC::~CbmMatchRecoToMC()
{
   fTrdClusterMatches->Clear("C");
   delete fTrdClusterMatches;
   fTrdHitMatches->Clear("C");
   delete fTrdHitMatches;
   fTrdTrackMatches->Clear("C");
   delete fTrdTrackMatches;
}

InitStatus CbmMatchRecoToMC::Init()
{
   ReadAndCreateDataBranches();

   return kSUCCESS;
}

void CbmMatchRecoToMC::Exec(
    Option_t* opt)
{
   fTrdClusterMatches->Clear();
   fTrdHitMatches->Clear();
   fTrdTrackMatches->Clear();

   // TRD
   if (fTrdDigis && fTrdClusters && fTrdHits) { // MC->digi->cluster->hit->track
      MatchClusters(fTrdDigiMatches, fTrdClusters, fTrdClusterMatches);
      MatchHitsClustering(fTrdClusterMatches, fTrdHits, fTrdHitMatches);
      MatchTracks(fTrdHitMatches, fTrdPoints, fTrdTracks, fTrdTrackMatches);
   } else if (fTrdHits) { // MC->hit->track
      MatchHitsSmearing(fTrdPoints, fTrdHits, fTrdHitMatches);
      MatchTracks(fTrdHitMatches, fTrdPoints, fTrdTracks, fTrdTrackMatches);
   }

   static Int_t eventNo = 0;
   LOG(INFO) << "CbmMatchRecoToMC::Exec eventNo=" << eventNo++ << FairLogger::endl;
}

void CbmMatchRecoToMC::Finish()
{

}

void CbmMatchRecoToMC::ReadAndCreateDataBranches()
{
   FairRootManager* ioman = FairRootManager::Instance();
   if (NULL == ioman) {
      LOG(FATAL) << "CbmMatchRecoToMC::ReadAndCreateDataBranches() NULL FairRootManager." << FairLogger::endl;
   }

   fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");

   fTrdPoints = (TClonesArray*) ioman->GetObject("TrdPoint");
   fTrdDigis = (TClonesArray*) ioman->GetObject("TrdDigi");
   fTrdClusters = (TClonesArray*) ioman->GetObject("TrdCluster");
   fTrdHits = (TClonesArray*) ioman->GetObject("TrdHit");
   fTrdTracks = (TClonesArray*) ioman->GetObject("TrdTrack");
   fTrdDigiMatches = (TClonesArray*) ioman->GetObject("TrdDigiMatch");
   fTrdClusterMatches = new TClonesArray("CbmMatch", 100);
   ioman->Register("TrdClusterMatch", "TRD", fTrdClusterMatches, kTRUE);
   fTrdHitMatches = new TClonesArray("CbmMatch", 100);
   ioman->Register("TrdHitMatch", "TRD", fTrdHitMatches, kTRUE);
   fTrdTrackMatches = new TClonesArray("CbmTrackMatchNew", 100);
   ioman->Register("TrdTrackMatch", "TRD", fTrdTrackMatches, kTRUE);
}

void CbmMatchRecoToMC::MatchClusters(
      const TClonesArray* digiMatches,
      const TClonesArray* clusters,
      TClonesArray* clusterMatches)
{
   if (!(digiMatches && clusters && clusterMatches)) return;
   Int_t nofClusters = clusters->GetEntriesFast();
   for (Int_t iCluster = 0; iCluster < nofClusters; iCluster++) {
      const CbmCluster* cluster = static_cast<const CbmCluster*>(clusters->At(iCluster));
      CbmMatch* clusterMatch = new ((*clusterMatches)[iCluster]) CbmMatch();
      Int_t nofDigis = cluster->GetNofDigis();
      for (Int_t iDigi = 0; iDigi < nofDigis; iDigi++) {
         const CbmMatch* digiMatch = static_cast<const CbmMatch*>(digiMatches->At(cluster->GetDigi(iDigi)));
         clusterMatch->AddReference(digiMatch);
      }
      //std::cout << "cluster " << iCluster << " " << clusterMatch->ToString();
   }
}

void CbmMatchRecoToMC::MatchHitsClustering(
      const TClonesArray* clusterMatches,
      const TClonesArray* hits,
      TClonesArray* hitMatches)
{
   if (!(clusterMatches && hits && hitMatches)) return;
   Int_t nofHits = hits->GetEntriesFast();
   for (Int_t iHit = 0; iHit < nofHits; iHit++) {
      const CbmBaseHit* hit = static_cast<const CbmBaseHit*>(hits->At(iHit));
      CbmMatch* hitMatch = new ((*hitMatches)[iHit]) CbmMatch();
      const CbmMatch* clusterMatch = static_cast<const CbmMatch*>(clusterMatches->At(hit->GetRefId()));
      hitMatch->AddReference(clusterMatch);
    //  std::cout << "hit " << iHit << " " << hitMatch->ToString();
   }
}

void CbmMatchRecoToMC::MatchHitsSmearing(
      const TClonesArray* points,
      const TClonesArray* hits,
      TClonesArray* hitMatches)
{
   if (!(hits && hitMatches)) return;
   Int_t nofHits = hits->GetEntriesFast();
   for (Int_t iHit = 0; iHit < nofHits; iHit++) {
      const CbmBaseHit* hit = static_cast<const CbmBaseHit*>(hits->At(iHit));
      CbmMatch* hitMatch = new ((*hitMatches)[iHit]) CbmMatch();
      const FairMCPoint* point = static_cast<const FairMCPoint*>(points->At(hit->GetRefId()));
      hitMatch->AddReference(hit->GetRefId(), point->GetEnergyLoss());
    //  std::cout << "hit " << iHit << " " << hitMatch->ToString();
   }
}

void CbmMatchRecoToMC::MatchTracks(
      const TClonesArray* hitMatches,
      const TClonesArray* points,
      const TClonesArray* tracks,
      TClonesArray* trackMatches)
{
   if (!(hitMatches && points && tracks && trackMatches)) return;

   Int_t nofTracks = tracks->GetEntriesFast();
   for (Int_t iTrack = 0; iTrack < nofTracks; iTrack++) {
      const CbmTrack* track = static_cast<const CbmTrack*>(tracks->At(iTrack));
      CbmTrackMatchNew* trackMatch = new ((*trackMatches)[iTrack]) CbmTrackMatchNew();
      Int_t nofHits = track->GetNofHits();
      for (Int_t iHit = 0; iHit < nofHits; iHit++) {
         const CbmMatch* hitMatch = static_cast<CbmMatch*>(hitMatches->At(track->GetHitIndex(iHit)));
         Int_t nofReferences = hitMatch->GetNofReferences();
         for (Int_t iReference = 0; iReference < nofReferences; iReference++) {
            const FairMCPoint* point = static_cast<const FairMCPoint*>(points->At(hitMatch->GetReferenceId(iReference)));
            if (NULL == point) continue;
            trackMatch->AddReference(point->GetTrackID());
         }
      }
      // Calculate number of true and wrong hits
      Int_t trueCounter = 0;
      Int_t wrongCounter = 0;
      for (Int_t iHit = 0; iHit < nofHits; iHit++) {
         const CbmMatch* hitMatch = static_cast<CbmMatch*>(hitMatches->At(track->GetHitIndex(iHit)));
         Int_t nofReferences = hitMatch->GetNofReferences();
         Bool_t hasTrue = false;
         for (Int_t iReference = 0; iReference < nofReferences; iReference++) {
            const FairMCPoint* point = static_cast<const FairMCPoint*>(points->At(hitMatch->GetReferenceId(iReference)));
            if (NULL == point) continue;
            if (point->GetTrackID() == trackMatch->GetMatchedReferenceId()) {
               hasTrue = true;
               break;
            }
         }
         if (hasTrue) trueCounter++; else wrongCounter++;
      }
      trackMatch->SetNofTrueHits(trueCounter);
      trackMatch->SetNofWrongHits(wrongCounter);
      //std::cout << "track " << iTrack << " " << trackMatch->ToString();
   }
}

ClassImp(CbmMatchRecoToMC);
