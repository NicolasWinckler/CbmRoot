
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

	fIsDoTrain = true;
	fAnnCut = 0.9789;
	fIdMethod == kBDT;
	fTransformType = 1;
	fAnnWeightsFile = "Neural_Net_Weights.txt";
	fNofInputNeurons = 12;
	fNofHiddenNeurons = 12;
	fNofLayers = 12;
	InitCumHistos();

//Histogramms for testing
	Int_t nofBins = 2000;
	fMaxEval = 1.3;
	fMinEval = -1.3;
	fhAnnOutputPi = new TH1D("fhAnnOutputPi","Output;Output;Entries",nofBins, fMinEval, fMaxEval);
	fhAnnOutputEl = new TH1D("fhAnnOutputEl","Optput;Output;Entries",nofBins, fMinEval, fMaxEval);
	fhCumProbPi = new TH1D("fhCumProbPi","Cumulative probability;ANN output;Cumulative probability",nofBins, fMinEval, fMaxEval);
	fhCumProbEl = new TH1D("fhCumProbEl","Cumulative probability;ANN output;Cumulative probability",nofBins, fMinEval, fMaxEval);
    fhElNofClusters = new TH1D("fhElNofClusters","Number of clusters;number of clusters; entries",13, 0, 13);
    fhPiNofClusters = new TH1D("fhPiNofClusters","Number of clusters;number of clusters; entries",13, 0., 13.);
    fhElElossMediana= new TH1D("fhElElossMediana","Eloss mediana;mediana, GeV; entries",100, 0, 1);
    fhPiElossMediana = new TH1D("fhPiElossMediana","Eloss mediana;mediana, GeV; entries",100, 0., 1);
}

void CbmTrdElectronsTrainAnn::InitCumHistos()
{
	cout << 0 << endl;
	for (int i = 0; i < fNofLayers; i++) {
		if (!fhCumProbSortPi[i]) delete fhCumProbSortPi[i];
		if (!fhCumProbSortEl[i]) delete fhCumProbSortEl[i];
	}
	cout << 1 << endl;
	TFile* f = new TFile(fFileNameCumHistos, "READ");
	if (!f->IsOpen()){
		cout << "-E- FILE NOT FOUND fFileNameCumHistos: "<< fFileNameCumHistos << endl;
		cout << "-E- NO TRAINING WILL BE PERFORMED!!!" << endl;
		return;
	}
	cout << 2 << endl;
	char hName[50];
	for (int i = 0; i < fNofLayers; i++) {
		sprintf(hName, "fhCumProbSortPi%d", i);
		fhCumProbSortPi[i] = (TH1D*) f->Get(hName)->Clone();
		sprintf(hName, "fhCumProbSortEl%d", i);
		fhCumProbSortEl[i] = (TH1D*) f->Get(hName)->Clone();
	}
	cout << 3 << endl;
	//f->Close();
	//delete f;
}

void CbmTrdElectronsTrainAnn::Run()
{
	if (fIdMethod == kBDT){
		if (fIsDoTrain) DoTrainTmva();
		DoTest();
	}else if (fIdMethod == kANN){
		if (fIsDoTrain) DoTrain();
		DoTest();
	}else if (fIdMethod == kCLUSTERS){
		DoTest();
	}else if (fIdMethod == kMEDIANA){
		DoTest();
	}
	DrawHistos();

}

