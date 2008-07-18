// ------------------------------------------------------------------
// -----                    CbmFitGlobalTracks                  -----
// -----             Created 06/03/2006 by D.Kresan             -----
// ------------------------------------------------------------------
#ifndef CBMFITGLOBALTRACKS
#define CBMFITGLOBALTRACKS

#include "CbmTask.h"

class TClonesArray;
class CbmGlobalTrackFitter;


class CbmFitGlobalTracks : public CbmTask {

private:
    Int_t                 fVerbose;        // Verbosity level
    CbmGlobalTrackFitter *fFitter;         // Fitter algorithm
    TClonesArray         *fArrayGlbTrack;  // Array of global tracks

public:
    CbmFitGlobalTracks();
    CbmFitGlobalTracks(const char* name, Int_t verbose = 1,
		       CbmGlobalTrackFitter* globalTrackFitter = NULL);
    virtual ~CbmFitGlobalTracks();

    virtual InitStatus Init();
    virtual void Exec(Option_t* option = "");
    virtual void Finish();

    inline void SetVerbose(Int_t verbose)                {  fVerbose = verbose; }
    inline void UseFitter (CbmGlobalTrackFitter *fitter) {  fFitter = fitter;   }

    ClassDef(CbmFitGlobalTracks, 1);
};


#endif
