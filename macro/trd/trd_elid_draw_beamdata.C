void calculateRMS(Float_t* eloss_exp, Float_t* yield, TH1D* fhElELoss)
{
	Double_t rms = 0.;
	for (Int_t i = 0; i < 100; i++) {
		if (eloss_exp[i] < 5.) continue;
		Int_t binNum  = fhElELoss->FindBin(eloss_exp[i]/10e5);
		Double_t binCont = fhElELoss->GetBinContent(binNum);
		Double_t d = yield[i] - binCont;
		rms += d*d;
	}
	rms = sqrt(rms/100.);
	cout<<endl << "rms = " << rms << endl;
}
void trd_elid_draw_beamdata(Int_t trdNFoils = 0, Float_t trdDFoils = 0.0, Float_t trdDGap = 0.0)
{
	Bool_t doSave = true;

	TString outFile = "/d/cbm02/slebedev/trd/JUL09/simtest/piel.0000.reco.root";
//	TString dir = "/d/cbm02/slebedev/trd/JUL09/tr_params/";
//	char txt[200];
//	sprintf(txt,"_%f_%f_%f", trdNFoils, trdDFoils, trdDGap);
//	TString outFile = dir + "piel.0001.reco."+txt+".root";
//	TString fileNamePi = dir+ "pi"+ txt +".gif";
//	TString fileNameEl = dir+"el"+ txt +".gif";

	FILE *occfile = fopen("q-p1.5.dat", "r");
	char xline[256];
	Float_t eloss_exp[100], yield_pim[100], err_yield_pim[100], yield_em[100],
			err_yield_em[100], err_eloss_exp[100];
	Float_t yield_sum = 0;
	Double_t maxExpEl = 0., maxExpPi = 0.;
	if (occfile != 0) {
		for (Int_t i = 0; i < 100; i++) {
			fscanf(occfile, "%f %f %f %f %f", &eloss_exp[i], &yield_pim[i],
					&err_yield_pim[i], &yield_em[i], &err_yield_em[i]);
//			printf("%f %f %f %f %f\n", eloss_exp[i], yield_pim[i],
//					err_yield_pim[i], yield_em[i], err_yield_em[i]);
			err_eloss_exp[i] = 0;
			if (yield_em[i] > maxExpEl)	maxExpEl = yield_em[i];
			if (yield_pim[i] > maxExpPi)maxExpPi = yield_pim[i];
			yield_sum += yield_em[i];
		}
	} else {
		printf("error: cannot open occupancy file\n");
	}

	printf("yield exp = %f\n", yield_sum);

	TGraphErrors *pim_yield = new TGraphErrors(100, eloss_exp, yield_pim,err_eloss_exp, err_yield_pim);
	TGraphErrors *em_yield = new TGraphErrors(100, eloss_exp, yield_em,	err_eloss_exp, err_yield_em);
	pim_yield->SetLineWidth(2);
	em_yield->SetLineWidth(2);

	TFile *simFileHist = new TFile(	outFile);
	fhElELoss->Scale(maxExpEl / fhElELoss->GetMaximum());
	fhPiELoss->Scale(maxExpPi / fhPiELoss->GetMaximum());
	Float_t eloss_sim[100], yield_sim_el[100], yield_sim_pi[100], err_eloss_sim[100],
			err_yield_sim_el[100], err_yield_sim_pi[100];
	yield_sum = 0;
	for (int i = 0; i < 100; i++) {
		yield_sim_el[i] = fhElELoss->GetBinContent(i);
		yield_sim_pi[i] = fhPiELoss->GetBinContent(i);
		eloss_sim[i] = fhElELoss->GetBinCenter(i) * 10e5;
		err_eloss_sim[i] = 0;
		err_yield_sim_el[i] = 0.;//fhElELoss->GetBinError(i);
		err_yield_sim_pi[i] = 0.;//fhPiELoss->GetBinError(i);
		yield_sum += yield_sim_el[i];
	}
	cout << "yield sim = " << yield_sum << endl;

	TGraphErrors *graphElSim = new TGraphErrors(100, eloss_sim, yield_sim_el,err_eloss_sim, err_yield_sim_el);
	graphElSim->SetLineColor(kRed);
	graphElSim->SetLineWidth(2);

	TGraphErrors *graphPiSim = new TGraphErrors(100, eloss_sim, yield_sim_pi,err_eloss_sim, err_yield_sim_pi);
	graphPiSim->SetLineColor(kBlue);
	graphPiSim->SetLineWidth(2);

	calculateRMS(eloss_exp, yield_em, fhElELoss);

	TCanvas* c1 = new TCanvas("c1", "c1");
	em_yield->Draw("AL*");
	graphElSim->Draw("L");
	gPad->SetGridx(true);
	gPad->SetGridy(true);
	if (fileNamePi == "" || fileNameEl == ""){
		c1->SaveAs(c1->GetName());
	}else {
		c1->SaveAs(fileNameEl);
	}

	TCanvas* c2 = new TCanvas("c2", "c2");
	pim_yield->Draw("AL*");
	graphPiSim->Draw("L");
	gPad->SetGridx(true);
	gPad->SetGridy(true);
	if(doSave){
		if (fileNamePi == "" || fileNameEl == ""){
			c2->SaveAs(c2->GetName());
		}else {
			c2->SaveAs(fileNamePi);
		}
	}
}
