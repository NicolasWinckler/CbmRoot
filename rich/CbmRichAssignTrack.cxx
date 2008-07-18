/******************************************************************************
*  $Id: CbmRichAssignTrack.cxx,v 1.2 2006/01/30 11:26:52 hoehne Exp $
*
*  Class  : CbmRichAssignTrack
*  Description: This is the implementation of the AssignTrack class. This
*               takes a particular Track Assignement Routine
*               to assign found rings and tracks
*               Input:  extrapolated tracks, found rings
*               Output: ring ID in CbmGlobalTrack
*
*
*  Author : Claudia Hoehne
*  E-mail : c.hoehne@gsi.de
*
*******************************************************************************
*  $Log: CbmRichAssignTrack.cxx,v $
*  Revision 1.2  2006/01/30 11:26:52  hoehne
*  check added whether RingIndex in GlobalTrack is already filled
*  (done for track based ring finders, no additional assignement necessary)
*
*  Revision 1.1  2006/01/26 09:54:27  hoehne
*  initial version: assignement of Rich rings and extrapolated tracks (base class, concrete implementation, Task)
*
*
*
*******************************************************************************/
#include "CbmRichAssignTrack.h"

#include "CbmRichPoint.h"
#include "CbmRichRingTrackAssign.h"

#include "CbmGlobalTrack.h"
#include "CbmRootManager.h"

#include "TClonesArray.h"

#include <iostream>

using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
CbmRichAssignTrack::CbmRichAssignTrack()
  : CbmTask("RICH Assign Tracks") {
  fAssign           = NULL;
  fTrackArray       = NULL;
  fRingArray        = NULL;
  gTrackArray       = NULL;
  fVerbose          = 1;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmRichAssignTrack::CbmRichAssignTrack(CbmRichRingTrackAssign* assign,
                                                   Int_t verbose)
  : CbmTask("RICH Assign Tracks") {
  fAssign               = assign;
  fTrackArray           = NULL;
  fRingArray            = NULL;
  gTrackArray       = NULL;
  fVerbose              = verbose;
}
// -------------------------------------------------------------------------



// -----   Constructor with name and title   -------------------------------
CbmRichAssignTrack::CbmRichAssignTrack(const char* name, const char* title,
                                       CbmRichRingTrackAssign* assign, Int_t verbose)
  : CbmTask(name) {
  fAssign               = assign;
  fTrackArray           = NULL;
  fRingArray            = NULL;
  gTrackArray           = NULL;
  fVerbose              = verbose;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmRichAssignTrack::~CbmRichAssignTrack() {
  fTrackArray->Delete();
  fRingArray->Delete();
}
// -------------------------------------------------------------------------



// -----   Public method Init (abstract in base class)  --------------------
InitStatus CbmRichAssignTrack::Init() {

  // Check for Track Assign
  if (! fAssign) {
    cout << "-E- CbmRichAssignTrack::Init: No track assignement selected!" << endl;
    return kERROR;
  }

  // Get and check CbmRootManager
  CbmRootManager* ioman = CbmRootManager::Instance();
  if (! ioman) {
    cout << "-E- CbmRichAssignTrack::Init: "
	 << "RootManager not instantised!" << endl;
    return kFATAL;
  }

  // Get track Array
    fTrackArray = (TClonesArray*) ioman->GetObject("RichProjection");
    if ( ! fTrackArray) {
        cout << "-W- CbmRichAssignTrack::Init: No extrapolated Track array!"
	    << endl;
     return kFATAL;
     }

     // Get ring Array
    fRingArray = (TClonesArray*) ioman->GetObject("RichRing");
    if ( ! fRingArray) {
        cout << "-W- CbmRichAssignTrack::Init: No found ring array!"
	    << endl;
	return kFATAL;
     }

     // Get track Array
    gTrackArray = (TClonesArray*) ioman->GetObject("GlobalTrack");
    if ( ! gTrackArray) {
        cout << "-W- CbmRichAssignTrack::Init: No global Track array!"
	    << endl;
	return kFATAL;
     }


  // Set verbosity of track assignement routine
  fAssign->SetVerbose(fVerbose);

  // Call the Init method of track assignement
  fAssign->Init();

  return kSUCCESS;

}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void CbmRichAssignTrack::Exec(Option_t* opt) {

   Int_t nTracks = gTrackArray->GetEntriesFast();
   for (Int_t iTrack=0; iTrack < nTracks; iTrack++){
     CbmGlobalTrack* gTrack = (CbmGlobalTrack*)gTrackArray->At(iTrack);
     Int_t iRing = gTrack->GetRichRingIndex();
     if (iRing != -1) {
       cout << "-I- CbmRichAssignTrack: RingIndex already filled --->> leave AssignTask without new assignement " << endl;
       return;
     }
   }

   if (fVerbose) cout << "-I- CbmRichAssignTrack: call concrete assign class" << endl;
   fAssign->DoAssign(fRingArray,fTrackArray);

}
// -------------------------------------------------------------------------



// -----   Public method Finish   ------------------------------------------
void CbmRichAssignTrack::Finish() {
}
// -------------------------------------------------------------------------




ClassImp(CbmRichAssignTrack)
