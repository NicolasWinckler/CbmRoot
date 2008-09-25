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
    CbmRichRingSelect::Init();
    
    TTree *simu = new TTree ("MonteCarlo","MontecarloData");
    Float_t x1,x3,x4,x5,x6,x7,x8, x9, x10, x11;

    simu->Branch("x1", &x1,"x1/F");
   // simu->Branch("x2", &x2,"x2/F");
    simu->Branch("x3", &x3,"x3/F");
    simu->Branch("x4", &x4,"x4/F");
    simu->Branch("x5", &x5,"x5/F");
    simu->Branch("x6", &x6,"x6/F");
    simu->Branch("x7", &x7,"x7/F");
    simu->Branch("x8", &x8,"x8/F");
    simu->Branch("x9", &x9,"x9/F");
    simu->Branch("x10", &x10,"x10/F");
    simu->Branch("x11", &x11,"x11/F");
        
    fNN = new TMultiLayerPerceptron("x1,x3,x4,x5,x6,x7,x8,x9,x10:20:x11",simu);
    cout << "-I- CbmRichRingSelectNeuralNet: get NeuralNet weight parameters from: " << fNeuralNetWeights << endl;
    fNN->LoadWeights(fNeuralNetWeights);
}

// -----   Exec   ----------------------------------------------------
void CbmRichRingSelectNeuralNet::DoSelect(CbmRichRing* ring)
{
    if (fVerbose > 1) ring->Print();
    
    if (ring->GetAaxis() >= 10. || ring->GetAaxis() <= 0. ||
        ring->GetBaxis() >= 10. || ring->GetBaxis() <= 0. ||
        ring->GetNofHits() <= 5. ||
        ring->GetDistance() <= 0. || ring->GetDistance() >= 999. ||
       // ring->GetAngle() <= -6.5 || ring->GetAngle() >= 6.5 ||
       // ring->GetNofHitsOnRing() <= 0. ||
        ring->GetRadialPosition() <= 0. || ring->GetRadialPosition() >= 999. ||
        ring->GetPhi() <= -6.5 || ring->GetPhi() >= 6.5 ||
        ring->GetRadialAngle() <=-6.5 || ring->GetRadialAngle() >= 6.5 ){
        
        ring->SetSelectionNN(-1.);
        return;
    }
    
    
    ring->SetAngle(GetAngle(ring));
    ring->SetNofHitsOnRing(GetNofHitsOnRing(ring));

    Double_t nnPar[10];
    nnPar[0] =  ring->GetNofHits();    
    //nnPar[1] =  ring->GetDistance();    
    nnPar[1] =  ring->GetAngle();
    nnPar[2] =  ring->GetNofHitsOnRing();
    nnPar[3] =  ring->GetRadialPosition();
    nnPar[4] =  ring->GetAaxisCor();
    nnPar[5] =  ring->GetBaxisCor();
    nnPar[6] =  ring->GetPhi();
    nnPar[7] =  ring->GetRadialAngle(); 
    nnPar[8] =  ring->GetChi2();    
    
    Double_t nnEval = fNN->Evaluate(0,nnPar);
    if (fVerbose > 1) cout <<"nnEval = "<<nnEval<<endl;

    if (TMath::IsNaN(nnEval) == 1) {
        cout << " -W- RingSelectNN: nnEval nan " << endl;
        nnEval = -1.;
    }

    ring->SetSelectionNN(nnEval);
}

// -------------------------------------------------------------------------
ClassImp(CbmRichRingSelectNeuralNet)
