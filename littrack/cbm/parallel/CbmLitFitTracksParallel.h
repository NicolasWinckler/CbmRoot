/**
 * \file CbmLitFitTracksParallel.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2013
 * \brief Task to fit TRD and MUCH tracks using parallel track fit algorithms.
 */

#ifndef CBMLITFITTRACKSPARALLEL_H_
#define CBMLITFITTRACKSPARALLEL_H_

#include "FairTask.h"

class CbmLitFitTracksParallel : public FairTask {
public:
   /**
    * \brief Constructor.
    */
   CbmLitFitTracksParallel();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitFitTracksParallel();

   /**
     * \brief Derived from FairTask.
     */
    virtual InitStatus Init();

    /**
     * \brief Derived from FairTask.
     */
    virtual void Exec(
       Option_t* opt);

    /**
     * \brief Derived from FairTask.
     */
    virtual void Finish();

private:
    /**
     * \brief Read data branches.
     */
    void ReadDataBranches();

    // Data branches
    TClonesArray* fGlobalTracks;
    TClonesArray* fStsTracks;
    TClonesArray* fTrdTracks;
    TClonesArray* fMuchTracks;
    TClonesArray* fTrdHits;
    TClonesArray* fMuchPixelHits;
    TClonesArray* fMuchStrawHits;

    ClassDef(CbmLitFitTracksParallel, 1);
};

#endif /* CBMLITFITTRACKSPARALLEL_H_ */
