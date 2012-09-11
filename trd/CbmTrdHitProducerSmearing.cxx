/**
 * \file CbmTrdHitProducerSmearing.cxx
 * \author M.Kalisky <m.kalisky@gsi.de>
 * \date 2005
 */

#include "CbmTrdHitProducerSmearing.h"

#include "CbmTrdRadiator.h"
#include "CbmTrdPoint.h"
#include "CbmTrdHit.h"
#include "CbmTrdGeoHandler.h"

#include "CbmMCTrack.h"

#include "FairRootManager.h"
#include "FairTask.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"
#include "TMath.h"
#include "TRandom.h"
#include "TVector3.h"

#include <boost/assign/list_of.hpp>

#include <iostream>
#include <vector>
#include <cassert>

using std::vector;
using std::map;
using boost::assign::list_of;

CbmTrdHitProducerSmearing::CbmTrdHitProducerSmearing(
      CbmTrdRadiator* radiator) :
      FairTask("CbmTrdHitProducerSmearing"),
      fTrdPoints(NULL),
      fTrdHits(NULL),
      fMCTracks(NULL),
      fDigiPar(NULL),
      fModuleInfo(NULL),
      fGeoHandler(new CbmTrdGeoHandler()),
      fRadiator(radiator),
      fEfficency(1.),
      fGhostRate(0.),
      fGhostDistance(0.),
      fHitMergingDistance(0.),
      fUseDigiPar(true)
{
   // Default resolution
   fSigmaX = list_of(0.03)(0.04)(0.05); // Resolution in x [cm]
   // Resolutions in y - station and angle dependent [cm]
   fSigmaY.push_back(list_of(0.27)(0.37)(1.5)(2.76)(3.3)(3.3)(3.3));
   fSigmaY.push_back(list_of(0.63)(0.83)(3.3)(3.3)(3.3)(3.3)(3.3));
   fSigmaY.push_back(list_of(1.03)(1.5)(3.3)(3.3)(3.3)(3.3)(3.3));
}

CbmTrdHitProducerSmearing::~CbmTrdHitProducerSmearing()
{
   fTrdHits->Clear("C");
   delete fTrdHits;
   if (fRadiator) delete fRadiator;
}

void CbmTrdHitProducerSmearing::SetParContainers()
{
   FairRuntimeDb* rtdb = FairRunAna::Instance()->GetRuntimeDb();
   fDigiPar = (CbmTrdDigiPar*) rtdb->getContainer("CbmTrdDigiPar");
}

InitStatus CbmTrdHitProducerSmearing::Init()
{
   FairRootManager* ioman = FairRootManager::Instance();

   fTrdPoints = (TClonesArray *) ioman->GetObject("TrdPoint");
   if (NULL == fTrdPoints) Fatal("CbmTrdHitProducerSmearing", "No TrdPoint array!");

   fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
   if (NULL == fMCTracks) Fatal("CbmTrdHitProducerSmearing", "No MCTrack array!");

   fTrdHits = new TClonesArray("CbmTrdHit", 100);
   ioman->Register("TrdHit", "TRD", fTrdHits, kTRUE);

   // Extract information about the number of TRD stations and
   // the number of layers per TRD station from the geomanager.
   // Store the information about the number of layers at the entrance
   // of subsequent stations in a vector.
   // This allows to calculate the layer number starting with 1 for the
   // first layer of the first station at a later stage by only adding
   // the layer number in the station to the number of layers in
   // previous stations

   fGeoHandler->Init();

   fRadiator->Init();

   return kSUCCESS;
}

void CbmTrdHitProducerSmearing::Exec(Option_t * option)
{
   fTrdHits->Clear();

   vector<CbmTrdHit*> hits; // Temporary storage for CbmTrdHits. Used for merging procedure.
   Int_t nofTrdPoints = fTrdPoints->GetEntries();
   for (Int_t iPoint = 0; iPoint < nofTrdPoints; iPoint++) {
      // If random value above fEfficency reject point
      if (gRandom->Rndm() > fEfficency) continue;

      CbmTrdHit* hit = CreateHit(iPoint);
      hits.push_back(hit);

      Int_t nofGhosts = (fGhostRate > 0.0) ? gRandom->Poisson(fGhostRate) : 0;
      for (Int_t iGhost = 0; iGhost < nofGhosts; iGhost++) {
         hits.push_back(CreateGhostHit(hit));
      }
   }

   if (fHitMergingDistance > 0) MergeHits(hits);

   // Copy hits to TClonesArray and clean the memory
   for (Int_t iHit = 0; iHit < hits.size(); iHit++) {
      new ((*fTrdHits)[iHit]) CbmTrdHit(*hits[iHit]);
      delete hits[iHit];
   }
   hits.clear();
}

