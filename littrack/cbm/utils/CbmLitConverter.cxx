#include "utils/CbmLitConverter.h"

#include "base/CbmLitEnvironment.h"
#include "data/CbmLitHit.h"
#include "data/CbmLitPixelHit.h"
#include "data/CbmLitStripHit.h"
#include "data/CbmLitTrack.h"
#include "data/CbmLitTrackParam.h"
#include "data/CbmLitFitNode.h"

#include "CbmTrack.h"
#include "CbmBaseHit.h"
#include "CbmPixelHit.h"
#include "CbmStripHit.h"
#include "CbmHit.h"
#include "FairTrackParam.h"
#include "CbmStsTrack.h"
#include "CbmMuchTrack.h"
#include "CbmTrdTrack.h"
#include "CbmMuchStrawHit.h"
#include "CbmMuchGeoScheme.h"

#include "CbmGlobalTrack.h"
#include "CbmTrdDetectorId.h"

#include "TClonesArray.h"

#include <iostream>
#include <cmath>
#include <set>
#include <cassert>

CbmLitConverter::CbmLitConverter()
{

}

CbmLitConverter::~CbmLitConverter()
{

}

void CbmLitConverter::TrackParamToLitTrackParam(
   const FairTrackParam* par,
   CbmLitTrackParam* litPar)
{
   litPar->SetX((litfloat)par->GetX());
   litPar->SetY((litfloat)par->GetY());
   litPar->SetZ((litfloat)par->GetZ());
   litPar->SetTx((litfloat)par->GetTx());
   litPar->SetTy((litfloat)par->GetTy());
   litPar->SetQp((litfloat)par->GetQp());
   Double_t cov[15];
   par->CovMatrix(cov);
   std::vector<litfloat> covVec(15);
   for (UInt_t i = 0; i < 15; ++i) { covVec[i] = (litfloat) cov[i]; }
   litPar->SetCovMatrix(covVec);
}

void CbmLitConverter::LitTrackParamToTrackParam(
   const CbmLitTrackParam* litPar,
   FairTrackParam* par)
{
   par->SetX(litPar->GetX());
   par->SetY(litPar->GetY());
   par->SetZ(litPar->GetZ());
   par->SetTx(litPar->GetTx());
   par->SetTy(litPar->GetTy());
   par->SetQp(litPar->GetQp());
   Double_t cov[15];
   std::vector<litfloat> covVec(litPar->GetCovMatrix());
   for (UInt_t i = 0; i < 15; ++i) { cov[i] = (Double_t) covVec[i]; }
   par->SetCovMatrix(cov);
}

void CbmLitConverter::PixelHitToLitPixelHit(
   const CbmPixelHit* hit,
   Int_t index,
   CbmLitPixelHit* litHit)
{
   assert(hit->GetType() == kTRDHIT || hit->GetType() == kMUCHPIXELHIT || hit->GetType() == kTOFHIT);

   litHit->SetX(hit->GetX());
   litHit->SetY(hit->GetY());
   litHit->SetZ(hit->GetZ());
   litHit->SetDx(hit->GetDx());
   litHit->SetDy(hit->GetDy());
   litHit->SetDz(hit->GetDz());
   litHit->SetDxy(hit->GetDxy());
   litHit->SetRefId(index);

   if (hit->GetType() == kTRDHIT) {
	   CbmTrdDetectorId trdDetId;
	   Int_t* info =  trdDetId.GetDetectorInfo(hit->GetDetectorId());
	   litHit->SetDetectorId(kLITTRD, info[1] - 1, info[2] - 1, 0, info[3] * 1000 + info[4] - 1);
   } else if (hit->GetType() == kMUCHPIXELHIT) {
	   Int_t detId = hit->GetDetectorId();
	   litHit->SetDetectorId(kLITMUCH, CbmMuchGeoScheme::GetStationIndex(detId), CbmMuchGeoScheme::GetLayerIndex(detId), CbmMuchGeoScheme::GetLayerSideIndex(detId), 0);
   } else if (hit->GetType() == kTOFHIT) {
	   litHit->SetDetectorId(kLITTOF, 0, 0, 0, 0);
   }
}

