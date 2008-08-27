TGraph* table[4];
TGraph* calc[4];

Double_t mom[26] = {
		47.04, 56.16, 68.02, 85.09, 100.3, 152.7, 
		176.4, 221.8, 286.8, 364.2, 391.7, 494.5, 899.5,   
		1101, 1502, 2103, 3104, 4104, 8105, 
		10110, 14110, 20110, 30110, 40110, 80110, 100100 
};

void draw_dedx() 
{
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
	cbmrootlibs();
	
	create_graphs();
	
	fill_table_iron();
//	fill_table_tungsten();
//	fill_table_carbon();
		
	calc_eloss("iron");
	
	draw_graphs();	
}

void draw_graphs()
{  
	TCanvas *c1 = new TCanvas("energy loss","c1",1200,1000);
//    c1->SetGrid();

	for (int i = 0; i < 4; i++) {
		table[i]->SetLineStyle(3);
		table[i]->SetLineColor(2);
		table[i]->SetMarkerColor(2);
		table[i]->SetLineWidth(3);
		table[i]->SetMarkerSize(2);
		
		calc[i]->SetLineStyle(1);
		calc[i]->SetLineColor(4);
		calc[i]->SetMarkerColor(4);
		calc[i]->SetLineWidth(3);
		calc[i]->SetMarkerSize(2);
	}
	table[0]->SetMarkerStyle(20);
	table[1]->SetMarkerStyle(26);
	table[2]->SetMarkerStyle(27);
	table[3]->SetMarkerStyle(25);

	calc[0]->SetMarkerStyle(20);
	calc[1]->SetMarkerStyle(26);
	calc[2]->SetMarkerStyle(27);
	calc[3]->SetMarkerStyle(25);
	
	TMultiGraph* mg = new TMultiGraph();
	for (int i = 0; i < 4; i++) {
		mg->Add(table[i]);
		mg->Add(calc[i]);
	}
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
	l1->AddEntry(table[0],"total (table)","lp");
	l1->AddEntry(calc[0],"total (Lit)","lp");
	l1->AddEntry(table[1],"ionization (table)","lp");
	l1->AddEntry(calc[1],"ionization (Lit)","lp");
	l1->AddEntry(table[2],"bremsstrahlung (table)","lp");
	l1->AddEntry(calc[2],"bremsstrahlung (Lit)","lp");
	l1->AddEntry(table[3],"pair production (table)","lp");
	l1->AddEntry(calc[3],"pair production (Lit)","lp");
	l1->Draw();	
}

void fill_table_iron()
{
	Double_t table_iron[26][4] = {
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
		table[1]->SetPoint(i, mom[i], table_iron[i][0]);
		table[2]->SetPoint(i, mom[i], table_iron[i][1]);
		table[3]->SetPoint(i, mom[i], table_iron[i][2]);
		table[0]->SetPoint(i, mom[i], table_iron[i][3]);
	}
}

void fill_table_tungsten()
{
	Double_t table_tungsten[26][4] = {
		//ion    //brems   //pair   //total
		4.000,   0.000,    0.000,   4.00,
		3.185,   0.000,    0.000,   3.185,
		2.534,   0.000,    0.000,   2.534,
		2.000,   0.000,    0.000,   2.000,
		1.726,   0.000,    0.000,   1.726,
		1.323,   0.000,    0.000,   1.323,
		
		1.251,   0.000,    0.000,   1.251,
		1.182,   0.000,    0.000,   1.182,
		1.149,   0.000,    0.000,   1.149,
		1.145,   0.000,    0.000,   1.145,
		1.150,   0.000,    0.000,   1.150,
		1.168,   0.000,    0.000,   1.168,
		1.247,   0.001,    0.000,   1.249,
		
		1.279,   0.001,    0.000,   1.281,
		1.329,   0.002,    0.001,   1.333,
		1.384,   0.004,    0.002,   1.391,
		1.446,   0.006,    0.005,   1.459,
		1.489,   0.009,    0.009,   1.509,
		1.587,   0.023,    0.026,   1.640,
		
		1.616,   0.031,    0.036,   1.687,
		1.658,   0.047,    0.057,   1.768,
		1.700,   0.073,    0.092,   1.874,
		1.745,   0.120,    0.159,   2.035,
		1.774,   0.169,    0.231,   2.190,
		1.840,   0.383,    0.548,   2.800,
		1.860,   0.496,    0.717,   3.110
	};
	
	for (Int_t i = 0; i < 26; i++) {
		table[1]->SetPoint(i, mom[i], table_tungsten[i][0]);
		table[2]->SetPoint(i, mom[i], table_tungsten[i][1]);
		table[3]->SetPoint(i, mom[i], table_tungsten[i][2]);
		table[0]->SetPoint(i, mom[i], table_tungsten[i][3]);
	}

}

