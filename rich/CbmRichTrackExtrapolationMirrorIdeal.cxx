/******************************************************************************
*  $Id: CbmRichTrackExtrapolationMirrorIdeal.cxx,v 1.2 2006/06/22 09:31:09 hoehne Exp $
*
*  Class  : CbmRichTrackExtrapolationMirrorIdeal
*  Description: This is the implementation of the TrackExtrapolation from MC points - operating on points in the RICH mirror!
*               It reads the STS track array, gets the corresponding MC MirrorPoint
*               and selects those to be projected to the Rich Photodetector
*               points will be stored on mirror surface again
*
*  Author : Claudia Hoehne
*  E-mail : c.hoehne@gsi.de
*
*******************************************************************************
*  $Log: CbmRichTrackExtrapolationMirrorIdeal.cxx,v $
*  Revision 1.2  2006/06/22 09:31:09  hoehne
*  check if idSTS > 0 (non-STS tracks in global track get index -1)
*
*  Revision 1.1  2006/06/21 13:53:38  hoehne
*  for extrapolation from MirrorPoint:
*  store mirror point with STS track selection and track ID as chosen in RichExtrapolate in mirror
*
*
*
*******************************************************************************/


// CBM includes
#include "CbmRichTrackExtrapolationMirrorIdeal.h"

#include "CbmRichPoint.h"

#include "FairTrackParam.h"
#include "CbmMCTrack.h"
#include "CbmStsTrack.h"
#include "CbmStsTrackMatch.h"
#include "CbmGlobalTrack.h"
#include "FairRootManager.h"

// ROOT includes
#include "TClonesArray.h"

// C++ includes
#include <iostream>

