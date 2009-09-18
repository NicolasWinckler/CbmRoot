/*  Description: This macro train the ANN for electron
 identification in the TRD.

 Author : Semen Lebedev
 E-mail : S.Lebedev@gsi.de
 */
#include <vector>

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

void trd_elid_trainANN_txt()
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
	  gSystem->Load("libTrd");
	  // ------------------------------------------------------------------------

	CbmTrdElectronsTrainAnn* trainer = new CbmTrdElectronsTrainAnn();
	trainer->Run();


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
	fhCumProbPi->SetLineWidth(3.);
	fhCumProbPi->Draw();
	fhCumProbEl->SetLineStyle(2);
	fhCumProbEl->SetLineWidth(3.);
	fhCumProbEl->Draw("same");
	gPad->SetGridx();
	gPad->SetGridy();

}
