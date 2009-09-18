
#include "CbmTrdElectronsTrainAnn.h"
#include "TMath.h"
#include "TFile.h"


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

	fIsDoTrain = false;
	fAnnWeightsFile = "Neural_Net_Weights.txt";
	fNofInputNeurons = 12;
	fNofHiddenNeurons = 12;
	fNofLayers = 12;
	fAnnCut = 0.9503;
	///d/cbm02/slebedev/trd/JUL09/st/piel.0001.reco.root
	TFile* f = new TFile("/d/cbm02/slebedev/trd/JUL09/"+geoType+"/piel."+fileNum+".reco.root", "READ");
	char hName[50];
	for (int i = 0; i < fNofLayers; i++) {
		sprintf(hName, "fhCumProbSortPi%d", i);
		fhCumProbSortPi[i] = (TH1D*) f->Get(hName);
		sprintf(hName, "fhCumProbSortEl%d", i);
		fhCumProbSortEl[i] = (TH1D*) f->Get(hName);
	}

//Histogramms for testing
	Int_t nofBins = 1000;
	fMaxEval = 1.3;
	fMinEval = -1.3;
	fhAnnOutputPi = new TH1D("fhAnnOutputPi","ANN output;ANN output;Entries",nofBins, fMinEval, fMaxEval);
	fhAnnOutputEl = new TH1D("fhAnnOutputEl","ANN optput;ANN output;Entries",nofBins, fMinEval, fMaxEval);
	fhCumProbPi = new TH1D("fhCumProbPi","Cumulative probability;ANN output;Cumulative probability",nofBins, fMinEval, fMaxEval);
	fhCumProbEl = new TH1D("fhCumProbEl","Cumulative probability;ANN output;Cumulative probability",nofBins, fMinEval, fMaxEval);

}

void CbmTrdElectronsTrainAnn::Run()
{
	if (fIsDoTrain) DoTrain();
	DoTest();

}

void CbmTrdElectronsTrainAnn::Transform()
{
	Transform2();
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
		if (binNum > fhCumProbSortPi[j]->GetNbinsX())
			binNum = fhCumProbSortPi[j]->GetNbinsX();
		fInVector[j] = fhCumProbSortPi[j]->GetBinContent(binNum);
	}
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

	TTree* simu = CreateTreeForAnn();

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

void CbmTrdElectronsTrainAnn::DoTest()
{
	if (fNN != NULL) delete fNN;
	fElossVec.clear();
	fElossVec.resize(fNofLayers);

	TTree* simu = CreateTreeForAnn();
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
		Double_t nnEval = fNN->Evaluate(0, &fInVector[0]);
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
		Double_t nnEval = fNN->Evaluate(0, &fInVector[0]);
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
TTree* CbmTrdElectronsTrainAnn::CreateTreeForAnn()
{
	fInVector.clear();
	fInVector.resize(fNofInputNeurons);

	TTree *simu = new TTree("MonteCarlo", "MontecarloData");
	char txt1[100], txt2[100];
	for (Int_t i = 0; i < fNofInputNeurons; i++){
		sprintf(txt1, "x%d",i);
		sprintf(txt2, "x%d/D", i);
		cout << txt1 << "|" << txt2 << "|" << endl;
		simu->Branch(txt1, &fInVector[i], txt2);
	}
	simu->Branch("xOut", &fXOut, "xOut/D");

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


ClassImp(CbmTrdElectronsTrainAnn)