void CbmTrdElectronsTrainAnn::Transform()
{
	if (fIdMethod == kBDT || fIdMethod == kANN){
		if (fTransformType == 1){
			Transform1();
		} else if (fTransformType == 2){
			Transform2();
		} else if (fTransformType == 3) {
			Transform3();
		}
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
	Int_t size = fElossVec.size();
	//size = 6;
	for (Int_t j = 0; j < size; j++) {
		Int_t binNum = fhCumProbSortPi[j]->FindBin(fElossVec[j]);
		if (binNum > fhCumProbSortPi[j]->GetNbinsX()){
			binNum = fhCumProbSortPi[j]->GetNbinsX();
		}
		fInVector[j] = fhCumProbSortPi[j]->GetBinContent(binNum);
	}

	//fInVector[fNofInputNeurons-2] = (fElossVec[5] + fElossVec[6])/2.;//GetNofClusters();
	//fInVector[fNofInputNeurons-1] = GetNofClusters();
}

void CbmTrdElectronsTrainAnn::Transform3()
{
	sort(fElossVec.begin(), fElossVec.end());

//	for (Int_t j = 0; j < fElossVec.size(); j++) {
//		Int_t binNum = fhCumProbSortPi[j]->FindBin(fElossVec[j]);
//		if (binNum > fhCumProbSortPi[j]->GetNbinsX()){
//			binNum = fhCumProbSortPi[j]->GetNbinsX();
//		}
//		fInVector[j] = fhCumProbSortPi[j]->GetBinContent(binNum);
//	}
	Int_t size = fElossVec.size();
	for (Int_t j = 0; j < size; j++) {
		Int_t binNum = fhCumProbSortEl[j]->FindBin(fElossVec[j]);
		if (binNum > fhCumProbSortEl[j]->GetNbinsX())
			binNum = fhCumProbSortEl[j]->GetNbinsX();
		fInVector[j] = fhCumProbSortEl[j]->GetBinContent(binNum);
	}

	//fInVector[fNofInputNeurons-2] = (fElossVec[5] + fElossVec[6])/2.;//GetNofClusters();
	//fInVector[fNofInputNeurons-1] = GetNofClusters();
}

Int_t CbmTrdElectronsTrainAnn::GetNofClusters()
{
	Int_t nofClusters = 0;
	for (Int_t iHit = 0; iHit < fElossVec.size(); iHit++) {
		if ( fElossVec[iHit] > 7.e-6){
			nofClusters++;
		}
	} //iHit
	return nofClusters;
}

Double_t CbmTrdElectronsTrainAnn::Eval(Bool_t isEl)
{

	if (fIdMethod == kBDT){
		return fReader->EvaluateMVA("BDT");
	}else if (fIdMethod == kANN){
		Double_t par[12];
		for (UInt_t i = 0; i <fInVector.size(); i++) par[i] = fInVector[i];
		return fNN->Evaluate(0, par);
	}else if (fIdMethod == kCLUSTERS){
		Double_t eval = GetNofClusters();
		if (eval > 4.5) return 1.;
		return -1.;
	}else if (fIdMethod == kMEDIANA){
		sort(fElossVec.begin(), fElossVec.end());
		Double_t eval = (fElossVec[5] + fElossVec[6])/2.;
		if (eval > 4.56e-6) return 1.;
		return -1.;

	}
}

void CbmTrdElectronsTrainAnn::DoTrain()
{
	if (!FileExists(fFileNameEl) ){
		cout << "-E- FILE NOT FOUND fFileNameEl: "<< fFileNameEl << endl;
		cout << "-E- NO TRAINING WILL BE PERFORMED!!!" << endl;
		return;
	}
	if (!FileExists(fFileNamePi)){
		cout << "-E- FILE NOT FOUND fFileNamePi: "<< fFileNamePi << endl;
		cout << "-E- NO TRAINING WILL BE PERFORMED!!!" << endl;
		return;
	}

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
	finPi.close();
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
	finEl.close();
	if (!fNN) delete fNN;
	TString mlpString = CreateAnnString();
	cout << "-I- create ANN: "<< mlpString << endl;

	fNN = new TMultiLayerPerceptron(mlpString,simu,"(Entry$+1)");
	fNN->Train(250, "+text,update=10");
	fNN->DumpWeights((const char*)fAnnWeightsFile);
}

void CbmTrdElectronsTrainAnn::DoTrainTmva()
{
	if (!FileExists(fFileNameEl) ){
		cout << "-E- FILE NOT FOUND fFileNameEl: "<< fFileNameEl << endl;
		cout << "-E- NO TRAINING WILL BE PERFORMED!!!" << endl;
		return;
	}
	if (!FileExists(fFileNamePi)){
		cout << "-E- FILE NOT FOUND fFileNamePi: "<< fFileNamePi << endl;
		cout << "-E- NO TRAINING WILL BE PERFORMED!!!" << endl;
		return;
	}

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
	finPi.close();
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
	finEl.close();

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

void CbmTrdElectronsTrainAnn::DoPreTest()
{
	if (!FileExists(fFileNameTestEl) ){
		cout << "-E- FILE NOT FOUND fFileNameTestEl: "<< fFileNameTestEl << endl;
		cout << "-E- NO TESTING WILL BE PERFORMED!!!" << endl;
		return;
	}
	if (!FileExists(fFileNameTestPi)){
		cout << "-E- FILE NOT FOUND fFileNameTestPi: "<< fFileNameTestPi << endl;
		cout << "-E- NO TESTING WILL BE PERFORMED!!!" << endl;
		return;
	}

	cout << "-I- Start pretesting " << endl;
	if (fIdMethod == kBDT){
		cout << "-I- IdMethod = kBDT " << endl;
	}else if (fIdMethod == kANN){
		cout << "-I- IdMethod = kANN " << endl;
	}else if (fIdMethod == kCLUSTERS){
		cout << "-I- IdMethod = kCLUSTERS " << endl;
	}else if (fIdMethod == kMEDIANA){
		cout << "-I- IdMethod = kMEDIANA " << endl;
	}

	fElossVec.clear();
	fElossVec.resize(fNofLayers);

	if (fIdMethod == kBDT){
		fReader = CreateTmvaReader();
		//reader->BookMVA("TMlpANN", "weights/TMVAnalysis_TMlpANN.weights.txt");
		fReader->BookMVA("BDT", "weights/TMVAnalysis_BDT.weights.txt");
	} else if (fIdMethod == kANN){
		if (fNN != NULL) delete fNN;
		TTree* simu = CreateTree();
		TString mlpString = CreateAnnString();
		fNN = new TMultiLayerPerceptron(mlpString,simu,"(Entry$+1)");
		fNN->LoadWeights((const char*)fAnnWeightsFile);
	}

	Double_t dEdX, tr, mom;
	std::vector<Double_t> inVectorTemp;
	inVectorTemp.resize(12);
	//input data for testing
	std::ifstream finPiTest((const char*) fFileNameTestPi);
	std::ifstream finElTest((const char*) fFileNameTestEl);

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
	}
	finElTest.close();

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
	}
	finPiTest.close();
	CreateCumProbHistos();
	CreateROCDiagramm();
}


