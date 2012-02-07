
#include "CbmTrdElectronsTrainAnn.h"
#include "TMath.h"
#include "TFile.h"
#include "TCut.h"
#include "TSystem.h"
#include "TRandom.h"
#include "TCanvas.h"
#include "TMVA/PDF.h"
#include "TMVA/Config.h"
#include <iostream>
#include <vector>
#include "TLine.h"
using std::cout;
using std::endl;
//#include "../littrack/utils/CbmLitDrawHist.cxx"
//#include "../littrack/utils/CbmLitUtils.cxx"
CbmTrdElectronsTrainAnn::CbmTrdElectronsTrainAnn() 
  : fFileNameEl(""),
    fFileNamePi(""),
    fFileNameTestEl(""),
    fFileNameTestPi(""),
    fFileNameCumHistos(""),
    fWeightFileDir(""),
    fSigmaError(-1.),
    fIsDoTrain(kTRUE),
    fTransformType(-1),
    fhCumProbSortPi(),
    fhCumProbSortEl(),
    fhPdfSortPi(),
    fhPdfSortEl(),
    fElossVec(),
    fInVector(),
    fElossPi(),
    fElossEl(),
    fFileNum(-1),
    fXOut(-1.),
    fAnnCut(-1.),
    fNofInputNeurons(-1),
    fNofHiddenNeurons(-1),
    fNofLayers(-1.),
    fMaxEval(-1.),
    fMinEval(-1.),
    fAnnWeightsFile(""),
    fNN(NULL),
    fReader(NULL),
    fIdMethod(),
    fNofAnnEpochs(-1),
    fMaxNofTrainPi(-1),
    fMaxNofTrainEl(-1),
    fRandom(),
    foutResults(),
    fhAnnOutputPi(NULL),
    fhAnnOutputEl(NULL),
    fhCumProbPi(NULL),
    fhCumProbEl(NULL),
    fhElNofClusters(NULL),
    fhPiNofClusters(NULL),
    fhElElossMediana(NULL),
    fhPiElossMediana(NULL), 
    fhPiProbSortEl(),
    fhPiProbSortPi(),
    fhElProbSortEl(),
    fhElProbSortPi(),
    fhMulProbSortEl(),
    fhMulProbSortPi(),
    fhInputEl(),
    fhInputPi(),
    fROCGraph(NULL)
{
	Init();
}

CbmTrdElectronsTrainAnn::~CbmTrdElectronsTrainAnn()
{
//	delete fhAnnOutputPi;
//	delete fhAnnOutputEl;
//	delete fhCumProbPi;
//	delete fhCumProbEl;
//	delete fhElNofClusters;
//	delete fhPiNofClusters;
//	delete fhElElossMediana;

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
	fNofAnnEpochs = 250;
	fNofLayers = 12;
	InitCumHistos();

	fElossPi.clear();
	fElossEl.clear();
	fElossPi.reserve(1000000);
	fElossEl.reserve(1000000);

//Histogramms for testing
	Int_t nofBins = 2000;
	fMaxEval = 1.3;
	fMinEval = -1.3;
	fhAnnOutputPi = new TH1D("fhAnnOutputPi","Output;output;yield",nofBins, fMinEval, fMaxEval);
	fhAnnOutputEl = new TH1D("fhAnnOutputEl","Optput;output;yield",nofBins, fMinEval, fMaxEval);
	fhCumProbPi = new TH1D("fhCumProbPi","Cumulative probability;output;cumulative probability",nofBins, fMinEval, fMaxEval);
	fhCumProbEl = new TH1D("fhCumProbEl","Cumulative probability;output;cumulative probability",nofBins, fMinEval, fMaxEval);
    fhElNofClusters = new TH1D("fhElNofClusters","Number of clusters;number of clusters; entries",13, 0, 13);
    fhPiNofClusters = new TH1D("fhPiNofClusters","Number of clusters;number of clusters; entries",13, 0., 13.);
    fhElElossMediana= new TH1D("fhElElossMediana","Eloss mediana;mediana, GeV; entries",100, 0, 1);
    fhPiElossMediana = new TH1D("fhPiElossMediana","Eloss mediana;mediana, GeV; entries",100, 0., 1);

	char histName[200];
	char histTitle[200];
	for (Int_t i = 0; i < 12; i++){
		sprintf(histName,"fhPiProbSortEl%d",i);
		sprintf(histTitle, "Pi prob. in %d hit;Probability;Entries", i);
		fhPiProbSortEl[i] = new TH1D(histName, histTitle, 100, -1.1, 1.1);
		sprintf(histName,"fhPiProbSortPi%d",i);
		fhPiProbSortPi[i] = new TH1D(histName, histTitle, 100, -1.1, 1.1);

		sprintf(histName,"fhElProbSortEl%d",i);
		sprintf(histTitle, "El prob. in %d hit;Probability;Entries", i);
		fhElProbSortEl[i] = new TH1D(histName, histTitle, 100, -1.1, 1.1);
		sprintf(histName,"fhElProbSortPi%d",i);
		fhElProbSortPi[i] = new TH1D(histName, histTitle, 100, -1.1, 1.1);

		sprintf(histName,"fhMulProbSortEl%d",i);
		sprintf(histTitle, "Mul prob. in %d hit;Probability;Entries", i);
		fhMulProbSortEl[i] = new TH1D(histName, histTitle, 100, -1.1, 1.1);
		sprintf(histName,"fhMulProbSortPi%d",i);
		fhMulProbSortPi[i] = new TH1D(histName, histTitle, 100, -1.1, 1.1);

		sprintf(histName,"fhInputEl%d",i);
		sprintf(histTitle, "Input in %d hit;Input;Entries", i);
		fhInputEl[i] = new TH1D(histName, histTitle, 100, -1.1, 1.1);
		sprintf(histName,"fhInputPi%d",i);
		fhInputPi[i] = new TH1D(histName, histTitle, 100, -1.1, 1.1);
	}
	fRandom = new TRandom();
	foutResults.open("trd_elid_results.txt",std::ios_base::app);
}

