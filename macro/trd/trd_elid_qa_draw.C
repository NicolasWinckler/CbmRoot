
Double_t fitFunction(Double_t *x, Double_t *par)
{
	Double_t xx = x[0];
	Double_t c = ( (par[0]/( sqrt(2*TMath::Pi()) ) * par[1] * xx ) );
	Double_t c1 = TMath::Exp( (-1. / (2. * par[1] * par[1])) * pow(log(xx) - par[2], 2) );
	return c * c1;
}

void trd_elid_qa_draw()
{
	TFile* file = new TFile("/d/cbm02/slebedev/trd/JUN09/ST/piel.0009.electronsQa.root");


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

	fhNofTrdHitsEl->SetLineWidth(3);
    fhNofTrdHitsPi->SetLineWidth(3);
    fhNofTrdHitsEl->SetLineStyle(2);
	char histName[50];
	TH1D* fhPidANNEl[7];
	TH1D* fhPidANNPi[7];
    for (Int_t i = 0; i < 7; i++){
    	sprintf(histName, "fhPidANNEl%d", i);
    	fhPidANNEl[i] = (TH1D*)file->Get(histName);
    	sprintf(histName, "fhPidANNPi%d", i);
    	fhPidANNPi[i] = (TH1D*)file->Get(histName);

		fhPidANNEl[i]->SetLineWidth(3);
		fhPidANNPi[i]->SetLineWidth(3);
		fhPidANNEl[i]->SetLineStyle(2);
    }

    c2 = new TCanvas("c2","c2",10,10,800,800);
    c2->Divide(3,3);
    c2->cd(1);
    fhNofTrdHitsEl->Draw();
    fhNofTrdHitsPi->Draw("same");
	gPad->SetLogy(true);
	char legTxt1[100];

    for (Int_t i = 0; i < 7; i++){
    	c2->cd(i+2);

    	fhPidANNEl[i]->Draw();
    	fhPidANNPi[i]->Draw("same");
    	gPad->SetLogy(true);

		TLegend* leg1 = new TLegend(0.3,0.6, 0.7, 0.85, "mean value");
		sprintf(legTxt1,"electrons (%.2f)", fhPidANNEl[i]->GetMean());
		leg1->AddEntry(fhPidANNEl[i], legTxt1);
		sprintf(legTxt1,"pions (%.2f)", fhPidANNPi[i]->GetMean());
		leg1->AddEntry(fhPidANNPi[i], legTxt1);
		leg1->DrawClone();
    }

	TCanvas* c3 = new TCanvas("c3","c3",10,10,800,800);
	c3->Divide(4, 3);
	TH1D *fhElossSortPi[12];
	TH1D *fhElossSortEl[12];

	for (int i = 0; i < 12; i++){
		c3->cd(i+1);
		sprintf(histName,"fhElossSortPi%d",i);
		fhElossSortPi[i] = (TH1D*)file->Get(histName);
		fhElossSortPi[i]->SetLineWidth(3);
		fhElossSortPi[i]->SetStats(false);
		fhElossSortPi[i]->Draw();

		sprintf(histName,"fhElossSortEl%d",i);
		fhElossSortEl[i] = (TH1D*)file->Get(histName);
		fhElossSortEl[i]->SetLineWidth(3);
		fhElossSortEl[i]->SetLineStyle(2);
		fhElossSortEl[i]->SetStats(false);
		fhElossSortEl[i]->Draw("same");

		TLegend* leg1 = new TLegend(0.4,0.7, 0.99, 0.99, "mean value");
		sprintf(legTxt1,"electrons (%.1e)", fhElossSortEl[i]->GetMean());
		leg1->AddEntry(fhElossSortEl[i], legTxt1);
		sprintf(legTxt1,"pions (%.1e)",fhElossSortPi[i]->GetMean());
		leg1->AddEntry(fhElossSortPi[i], legTxt1);
		leg1->DrawClone();

    	gPad->SetLogy(true);
        //gPad->SetGridy(true);
        gPad->SetGridx(true);
	}


	TCanvas* c4 = new TCanvas("c4","c4",10,10,800,800);
	c4->Divide(4, 3);
	TH1D *fhCumProbSortPi[12];
	TH1D *fhCumProbSortEl[12];

	for (int i = 0; i < 12; i++){
		c4->cd(i+1);
		sprintf(histName,"fhCumProbSortPi%d",i);
		fhElossSortPi[i] = (TH1D*)file->Get(histName);
		fhElossSortPi[i]->SetLineWidth(3);
		fhElossSortPi[i]->SetStats(false);
		fhElossSortPi[i]->Draw();

		sprintf(histName,"fhCumProbSortEl%d",i);
		fhElossSortEl[i] = (TH1D*)file->Get(histName);
		fhElossSortEl[i]->SetLineWidth(3);
		fhElossSortEl[i]->SetLineStyle(2);
		fhElossSortEl[i]->SetStats(false);
		fhElossSortEl[i]->Draw("same");

		TLegend* leg1 = new TLegend(0.5,0.15, 0.99, 0.4);
		leg1->AddEntry(fhElossSortEl[i], "electrons");
		leg1->AddEntry(fhElossSortPi[i], "pions");
		leg1->DrawClone();

    	//gPad->SetLogy(true);
        //gPad->SetGridy(true);
        gPad->SetGridx(true);
	}
}
