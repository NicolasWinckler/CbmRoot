/******************************************************************************
*  $Id: CbmRichTrackExtrapolationIdeal.cxx,v 1.3 2006/06/22 09:31:09 hoehne Exp $
*
*  Class  : CbmRichTrackExtrapolationIdeal
*  Description: This is the implementation of the TrackExtrapolation from MC points.
*               It reads the STS track array, gets the corresponding MC RefPlanePoint
*               and selects those to be projected to the Rich Photodetector
*
*  Author : Claudia Hoehne
*  E-mail : c.hoehne@gsi.de
*
*******************************************************************************
*  $Log: CbmRichTrackExtrapolationIdeal.cxx,v $
*  Revision 1.3  2006/06/22 09:31:09  hoehne
*  check if idSTS > 0 (non-STS tracks in global track get index -1)
*
*  Revision 1.2  2006/01/30 11:00:55  hoehne
*  bug fix: Point Array at z-plane was not filled correctly (counters not correct)
*
*  Revision 1.1  2006/01/26 09:53:22  hoehne
*  initial version for track extrapolation (base class + concrete implementations + task) to z-plane in RICH
*
*
*
*******************************************************************************/
#include "CbmRichTrackExtrapolationIdeal.h"

#include "CbmRichPoint.h"

// CBM includes
#include "FairTrackParam.h"
#include "CbmMCTrack.h"
#include "CbmStsTrack.h"
#include "CbmTrackMatch.h"
#include "CbmGlobalTrack.h"
#include "FairRootManager.h"

// ROOT includes
#include "TClonesArray.h"

// C++ includes
#include <iostream>

