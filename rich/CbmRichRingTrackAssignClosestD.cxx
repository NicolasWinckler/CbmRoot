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
#include "CbmRootManager.h"
#include "CbmTrackParam.h"
#include "CbmGlobalTrack.h"
#include "CbmStsTrackMatch.h"
#include "CbmRichRingMatch.h"
#include "CbmTrdTrack.h"

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

// -----   Default constructor   -------------------------------------------
CbmRichRingTrackAssignClosestD::CbmRichRingTrackAssignClosestD()
{
fDistance = 1.;  /** max. distance between projected track and ring center [cm] */
fNpoints  = 5;   /** min. number of points per ring */
fVerbose  = 1;   /** verbosity level */
}

// -----   Standard constructor   -------------------------------------------
CbmRichRingTrackAssignClosestD::CbmRichRingTrackAssignClosestD(Double_t distance, Int_t npoints, Int_t verbose)
{
fDistance = distance;  /** max. distance between projected track and ring center [cm] */
fNpoints  = npoints;   /** min. number of points per ring */
fVerbose  = verbose;   /** verbosity level */
}

// -------------------------------------------------------------------------


//---------- Standard constructor with name and title  --------------------------
CbmRichRingTrackAssignClosestD::CbmRichRingTrackAssignClosestD(const char *name, const char *title,
					Double_t distance, Int_t npoints, Int_t verbose)
{
fDistance = distance;  /** max. distance between projected track and ring center [cm] */
fNpoints  = npoints;   /** min. number of points per ring */
fVerbose  = verbose;   /** verbosity level */
}

//------------Destructor -----------------------------------------------------
CbmRichRingTrackAssignClosestD::~CbmRichRingTrackAssignClosestD()
{
}
//------------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
void CbmRichRingTrackAssignClosestD::Init() 
{
	// Get and check CbmRootManager
	CbmRootManager* ioman = CbmRootManager::Instance();
	if (!ioman) {
		cout << "-E- CbmRichRingTrackAssignClosestD:Init: "
				<< "RootManager not instantised!" << endl;
		return;
	}

	// Get global track array
	gTrackArray = (TClonesArray*) ioman->GetObject("GlobalTrack");
	if ( !gTrackArray) {
		cout
				<< "-W- CbmRichRingTrackAssignClosestD::Init: No global track array!"
				<< endl;
		return;
	}

	fTrdTracks = (TClonesArray*) ioman->GetObject("TRDTrack");
	if ( !fTrdTracks) {
		cout << "-W- CbmRichElectronsQa::Init: No TRDTrack array!" << endl;
		return;
	}

}

// -----   Public method DoAssign   ------------------------------------------
void CbmRichRingTrackAssignClosestD::DoAssign(TClonesArray *pRingArray, TClonesArray* pTringArray)
{
    Int_t fNTracks = pTringArray->GetEntriesFast();
	Int_t fNRings = pRingArray->GetEntriesFast();

	CbmTrackParam *pTrack;
	CbmRichRing *pRing;

	CbmGlobalTrack *gTrack;

	Double_t xRing, yRing;
	Double_t xTrack, yTrack;
	Double_t dist_RingTrack, rMin;
	Int_t iTrackMin;
	
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
			
			pRing = (CbmRichRing*)pRingArray->At(iRing);
		
			if (pRing->GetNofHits() < fNpoints) continue;
		
			xRing = pRing->GetCenterX();
			yRing = pRing->GetCenterY();
		
			rMin = 999.;
			iTrackMin = -1;
		
			for (Int_t iTrack=0; iTrack < fNTracks; iTrack++) {
				vector<Int_t>::iterator it = find(trackIndex.begin(), trackIndex.end(), iTrack);
				if (it != trackIndex.end()) continue;
				
				pTrack = (CbmTrackParam*)pTringArray->At(iTrack);
				xTrack = pTrack->GetX();
				yTrack = pTrack->GetY();
		
				if (xTrack == 0 && yTrack == 0) continue; // no projection to photodetector plane
				  
				//if (!IsTrdElectron(iTrack)) continue;
	
				dist_RingTrack = TMath::Sqrt( (xRing-xTrack)*(xRing-xTrack) +
						(yRing-yTrack)*(yRing-yTrack) );

				if (dist_RingTrack < rMin) {
					rMin = dist_RingTrack;
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
		pRing = (CbmRichRing*)pRingArray->At(i);
		pRing->SetTrackID(trackIndex[i]);
		pRing->SetDistance(trackDist[i]);
		if (trackIndex[i] == -1) continue;
		gTrack = (CbmGlobalTrack*)gTrackArray->At(trackIndex[i]);
		gTrack->SetRichRingIndex(i);
	}
}

Bool_t CbmRichRingTrackAssignClosestD::IsTrdElectron(Int_t iTrack)
{
	CbmGlobalTrack* gTrack = (CbmGlobalTrack*)gTrackArray->At(iTrack);
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