void CbmLitConverter::StripHitToLitStripHit(
   const CbmStripHit* hit,
   Int_t index,
   CbmLitStripHit* litHit)
{
   assert(hit->GetType() == kMUCHSTRAWHIT);

   litHit->SetU(hit->GetU());
   litHit->SetDu(hit->GetDu());
   litHit->SetZ(hit->GetZ());
   litHit->SetDz(hit->GetDz());
   litHit->SetPhi(hit->GetPhi());
   litHit->SetCosPhi(std::cos(litHit->GetPhi()));
   litHit->SetSinPhi(std::sin(litHit->GetPhi()));
   litHit->SetRefId(index);

   if (hit->GetType() == kMUCHSTRAWHIT) {
        litHit->SetSegment((static_cast<const CbmMuchStrawHit*>(hit))->GetSegment());
        Int_t detId = hit->GetDetectorId();
 	    litHit->SetDetectorId(kLITMUCH, CbmMuchGeoScheme::GetStationIndex(detId), CbmMuchGeoScheme::GetLayerIndex(detId), CbmMuchGeoScheme::GetLayerSideIndex(detId), CbmMuchGeoScheme::GetModuleIndex(detId));
   }
}

void CbmLitConverter::CbmHitToLitPixelHit(
   const CbmHit* hit,
   Int_t index,
   CbmLitPixelHit* litHit,
   LitSystemId sysId)
{
   assert(sysId == kLITMVD || sysId == kLITSTS);

   litHit->SetX(hit->GetX());
   litHit->SetY(hit->GetY());
   litHit->SetZ(hit->GetZ());
   litHit->SetDx(hit->GetDx());
   litHit->SetDy(hit->GetDy());
   litHit->SetDz(hit->GetDz());
   litHit->SetDxy(hit->GetCovXY());
   litHit->SetRefId(index);

   litHit->SetDetectorId(sysId, 0, hit->GetStationNr() - 1, 0, 0);
}

void CbmLitConverter::StsTrackToLitTrack(
   const CbmStsTrack* stsTrack,
   CbmLitTrack* litTrack)
{
   // TODO note that hits are not copied now

   litTrack->SetQuality(kLITGOOD);
   litTrack->SetChi2(stsTrack->GetChi2());
   litTrack->SetNDF(stsTrack->GetNDF());
   litTrack->SetPreviousTrackId(-1);
   CbmLitTrackParam paramFirst, paramLast;
   //TODO remove this const typecasting
   CbmLitConverter::TrackParamToLitTrackParam((const_cast<CbmStsTrack*> (stsTrack))->GetParamFirst(), &paramFirst);
   CbmLitConverter::TrackParamToLitTrackParam((const_cast<CbmStsTrack*> (stsTrack))->GetParamLast(), &paramLast);
   litTrack->SetParamFirst(&paramFirst);
   litTrack->SetParamLast(&paramLast);
}

void CbmLitConverter::TrackToLitTrack(
   const CbmTrack* track,
   CbmLitTrack* litTrack,
   const TClonesArray* pixelHits,
   const TClonesArray* stripHits)
{
   for (Int_t iHit = 0; iHit < track->GetNofHits(); iHit++) {
      Int_t index = track->GetHitIndex(iHit);
      Int_t type = track->GetHitType(iHit);
      if (type == kMUCHSTRAWHIT) {
         CbmStripHit* stripHit = (CbmStripHit*) stripHits->At(index);
         CbmLitStripHit litHit;
         StripHitToLitStripHit(stripHit, index, &litHit);
         litTrack->AddHit(&litHit);
      } else {
         CbmPixelHit* pixelHit = (CbmPixelHit*) pixelHits->At(index);
         CbmLitPixelHit litHit;
         PixelHitToLitPixelHit(pixelHit, index, &litHit);
         litTrack->AddHit(&litHit);
      }
   }

   litTrack->SetQuality(kLITGOOD);
   litTrack->SetChi2(track->GetChiSq());
   litTrack->SetNDF(track->GetNDF());
   litTrack->SetPreviousTrackId(track->GetPreviousTrackId());
   litTrack->SetLastPlaneId(track->GetFlag());
   litTrack->SetPDG(track->GetPidHypo());
   CbmLitTrackParam paramFirst, paramLast;
   CbmLitConverter::TrackParamToLitTrackParam(track->GetParamFirst(), &paramFirst);
   CbmLitConverter::TrackParamToLitTrackParam(track->GetParamLast(), &paramLast);
   litTrack->SetParamFirst(&paramFirst);
   litTrack->SetParamLast(&paramLast);
}

