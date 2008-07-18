// -------------------------------------------------------------------------
// -----                  CbmLitTrdTrackFinderMuch header file               -----
// -----                  Created 29/08/07  by A. Lebedev               -----
// -------------------------------------------------------------------------


/** CbmLitTrdTrackFinderMuch.h
 *@author A.Lebedev <alebedev@jinr.ru>
 **
 ** MUCH -> TRD track-finder
 **/  

#ifndef CBMLITTRDTRACKFINDERMUCH_H
#define CBMLITTRDTRACKFINDERMUCH_H

#include "CbmLitTrackFinderSeed.h"
#include "CbmTrdTrackFinder.h"


#include <set>

class TClonesArray;


class CbmLitTrdTrackFinderMuch : public CbmLitTrackFinderSeed,
                                 public CbmTrdTrackFinder
{

public:

    // Default constructor
    CbmLitTrdTrackFinderMuch();


    // Destructor
    ~CbmLitTrdTrackFinderMuch();

    // Initialisation
    void Init();

    // Track finding algorithm
    Int_t DoFind(TClonesArray* hitArray,
                 TClonesArray* trackArray);

private:

    // Creates Much tracks from Sts tracks
    void CreateTracksFromSeeds();

    // Copy to output TClonesArray
    void CopyToOutput(TClonesArray* trackArray);
    
    virtual void ReadDetectorGeometry();
    
    // Set iteration parameters
    void SetIterPar(Double_t SigmaCoef, Int_t beginStation, 
                    Int_t endStation, Int_t maxNofMissingHitsInStation);

public:


    ClassDef(CbmLitTrdTrackFinderMuch, 1);

};


#endif