void CbmTrdElectronsTrainAnn::InitCumHistos()
{
	for (int i = 0; i < fNofLayers; i++) {
		if (!fhCumProbSortPi[i]) delete fhCumProbSortPi[i];
		if (!fhCumProbSortEl[i]) delete fhCumProbSortEl[i];
	}
	cout << "-I- file for cum histogramms: " << fFileNameCumHistos <<endl;

	TFile* f = new TFile(fFileNameCumHistos, "READ");
	if (!f->IsOpen()){
		cout << "-E- FILE NOT FOUND fFileNameCumHistos: "<< fFileNameCumHistos << endl;
		cout << "-E- NO TRAINING WILL BE PERFORMED!!!" << endl;
		return;
	}

	char hName[50];
	for (int i = 0; i < fNofLayers; i++) {
		sprintf(hName, "fhCumProbSortPi%d", i);
		fhCumProbSortPi[i] = (TH1D*) f->Get(hName)->Clone();
		sprintf(hName, "fhCumProbSortEl%d", i);
		fhCumProbSortEl[i] = (TH1D*) f->Get(hName)->Clone();
		sprintf(hName, "fhElossSortPi%d", i);
		fhPdfSortPi[i] = (TH1D*) f->Get(hName)->Clone();
		fhPdfSortPi[i]->Rebin(20);
		fhPdfSortPi[i]->Scale(1./fhPdfSortPi[i]->Integral());
		sprintf(hName, "fhElossSortEl%d", i);
		fhPdfSortEl[i] = (TH1D*) f->Get(hName)->Clone();
		fhPdfSortEl[i]->Rebin(20);
		fhPdfSortEl[i]->Scale(1./fhPdfSortEl[i]->Integral());
	}
	//f->Close();
	//delete f;
}

void CbmTrdElectronsTrainAnn::Run()
{
	ReadFile();
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
	}else if (fIdMethod == kLIKELIHOOD){
		DoTest();
	}
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
		} else if (fTransformType == 4) {
			Transform4();
		}
	}
}

void CbmTrdElectronsTrainAnn::Transform1()
{
//	Double_t ANNCoef1 = 1.06;
//	Double_t ANNCoef2 = 0.57;
	Double_t ANNCoef1[] = {1.04,1.105,1.154,1.277,1.333,1.394,1.47,1.50,1.54,1.58};
	Double_t ANNCoef2[] = {0.548,0.567,0.585,0.63,0.645,0.664,0.69,0.705,0.716,0.723};
	for (Int_t i = 0; i<fElossVec.size(); i++) {
		fInVector[i]=fElossVec[i]*1e6;
		fInVector[i]=(fInVector[i]-ANNCoef1[fFileNum])/ANNCoef2[fFileNum] -0.225;
	}
	sort(fInVector.begin(), fInVector.end());
	for (Int_t i = 0; i<fInVector.size(); i++)
		fInVector[i]=TMath::LandauI(fInVector[i]);
}

void CbmTrdElectronsTrainAnn::Transform2()
{
	sort(fElossVec.begin(), fElossVec.end());
	Int_t size = fElossVec.size();
	for (Int_t j = 0; j < size; j++) {
		Int_t binNum = fhCumProbSortPi[j]->FindBin(fElossVec[j]);
		if (binNum > fhCumProbSortPi[j]->GetNbinsX()){
			binNum = fhCumProbSortPi[j]->GetNbinsX();
		}
		Double_t prob = fhCumProbSortPi[j]->GetBinContent(binNum);
		fInVector[j] = prob;

//		Int_t binNumPi = fhCumProbSortPi[j]->FindBin(fElossVec[j]);
//		if (binNumPi > fhCumProbSortPi[j]->GetNbinsX()){
//			binNumPi = fhCumProbSortPi[j]->GetNbinsX();
//		}
//	    Double_t probPi = fhCumProbSortPi[j]->GetBinContent(binNumPi);
//		probPi = 0.3*TMath::ErfInverse(2*probPi-1.);
//		if (probPi == 0.) probPi = 1.;
//		fInVector[j] = probPi;
	}


	//fInVector[fNofInputNeurons-2] = (fElossVec[5] + fElossVec[6])/2.;//GetNofClusters();
	//fInVector[fNofInputNeurons-1] = GetNofClusters();
}

