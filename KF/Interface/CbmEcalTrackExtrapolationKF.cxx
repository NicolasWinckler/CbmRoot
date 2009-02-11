/******************************************************************************
*  $Id: CbmEcalTrackExtrapolationKF.cxx,v 1.2 2006/06/22 09:36:37 hoehne Exp $
*
*  Class  : CbmEcalTrackExtrapolationKF
*  Description: This is the implementation of the TrackExtrapolation from MC points.
*               It reads the PointArray with ImPlanePoints from MC and selects those to
*               be projected to the Rich Photodetector
*
*  Author : Claudia Hoehne
*  E-mail : c.hoehne@gsi.de
*
*******************************************************************************
*  $Log: CbmEcalTrackExtrapolationKF.cxx,v $
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

#include <iostream>

#include "TClonesArray.h"

#include "CbmEcalStructure.h"
#include "CbmEcalInf.h"
#include "FairTrackParam.h"
#include "CbmMCTrack.h"
#include "CbmKFTrack.h"
#include "CbmStsTrack.h"
#include "CbmGlobalTrack.h"
#include "CbmStsKFTrackFitter.h"
#include "FairTrackParam.h"
#include "FairRootManager.h"
#include "CbmEcalTrackExtrapolationKF.h"

using namespace std;

// -----   Default constructor   -------------------------------------------
CbmEcalTrackExtrapolationKF::CbmEcalTrackExtrapolationKF()
  : CbmEcalTrackExtrapolation(1), fTrackParamArray(NULL), fSTSArray(NULL)
{
  ;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmEcalTrackExtrapolationKF::CbmEcalTrackExtrapolationKF(Int_t verbose)
  : CbmEcalTrackExtrapolation(verbose), fTrackParamArray(NULL), fSTSArray(NULL)
{
  ;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmEcalTrackExtrapolationKF::~CbmEcalTrackExtrapolationKF()
{
  ;
}
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
void CbmEcalTrackExtrapolationKF::Init()
{
  //Get and check FairRootManager
  FairRootManager* ioman=FairRootManager::Instance();
  if (!ioman)
  {
    cout << "-E- CbmEcalTrackExtrapolationKF::Init: " << "RootManager not instantised!" << endl;
    Fatal("CbmEcalTrackExtrapolationKF::Init()", "Can't instantise Rootmanager");
  }

  //Get STS track array
  fSTSArray=(TClonesArray*)ioman->GetObject("STSTrack");
  if (!fSTSArray)
    cout << "-W- CbmEcalTrackExtrapolationKF::Init: No sts track array!" << endl;


}
// -------------------------------------------------------------------------



// -----   Public method DoExtrapolate   ------------------------------------------
Int_t CbmEcalTrackExtrapolationKF::DoExtrapolate(TClonesArray *gTrackArray, TClonesArray *fTrackParamArray)
{
  if (!fTrackParamArray)
  {
    cout << "-E- CbmEcalTrackExtrapolationKF::DoExtrapolate: ";
    cout << "TrackParamArray missing! " << endl;
    return -1;
  }

  if (!gTrackArray)
  {
    cout << "-E- CbmEcalTrackExtrapolationKF::DoExtrapolate: ";
    cout << "Track Array missing! " << endl;
    return -1;
  }
  Int_t nTr=0;
  Int_t i=0;
  Int_t n=gTrackArray->GetEntriesFast();
  CbmGlobalTrack* tr;
  CbmKFTrack kfTr;
  FairTrackParam trpar;
  for(;i<n;i++)
  {
    tr=(CbmGlobalTrack*)gTrackArray->At(i);
    if (!tr) continue;
    if (tr->GetStsTrackIndex()<0||tr->GetTrdTrackIndex()<0)
      continue;
    kfTr.SetTrackParam(*(tr->GetParamLast()));
    kfTr.Extrapolate(Str()->GetEcalInf()->GetZPos());
    kfTr.GetTrackParam(trpar);
    new ((*fTrackParamArray)[nTr++]) FairTrackParam(trpar);
  }
  
  return nTr;
}
// -------------------------------------------------------------------------


// -----   Public method Finish   ------------------------------------------
void CbmEcalTrackExtrapolationKF::Finish()
{
  ;
}
//-----------------------------------------------------------------------------------

ClassImp(CbmEcalTrackExtrapolationKF)
