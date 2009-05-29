
Double_t fitFunction(Double_t *x, Double_t *par)
{
	Double_t xx = x[0];
	Double_t c = ( (par[0]/( sqrt(2*TMath::Pi()) ) * par[1] * xx ) );
	Double_t c1 = TMath::Exp( (-1. / (2. * par[1] * par[1])) * pow(log(xx) - par[2], 2) );
	return c * c1;
}

void trd_elid_qa_draw()
{
	TFile* file = new TFile("/d/cbm02/slebedev/trd/JUN09/MB/piel.0000.electronsQa.root");


    gStyle->SetPalette(1,0);
    gStyle->SetHistLineWidth(3);

    fhPiELossSum->SetLineWidth(3);
    fhElELossSum->SetLineWidth(3);
    fhPiELoss->SetLineWidth(3);
    fhElNofZeroTR->SetLineWidth(3);
    fhElTR->SetLineWidth(3);
    fhEldEdX->SetLineWidth(3);
    fhElELoss->SetLineWidth(3);

    fhElELossSum->SetLineStyle(2);
    fhElNofZeroTR->SetLineStyle(2);
    fhElTR->SetLineStyle(2);
    fhEldEdX->SetLineStyle(2);
    fhElELoss->SetLineStyle(2);

    c1 = new TCanvas("c1","c1",10,10,800,800);
    c1->Divide(2,3);
    c1->cd(1);
    fhPiELossSum->Draw();
    fhElELossSum->Draw("same");

    c1->cd(2);
    fhPiELoss->Draw();
    double maxX = 50, minX = 0;
	TF1 *fitFcn = new TF1("fitLogNorm",fitFunction, minX, maxX, 3);
	fitFcn->SetParameter(1, 1000.);
	fitFcn->SetParameter(2, 0.7);
	fhPiELoss->Fit("fitLogNorm","","",minX, maxX);
	//fhPiELoss->Fit("landau");
	gPad->SetGridx(true);
	gPad->SetGridy(true);

	double par[3];
	par[0] = fitFcn->GetParameter(0);
	par[1] = fitFcn->GetParameter(1);
	par[2] = fitFcn->GetParameter(2);

	int nofPoints = 5000;
	double curX = 0;
	double dx = maxX/nofPoints;
	double maximum = 0.;//fitFcn->GetMaximum();
	double maximumX = 0.;//fitFcn->GetMaximumX();
	for (int i = 0; i < nofPoints; i++){
		double y = fitFunction(&curX, par);
		if (y > maximum){
			maximum = y;
			maximumX = curX;
		}
		curX+=dx;
	}

	double halfMaxF = maximum/ 2.;
	double x0 = -1;
	double x1 = -1;
	curX = 0.;
	for (int i = 0; i < nofPoints; i++){
		double y = fitFunction(&curX, par);
		if (y > halfMaxF && x0 == -1) x0 = curX;
		if (y < halfMaxF && x0 != -1){
			x1 = curX;
			break;
		}
		curX+=dx;
	}
	cout << "Maximum = " << maximum <<"  MaximumX = " <<maximumX<< endl;
	cout << "x0 = " << x0 <<"  x1=" << x1 <<"  FWHM = " << x1 - x0 << "  FWHM/4.02 = "<< (x1 - x0)/4.02 << endl;

    c1->cd(3);
    fhElNofZeroTR->Draw();

    c1->cd(4);
    fhElTR->Draw();

    c1->cd(5);
    fhEldEdX->Draw();

    c1->cd(6);
    fhElELoss->Draw();

}