void CbmTrdHitProducerSmearing::Finish()
{

}

CbmTrdHit* CbmTrdHitProducerSmearing::CreateHit(Int_t pointId)
{
   CbmTrdPoint* trdPoint = static_cast<CbmTrdPoint*>(fTrdPoints->At(pointId));
   const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(fMCTracks->At(trdPoint->GetTrackID()));

   Int_t detectorId = trdPoint->GetDetectorID();
   Int_t station = fGeoHandler->GetStation(detectorId);
   Int_t layer = fGeoHandler->GetLayer(detectorId);
   Int_t plane = fGeoHandler->GetPlane(detectorId);

   Double_t ELossTR = 0.0; // TR energy loss for e- and e+
   Double_t ELossdEdX = trdPoint->GetEnergyLoss(); // Ionization energy loss
   Double_t ELoss = ELossdEdX; // Total energy loss

   TVector3 mcPos, mcMom;
   trdPoint->PositionOut(mcPos);
   trdPoint->Momentum(mcMom);

   // TR: Sorry, electrons and positrons only
   if (TMath::Abs(mcTrack->GetPdgCode()) == 11) {
      ELossTR = fRadiator->GetTR(mcMom);
      ELoss += ELossTR;
   }

   Int_t rotation = layer % 2; // Rotation of the TRD planes. Even layers are rotated.

   Double_t sigmaX = 0.;
   Double_t sigmaY = 0.;
   if (rotation == 1) { // rotated x->Y  y->X
      Double_t teta = TMath::ATan(TMath::Abs(mcPos.X() / mcPos.Z())) * 1000; // mrad
      sigmaY = GetSigmaX(station);
      sigmaX = GetSigmaY(teta, station);
   } else if (rotation == 0) { // not rotated x->X  y->Y
      Double_t phi = TMath::ATan(TMath::Abs(mcPos.Y() / mcPos.Z())) * 1000; // mrad
      sigmaX = GetSigmaX(station);
      sigmaY = GetSigmaY(phi, station);
   }

   TVector3 hitPos;
   if (!fUseDigiPar) {
      Double_t errX = GetHitErr(sigmaX);
      Double_t errY = GetHitErr(sigmaY);
      hitPos.SetX(mcPos.X() + errX);
      hitPos.SetY(mcPos.Y() + errY);
      hitPos.SetZ(mcPos.Z());
   } else {
      Bool_t isOutside = true;
      Bool_t hitPosSet = false;
      Int_t counter = 0;
      do {
         Double_t errX = GetHitErr(sigmaX);
         Double_t errY = GetHitErr(sigmaY);
         Double_t hitPosX = mcPos.X() + errX;
         Double_t hitPosY = mcPos.Y() + errY;
         Double_t hitPosZ = mcPos.Z();

         // If digitization scheme is provided than check if hit position is inside the detector
         assert(fDigiPar != NULL);
         fModuleInfo = fDigiPar->GetModule(detectorId);
         Double_t moduleXmax = fModuleInfo->GetX() + fModuleInfo->GetSizex();
         Double_t moduleXmin = fModuleInfo->GetX() - fModuleInfo->GetSizex();
         Double_t moduleYmax = fModuleInfo->GetY() + fModuleInfo->GetSizey();
         Double_t moduleYmin = fModuleInfo->GetY() - fModuleInfo->GetSizey();
         isOutside = (hitPosX > moduleXmax || hitPosX < moduleXmin)
                            || (hitPosX > moduleXmax || hitPosX < moduleXmin);
         counter++;
         if (!isOutside) { // If hit position is inside active volume
            hitPos.SetX(hitPosX);
            hitPos.SetY(hitPosY);
            hitPos.SetZ(hitPosZ);
            hitPosSet = true;
            break;
         }
      } while (isOutside && counter < 10);
      if (!hitPosSet) { // If hit position was not set than use Mc position
         hitPos.SetX(mcPos.X());
         hitPos.SetY(mcPos.Y());
         hitPos.SetZ(mcPos.Z());
      }
   }

   TVector3 hitErr(sigmaX, sigmaY, 0.);

   return new CbmTrdHit(detectorId, hitPos, hitErr, 0., pointId, plane, ELossTR, ELossdEdX, ELoss);
}