void CbmLitConverter::LitTrackToTrack(
   const CbmLitTrack* litTrack,
   CbmTrack* track)
{
   for (Int_t iHit = 0; iHit < litTrack->GetNofHits(); iHit++) {
      const CbmLitHit* hit = litTrack->GetHit(iHit);
      LitHitType type = hit->GetType();
      LitSystemId det = hit->GetSystem();
      if (det == kLITMUCH && type == kLITPIXELHIT) { track->AddHit(hit->GetRefId(), kMUCHPIXELHIT); }
      else if (det == kLITMUCH && type == kLITSTRIPHIT) { track->AddHit(hit->GetRefId(), kMUCHSTRAWHIT); }
      else if (det == kLITTRD) { track->AddHit(hit->GetRefId(), kTRDHIT); }
   }

   track->SetChiSq(litTrack->GetChi2());
   track->SetNDF(litTrack->GetNDF());
   track->SetPreviousTrackId(litTrack->GetPreviousTrackId());
   track->SetFlag(litTrack->GetQuality());
   FairTrackParam parLast, parFirst;
   CbmLitConverter::LitTrackParamToTrackParam(litTrack->GetParamLast(), &parLast);
   CbmLitConverter::LitTrackParamToTrackParam(litTrack->GetParamFirst(), &parFirst);
   track->SetParamLast(&parLast);
   track->SetParamFirst(&parFirst);
}

void CbmLitConverter::GlobalTrackArrayToLitTrackVector(
   const TClonesArray* globalTracks,
   const TClonesArray* stsTracks,
   const TClonesArray* trdTracks,
   const TClonesArray* muchTracks,
   const TClonesArray* mvdHits,
   const TClonesArray* stsHits,
   const TClonesArray* trdHits,
   const TClonesArray* muchStrawHits,
   const TClonesArray* muchPixelHits,
   const TClonesArray* tofHits,
   TrackPtrVector& litTracks)
{
	Int_t nofGlobalTracks = globalTracks->GetEntriesFast();
	for (Int_t iTrack = 0; iTrack < nofGlobalTracks; iTrack++) {
		const CbmGlobalTrack* globalTrack = static_cast<const CbmGlobalTrack*>(globalTracks->At(iTrack));

		CbmLitTrack* litTrack = new CbmLitTrack();

		Int_t stsId = globalTrack->GetStsTrackIndex();
		Int_t trdId = globalTrack->GetTrdTrackIndex();
		Int_t muchId = globalTrack->GetMuchTrackIndex();
		Int_t tofId = globalTrack->GetTofHitIndex();

		// STS: set initial track parameters from STS track
		if (stsId > -1) {
		   CbmStsTrack* stsTrack = static_cast<CbmStsTrack*>(stsTracks->At(stsId));
		   CbmLitTrackParam par;
		   CbmLitConverter::TrackParamToLitTrackParam((const_cast<CbmStsTrack*> (stsTrack))->GetParamFirst(), &par);
		   litTrack->SetParamFirst(&par);

		   for (Int_t iHit = 0; iHit < stsTrack->GetNMvdHits(); iHit++) {
			  Int_t index = stsTrack->GetMvdHitIndex(iHit);
		      CbmHit* hit = static_cast<CbmHit*>(mvdHits->At(iHit));
		      CbmLitPixelHit* litHit = new CbmLitPixelHit();
		      CbmHitToLitPixelHit(hit, iHit, litHit, kLITMVD);
			  litTrack->AddHit(litHit);
		   }

		   for (Int_t iHit = 0; iHit < stsTrack->GetNStsHits(); iHit++) {
			  Int_t index = stsTrack->GetStsHitIndex(iHit);
		      CbmHit* hit = static_cast<CbmHit*>(stsHits->At(iHit));
		      CbmLitPixelHit* litHit = new CbmLitPixelHit();
		      CbmHitToLitPixelHit(hit, iHit, litHit, kLITSTS);
			  litTrack->AddHit(litHit);
		   }
		}

		// TRD: attach hits from TRD track
		if (trdId > -1) {
		   const CbmTrdTrack* trdTrack = static_cast<const CbmTrdTrack*>(trdTracks->At(trdId));
		   for (Int_t iHit = 0; iHit < trdTrack->GetNofHits(); iHit++) {
			  Int_t index = trdTrack->GetHitIndex(iHit);
			  const CbmPixelHit* hit = static_cast<const CbmPixelHit*>(trdHits->At(index));
			  CbmLitPixelHit* litHit = new CbmLitPixelHit();
			  PixelHitToLitPixelHit(hit, index, litHit);
			  litTrack->AddHit(litHit);
		   }
		}

		// MUCH: attach hits from MUCH track
		if (muchId > -1) {
		   const CbmMuchTrack* muchTrack = static_cast<const CbmMuchTrack*>(muchTracks->At(muchId));
		   for (Int_t iHit = 0; iHit < muchTrack->GetNofHits(); iHit++) {
			  Int_t index = muchTrack->GetHitIndex(iHit);
			  HitType type = muchTrack->GetHitType(iHit);
			  if (type == kMUCHSTRAWHIT) {
					CbmLitStripHit* litHit = new CbmLitStripHit();
					const CbmStripHit* stripHit = static_cast<const CbmStripHit*>(muchStrawHits->At(index));
					StripHitToLitStripHit(stripHit, index, litHit);
					litTrack->AddHit(litHit);
			  } else {
					CbmLitPixelHit* litHit = new CbmLitPixelHit();
					const CbmPixelHit* pixelHit = static_cast<const CbmPixelHit*>(muchPixelHits->At(index));
					PixelHitToLitPixelHit(pixelHit, index, litHit);
					litTrack->AddHit(litHit);
			  }
		   }
		}

		// TOF: attach TOF hit
		if (tofId > -1) {
			  const CbmPixelHit* hit = static_cast<const CbmPixelHit*>(tofHits->At(tofId));
			  CbmLitPixelHit* litHit = new CbmLitPixelHit();
			  PixelHitToLitPixelHit(hit, tofId, litHit);
			  litTrack->AddHit(litHit);
		}

		litTracks.push_back(litTrack);
	}
}


