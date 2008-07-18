/******************************************************************************
*  $Id: CbmRichExtrapolateTracks.cxx,v 1.3 2006/02/01 13:23:27 hoehne Exp $
*
*  Class  : CbmRichExtrapolateTracks
*  Description: This is the implementation of the ExtrapolateTracks class. This
*               takes a particular Track Extrapolation Routine
*               to create Points in a z-Plane in the RICH
*
*  Author : Claudia Hoehne
*  E-mail : c.hoehne@gsi.de
*
*******************************************************************************
*  $Log: CbmRichExtrapolateTracks.cxx,v $
*  Revision 1.3  2006/02/01 13:23:27  hoehne
*  do not store intermediate "RichTrackZ" point in output file
*
*  Revision 1.2  2006/01/30 10:59:11  hoehne
*  changes due to bug fix in filling RichPointArray (see Extrapolation methods)
*
*  Revision 1.1  2006/01/26 09:53:21  hoehne
*  initial version for track extrapolation (base class + concrete implementations + task) to z-plane in RICH
*
*
*
*******************************************************************************/
#include "CbmRichExtrapolateTracks.h"

#include "CbmRichTrackExtrapolation.h"

#include "CbmRootManager.h"
#include "CbmGlobalTrack.h"

#include "TClonesArray.h"

#include <iostream>

using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
CbmRichExtrapolateTracks::CbmRichExtrapolateTracks()
  : CbmTask("RICH Extrapolate Tracks") {
  fExtrapolation    = NULL;
  gTrackArray       = NULL;
  fZ                = 300.;
  fVerbose          = 1;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmRichExtrapolateTracks::CbmRichExtrapolateTracks(CbmRichTrackExtrapolation* extrapolation,
                                                   Double_t Zpos,
				                   Int_t verbose)
  : CbmTask("RICH Extrapolate Tracks") {
  fExtrapolation         = extrapolation;
  gTrackArray            = NULL;
  fZ                     = Zpos;
  fVerbose               = verbose;
}
// -------------------------------------------------------------------------



// -----   Constructor with name and title   -------------------------------
CbmRichExtrapolateTracks::CbmRichExtrapolateTracks(const char* name, const char* title,
				   CbmRichTrackExtrapolation* extrapolation,
				   Double_t Zpos, Int_t verbose)
  : CbmTask(name) {
  fExtrapolation     = extrapolation;
  gTrackArray        = NULL;
  fZ                 = Zpos;
  fVerbose           = verbose;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmRichExtrapolateTracks::~CbmRichExtrapolateTracks() {
fTrackParamArray->Delete();
}
// -------------------------------------------------------------------------



// -----   Public method Init (abstract in base class)  --------------------
InitStatus CbmRichExtrapolateTracks::Init() {

  // Check for Track Extrapolation
  if (! fExtrapolation) {
    cout << "-E- CbmRichExtrapolateTracks::Init: No track extrapolation selected!" << endl;
    return kERROR;
  }

  // Get and check CbmRootManager
  CbmRootManager* ioman = CbmRootManager::Instance();
  if (! ioman) {
    cout << "-E- CbmRichExtrapolateTracks::Init: "
	 << "RootManager not instantised!" << endl;
    return kFATAL;
  }

  // Get global track Array
    gTrackArray = (TClonesArray*) ioman->GetObject("GlobalTrack");
    if (fVerbose) cout << " Global TrackArray used: extrapolation to z-plane at z = " << fZ << " cm" << endl;
    if ( ! gTrackArray) {
        cout << "-E- CbmRichExtrapolateTracks::Init: No Global Track array!"
	    << endl;
      return kERROR;
     }



  // Create and register RichPoint array
  fTrackParamArray = new TClonesArray("CbmTrackParam",100);
  ioman->Register("RichTrackParamZ", "RICH", fTrackParamArray, kFALSE);

  // Set verbosity of track extrapolation
  fExtrapolation->SetVerbose(fVerbose);

  // Call the Init method of track extrapolation
  fExtrapolation->Init();

  return kSUCCESS;

}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void CbmRichExtrapolateTracks::Exec(Option_t* opt) {

  fTrackParamArray->Clear();

  fNofTracks = fExtrapolation->DoExtrapolate(gTrackArray, fZ, fTrackParamArray);
  Int_t nTracks = gTrackArray->GetEntriesFast();

  if (fVerbose) {
    cout << endl;
    cout << "--------------------------------------------------------------------------------------"
	 << endl;
    cout << "-I-                   Track Extrapolation                                          -I-"
	 << endl;
    cout << "Global tracks: " << nTracks << endl;
    cout << "Points written to Point Array for Projection to RICH photodetector plane: "
         << fNofTracks << endl;
    cout << endl;
    cout << "--------------------------------------------------------------------------------------"
	 << endl;
  }
  else cout << "-I- CbmRichExtrapolateTracks: " << nTracks << " tracks - accepted at z = " << fZ
            << " cm " << fNofTracks << endl;

}
// -------------------------------------------------------------------------



// -----   Public method Finish   ------------------------------------------
void CbmRichExtrapolateTracks::Finish() {
  fTrackParamArray->Clear();
}
// -------------------------------------------------------------------------




ClassImp(CbmRichExtrapolateTracks)
