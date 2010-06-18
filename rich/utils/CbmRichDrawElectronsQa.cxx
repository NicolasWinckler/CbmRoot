/*
 * CbmRichDrawElectronsQa.cxx
 *
 *  Created on: 18.06.2010
 *      Author: slebedev
 */

#include "../../littrack/utils/CbmLitDrawHist.cxx"

void DrawAcc(TH1D* mc, TH1D* acc, TH1D* richTrd, TH1D* richTrdTof){
	TCanvas *c1 = new TCanvas("c1","c1",500,500);
	TH1D* accEff1 = Divide1DHists(acc, mc, "elid_acc_rich_eff",
		"Efficiencies for primary electrons normalized to MC rings",
		"momentum, GeV/c", "efficiency");
	accEff1->SetMinimum(0.);

    TH1D* accEff2 = Divide1DHists(richTrd, mc, "elid_acc_rich_trd_eff","","", "");
    TH1D* accEff3 = Divide1DHists(richTrdTof, mc, "elid_acc_rich_trd_tof_eff", "", "", "");

	std::string hname1, hname2, hname3, hname4;
	hname1 = "accepted RICH rings (" + CalcEfficiency(acc, mc)+")";
	hname2 = "accepted RICH rings+TRD ("+ CalcEfficiency(richTrd, mc)+")";
	hname3 = "accepted RICH rings+TRD+TOF ("+ CalcEfficiency(richTrdTof, mc)+")";
    DrawHist1D(accEff1, accEff2, accEff3, NULL, "", "momentum, GeV/c", "efficiency",
    	hname1, hname2, hname3, "",	false, false, true, 0.3,0.15, 0.99, 0.3);

    gPad->SetGridx(true);
    gPad->SetGridy(true);
}

void DrawMatchingEff(TH1D* fhAccRings, TH1D* fhTrueFoundRings, TH1D* fhTrueMatchStsRichGlobal,
		TH1D* fhTrueMatchStsRichTrdGlobal, TH1D* fhTrueMatchStsRichTrdTofGlobal)
{
	TCanvas *c2 = new TCanvas("c2","c2",500,500);
	TH1D* ringFindEff = Divide1DHists(fhTrueFoundRings, fhAccRings, "elid_ring_find_eff",
		"Ring finding efficiencies for primary electrons (norm. acc. rings)",
		"momentum, GeV/c", "efficiency");
	ringFindEff->SetMinimum(0.);

	TH1D* stsRichEff = Divide1DHists(fhTrueMatchStsRichGlobal, fhAccRings,"elid_sts_rich_eff","",	"", "");
	TH1D* stsRichTrdEff = Divide1DHists(fhTrueMatchStsRichTrdGlobal, fhAccRings,"elid_sts_rich_trd_eff","",	"", "");
	TH1D* stsRichTrdTofEff = Divide1DHists(fhTrueMatchStsRichTrdTofGlobal, fhAccRings,"elid_sts_rich_trd_eff","",	"", "");

	hname1 = "ring finding (" +CalcEfficiency(fhTrueFoundRings,fhAccRings)+")";
	hname2 = "STS+RICH match (" +CalcEfficiency(fhTrueMatchStsRichGlobal,fhAccRings)+")";
	hname3 = "STS+RICH+TRD match ("+CalcEfficiency(fhTrueMatchStsRichTrdGlobal,fhAccRings)+")";
	hname4 = "STS+RICH+TRD+TOF match (" +CalcEfficiency(fhTrueMatchStsRichTrdTofGlobal,fhAccRings)+")";
	DrawHist1D(ringFindEff, stsRichEff, stsRichTrdEff, stsRichTrdTofEff,
		"", "momentum, GeV/c", "efficiency",
		hname1, hname2, hname3, hname4,	false, false, true, 0.3,0.15, 0.99, 0.35);

	gPad->SetGridx(true);
	gPad->SetGridy(true);
}

void DrawMatchingEff2(TH1D* fhAccRings, TH1D* fhTrueFoundRings, TH1D* fhTrueMatchStsRichGlobal,
		TH1D* fhTrueMatchStsRichTrdGlobal,TH1D* fhTrueMatchStsRichTrdTofGlobal)
{
    TCanvas *c3 = new TCanvas("c3","c3",800,800);
    c3->Divide(2,2);
    c3->cd(1);
	TH1D* eff1 = Divide1DHists(fhTrueFoundRings, fhAccRings, "elid_ring_find_eff2",
		"Ring finding efficiencies for primary electrons","momentum, GeV/c", "efficiency");
	hname1 = "Mean eff. = (" +CalcEfficiency(fhTrueFoundRings,fhAccRings)+")";
    DrawHist1D(eff1, NULL, NULL, NULL, "", "momentum, GeV/c", "efficiency",
     	hname1, "", "", "",	false, false, true, 0.3,0.15, 0.99, 0.3);
    eff1->SetMinimum(0.);
    gPad->SetGridx(true);
    gPad->SetGridy(true);

    c3->cd(2);
    TH1D* eff2 = Divide1DHists(fhTrueMatchStsRichGlobal, fhTrueFoundRings, "elid_sts_rich_eff2",
    		"STS+RICH matching efficiency for primary electrons","momentum, GeV/c", "efficiency");
    hname1 = "Mean eff. = ("+CalcEfficiency(fhTrueMatchStsRichGlobal,fhTrueFoundRings)+")";
    DrawHist1D(eff2, NULL, NULL, NULL, "", "momentum, GeV/c", "efficiency",
     	hname1, "", "", "",	false, false, true, 0.3,0.15, 0.99, 0.3);
    eff2->SetMinimum(0.);
    gPad->SetGridx(true);
    gPad->SetGridy(true);

    c3->cd(3);
	TH1D* eff3 = Divide1DHists(fhTrueMatchStsRichTrdGlobal, fhTrueMatchStsRichGlobal, "elid_sts_rich__trd_eff2",
			"STS+RICH+TRD matching efficiency for primary electrons","momentum, GeV/c", "efficiency");
	hname1 = "Mean eff. = ("+CalcEfficiency(fhTrueMatchStsRichTrdGlobal,fhTrueMatchStsRichGlobal)+")";
	DrawHist1D(eff3, NULL, NULL, NULL, "", "momentum, GeV/c", "efficiency",
		hname1, "", "", "",	false, false, true, 0.3,0.15, 0.99, 0.3);
	eff3->SetMinimum(0.);
	gPad->SetGridx(true);
	gPad->SetGridy(true);

    c3->cd(4);
	TH1D* eff4 = Divide1DHists(fhTrueMatchStsRichTrdTofGlobal, fhTrueMatchStsRichTrdGlobal, "elid_sts_rich__trd_eff2",
			"STS+RICH+TRD+TOF matching efficiency for primary electrons","momentum, GeV/c", "efficiency");
	hname1 = "Mean eff. = ("+CalcEfficiency(fhTrueMatchStsRichTrdTofGlobal,fhTrueMatchStsRichTrdGlobal)+")";
	DrawHist1D(eff4, NULL, NULL, NULL, "", "momentum, GeV/c", "efficiency",
		hname1, "", "", "",	false, false, true, 0.3,0.15, 0.99, 0.3);
	eff3->SetMinimum(0.);
	gPad->SetGridx(true);
	gPad->SetGridy(true);
}

