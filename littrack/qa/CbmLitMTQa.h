#ifndef CBMLITMTQA_H_
#define CBMLITMTQA_H_

#include "base/CbmLitTask.h"

#include <vector>

class LitScalTrack;
class LitScalPixelHit;

class CbmLitMTQa : public CbmLitTask
{
public:
   /* Constructor */
   CbmLitMTQa();

   /* Destructor */
   virtual ~CbmLitMTQa();

   /* Inherited from FairTask */
   virtual InitStatus Init();

   /* Inherited from FairTask */
   virtual void Exec(Option_t* option);

   /* Inherited from FairTask */
   virtual void Finish();

   /* Inherited from FairTask */
   virtual void SetParContainers();

private:
   TClonesArray* fStsTracks; // CbmStsTrack array
   TClonesArray* fMuchPixelHits; // CbmMuchPixelHits array

   void DoTestWithTasks(Int_t nofTasks);

   // Event buffers
   // [first index] - event number
   // [second index] - hit/track number
   std::vector<std::vector<LitScalPixelHit*> > fHits;
   std::vector<std::vector<LitScalTrack*> > fTrackSeeds;

//    Int_t fEventNumber;
//    Double_t fExecTime;
//    static const int kMAX_NOF_THREADS = 80;
//    int fNofTasks;
//    int fNofEvents;
//
//    CbmRichRingFinderHough* fHT[kMAX_NOF_THREADS];
//    //CbmL1RichENNRingFinder* fHT[kMAX_NOF_THREADS];
//    std::vector<std::vector<CbmRichHoughHit> > fData;

   ClassDef(CbmLitMTQa, 1);
};

#endif /* CBMLITMTQA_H_ */