void CbmTrdElectronsTrainAnn::Transform3()
{

	sort(fElossVec.begin(), fElossVec.end());

	Int_t size = fElossVec.size();
	for (Int_t j = 0; j < size; j++) {
//		Int_t binNum = fhCumProbSortEl[j]->FindBin(fElossVec[j]);
//		if (binNum > fhCumProbSortEl[j]->GetNbinsX())
//			binNum = fhCumProbSortEl[j]->GetNbinsX();
//
//		Double_t prob = fhCumProbSortEl[j]->GetBinContent(binNum);
//		fInVector[j] = prob;

//		Int_t binNumEl = fhCumProbSortEl[j]->FindBin(fElossVec[j]);
//		if (binNumEl > fhCumProbSortEl[j]->GetNbinsX())
//			binNumEl = fhCumProbSortEl[j]->GetNbinsX();
//		Double_t probEl = fhCumProbSortEl[j]->GetBinContent(binNumEl);
//		probEl = 0.3*TMath::ErfInverse(2*probEl-1.);
//		if (probEl == 0.) probEl = -1.;
//		fInVector[j] = probEl;
//
		Int_t binNumEl = fhPdfSortEl[j]->FindBin(fElossVec[j]);
		if (binNumEl > fhPdfSortEl[j]->GetNbinsX())
			binNumEl = fhPdfSortEl[j]->GetNbinsX();
		Double_t probEl = fhPdfSortEl[j]->GetBinContent(binNumEl);

		Int_t binNumPi = fhPdfSortPi[j]->FindBin(fElossVec[j]);
		if (binNumPi > fhPdfSortPi[j]->GetNbinsX())
			binNumPi = fhPdfSortPi[j]->GetNbinsX();
		Double_t probPi = fhPdfSortPi[j]->GetBinContent(binNumPi);

		if (TMath::IsNaN(probPi/(probEl+probPi))){
			fInVector[j] =0.;
			//fInVector[j+12] =0.;
		}else{
			fInVector[j] = probPi/(probEl+probPi);
			//fInVector[j+12] = probEl/(probEl+probPi);
		}
		//fInVector[j+12] = probEl/(probEl+probPi);
	}

	//fInVector[fNofInputNeurons-2] = (fElossVec[5] + fElossVec[6])/2.;//GetNofClusters();
	//fInVector[fNofInputNeurons-1] = GetNofClusters();
}

void CbmTrdElectronsTrainAnn::Transform4()
{
	sort(fElossVec.begin(), fElossVec.end());
	Int_t size = fElossVec.size();
	for (Int_t j = 0; j < size; j++) {
//		Int_t binNum = fhCumProbSortPi[j]->FindBin(fElossVec[j]);
//		if (binNum > fhCumProbSortPi[j]->GetNbinsX()){
//			binNum = fhCumProbSortPi[j]->GetNbinsX();
//		}
//		fInVector[j] = fhCumProbSortPi[j]->GetBinContent(binNum);

		Int_t binNumPi = fhCumProbSortPi[j]->FindBin(fElossVec[j]);
		if (binNumPi > fhCumProbSortPi[j]->GetNbinsX()){
			binNumPi = fhCumProbSortPi[j]->GetNbinsX();
		}
	    Double_t probPi = fhCumProbSortPi[j]->GetBinContent(binNumPi);
		probPi = 0.3*TMath::ErfInverse(2*probPi-1.);
		if (probPi == 0.) probPi = 1.;
		fInVector[j] = probPi;
	}

	for (Int_t j = 0; j < size; j++) {
//		Int_t binNum = fhCumProbSortEl[j]->FindBin(fElossVec[j]);
//		if (binNum > fhCumProbSortEl[j]->GetNbinsX())
//			binNum = fhCumProbSortEl[j]->GetNbinsX();
//		fInVector[j+12] = fhCumProbSortEl[j]->GetBinContent(binNum);

		Int_t binNumEl = fhCumProbSortEl[j]->FindBin(fElossVec[j]);
		if (binNumEl > fhCumProbSortEl[j]->GetNbinsX())
			binNumEl = fhCumProbSortEl[j]->GetNbinsX();
		Double_t probEl = fhCumProbSortEl[j]->GetBinContent(binNumEl);
		probEl = 0.3*TMath::ErfInverse(2*probEl-1.);
		if (probEl == 0.) probEl = -1.;
		fInVector[j+12] = probEl;

	}
}

