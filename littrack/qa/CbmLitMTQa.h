#ifndef CBMLITMTQA_H_
#define CBMLITMTQA_H_

#include "FairTask.h"
#include "CbmRichRingFinderHough.h"
#include "CbmRichRingFinderHoughParallel.h"
#include <vector>
#include "../../L1/OffLineInterface/CbmL1RichENNRingFinder.h"

class CbmLitMTQa : public FairTask {

    TClonesArray* fRichRings;        // Array of CbmRichRings
    TClonesArray* fRichHits;         // Array of CbmRichHits

    Int_t fEventNumber;
    Double_t fExecTime;
    static const int kMAX_NOF_THREADS = 80;
    int fNofTasks;
    int fNofEvents;

    CbmRichRingFinderHough* fHT[kMAX_NOF_THREADS];
    //CbmL1RichENNRingFinder* fHT[kMAX_NOF_THREADS];
    std::vector<std::vector<CbmRichHoughHit> > fData;
public:

    CbmLitMTQa();
    virtual ~CbmLitMTQa();
    virtual InitStatus Init();
    virtual void Exec(Option_t* option);
    void DoTestWithTask();

    void SetNofTasks(int n){fNofTasks = n;}
    void SetNofEvents(int n){fNofEvents = n;}

    ClassDef(CbmLitMTQa,1)
};

#endif /* CBMLITMTQA_H_ */