void CbmLitConverter::LitTrackVectorToGlobalTrackArray(
   const TrackPtrVector& litTracks,
   TClonesArray* globalTracks,
   TClonesArray* stsTracks,
   TClonesArray* trdTracks,
   TClonesArray* muchTracks)
{
   Int_t globalTrackNo = globalTracks->GetEntriesFast();
   Int_t trdTrackNo, muchTrackNo;
   if (trdTracks != NULL) { trdTrackNo = trdTracks->GetEntriesFast(); }
   if (muchTracks != NULL) { muchTrackNo = muchTracks->GetEntriesFast(); }

   std::set<Int_t> stsTracksSet;
   for (Int_t iTrack = 0; iTrack < litTracks.size(); iTrack++) {
      CbmLitTrack* litTrack = litTracks[iTrack];

      if (litTrack->GetQuality() == kLITBAD) { continue; }
      if (litTrack->GetNofHits() < 1) { continue; }
      if (litTrack->GetNofHits() != litTrack->GetFitNodes().size()) {
         std::cout << "-E- CbmLitConverter::LitTrackVectorToGlobalTrackArray: unequal number of hits and fit nodes" << std::endl;
         std::cout << litTrack->ToString();
         continue;
      }

      CbmGlobalTrack* globalTrack = new ((*globalTracks)[globalTrackNo++]) CbmGlobalTrack();

      // Set last parameter of the CbmGlobal track to be last parameter of CbmLitTrack
      FairTrackParam parLast;
      CbmLitConverter::LitTrackParamToTrackParam(litTrack->GetParamLast(), &parLast);
      globalTrack->SetParamLast(&parLast);

      Bool_t isCreateMuchTrack = false, isCreateTrdTrack = false;
      for (Int_t iHit = 0; iHit < litTrack->GetNofHits(); iHit++) {
         const CbmLitHit* thisHit = litTrack->GetHit(iHit);
         LitSystemId thisDetId = thisHit->GetSystem();
         if (thisDetId == kLITMUCH && muchTracks != NULL) { isCreateMuchTrack = true; }
         if (thisDetId == kLITTRD && trdTracks != NULL) { isCreateTrdTrack = true; }
      }

      std::vector<std::pair<LitSystemId, Int_t> > vDetId;
      vDetId.push_back(std::make_pair(litTrack->GetHit(0)->GetSystem(), 0));
      for (Int_t iHit = 0; iHit < litTrack->GetNofHits(); iHit++) {
         const CbmLitHit* thisHit = litTrack->GetHit(iHit);
         LitSystemId thisDetId = thisHit->GetSystem();
         if (vDetId.back().first != thisDetId) {
            vDetId.push_back(std::make_pair(thisDetId, iHit));
         }
      }
      Int_t muchFirst = -1, muchLast = -1, trdFirst = -1, trdLast = -1;
      for (Int_t i = 0; i < vDetId.size(); i++) {
         if (vDetId[i].first == kLITMUCH) {
            muchFirst = vDetId[i].second;
            if (i < vDetId.size() - 1) { muchLast = vDetId[i + 1].second - 1; }
            else { muchLast = litTrack->GetNofHits() - 1; }
         }
         if (vDetId[i].first == kLITTRD) {
            trdFirst = vDetId[i].second;
            if (i < vDetId.size() - 1) { trdLast = vDetId[i + 1].second - 1; }
            else { trdLast = litTrack->GetNofHits() - 1; }
         }
      }

      CbmMuchTrack* muchTrack = NULL;
      CbmTrdTrack* trdTrack = NULL;
      if (isCreateMuchTrack) { muchTrack = new ((*muchTracks)[muchTrackNo++]) CbmMuchTrack(); }
      if (isCreateTrdTrack) { trdTrack = new ((*trdTracks)[trdTrackNo++]) CbmTrdTrack(); }

      double chiSqMuch = 0., chiSqTrd = 0.;
      Int_t ndfMuch = 0, ndfTrd = 0;
      for (Int_t iHit = 0; iHit < litTrack->GetNofHits(); iHit++) {
         const CbmLitHit* hit = litTrack->GetHit(iHit);
         const CbmLitFitNode* node = litTrack->GetFitNode(iHit);
         LitSystemId detId = hit->GetSystem();
         if (detId == kLITTRD && isCreateTrdTrack) {
            trdTrack->AddHit(hit->GetRefId(), kTRDHIT);
            chiSqTrd += node->GetChiSqFiltered();
            ndfTrd += 2;
         }
         if (detId == kLITMUCH && isCreateMuchTrack) {
            if (hit->GetType() == kLITPIXELHIT) {
               muchTrack->AddHit(hit->GetRefId(), kMUCHPIXELHIT);
               ndfMuch += 2;
            } else {
               muchTrack->AddHit(hit->GetRefId(), kMUCHSTRAWHIT);
               ndfMuch++;
            }
            chiSqMuch += node->GetChiSqFiltered();
         }
         if (detId == kLITTOF) {
            globalTrack->SetTofHitIndex(hit->GetRefId());
         }
      }
      ndfMuch -= 5;
      ndfTrd -= 5;
      if (ndfMuch <= 0) { ndfMuch = 1; }
      if (ndfTrd <= 0) { ndfTrd = 1; }

      globalTrack->SetStsTrackIndex(litTrack->GetPreviousTrackId());
      stsTracksSet.insert(litTrack->GetPreviousTrackId());
      if (isCreateMuchTrack) {
         globalTrack->SetMuchTrackIndex(muchTrackNo - 1);
         muchTrack->SetPreviousTrackId(litTrack->GetPreviousTrackId());
         muchTrack->SetChiSq(chiSqMuch);
         muchTrack->SetNDF(ndfMuch);

         FairTrackParam parLast, parFirst;
         CbmLitConverter::LitTrackParamToTrackParam(litTrack->GetFitNode(muchLast)->GetUpdatedParam(), &parLast);
         CbmLitConverter::LitTrackParamToTrackParam(litTrack->GetFitNode(muchFirst)->GetUpdatedParam(), &parFirst);
         muchTrack->SetParamLast(&parLast);
         muchTrack->SetParamFirst(&parFirst);
      }
      if (isCreateTrdTrack) {
         globalTrack->SetTrdTrackIndex(trdTrackNo - 1);
         trdTrack->SetPreviousTrackId(litTrack->GetPreviousTrackId());
         trdTrack->SetChiSq(chiSqTrd);
         trdTrack->SetNDF(ndfTrd);

         FairTrackParam parLast, parFirst;
         CbmLitConverter::LitTrackParamToTrackParam(litTrack->GetFitNode(trdLast)->GetUpdatedParam(), &parLast);
         CbmLitConverter::LitTrackParamToTrackParam(litTrack->GetFitNode(trdFirst)->GetUpdatedParam(), &parFirst);
         trdTrack->SetParamLast(&parLast);
         trdTrack->SetParamFirst(&parFirst);
      }
   }

   // Loop over STS tracks in order to create additional CbmGlobalTrack,
   // consisting only CbmStsTrack
   for (Int_t iTrack = 0; iTrack < stsTracks->GetEntriesFast(); iTrack++) {
      // if CbmGlobalTrack for STS track was not created, than create one
      if (stsTracksSet.find(iTrack) == stsTracksSet.end()) {
         CbmGlobalTrack* globalTrack = new ((*globalTracks)[globalTrackNo++]) CbmGlobalTrack();
         globalTrack->SetStsTrackIndex(iTrack);
      }
   }
}

