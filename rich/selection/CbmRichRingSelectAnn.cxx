#include "CbmRichRingSelectAnn.h"
#include "CbmRichRingLight.h"
#include "CbmRichRingSelectImpl.h"

#ifdef ANN_FILE
#include "TTree.h"
#include "TMultiLayerPerceptron.h"
#endif

#ifdef NN_FUNCTION
#include "NNfunction.h"
#endif

#include <iostream>

using std::cout;
using std::endl;

CbmRichRingSelectAnn::CbmRichRingSelectAnn (
      const std::string& annFile ):
   fNeuralNetWeights(annFile),
   fNN(NULL),
   fNNfunction(NULL),
   fSelectImpl(NULL)
{

}

CbmRichRingSelectAnn::~CbmRichRingSelectAnn()
{

}

void CbmRichRingSelectAnn::Init ()
{
    fSelectImpl = new CbmRichRingSelectImpl();

#ifdef ANN_FILE
    TTree *simu = new TTree ("MonteCarlo","MontecarloData");
    Float_t x1,x2,x3,x4,x5,x6,x7,x8, x9, x10, x11;

    simu->Branch("x1", &x1,"x1/F");
    simu->Branch("x2", &x2,"x2/F");
    simu->Branch("x3", &x3,"x3/F");
    simu->Branch("x4", &x4,"x4/F");
    simu->Branch("x5", &x5,"x5/F");
    simu->Branch("x6", &x6,"x6/F");
    simu->Branch("x11", &x11,"x11/F");

    fNN = new TMultiLayerPerceptron("x1,x2,x3,x4,x5,x6:5:x11",simu);
    cout << "-I- CbmRichRingSelectNeuralNet: get NeuralNet weight parameters from: " << fNeuralNetWeights << endl;
    fNN->LoadWeights(fNeuralNetWeights);
#endif

#ifdef NN_FUNCTION
    fNNfunction = new NNfunction();
#endif
}

// -----   Exec   ----------------------------------------------------
void CbmRichRingSelectAnn::DoSelect(CbmRichRingLight* ring)
{
    if (ring->GetRadius() >= 10.f || ring->GetRadius() <= 0.f ||
        ring->GetNofHits() <= 5.f ||
        ring->GetRadialPosition() <= 0.f || ring->GetRadialPosition() >= 999.f ){

        ring->SetSelectionNN(-1.f);
        return;
    }
    ring->SetNofHitsOnRing(fSelectImpl->GetNofHitsOnRingCircle(ring));
    if (ring->GetNofHitsOnRing() < 5){
    	ring->SetSelectionNN(-1.f);
    	return;
    }

    ring->SetAngle(fSelectImpl->GetAngle(ring));
    if (ring->GetAngle() < 0.f || ring->GetAngle() > 6.3f){
    	ring->SetSelectionNN(-1.f);
    	return;
    }

    double nnPar[10];
    nnPar[0] = ring->GetNofHits();
    nnPar[1] = ring->GetAngle();
    nnPar[2] = ring->GetNofHitsOnRing();
    nnPar[3] = ring->GetRadialPosition();
    nnPar[4] = ring->GetRadius();
    nnPar[5] = ring->GetChi2() / ring->GetNofHits();
#ifdef ANN_FILE
    float nnEval = fNN->Evaluate(0,nnPar);
#endif

#ifdef NN_FUNCTION
    float nnEval = fNNfunction->Value(0,nnPar);
#endif

    ring->SetSelectionNN(nnEval);
}
