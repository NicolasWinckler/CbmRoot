 /******************************************************************************
*  $Id: CbmRichRingSelectNeuralNet.cxx,v 1.1 2006/09/13 14:53:31 hoehne Exp $
*
*  Class  : CbmRichRingSelect2DCuts
*  Description : Implementation for concrete RICH ring selection algorithm:
*                reject rings using a trained neural net (input file with weights needed!)
*                store resulting value (from -1 to 1) in "SelectionNN":
*                1 = good rings
*                -1 = rings to be rejected
*                --> choose a value in between depending on required purity/ efficiency
*
*  Author : Simeon Lebedev
*  E-mail : salebedev@jinr.ru
*
*******************************************************************************
*  $Log: CbmRichRingSelectNeuralNet.cxx,v $
*  Revision 1.1  2006/09/13 14:53:31  hoehne
*  initial version
*
*
*
*******************************************************************************/


#include "CbmRichRingSelectNeuralNet.h"

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
CbmRichRingSelectNeuralNet::CbmRichRingSelectNeuralNet()
{
    fVerbose = 1;
}

// -----   Standard constructor   ------------------------------------------
CbmRichRingSelectNeuralNet::CbmRichRingSelectNeuralNet ( Int_t verbose, const char* NNFile )
{
    fVerbose = verbose;
    fNeuralNetWeights = NNFile;
}

// -----   Destructor   ----------------------------------------------------
CbmRichRingSelectNeuralNet::~CbmRichRingSelectNeuralNet()
{}

// -----   Initialization   ----------------------------------------------------
void CbmRichRingSelectNeuralNet::Init ()
{
    fSelectImpl = new CbmRichRingSelectImpl();
    fSelectImpl->Init();

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

    //fNNfunction = new NNfunction();
}

// -----   Exec   ----------------------------------------------------
void CbmRichRingSelectNeuralNet::DoSelect(CbmRichRing* ring)
{
    if (fVerbose > 1) ring->Print();

    if (ring->GetRadius() >= 10. || ring->GetRadius() <= 0. ||
        ring->GetNofHits() <= 5. ||
        ring->GetRadialPosition() <= 0. || ring->GetRadialPosition() >= 999. ){

        ring->SetSelectionNN(-1.);
        return;
    }
    ring->SetNofHitsOnRing(fSelectImpl->GetNofHitsOnRingCircle(ring));
    if (ring->GetNofHitsOnRing() < 5){
    	ring->SetSelectionNN(-1.);
    	return;
    }

    ring->SetAngle(fSelectImpl->GetAngle(ring));

    Double_t nnPar[10];
    nnPar[0] =  ring->GetNofHits();
    nnPar[1] =  ring->GetAngle();
    nnPar[2] =  ring->GetNofHitsOnRing();
    nnPar[3] =  ring->GetRadialPosition();
    nnPar[4] =  ring->GetRadius();
    nnPar[5] =  ring->GetChi2() / ring->GetNDF();

    Double_t nnEval = fNN->Evaluate(0,nnPar);
    //Double_t nnEval = fNNfunction->Value(0,nnPar);
    if (fVerbose > 1) cout <<"nnEval = "<<nnEval<<endl;

    if (TMath::IsNaN(nnEval) == 1) {
        cout << " -W- RingSelectNN: nnEval nan " << endl;
        nnEval = -1.;
    }

    ring->SetSelectionNN(nnEval);
}

// -------------------------------------------------------------------------
ClassImp(CbmRichRingSelectNeuralNet)
