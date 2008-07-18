//-------------------------------------------------------------------
//-----                  CbmTrdHitProducerIdeal                 -----
//-----               Created 20/06/06 by D.Kresan              -----
//-------------------------------------------------------------------

#ifndef _CBM_TRD_HIT_PRODUCER_IDEAL_
#define _CBM_TRD_HIT_PRODUCER_IDEAL_

#include "CbmTask.h"

class TClonesArray;


class CbmTrdHitProducerIdeal : public CbmTask {

private:
    TClonesArray *fArrayTrdPoint;  // Array of TRD points
    TClonesArray *fArrayTrdHit;    // Array of TRD hits
    Int_t fEvents;                 // Number of events processed
    Int_t fNoTrdStations;          // Number of TRD stations
    Int_t fNoTrdPerStation;        // Number of layers per station

public:
    CbmTrdHitProducerIdeal();
    CbmTrdHitProducerIdeal(const char *name,
			   Int_t verbose = 1);
    virtual ~CbmTrdHitProducerIdeal();
    virtual InitStatus Init();
    virtual void Exec(Option_t *option = "");
    virtual void Finish() {}

    ClassDef(CbmTrdHitProducerIdeal, 1);
};


#endif

