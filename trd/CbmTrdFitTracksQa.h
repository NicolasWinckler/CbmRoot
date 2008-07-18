//-------------------------------------------------------------------------
//-----                       CbmTrdFitTracksQa                       -----
//-----                 Created 29/11/05 by D. Kresan                 -----
//-------------------------------------------------------------------------
#ifndef CBMTRDFITTRACKSQA
#define CBMTRDFITTRACKSQA

#include "CbmTask.h"

class TClonesArray;
class TH1F;


class CbmTrdFitTracksQa : public CbmTask {

private:
    TClonesArray *fArrayTrdPoint;    // TRD points
    TClonesArray *fArrayTrdHit;      // TRD hits
    TClonesArray *fArrayTrdTrack;    // TRD tracks
    Int_t         fNoEvents;         // Number of events processed
    TH1F         *fh_first_resx;     // X res. at first plane
    TH1F         *fh_first_resy;     // Y res. at first plane
    TH1F         *fh_first_restx;    // Tx res. at first plane
    TH1F         *fh_first_resty;    // Ty res. at first plane
    TH1F         *fh_first_resz;     // Z res. at first plane
    TH1F         *fh_last_resx;      // X res. at last plane
    TH1F         *fh_last_resy;      // Y res. at last plane
    TH1F         *fh_last_restx;     // Tx res. at last plane
    TH1F         *fh_last_resty;     // Ty res. at last plane
    TH1F         *fh_last_resz;      // Z res. at last plane
    TH1F         *fh_first_pullx;    // X pull at first plane
    TH1F         *fh_first_pully;    // Y pull at first plane
    TH1F         *fh_first_pulltx;   // Tx pull at first plane
    TH1F         *fh_first_pullty;   // Ty pull at first plane
    TH1F         *fh_last_pullx;     // X pull at last plane
    TH1F         *fh_last_pully;     // Y pull at last plane
    TH1F         *fh_last_pulltx;    // Tx pull at last plane
    TH1F         *fh_last_pullty;    // Ty pull at last plane
    TH1F         *fh_chi2ndf;        // Chi2/NDF of track fit

    void CreateHistogramms();

public:
    CbmTrdFitTracksQa();
    CbmTrdFitTracksQa(const char* name, Int_t verbose = 1);
    virtual ~CbmTrdFitTracksQa();

    virtual InitStatus Init();
    virtual void Exec(Option_t* option);
    virtual void Finish();

    void WriteHistogramms();

    ClassDef(CbmTrdFitTracksQa, 1);
};


#endif


