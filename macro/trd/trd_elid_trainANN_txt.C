/*  Description: This macro train the ANN for electron
 identification in the TRD.

 Author : Semen Lebedev
 E-mail : S.Lebedev@gsi.de
 */
#include <vector>
//#include "../../trd/CbmTrdElectronsTrainAnn.h"

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

void trd_elid_trainANN_txt(Int_t paramNum = 1, Int_t fileNum = 0)
{
	///load libraries for neural net
	if (!gROOT->GetClass("TMultiLayerPerceptron"))
		gSystem->Load("libMLP");
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gSystem->Load("libGeoBase");
	gSystem->Load("libParBase");
	gSystem->Load("libBase");
	gSystem->Load("libCbmBase");
	gSystem->Load("libCbmData");
	gSystem->Load("libField");
	gSystem->Load("libGen");
	gSystem->Load("libPassive");
	gSystem->Load("libTMVA");
	gSystem->Load("libTrd");
//
//	paramNum = 1;
//	fileNum = 2;

	TString fileNumSt;
	if (fileNum == 0)fileNumSt = "0000";
	if (fileNum == 1)fileNumSt = "0001";
	if (fileNum == 2)fileNumSt = "0002";
	if (fileNum == 3)fileNumSt = "0003";
	if (fileNum == 4)fileNumSt = "0004";
	if (fileNum == 5)fileNumSt = "0005";
	if (fileNum == 6)fileNumSt = "0006";
	if (fileNum == 7)fileNumSt = "0007";
	if (fileNum == 8)fileNumSt = "0008";
	if (fileNum == 9)fileNumSt = "0009";

	TString paramSt;
	if (paramNum == 1)paramSt = "param1";
	if (paramNum == 2)paramSt = "param2";
	if (paramNum == 3)paramSt = "param3";

	cout << "-I- Current file number is " << fileNumSt << endl;
	TString geoType = "st";
	TString inputDir = "/d/cbm06/user/slebedev/trd/JUL09/reco/"+paramSt+"/st/";
	fileNameEl = inputDir + geoType + "_" + "electrons_mom_" + fileNumSt	+ ".txt";
	fileNamePi = inputDir + geoType + "_" + "pions_mom_" + fileNumSt + ".txt";
	fileNameTestEl = inputDir + geoType + "_" + "electrons_mom_" + fileNumSt	+ ".txt";
	fileNameTestPi = inputDir + geoType + "_" + "pions_mom_" + fileNumSt	+ ".txt";
	fileNameCumHistos ="/d/cbm06/user/slebedev/trd/JUL09/reco/"+paramSt+"/st/piel."+fileNumSt+".reco.root";
	weightFileDir="weights/"+paramSt+"_"+fileNumSt;

	CbmTrdElectronsTrainAnn* trainer = new CbmTrdElectronsTrainAnn();
	trainer->SetFileNameEl(fileNameEl);
	trainer->SetFileNamePi(fileNamePi);
	trainer->SetFileNameTestEl(fileNameTestEl);
	trainer->SetFileNameTestPi(fileNameTestPi);
	trainer->SetFileNameCumHistos(fileNameCumHistos);
	trainer->SetWeightFileDir(weightFileDir);
	trainer->SetNofHiddenNeurons(12);
	trainer->SetNofAnnEpochs(250);
	trainer->SetMaxNofTrainPi(3000);
	trainer->SetMaxNofTrainEl(3000);

	//Cut will be calculated automatically, taken into account 90% of electron efficiency
	trainer->SetIsDoTrain(true);
	trainer->SetTransformType(3);
	//kANN = 0, kBDT = 1, kCLUSTERS = 2, kMEDIANA = 3, kLIKELIHOOD = 4
	trainer->SetIdMethod(1);
	trainer->Run();
	//trainer->DrawHistos();

}
