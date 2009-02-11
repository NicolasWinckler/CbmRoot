/******************************************************************************
*  $Id: CbmRichTrackExtrapolationKF.cxx,v 1.2 2006/06/22 09:36:37 hoehne Exp $
*
*  Class  : CbmRichTrackExtrapolationKF
*  Description: This is the implementation of the TrackExtrapolation from MC points.
*               It reads the PointArray with ImPlanePoints from MC and selects those to
*               be projected to the Rich Photodetector
*
*  Author : Claudia Hoehne
*  E-mail : c.hoehne@gsi.de
*
*******************************************************************************
*  $Log: CbmRichTrackExtrapolationKF.cxx,v $
*  Revision 1.2  2006/06/22 09:36:37  hoehne
*  check if idSTS > 0 (non-STS tracks in global track get index -1)
*
*  Revision 1.1  2006/02/17 23:13:37  sgorboun
*  Update of KF&L1 classes
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
// CBM includes
#include "CbmRichTrackExtrapolationKF.h"

#include "CbmStsKFTrackFitter.h"

#include "CbmRichPoint.h"
#include "FairTrackParam.h"
#include "CbmMCTrack.h"
#include "CbmStsTrack.h"
#include "CbmGlobalTrack.h"
#include "FairTrackParam.h"
#include "FairRootManager.h"

// ROOT includes
#include "TClonesArray.h"

// C++ includes
#include <iostream>

using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
CbmRichTrackExtrapolationKF::CbmRichTrackExtrapolationKF() {
  fTrackParamArray = NULL;
  fSTSArray = NULL;
  fMinNsts       = 6;    // minimum number of STS stations to be crossed
  fVerbose       = 1;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmRichTrackExtrapolationKF::CbmRichTrackExtrapolationKF(Int_t MinNsts,
                                                                   Int_t verbose) {
  fTrackParamArray = NULL;
  fSTSArray = NULL;
  fMinNsts          = MinNsts;
  fVerbose          = verbose;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmRichTrackExtrapolationKF::~CbmRichTrackExtrapolationKF() {
}
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
void CbmRichTrackExtrapolationKF::Init() {

  // Get and check FairRootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if (! ioman) {
    cout << "-E- CbmRichTrackExtrapolationKF::Init: "
	 << "RootManager not instantised!" << endl;
  }

     // Get STS track array
    fSTSArray = (TClonesArray*) ioman->GetObject("STSTrack");
    if ( ! fSTSArray) {
        cout << "-W- CbmRichTrackExtrapolationKF::Init: No sts track array!"
	    << endl;
     }


}
// -------------------------------------------------------------------------



// -----   Public method DoExtrapolate   ------------------------------------------
Int_t CbmRichTrackExtrapolationKF::DoExtrapolate(TClonesArray *gTrackArray, Double_t fZ,
                                                    TClonesArray *fTrackParamArray)
   {

  if ( !fTrackParamArray ) {
    cout << "-E- CbmRichTrackExtrapolationKF::DoExtrapolate: "
	 << "TrackParamArray missing! " << endl;
    return -1;
  }

  if ( !gTrackArray ) {
    cout << "-E- CbmRichTrackExtrapolationKF::DoExtrapolate: "
	 << "Track Array missing! " << endl;
    return -1;
  }


   CbmStsTrack * pSTStr = NULL;

   // some default variables
   TMatrixFSym covMat(5);
   for(Int_t i=0;i<5;i++) for(Int_t j=0; j<=i; j++) covMat(i,j) = 0;
   covMat(0,0) = covMat(1,1) = covMat(2,2) = covMat(3,3) = covMat(4,4) = 1.e-4; 
   

   Int_t Nsts = 0;
   TVector3 pos, mom;

   Int_t nTracks = gTrackArray->GetEntriesFast();
   for (Int_t iTrack=0; iTrack < nTracks; iTrack++){
     CbmGlobalTrack* gTrack = (CbmGlobalTrack*)gTrackArray->At(iTrack);
     new((*fTrackParamArray)[iTrack]) FairTrackParam(0.,0.,0.,0.,0.,0.,covMat);
     Int_t idSTS = gTrack->GetStsTrackIndex();
   if (fVerbose > 1) cout << "-I- KF-Trackextrapolation, iTrack(STS) " << idSTS
                          << " pointer " << (CbmStsTrack*)fSTSArray->At(idSTS) << endl;
   if (idSTS < 0 ) continue;     // global tracks having no STS part get STSIndex -1
   pSTStr = (CbmStsTrack*)fSTSArray->At(idSTS);
   if ( ! pSTStr ) continue;
    CbmStsKFTrackFitter KF;
    FairTrackParam ExTrack;

    KF.Extrapolate(pSTStr, fZ, &ExTrack);

    Nsts = pSTStr->GetNStsHits();
    if (fVerbose > 1) cout << "-I- KF-Trackextrapolation, iTrack(Global), iTrack(STS), Nsts: " << iTrack << " " << idSTS << " " << Nsts << endl;

    if ( Nsts >= fMinNsts) {
        * (FairTrackParam*)(fTrackParamArray->At(iTrack)) = ExTrack;
      if (fVerbose > 1) cout << "-I- KF Track Extrapolation - extrapolate track " << iTrack << endl;
      }

    }

    return nTracks;

}
// -------------------------------------------------------------------------


// -----   Public method Finish   ------------------------------------------
  void CbmRichTrackExtrapolationKF::Finish() {
  }

//-----------------------------------------------------------------------------------
ClassImp(CbmRichTrackExtrapolationKF)
