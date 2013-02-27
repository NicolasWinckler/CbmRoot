//-------------------------------------------------------------------
//-----                  CbmTrdHitProducerIdeal                 -----
//-----               Created 20/06/06 by D.Kresan              -----
//-------------------------------------------------------------------

#ifndef _CBM_TRD_HIT_PRODUCER_IDEAL_
#define _CBM_TRD_HIT_PRODUCER_IDEAL_

#include "FairTask.h"

#include "CbmTrdDetectorId.h"

#include <vector>

class TClonesArray;


class CbmTrdHitProducerIdeal : public FairTask {

private:
    TClonesArray *fArrayTrdPoint;  // Array of TRD points
    TClonesArray *fArrayTrdHit;    // Array of TRD hits
    Int_t fEvents;                 // Number of events processed

    CbmTrdDetectorId fDetId;      //! Get detector info from detector id

    std::vector<Int_t> fLayersBeforeStation; //! 

    CbmTrdHitProducerIdeal(const CbmTrdHitProducerIdeal&);
    CbmTrdHitProducerIdeal& operator=(const CbmTrdHitProducerIdeal&);

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

