
TGraph* dedx_table;
TGraph* ion_table;
TGraph* brems_table;
TGraph* pair_table;
TGraph* dedx_calc;
TGraph* ion_calc;
TGraph* brems_calc;
TGraph* pair_calc;

Double_t mom[26] = {
		47.04, 56.16, 68.02, 85.09, 100.3, 152.7, 
		176.4, 221.8, 286.8, 364.2, 391.7, 494.5, 899.5,   
		1101, 1502, 2103, 3104, 4104, 8105, 
		10110, 14110, 20110, 30110, 40110, 80110, 100100 
};


void draw_dedx() 
{
	
	load_libraries();
	
	create_graphs();
	
	fill_table();
	
	calc_eloss();
	
	draw_graphs();	
}

void draw_graphs()
{  
	TCanvas *c1 = new TCanvas("energy loss","c1",1200,1000);
//    c1->SetGrid();

	
	dedx_table->SetLineStyle(3);
	ion_table->SetLineStyle(3);
	brems_table->SetLineStyle(3);
	pair_table->SetLineStyle(3);	
	
	dedx_table->SetLineColor(2);
	ion_table->SetLineColor(2);
	brems_table->SetLineColor(2);
	pair_table->SetLineColor(2);
	
	dedx_table->SetMarkerColor(2);
	ion_table->SetMarkerColor(2);
	brems_table->SetMarkerColor(2);
	pair_table->SetMarkerColor(2);
	
	dedx_table->SetMarkerStyle(20);
	ion_table->SetMarkerStyle(26);
	brems_table->SetMarkerStyle(27);
	pair_table->SetMarkerStyle(25);
	
	
	dedx_calc->SetLineStyle(1);
	ion_calc->SetLineStyle(1);
	brems_calc->SetLineStyle(1);
	pair_calc->SetLineStyle(1);	
	
	dedx_calc->SetLineColor(4);
	ion_calc->SetLineColor(4);
	brems_calc->SetLineColor(4);
	pair_calc->SetLineColor(4);	
	
	dedx_calc->SetMarkerColor(4);
	ion_calc->SetMarkerColor(4);
	brems_calc->SetMarkerColor(4);
	pair_calc->SetMarkerColor(4);
	
	dedx_calc->SetMarkerStyle(20);
	ion_calc->SetMarkerStyle(26);
	brems_calc->SetMarkerStyle(27);
	pair_calc->SetMarkerStyle(25);
	
	
	TMultiGraph* mg = new TMultiGraph();
	mg->Add(dedx_table);
	mg->Add(ion_table);
	mg->Add(brems_table);
	mg->Add(pair_table);
	
	mg->Add(dedx_calc);
	mg->Add(ion_calc);
	mg->Add(brems_calc);
	mg->Add(pair_calc);
	
	mg->SetMinimum(0.1);
	mg->SetMaximum(10.);
    gPad->SetLogx();
	gPad->SetLogy();
	
	mg->Draw("ALP");
	
	mg->GetXaxis()->SetTitle("Muon momentum [MeV/c]");
	mg->GetYaxis()->SetTitle("Mean energy loss in iron [Mev*cm^2/g]");
	mg->GetXaxis()->SetLimits(45, 100102);
	
	TLegend* l1 = new TLegend(0.6, 0.1, 0.9, 0.4);
	l1->SetHeader("Energy losses for muons in iron vs. momentum");
	l1->AddEntry(dedx_table,"total (table)","lp");
	l1->AddEntry(dedx_calc,"total (Lit)","lp");
	l1->AddEntry(ion_table,"ionization (table)","lp");
	l1->AddEntry(ion_calc,"ionization (Lit)","lp");
	l1->AddEntry(brems_table,"bremmstrahlung (table)","lp");
	l1->AddEntry(brems_calc,"bremmstrahlung (Lit)","lp");
	l1->AddEntry(pair_table,"pair production (table)","lp");
	l1->AddEntry(pair_calc,"pair production (Lit)","lp");
	l1->Draw();
	
	
}

