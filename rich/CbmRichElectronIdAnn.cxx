#include "CbmRichElectronIdAnn.h"

#include "CbmRichHit.h"
#include "CbmRichRing.h"

#include "TMultiLayerPerceptron.h"
#include "TTree.h"
#include "TMath.h"

#include <iostream>

using std::cout;
using std::endl;
//--------------------------------------------------------------------------
// -----   Default constructor   -------------------------------------------
CbmRichElectronIdAnn::CbmRichElectronIdAnn()
{

}

// -----   Standard constructor   ------------------------------------------
CbmRichElectronIdAnn::CbmRichElectronIdAnn ( Int_t verbose, const char* NNFile )
{

    fNeuralNetWeights = NNFile;
}

// -----   Destructor   ----------------------------------------------------
CbmRichElectronIdAnn::~CbmRichElectronIdAnn()
{}

// -----   Initialization   ----------------------------------------------------
void CbmRichElectronIdAnn::Init ()
{
    TTree *simu = new TTree ("MonteCarlo","MontecarloData");
    Float_t x1,x2,x3,x4,x5,x6,x7,x8, x9, x10;

    simu->Branch("x1", &x1,"x1/F");
    simu->Branch("x2", &x2,"x2/F");
    simu->Branch("x3", &x3,"x3/F");
    simu->Branch("x4", &x4,"x4/F");
    simu->Branch("x5", &x5,"x5/F");
    simu->Branch("x6", &x6,"x6/F");
    simu->Branch("x7", &x7,"x7/F");
    simu->Branch("x8", &x8,"x8/F");
    simu->Branch("x9", &x9,"x9/F");
    simu->Branch("x10", &x10,"x10/F");

    fNN = new TMultiLayerPerceptron("x1,x2,x3,x4,x5,x6,x7,x8,x9:20:x10",simu);
    cout << "-I- CbmRichElIdAnn: get NeuralNet weight parameters from: " << fNeuralNetWeights << endl;
    fNN->LoadWeights(fNeuralNetWeights);
}

// -----   Exec   ----------------------------------------------------
Double_t CbmRichElectronIdAnn::DoSelect(CbmRichRing* ring, Double_t momentum)
{
    if (ring->GetAaxis() >= 10. || ring->GetAaxis() <= 0. ||
        ring->GetBaxis() >= 10. || ring->GetBaxis() <= 0. ||
        ring->GetNofHits() <= 5. ||
        ring->GetDistance() <= 0. || ring->GetDistance() >= 999. ||
        ring->GetRadialPosition() <= 0. || ring->GetRadialPosition() >= 999. ||
        ring->GetPhi() <= -6.5 || ring->GetPhi() >= 6.5 ||
        ring->GetRadialAngle() <=-6.5 || ring->GetRadialAngle() >= 6.5 ){

        return -1.;
    }
    Double_t nnPar[9];
    nnPar[0] =  ring->GetAaxis();
    nnPar[1] =  ring->GetBaxis();
    nnPar[2] =  ring->GetPhi();
    nnPar[3] =  ring->GetRadialAngle();
    nnPar[4] =  ring->GetChi2()/ring->GetNDF();
    nnPar[5] =  ring->GetRadialPosition();
    nnPar[6] =  ring->GetNofHits();
    nnPar[7] =  ring->GetDistance();
    nnPar[8] =  momentum;


    Double_t nnEval = fNN->Evaluate(0,nnPar);

    if (TMath::IsNaN(nnEval) == 1) {
        cout << " -W- RingSelectNN: nnEval nan " << endl;
        nnEval = -1.;
    }

    return nnEval;
}

// -------------------------------------------------------------------------
ClassImp(CbmRichElectronIdAnn)
