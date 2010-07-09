// -------------------------------------------------------------------------
// -----                    CbmTrdSetTracksPidANN source file          -----
// -----                  Created 06/03/07 by Simeon Lebedev           -----
// -------------------------------------------------------------------------
#include "CbmTrdSetTracksPidANN.h"

#include "CbmTrdTrack.h"
#include "CbmTrdHit.h"

#include "FairRootManager.h"

#include "TClonesArray.h"
#include "TMultiLayerPerceptron.h"
#include "TTree.h"
//#include "TFile.h"
#include "TMath.h"
#include "TSystem.h"
//#include "TROOT.h"
#include "TString.h"

#include <iostream>
#include <vector>
#include <fstream>
using std::cout;
using std::endl;
//using TMath::LandauI;
//using TMath::IsNaN;
// -----   Default constructor   -------------------------------------------
CbmTrdSetTracksPidANN::CbmTrdSetTracksPidANN() {

	fTrackArray = NULL;
	fTrdHitArray = NULL;
	fNofTracks = 0;
	fTRDGeometryType = "st";
}
// -------------------------------------------------------------------------


// -----   Standard constructor   ------------------------------------------
CbmTrdSetTracksPidANN::CbmTrdSetTracksPidANN(const char* name, const char* title) :
	FairTask(name) {
	fTrackArray = NULL;
	fTrdHitArray = NULL;
	fNofTracks = 0;
	fTRDGeometryType = "st";
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
CbmTrdSetTracksPidANN::~CbmTrdSetTracksPidANN() {
}
// -------------------------------------------------------------------------


// -----  SetParContainers -------------------------------------------------
void CbmTrdSetTracksPidANN::SetParContainers() {

}
// -------------------------------------------------------------------------


// -----  RaedData -------------------------------------------------
Bool_t CbmTrdSetTracksPidANN::ReadData() {

	//
	// Read the weights for the neural network.
	//

	// Check if the input filename is valid
	TString fileName;
	std::vector<TString> weightsFilesANN;
	fileName = gSystem->Getenv("VMCWORKDIR");

	if (fTRDGeometryType != "mb" && fTRDGeometryType != "st"){
		fTRDGeometryType = "st";
		cout << "-E- CbmTrdSetTracksPidANN::Init: "
			<< "wrong geometry type." << endl;
	}
	if (fTRDGeometryType == "st"){
		weightsFilesANN.push_back(fileName + "/parameters/trd/Neural_Net_Weights_El_ID_12.txt");
		weightsFilesANN.push_back(fileName + "/parameters/trd/Neural_Net_Weights_El_ID_11.txt");
		weightsFilesANN.push_back(fileName + "/parameters/trd/Neural_Net_Weights_El_ID_10.txt");
		weightsFilesANN.push_back(fileName + "/parameters/trd/Neural_Net_Weights_El_ID_9.txt");
		weightsFilesANN.push_back(fileName + "/parameters/trd/Neural_Net_Weights_El_ID_8.txt");
		weightsFilesANN.push_back(fileName + "/parameters/trd/Neural_Net_Weights_El_ID_7.txt");
		weightsFilesANN.push_back(fileName + "/parameters/trd/Neural_Net_Weights_El_ID_6.txt");
		fANNPar1 = 1.21;
		fANNPar2 = 0.67;
	}else if (fTRDGeometryType == "mb"){
		weightsFilesANN.push_back(fileName + "/parameters/trd/Neural_Net_Weights_El_ID_MB_12.txt");
		weightsFilesANN.push_back(fileName + "/parameters/trd/Neural_Net_Weights_El_ID_MB_11.txt");
		weightsFilesANN.push_back(fileName + "/parameters/trd/Neural_Net_Weights_El_ID_MB_10.txt");
		weightsFilesANN.push_back(fileName + "/parameters/trd/Neural_Net_Weights_El_ID_MB_9.txt");
		weightsFilesANN.push_back(fileName + "/parameters/trd/Neural_Net_Weights_El_ID_MB_8.txt");
		weightsFilesANN.push_back(fileName + "/parameters/trd/Neural_Net_Weights_El_ID_MB_7.txt");
		weightsFilesANN.push_back(fileName + "/parameters/trd/Neural_Net_Weights_El_ID_MB_6.txt");
		fANNPar1 = 3.88;
		fANNPar2 = 1.28;
	}


	for (Int_t i = 0; i < weightsFilesANN.size(); i++) {
		const char* file = (const char*)weightsFilesANN[i];
		ifstream myfile(file);
		if (!myfile.is_open()) {
			cout << "-E- CbmTrdSetTracksPidANN::Init: "
					<< "Could not open input file." << endl;
			return kFALSE;
		}
		myfile.close();
	}

	cout << "-I- CbmTrdSetTracksPidANN::Init : get NeuralNet weight parameters from: ";
	for (Int_t i = 0; i < weightsFilesANN.size(); i++) {
		cout << weightsFilesANN[i] << " ," << std::endl;
	}

	Float_t inVector[12];

	Int_t x13;//output value

	//init TTree as input data to neural net
	TTree *simu = new TTree ("MonteCarlo","MontecarloData");
	simu->Branch("x1", &inVector[0], "x1/F");
	simu->Branch("x2", &inVector[1], "x2/F");
	simu->Branch("x3", &inVector[2], "x3/F");
	simu->Branch("x4", &inVector[3], "x4/F");
	simu->Branch("x5", &inVector[4], "x5/F");
	simu->Branch("x6", &inVector[5], "x6/F");
	simu->Branch("x7", &inVector[6], "x7/F");
	simu->Branch("x8", &inVector[7], "x8/F");
	simu->Branch("x9", &inVector[8], "x9/F");
	simu->Branch("x10", &inVector[9], "x10/F");
	simu->Branch("x11", &inVector[10], "x11/F");
	simu->Branch("x12", &inVector[11], "x12/F");
	simu->Branch("x13", &x13, "x13/I");

	fNN.clear();

	TMultiLayerPerceptron* ann12 = new TMultiLayerPerceptron("x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12:12:x13",simu);
	ann12->LoadWeights(weightsFilesANN[0]);
	fNN.push_back(ann12);

	TMultiLayerPerceptron* ann11 = new TMultiLayerPerceptron("x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11:12:x13",simu);
	ann11->LoadWeights(weightsFilesANN[1]);
	fNN.push_back(ann11);

	TMultiLayerPerceptron* ann10 = new TMultiLayerPerceptron("x1,x2,x3,x4,x5,x6,x7,x8,x9,x10:12:x13",simu);
	ann10->LoadWeights(weightsFilesANN[2]);
	fNN.push_back(ann10);

	TMultiLayerPerceptron* ann9 = new TMultiLayerPerceptron("x1,x2,x3,x4,x5,x6,x7,x8,x9:12:x13",simu);
	ann9->LoadWeights(weightsFilesANN[3]);
	fNN.push_back(ann9);

	TMultiLayerPerceptron* ann8 = new TMultiLayerPerceptron("x1,x2,x3,x4,x5,x6,x7,x8:12:x13",simu);
	ann8->LoadWeights(weightsFilesANN[4]);
	fNN.push_back(ann8);

	TMultiLayerPerceptron* ann7 = new TMultiLayerPerceptron("x1,x2,x3,x4,x5,x6,x7:12:x13",simu);
	ann7->LoadWeights(weightsFilesANN[5]);
	fNN.push_back(ann7);

	TMultiLayerPerceptron* ann6 = new TMultiLayerPerceptron("x1,x2,x3,x4,x5,x6:12:x13",simu);
	ann6->LoadWeights(weightsFilesANN[6]);
	fNN.push_back(ann6);


	return kTRUE;
}

// -----   Public method Init (abstract in base class)  --------------------
InitStatus CbmTrdSetTracksPidANN::Init() {

	// Read the data from ROOT file. In case of problems return kFATAL;
	if (!ReadData())
		return kFATAL;

	// Get and check FairRootManager
	FairRootManager* ioman = FairRootManager::Instance();
	if (!ioman) {
		cout << "-E- CbmTrdSetTracksPidANN::Init: "
				<< "RootManager not instantised!" << endl;
		return kFATAL;
	}

	// Get TrdTrack array
	fTrackArray = (TClonesArray*) ioman->GetObject("TrdTrack");
	if ( !fTrackArray) {
		cout << "-E- CbmTrdSetTracksPidANN::Init: No TrdTrack array!" << endl;
		return kERROR;
	}

	// Get TrdTrack array
	fTrdHitArray = (TClonesArray*) ioman->GetObject("TrdHit");
	if ( !fTrdHitArray) {
		cout << "-E- CbmTrdSetTracksPidANN::Init: No TrdHit array!" << endl;
		return kERROR;
	}

	return kSUCCESS;

}
// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void CbmTrdSetTracksPidANN::Exec(Option_t* opt) {

	if ( !fTrackArray)
		return;

	Int_t nTracks = fTrackArray->GetEntriesFast();
	std::vector<Double_t> eLossVector;

	for (Int_t iTrack=0; iTrack<nTracks; iTrack++) {

		eLossVector.clear();

		CbmTrdTrack* pTrack = (CbmTrdTrack*)fTrackArray->At(iTrack);

		if (pTrack->GetNofHits() < 6 || pTrack->GetNofHits() > 12) {
			fNofTracks++;
			pTrack->SetPidANN(-2);
			continue;
		}

		for (Int_t iTRD=0; iTRD < pTrack->GetNofHits(); iTRD++) {
			Int_t index = pTrack->GetHitIndex(iTRD);
			CbmTrdHit* trdHit = (CbmTrdHit*) fTrdHitArray->At(index);
			eLossVector.push_back(trdHit->GetELoss());
		}

		//------------------transform Data BEGIN--------------------------

		for (UInt_t j = 0; j<eLossVector.size(); j++) {
			eLossVector[j]=eLossVector[j]*1.e6;
			eLossVector[j]=(eLossVector[j]-fANNPar1)/fANNPar2-0.225;
		}

		sort(eLossVector.begin(), eLossVector.end());

		for (UInt_t j = 0; j<eLossVector.size(); j++) {
		  eLossVector[j]=TMath::LandauI(eLossVector[j]);
		}

		//------------------transform Data END----------------------------
		Int_t iANN = 0;
		if(eLossVector.size() == 12) iANN = 0;
		if(eLossVector.size() == 11) iANN = 1;
		if(eLossVector.size() == 10) iANN = 2;
		if(eLossVector.size() == 9) iANN = 3;
		if(eLossVector.size() == 8) iANN = 4;
		if(eLossVector.size() == 7) iANN = 5;
		if(eLossVector.size() == 6) iANN = 6;

		Double_t nnEval = fNN[iANN]->Evaluate(0, &eLossVector[0]);

		if (TMath::IsNaN(nnEval) == 1) {
			cout << " -W- CbmTrdSetTracksPidANN: nnEval nan " << endl;
			nnEval = -2;
		}
		//cout << "iANN = " << iANN << " nnEval = " << nnEval << endl;
		pTrack->SetPidANN(nnEval);

	}
}
// -------------------------------------------------------------------------


// -----   Public method Finish   ------------------------------------------
void CbmTrdSetTracksPidANN::Finish() {
}

ClassImp(CbmTrdSetTracksPidANN)