void DrawElidEff(TH1D* fhAccRings, TH1D* fhTrueFoundRings, TH1D* fhTrueIdRich,
		TH1D* fhTrueIdRichTrd, TH1D* fhTrueIdRichTrdTof,
		TH1D* fhAccPi, TH1D* fhPiasElRich, TH1D* fhPiasElRichTrd, TH1D* fhPiasElRichTrdTof)
{

	TCanvas* c7 = new TCanvas("c7","c7",800,400);
	c7->Divide(2,1);
	c7->cd(1);
	TH1D* ringFindEff = Divide1DHists(fhTrueFoundRings, fhAccRings, "elid_ring_find_eff",
		"Efficiencies for primary electrons (norm. acc. rings)",
		"momentum, GeV/c", "efficiency");
	ringFindEff->SetMinimum(0.);

	TH1D* elidEffRich = Divide1DHists(fhTrueIdRich, fhAccRings,	"elid_rich_eff","",	"", "");
	TH1D* elidEffRichTrd = Divide1DHists(fhTrueIdRichTrd, fhAccRings, "elid_rich_trd_eff","","", "");
	TH1D* elidEffRichTrdTof = Divide1DHists(fhTrueIdRichTrdTof, fhAccRings,	"elid_rich_trd_eff","",	"", "");

	hname1 = "ring finding (" +CalcEfficiency(fhTrueFoundRings,fhAccRings)+")";
	hname2 = "RICH el id (" +CalcEfficiency(fhTrueIdRich,fhAccRings)+")";
	hname3 = "RICH+TRD el id ("+CalcEfficiency(fhTrueIdRichTrd,fhAccRings)+")";
	hname4 = "RICH+TRD+TOF el id (" +CalcEfficiency(fhTrueIdRichTrdTof,fhAccRings)+")";
	DrawHist1D(ringFindEff, elidEffRich, elidEffRichTrd, elidEffRichTrdTof,
		"", "momentum, GeV/c", "efficiency", hname1, hname2, hname3, hname4,
		false, false, true, 0.3,0.15, 0.99, 0.35);

	gPad->SetGridx(true);
	gPad->SetGridy(true);

	c7->cd(2);
	TH1D* piSuprRich = Divide1DHists(fhAccPi, fhPiasElRich,	"elid_pisup_rich","","", "");
	piSuprRich->SetMinimum(5);
	piSuprRich->SetMaximum(1e5);
	TH1D* piSuprRichTrd = Divide1DHists(fhAccPi, fhPiasElRichTrd, "elid_pisup_rich_trd","","", "");
	TH1D* piSuprRichTrdTof = Divide1DHists(fhAccPi, fhPiasElRichTrdTof,	"elid_pisup_rich_trd_tof","","", "");

	hname1 = "RICH (" +CalcEfficiency(fhAccPi,fhPiasElRich)+")";
	hname2 = "RICH+TRD (" +CalcEfficiency(fhAccPi,fhPiasElRichTrd)+")";
	hname3 = "RICH+TRD+TOF ("+CalcEfficiency(fhAccPi,fhPiasElRichTrdTof)+")";
	for (int i = 1; i < piSuprRichTrd->GetNbinsX(); i++){
		if ( fabs(piSuprRichTrd->GetBinContent(i)) < 0.00000001) piSuprRichTrd->SetBinContent(i, 0.0000001);
		if ( fabs(piSuprRichTrdTof->GetBinContent(i)) < 0.00000001) piSuprRichTrdTof->SetBinContent(i, 0.0000001);
	}
	DrawHist1D(piSuprRich, piSuprRichTrd, piSuprRichTrdTof, NULL,
		"", "momentum, GeV/c", "pion suppression", hname1, hname2, hname3, "",
		false, false, true, 0.3,0.15, 0.99, 0.30);

	gPad->SetGridx(true);
	gPad->SetGridy(true);
	gPad->SetLogy(true);
}