CbmTrdHit* CbmTrdHitProducerSmearing::CreateGhostHit(
      const CbmTrdHit* hit)
{
   CbmTrdHit* ghostHit = new CbmTrdHit(*hit);
   // Additional errors for the ghost hit
   Double_t x = ghostHit->GetX() + gRandom->Gaus(0, fGhostDistance);
   Double_t y = ghostHit->GetY() + gRandom->Gaus(0, fGhostDistance);
   ghostHit->SetX(x);
   ghostHit->SetY(y);
   return ghostHit;
}

void CbmTrdHitProducerSmearing::MergeHits(
      vector<CbmTrdHit*>& hits)
{
   for (Int_t i = 0; i < hits.size() - 1; i++) {
      for (Int_t j = i + 1; j < hits.size(); j++) {
         CbmTrdHit* hit1 = hits[i];
         CbmTrdHit* hit2 = hits[j];
         Double_t dx = hit1->GetX() - hit2->GetX();
         Double_t dy = hit1->GetY() - hit2->GetY();
         Double_t dr = sqrt(dx * dx + dy * dy);
         if (dr < fHitMergingDistance) {
            // Move merged new hit to center between mother hits
            hit1->SetX(hit1->GetX() + 0.5 * (hit2->GetX() - hit1->GetX()));
            hit1->SetY(hit1->GetY() + 0.5 * (hit2->GetY() - hit1->GetY()));
            hit1->SetZ(hit1->GetZ() + 0.5 * (hit2->GetZ() - hit1->GetZ()));
            // Sum ELoss ELossTR ELossdEdX
            hit1->SetELossdEdx(hit1->GetELossdEdX() + hit2->GetELossdEdX());
            hit1->SetELoss(hit1->GetELoss() + hit2->GetELoss());
            hit1->SetELossTR(hit1->GetELossTR() + hit2->GetELossTR());
            delete hit2;
            hits.erase(hits.begin() + j);
         }
      }
   }
}

Double_t CbmTrdHitProducerSmearing::GetHitErr(
      Double_t sigma)
{
   Double_t err = gRandom->Gaus(0, sigma);
   return (TMath::Abs(err) < 3 * sigma) ? err : (err > 0) ? 3 * sigma : -3 * sigma;
}

void CbmTrdHitProducerSmearing::SetSigmaX(Double_t sigma[])
{
   for (Int_t i = 0; i < 3; i++) fSigmaX[i] = sigma[i];
}

void CbmTrdHitProducerSmearing::SetSigmaY(
      Double_t s1[],
      Double_t s2[],
      Double_t s3[])
{
   for (Int_t i = 0; i < 7; i++) {
      fSigmaY[0][i] = s1[i];
      fSigmaY[1][i] = s2[i];
      fSigmaY[2][i] = s3[i];
   }
}

Double_t CbmTrdHitProducerSmearing::GetSigmaX(Int_t stack) const
{
   return fSigmaX[stack - 1];
}

Double_t CbmTrdHitProducerSmearing::GetSigmaY(Double_t teta,
      Int_t stack) const
{
   if (teta <= 50) return fSigmaY[stack - 1][0];
   else if (teta > 50 && teta <= 100) return fSigmaY[stack - 1][1];
   else if (teta > 100 && teta <= 200) return fSigmaY[stack - 1][2];
   else if (teta > 200 && teta <= 300) return fSigmaY[stack - 1][3];
   else if (teta > 300 && teta <= 400) return fSigmaY[stack - 1][4];
   else if (teta > 400 && teta <= 500) return fSigmaY[stack - 1][5];
   else if (teta > 500) return fSigmaY[stack - 1][6];
   else return 0;
}

ClassImp(CbmTrdHitProducerSmearing)
