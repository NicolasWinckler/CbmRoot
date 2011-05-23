/** CbmLitReconstructionQa.cxx
 * @author Semen Lebedev <s.lebedev@gsi.de>
 * @since 2011
 * @version 1.0
 **/

#include "elid/CbmLitGlobalElectronId.h"
#include "FairRootManager.h"
#include "CbmRichRing.h"
#include "CbmTrdTrack.h"
#include "CbmTofHit.h"
#include "CbmRichElectronIdAnn.h"
#include "CbmGlobalTrack.h"

#include "TMath.h"
#include "TString.h"
#include "TClonesArray.h"
#include "TSystem.h"

#include <cmath>

CbmLitGlobalElectronId::CbmLitGlobalElectronId():
   fTrdAnnCut(0.85),
   fRichAnnCut(-0.5),
   fRichUseAnn(true),
   fRichMeanA(-1.),
   fRichMeanB(-1.),
   fRichRmsA(-1.),
   fRichRmsB(-1.),
   fRichRmsCoeff(-1.),
   fRichDistCut(-1.),

   fRichElIdAnn(NULL),
   fRichRings(NULL),
   fTrdTracks(NULL),
   fTofHits(NULL)
{

}

CbmLitGlobalElectronId::~CbmLitGlobalElectronId()
{

}

void CbmLitGlobalElectronId::Init()
{
	FairRootManager* ioman = FairRootManager::Instance();
	if (NULL == ioman) {Fatal("CbmLitGlobalElectronId::Init:", "RootManager not initialized!");}

	fRichRings = (TClonesArray*) ioman->GetObject("RichRing");
	if (NULL == fRichRings) {Fatal("CbmLitGlobalElectronId::Init","No RichRing array!");}

	fTrdTracks = (TClonesArray*) ioman->GetObject("TrdTrack");
	if ( !fTrdTracks) {Fatal("CbmLitGlobalElectronId::Init","No TrdTrack array!");}

	fTofHits = (TClonesArray*) ioman->GetObject("TofHit");
	if ( !fTofHits) {Fatal("CbmLitGlobalElectronId::Init","No TOFHit array!");}

    if (fRichUseAnn){
        TString richANNFile = gSystem->Getenv("VMCWORKDIR");
        richANNFile += "/parameters/rich/el_id_ann_weights_rich_compact.txt";
        fRichElIdAnn = new CbmRichElectronIdAnn(0, richANNFile);
        fRichElIdAnn->Init();
    }
}

Bool_t CbmLitGlobalElectronId::IsRichElectron(
		const CbmGlobalTrack* globalTrack,
		Double_t momentum)
{
   if (NULL == globalTrack) return false;
   Int_t richId = globalTrack->GetRichRingIndex();
   if (richId < 0) return false;
   CbmRichRing* ring = static_cast<CbmRichRing*> (fRichRings->At(richId));
   if (NULL == ring) return false;

   if (fRichUseAnn == false) {
      Double_t axisA = ring->GetAaxis();
      Double_t axisB = ring->GetBaxis();
      Double_t dist = ring->GetDistance();
      if (fabs(axisA - fRichMeanA) < fRichRmsCoeff * fRichRmsA && fabs(axisB
            - fRichMeanB) < fRichRmsCoeff * fRichRmsB && dist < fRichDistCut) {
         return true;
      } else {
         return false;
      }
   } else {
      Double_t ann = fRichElIdAnn->DoSelect(ring, momentum);
      if (ann > fRichAnnCut) return true;
      else return false;
   }
}

Bool_t CbmLitGlobalElectronId::IsTrdElectron(
		const CbmGlobalTrack* globalTrack,
		Double_t momentum)
{
   if (NULL == globalTrack) return false;
   Int_t trdId = globalTrack->GetTrdTrackIndex();
   if (trdId < 0) return false;
   CbmTrdTrack* trdTrack = static_cast<CbmTrdTrack*>(fTrdTracks->At(trdId));
   if (NULL == trdTrack) return false;

   Double_t ann = trdTrack->GetPidANN();
   if (ann > fTrdAnnCut) return true;
   else return false;
}

Bool_t CbmLitGlobalElectronId::IsTofElectron(
		const CbmGlobalTrack* globalTrack,
		Double_t momentum)
{
   Double_t trackLength = globalTrack->GetLength() / 100.;

   Int_t tofId = globalTrack->GetTofHitIndex();
   if (tofId < 0) return false;
   CbmTofHit* tofHit = (CbmTofHit*) fTofHits->At(tofId);
   if (NULL == tofHit)return false;

   Double_t time = 0.2998 * tofHit->GetTime(); // time in ns -> transfrom to ct in m
   Double_t mass2 = TMath::Power(momentum, 2.) * (TMath::Power(time/ trackLength, 2) - 1);

   if (momentum >= 1.) {
       if (mass2 < (0.013*momentum - 0.003)){
           return true;
       }
   } else {
       if (mass2 < 0.01){
           return true;//fTofM2
       }
   }
   return false;
}

ClassImp(CbmLitGlobalElectronId);
