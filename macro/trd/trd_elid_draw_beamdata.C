void trd_elid_draw_beamdata()
{

	FILE *occfile = fopen("q-p1.5.dat","r");
	  char  xline[256];
	  Float_t eloss_exp[100], yield_pim[100], err_yield_pim[100], yield_em[100], err_yield_em[100],err_eloss_exp[100];
	  Float_t yield_sum=0;
	  Double_t maxExpEl = 0.;
	  if(occfile!=0){
	    for (Int_t i=0; i<100;i++){
	      fscanf (occfile, "%f %f %f %f %f",&eloss_exp[i],&yield_pim[i],&err_yield_pim[i],&yield_em[i],&err_yield_em[i]);
	      printf( "%f %f %f %f %f\n",eloss_exp[i],yield_pim[i],err_yield_pim[i],yield_em[i],err_yield_em[i]);
	      err_eloss_exp[i]=0;
	      if (yield_em[i] > maxExpEl) maxExpEl = yield_em[i];
	      yield_sum += yield_em[i];
	    }
	  }
	  else{
	    printf("error: cannot open occupancy file\n");
	  }

	    printf("yield:%f\n",yield_sum);

	  TGraphErrors *pim_yield = new TGraphErrors(100,eloss_exp,yield_pim,err_eloss_exp,err_yield_pim);
	  TGraphErrors *em_yield = new TGraphErrors(100,eloss_exp,yield_em,err_eloss_exp,err_yield_em);

	 // pim_yield->Draw("AC*");
	  em_yield->SetLineWidth(3);
	  em_yield->Draw("AC*");

	  TFile *simFileHist = new TFile("/d/cbm02/slebedev/trd/JUL09/simtest/old/piel.0000.reco.root");

	  fhElELoss->Scale(maxExpEl/fhElELoss->GetMaximum());
	  //cout << maxExpEl << " " << fhElELoss->GetMaximum() << endl;
	  Float_t eloss_sim[100], yield_sim_el[100], err_eloss_sim[100], err_yield_sim_el[100];
	  yield_sum=0;
	  for (int i = 0; i < 100; i++){
		  yield_sim_el[i] = fhElELoss->GetBinContent(i);
		  eloss_sim[i] = fhElELoss->GetBinCenter(i)* 10e5;
		  err_eloss_sim[i] = 0;
		  err_yield_sim_el[i] = 0.;//fhElELoss->GetBinError(i);
		  yield_sum += yield_sim_el[i];
	  }
	  cout << "yield sim = " << yield_sum << endl;

	  TGraphErrors *em_yield_sim = new TGraphErrors(100,eloss_sim,yield_sim_el,err_eloss_sim,err_yield_sim_el);
	  em_yield_sim->SetLineColor(kRed);
	  em_yield_sim->SetLineWidth(3);
	  em_yield_sim->Draw("C*");
}
