/*  Description: This macro train the ANN for electron
 identification in the TRD.
 Input: txt file produced by trd_elid_make_txt.C macro

 Author : Simeon Lebedev
 E-mail : S.Lebedev@gsi.de
 */
#include <vector>

	Int_t fNofLayers = 12;
	TString mlpString = "x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12:12:x13";
	TString mlpWeightsFile = "Neural_Net_Weights.txt";
	///vector to store and transform data from file
	std::vector<Float_t> inVector;
	Float_t x13;///output value
	Double_t ANNCoef1 = 1.06;
	Double_t ANNCoef2 = 0.57;
	Bool_t doTraining = true;
	//cut on the output value of ANN
	Double_t ANNCut = 0.98;

void coeffCalc(double mom, double* coeff1, double* coeff2) {
	double momAr[] = { 1., 1.5, 2., 3., 4., 5., 7., 9., 11., 13. };
	double coeffAr1[] = { 1.04, 1.105, 1.154, 1.277, 1.333, 1.394, 1.47, 1.50,
			1.54, 1.58 };
	double coeffAr2[] = { 0.548, 0.567, 0.585, 0.63, 0.645, 0.664, 0.69, 0.705,
			0.716, 0.723 };
	for (int i = 0; i <10; i++) {
		if (mom < momAr[i] + 0.01 && mom > momAr[0] - 0.01) {
			*coeff1 = coeffAr1[i];
			*coeff2 = coeffAr2[i];
			break;
		}
	}
}

void transform1()
{
	for (Int_t j = 0; j<inVector.size(); j++) {
		inVector[j]=inVector[j]*1e6;
		inVector[j]=(inVector[j]-ANNCoef1)/ANNCoef2 -0.225;
	}
	sort(inVector.begin(), inVector.end());
	for (Int_t j = 0; j<inVector.size(); j++)
		inVector[j]=TMath::LandauI(inVector[j]);
}

