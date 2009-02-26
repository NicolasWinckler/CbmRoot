
#ifndef CBMLITMUCHTRACKFINDERTRIGGER_H_
#define CBMLITMUCHTRACKFINDERTRIGGER_H_


#include "CbmMuchTrackFinder.h"

#include "CbmLitHit.h"
#include "CbmLitTrack.h"
#include <vector>



class CbmLitMuchTrackFinderTrigger: public CbmMuchTrackFinder
{

public:

	CbmLitMuchTrackFinderTrigger();
	virtual ~CbmLitMuchTrackFinderTrigger();
    
	// Initialisation
    virtual void Init();

    // Track finding algorithm
    virtual Int_t DoFind(TClonesArray* hitArray, TClonesArray* trackArray);
    
protected:

	// number of stations
    Int_t fNofStations;
    // number of layers per each station
    std::vector<Int_t> fNofLayersPerStation;
    // number of layers
    Int_t fNofLayers;
    
    // z positions of the layer
    std::vector<Double_t> fLayerZPos;
    
    // Data
    TClonesArray* fHitArray;
    
    // Verbosity level
    Int_t fVerbose;

    // Event number
    Int_t fEvents;

    // vector with found tracks
    std::vector<CbmLitTrack*> fFoundTracks;

    // vector with pointers to hits
    std::vector<std::vector<CbmLitPixelHit*> > fHits;
    
    Int_t fNofTracks;
    
    // Map from pointer to CbmHit to Hit index
   // std::map<CbmHit*, Int_t> fHitIdMap;
    
    void ArrangeHits();
    
    void CreateTracks();
    
    void AddTrack(std::vector<CbmLitPixelHit*>& trackHits);
    
    Bool_t IsIn(Double_t xpred, Double_t ypred, CbmLitPixelHit* pHit);
    
    void CopyToOutput(TClonesArray* trackArray);
    
    ClassDef(CbmLitMuchTrackFinderTrigger, 1);
};


#endif /*CBMLITMUCHTRACKFINDERTRIGGER_H_*/
