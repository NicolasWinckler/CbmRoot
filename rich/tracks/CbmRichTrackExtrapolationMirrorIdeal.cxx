/**
* \file CbmRichTrackExtrapolationMirrorIdeal.cxx
*
* \author Claudia Hoehne
* \date 2006
**/

#include "CbmRichTrackExtrapolationMirrorIdeal.h"

#include "CbmRichPoint.h"

#include "FairTrackParam.h"
#include "CbmMCTrack.h"
#include "CbmStsTrack.h"
#include "CbmTrackMatch.h"
#include "CbmGlobalTrack.h"
#include "FairRootManager.h"

#include "TClonesArray.h"
#include <iostream>

using std::cout;
using std::endl;

CbmRichTrackExtrapolationMirrorIdeal::CbmRichTrackExtrapolationMirrorIdeal():
   fRichMirrorPoints(NULL),
   fMcTracks(NULL),
   fSTSArray(NULL),
   fTrackMatchArray(NULL),

   fMinNofStsHits(4)
{

}

CbmRichTrackExtrapolationMirrorIdeal::~CbmRichTrackExtrapolationMirrorIdeal()
{

}

void CbmRichTrackExtrapolationMirrorIdeal::Init()
{
   FairRootManager* ioman = FairRootManager::Instance();
   if (NULL == ioman) {Fatal("CbmRichTrackExtrapolationMirrorIdeal::Init", "RootManager not instantised!");}

   fMcTracks  = (TClonesArray*) ioman->GetObject("MCTrack");
   if ( NULL == fMcTracks) {Fatal("CbmRichTrackExtrapolationMirrorIdeal::Init", "No MCTrack array!");}

   fRichMirrorPoints  = (TClonesArray*) ioman->GetObject("RichMirrorPoint");
   if ( NULL == fRichMirrorPoints) {Fatal("CbmRichTrackExtrapolationMirrorIdeal::Init", "No RichMirrorPoint array!");}

   fSTSArray = (TClonesArray*) ioman->GetObject("StsTrack");
   if ( NULL == fSTSArray) {Fatal("CbmRichTrackExtrapolationMirrorIdeal::Init", "No StsTrack array!");}

   fTrackMatchArray = (TClonesArray*) ioman->GetObject("StsTrackMatch");
   if ( NULL == fTrackMatchArray) {Fatal("CbmRichTrackExtrapolationMirrorIdeal::Init", "No StsTrackMatch array!");}

   cout << "---------------------- RICH Track Extrapolation Mirror Ideal ---------------------------------------" << endl;
   cout <<"-W- RichMirror Point selected for extrapolation --> ProjectionProducer needs to be run with zflag==2!!!" << endl;
   cout << "----------------------------------------------------------------------------------------------------" << endl;
}

Int_t CbmRichTrackExtrapolationMirrorIdeal::DoExtrapolate(
      TClonesArray *gTrackArray,
		Double_t z,
		TClonesArray *trackParamArray)
{
   if ( NULL == trackParamArray ) {
      cout << "-E- CbmRichTrackExtrapolationMirrorIdeal::DoExtrapolate: TrackParam Array missing!" << endl;
      return -1;
   }

   if ( NULL == gTrackArray ) {
      cout << "-E- CbmRichTrackExtrapolationMirrorIdeal::DoExtrapolate: Global Track Array missing!" << endl;
      return -1;
   }

   Double_t tx,ty,qp;
   Double_t charge = 1.;
   TMatrixFSym covMat(5);
   for(Int_t i=0;i<5;i++) for(Int_t j=0; j<=i; j++) covMat(i,j) = 0;
   covMat(0,0) = covMat(1,1) = covMat(2,2) = covMat(3,3) = covMat(4,4) = 1.e-4;

   TVector3 pos, mom;
   Int_t nTracks = gTrackArray->GetEntriesFast();
   for (Int_t iTrack=0; iTrack < nTracks; iTrack++){
      CbmGlobalTrack* gTrack = (CbmGlobalTrack*)gTrackArray->At(iTrack);
      new((*trackParamArray)[iTrack]) FairTrackParam(0.,0.,0.,0.,0.,0.,covMat);
      Int_t idSTS = gTrack->GetStsTrackIndex();

      if (idSTS < 0 ) continue;
      CbmStsTrack* pSTStr = (CbmStsTrack*) fSTSArray->At(idSTS);
      if ( NULL == pSTStr ) continue;
      Int_t Nsts = pSTStr->GetNStsHits() + pSTStr->GetNMvdHits();
      if ( Nsts >= fMinNofStsHits) {
         CbmTrackMatch* pTrackMatch = (CbmTrackMatch*)fTrackMatchArray->At(idSTS);
         if (NULL == pTrackMatch) continue;
         Int_t iMCmatch = pTrackMatch->GetMCTrackId();
         for (Int_t ii=0; ii < fRichMirrorPoints->GetEntriesFast(); ii++){
            CbmRichPoint* pMirror = (CbmRichPoint*) fRichMirrorPoints->At(ii);
            if (pMirror->GetTrackID() == iMCmatch){
               pMirror->Momentum(mom);
               pMirror->Position(pos);
               tx = mom.Px()/mom.Pz();
               ty = mom.Py()/mom.Pz();
               qp = charge/mom.Mag();
               FairTrackParam richtrack(pos.X(),pos.Y(),pos.Z(),tx,ty,qp,covMat);
               *(FairTrackParam*)(trackParamArray->At(iTrack)) = richtrack;
            }
         }
      }
   }
   return nTracks;
}

void CbmRichTrackExtrapolationMirrorIdeal::Finish()
{
}

ClassImp(CbmRichTrackExtrapolationMirrorIdeal)
