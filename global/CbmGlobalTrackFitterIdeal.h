// ------------------------------------------------------------------
// -----                  CbmGlobalTrackFitterIdeal             -----
// -----              Created 12/05/2006 by D.Kresan            -----
// ------------------------------------------------------------------
#ifndef CBMGLOBALTRACKFITTERIDEAL
#define CBMGLOBALTRACKFITTERIDEAL

#include "CbmGlobalTrackFitter.h"

class TClonesArray;


class CbmGlobalTrackFitterIdeal : public CbmGlobalTrackFitter {

private:
    TClonesArray *fArrayStsTrack;  // STS tracks
    TClonesArray *fArrayTrdTrack;  // TRD tracks

public:
    CbmGlobalTrackFitterIdeal();
    virtual ~CbmGlobalTrackFitterIdeal();

    void Init();
    void DoFit(CbmGlobalTrack* glbTrack);

    ClassDef(CbmGlobalTrackFitterIdeal, 1);
};


#endif
