/**
* \file CbmRichElectronIdAnn.cxx
*
* \author Semen Lebedev
* \date 2008
**/

#include "CbmRichElectronIdAnn.h"
#include "CbmRichRing.h"
#include "TMultiLayerPerceptron.h"
#include "TTree.h"
#include "TMath.h"

#include <iostream>

using std::cout;
using std::endl;

CbmRichElectronIdAnn::CbmRichElectronIdAnn(
      const string& annFile):
   fNeuralNetWeights(annFile),
   fNN(NULL)
{

}

CbmRichElectronIdAnn::~CbmRichElectronIdAnn()
{

}

void CbmRichElectronIdAnn::Init()
{
   TTree *simu = new TTree ("MonteCarlo","MontecarloData");
   Double_t x[9];
   Double_t xOut;

   simu->Branch("x0", &x[0],"x0/D");
   simu->Branch("x1", &x[1],"x1/D");
   simu->Branch("x2", &x[2],"x2/D");
   simu->Branch("x3", &x[3],"x3/D");
   simu->Branch("x4", &x[4],"x4/D");
   simu->Branch("x5", &x[5],"x5/D");
   simu->Branch("x6", &x[6],"x6/D");
   simu->Branch("x7", &x[7],"x7/D");
   simu->Branch("x8", &x[8],"x8/D");
   simu->Branch("xOut", &xOut,"xOut/D");

   fNN = new TMultiLayerPerceptron("x0,x1,x2,x3,x4,x5,x6,x7,x8:9:xOut",simu);
   cout << "-I- CbmRichElIdAnn: get NeuralNet weight parameters from: " << fNeuralNetWeights << endl;
   fNN->LoadWeights(fNeuralNetWeights.c_str());
}

double CbmRichElectronIdAnn::DoSelect(
      CbmRichRing* ring,
      double momentum)
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
    double nnPar[9];
    nnPar[0] = ring->GetAaxis();
    nnPar[1] = ring->GetBaxis();
    nnPar[2] = ring->GetPhi();
    nnPar[3] = ring->GetRadialAngle();
    nnPar[4] = ring->GetChi2()/ring->GetNDF();
    nnPar[5] = ring->GetRadialPosition();
    nnPar[6] = ring->GetNofHits();
    nnPar[7] = ring->GetDistance();
    nnPar[8] =  momentum;

    double nnEval = fNN->Evaluate(0,nnPar);

    if (TMath::IsNaN(nnEval) == 1) {
        cout << " -W- CbmRichElectronIdAnn: nnEval nan " << endl;
        nnEval = -1.;
    }

    return nnEval;
}
