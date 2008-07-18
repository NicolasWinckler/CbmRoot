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

#include "TParticle.h"
#include "TArrayD.h"
#include "TVector3.h"
#include "TLorentzVector.h"

#include <iostream>

using std::cout;
using std::endl;


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
void CbmRichRingTrackAssignClosestD::Init() {

// Get and check CbmRootManager
  CbmRootManager* ioman = CbmRootManager::Instance();
  if (! ioman) {
    cout << "-E- CbmRichRingTrackAssignClosestD:Init: "
	 << "RootManager not instantised!" << endl;
    return;
  }

  // Get MCTrack array
  fMCTrackArray  = (TClonesArray*) ioman->GetObject("MCTrack");
  if ( ! fMCTrackArray) {
    cout << "-E- CbmRichRingTrackAssignClosestD::Init: No MCTrack array!"
	 << endl;
    return;
  }

// Get global track array
    gTrackArray = (TClonesArray*) ioman->GetObject("GlobalTrack");
    if ( ! gTrackArray) {
        cout << "-W- CbmRichRingTrackAssignClosestD::Init: No global track array!"
	    << endl;
	return;
     }

  // get RichRingMatch array
  fRingMatchArray = (TClonesArray*) ioman->GetObject("RichRingMatch");
  if ( ! fRingMatchArray) {
        cout << "-W- CbmRichRingTrackAssignClosestD::Init: No ring match array!"
	    << endl;
	return;
     }

   // get TrackMatch array
  fTrackMatchArray = (TClonesArray*) ioman->GetObject("STSTrackMatch");
  if ( ! fTrackMatchArray) {
        cout << "-W- CbmRichRingTrackAssignClosestD::Init: No track match array!"
	    << endl;
	return;
     }

  }

// -----   Public method DoAssign   ------------------------------------------
void CbmRichRingTrackAssignClosestD::DoAssign(TClonesArray *pRingArray, TClonesArray* pTringArray)
{


  Int_t fNTracks = pTringArray->GetEntriesFast();
  Int_t fNRings  = pRingArray->GetEntriesFast();

  //some statistics
  Int_t nAssign = 0;
  Int_t nCorrect = 0;
  Int_t nEl = 0;
  Int_t nCorEl = 0;

  CbmTrackParam *pTrack;
  CbmRichRing *pRing;
  CbmRichRing *pRingT;
  CbmGlobalTrack *gTrack;
  CbmRichRingMatch *pRingMatch;
  CbmStsTrackMatch *pTrackMatch;

  Double_t xRing, yRing;
  Double_t xTrack, yTrack;
  Double_t dist_RingTrack, rMin;
  Int_t    iTrackMin;
  Double_t xtest, ytest;

  for (Int_t iRing=0; iRing < fNRings; iRing++){
    pRing = (CbmRichRing*)pRingArray->At(iRing);
    if (fVerbose > 1) cout << "-I- RingTrackAssignClostestD: check ring " << iRing << endl;

    if (pRing->GetNofHits() < fNpoints) continue;

    xRing = pRing->GetCenterX();
    yRing = pRing->GetCenterY();
    if (pRing->GetTrackID() != -1) cout << "-E- Ring Track ID of found Rings != -1" << endl;

    rMin = 999.;
    iTrackMin = -1;
    xtest=0.;
    ytest=0.;

    for (Int_t iTrack=0; iTrack < fNTracks; iTrack++){
      pTrack = (CbmTrackParam*)pTringArray->At(iTrack);
      xTrack = pTrack->GetX();
      yTrack = pTrack->GetY();
      
      if (xTrack == 0 && yTrack == 0) continue;          // no projection to photodetector plane

      dist_RingTrack = TMath::Sqrt( (xRing-xTrack)*(xRing-xTrack) +
				            (yRing-yTrack)*(yRing-yTrack) );

    // search for ring-track combination with shortest distance
      if (dist_RingTrack < rMin){
	rMin = dist_RingTrack;
        iTrackMin = iTrack;
      }
   } // loop tracks

   dist_RingTrack = 999.;
   if (rMin != 999.){
     if (fVerbose > 1) cout << " -I- RingTrackAssignClostestD: distance of ring " << iRing <<
                       " and track " << iTrackMin << " = " << rMin << endl;
     if (rMin < fDistance){
       if (iTrackMin == -1) {
          cout << " -E- RingTrackAssignClosestD: iTrackMin = -1 " << endl;
	  continue;
	  }
       pTrack = (CbmTrackParam*)pTringArray->At(iTrackMin);
       xTrack = pTrack->GetX();
       yTrack = pTrack->GetY();
       if (iRing != 0){
         for (Int_t ii=0; ii < iRing; ii++){  //check that this track was not yet assigned to another ring
           pRingT = (CbmRichRing*)pRingArray->At(ii);
 	   if (pRingT->GetTrackID() != -1){
           if (pRingT->GetTrackID() == iTrackMin){
            xRing = pRingT->GetCenterX();
            yRing = pRingT->GetCenterY();
            dist_RingTrack = TMath::Sqrt( (xRing-xTrack)*(xRing-xTrack) +
				            (yRing-yTrack)*(yRing-yTrack) );
           }
        }
        }
       }
      if (dist_RingTrack < rMin) continue;
      if (iTrackMin == -1) {
         cout << "-E- RingTrackAssignClostestD: trackID = -1" << endl;
	 continue;
	 }
      if (gTrackArray->GetEntriesFast() < iTrackMin) {
         cout << "-E- RingTrackAssignClostestD: too high track number " << endl;
	 continue;
	 }
      gTrack = (CbmGlobalTrack*)gTrackArray->At(iTrackMin);
      gTrack -> SetRichRingIndex(iRing);
      pRing -> SetTrackID(iTrackMin);
      pRing -> SetDistance(rMin);
      nAssign++;

      if (fVerbose > 1) cout << "-I- Ring-Track Assignement: iRing " <<
                       iRing << " and iTrack " << iTrackMin <<
		       " in GlobalTrack " << gTrack -> GetRichRingIndex() <<
		       " with distance " << rMin << endl;

      pRingMatch = (CbmRichRingMatch*)fRingMatchArray->At(iRing);
      
      if (gTrack->GetStsTrackIndex() == -1) {
        if (fVerbose > 1) cout << "-W- CbmRichRingTrackAssignClosestD: gTrack->GetStsTrackIndex() = -1 " << endl;
	 continue;
      }
      
      pTrackMatch = (CbmStsTrackMatch*)fTrackMatchArray->At(gTrack->GetStsTrackIndex());
      
      if (pRingMatch->GetMCTrackID() == -1) {
	   cout << " -E- RingTrackAssignClosestD: pRingMatch->GetMCTrackID() = -1 " << endl;
	  }
      if (pTrackMatch->GetMCTrackId() == -1) {
	   cout << " -E- RingTrackAssignClosestD: pTrackMatch->GetMCTrackID() = -1 " << endl;
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

     }
   }

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
    cout << " % of correctly attached rings: " << (Double_t)(nCorrect)/(Double_t)(nAssign) << endl;
    cout << "Electron rings attached to track " << nEl << endl;
    cout << " % of correctly attached Electron rings: " << (Double_t)(nCorEl)/(Double_t)(nEl) << endl;
    cout << endl;
    cout << "--------------------------------------------------------------------------------------"
	 << endl;
  }

}




//------------------------------------------------------------------------------
ClassImp(CbmRichRingTrackAssignClosestD)
