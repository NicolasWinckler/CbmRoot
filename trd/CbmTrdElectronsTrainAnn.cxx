
#include "CbmTrdElectronsTrainAnn.h"
#include "TMath.h"
#include "TFile.h"
#include "TCut.h"
#include "TSystem.h"
#include "TCanvas.h"

#include <iostream>
#include <vector>
using std::cout;
using std::endl;

CbmTrdElectronsTrainAnn::CbmTrdElectronsTrainAnn()
{
	Init();
}

CbmTrdElectronsTrainAnn::~CbmTrdElectronsTrainAnn()
{

}

void CbmTrdElectronsTrainAnn::Init()
{

	TString fileNum = "0001";
	TString geoType = "st";
	TString inputDir = "/d/cbm02/slebedev/trd/JUL09/"+geoType+"/";
	fFileNameEl = inputDir + geoType+"_"+"electrons_mom_"+fileNum+".txt";
	fFileNamePi = inputDir + geoType+"_"+"pions_mom_"+fileNum+".txt";
	fFileNameTestEl = inputDir + geoType+"_"+"electrons_mom_"+fileNum+".txt";
	fFileNameTestPi = inputDir + geoType+"_"+"pions_mom_"+fileNum+".txt";

	fIsDoTrain = true;
	fAnnCut = 0.9789;
	fTransformType = 1;
	fAnnWeightsFile = "Neural_Net_Weights.txt";
	fNofInputNeurons = 12;
	fNofHiddenNeurons = 20;
	fNofLayers = 12;
	TFile* f = new TFile("/d/cbm02/slebedev/trd/JUL09/"+geoType+"/piel."+fileNum+".reco.root", "READ");
	char hName[50];
	for (int i = 0; i < fNofLayers; i++) {
		sprintf(hName, "fhCumProbSortPi%d", i);
		fhCumProbSortPi[i] = (TH1D*) f->Get(hName)->Clone();
		sprintf(hName, "fhCumProbSortEl%d", i);
		fhCumProbSortEl[i] = (TH1D*) f->Get(hName)->Clone();
	}
	//f->Close();
	//delete f;

//Histogramms for testing
	Int_t nofBins = 1000;
	fMaxEval = 1.3;
	fMinEval = -1.3;
	fhAnnOutputPi = new TH1D("fhAnnOutputPi","ANN output;ANN output;Entries",nofBins, fMinEval, fMaxEval);
	fhAnnOutputEl = new TH1D("fhAnnOutputEl","ANN optput;ANN output;Entries",nofBins, fMinEval, fMaxEval);
	fhCumProbPi = new TH1D("fhCumProbPi","Cumulative probability;ANN output;Cumulative probability",nofBins, fMinEval, fMaxEval);
	fhCumProbEl = new TH1D("fhCumProbEl","Cumulative probability;ANN output;Cumulative probability",nofBins, fMinEval, fMaxEval);
    fhElNofClusters = new TH1D("fhElNofClusters","Number of clusters;number of clusters; entries",13, 0, 13);
    fhPiNofClusters = new TH1D("fhPiNofClusters","Number of clusters;number of clusters; entries",13, 0., 13.);
    fhElElossMediana= new TH1D("fhElElossMediana","Eloss mediana;mediana, GeV; entries",100, 0, 50e-6);
    fhPiElossMediana = new TH1D("fhPiElossMediana","Eloss mediana;mediana, GeV; entries",100, 0., 50e-6);

}

void CbmTrdElectronsTrainAnn::Run()
{
	if (fIsDoTrain) DoTrainTmva();
	DoTestTmva();
	DrawHistos();

}

void CbmTrdElectronsTrainAnn::Transform()
{
	if (fTransformType == 1){
		Transform1();
	} else if (fTransformType == 2){
		Transform2();
	} else if (fTransformType == 3) {
		Transform3();
	}
}