void fill_table_carbon()
{
	Double_t table_carbon[26][4] = {
		//ion    //brems   //pair   //total
		7.119,   0.000,    0.000,   7.119,
		5.551,   0.000,    0.000,   5.551,
		4.332,   0.000,    0.000,   4.332,
		3.356,   0.000,    0.000,   3.356,
		2.861,   0.000,    0.000,   2.861,
		2.127,   0.000,    0.000,   2.127,
		
		1.992,   0.000,    0.000,   1.992,
		1.854,   0.000,    0.000,   1.854,
		1.775,   0.000,    0.000,   1.775,
		1.745,   0.000,    0.000,   1.745,
		1.745,   0.000,    0.000,   1.745,
		1.751,   0.000,    0.000,   1.751,
		1.819,   0.000,    0.000,   1.820,
		
		1.850,   0.000,    0.000,   1.851,
		1.900,   0.000,    0.000,   1.901,
		1.955,   0.000,    0.000,   1.957,
		2.018,   0.001,    0.001,   2.021,
		2.062,   0.001,    0.001,   2.066,
		2.161,   0.003,    0.003,   2.171,
		
		2.191,   0.004,    0.004,   2.204,
		2.234,   0.006,    0.007,   2.254,
		2.278,   0.010,    0.011,   2.308,
		2.326,   0.016,    0.019,   2.374,
		2.359,   0.022,    0.028,   2.427,
		2.434,   0.050,    0.068,   2.587,
		2.458,   0.065,    0.089,   2.655
	};
	
	for (Int_t i = 0; i < 26; i++) {
		table[1]->SetPoint(i, mom[i], table_carbon[i][0]);
		table[2]->SetPoint(i, mom[i], table_carbon[i][1]);
		table[3]->SetPoint(i, mom[i], table_carbon[i][2]);
		table[0]->SetPoint(i, mom[i], table_carbon[i][3]);
	}

}

void calc_eloss(
		const string& mat)
{
	CbmLitMaterialInfo material;
	if (mat == "tungsten") {
		material.SetA(183.84);
		material.SetZ(74.0);
		material.SetRho(19.3);
		material.SetRL(0.35);
	} else 
	if (mat == "iron") {
		material.SetA(55.85);
		material.SetZ(26.0);
		material.SetRho(7.87);
		material.SetRL(1.757622);
	} else 
	if (mat == "carbon") {
		material.SetA(12.0107);
		material.SetZ(6);
		material.SetRho(2.265);
		material.SetRL(18.8);
	} else exit(0);
	
	CbmLitMaterialEffectsImp me;
	CbmLitTrackParam par;	
	for (Int_t i = 0; i < 26; i++) {
		par.SetQp(1./(mom[i]*1e-3));
		calc[1]->SetPoint(i, mom[i], me.BetheBloch(&par, &material) * 1e3);
		calc[2]->SetPoint(i, mom[i], me.BetheHeitler(&par, &material) * 1e3);
		calc[3]->SetPoint(i, mom[i], me.PairProduction(&par, &material) * 1e3);
		calc[0]->SetPoint(i, mom[i], me.dEdx(&par, &material) * 1e3);
	}
}

void create_graphs()
{
	for (int i = 0; i < 4; i++) {
	    table[i] = new TGraph();
	    calc[i] = new TGraph();
	}
}