Int_t CbmTrdElectronsTrainAnn::GetNofClusters()
{
	Int_t nofClusters = 0;
	for (Int_t iHit = 0; iHit < fElossVec.size(); iHit++) {
		if ( fElossVec[iHit] > 5.e-6){
			nofClusters++;
		}
	} //iHit
	return nofClusters;
}

Double_t CbmTrdElectronsTrainAnn::Likelihood()
{
	Double_t lPi = 1.;
	Double_t lEl = 1.;
	//cout << "likelihood: "<<endl; ;
    sort(fElossVec.begin(), fElossVec.end());
    for (Int_t j = 0; j < fElossVec.size(); j++) {
		Int_t binNumEl = fhPdfSortEl[j]->FindBin(fElossVec[j]);
		if (binNumEl > fhPdfSortEl[j]->GetNbinsX())
			binNumEl = fhPdfSortEl[j]->GetNbinsX();
		Double_t probEl = fhPdfSortEl[j]->GetBinContent(binNumEl);
		lEl = lEl*probEl;
		//cout << binNumEl << " " << probEl << " " << lEl<< endl;

		Int_t binNumPi = fhPdfSortPi[j]->FindBin(fElossVec[j]);
		if (binNumPi > fhPdfSortPi[j]->GetNbinsX())
			binNumPi = fhPdfSortPi[j]->GetNbinsX();
		Double_t probPi = fhPdfSortPi[j]->GetBinContent(binNumPi);
		lPi = lPi*probPi;
	}
	//cout << lEl << " " <<lPi << " " << lPi/(lEl + lPi)	<<endl;
	return lEl/(lEl + lPi);

}

Double_t CbmTrdElectronsTrainAnn::Eval(Bool_t isEl)
{

	if (fIdMethod == kBDT){
		return fReader->EvaluateMVA("BDT");
	}else if (fIdMethod == kANN){
		Double_t par[fNofInputNeurons];
		for (UInt_t i = 0; i <fInVector.size(); i++) par[i] = fInVector[i];
		return fNN->Evaluate(0, par);
	}else if (fIdMethod == kCLUSTERS){
		Double_t eval = GetNofClusters();
		if (eval > 4.5) return 1.;
		return -1.;
	}else if (fIdMethod == kMEDIANA){
		sort(fElossVec.begin(), fElossVec.end());
		Double_t eval = (fElossVec[5] + fElossVec[6])/2.;
		if (eval > 3.9e-6) return 1.;
		return -1.;

	}else if (fIdMethod == kLIKELIHOOD){
		return Likelihood();

	}
}

void CbmTrdElectronsTrainAnn::ReadFile()
{
	cout << "-I- ReadFile" << endl;
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
	Double_t dEdX, tr, mom;
	Int_t count = 0;
	while (!finPi.eof()) {
		count++;
		if (count%50000 == 0) cout << "-I- read Pion file line number: "<< count << endl;
		vector<Double_t> eloss;
		eloss.resize(12);
		for (Int_t iStation = 0; iStation < 12; iStation++) {
			finPi >> dEdX >> tr >> eloss[iStation];
		}
		finPi >> mom;
		fElossPi.push_back(eloss);
	}
	finPi.close();
	count = 0;
	while (!finEl.eof()) {
		count++;
		if (count%50000 == 0) cout << "-I- read Electron file line number: "<< count << endl;
		vector<Double_t> eloss;
		eloss.resize(12);
		for (Int_t iStation = 0; iStation < 12; iStation++) {
			finEl >> dEdX >> tr >> eloss[iStation];
		}
		finEl >> mom;
		fElossEl.push_back(eloss);
	}
	finEl.close();
//
//	for (Int_t iP = 0; iP < 30; iP++) {
//		for (int i = 0; i < 12; i++){
//			cout << fElossPi[iP][i] << " ";
//		}
//		cout <<endl;
//	}

}

void CbmTrdElectronsTrainAnn::DoTrain()
{
	Int_t nofPi = 0, nofEl = 0;
	fElossVec.clear();
	fElossVec.resize(fNofLayers);

	TTree* simu = CreateTree();

	for (Int_t iP = 0; iP < fElossPi.size() - 2; iP++) {
		for (int i = 0; i < fNofLayers; i++)
			fElossVec[i] = fElossPi[iP][i];
		fXOut = -1.;
		Transform();

		simu->Fill();
		nofPi++;
		if (nofPi >= fMaxNofTrainPi) break;
	}
	for (Int_t iE = 0; iE < fElossEl.size() - 2; iE++){
		for (int i = 0; i < fNofLayers; i++)
			fElossVec[i] = fElossEl[iE][i];
		fXOut = 1.;
		Transform();

		simu->Fill();
		nofEl++;
		if (nofEl >= fMaxNofTrainEl) break;
	}
	if (!fNN) delete fNN;
	TString mlpString = CreateAnnString();
	cout << "-I- create ANN: "<< mlpString << endl;
	cout << "-I- number of training epochs = " << fNofAnnEpochs << endl;
	fNN = new TMultiLayerPerceptron(mlpString,simu,"(Entry$+1)");
	fNN->Train(fNofAnnEpochs, "+text,update=10");
	fNN->DumpWeights((const char*)(fWeightFileDir+"/"+fAnnWeightsFile));
}