using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
CbmRichTrackExtrapolationMirrorIdeal::CbmRichTrackExtrapolationMirrorIdeal() {
  fTrackParamArray = NULL;
  fSTSArray = NULL;
  fMCPointArray = NULL;
  fMinNsts       = 4;    // minimum number of STS stations to be crossed
  fVerbose       = 1;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmRichTrackExtrapolationMirrorIdeal::CbmRichTrackExtrapolationMirrorIdeal(Int_t MinNsts,
                                                                   Int_t verbose) {
  fTrackParamArray = NULL;
  fSTSArray = NULL;
  fMCPointArray = NULL;
  fMinNsts          = MinNsts;
  fVerbose          = verbose;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmRichTrackExtrapolationMirrorIdeal::~CbmRichTrackExtrapolationMirrorIdeal() {
}
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
void CbmRichTrackExtrapolationMirrorIdeal::Init() {

  // Get and check FairRootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if (! ioman) {
    cout << "-E- CbmRichTrackExtrapolationMirrorIdeal::Init: "
	 << "RootManager not instantised!" << endl;
    return;
  }

  // Get MCTrack array
  fMCTrackArray  = (TClonesArray*) ioman->GetObject("MCTrack");
  if ( ! fMCTrackArray) {
    cout << "-E- CbmRichTrackExtrapolationMirrorIdeal::Init: No MCTrack array!"
	 << endl;
    return;
  }

  // Get MC Point array of ImPlane
  fMCPointArray  = (TClonesArray*) ioman->GetObject("RichMirrorPoint");
  if ( ! fMCPointArray) {
    cout << "-E- CbmRichTrackExtrapolationMirrorIdeal::Init: No MCMirrorPoint array!"
	 << endl;
    return;
  }

  // Get STS track array
    fSTSArray = (TClonesArray*) ioman->GetObject("STSTrack");
    if ( ! fSTSArray) {
        cout << "-W- CbmRichTrackExtrapolationMirrorIdeal::Init: No sts track array!"
	    << endl;
     }


     // get TrackMatch array
   fTrackMatchArray = (TClonesArray*) ioman->GetObject("STSTrackMatch");
   if ( ! fTrackMatchArray) {
        cout << "-W- CbmRichTrackExtrapolationMirrorIdeal::Init: No track match array!"
	    << endl;
	return;
     }

   if (fVerbose) {
   cout << "---------------------- RICH Track Extrapolation Mirror Ideal ---------------------------------------" << endl;
   cout <<"-W- RichMirror Point selected for extrapolation --> ProjectionProducer needs to be run with zflag==2!!!" << endl;
   cout << "----------------------------------------------------------------------------------------------------" << endl;
     }

}
// -------------------------------------------------------------------------



// -----   Public method DoExtrapolate   ------------------------------------------
Int_t CbmRichTrackExtrapolationMirrorIdeal::DoExtrapolate(TClonesArray *gTrackArray, Double_t fZ,
                                                    TClonesArray *fTrackParamArray)
   {

  // Check pointers

  if ( !fMCPointArray ) {
    cout << "-E- CbmRichTrackExtrapolationMirrorIdeal::DoExtrapolate: "
	 << "MCMirrorPoint array missing! " << endl;
    return -1;
  }

  if ( !fTrackParamArray ) {
    cout << "-E- CbmRichTrackExtrapolationMirrorIdeal::DoExtrapolate: "
	 << "TrackParam Array missing! " << endl;
    return -1;
  }

  if ( !gTrackArray ) {
    cout << "-E- CbmRichTrackExtrapolationMirrorIdeal::DoExtrapolate: "
	 << "Global Track Array missing! " << endl;
    return -1;
  }

   CbmStsTrack*      pSTStr = NULL;
   CbmStsTrackMatch* pTrackMatch = NULL;
   CbmRichPoint*     pMirror = NULL;

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
     new((*fTrackParamArray)[iTrack]) FairTrackParam(0.,0.,0.,0.,0.,0.,covMat);
     Int_t idSTS = gTrack->GetStsTrackIndex();
     if (fVerbose > 1) cout << "-I- Ideal-Trackextrapolation, iTrack(STS) " << idSTS
                             << " pointer " << (CbmStsTrack*)fSTSArray->At(idSTS) << endl;
     if (idSTS < 0 ) continue;     // global tracks having no STS part get STSIndex -1
     pSTStr = (CbmStsTrack*)fSTSArray->At(idSTS);
     if ( ! pSTStr ) continue;
     Nsts = pSTStr->GetNStsHits() + pSTStr->GetNMvdHits();
     if (fVerbose > 1) cout << "-I- Ideal-Trackextrapolation, iTrack(global), iTrack(STS), Nsts: " << iTrack << " " << idSTS << " " << Nsts << endl;
     if ( Nsts >= fMinNsts) {
       pTrackMatch = (CbmStsTrackMatch*)fTrackMatchArray->At(idSTS);
       if (!pTrackMatch) cout << "no matched track!: fake?"<< endl;
       if (!pTrackMatch) continue;
       Int_t iMCmatch = pTrackMatch->GetMCTrackId();
       for (Int_t ii=0; ii < fMCPointArray->GetEntriesFast(); ii++){
         pMirror = (CbmRichPoint*)fMCPointArray->At(ii);
	 if (pMirror->GetTrackID() == iMCmatch){
           pMirror->Momentum(mom);
           pMirror->Position(pos);
           tx = mom.Px()/mom.Pz();
           ty = mom.Py()/mom.Pz();
           qp = charge/mom.Mag();
	   FairTrackParam richtrack(pos.X(),pos.Y(),pos.Z(),tx,ty,qp,covMat);
           * (FairTrackParam*)(fTrackParamArray->At(iTrack)) = richtrack;

           if (fVerbose > 1) {
	      cout << "-I- Ideal Track Extrapolation - extrapolate track " << iTrack  << endl;
              cout << " ....... MCid - PDGcode: " << iMCmatch << " " <<
	             ((CbmMCTrack*)fMCTrackArray->At(iMCmatch))->GetPdgCode() << " " <<
		      endl;
	     }
    
	 }
       }

     }

     }

    return nTracks;

}
// -------------------------------------------------------------------------

// -----   Public method Finish   ------------------------------------------
  void CbmRichTrackExtrapolationMirrorIdeal::Finish() {
  }


//-----------------------------------------------------------------------------------
ClassImp(CbmRichTrackExtrapolationMirrorIdeal)
