#ifndef CBMLITMUCHTRACKFINDERBRANCH_H_
#define CBMLITMUCHTRACKFINDERBRANCH_H_

#include "base/CbmLitTypes.h"
#include "base/CbmLitDetectorLayout.h"
#include "finder/CbmLitTrackFinderBranch.h"
#include "finder_cbm/CbmLitStsBasedTrackFinder.h"

#include "CbmMuchTrackFinder.h"

#include <vector>

class CbmLitTrackPropagator;
class CbmLitTrack;
class CbmLitPixelHit;
class TClonesArray;

class CbmLitMuchTrackFinderBranch : public CbmMuchTrackFinder,
   private CbmLitTrackFinderBranch,
   private CbmLitStsBasedTrackFinder
{
public:
   CbmLitMuchTrackFinderBranch();
   virtual ~CbmLitMuchTrackFinderBranch();

   virtual void Init();

   virtual Int_t DoFind(
      TClonesArray* trackArray);

   ClassDef(CbmLitMuchTrackFinderBranch, 1);
};

#endif /*CBMLITMUCHTRACKFINDERBRANCH_H_*/