void trd_elid_trainANN_txt() {
	///load libraries for neural net
	if (!gROOT->GetClass("TMultiLayerPerceptron"))
		gSystem->Load("libMLP");

	TString fileNum = "0002";
	TString geoType = "st";
	TString inputDir = "/d/cbm02/slebedev/trd/JUN09/"+geoType+"/";
	TString fileNameEl = inputDir + geoType+"_"+"electrons_mom_"+fileNum+".txt";
	TString fileNamePi = inputDir + geoType+"_"+"pions_mom_"+fileNum+".txt";
	TString fileNameTestEl = inputDir + geoType+"_"+"electrons_mom_"+fileNum+".txt";
	TString fileNameTestPi = inputDir + geoType+"_"+"pions_mom_"+fileNum+".txt";

//	TString fileNameEl = "/d/cbm02/slebedev/trd/MAY08/electrons_mom_8.txt";
//	TString fileNamePi = "/d/cbm02/slebedev/trd/MAY08/pions_mom_8.txt";
//	TString fileNameTestEl = "/d/cbm02/slebedev/trd/MAY08/electrons_mom_8.txt";
//	TString fileNameTestPi = "/d/cbm02/slebedev/trd/MAY08/pions_mom_8.txt";

	//input files for electrons and pions
	std::ifstream finEl((const char*) fileNameEl);
	std::ifstream finPi((const char*) fileNamePi);

	inVector.resize(fNofLayers);///12 input elements

	///init TTree as input data to neural net
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

	simu->Branch("x13", &x13, "x13/F");

	Int_t NofPi = 0, NofEl = 0;
	//Max number of electrons and pions for training samples
	Int_t MaxNofPi = 3000, MaxNofEl = 3000;
	Double_t dEdX, tr, mom;
	std::vector<Float_t> inVectorTemp;
	inVectorTemp.resize(12);

	if (doTraining){

		while ( !finPi.eof() ) {
			//cout << "--";
			for (Int_t iStation = 0; iStation < 12; iStation++) {
				finPi >> dEdX >> tr >> inVectorTemp[iStation];
				//finPi >> inVector[iStation];
				//cout <<inVector[iStation] << " ";
			}
			for (int i = 0; i < fNofLayers; i++)
				inVector[i] = inVectorTemp[i];
			finPi >> mom;
			x13 = -1.;
			// coeffCalc(mom, &ANNCoef1, &ANNCoef2);
			transform1();

			simu->Fill();
			NofPi++;
			if (NofPi >= MaxNofPi) {
				break;
			}
		}

		while ( !finEl.eof() ) {
			for (Int_t iStation = 0; iStation < 12; iStation++) {
				finEl >> dEdX >> tr >> inVectorTemp[iStation];
				//finEl >> inVector[iStation];
			}
			for (int i = 0; i < fNofLayers; i++)
				inVector[i] = inVectorTemp[i];
			finEl >> mom;

			x13 = 1.;
			// coeffCalc(mom, &ANNCoef1, &ANNCoef2);
			transform1();

			simu->Fill();
			NofEl++;
			if (NofEl >= MaxNofEl) {
				break;
			}
		}
	}

	TMultiLayerPerceptron* fNN = new TMultiLayerPerceptron(mlpString,simu,"(Entry$+1)");
	if (doTraining){
		fNN->Train(250, "+text,update=1");
		fNN->DumpWeights((const char*)mlpWeightsFile);
	}else{
		fNN->LoadWeights((const char*)mlpWeightsFile);
	}

	/////////////////////////////////////////////////////////////////////////
	//////////////////////////TEST///////////////////////////////////////////
	cout << "TESTING......." << endl;
	Double_t params[12];
	Double_t maxEval = 1.3;
	Double_t minEval = -1.3;

	TH1F* hANNOutputPi = new TH1F("hANNOutputPi","ANN output;ANN output;Entries",100, minEval, maxEval);
	TH1F* hANNOutputEl = new TH1F("hANNOutputEl","ANN optput;ANN output;Entries",100, minEval, maxEval);
	TH1F* hCumProbPi = new TH1F("hCumProbPi","ANN output;ANN output;Cumulative probability",100, minEval, maxEval);
	TH1F* hCumProbEl = new TH1F("hCumProbEl","ANN optput;ANN output;Cumulative probability",100, minEval, maxEval);
	TH1F* hANNErrorPi =	new TH1F("hANNErrorPi","Error = ANN output - true;Error;Entries",100, -0.5,0.5);
	TH1F* hANNErrorEl =	new TH1F("hANNErrorEl","Error = ANN output - true;Error;Entries",100, -0.5,0.5);

	//input data for testing
	std::ifstream finPiTest((const char*) fileNameTestPi);
	std::ifstream finElTest((const char*) fileNameTestEl);

	Int_t NofPiLikeEl = 0;
	Int_t NofElLikePi = 0;
	Int_t NofElTest = 0;
	Int_t NofPiTest = 0;

	while ( !finElTest.eof() ) {
		for (Int_t iStation = 0; iStation < 12; iStation++) {
			finElTest >> dEdX >> tr >> inVectorTemp[iStation];
			//finElTest >> inVector[iStation];
		}
		for (int i = 0; i < fNofLayers; i++)
			inVector[i] = inVectorTemp[i];
		finElTest >> mom;
		//coeffCalc(mom, &ANNCoef1, &ANNCoef2);
		transform1();

		for (UInt_t k = 0; k < inVector.size(); k++)
			params[k] = inVector[k];
		Double_t nnEval = fNN->Evaluate(0, params);
		if (nnEval > maxEval)
			nnEval = maxEval - 0.01;
		if (nnEval < minEval)
			nnEval = minEval + 0.01;
		hANNOutputEl->Fill(nnEval);
		hANNErrorEl->Fill(1. - nnEval);
		NofElTest++;
		if (nnEval < ANNCut)
			NofElLikePi++;
	}

	while ( !finPiTest.eof() ) {
		for (Int_t iStation = 0; iStation < 12; iStation++) {
			finPiTest >> dEdX >> tr >> inVectorTemp[iStation];
			// finPiTest >> inVector[iStation];
		}
		for (int i = 0; i < fNofLayers; i++)
			inVector[i] = inVectorTemp[i];
		finPiTest >> mom;

		//coeffCalc(mom, &ANNCoef1, &ANNCoef2);
		transform1();

		for (UInt_t k = 0; k < inVector.size(); k++)
			params[k] = inVector[k];
		Double_t nnEval = fNN->Evaluate(0, params);
		if (nnEval > maxEval)
			nnEval = maxEval - 0.01;
		if (nnEval < minEval)
			nnEval = minEval + 0.01;
		hANNOutputPi->Fill(nnEval);
		hANNErrorPi->Fill(-1. - nnEval);
		NofPiTest++;
		if (nnEval > ANNCut)
			NofPiLikeEl++;
	}

	cout <<"NofEl = " <<NofElTest<<endl;
	cout <<"NofPi = " <<NofPiTest<<endl;
	cout <<"Pi like El = " <<NofPiLikeEl<<endl;
	cout <<"El like Pi = " <<NofElLikePi<<endl;
	cout << "Pion supression = "<< NofPiTest<< "/"<< NofPiLikeEl<< " = "
			<< (double) NofPiTest / NofPiLikeEl << endl;
	cout << "Electron efficiency loss in % = " << NofElLikePi << "/"
			<< NofElTest << " = " << (double) NofElLikePi/NofElTest *100
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
		hCumProbPi->SetBinContent(i, (Double_t)cumProbPi/nofPi);

		cumProbEl += hANNOutputEl->GetBinContent(i);
		hCumProbEl->SetBinContent(i, 1. - (Double_t)cumProbEl / nofEl);
	}

	Double_t optimalCut = -1;
	for (Int_t i = 1; i <= hCumProbEl->GetNbinsX(); i++) {
		// cout <<i << " " << hCumProbEl->GetBinContent(i) << " " << hCumProbEl->GetBinCenter(i)<< endl;

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