void fill_table()
{
	Double_t table[26][4] = {
		//ion    //brems   //pair   //total
		5.494,   0.000,    0.000,   5.494,
		4.321,   0.000,    0.000,   4.321,
		3.399,   0.000,    0.000,   3.399,
		2.654,   0.000,    0.000,   2.654,
		2.274,   0.000,    0.000,   2.274,
		1.717,   0.000,    0.000,   1.717,
		
		1.616,   0.000,    0.000,   1.616,
		1.516,   0.000,    0.000,   1.516,
		1.463,   0.000,    0.000,   1.463,
		1.451,   0.000,    0.000,   1.451,
		1.452,   0.000,    0.000,   1.453,
		1.467,   0.000,    0.000,   1.467,
		1.548,   0.000,    0.000,   1.548,
		
		1.581,   0.001,    0.000,   1.582,
		1.635,   0.001,    0.000,   1.637,
		1.694,   0.002,    0.001,   1.697,
		1.760,   0.003,    0.002,   1.767,
		1.806,   0.004,    0.004,   1.816,
		1.911,   0.010,    0.011,   1.936,
		
		1.942,   0.014,    0.015,   1.975,
		1.987,   0.021,    0.024,   2.039,
		2.032,   0.033,    0.039,   2.113,
		2.080,   0.054,    0.068,   2.214,
		2.112,   0.076,    0.099,   2.303,
		2.184,   0.171,    0.236,   2.623,
		2.207,   0.221,    0.310,   2.777
	};
	
	for (Int_t i = 0; i < 26; i++) {
		ion_table->SetPoint(i, mom[i], table[i][0]);
		brems_table->SetPoint(i, mom[i], table[i][1]);
		pair_table->SetPoint(i, mom[i], table[i][2]);
		dedx_table->SetPoint(i, mom[i], table[i][3]);
	}
}

void calc_eloss()
{
	CbmLitMaterialEffectsImp me;
	CbmLitMaterialInfo mat;
	mat.SetA(55.85);
	mat.SetZ(26.0);
	mat.SetRho(7.87);
	mat.SetRL(1.757622);
	
	CbmLitTrackParam par;	
	
	for (Int_t i = 0; i < 26; i++) {
		par.SetQp(1./(mom[i]*1e-3));
		ion_calc->SetPoint(i, mom[i], me.BetheBloch(&par, &mat) * 1e3);
		brems_calc->SetPoint(i, mom[i], me.BetheHeitler(&par, &mat) * 1e3);
		pair_calc->SetPoint(i, mom[i], me.PairProduction(&par, &mat) * 1e3);
		dedx_calc->SetPoint(i, mom[i], me.EnergyLoss(&par, &mat) * 1e3);
	}
}

void create_graphs()
{
	Int_t nBinsMom = 100;
	Int_t nBins = 200;
	Double_t minMom = 0.1;
	Double_t maxMom = 35;
	Double_t minEloss = 0.;
	Double_t maxEloss = 4.;
    dedx_table = new TGraph();
    ion_table = new TGraph();
    brems_table = new TGraph();
    pair_table = new TGraph();    
    
    dedx_calc = new TGraph();
    ion_calc = new TGraph();
    brems_calc = new TGraph();
    pair_calc = new TGraph();
}

void load_libraries()
{
   gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
   basiclibs();
   gSystem->Load("libGeoBase");
   gSystem->Load("libParBase");
   gSystem->Load("libBase");
   gSystem->Load("libCbmBase");
   gSystem->Load("libField");
   gSystem->Load("libGen");
   gSystem->Load("libPassive"); 
   gSystem->Load("libTrkBase");
   gSystem->Load("libGeane");
   gSystem->Load("libMvd");
   gSystem->Load("libSts");
   gSystem->Load("libRich");
   gSystem->Load("libTrd");
   gSystem->Load("libTof");
   gSystem->Load("libEcal");
   gSystem->Load("libGlobal");
   gSystem->Load("libKF");
   gSystem->Load("libL1");   
   gSystem->Load("libMuch"); 
   gSystem->Load("libLittrack");
}
