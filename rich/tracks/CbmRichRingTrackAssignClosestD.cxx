/******************************************************************************
*  $Id: CbmRichRingTrackAssignClosestD.cxx,v 1.3 2006/08/02 07:29:58 hoehne Exp $
*
*  Class  : CbmRichRingTrackAssignClostestD
*  Description: This is the implementation of a particular ring-track assigner
*               Criterium: closest distance
*
*  Author : Claudia Hoehne
*  E-mail : C.Hoehne@gsi.de
*
*******************************************************************************
*  $Log: CbmRichRingTrackAssignClosestD.cxx,v $
*  Revision 1.3  2006/08/02 07:29:58  hoehne
*  MCTrackID in RingMatch might be -1 (no MC ring assigned) -> check for this!
*
*  Revision 1.2  2006/07/12 06:28:51  hoehne
*  SetDistance added: distance between ring center and track assigned to ring
*
*  Revision 1.1  2006/01/26 09:54:27  hoehne
*  initial version: assignement of Rich rings and extrapolated tracks (base class, concrete implementation, Task)
*
*
*
*******************************************************************************/
#include "CbmRichRingTrackAssignClosestD.h"

#include "CbmRichRing.h"

#include "CbmMCTrack.h"
#include "FairRootManager.h"
#include "FairTrackParam.h"
#include "CbmGlobalTrack.h"
#include "CbmRichRingMatch.h"
#include "CbmTrdTrack.h"

#include "TClonesArray.h"
#include "TParticle.h"
#include "TArrayD.h"
#include "TVector3.h"
#include "TLorentzVector.h"

#include <iostream>
#include <algorithm>
#include <vector>

using std::cout;
using std::endl;
using std::vector;

CbmRichRingTrackAssignClosestD::CbmRichRingTrackAssignClosestD()
{
   fMaxDistance = 100.;
   fMinNofHitsInRing = 5;
}

CbmRichRingTrackAssignClosestD::~CbmRichRingTrackAssignClosestD()
{
}

void CbmRichRingTrackAssignClosestD::Init()
{
	FairRootManager* ioman = FairRootManager::Instance();
	if (NULL == ioman) {Fatal("CbmRichRingTrackAssignClosestD::Init", "RootManager not instantised!");}

	fGlobalTracks = (TClonesArray*) ioman->GetObject("GlobalTrack");
	if (NULL == fGlobalTracks) {Fatal("CbmRichRingTrackAssignClosestD::Init", "No GlobalTrack array!");}

	fTrdTracks = (TClonesArray*) ioman->GetObject("TrdTrack");
	if (NULL == fTrdTracks) {Fatal("CbmRichRingTrackAssignClosestD::Init", "No TrdTrack array!");}
}

void CbmRichRingTrackAssignClosestD::DoAssign(
      TClonesArray *pRingArray,
      TClonesArray* pTringArray)
{
   Int_t fNTracks = pTringArray->GetEntriesFast();
	Int_t fNRings = pRingArray->GetEntriesFast();

	vector<Int_t> trackIndex;
	vector<Double_t> trackDist;
	trackIndex.resize(fNRings);
	trackDist.resize(fNRings);
	for (UInt_t i = 0; i < trackIndex.size(); i++){
		trackIndex[i] = -1;
		trackDist[i] = 999.;
	}
	for (Int_t iIter = 0; iIter < 4; iIter++){
		for (Int_t iRing=0; iRing < fNRings; iRing++) {
			if (trackIndex[iRing] != -1) continue;

			CbmRichRing* pRing = (CbmRichRing*)pRingArray->At(iRing);

			if (pRing->GetNofHits() < fMinNofHitsInRing) continue;

			Double_t xRing = pRing->GetCenterX();
			Double_t yRing = pRing->GetCenterY();

			Double_t rMin = 999.;
			Int_t iTrackMin = -1;

			for (Int_t iTrack=0; iTrack < fNTracks; iTrack++) {
				vector<Int_t>::iterator it = find(trackIndex.begin(), trackIndex.end(), iTrack);
				if (it != trackIndex.end()) continue;

				FairTrackParam* pTrack = (FairTrackParam*)pTringArray->At(iTrack);
				Double_t xTrack = pTrack->GetX();
				Double_t yTrack = pTrack->GetY();

				// no projection to photodetector plane
				if (xTrack == 0 && yTrack == 0) continue;

				//if (!IsTrdElectron(iTrack)) continue;

				Double_t dist = TMath::Sqrt( (xRing-xTrack)*(xRing-xTrack) +
						(yRing-yTrack)*(yRing-yTrack) );

				if (dist < rMin) {
					rMin = dist;
					iTrackMin = iTrack;
				}
			} // loop tracks
			trackIndex[iRing] = iTrackMin;
			trackDist[iRing] = rMin;
		}//loop rings

		for (UInt_t i1 = 0; i1 < trackIndex.size(); i1++){
			for (UInt_t i2 = 0; i2 < trackIndex.size(); i2++){
				if (i1 == i2) continue;
				if (trackIndex[i1] == trackIndex[i2] && trackIndex[i1] != -1){
					if (trackDist[i1] >= trackDist[i2]){
						trackDist[i1] = 999.;
						trackIndex[i1] = -1;
					}else{
						trackDist[i2] = 999.;
						trackIndex[i2] = -1;
					}
				}
			}
		}
	}//iIter

	// fill global tracks
	for (UInt_t i = 0; i < trackIndex.size(); i++){
		CbmRichRing* pRing = (CbmRichRing*)pRingArray->At(i);
		pRing->SetTrackID(trackIndex[i]);
		pRing->SetDistance(trackDist[i]);
		if (trackIndex[i] == -1) continue;
		CbmGlobalTrack* gTrack = (CbmGlobalTrack*) fGlobalTracks->At(trackIndex[i]);
		gTrack->SetRichRingIndex(i);
	}
}

Bool_t CbmRichRingTrackAssignClosestD::IsTrdElectron(
      Int_t iTrack)
{
	CbmGlobalTrack* gTrack = (CbmGlobalTrack*) fGlobalTracks->At(iTrack);
	Int_t trdIndex = gTrack->GetTrdTrackIndex();
	if (trdIndex == -1) return false;

	CbmTrdTrack* trdTrack = (CbmTrdTrack*)fTrdTracks->At(trdIndex);
    if (!trdTrack)return false;

    if (trdTrack->GetPidANN() > -0.5) {
    	return true;
    }

    return false;

}

//------------------------------------------------------------------------------
ClassImp(CbmRichRingTrackAssignClosestD)