void CbmLitConverter::HitArrayToHitVector(
   const TClonesArray* hits,
   HitPtrVector& litHits)
{
   Int_t nofHits = hits->GetEntriesFast();
   for(Int_t iHit = 0; iHit < nofHits; iHit++) {
      CbmBaseHit* hit = (CbmBaseHit*) hits->At(iHit);
      if(NULL == hit) { continue; }
      if (hit->GetType() == kMUCHSTRAWHIT) {
         CbmLitStripHit* litHit = new CbmLitStripHit();
         CbmStripHit* stripHit = static_cast<CbmStripHit*>(hit);
         StripHitToLitStripHit(stripHit, iHit, litHit);
         litHits.push_back(litHit);
      } else {
         CbmLitPixelHit* litHit = new CbmLitPixelHit();
         CbmPixelHit* pixelHit = static_cast<CbmPixelHit*>(hit);
         PixelHitToLitPixelHit(pixelHit, iHit, litHit);
         litHits.push_back(litHit);
      }
   }
}

void CbmLitConverter::MvdHitArrayToHitVector(
   const TClonesArray* hits,
   HitPtrVector& litHits)
{
   Int_t nofHits = hits->GetEntriesFast();
   for(Int_t iHit = 0; iHit < nofHits; iHit++) {
      CbmHit* hit = static_cast<CbmHit*>(hits->At(iHit));
      if(NULL == hit) { continue; }
      CbmLitPixelHit* litHit = new CbmLitPixelHit();
      CbmHitToLitPixelHit(hit, iHit, litHit, kLITMVD);
      litHits.push_back(litHit);
   }
}

