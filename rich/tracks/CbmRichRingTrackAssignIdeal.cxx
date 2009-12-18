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


// -----   Default constructor   -------------------------------------------
CbmRichRingTrackAssignIdeal::CbmRichRingTrackAssignIdeal()
{
fDistance = 1.;  /** max. distance between projected track and ring center [cm] */
fNpoints  = 5;   /** min. number of points per ring */
fVerbose  = 1;   /** verbosity level */
}

// -----   Standard constructor   -------------------------------------------
CbmRichRingTrackAssignIdeal::CbmRichRingTrackAssignIdeal(Double_t distance, Int_t npoints, Int_t verbose)
{
fDistance = distance;  /** max. distance between projected track and ring center [cm] */
fNpoints  = npoints;   /** min. number of points per ring */
fVerbose  = verbose;   /** verbosity level */
}

// -------------------------------------------------------------------------


//---------- Standard constructor with name and title  --------------------------
CbmRichRingTrackAssignIdeal::CbmRichRingTrackAssignIdeal(const char *name, const char *title,
					Double_t distance, Int_t npoints, Int_t verbose)
{
fDistance = distance;  /** max. distance between projected track and ring center [cm] */
fNpoints  = npoints;   /** min. number of points per ring */
fVerbose  = verbose;   /** verbosity level */
}

//------------Destructor -----------------------------------------------------
CbmRichRingTrackAssignIdeal::~CbmRichRingTrackAssignIdeal()
{
}
//------------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
void CbmRichRingTrackAssignIdeal::Init() {

// Get and check FairRootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if (! ioman) {
    cout << "-E- CbmRichRingTrackAssignIdeal:Init: "
	 << "RootManager not instantised!" << endl;
    return;
  }

  // Get MCTrack array
  fMCTrackArray  = (TClonesArray*) ioman->GetObject("MCTrack");
  if ( ! fMCTrackArray) {
    cout << "-E- CbmRichRingTrackAssignIdeal::Init: No MCTrack array!"
	 << endl;
    return;
  }

// Get global track array
    gTrackArray = (TClonesArray*) ioman->GetObject("GlobalTrack");
    if ( ! gTrackArray) {
        cout << "-W- CbmRichRingTrackAssignIdeal::Init: No global track array!"
	    << endl;
	return;
     }

  // get RichRingMatch array
  fRingMatchArray = (TClonesArray*) ioman->GetObject("RichRingMatch");
  if ( ! fRingMatchArray) {
        cout << "-W- CbmRichRingTrackAssignIdeal::Init: No ring match array!"
	    << endl;
	return;
     }

   // get TrackMatch array
  fTrackMatchArray = (TClonesArray*) ioman->GetObject("StsTrackMatch");
  if ( ! fTrackMatchArray) {
        cout << "-W- CbmRichRingTrackAssignIdeal::Init: No track match array!"
	    << endl;
	return;
     }

  }