void CbmTrdElectronsTrainAnn::DoTrainTmva()
{
	TTree* simu = CreateTree();

	Int_t nofPi = 0, nofEl = 0;
	fElossVec.clear();
	fElossVec.resize(fNofLayers);

	for (Int_t iP = 0; iP < fElossPi.size(); iP++) {
		for (Int_t i = 0; i < fNofLayers; i++){
			fElossVec[i] = fElossPi[iP][i];
			//cout << fElossPi[iP][i]<< " ";
		}
		//cout<< endl;
		fXOut = -1.;
		Transform();

		simu->Fill();
		nofPi++;
		if (nofPi >= fMaxNofTrainPi)break;
	}
	for (Int_t iE = 0; iE < fElossEl.size(); iE++) {
		for (Int_t i = 0; i < fNofLayers; i++)
			fElossVec[i] = fElossEl[iE][i];
		fXOut = 1.;
		Transform();

		simu->Fill();
		nofEl++;
		if (nofEl >= fMaxNofTrainEl)break;
	}

	TMVA::Factory* factory = CreateFactory(simu);
	(TMVA::gConfig().GetIONames()).fWeightFileDir = fWeightFileDir;

	TCut mycuts = "";
	TCut mycutb = "";
	factory->PrepareTrainingAndTestTree(mycuts, mycutb,
			"SplitMode=Random:NormMode=NumEvents:!V");

//	factory->BookMethod(TMVA::Types::kTMlpANN, "TMlpANN","!H:!V:NCycles=50:HiddenLayers=N+1");
	char txt1[150];
	sprintf(txt1, "nTrain_Signal=%d:nTrain_Background=%d:nTest_Signal=0:nTest_Background=0",
			fMaxNofTrainEl-500, fMaxNofTrainPi-500);

	factory->PrepareTrainingAndTestTree("",txt1);
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

	cout << "-I- Start pretesting " << endl;
	if (fIdMethod == kBDT){
		cout << "-I- IdMethod = kBDT " << endl;
	}else if (fIdMethod == kANN){
		cout << "-I- IdMethod = kANN " << endl;
	}else if (fIdMethod == kCLUSTERS){
		cout << "-I- IdMethod = kCLUSTERS " << endl;
	}else if (fIdMethod == kMEDIANA){
		cout << "-I- IdMethod = kMEDIANA " << endl;
	}else if (fIdMethod == kLIKELIHOOD){
		cout << "-I- IdMethod = kLIKELIHOOD " << endl;
	}

	fElossVec.clear();
	fElossVec.resize(fNofLayers);

	if (fIdMethod == kBDT){
		fReader = CreateTmvaReader();
		//reader->BookMVA("TMlpANN", "weights/TMVAnalysis_TMlpANN.weights.txt");
		fReader->BookMVA("BDT", fWeightFileDir+"/TMVAnalysis_BDT.weights.xml");
	} else if (fIdMethod == kANN){
		if (fNN != NULL) delete fNN;
		TTree* simu = CreateTree();
		TString mlpString = CreateAnnString();
		fNN = new TMultiLayerPerceptron(mlpString,simu,"(Entry$+1)");
		fNN->LoadWeights((const char*)(fWeightFileDir+"/"+fAnnWeightsFile));
	}

	Int_t count = 0;
	for (Int_t iE = 0; iE < fElossEl.size(); iE++) {
		count++;
		if (count < fMaxNofTrainEl) continue;//exclude training samples
		if (count%50000 == 0) cout << "-I- read Electron number: "<< count << endl;

		//change electon hit to pion hit
		//int randPi = rand()%fElossPi.size();
	        //for (int i1 = 0; i1 < 5; i1++){
	       //     fElossEl[iE][i1] = fElossPi[randPi][i1];
	       // }
		for (int i = 0; i < fNofLayers; i++){
			if (fSigmaError !=0.)fElossEl[iE][i]+=fRandom->Gaus(0., fSigmaError);
			fElossVec[i] = fElossEl[iE][i];
		}

		Transform();
		FillProbabilityHistos(true);
		Double_t nnEval = Eval(true);
		if (nnEval > fMaxEval)nnEval = fMaxEval - 0.01;
		if (nnEval < fMinEval)nnEval = fMinEval + 0.01;
		fhAnnOutputEl->Fill(nnEval);
	}

	count = 0;
	for (Int_t iP = 0; iP < fElossPi.size(); iP++) {
		count++;
		if (count < fMaxNofTrainPi) continue; //exclude training samples
		if (count%50000 == 0) cout << "-I- read Pion number: "<< count << endl;

		//change pion hit to electron hit
	       // int randEl = rand()%fElossEl.size();
	       // for (int i1 = 0; i1 < 1; i1++){
	       //     fElossPi[iP][i1] = fElossEl[randEl][i1];
	       // }
		for (int i = 0; i < fNofLayers; i++){
			if (fSigmaError !=0.)fElossPi[iP][i]+=fRandom->Gaus(0., fSigmaError);
			fElossVec[i] = fElossPi[iP][i];
		}

		Transform();
		FillProbabilityHistos(false);
		Double_t nnEval = Eval(false);
		if (nnEval > fMaxEval)nnEval = fMaxEval - 0.01;
		if (nnEval < fMinEval)nnEval = fMinEval + 0.01;
		fhAnnOutputPi->Fill(nnEval);
	}
	CreateCumProbHistos();
	CreateROCDiagramm();
}