void CbmTrdElectronsTrainAnn::Transform1()
{
	Double_t ANNCoef1 = 1.06;
	Double_t ANNCoef2 = 0.57;
	for (Int_t i = 0; i<fElossVec.size(); i++) {
		fElossVec[i]=fElossVec[i]*1e6;
		fElossVec[i]=(fElossVec[i]-ANNCoef1)/ANNCoef2 -0.225;
	}
	sort(fElossVec.begin(), fElossVec.end());
	for (Int_t i = 0; i<fElossVec.size(); i++)
		fInVector[i]=TMath::LandauI(fElossVec[i]);
}

void CbmTrdElectronsTrainAnn::Transform2()
{
	sort(fElossVec.begin(), fElossVec.end());
//	for (Int_t j = 0; j < fElossVec.size(); j++) {
//		Int_t binNum = fhCumProbSortEl[j]->FindBin(fElossVec[j]);
//		if (binNum > fhCumProbSortEl[j]->GetNbinsX())
//			binNum = fhCumProbSortEl[j]->GetNbinsX();
//		fInVector[j] = fhCumProbSortEl[j]->GetBinContent(binNum);
//	}

	for (Int_t j = 0; j < fElossVec.size(); j++) {
		Int_t binNum = fhCumProbSortPi[j]->FindBin(fElossVec[j]);
		if (binNum > fhCumProbSortPi[j]->GetNbinsX()){
			binNum = fhCumProbSortPi[j]->GetNbinsX();
		}
		if (binNum > fhCumProbSortPi[j]->GetNbinsX() || binNum < 0){
			cout << "-E- bin num < 0" << endl;
		}
		fInVector[j] = fhCumProbSortPi[j]->GetBinContent(binNum);
	}
	//fInVector[fNofInputNeurons-2] = (fElossVec[5] + fElossVec[6])/2.;//GetNofClusters();
	//fInVector[fNofInputNeurons-1] = GetNofClusters();
}

void CbmTrdElectronsTrainAnn::Transform3()
{
	sort(fElossVec.begin(), fElossVec.end());
	for (Int_t j = 0; j < fElossVec.size(); j++) {
		Int_t binNum = fhCumProbSortEl[j]->FindBin(fElossVec[j]);
		if (binNum > fhCumProbSortEl[j]->GetNbinsX())
			binNum = fhCumProbSortEl[j]->GetNbinsX();
		fInVector[j] = fhCumProbSortEl[j]->GetBinContent(binNum);
	}

	for (Int_t j = 0; j < fElossVec.size(); j++) {
		Int_t binNum = fhCumProbSortPi[j]->FindBin(fElossVec[j]);
		if (binNum > fhCumProbSortPi[j]->GetNbinsX()){
			binNum = fhCumProbSortPi[j]->GetNbinsX();
		}
		fInVector[j+12] = fhCumProbSortPi[j]->GetBinContent(binNum);
	}
	//fInVector[fNofInputNeurons-2] = (fElossVec[5] + fElossVec[6])/2.;//GetNofClusters();
	//fInVector[fNofInputNeurons-1] = GetNofClusters();
}


//void CbmTrdElectronsTrainAnn::Transform3()
//{
//	sort(fElossVec.begin(), fElossVec.end());
//
//	for (Int_t i = 0; i < fElossVec.size(); i++) {
//		if (i != 11){
//			fInVector[i] = fElossVec[i+1] - fElossVec[i];
//		}else {
//			fInVector[i] = fElossVec[i] - fElossVec[0];
//		}
//	}
//	fInVector[fNofInputNeurons-1] = GetNofClusters();
//}

Int_t CbmTrdElectronsTrainAnn::GetNofClusters()
{
	Int_t nofClusters = 0;
	for (Int_t iHit = 0; iHit < fElossVec.size(); iHit++) {
		if ( fElossVec[iHit] > 5e-6){
			nofClusters++;
		}
	} //iHit
	return nofClusters;
}

