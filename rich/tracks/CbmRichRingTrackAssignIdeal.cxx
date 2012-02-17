/******************************************************************************
*  $Id: CbmRichRingTrackAssignIdeal.cxx,v 1.3 2006/08/02 07:50:10 hoehne Exp $
*
*  Class  : CbmRichRingTrackAssignIdeal
*  Description: This is the implementation of a particular ring-track assigner
*               ideal assignement (distance parameter of no effect!)
*
*  Author : Claudia Hoehne
*  E-mail : C.Hoehne@gsi.de
*
*******************************************************************************
*  $Log: CbmRichRingTrackAssignIdeal.cxx,v $
*  Revision 1.3  2006/08/02 07:50:10  hoehne
*  MCTrackID in RingMatch might be -1 (no MC ring assigned) -> check for this!
*  (same for StsTrack)
*
*  Revision 1.2  2006/07/12 06:29:11  hoehne
*  SetDistance added: distance between ring center and track assigned to ring
*
*  Revision 1.1  2006/02/23 14:56:00  hoehne
*  initial version: Ideal ring-track assignement
*
*
*
*******************************************************************************/
#include "CbmRichRingTrackAssignIdeal.h"

#include "CbmRichRing.h"

#include "CbmMCTrack.h"
#include "FairRootManager.h"
#include "FairTrackParam.h"
#include "CbmGlobalTrack.h"
#include "CbmTrackMatch.h"
#include "CbmRichRingMatch.h"

#include "TParticle.h"
#include "TArrayD.h"
#include "TVector3.h"
#include "TLorentzVector.h"

#include <iostream>

using std::cout;
using std::endl;

CbmRichRingTrackAssignIdeal::CbmRichRingTrackAssignIdeal()
{
   fMaxDistance = 100.;
   fMinNofHitsInRing  = 5;
}

CbmRichRingTrackAssignIdeal::~CbmRichRingTrackAssignIdeal()
{
}

void CbmRichRingTrackAssignIdeal::Init()
{
   FairRootManager* ioman = FairRootManager::Instance();
   if (NULL == ioman) { Fatal("CbmRichRingTrackAssignIdeal::Init", "RootManager not instantised!");}

   fMcTracks  = (TClonesArray*) ioman->GetObject("MCTrack");
   if ( NULL == fMcTracks) {Fatal("CbmRichRingTrackAssignIdeal::Init", "No MCTrack array!");}

   fGlobalTracks = (TClonesArray*) ioman->GetObject("GlobalTrack");
   if ( NULL == fGlobalTracks) {Fatal("CbmRichRingTrackAssignIdeal::Init", "No GlobalTrack array!");}

   fRingMatches = (TClonesArray*) ioman->GetObject("RichRingMatch");
   if ( NULL == fRingMatches) {Fatal("CbmRichRingTrackAssignIdeal::Init", "No RichRingMatch array!");}

   fStsTrackMatches = (TClonesArray*) ioman->GetObject("StsTrackMatch");
   if ( NULL == fStsTrackMatches) {Fatal("CbmRichRingTrackAssignIdeal::Init", "No StsTrackMatch array!");}
}

void CbmRichRingTrackAssignIdeal::DoAssign(
      TClonesArray *rings,
      TClonesArray* richProj)
{
   Int_t fNTracks = richProj->GetEntriesFast();
   Int_t fNRings = rings->GetEntriesFast();

   //some statistics
   Int_t nAssign = 0;
   Int_t nCorrect = 0;
   Int_t nEl = 0;
   Int_t nCorEl = 0;

   for (Int_t iRing=0; iRing < fNRings; iRing++){
      CbmRichRing* pRing = (CbmRichRing*) rings->At(iRing);
      if (pRing->GetNofHits() < fMinNofHitsInRing) continue;

      if (pRing->GetTrackID() != -1) cout << "-E- Ring Track ID of found Rings != -1" << endl;

      CbmRichRingMatch* pRingMatch = (CbmRichRingMatch*) fRingMatches->At(iRing);
      Int_t ringID = pRingMatch->GetMCTrackID();
      Double_t xRing = pRing->GetCenterX();
      Double_t yRing = pRing->GetCenterY();

      for (Int_t iTrack=0; iTrack < fNTracks; iTrack++){
         FairTrackParam* pTrack = (FairTrackParam*) richProj->At(iTrack);
         Double_t xTrack = pTrack->GetX();
         Double_t yTrack = pTrack->GetY();

         // no projection to photodetector plane
         if (xTrack == 0 && yTrack == 0) continue;

         CbmGlobalTrack* gTrack = (CbmGlobalTrack*) fGlobalTracks->At(iTrack);
         if (gTrack->GetStsTrackIndex() == -1) continue;
         CbmTrackMatch* pTrackMatch = (CbmTrackMatch*) fStsTrackMatches->At(gTrack->GetStsTrackIndex());

         if (pTrackMatch->GetMCTrackId() == ringID){
            gTrack -> SetRichRingIndex(iRing);
            pRing -> SetTrackID(iTrack);
            nAssign++;
            Double_t dist = TMath::Sqrt( (xRing-xTrack)*(xRing-xTrack)+(yRing-yTrack)*(yRing-yTrack) );
            pRing->SetDistance(dist);

            if (pRingMatch->GetMCTrackID() != -1){
               if (TMath::Abs(((CbmMCTrack*) fMcTracks->At(pRingMatch->GetMCTrackID()))->GetPdgCode())==11) nEl++;
               if (pRingMatch->GetMCTrackID() == pTrackMatch->GetMCTrackId()) {
                  nCorrect++;
                  if (TMath::Abs(((CbmMCTrack*) fMcTracks->At(pRingMatch->GetMCTrackID()))->GetPdgCode())==11) nCorEl++;
               }
            }
         } // ideal assignement
      } // loop tracks
   } // loop rings

   cout << endl;
   cout << "-I- Ring-Track Assignement" << endl;
   cout << "maximum distance between rings and tracks: " << fMaxDistance << " cm" << endl;
   cout << "extrapolated tracks: " << fNTracks << endl;
   cout << "found rings: " << fNRings << endl;
   cout << "rings attached to track: " << nAssign << endl;
   if (nAssign !=0) cout << " % of correctly attached rings: " << (Double_t)(nCorrect)/(Double_t)(nAssign) << endl;
   cout << "Electron rings attached to track " << nEl << endl;
   if (nEl !=0 ) cout << " % of correctly attached Electron rings: " << (Double_t)(nCorEl)/(Double_t)(nEl) << endl;
   cout << endl;
}

ClassImp(CbmRichRingTrackAssignIdeal)
