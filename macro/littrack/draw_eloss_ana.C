TString engine = "geant4/";
TString setup = "10gev10X0iron/";
TString dir = "/d/cbm02/andrey/events/eloss/" + engine + setup;
TString fileName = dir + "eloss.ana.root";
TString outDir = "./" + engine + setup;

void draw_eloss_ana()
{
	gStyle->SetOptStat("");
	gStyle->SetOptFit(0);
	gStyle->SetOptTitle(0);
	//gStyle->SetOptFit(0001);

	TFile *file = new TFile(fileName); 
	
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/draw_hist.C");
  

	c3->cd(1);
	draw_hist_1D(fh_theta_mc, "projected scattering angle, rad", "counter",
		  kBlue, lwidth, 1, msize, kDot, false, true, "");
	fh_theta_mc->Fit("gaus");
	//draw_hist_1D(fh_theta_calc, "projected scattering angle, rad", "counter",
	//		  kRed, lwidth, 1, msize, kDot, false, true, "SAME");

	stringstream oss1;
	oss1.precision(3);
	oss1 << "Geant4, #sigma = " << fh_theta_mc->GetFunction("gaus")->GetParameter(2);
	TLegend* l1 = new TLegend(0.5,0.7,0.9,0.9);
	l1->AddEntry(NULL,oss1.str().c_str(),"");
	l1->AddEntry(NULL,"Lit, #sigma = 0.0047","");
	l1->Draw();
	
	TLegend* l2 = new TLegend(0.5,0.5,0.9,0.9);
	l2->SetHeader("#splitline{Projected scattering angle;}{10 X_{0} of iron, p=10 Gev/c}");
	l2->Draw();
	
	c3->cd(4);
	
}

  