void CbmTrdElectronsTrainAnn::DoTrain()
{
	std::ifstream finEl((const char*) fFileNameEl);
	std::ifstream finPi((const char*) fFileNamePi);

	Int_t nofPi = 0, nofEl = 0;
	//Max number of electrons and pions for training samples
	Int_t maxNofPi = 3000, maxNofEl = 3000;
	Double_t dEdX, tr, mom;
	std::vector<Double_t> inVectorTemp;
	inVectorTemp.resize(12);
	fElossVec.clear();
	fElossVec.resize(fNofLayers);

	TTree* simu = CreateTree();

	while (!finPi.eof()) {
		for (Int_t iStation = 0; iStation < 12; iStation++) {
			finPi >> dEdX >> tr >> inVectorTemp[iStation];
		}
		for (int i = 0; i < fNofLayers; i++)
			fElossVec[i] = inVectorTemp[i];

		finPi >> mom;
		fXOut = -1.;
		Transform();

		simu->Fill();
		nofPi++;
		if (nofPi >= maxNofPi) break;
	}

	while (!finEl.eof()) {
		for (Int_t iStation = 0; iStation < 12; iStation++) {
			finEl >> dEdX >> tr >> inVectorTemp[iStation];
		}
		for (int i = 0; i < fNofLayers; i++)
			fElossVec[i] = inVectorTemp[i];
		finEl >> mom;

		fXOut = 1.;
		Transform();

		simu->Fill();
		nofEl++;
		if (nofEl >= maxNofEl) break;
	}

	if (!fNN) delete fNN;
	TString mlpString = CreateAnnString();
	cout << "-I- create ANN: "<< mlpString << endl;

	fNN = new TMultiLayerPerceptron(mlpString,simu,"(Entry$+1)");
	fNN->Train(250, "+text,update=10");
	fNN->DumpWeights((const char*)fAnnWeightsFile);
}

Double_t CbmTrdElectronsTrainAnn::Eval(Bool_t isEl)
{
	//	//sort(fElossVec.begin(), fElossVec.end());
	//	//Double_t eval = (fElossVec[5] + fElossVec[6])/2.;
	//	Double_t eval = GetNofClusters();
	//	if(isEl){
	//	    fhElNofClusters->Fill(GetNofClusters());
	//	    fhElElossMediana->Fill(eval);
	//	}else {
	//	    fhPiNofClusters->Fill(GetNofClusters());
	//	    fhPiElossMediana->Fill(eval);
	//	}
//
//	if (eval > 4){
//		return 1;
//	} else {
//		return -1;
//	}
	return fReader->EvaluateMVA("BDT");
	//return fNN->Evaluate(0, &fInVector[0]);
}

void CbmTrdElectronsTrainAnn::DoTest()
{
	if (fNN != NULL) delete fNN;
	fElossVec.clear();
	fElossVec.resize(fNofLayers);

	TTree* simu = CreateTree();
	TString mlpString = CreateAnnString();
	fNN = new TMultiLayerPerceptron(mlpString,simu,"(Entry$+1)");
	fNN->LoadWeights((const char*)fAnnWeightsFile);

	Double_t dEdX, tr, mom;
	std::vector<Double_t> inVectorTemp;
	inVectorTemp.resize(12);
	//input data for testing
	std::ifstream finPiTest((const char*) fFileNameTestPi);
	std::ifstream finElTest((const char*) fFileNameTestEl);

	Int_t nofPiLikeEl = 0;
	Int_t nofElLikePi = 0;
	Int_t nofElTest = 0;
	Int_t nofPiTest = 0;

	while (!finElTest.eof()) {
		for (Int_t iStation = 0; iStation < 12; iStation++) {
			finElTest >> dEdX >> tr >> inVectorTemp[iStation];
		}
		finElTest >> mom;
		for (int i = 0; i < fNofLayers; i++)
			fElossVec[i] = inVectorTemp[i];

		Transform();
		Double_t nnEval = Eval(true);
		if (nnEval > fMaxEval)nnEval = fMaxEval - 0.01;
		if (nnEval < fMinEval)nnEval = fMinEval + 0.01;
		fhAnnOutputEl->Fill(nnEval);
		nofElTest++;
		if (nnEval < fAnnCut)nofElLikePi++;
	}

	while (!finPiTest.eof()) {
		for (Int_t iStation = 0; iStation < 12; iStation++) {
			finPiTest >> dEdX >> tr >> inVectorTemp[iStation];
		}
		for (int i = 0; i < fNofLayers; i++)
			fElossVec[i] = inVectorTemp[i];
		finPiTest >> mom;

		Transform();
		Double_t nnEval = Eval(false);
		if (nnEval > fMaxEval)nnEval = fMaxEval - 0.01;
		if (nnEval < fMinEval)nnEval = fMinEval + 0.01;
		fhAnnOutputPi->Fill(nnEval);
		nofPiTest++;
		if (nnEval > fAnnCut)nofPiLikeEl++;
	}
	cout << "Testing results:" <<endl;
	cout <<"nof El = " <<nofElTest<<endl;
	cout <<"nof Pi = " <<nofPiTest<<endl;
	cout <<"nof Pi identified as El = " <<nofPiLikeEl<<endl;
	cout <<"nof El identified as Pi = " <<nofElLikePi<<endl;
	cout << "pion supression = "<< nofPiTest<< "/"<< nofPiLikeEl<< " = ";
	if (nofPiLikeEl != 0){
		cout<< (Double_t) nofPiTest / nofPiLikeEl << endl;
	}else {
		cout<< " no misidentified pi" << endl;
	}
	cout << "electron efficiency loss in % = " << nofElLikePi << "/"
			<< nofElTest << " = " << (Double_t) nofElLikePi/nofElTest *100. << endl;

	CreateCumProbHistos();

	Double_t optimalCut = FindOptimalCut();
	cout << " optimal cut = " << optimalCut
			<< "    90% electron efficiency loss" << endl;

}