void CbmTrdElectronsTrainAnn::DoTest()
{
	fElossVec.clear();
	fElossVec.resize(fNofLayers);

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
		fReader->BookMVA("BDT", fWeightFileDir+"/TMVAnalysis_BDT.weights.xml");
	} else if (fIdMethod == kANN){
		if (fNN != NULL) delete fNN;
		TTree* simu = CreateTree();
		TString mlpString = CreateAnnString();
		fNN = new TMultiLayerPerceptron(mlpString,simu,"(Entry$+1)");
		fNN->LoadWeights((const char*)(fWeightFileDir+"/"+fAnnWeightsFile));
	}

	Int_t nofPiLikeEl = 0;
	Int_t nofElLikePi = 0;
	Int_t nofElTest = 0;
	Int_t nofPiTest = 0;

	Int_t count = 0;
	cout << "fMaxNofTrainEl = "  << fMaxNofTrainEl << endl;
	for (Int_t iE = 0; iE < fElossEl.size(); iE++) {
		count++;
		if (count < fMaxNofTrainEl) continue;//exclude training samples
		if (count%50000 == 0) cout << "-I- read Electron number: "<< count<< endl;

		for (int i = 0; i < fNofLayers; i++)
			fElossVec[i] = fElossEl[iE][i];

		Transform();
		Double_t nnEval = Eval(true);
		if (nnEval > fMaxEval)nnEval = fMaxEval - 0.01;
		if (nnEval < fMinEval)nnEval = fMinEval + 0.01;
		//fhAnnOutputEl->Fill(nnEval);
		nofElTest++;
		if (nnEval < fAnnCut)nofElLikePi++;
	}
	count = 0;
	for (Int_t iP = 0; iP < fElossPi.size(); iP++){
		count++;
		if (count < fMaxNofTrainPi) continue;//exclude training samples
		if (count%50000 == 0) cout << "-I- read Pion number: "<< count <<endl;

		for (int i = 0; i < fNofLayers; i++)
			fElossVec[i] = fElossPi[iP][i];
		Transform();
		Double_t nnEval = Eval(false);
		if (nnEval > fMaxEval)nnEval = fMaxEval - 0.01;
		if (nnEval < fMinEval)nnEval = fMinEval + 0.01;
		//fhAnnOutputPi->Fill(nnEval);
		nofPiTest++;
		if (nnEval > fAnnCut)nofPiLikeEl++;
	}

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

	foutResults << "------------------------"<< endl;
	foutResults << fIdMethod << " ";
	foutResults << fTransformType << endl;
	foutResults << fMaxNofTrainPi << " " << fMaxNofTrainEl << endl;
	foutResults << fSigmaError <<endl;
	foutResults << fFileNameTestEl <<endl;
	foutResults << fFileNameTestPi<<endl;
	foutResults << fFileNameCumHistos<<endl;
	foutResults << (Double_t) nofPiTest / nofPiLikeEl<<endl;
	foutResults << (Double_t) nofElLikePi/nofElTest *100. <<endl;
	foutResults << "------------------------"<< endl;

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
	fROCGraph->SetTitle("ROC diagram");
	fROCGraph->GetXaxis()->SetTitle("electron efficiency");
	fROCGraph->GetYaxis()->SetTitle("pion suppression");
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
		reader->AddVariable(txt1, &fInVector[i]);
	}
	return reader;
}