void CbmTrdElectronsTrainAnn::DoTest()
{
	if (!FileExists(fFileNameTestEl) ){
		cout << "-E- FILE NOT FOUND fFileNameTestEl: "<< fFileNameTestEl << endl;
		cout << "-E- NO TESTING WILL BE PERFORMED!!!" << endl;
		return;
	}
	if (!FileExists(fFileNameTestPi)){
		cout << "-E- FILE NOT FOUND fFileNameTestPi: "<< fFileNameTestPi << endl;
		cout << "-E- NO TESTING WILL BE PERFORMED!!!" << endl;
		return;
	}

	DoPreTest();
	fAnnCut = FindOptimalCut();
	cout << " optimal cut = " << fAnnCut
			<< " for 90% electron efficiency" << endl;
	//fAnnCut = 0.5;
	cout << "-I- Start testing " << endl;
	fElossVec.clear();
	fElossVec.resize(fNofLayers);

	if (fIdMethod == kBDT){
		fReader = CreateTmvaReader();
		//reader->BookMVA("TMlpANN", "weights/TMVAnalysis_TMlpANN.weights.txt");
		fReader->BookMVA("BDT", "weights/TMVAnalysis_BDT.weights.txt");
	} else if (fIdMethod == kANN){
		if (fNN != NULL) delete fNN;
		TTree* simu = CreateTree();
		TString mlpString = CreateAnnString();
		fNN = new TMultiLayerPerceptron(mlpString,simu,"(Entry$+1)");
		fNN->LoadWeights((const char*)fAnnWeightsFile);
	}

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
		//fhAnnOutputEl->Fill(nnEval);
		nofElTest++;
		if (nnEval < fAnnCut)nofElLikePi++;
	}
	finElTest.close();

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
		//fhAnnOutputPi->Fill(nnEval);
		nofPiTest++;
		if (nnEval > fAnnCut)nofPiLikeEl++;
	}
	finPiTest.close();

	cout << "Testing results:" << endl;
	cout << "cut = " << fAnnCut << endl;
	cout <<"nof El = " <<nofElTest<<endl;
	cout <<"nof Pi = " <<nofPiTest<<endl;
	cout <<"nof Pi identified as El = " <<nofPiLikeEl<<endl;
	cout <<"nof El identified as Pi = " <<nofElLikePi<<endl;
	cout << "pion suppression = "<< nofPiTest<< "/"<< nofPiLikeEl<< " = ";
	if (nofPiLikeEl != 0){
		cout<< (Double_t) nofPiTest / nofPiLikeEl << endl;
	}else {
		cout<< " no misidentified pi" << endl;
	}
	cout << "electron efficiency loss in % = " << nofElLikePi << "/"
			<< nofElTest << " = " << (Double_t) nofElLikePi/nofElTest *100. << endl;

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
	Int_t nBins = fhCumProbEl->GetNbinsX();
	Double_t x[nBins], y[nBins];

	for (Int_t i = 1; i <= nBins; i++) {
		Double_t cpi = 100.*fhCumProbPi->GetBinContent(i);
		if (cpi == 100.)cpi = 100. - 0.0001;
		y[i-1] = 100./(100. - cpi);
		x[i-1] = 100.*fhCumProbEl->GetBinContent(i);
	}
	fROCGraph = new TGraph(nBins, x, y);
	fROCGraph->SetTitle("ROC diagramm");
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
		//cout << txt1 << endl;
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

	c1->cd(3);
	fROCGraph->SetLineWidth(3);
	fROCGraph->Draw("AL");

	TCanvas* c2 = new TCanvas();
	c2->Divide(2, 1);
	fhElNofClusters->SetLineWidth(3);
	fhPiNofClusters->SetLineWidth(3);
    fhElElossMediana->SetLineWidth(3);
    fhPiElossMediana->SetLineWidth(3);

	fhElNofClusters->SetLineStyle(2);
	fhElElossMediana->SetLineStyle(2);

	c2->cd(1);
	fhElNofClusters->Draw();
	fhPiNofClusters->Draw("same");
	gPad->SetLogy(true);
	c2->cd(2);
	fhElElossMediana->Draw();
	fhPiElossMediana->Draw("same");


}

Bool_t CbmTrdElectronsTrainAnn::FileExists(TString fileName)
{
	bool flag = false;
	fstream fin;
	fin.open((const char*) fileName, std::ios::in);
	if (fin.is_open()) {
		flag = true;
	}
	fin.close();
	return flag;
}

ClassImp(CbmTrdElectronsTrainAnn)