void CbmTrdElectronsTrainAnn::DoTrainTmva()
{
	TTree* simu = CreateTree();

	std::ifstream finEl((const char*) fFileNameEl);
	std::ifstream finPi((const char*) fFileNamePi);

	Int_t nofPi = 0, nofEl = 0;
	//Max number of electrons and pions for training samples
	Int_t maxNofPi = 6000, maxNofEl = 6000;
	Double_t dEdX, tr, mom;
	std::vector<Double_t> inVectorTemp;
	inVectorTemp.resize(12);
	fElossVec.clear();
	fElossVec.resize(fNofLayers);

	while (!finPi.eof()) {
		for (Int_t iStation = 0; iStation < 12; iStation++) {
			finPi >> dEdX >> tr >> inVectorTemp[iStation];
		}
		for (int i = 0; i < fNofLayers; i++)
			fElossVec[i] = inVectorTemp[i];
		finPi >> mom;
		fXOut = -1.;
		Transform();
		simu->Fill();
		nofPi++;
		if (nofPi >= maxNofPi) break;
	}

	while (!finEl.eof()) {
		for (Int_t iStation = 0; iStation < 12; iStation++) {
			finEl >> dEdX >> tr >> inVectorTemp[iStation];
		}
		for (int i = 0; i < fNofLayers; i++)
			fElossVec[i] = inVectorTemp[i];
		finEl >> mom;
		fXOut = 1.;
		Transform();
		simu->Fill();
		nofEl++;
		if (nofEl >= maxNofEl) break;
	}


	TMVA::Factory* factory = CreateFactory(simu);

	TCut mycuts = "";
	TCut mycutb = "";
	factory->PrepareTrainingAndTestTree(mycuts, mycutb,
			"NSigTrain=3000:NBkgTrain=3000:SplitMode=Random:NormMode=NumEvents:!V");

	//factory->BookMethod(TMVA::Types::kTMlpANN, "TMlpANN","!H:!V:NCycles=50:HiddenLayers=N+1");

	factory->BookMethod(TMVA::Types::kBDT,"BDT",
			"!H:!V:NTrees=400:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20:PruneMethod=CostComplexity:PruneStrength=4.5");

//  factory->BookMethod( TMVA::Types::kBDT, "BDTD",
//          "!H:!V:NTrees=400:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20:PruneMethod=CostComplexity:PruneStrength=4.5:VarTransform=Decorrelate" );

	factory->TrainAllMethods();
	//factory->TestAllMethods();
	//factory->EvaluateAllMethods();

	//outputFile->Close();
}