using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
CbmRichTrackExtrapolationIdeal::CbmRichTrackExtrapolationIdeal() {
  fTrackParamArray = NULL;
  fSTSArray = NULL;
  fMCPointArray = NULL;
  fMinNsts       = 4;    // minimum number of STS stations to be crossed
  fVerbose       = 1;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmRichTrackExtrapolationIdeal::CbmRichTrackExtrapolationIdeal(Int_t MinNsts,
                                                                   Int_t verbose) {
  fTrackParamArray = NULL;
  fSTSArray = NULL;
  fMCPointArray = NULL;
  fMinNsts          = MinNsts;
  fVerbose          = verbose;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmRichTrackExtrapolationIdeal::~CbmRichTrackExtrapolationIdeal() {
}
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
void CbmRichTrackExtrapolationIdeal::Init() {

  // Get and check FairRootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if (! ioman) {
    cout << "-E- CbmRichTrackExtrapolationIdeal::Init: "
	 << "RootManager not instantised!" << endl;
    return;
  }

  // Get MCTrack array
  fMCTrackArray  = (TClonesArray*) ioman->GetObject("MCTrack");
  if ( ! fMCTrackArray) {
    cout << "-E- CbmRichTrackExtrapolationIdeal::Init: No MCTrack array!"
	 << endl;
    return;
  }

  // Get MC Point array of RefPlane
  fMCPointArray  = (TClonesArray*) ioman->GetObject("RefPlanePoint");
  if ( ! fMCPointArray) {
    cout << "-E- CbmRichTrackExtrapolationIdeal::Init: No MCRefPlanePoint array!"
	 << endl;
    return;
  }

  // Get STS track array
    fSTSArray = (TClonesArray*) ioman->GetObject("StsTrack");
    if ( ! fSTSArray) {
        cout << "-W- CbmRichTrackExtrapolationIdeal::Init: No sts track array!"
	    << endl;
     }


     // get TrackMatch array
   fTrackMatchArray = (TClonesArray*) ioman->GetObject("StsTrackMatch");
   if ( ! fTrackMatchArray) {
        cout << "-W- CbmRichTrackExtrapolationIdeal::Init: No track match array!"
	    << endl;
	return;
     }


}
// -------------------------------------------------------------------------



// -----   Public method DoExtrapolate   ------------------------------------------
Int_t CbmRichTrackExtrapolationIdeal::DoExtrapolate(TClonesArray *gTrackArray, 
						    Double_t z,
                                                    TClonesArray *trackParamArray)
   {

  // Check pointers

  if ( !fMCPointArray ) {
    cout << "-E- CbmRichTrackExtrapolationIdeal::DoExtrapolate: "
	 << "MCRefPlanePoint array missing! " << endl;
    return -1;
  }

  if ( !trackParamArray ) {
    cout << "-E- CbmRichTrackExtrapolationIdeal::DoExtrapolate: "
	 << "TrackParam Array missing! " << endl;
    return -1;
  }

  if ( !gTrackArray ) {
    cout << "-E- CbmRichTrackExtrapolationIdeal::DoExtrapolate: "
	 << "Global Track Array missing! " << endl;
    return -1;
  }

   CbmStsTrack*      pSTStr = NULL;
   CbmTrackMatch* pTrackMatch = NULL;
   CbmRichPoint*     pRefPlane = NULL;

   // some default variables
   Double_t tx,ty,qp;
   Double_t charge = 1.;
   TMatrixFSym covMat(5);
   for(Int_t i=0;i<5;i++) for(Int_t j=0; j<=i; j++) covMat(i,j) = 0;
   covMat(0,0) = covMat(1,1) = covMat(2,2) = covMat(3,3) = covMat(4,4) = 1.e-4;

   TVector3 pos, mom;

   Int_t Nsts = 0;

   Int_t nTracks = gTrackArray->GetEntriesFast();
   for (Int_t iTrack=0; iTrack < nTracks; iTrack++){
     CbmGlobalTrack* gTrack = (CbmGlobalTrack*)gTrackArray->At(iTrack);
     new((*trackParamArray)[iTrack]) FairTrackParam(0.,0.,0.,0.,0.,0.,covMat);
     Int_t idSTS = gTrack->GetStsTrackIndex();
     if (idSTS < 0 ) continue;     // global tracks having no STS part get STSIndex -1
     if (fVerbose > 1) cout << "-I- Ideal-Trackextrapolation, iTrack(STS) " << idSTS
                             << " pointer " << (CbmStsTrack*)fSTSArray->At(idSTS) << endl;

     pSTStr = (CbmStsTrack*)fSTSArray->At(idSTS);
     if ( ! pSTStr ) continue;
     Nsts = pSTStr->GetNStsHits() + pSTStr->GetNMvdHits();
     if (fVerbose > 1) cout << "-I- Ideal-Trackextrapolation, iTrack(global), iTrack(STS), Nsts: " << iTrack << " " << idSTS << " " << Nsts << endl;
     if ( Nsts >= fMinNsts) {
       pTrackMatch = (CbmTrackMatch*)fTrackMatchArray->At(idSTS);
       if (!pTrackMatch) cout << "no matched track!: fake?"<< endl;
       if (!pTrackMatch) continue;
       Int_t iMCmatch = pTrackMatch->GetMCTrackId();
       for (Int_t ii=0; ii < fMCPointArray->GetEntriesFast(); ii++){
         pRefPlane = (CbmRichPoint*)fMCPointArray->At(ii);
	 if (pRefPlane->GetTrackID() == iMCmatch){
           pRefPlane->Momentum(mom);
           pRefPlane->Position(pos);
           tx = mom.Px()/mom.Pz();
           ty = mom.Py()/mom.Pz();
           qp = charge/mom.Mag();
	   FairTrackParam richtrack(pos.X(),pos.Y(),pos.Z(),tx,ty,qp,covMat);
           * (FairTrackParam*)(trackParamArray->At(iTrack)) = richtrack;

           if (fVerbose > 1) {
	      cout << "-I- Ideal Track Extrapolation - extrapolate track " << iTrack << endl;
              cout << " ....... MCid - PDGcode: " << iMCmatch << " " <<
	             ((CbmMCTrack*)fMCTrackArray->At(iMCmatch))->GetPdgCode() << " " <<
		      endl;
	     }
	 }  // select MCPoint to that track
       }  // loop MCPoints

     }  // require fMinNsts

   } // loop tracks

    return nTracks;

}
// -------------------------------------------------------------------------

// -----   Public method Finish   ------------------------------------------
  void CbmRichTrackExtrapolationIdeal::Finish() {
  }


//-----------------------------------------------------------------------------------
ClassImp(CbmRichTrackExtrapolationIdeal)
