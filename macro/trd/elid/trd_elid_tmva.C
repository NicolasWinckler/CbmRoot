#include <iostream>

#include "TCut.h"
#include "TFile.h"
#include "TSystem.h"
#include "TTree.h"
#include <vector>


	TH1D* fhCumProbSortPi[12];
	TH1D* fhCumProbSortEl[12];
	Int_t fNofLayers = 12;
	std::vector<Float_t> inVector;
	std::vector<Float_t> inVector1;
	std::vector<Float_t> inVector2;
	Float_t xOut;

	Bool_t doTraining = false;
	TString fileNum = "0001";
	TString geoType = "st";

void transformInputData1(std::vector<Float_t> &inVector)
{
	for (Int_t j = 0; j < inVector.size(); j++) {
		inVector[j] = inVector[j] * 1e6;
		inVector[j] = (inVector[j] - ANNCoef1) / ANNCoef2 - 0.225;
	}
	sort(inVector.begin(), inVector.end());
	for (Int_t j = 0; j < inVector.size(); j++)
		inVector[j] = TMath::LandauI(inVector[j]);

}

void transformInputData2(std::vector<Float_t> &inVector)
{

	sort(inVector.begin(), inVector.end());
	for (Int_t j = 0; j < inVector.size(); j++) {
		Int_t binNum = fhCumProbSortEl[j]->FindBin(inVector[j]);
		if (binNum > fhCumProbSortEl[j]->GetNbinsX())
			binNum = fhCumProbSortEl[j]->GetNbinsX();
		inVector1[j] = fhCumProbSortEl[j]->GetBinContent(binNum);
	}

	for (Int_t j = 0; j < inVector.size(); j++) {
		Int_t binNum = fhCumProbSortPi[j]->FindBin(inVector[j]);
		if (binNum > fhCumProbSortPi[j]->GetNbinsX())
			binNum = fhCumProbSortPi[j]->GetNbinsX();
		inVector2[j] = fhCumProbSortPi[j]->GetBinContent(binNum);
	}

}