void CbmTrdElectronsTrainAnn::DoTestTmva()
{
	cout << "-I- Start testing " << endl;
	fElossVec.clear();
	fElossVec.resize(fNofLayers);

	fReader = CreateTmvaReader();

	//reader->BookMVA("TMlpANN", "weights/TMVAnalysis_TMlpANN.weights.txt");
	fReader->BookMVA("BDT", "weights/TMVAnalysis_BDT.weights.txt");

	Double_t dEdX, tr, mom;
	std::vector<Double_t> inVectorTemp;
	inVectorTemp.resize(12);
	//input data for testing
	std::ifstream finPiTest((const char*) fFileNameTestPi);
	std::ifstream finElTest((const char*) fFileNameTestEl);

	Int_t nofPiLikeEl = 0;
	Int_t nofElLikePi = 0;
	Int_t nofElTest = 0;
	Int_t nofPiTest = 0;

	cout << "-I- read data from file " << endl;
	while (!finElTest.eof()) {
		for (Int_t iStation = 0; iStation < 12; iStation++) {
			finElTest >> dEdX >> tr >> inVectorTemp[iStation];
		}
		finElTest >> mom;
		for (int i = 0; i < fNofLayers; i++)
			fElossVec[i] = inVectorTemp[i];

		Transform();
		Double_t nnEval = Eval(true);
		if (nnEval > fMaxEval)nnEval = fMaxEval - 0.01;
		if (nnEval < fMinEval)nnEval = fMinEval + 0.01;
		fhAnnOutputEl->Fill(nnEval);
		nofElTest++;
		if (nnEval < fAnnCut)nofElLikePi++;
	}

	while (!finPiTest.eof()) {
		for (Int_t iStation = 0; iStation < 12; iStation++) {
			finPiTest >> dEdX >> tr >> inVectorTemp[iStation];
		}
		for (int i = 0; i < fNofLayers; i++)
			fElossVec[i] = inVectorTemp[i];
		finPiTest >> mom;

		Transform();
		Double_t nnEval = Eval(false);
		if (nnEval > fMaxEval)nnEval = fMaxEval - 0.01;
		if (nnEval < fMinEval)nnEval = fMinEval + 0.01;
		fhAnnOutputPi->Fill(nnEval);
		nofPiTest++;
		if (nnEval > fAnnCut)nofPiLikeEl++;
	}

	cout << "Testing results:" <<endl;
	cout <<"nof El = " <<nofElTest<<endl;
	cout <<"nof Pi = " <<nofPiTest<<endl;
	cout <<"nof Pi identified as El = " <<nofPiLikeEl<<endl;
	cout <<"nof El identified as Pi = " <<nofElLikePi<<endl;
	cout << "pion supression = "<< nofPiTest<< "/"<< nofPiLikeEl<< " = ";
	if (nofPiLikeEl != 0){
		cout<< (Double_t) nofPiTest / nofPiLikeEl << endl;
	}else {
		cout<< " no misidentified pi" << endl;
	}
	cout << "electron efficiency loss in % = " << nofElLikePi << "/"
			<< nofElTest << " = " << (Double_t) nofElLikePi/nofElTest *100. << endl;

	CreateCumProbHistos();

	Double_t optimalCut = FindOptimalCut();
	cout << " optimal cut = " << optimalCut
			<< "    90% electron efficiency loss" << endl;

}



void CbmTrdElectronsTrainAnn::CreateCumProbHistos()
{
	Double_t cumProbPi = 0.;
	Double_t cumProbEl = 0.;
	Double_t nofPi = fhAnnOutputPi->GetEntries();
	Double_t nofEl = fhAnnOutputEl->GetEntries();
	for (Int_t i = 1; i <= fhAnnOutputEl->GetNbinsX(); i++) {
		cumProbPi += fhAnnOutputPi->GetBinContent(i);
		fhCumProbPi->SetBinContent(i, (Double_t)cumProbPi/nofPi);

		cumProbEl += fhAnnOutputEl->GetBinContent(i);
		fhCumProbEl->SetBinContent(i, 1. - (Double_t)cumProbEl / nofEl);
	}
}

void CbmTrdElectronsTrainAnn::RunMany()
{

}

void CbmTrdElectronsTrainAnn::CreateROCDiagramm()
{

}