// -----   Public method DoAssign   ------------------------------------------
void CbmRichRingTrackAssignIdeal::DoAssign(TClonesArray *pRingArray, TClonesArray* pTringArray)
{


  Int_t fNTracks = pTringArray->GetEntriesFast();
  Int_t fNRings  = pRingArray->GetEntriesFast();

  //some statistics
  Int_t nAssign = 0;
  Int_t nCorrect = 0;
  Int_t nEl = 0;
  Int_t nCorEl = 0;

  FairTrackParam *pTrack;
  CbmRichRing *pRing;
//  CbmRichRing *pRingT;
  CbmGlobalTrack *gTrack;
  CbmRichRingMatch *pRingMatch;
  CbmTrackMatch *pTrackMatch;

  Int_t ringID;

  Double_t xRing, yRing;
  Double_t xTrack, yTrack;
  Double_t dist_RingTrack;


  for (Int_t iRing=0; iRing < fNRings; iRing++){
    pRing = (CbmRichRing*)pRingArray->At(iRing);

    if (fVerbose > 1) cout << "-I- RingTrackAssignIdeal: check ring " << iRing << endl;

    if (pRing->GetNofHits() < fNpoints) continue;

    if (pRing->GetTrackID() != -1) cout << "-E- Ring Track ID of found Rings != -1" << endl;

    pRingMatch = (CbmRichRingMatch*)fRingMatchArray->At(iRing);
    ringID = pRingMatch->GetMCTrackID();
    xRing = pRing->GetCenterX();
    yRing = pRing->GetCenterY();

    for (Int_t iTrack=0; iTrack < fNTracks; iTrack++){
      pTrack = (FairTrackParam*)pTringArray->At(iTrack);
      xTrack = pTrack->GetX();
      yTrack = pTrack->GetY();

      if (xTrack == 0 && yTrack == 0) continue;          // no projection to photodetector plane

      gTrack = (CbmGlobalTrack*)gTrackArray->At(iTrack);

      if (gTrack->GetStsTrackIndex() == -1) {
      if (fVerbose> 1) cout << "-W- CbmRichRingTrackAssignIdeal: gTrack->GetStsTrackIndex() = -1 " << endl;
	 continue;
      }

      pTrackMatch = (CbmTrackMatch*)fTrackMatchArray->At(gTrack->GetStsTrackIndex());

      if (pTrackMatch->GetMCTrackId() == ringID){
         gTrack -> SetRichRingIndex(iRing);
         pRing -> SetTrackID(iTrack);
	 nAssign++;
         dist_RingTrack = TMath::Sqrt( (xRing-xTrack)*(xRing-xTrack) +
				            (yRing-yTrack)*(yRing-yTrack) );
	 pRing -> SetDistance(dist_RingTrack);

	if (fVerbose > 1) cout << " -I- RingTrackAssignIdeal: distance of ring " << iRing <<
                       " and track " << iTrack << " = " << dist_RingTrack << endl;

	if (pRingMatch->GetMCTrackID() == -1){
          cout << "-W- CbmRichRingTrackAssignIdeal: pRingMatch->GetMCTrackID() = -1 " << endl;
         }
	if (pRingMatch->GetMCTrackID() != -1){
        if (TMath::Abs(((CbmMCTrack*)fMCTrackArray->At(pRingMatch->GetMCTrackID()))->GetPdgCode())==11) nEl++;
        if (pRingMatch->GetMCTrackID() == pTrackMatch->GetMCTrackId()) {
         nCorrect++;
	 if (TMath::Abs(((CbmMCTrack*)fMCTrackArray->At(pRingMatch->GetMCTrackID()))->GetPdgCode())==11) nCorEl++;
	 }
	 }
        if (fVerbose >1) cout << "... MCTrack ID of ring " << pRingMatch->GetMCTrackID() <<
	                         " and track " << pTrackMatch->GetMCTrackId() << endl;
	if ((fVerbose >1) && (pRingMatch->GetMCTrackID() != -1) && (pTrackMatch->GetMCTrackId() != -1))
              cout << " ... PdgCode of ring " <<
             ((CbmMCTrack*)fMCTrackArray->At(pRingMatch->GetMCTrackID()))->GetPdgCode() <<
	     " and track " <<
	     ((CbmMCTrack*)fMCTrackArray->At(pTrackMatch->GetMCTrackId()))->GetPdgCode() <<
	     " Ring Radius r = " << pRing->GetRadius() <<
	     " cm --- Track Momentum p = " <<
		((CbmMCTrack*)fMCTrackArray->At(pTrackMatch->GetMCTrackId()))->GetP() <<
	     " GeV" << endl;

	}   // ideal assignement

   } // loop tracks

   } // loop rings


   if (fVerbose) {
    cout << endl;
    cout << "--------------------------------------------------------------------------------------"
	 << endl;
    cout << "-I-           Ring-Track Assignement                                               -I-"
	 << endl;
    cout << "maximum distance between rings and tracks: " << fDistance << " cm" << endl;
    cout << "extrapolated tracks: " << fNTracks << endl;
    cout << "found rings: " << fNRings << endl;
    cout << "rings attached to track: " << nAssign << endl;
    if (nAssign !=0) cout << " % of correctly attached rings: " << (Double_t)(nCorrect)/(Double_t)(nAssign) << endl;
    cout << "Electron rings attached to track " << nEl << endl;
    if (nEl !=0 ) cout << " % of correctly attached Electron rings: " << (Double_t)(nCorEl)/(Double_t)(nEl) << endl;
    cout << endl;
    cout << "--------------------------------------------------------------------------------------"
	 << endl;
  }

}




//------------------------------------------------------------------------------
ClassImp(CbmRichRingTrackAssignIdeal)