void CbmTrdElectronsTrainAnn::FillProbabilityHistos(Bool_t isEl)
{
	Double_t probEl, probPi;
	sort(fElossVec.begin(), fElossVec.end());
	for (Int_t j = 0; j < fElossVec.size(); j++) {
		Int_t binNumEl = fhCumProbSortEl[j]->FindBin(fElossVec[j]);
		if (binNumEl > fhCumProbSortEl[j]->GetNbinsX())
			binNumEl = fhCumProbSortEl[j]->GetNbinsX();
		probEl = fhCumProbSortEl[j]->GetBinContent(binNumEl);
		probEl = 0.3*TMath::ErfInverse(2*probEl-1.);
		if (probEl == 0.) probEl = -1.;

		Int_t binNumPi = fhCumProbSortPi[j]->FindBin(fElossVec[j]);
		if (binNumPi > fhCumProbSortPi[j]->GetNbinsX()){
			binNumPi = fhCumProbSortPi[j]->GetNbinsX();
		}
	    probPi = fhCumProbSortPi[j]->GetBinContent(binNumPi);
		probPi = 0.3*TMath::ErfInverse(2*probPi-1.);
		if (probPi == 0.) probPi = -1.;
		if (isEl) {
			fhMulProbSortEl[j]->Fill((probPi)*(probEl));
			fhPiProbSortEl[j]->Fill(probPi);
			fhElProbSortEl[j]->Fill(probEl);
		}else {
			fhMulProbSortPi[j]->Fill((probPi)*(probEl));
			fhPiProbSortPi[j]->Fill(probPi);
			fhElProbSortPi[j]->Fill(probEl);
		}
	}

	for (Int_t j = 0; j < fElossVec.size(); j++) {
		if (isEl) {
			fhInputEl[j]->Fill(fInVector[j]);
		}else {
			fhInputPi[j]->Fill(fInVector[j]);
		}
	}
}