Double_t CbmTrdElectronsTrainAnn::FindOptimalCut()
{
	Double_t optimalCut = -1;
	for (Int_t i = 1; i <= fhCumProbEl->GetNbinsX(); i++) {
		if (fhCumProbEl->GetBinContent(i) <= 0.9) {
			optimalCut = fhCumProbEl->GetBinCenter(i);
			return optimalCut;
		}
	}
}
TTree* CbmTrdElectronsTrainAnn::CreateTree()
{
	fInVector.clear();
	fInVector.resize(fNofInputNeurons);
	TTree *simu = new TTree("MonteCarlo", "MontecarloData");
	char txt1[100], txt2[100];
	for (Int_t i = 0; i < fNofInputNeurons; i++){
		sprintf(txt1, "x%d",i);
		sprintf(txt2, "x%d/F", i);
		simu->Branch(txt1, &fInVector[i], txt2);
	}
	simu->Branch("xOut", &fXOut, "xOut/F");

	return simu;
}

TString CbmTrdElectronsTrainAnn::CreateAnnString()
{
	TString st = "";
	char txt[50];
	for (Int_t i = 0; i < fNofInputNeurons - 1; i++){
		sprintf(txt, "x%d",i);
		st = st + txt + ",";

	}
	sprintf(txt, "x%d",fNofInputNeurons - 1);
	st = st + txt+ "";
	sprintf(txt, "%d",fNofHiddenNeurons);
	st = st+ ":"+txt+":xOut";
	return st;
}

TMVA::Factory* CbmTrdElectronsTrainAnn::CreateFactory(TTree* simu)
{
	TFile* outputFile = TFile::Open("TMVA.root", "RECREATE");

	TMVA::Factory *factory = new TMVA::Factory("TMVAnalysis", outputFile);

	TCut piCut = "xOut<0";
	TCut elCut = "xOut>0";

	factory->SetInputTrees(simu, elCut, piCut);
	char txt1[100];
	for (Int_t i = 0; i < fNofInputNeurons; i++){
		sprintf(txt1, "x%d",i);
		factory->AddVariable(txt1, 'F');
	}

	return factory;
}

TMVA::Reader* CbmTrdElectronsTrainAnn::CreateTmvaReader()
{
	if (!fReader) delete fReader;

	fInVector.clear();
	fInVector.resize(fNofInputNeurons);

	TMVA::Reader* reader = new TMVA::Reader();

	char txt1[100];
	for (Int_t i = 0; i < fNofInputNeurons; i++){
		sprintf(txt1, "x%d",i);
		cout << txt1 << endl;
		reader->AddVariable(txt1, &fInVector[i]);
	}
	return reader;
}

void CbmTrdElectronsTrainAnn::DrawHistos()
{

	TCanvas* c1 = new TCanvas();
	c1->Divide(2, 2);
	c1->cd(1);
	fhAnnOutputEl->SetLineStyle(2);
	fhAnnOutputEl->SetLineWidth(3);
	fhAnnOutputPi->SetLineWidth(3);
	fhAnnOutputEl->Rebin(10);
	fhAnnOutputPi->Rebin(10);
	fhAnnOutputEl->Draw();
	fhAnnOutputPi->Draw("Same");
	gPad->SetLogy();

	c1->cd(2);
	fhCumProbPi->SetLineWidth(3);
	fhCumProbPi->Draw();
	fhCumProbEl->SetLineStyle(2);
	fhCumProbEl->SetLineWidth(3);
	fhCumProbEl->Draw("same");
	gPad->SetGridx();
	gPad->SetGridy();

	fhElNofClusters->SetLineWidth(3);
	fhPiNofClusters->SetLineWidth(3);
    fhElElossMediana->SetLineWidth(3);
    fhPiElossMediana->SetLineWidth(3);

	fhElNofClusters->SetLineStyle(2);
	fhElElossMediana->SetLineStyle(2);

	c1->cd(3);
	fhElNofClusters->Draw();
	fhPiNofClusters->Draw("same");
	gPad->SetLogy(true);
	c1->cd(4);
	fhElElossMediana->Draw();
	fhPiElossMediana->Draw("same");
}

ClassImp(CbmTrdElectronsTrainAnn)
