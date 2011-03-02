/*  Description: This macro draw the histograms which
    are created by the CbmAnaElectronsQa class.

    Author : Semen Lebedev
    E-mail : S.Lebedev@gsi.de
*/

#include "../../../littrack/utils/CbmLitDrawHist.cxx"
#include "../../../littrack/utils/CbmLitUtils.h"

void draw_electrons_qa()
{
	//SetStyles();

	TFile *file = new TFile("/lustre/cbm/user/ebelolap/oct10/urqmd_rho0/25gev/100_field/real/elid.qa.all.root");
   // gROOT->SetStyle("Plain");
   // gStyle->SetPalette(1,0);
   // gStyle->SetOptStat(0000);

	//gStyle->SetHistLineWidth(3);
	//gROOT->ForceStyle();

// get all histograms from file
	TH1D* fh_mc_el = (TH1D*)file->Get("fh_mc_el");
	TH1D* fh_acc_sts_el = (TH1D*)file->Get("fh_acc_sts_el");
	TH1D* fh_acc_rich_el = (TH1D*)file->Get("fh_acc_rich_el");
	TH1D* fh_acc_trd_el = (TH1D*)file->Get("fh_acc_rich_el");
	TH1D* fh_acc_tof_el = (TH1D*)file->Get("fh_acc_tof_el");
	TH1D* fh_match_rich = (TH1D*)file->Get("fh_match_rich");
	TH1D* fh_match_trd = (TH1D*)file->Get("fh_match_trd");
	TH1D* fh_match_tof = (TH1D*)file->Get("fh_match_tof");
	TH1D* fh_elid_rich = (TH1D*)file->Get("fh_elid_rich");
	TH1D* fh_elid_trd = (TH1D*)file->Get("fh_elid_trd");
	TH1D* fh_elid_tof = (TH1D*)file->Get("fh_elid_tof");
	TH1D* fh_acc_pi = (TH1D*)file->Get("fh_acc_pi");
	TH1D* fh_pi_as_el_rich = (TH1D*)file->Get("fh_pi_as_el_rich");
	TH1D* fh_pi_as_el_trd = (TH1D*)file->Get("fh_pi_as_el_trd");
	TH1D* fh_pi_as_el_tof = (TH1D*)file->Get("fh_pi_as_el_tof");
	TH1D* fh_nof_global = (TH1D*)file->Get("fh_nof_global");
	TH1D* fh_nof_sts_tracks = (TH1D*)file->Get("fh_nof_sts_tracks");
	TH1D* fh_nof_rich_rings = (TH1D*)file->Get("fh_nof_rich_rings");
	TH1D* fh_nof_trd_tracks = (TH1D*)file->Get("fh_nof_trd_tracks");

//signal electron acceptance
	{
	TCanvas *c1 = new TCanvas("acc_eff","acc_eff",500,500);
	TH1D* accEff1 = Divide1DHists(fh_acc_sts_el, fh_mc_el, "fh_acc_sts_el_eff",	"Acceptance",
		"momentum, GeV/c", "efficiency");
	accEff1->SetMinimum(0.);
	TH1D* accEff2 = Divide1DHists(fh_acc_rich_el, fh_mc_el, "fh_acc_rich_el_eff","","", "");
	TH1D* accEff3 = Divide1DHists(fh_acc_trd_el, fh_mc_el, "fh_acc_trd_el_eff", "", "", "");
	TH1D* accEff4 = Divide1DHists(fh_acc_tof_el, fh_mc_el, "fh_acc_tof_el_eff", "", "", "");

	std::string hname1, hname2, hname3, hname4;
	hname1 = "STS (" + CalcEfficiency(fh_acc_sts_el, fh_mc_el)+")";
	hname2 = "RICH ("+ CalcEfficiency(fh_acc_rich_el, fh_mc_el)+")";
	hname3 = "TRD ("+ CalcEfficiency(fh_acc_trd_el, fh_mc_el)+")";
	hname4 = "TOF ("+ CalcEfficiency(fh_acc_tof_el, fh_mc_el)+")";
	DrawHist1D(accEff1, accEff2, accEff3, accEff4, "", "momentum, GeV/c", "efficiency",
		hname1, hname2, hname3, hname4,	false, false, true, 0.3,0.15, 0.99, 0.3);

	gPad->SetGridx(true);
	gPad->SetGridy(true);
	}

//signal electron matching
	{
	TCanvas *c2 = new TCanvas("match_eff","match_eff",500,500);
	TH1D* match_rich_eff = Divide1DHists(fh_match_rich, fh_acc_rich_el, "match_rich_eff",
		"Matching","momentum, GeV/c", "efficiency");
	match_rich_eff->SetMinimum(0.);
	TH1D* match_trd_eff = Divide1DHists(fh_match_trd, fh_acc_rich_el,"match_trd_eff","","", "");
	TH1D* match_tof_eff = Divide1DHists(fh_match_tof, fh_acc_rich_el,"match_tof_eff","","", "");

	std::string hname1, hname2, hname3;
	hname1 = "STS-RICH (" +CalcEfficiency(fh_match_rich,fh_acc_rich_el)+")";
	hname2 = "TRD (" +CalcEfficiency(fh_match_trd,fh_acc_rich_el)+")";
	hname3 = "TOF ("+CalcEfficiency(fh_match_tof,fh_acc_rich_el)+")";
	DrawHist1D(match_rich_eff, match_trd_eff, match_tof_eff, NULL,
		"", "momentum, GeV/c", "efficiency",
		hname1, hname2, hname3, "",	false, false, true, 0.3,0.15, 0.99, 0.35);

	gPad->SetGridx(true);
	gPad->SetGridy(true);
	}


//signal electron matching separate
	{
	std::string hname1;
	TCanvas *c3 = new TCanvas("match_eff2","match_eff2",800,800);
	c3->Divide(2,2);
	c3->cd(1);
	TH1D* eff1 = Divide1DHists(fh_match_rich, fh_acc_rich_el, "eff1",
		"STS-RICH matching","momentum, GeV/c", "efficiency");
	hname1 = "Mean eff. = (" +CalcEfficiency(fh_match_rich,fh_acc_rich_el)+")";
	DrawHist1D(eff1, NULL, NULL, NULL, "", "momentum, GeV/c", "efficiency",
		hname1, "", "", "",	false, false, true, 0.3,0.15, 0.99, 0.3);
	eff1->SetMinimum(0.);
	gPad->SetGridx(true);
	gPad->SetGridy(true);

	c3->cd(2);
	TH1D* eff2 = Divide1DHists(fh_match_trd, fh_match_rich, "eff2",
			"TRD matching","momentum, GeV/c", "efficiency");
	hname1 = "Mean eff. = ("+CalcEfficiency(fh_match_trd, fh_match_rich)+")";
	DrawHist1D(eff2, NULL, NULL, NULL, "", "momentum, GeV/c", "efficiency",
		hname1, "", "", "",	false, false, true, 0.3,0.15, 0.99, 0.3);
	eff2->SetMinimum(0.);
	gPad->SetGridx(true);
	gPad->SetGridy(true);

	c3->cd(3);
	TH1D* eff3 = Divide1DHists(fh_match_tof, fh_match_trd, "eff3",
			"TOF matching","momentum, GeV/c", "efficiency");
	hname1 = "Mean eff. = ("+CalcEfficiency(fh_match_tof, fh_match_trd)+")";
	DrawHist1D(eff3, NULL, NULL, NULL, "", "momentum, GeV/c", "efficiency",
		hname1, "", "", "",	false, false, true, 0.3,0.15, 0.99, 0.3);
	eff3->SetMinimum(0.);
	gPad->SetGridx(true);
	gPad->SetGridy(true);
	}

//ELECTRON IDENTIFICATION AND PION SUPPRESSION
	{
	std::string hname1, hname2, hname3, hname4;
	TCanvas* c4 = new TCanvas("el_eff_pi_supp","el_eff_pi_supp",800,400);
	c4->Divide(2,1);
	c4->cd(1);
	TH1D* eff1 = Divide1DHists(fh_elid_rich, fh_acc_rich_el, "eff1", "Electron Identification",
		"momentum, GeV/c", "efficiency");
	eff1->SetMinimum(0.);
	TH1D* eff2 = Divide1DHists(fh_elid_trd, fh_acc_rich_el,	"eff2","",	"", "");
	TH1D* eff3 = Divide1DHists(fh_elid_tof, fh_acc_rich_el, "eff3","","", "");

	hname1 = "RICH (" +CalcEfficiency(fh_elid_rich,fh_acc_rich_el)+")";
	hname2 = "TRD (" +CalcEfficiency(fh_elid_trd,fh_acc_rich_el)+")";
	hname3 = "TOF ("+CalcEfficiency(fh_elid_tof,fh_acc_rich_el)+")";
	DrawHist1D(eff1, eff2, eff3, NULL, "", "momentum, GeV/c", "efficiency",
			hname1, hname2, hname3, "",	false, false, true, 0.3,0.15, 0.99, 0.35);

	gPad->SetGridx(true);
	gPad->SetGridy(true);

	c4->cd(2);
	TH1D* supp1 = Divide1DHists(fh_acc_pi, fh_pi_as_el_rich,	"elid_pi_supp_rich","","", "");
	supp1->SetMinimum(5);
	supp1->SetMaximum(1e5);
	TH1D* supp2 = Divide1DHists(fh_acc_pi, fh_pi_as_el_trd, "elid_pi_supp_rich_trd","","", "");
	TH1D* supp3 = Divide1DHists(fh_acc_pi, fh_pi_as_el_tof,	"elid_pi_supp_rich_trd_tof","","", "");

	hname1 = "RICH (" +CalcEfficiency(fh_acc_pi,fh_pi_as_el_rich)+")";
	hname2 = "TRD (" +CalcEfficiency(fh_acc_pi,fh_pi_as_el_trd)+")";
	hname3 = "TOF ("+CalcEfficiency(fh_acc_pi,fh_pi_as_el_tof)+")";
	for (int i = 1; i < fh_pi_as_el_trd->GetNbinsX(); i++){
		if ( fabs(fh_pi_as_el_trd->GetBinContent(i)) < 0.00000001) fh_pi_as_el_trd->SetBinContent(i, 0.0000001);
		if ( fabs(fh_pi_as_el_tof->GetBinContent(i)) < 0.00000001) fh_pi_as_el_tof->SetBinContent(i, 0.0000001);
	}
	DrawHist1D(supp1, supp2, supp3, NULL,"", "momentum, GeV/c", "pion suppression",
			hname1, hname2, hname3, "",	false, false, true, 0.3,0.15, 0.99, 0.30);

	gPad->SetGridx(true);
	gPad->SetGridy(true);
	gPad->SetLogy(true);
	}

//test distributions
	{
	std::string hname1;
    std::stringstream ss;
    ss.precision(3);

	TCanvas *c5 = new TCanvas("test_distr","test_distr",800,800);
	c5->Divide(2,2);
	c5->cd(1);
    hname1 = "Mean=" + ToString<float>(fh_nof_global->GetMean());
	DrawHist1D(fh_nof_global, NULL, NULL, NULL, "", "momentum, GeV/c", "Entries",
		hname1, "", "", "",	false, false, true, 0.3,0.15, 0.99, 0.3);
	gPad->SetGridx(true);
	gPad->SetGridy(true);

	c5->cd(2);
    hname1 = "Mean=" + ToString<float>(fh_nof_sts_tracks->GetMean());
	DrawHist1D(fh_nof_sts_tracks, NULL, NULL, NULL, "", "momentum, GeV/c", "Entries",
		hname1, "", "", "",	false, false, true, 0.3,0.15, 0.99, 0.3);
	gPad->SetGridx(true);
	gPad->SetGridy(true);

	c5->cd(3);
    hname1 = "Mean=" + ToString<float>(fh_nof_rich_rings->GetMean());
	DrawHist1D(fh_nof_rich_rings, NULL, NULL, NULL, "", "momentum, GeV/c", "Entries",
		hname1, "", "", "",	false, false, true, 0.3,0.15, 0.99, 0.3);
	gPad->SetGridx(true);
	gPad->SetGridy(true);

	c5->cd(4);
    hname1 = "Mean=" + ToString<float>(fh_nof_trd_tracks->GetMean());
	DrawHist1D(fh_nof_trd_tracks, NULL, NULL, NULL, "", "momentum, GeV/c", "Entries",
		hname1, "", "", "",	false, false, true, 0.3,0.15, 0.99, 0.3);
	gPad->SetGridx(true);
	gPad->SetGridy(true);
	}

//StsQa
	{
	TCanvas *c6 = new TCanvas("stsQa","stsQa",1200,800);
	c6->Divide(3,2);
	c6->cd(1);
	fh_rec_mc_mom_signal->Draw();
	gPad->SetGridx(true);
	gPad->SetGridy(true);
	gPad->SetLogy(true);
	c2->cd(2);
	fh_mom_res_vs_mom_signal->Draw("COLZ");
	gPad->SetGridx(true);
	gPad->SetGridy(true);
	c2->cd(3);
	fh_mean_mom_vs_mom_signal->Draw();
	gPad->SetGridx(true);
	gPad->SetGridy(true);
	c2->cd(4);
	fh_chiprim_signal->Draw();
	gPad->SetGridx(true);
	gPad->SetGridy(true);
	c2->cd(5);
	fh_chiprim_signal2->Draw();
	gPad->SetGridx(true);
	gPad->SetGridy(true);
	}
}

