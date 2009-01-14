
#ifndef CBM_RICH_ELECTRONID_ANN_H
#define CBM_RICH_ELECTRONID_ANN_H

#include "TClonesArray.h"
#include "CbmRichRing.h"
#include "CbmRichRingSelect.h"
#include "TMultiLayerPerceptron.h"

class TMultiLayerPerceptron;

class CbmRichElectronIdAnn{
    const char* fNeuralNetWeights;

public:
    CbmRichElectronIdAnn();// Default constructor
    CbmRichElectronIdAnn ( Int_t verbose, const char* NNFile);// Standard constructor
    ~CbmRichElectronIdAnn();

    void Init();
    Double_t DoSelect(CbmRichRing* ring, Double_t momentum);

    TMultiLayerPerceptron* fNN;

    ClassDef(CbmRichElectronIdAnn,1);
};

#endif