void CbmLitConverter::StsTrackArrayToTrackVector(
   const TClonesArray* tracks,
   TrackPtrVector& litTracks)
{
   Int_t nofTracks = tracks->GetEntriesFast();
   for(Int_t iTrack = 0; iTrack < nofTracks; iTrack++) {
      CbmStsTrack* track = (CbmStsTrack*) tracks->At(iTrack);
      if (track == NULL) { continue; }
      if (track->GetParamLast()->GetQp() == 0) { continue; }
      CbmLitTrack* litTrack = new CbmLitTrack;
      StsTrackToLitTrack(track, litTrack);
      litTrack->SetPreviousTrackId(iTrack);
      litTrack->SetRefId(iTrack);
      litTracks.push_back(litTrack);
   }
}

void CbmLitConverter::TrackVectorToMuchTrackArray(
   TrackPtrVector& tracks,
   TClonesArray* muchTracks)
{
   Int_t trackNo = muchTracks->GetEntriesFast();
   for(TrackPtrIterator iTrack = tracks.begin(); iTrack != tracks.end(); iTrack++) {
      CbmMuchTrack track;
      LitTrackToTrack(*iTrack, &track);
      new ((*muchTracks)[trackNo++]) CbmMuchTrack(track);
   }
}

void CbmLitConverter::TrackVectorToTrdTrackArray(
   TrackPtrVector& tracks,
   TClonesArray* trdTracks)
{
   Int_t trackNo = trdTracks->GetEntriesFast();
   for(TrackPtrIterator iTrack = tracks.begin(); iTrack != tracks.end(); iTrack++) {
      CbmTrdTrack track;
      LitTrackToTrack(*iTrack, &track);
      new ((*trdTracks)[trackNo++]) CbmTrdTrack(track);
   }
}