void CbmTrdElectronsTrainAnn::DrawHistos()
{
    //SetStyles();

	TCanvas* c1_1 = new TCanvas("classifier output","classifier output", 500,500);
	fhAnnOutputEl->SetLineStyle(2);
	fhAnnOutputEl->SetLineWidth(3);
	fhAnnOutputPi->SetLineWidth(3);
	fhAnnOutputEl->Rebin(10);
	fhAnnOutputPi->Rebin(10);
	fhAnnOutputEl->Draw();
	fhAnnOutputPi->Draw("Same");
	fhAnnOutputEl->Scale(1./fhAnnOutputEl->Integral());
	fhAnnOutputPi->Scale(1./fhAnnOutputPi->Integral());
	fhAnnOutputEl->Rebin(2);
	fhAnnOutputPi->Rebin(2);
	TLine* l1= new TLine(fAnnCut,0,fAnnCut,1);
	l1->SetLineWidth(3);
        l1->SetLineColor(4);
	l1->Draw();
	gPad->SetLogy();
	gPad->SetGridx();
	gPad->SetGridy();

	TCanvas* c1_2 = new TCanvas("cumProb","cumProb", 500,500);
	fhCumProbPi->SetLineWidth(3);
	fhCumProbPi->Draw();
	fhCumProbEl->SetLineStyle(2);
	fhCumProbEl->SetLineWidth(3);
	fhCumProbEl->Draw("same");
        TLine* l2= new TLine(fAnnCut,0,fAnnCut,1);
	l2->SetLineWidth(3);
        l2->SetLineColor(4);
	l2->Draw();
       // gPad->SetLogy();
	gPad->SetGridx();
	gPad->SetGridy();

	TCanvas* c1_3 = new TCanvas("ROC","ROC", 500,500);

       // DrawGraph(fROCGraph, "electron efficiency,%", "pion suppression",
       // 		kBlack, LIT_LINE_WIDTH, LIT_LINE_STYLE1, LIT_MARKER_SIZE,
	// 	  LIT_MARKER_STYLE1, false, true, "ACP");
	gPad->SetLogy();
	fROCGraph->GetXaxis()->SetTitle("electron efficiency,%");
	fROCGraph->GetYaxis()->SetTitle("pion suppression");
	fROCGraph->SetLineWidth(3);
        fROCGraph->Draw("AL");
	//TLine* l3= new TLine(90,10e10,90,10e10);
       // l3->SetLineWidth(3);
       // l3->SetLineColor(4);
       // l3->Draw();

	gPad->SetGridx();
	gPad->SetGridy();

	return;

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

	TCanvas* c6 = new TCanvas("c6", "c6", 10, 10, 800, 800);
	c6->Divide(4, 3);
	for (int i = 0; i < 12; i++) {
		c6->cd(i + 1);
		fhElProbSortPi[i]->SetLineWidth(3);
		fhElProbSortPi[i]->SetStats(false);
		fhElProbSortPi[i]->SetMinimum(1);
		fhElProbSortPi[i]->SetMaximum(1.1*
				TMath::Max(fhElProbSortPi[i]->GetMaximum(), fhElProbSortEl[i]->GetMaximum()));
		fhElProbSortPi[i]->Draw();

		fhElProbSortEl[i]->SetLineWidth(3);
		fhElProbSortEl[i]->SetLineStyle(2);
		fhElProbSortEl[i]->SetStats(false);
		fhElProbSortEl[i]->SetMinimum(1);
		fhElProbSortEl[i]->Draw("same");

//		TLegend* leg1 = new TLegend(0.5, 0.15, 0.99, 0.4);
//		leg1->AddEntry(fhElossDiffEl[i], "electrons");
//		leg1->AddEntry(fhElossDiffPi[i], "pions");
//		leg1->DrawClone();

		gPad->SetLogy(true);
		gPad->SetGridy(true);
		gPad->SetGridx(true);
	}

	TCanvas* c7 = new TCanvas("c7", "c7", 10, 10, 800, 800);
	c7->Divide(4, 3);
	for (int i = 0; i < 12; i++) {
		c7->cd(i + 1);
		fhPiProbSortPi[i]->SetLineWidth(3);
		fhPiProbSortPi[i]->SetStats(false);
		fhPiProbSortPi[i]->SetMinimum(1);
		fhPiProbSortPi[i]->SetMaximum(1.1*
				TMath::Max(fhPiProbSortPi[i]->GetMaximum(), fhPiProbSortEl[i]->GetMaximum()));
		fhPiProbSortPi[i]->Draw();

		fhPiProbSortEl[i]->SetLineWidth(3);
		fhPiProbSortEl[i]->SetLineStyle(2);
		fhPiProbSortEl[i]->SetStats(false);
		fhPiProbSortEl[i]->SetMinimum(1);
		fhPiProbSortEl[i]->Draw("same");

//		TLegend* leg1 = new TLegend(0.5, 0.15, 0.99, 0.4);
//		leg1->AddEntry(fhElossDiffEl[i], "electrons");
//		leg1->AddEntry(fhElossDiffPi[i], "pions");
//		leg1->DrawClone();

		gPad->SetLogy(true);
		gPad->SetGridy(true);
		gPad->SetGridx(true);
	}

	TCanvas* c8 = new TCanvas("c8", "c8", 10, 10, 800, 800);
	c8->Divide(4, 3);
	for (int i = 0; i < 12; i++) {
		c8->cd(i + 1);
		fhMulProbSortPi[i]->SetLineWidth(3);
		fhMulProbSortPi[i]->SetStats(false);
		fhMulProbSortPi[i]->SetMinimum(1);
		fhMulProbSortPi[i]->SetMaximum(1.1*
				TMath::Max(fhMulProbSortPi[i]->GetMaximum(), fhMulProbSortEl[i]->GetMaximum()));
		fhMulProbSortPi[i]->Draw();

		fhMulProbSortEl[i]->SetLineWidth(3);
		fhMulProbSortEl[i]->SetLineStyle(2);
		fhMulProbSortEl[i]->SetStats(false);
		fhMulProbSortEl[i]->SetMinimum(1);
		fhMulProbSortEl[i]->Draw("same");

//		TLegend* leg1 = new TLegend(0.5, 0.15, 0.99, 0.4);
//		leg1->AddEntry(fhElossDiffEl[i], "electrons");
//		leg1->AddEntry(fhElossDiffPi[i], "pions");
//		leg1->DrawClone();

		gPad->SetLogy(true);
		gPad->SetGridy(true);
		gPad->SetGridx(true);
	}

	TCanvas* c9 = new TCanvas("c9", "c9", 10, 10, 800, 800);
	c9->Divide(4, 3);
	for (int i = 0; i < 12; i++) {
		c9->cd(i + 1);
		fhInputPi[i]->SetLineWidth(3);
		fhInputPi[i]->SetStats(false);
		fhInputPi[i]->SetMinimum(1);
		fhInputPi[i]->SetMaximum(1.1*
				TMath::Max(fhInputPi[i]->GetMaximum(), fhInputEl[i]->GetMaximum()));
		fhInputPi[i]->Draw();

		fhInputEl[i]->SetLineWidth(3);
		fhInputEl[i]->SetLineStyle(2);
		fhInputEl[i]->SetStats(false);
		fhInputEl[i]->SetMinimum(1);
		fhInputEl[i]->Draw("same");

		gPad->SetLogy(true);
		gPad->SetGridy(true);
		gPad->SetGridx(true);
	}


	TCanvas* c10 = new TCanvas("c10", "c10", 10, 10, 800, 800);
	c10->Divide(4, 3);
	for (int i = 0; i < 12; i++) {
		c10->cd(i + 1);
		fhPdfSortPi[i]->SetLineWidth(3);
		fhPdfSortPi[i]->SetStats(false);
		fhPdfSortPi[i]->SetMinimum(0.001);
		fhPdfSortPi[i]->SetMaximum(1.1*
				TMath::Max(fhPdfSortPi[i]->GetMaximum(), fhPdfSortPi[i]->GetMaximum()));
		fhPdfSortPi[i]->Draw();

		fhPdfSortPi[i]->SetLineWidth(3);
		fhPdfSortPi[i]->SetLineStyle(2);
		fhPdfSortPi[i]->SetStats(false);
		fhPdfSortPi[i]->SetMinimum(0.001);
		fhPdfSortPi[i]->Draw("same");

		gPad->SetLogy(true);
		gPad->SetGridy(true);
		gPad->SetGridx(true);
	}
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