void trd_elid_tmva() {

	inVector.resize(fNofLayers);
	inVector1.resize(fNofLayers);
	inVector2.resize(fNofLayers);
	TString inputDir = "/d/cbm02/slebedev/trd/JUL09/"+geoType+"/";
	TString fileNameEl = inputDir + geoType+"_"+"electrons_mom_"+fileNum+".txt";
	TString fileNamePi = inputDir + geoType+"_"+"pions_mom_"+fileNum+".txt";
	TString fileNameTestEl = inputDir + geoType+"_"+"electrons_mom_"+fileNum+".txt";
	TString fileNameTestPi = inputDir + geoType+"_"+"pions_mom_"+fileNum+".txt";
	Int_t NofPi = 0, NofEl = 0;
	//Max number of electrons and pions for training samples
	Int_t MaxNofPi = 30000, MaxNofEl = 30000;
	Double_t mom, dEdX, tr;
	Double_t ANNCoef1 = 3.88;
	Double_t ANNCoef2 = 1.28;

	TFile* f = new TFile("/d/cbm02/slebedev/trd/JUN09/"+geoType+"/piel."+fileNum+".electronsQa.root", "READ");
	char hName[50];
	for (int i = 0; i < fNofLayers; i++) {
		sprintf(hName, "fhCumProbSortPi%d", i);
		fhCumProbSortPi[i] = (TH1D*) f->Get(hName);
		sprintf(hName, "fhCumProbSortEl%d", i);
		fhCumProbSortEl[i] = (TH1D*) f->Get(hName);
	}

	if (doTraining){
		TString outfileName("TMVA.root");
		TFile* outputFile = TFile::Open(outfileName, "RECREATE");

		TMVA::Factory *factory = new TMVA::Factory("TMVAnalysis", outputFile, Form(
				"!V:%sColor", gROOT->IsBatch() ? "!" : ""));

		std::ifstream finEl((const char*) fileNameEl);
		std::ifstream finPi((const char*) fileNamePi);

		TTree *simu = new TTree("MonteCarlo", "MontecarloData");
		simu->Branch("x1", &inVector1[0], "x1/F");
		simu->Branch("x2", &inVector1[1], "x2/F");
		simu->Branch("x3", &inVector1[2], "x3/F");
		simu->Branch("x4", &inVector1[3], "x4/F");
		simu->Branch("x5", &inVector1[4], "x5/F");
		simu->Branch("x6", &inVector1[5], "x6/F");
		simu->Branch("x7", &inVector1[6], "x7/F");
		simu->Branch("x8", &inVector1[7], "x8/F");
		simu->Branch("x9", &inVector1[8], "x9/F");
		simu->Branch("x10", &inVector1[9], "x10/F");
		simu->Branch("x11", &inVector1[10], "x11/F");
		simu->Branch("x12", &inVector1[11], "x12/F");

		simu->Branch("x13", &inVector2[0], "x13/F");
		simu->Branch("x14", &inVector2[1], "x14/F");
		simu->Branch("x15", &inVector2[2], "x15/F");
		simu->Branch("x16", &inVector2[3], "x16/F");
		simu->Branch("x17", &inVector2[4], "x17/F");
		simu->Branch("x18", &inVector2[5], "x18/F");
		simu->Branch("x19", &inVector2[6], "x19/F");
		simu->Branch("x20", &inVector2[7], "x20/F");
		simu->Branch("x21", &inVector2[8], "x21/F");
		simu->Branch("x22", &inVector2[9], "x22/F");
		simu->Branch("x23", &inVector2[10], "x23/F");
		simu->Branch("x24", &inVector2[11], "x24/F");

		simu->Branch("xOut", &xOut, "x25/F");

		cout << "Reading input files..." << endl;
		cout << "Reading pions..." << endl;

		while (!finPi.eof()) {
			for (Int_t iStation = 0; iStation < 12; iStation++) {
				finPi >> dEdX >> tr >> inVector[iStation];
				//cout << inVector[iStation] << " " ;
			}
			finPi >> mom;
			//cout << mom << endl;

			xOut = -1.;

			transformInputData2(inVector);

			simu->Fill();
			NofPi++;
			if (NofPi >= MaxNofPi) {
				break;
			}
		}
		cout << "Reading electrons..." << endl;
		while (!finEl.eof()) {
			for (Int_t iStation = 0; iStation < 12; iStation++) {
				finEl >> dEdX >> tr >> inVector[iStation];
				//cout << inVector[iStation] << " " ;
			}
			finEl >> mom;
			//cout << mom << endl;
			xOut = 1.;

			transformInputData2(inVector);

			simu->Fill();
			NofEl++;
			if (NofEl >= MaxNofEl) {
				break;
			}
		}
		cout << "TRAINING......." << endl;

		TCut piCut = "xOut<0";
		TCut elCut = "xOut>0";

		factory->SetInputTrees(simu, elCut, piCut);

		factory->AddVariable("x1", 'F');
		factory->AddVariable("x2", 'F');
		factory->AddVariable("x3", 'F');
		factory->AddVariable("x4", 'F');
		factory->AddVariable("x5", 'F');
		factory->AddVariable("x6", 'F');
		factory->AddVariable("x7", 'F');
		factory->AddVariable("x8", 'F');
		factory->AddVariable("x9", 'F');
		factory->AddVariable("x10", 'F');
		factory->AddVariable("x11", 'F');
		factory->AddVariable("x12", 'F');
		factory->AddVariable("x13", 'F');
		factory->AddVariable("x14", 'F');
		factory->AddVariable("x15", 'F');
		factory->AddVariable("x16", 'F');
		factory->AddVariable("x17", 'F');
		factory->AddVariable("x18", 'F');
		factory->AddVariable("x19", 'F');
		factory->AddVariable("x20", 'F');
		factory->AddVariable("x21", 'F');
		factory->AddVariable("x22", 'F');
		factory->AddVariable("x23", 'F');
		factory->AddVariable("x24", 'F');
		TCut mycuts = "";
		TCut mycutb = "";

		factory->PrepareTrainingAndTestTree(mycuts, mycutb,
				"NSigTrain=3000:NBkgTrain=3000:SplitMode=Random:NormMode=NumEvents:!V");

		factory->BookMethod(TMVA::Types::kTMlpANN, "TMlpANN",
				"!H:!V:NCycles=250:HiddenLayers=N+1"); // n_cycles:#nodes:#nodes:...

	//	factory->BookMethod(TMVA::Types::kBDT,"BDT",
	//			"!H:!V:NTrees=400:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20:PruneMethod=CostComplexity:PruneStrength=4.5");

		//  factory->BookMethod( TMVA::Types::kBDT, "BDTD",
		//                       "!H:!V:NTrees=400:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20:PruneMethod=CostComplexity:PruneStrength=4.5:VarTransform=Decorrelate" );

		factory->TrainAllMethods();
		//factory->TestAllMethods();
		//factory->EvaluateAllMethods();

		// Save the output
		//outputFile->Close();

		//delete factory;
	}//doTraining
	/////////////////////////////////////////////////////////////////////////
	//////////////////////////TEST///////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////

	cout << "TESTING......." << endl;

	TMVA::Reader* reader = new TMVA::Reader();
	reader->AddVariable("x1", &inVector1[0]);
	reader->AddVariable("x2", &inVector1[1]);
	reader->AddVariable("x3", &inVector1[2]);
	reader->AddVariable("x4", &inVector1[3]);
	reader->AddVariable("x5", &inVector1[4]);
	reader->AddVariable("x6", &inVector1[5]);
	reader->AddVariable("x7", &inVector1[6]);
	reader->AddVariable("x8", &inVector1[7]);
	reader->AddVariable("x9", &inVector1[8]);
	reader->AddVariable("x10", &inVector1[9]);
	reader->AddVariable("x11", &inVector1[10]);
	reader->AddVariable("x12", &inVector1[11]);

	reader->AddVariable("x13", &inVector2[0]);
	reader->AddVariable("x14", &inVector2[1]);
	reader->AddVariable("x15", &inVector2[2]);
	reader->AddVariable("x16", &inVector2[3]);
	reader->AddVariable("x17", &inVector2[4]);
	reader->AddVariable("x18", &inVector2[5]);
	reader->AddVariable("x19", &inVector2[6]);
	reader->AddVariable("x20", &inVector2[7]);
	reader->AddVariable("x21", &inVector2[8]);
	reader->AddVariable("x22", &inVector2[9]);
	reader->AddVariable("x23", &inVector2[10]);
	reader->AddVariable("x24", &inVector2[11]);

	//reader->BookMVA("BDT", "weights/TMVAnalysis_BDT.weights.txt");
	reader->BookMVA("TMlpANN", "weights/TMVAnalysis_TMlpANN.weights.txt");

	Double_t params[24];
	Double_t maxEval = 1.3;
	Double_t minEval = -1.3;

	TH1F* hANNOutputPi = new TH1F("hANNOutputPi","ANN output;ANN output;Entries", 100, minEval, maxEval);
	TH1F* hANNOutputEl = new TH1F("hANNOutputEl","ANN optput;ANN output;Entries", 100, minEval, maxEval);
	TH1F* hCumProbPi = new TH1F("hCumProbPi","ANN output;ANN output;Cumulative probability", 100, minEval,maxEval);
	TH1F* hCumProbEl = new TH1F("hCumProbEl","ANN optput;ANN output;Cumulative probability", 100, minEval,maxEval);
	TH1F* hANNErrorPi = new TH1F("hANNErrorPi","Error = ANN output - true;Error;Entries", 100, -0.5, 0.5);
	TH1F* hANNErrorEl = new TH1F("hANNErrorEl","Error = ANN output - true;Error;Entries", 100, -0.5, 0.5);

	//cut on the output value of ANN
	Double_t ANNCut = 0.15;

	//input data for testing

	std::ifstream finElTest((const char*) fileNameTestEl);
	std::ifstream finPiTest((const char*) fileNameTestPi);

	Int_t NofPiLikeEl = 0;
	Int_t NofElLikePi = 0;
	Int_t NofElTest = 0;
	Int_t NofPiTest = 0;

	cout << "Reading electrons..." << endl;
	while (!finElTest.eof()) {
		for (Int_t iStation = 0; iStation < 12; iStation++) {
			finElTest >> dEdX >> tr >> inVector[iStation];
		}
		finElTest >> mom;

		transformInputData2(inVector);

		Double_t nnEval = reader->EvaluateMVA("TMlpANN");
		if (nnEval > maxEval)nnEval = maxEval - 0.01;
		if (nnEval < minEval)nnEval = minEval + 0.01;
		hANNOutputEl->Fill(nnEval);
		hANNErrorEl->Fill(1. - nnEval);
		NofElTest++;
		if (nnEval < ANNCut)
			NofElLikePi++;
	}

	cout << "Reading pions..." << endl;
	while (!finPiTest.eof()) {
		for (Int_t iStation = 0; iStation < 12; iStation++) {
			finPiTest >> dEdX >> tr >> inVector[iStation];
		}
		finPiTest >> mom;

		transformInputData2(inVector);

		Double_t nnEval = reader->EvaluateMVA("TMlpANN");

		if (nnEval > maxEval)nnEval = maxEval - 0.01;
		if (nnEval < minEval)nnEval = minEval + 0.01;
		hANNOutputPi->Fill(nnEval);
		hANNErrorPi->Fill(-1. - nnEval);
		NofPiTest++;
		if (nnEval > ANNCut)
			NofPiLikeEl++;
	}

	cout << "NofEl = " << NofElTest << endl;
	cout << "NofPi = " << NofPiTest << endl;
	cout << "Pi like El = " << NofPiLikeEl << endl;
	cout << "El like Pi = " << NofElLikePi << endl;
	cout << "Pion supression = " << NofPiTest << "/" << NofPiLikeEl << " = "
			<< (double) NofPiTest / NofPiLikeEl << endl;
	cout << "Electron efficiency loss in % = " << NofElLikePi << "/"
			<< NofElTest << " = " << (double) NofElLikePi / NofElTest * 100
			<< endl;

	TCanvas* c1 = new TCanvas();
	c1->Divide(2, 2);
	c1->cd(1);
	hANNOutputEl->SetLineStyle(2);
	hANNOutputEl->SetLineWidth(3);
	hANNOutputPi->SetLineWidth(3);
	hANNOutputEl->Draw();
	hANNOutputPi->Draw("Same");
	gPad->SetLogy();

	c1->cd(2);
	hANNErrorEl->SetLineWidth(3.);
	hANNErrorEl->Draw();
	gPad->SetLogy();

	c1->cd(3);
	hANNErrorPi->SetLineWidth(3.);
	hANNErrorPi->Draw();
	gPad->SetLogy();

	Double_t cumProbPi = 0.;
	Double_t cumProbEl = 0.;
	Int_t nofPi = hANNOutputPi->GetEntries();
	Int_t nofEl = hANNOutputEl->GetEntries();
	for (Int_t i = 1; i <= hANNOutputEl->GetNbinsX(); i++) {
		cumProbPi += hANNOutputPi->GetBinContent(i);
		hCumProbPi->SetBinContent(i, (Double_t) cumProbPi / nofPi);

		cumProbEl += hANNOutputEl->GetBinContent(i);
		hCumProbEl->SetBinContent(i, 1. - (Double_t) cumProbEl / nofEl);
	}

	Double_t optimalCut = -1;
	for (Int_t i = 1; i <= hCumProbEl->GetNbinsX(); i++) {
		if (hCumProbEl->GetBinContent(i) <= 0.9) {
			optimalCut = hCumProbEl->GetBinCenter(i);
			break;
		}
	}
	cout << " optimal cut = " << optimalCut
			<< "    90% electron efficiency loss" << endl;

	c1->cd(4);
	hCumProbPi->SetLineWidth(3.);
	hCumProbPi->Draw();
	hCumProbEl->SetLineStyle(2);
	hCumProbEl->SetLineWidth(3.);
	hCumProbEl->Draw("same");
	gPad->SetGridx();
	gPad->SetGridy();
}
