/** draw_analysis.C
 * @author Elena Lebedeva <e.lebedeva@gsi.de>
 * @since 2010
 * @version 1.0
 **/

void draw_eff_pty(TH2D* h1, TH2D* h2){
    char effTxt[60];
    if (h2->GetEntries() != 0) sprintf(effTxt,"%.1f\%", (Double_t)h1->GetEntries()/h2->GetEntries()*100.);
    TText *t3 = new TText(0.2, 1.8, effTxt);
    t3->Draw();
}

void draw_eff_mom(TH1D* h1, TH1D* h2){
    char effTxt[60];
    if (h2->GetEntries() != 0) sprintf(effTxt, "%.1f\%", (Double_t)h1->GetEntries()/h2->GetEntries()*100.);
    TText *t3 = new TText(2., 0.2, effTxt);
    t3->Draw();
}

TH2D* divideHisto2D(TH2D* h1, TH2D* h2) 
{
    Int_t nBinsX = h1->GetNbinsX();
    Double_t minX = h1->GetXaxis()->GetXmin();
    Double_t maxX = h1->GetXaxis()->GetXmax();
    Int_t nBinsY = h1->GetNbinsY();
    Double_t minY = h1->GetYaxis()->GetXmin();
    Double_t maxY = h1->GetYaxis()->GetXmax();

    TH2D* h3 = new TH2D(h1->GetName(), h1->GetTitle(), nBinsX, minX, maxX, nBinsY, minY, maxY);
    h1->Sumw2();
    h2->Sumw2();
    h3->Divide(h1,h2,1,1,"B");
    return h3;
}

TH1D* divideHisto1D(TH1D* h1, TH1D* h2)
{
    Int_t nBins = h1->GetNbinsX();
    Double_t min = h1->GetXaxis()->GetXmin();
    Double_t max = h1->GetXaxis()->GetXmax();
    h1->Sumw2();
    h2->Sumw2();
    TH1D* h3 = new TH1D(h1->GetName(), h1->GetTitle(), nBins, min, max);
    h3->Divide(h1, h2);
    return h3;

}

void calculateSignalOverBg(TH1D* s, TH1D* bg)
{

    TH1D* sClone = (TH1D*)s->Clone();
    //ROOT::Math::MinimizerOptions::SetDefaultPrintLevel(-1);    
    //TF1* fit = new TF1("gaus1", "gaus");
   // fit->Config().MinimizerOptions().SetPrintLevel(-1);
    sClone->Fit("gaus");

    Double_t mean = sClone->GetFunction("gaus")->GetParameter("Mean");
    Double_t sigma = sClone->GetFunction("gaus")->GetParameter("Sigma");
    cout << "Mean = " << mean << " Sigma = " << sigma <<  endl;
    cout << "Min = " << mean - 2.*sigma << " Max = " << mean + 2*sigma << endl;

    Int_t minInd = s->FindBin(mean - 2.*sigma);
    Int_t maxInd = s->FindBin(mean + 2.*sigma);

    cout << "nof bins = " << maxInd - minInd +1 << endl;

    Double_t sumSignal = 0.;
    Double_t sumBg = 0.;
    for (Int_t i = minInd + 1; i <= maxInd - 1; i++){
       sumSignal += s->GetBinContent(i);
       sumBg += bg->GetBinContent(i);
    }

    cout << s->GetName() << " - S/BG = " << sumSignal/sumBg << ", BG/S = " << sumBg/sumSignal<<endl;
}

//option == forward or back
TH1D* CalculateSignificance(TH1D* signal, TH1D* bg, TString name, TString title, TString option)
{
    Int_t nBins = signal->GetNbinsX();
    Double_t xmin = signal->GetXaxis()->GetXmin();
    Double_t xmax = signal->GetXaxis()->GetXmax();
    TH1D* hsig = new TH1D(name,title, nBins, xmin, xmax);

    Double_t sumSignal = 0;
    Double_t sumBg = 0;
    Double_t significance = 0;
    if(option == "forward"){
        for (Int_t i = 1; i <= nBins; i++) {
            sumSignal = signal->Integral(1, i,"width");
            sumBg = bg->Integral(1, i,"width");
            Double_t prov = TMath::Sqrt(sumSignal+sumBg);
            if (prov != 0. ) significance = sumSignal / prov;
            else significance = 0.;
            hsig->SetBinContent(i, significance);
        }
    }else if (option == "back"){
        for (Int_t i = nBins; i >= 1; i--) {
            sumSignal = signal->Integral(i, nBins,"width");
            sumBg = bg->Integral(i, nBins,"width");
            Double_t prov = TMath::Sqrt(sumSignal+sumBg);
            if (prov != 0. ) significance = sumSignal / prov;
            else significance = 0.;
            hsig->SetBinContent(i, significance);
        }
    }
    return hsig;
}

void draw_minv(TH1* s1, TH1* bg1)
{
    TH1D* s = (TH1D*)s1->Clone();
	s->SetFillColor(kRed);
    s->SetLineColor(kBlack);
    TH1D* bg = (TH1D*)bg1->Clone();
	bg->SetFillColor(kYellow - 10);
    bg->SetLineColor(kBlack);
	TH1D* sbg = (TH1D*)bg->Clone();
    sbg->SetFillColor(kBlue);
    sbg->SetLineColor(kBlack);
    sbg->Add(s);
	sbg->SetMinimum(1e-8);
	sbg->Draw();
	bg->Draw("same");
	s->Draw("same");
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);
}

void scaleAllHistogramms(Int_t nofEvents)
{
    cout << "Scale all histogramms:" << endl;
    TDirectory * dir = gDirectory;
    TIter nextkey( dir->GetListOfKeys());
    TKey *key;

    Int_t count = 0;
    while (key = (TKey*) nextkey()) {
        TObject* obj = key->ReadObj();

        if (obj->IsA()->InheritsFrom (TH1::Class())) {
            TH1* h1 = (TH1*) obj;
            cout << h1->GetName() << endl;
            h1->Scale(1./(Double_t)nofEvents);
            count++;
        }
    }
    cout << count << " histogramms were scaled." << endl;
}

void draw_analysis(){

    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();

    gROOT->LoadMacro("$VMCWORKDIR/macro/rich/cbmlibs.C");
    cbmlibs();

    TString fileName = "/lustre/cbm/user/ebelolap/oct10/urqmd_omega/25gev/100_field/real/mytask.analysis.all_temp.root";
    TFile *file = new TFile(fileName);

    gStyle->SetHistLineWidth(3);
	//SetStyles();
    gROOT->SetStyle("Plain");
    gStyle->SetPalette(1,0);
    
    Int_t fNofEvents = fh_event_number->GetEntries();
    cout << "File name = " << fileName << endl;
    cout << "Number of events = " << fNofEvents<< endl;

    TCanvas *c1 = new TCanvas("c1-signal-fit", "c1-signal-fit", 600, 600);
    calculateSignalOverBg(fh_rec_signal_minv, fh_rec_bg_minv);
    calculateSignalOverBg(fh_chi_prim_signal_minv, fh_chi_prim_bg_minv);
    calculateSignalOverBg(fh_rich_id_signal_minv, fh_rich_id_bg_minv);
    calculateSignalOverBg(fh_trd_id_signal_minv, fh_trd_id_bg_minv);
    calculateSignalOverBg(fh_tof_id_signal_minv, fh_tof_id_bg_minv);
    calculateSignalOverBg(fh_gammacut_signal_minv, fh_gammacut_bg_minv);
    calculateSignalOverBg(fh_stcut_signal_minv, fh_stcut_bg_minv);
    calculateSignalOverBg(fh_ttcut_signal_minv, fh_ttcut_bg_minv);
    calculateSignalOverBg(fh_ptcut_signal_minv, fh_ptcut_bg_minv);
    calculateSignalOverBg(fh_anglecut_signal_minv, fh_anglecut_bg_minv);
  //  calculateSignalOverBg(fh_apmcut_signal_minv, fh_apmcut_bg_minv);

//SCALE ALL HISTOGRAMMS
    scaleAllHistogramms(fNofEvents);

// pty distribution of signal
    TCanvas *c2 = new TCanvas("c2-pty","c2-pty",1000,1000);
    c2->Divide(4,4);
    c2->cd(1);
    fh_mc_signal_pty->Draw("COLZ");
    draw_eff_pty(fh_mc_signal_pty, fh_mc_signal_pty);
    c2->cd(2);
    fh_acc_signal_pty->Draw("COLZ");
    draw_eff_pty(fh_acc_signal_pty, fh_mc_signal_pty);
    c2->cd(3);
    fh_reco_signal_pty->Draw("COLZ");
    draw_eff_pty(fh_reco_signal_pty, fh_mc_signal_pty);
    c2->cd(4);
    fh_chi_prim_signal_pty->Draw("COLZ");
    draw_eff_pty(fh_chi_prim_signal_pty, fh_mc_signal_pty);
    c2->cd(5);
    fh_rich_id_signal_pty->Draw("COLZ");
    draw_eff_pty(fh_rich_id_signal_pty, fh_mc_signal_pty);
    c2->cd(6);
    fh_trd_id_signal_pty->Draw("COLZ");
    draw_eff_pty(fh_trd_id_signal_pty, fh_mc_signal_pty);
    c2->cd(7);
    fh_tof_id_signal_pty->Draw("COLZ");
    draw_eff_pty(fh_tof_id_signal_pty, fh_mc_signal_pty);
    c2->cd(8);
    fh_gammacut_signal_pty->Draw("COLZ");
    draw_eff_pty(fh_gammacut_signal_pty, fh_mc_signal_pty);
    c2->cd(9);
    fh_stcut_signal_pty->Draw("COLZ");
    draw_eff_pty(fh_stcut_signal_pty, fh_mc_signal_pty);
    c2->cd(10);
    fh_ttcut_signal_pty->Draw("COLZ");
    draw_eff_pty(fh_ttcut_signal_pty, fh_mc_signal_pty);
    c2->cd(11);
    fh_ptcut_signal_pty->Draw("COLZ");
    draw_eff_pty(fh_ptcut_signal_pty, fh_mc_signal_pty);
    c2->cd(12);
    fh_anglecut_signal_pty->Draw("COLZ");
    draw_eff_pty(fh_anglecut_signal_pty, fh_mc_signal_pty);
/*    c2->cd(13);
   fh_apmcut_signal_pty->Draw("COLZ");
    draw_eff_pty(fh_apmcut_signal_pty, fh_mc_signal_pty);
*/


//pty efficiency of signal
    TCanvas *c3 = new TCanvas("c3-pty eff","c3-pty eff",1000,750);
    c3->Divide(4,3);
    c3->cd(1);
    TH2D* fh_acc_signal_pty_eff = divideHisto2D(fh_acc_signal_pty, fh_mc_signal_pty);
    fh_acc_signal_pty_eff->Draw("COLZ");
    draw_eff_pty(fh_acc_signal_pty, fh_mc_signal_pty);

    c3->cd(2);
    TH2D* fh_reco_signal_pty_eff = divideHisto2D(fh_reco_signal_pty, fh_acc_signal_pty);
    fh_reco_signal_pty_eff->Draw("COLZ");
    draw_eff_pty(fh_reco_signal_pty, fh_acc_signal_pty);

    c3->cd(3);
    TH2D* fh_chi_prim_signal_pty_eff = divideHisto2D(fh_chi_prim_signal_pty, fh_reco_signal_pty);
    fh_chi_prim_signal_pty_eff->Draw("COLZ");
    draw_eff_pty(fh_chi_prim_signal_pty, fh_reco_signal_pty);

    c3->cd(4);
    TH2D* fh_rich_id_signal_pty_eff = divideHisto2D(fh_rich_id_signal_pty, fh_chi_prim_signal_pty);
    fh_rich_id_signal_pty_eff->Draw("COLZ");
    draw_eff_pty(fh_rich_id_signal_pty, fh_chi_prim_signal_pty);

    c3->cd(5);
    TH2D* fh_trd_id_signal_pty_eff = divideHisto2D(fh_trd_id_signal_pty, fh_rich_id_signal_pty);
    fh_trd_id_signal_pty_eff->Draw("COLZ");
    draw_eff_pty(fh_trd_id_signal_pty, fh_rich_id_signal_pty);

    c3->cd(6);
    TH2D* fh_tof_id_signal_pty_eff = divideHisto2D(fh_tof_id_signal_pty, fh_trd_id_signal_pty);
    fh_tof_id_signal_pty_eff->Draw("COLZ");
    draw_eff_pty(fh_tof_id_signal_pty, fh_trd_id_signal_pty);

    c3->cd(7);
    TH2D* fh_gammacut_signal_pty_eff = divideHisto2D(fh_gammacut_signal_pty, fh_tof_id_signal_pty);
    fh_gammacut_signal_pty_eff->Draw("COLZ");
    draw_eff_pty(fh_gammacut_signal_pty, fh_tof_id_signal_pty);

    c3->cd(8);
    TH2D* fh_stcut_signal_pty_eff = divideHisto2D(fh_stcut_signal_pty, fh_gammacut_signal_pty);
    fh_stcut_signal_pty_eff->Draw("COLZ");
    draw_eff_pty(fh_stcut_signal_pty, fh_gammacut_signal_pty);

    c3->cd(9);
    TH2D* fh_ttcut_signal_pty_eff = divideHisto2D(fh_ttcut_signal_pty, fh_stcut_signal_pty);
    fh_ttcut_signal_pty_eff->Draw("COLZ");
    draw_eff_pty(fh_ttcut_signal_pty, fh_stcut_signal_pty);

    c3->cd(10);
    TH2D* fh_ptcut_signal_pty_eff = divideHisto2D(fh_ptcut_signal_pty, fh_ttcut_signal_pty);
    fh_ptcut_signal_pty_eff->Draw("COLZ");
    draw_eff_pty(fh_ptcut_signal_pty, fh_ttcut_signal_pty);

    c3->cd(11);
    TH2D* fh_anglecut_signal_pty_eff = divideHisto2D(fh_anglecut_signal_pty, fh_ptcut_signal_pty);
    fh_anglecut_signal_pty_eff->Draw("COLZ");
    draw_eff_pty(fh_anglecut_signal_pty, fh_ptcut_signal_pty);

    c3->cd(12);
    //TH2D* fh_apmcut_signal_pty_eff = divideHisto2D(fh_apmcut_signal_pty, fh_anglecut_signal_pty);
    //fh_apmcut_signal_pty_eff->Draw("COLZ");
    //draw_eff_pty(fh_apmcut_signal_pty, fh_anglecut_signal_pty);


//MOMENTUM DISTRIBUTION
    TCanvas *c4 = new TCanvas("c4-mom", "c4-mom", 400, 400);
    fh_mc_signal_mom->SetLineColor(kRed);
    fh_reco_signal_mom->SetLineColor(kBlue);
    fh_chi_prim_signal_mom->SetLineColor(kViolet);
    fh_rich_id_signal_mom->SetLineColor(kGreen);
    fh_trd_id_signal_mom->SetLineColor(kOrange);
    fh_tof_id_signal_mom->SetLineColor(kMagenta);
    fh_gammacut_signal_mom->SetLineColor(kGreen+3);
    fh_stcut_signal_mom->SetLineColor(kOrange-3);
    fh_ttcut_signal_mom->SetLineColor(kYellow+7);
    fh_ptcut_signal_mom->SetLineColor(kPink);
    fh_anglecut_signal_mom->SetLineColor(kYellow);
 //   fh_apmcut_signal_mom->SetLineColor(kCyan+2);
    fh_mc_signal_mom->Draw();
    fh_acc_signal_mom->Draw("same");
    fh_reco_signal_mom->Draw("same");
    fh_chi_prim_signal_mom->Draw("same");
    fh_rich_id_signal_mom->Draw("same");
    fh_trd_id_signal_mom->Draw("same");
    fh_tof_id_signal_mom->Draw("same");
    fh_gammacut_signal_mom->Draw("same");
    fh_stcut_signal_mom->Draw("same");
    fh_ttcut_signal_mom->Draw("same");
    fh_ptcut_signal_mom->Draw("same");
    fh_anglecut_signal_mom->Draw("same");
//    fh_apmcut_signal_mom->Draw("same");

    TLegend* leg3 = new TLegend(0.65,0.4,1., 1.);
    leg3->AddEntry(fh_mc_signal_mom, "mc", "l");
    leg3->AddEntry(fh_acc_signal_mom, "acc", "l");
    leg3->AddEntry(fh_reco_signal_mom, "rec", "l");
    leg3->AddEntry(fh_chi_prim_signal_mom, "chi prim", "l");
    leg3->AddEntry(fh_rich_id_signal_mom, "rich id", "l");
    leg3->AddEntry(fh_trd_id_signal_mom, "trd id", "l");
    leg3->AddEntry(fh_tof_id_signal_mom, "tof id", "l");
    leg3->AddEntry(fh_gammacut_signal_mom, "gamma cut", "l");
    leg3->AddEntry(fh_stcut_signal_mom, "st cut", "l");
    leg3->AddEntry(fh_ttcut_signal_mom, "tt cut", "l");
    leg3->AddEntry(fh_ptcut_signal_mom, "pt cut", "l");
    leg3->AddEntry(fh_anglecut_signal_mom, "angle cut", "l"); 
//    leg3->AddEntry(fh_apmcut_signal_mom, "apm cut", "l");

    leg3->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);


//EFFICIENCY vs. MOMENTUM 
    TCanvas *c5 = new TCanvas("c5-mom eff","c5-mom eff", 1000, 750);
    c5->Divide(4,3);
    c5->cd(1);
    TH1D* fh_acc_signal_mom_eff = divideHisto1D(fh_acc_signal_mom,fh_mc_signal_mom);
    fh_acc_signal_mom_eff->Draw();
    draw_eff_mom(fh_acc_signal_mom,fh_mc_signal_mom);
    gPad->SetGridx(true);
    gPad->SetGridy(true);

    c5->cd(2);
    TH1D* fh_reco_signal_mom_eff = divideHisto1D(fh_reco_signal_mom,fh_acc_signal_mom);
    fh_reco_signal_mom_eff->Draw();
    draw_eff_mom(fh_reco_signal_mom,fh_acc_signal_mom);
    gPad->SetGridx(true);
    gPad->SetGridy(true);

    c5->cd(3);
    TH1D* fh_chi_prim_signal_mom_eff = divideHisto1D(fh_chi_prim_signal_mom,fh_reco_signal_mom);
    fh_chi_prim_signal_mom_eff->Draw();
    draw_eff_mom(fh_chi_prim_signal_mom,fh_reco_signal_mom);
    gPad->SetGridx(true);
    gPad->SetGridy(true);

    c5->cd(4);
    TH1D* fh_rich_id_signal_mom_eff = divideHisto1D(fh_rich_id_signal_mom,fh_chi_prim_signal_mom);
    fh_rich_id_signal_mom_eff->Draw();
    draw_eff_mom(fh_rich_id_signal_mom,fh_chi_prim_signal_mom);
    gPad->SetGridx(true);
    gPad->SetGridy(true);

    c5->cd(5);
    TH1D* fh_trd_id_signal_mom_eff = divideHisto1D(fh_trd_id_signal_mom,fh_rich_id_signal_mom);
    fh_trd_id_signal_mom_eff->Draw();
    draw_eff_mom(fh_trd_id_signal_mom,fh_rich_id_signal_mom);
    gPad->SetGridx(true);
    gPad->SetGridy(true);

    c5->cd(6);
    TH1D* fh_tof_id_signal_mom_eff = divideHisto1D(fh_tof_id_signal_mom,fh_trd_id_signal_mom);
    fh_tof_id_signal_mom_eff->Draw();
    draw_eff_mom(fh_tof_id_signal_mom,fh_trd_id_signal_mom);
    gPad->SetGridx(true);
    gPad->SetGridy(true);

    c5->cd(7);
    TH1D* fh_gammacut_signal_mom_eff = divideHisto1D(fh_gammacut_signal_mom,fh_tof_id_signal_mom);
    fh_gammacut_signal_mom_eff->Draw();
    draw_eff_mom(fh_gammacut_signal_mom,fh_tof_id_signal_mom);
    gPad->SetGridx(true);
    gPad->SetGridy(true);

    c5->cd(8);
    TH1D* fh_stcut_signal_mom_eff = divideHisto1D(fh_stcut_signal_mom,fh_gammacut_signal_mom);
    fh_stcut_signal_mom_eff->Draw();
    draw_eff_mom(fh_stcut_signal_mom,fh_gammacut_signal_mom);
    gPad->SetGridx(true);
    gPad->SetGridy(true);

    c5->cd(9);
    TH1D* fh_ttcut_signal_mom_eff = divideHisto1D(fh_ttcut_signal_mom,fh_stcut_signal_mom);
    fh_ttcut_signal_mom_eff->Draw();
    draw_eff_mom(fh_ttcut_signal_mom,fh_stcut_signal_mom);
    gPad->SetGridx(true);
    gPad->SetGridy(true);

    c5->cd(10);
    TH1D* fh_ptcut_signal_mom_eff = divideHisto1D(fh_ptcut_signal_mom,fh_ttcut_signal_mom);
    fh_ptcut_signal_mom_eff->Draw();
    draw_eff_mom(fh_ptcut_signal_mom,fh_ttcut_signal_mom);
    gPad->SetGridx(true);
    gPad->SetGridy(true);

    c5->cd(11);
    TH1D* fh_anglecut_signal_mom_eff = divideHisto1D(fh_anglecut_signal_mom,fh_ptcut_signal_mom);
    fh_anglecut_signal_mom_eff->Draw();
    draw_eff_mom(fh_anglecut_signal_mom,fh_ptcut_signal_mom);
    gPad->SetGridx(true);
    gPad->SetGridy(true);

    c5->cd(12);
 //   TH1D* fh_apmcut_signal_mom_eff = divideHisto1D(fh_apmcut_signal_mom,fh_anglecut_signal_mom);
 //   fh_apmcut_signal_mom_eff->Draw();
 //   draw_eff_mom(fh_apmcut_signal_mom,fh_anglecut_signal_mom);
 //   gPad->SetGridx(true);
 //   gPad->SetGridy(true);
 //   gPad->SetLogy(true);


//MOTHER PDG
    TCanvas *c6 = new TCanvas("c6-mother pdg", "c6-mother pdg", 500, 500);
    fh_mc_mother_pdg->Draw();
    fh_acc_mother_pdg->SetLineColor(kRed);
    fh_acc_mother_pdg->Draw("same");
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);

//ANALYSIS CUTS
    TCanvas *c7 = new TCanvas("c7-cuts", "c7-cuts", 1000, 1000);
    c7->Divide(4,3);
    c7->cd(1);
    Double_t scaleSig = 1./fh_chi2_prim_signal->Integral();
    Double_t scaleBg = 1./fh_chi2_prim_bg->Integral();
    fh_chi2_prim_signal->SetLineColor(kRed);
    fh_chi2_prim_bg->SetLineColor(kBlue);
    fh_chi2_prim_signal->Draw();
    fh_chi2_prim_bg->Draw("same");
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);
    c7->cd(2);
    TH1D* fh_sig_chi2_prim = CalculateSignificance(fh_chi2_prim_signal, fh_chi2_prim_bg,
        "chi2_prim_significance", "significance", "forward");
    fh_sig_chi2_prim->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    fh_chi2_prim_signal->Scale(scaleSig);
    fh_chi2_prim_bg->Scale(scaleBg);

    c7->cd(3);
    Double_t scaleSig = 1./fh_pt_signal->Integral();
    Double_t scaleBg = 1./fh_pt_bg->Integral();
    fh_pt_signal->SetLineColor(kRed);
    fh_pt_bg->SetLineColor(kBlue);
    fh_pt_signal->Draw();
    fh_pt_bg->Draw("same");
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);
    c7->cd(4);
    TH1D* fh_sig_pt = CalculateSignificance(fh_pt_signal, fh_pt_bg , "pt_significance", "significance", "back");
    fh_sig_pt->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    fh_pt_signal->Scale(scaleSig);
    fh_pt_bg->Scale(scaleBg);

    c7->cd(5);
    Double_t scaleSig = 1./fh_position_signal->Integral();
    Double_t scaleBg = 1./fh_position_bg->Integral();
    fh_position_signal->SetLineColor(kRed);
    fh_position_bg->SetLineColor(kBlue);
    fh_position_signal->Draw();
    fh_position_bg->Draw("same");
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);
    c7->cd(6);
    TH1D* fh_sig_position = CalculateSignificance(fh_position_signal, fh_position_bg, "position_significance", "significance", "back");
    fh_sig_position->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true); 
    fh_position_signal->Scale(scaleSig);
    fh_position_bg->Scale(scaleBg);

    c7->cd(7);
    Double_t scaleSig = 1./fh_angle_signal->Integral();
    Double_t scaleBg = 1./fh_angle_bg->Integral();
    fh_angle_signal->SetLineColor(kRed);
    fh_angle_bg->SetLineColor(kBlue);
    fh_angle_signal->Draw();
    fh_angle_bg->Draw("same");
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);
    c7->cd(8);
    TH1D* fh_sig_angle = CalculateSignificance(fh_angle_signal,fh_angle_bg, "angle_significance", "significance", "back");
    fh_sig_angle->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    fh_angle_signal->Scale(scaleSig);
    fh_angle_bg->Scale(scaleBg);

    c7->cd(9);
    Double_t scaleSig = 1./fh_mom_signal->Integral();
    Double_t scaleBg = 1./fh_mom_bg->Integral();
    fh_mom_signal->SetLineColor(kRed);
    fh_mom_bg->SetLineColor(kBlue);
    fh_mom_signal->Draw();
    fh_mom_bg->Draw("same"); 
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);
    c7->cd(10); 
    TH1D* fh_sig_mom = CalculateSignificance(fh_mom_signal,fh_mom_bg, "momentum_significance", "significance", "back");
    fh_sig_mom->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    fh_mom_signal->Scale(scaleSig);
    fh_mom_bg->Scale(scaleBg);

//    c7->cd(11);
//    Double_t scaleSig = 1./fh_chi2sts_signal->Integral();
//    Double_t scaleBg = 1./fh_chi2sts_bg->Integral();
//    fh_chi2sts_signal->SetLineColor(kRed);
//    fh_chi2sts_bg->SetLineColor(kBlue);
//    fh_chi2sts_signal->Draw();
//    fh_chi2sts_bg->Draw("same");
//    gPad->SetGridx(true);
//    gPad->SetGridy(true);
//    gPad->SetLogy(true);
//    c7->cd(12);
//    TH1D* fh_sig_chi2sts = CalculateSignificance(fh_chi2sts_signal,fh_chi2sts_bg, "chi2sts_significance", "significance", "forward");
//    fh_sig_chi2sts->Draw();
 //   gPad->SetGridx(true);
 //   gPad->SetGridy(true);
 //   fh_chi2sts_signal->Scale(scaleSig);
 //   fh_chi2sts_bg->Scale(scaleBg);

//SOURCE OF BG CANDIDATES
    Double_t sumofbins1 =0;
    Double_t sumofbins2 =0;
    Double_t sumofbins3 =0;
    Double_t sumofbins4 =0;
    Double_t sumofbins5 =0;
    Double_t sumofbins6 =0;
    Double_t sumofbins7 =0;
    Double_t sumofbins8 =0;
    Double_t sumofbins9 =0;
    Double_t sumofbins10 =0;
    Double_t sumofbins11 =0;
    for (int x = 1; x <= 3; x++){
        for (int y =1;y<=3; y++) {
          sumofbins1 += fh_source_pair_reco->GetBinContent(x,y);
          sumofbins2 += fh_source_pair_rich_id->GetBinContent(x,y);
          sumofbins3 += fh_source_pair_trd_id->GetBinContent(x,y);
          sumofbins4 += fh_source_pair_tof_id->GetBinContent(x,y);
          sumofbins5 += fh_source_pair_chi_prim->GetBinContent(x,y);
          sumofbins6 += fh_source_pair_ptcut->GetBinContent(x,y);
          sumofbins7 += fh_source_pair_anglecut->GetBinContent(x,y);
          sumofbins8 += fh_source_pair_gammacut->GetBinContent(x,y);
          sumofbins9 += fh_source_pair_ttcut->GetBinContent(x,y);
   //       sumofbins10 += fh_source_pair_apmcut->GetBinContent(x,y);
          sumofbins11 += fh_source_pair_stcut->GetBinContent(x,y);
        }
    }
    if (sumofbins1 != 0) fh_source_pair_reco->Scale(100. * 1./sumofbins1);
    if (sumofbins2 != 0) fh_source_pair_rich_id->Scale(100. * 1./sumofbins2);
    if (sumofbins3 != 0) fh_source_pair_trd_id->Scale(100. * 1./sumofbins3);
    if (sumofbins4 != 0) fh_source_pair_tof_id->Scale(100. * 1./sumofbins4);
    if (sumofbins5 != 0) fh_source_pair_chi_prim->Scale(100. * 1./sumofbins5);
    if (sumofbins6 != 0) fh_source_pair_ptcut->Scale(100. * 1./sumofbins6);
    if (sumofbins7 != 0) fh_source_pair_anglecut->Scale(100. * 1./sumofbins7);
    if (sumofbins8 != 0) fh_source_pair_gammacut->Scale(100. * 1./sumofbins8);
    if (sumofbins9 != 0) fh_source_pair_ttcut->Scale(100. * 1./sumofbins9);
 //   if (sumofbins10 != 0) fh_source_pair_apmcut->Scale(100. * 1./sumofbins10);
    if (sumofbins11 != 0) fh_source_pair_stcut->Scale(100. * 1./sumofbins11);

    TCanvas *c8 = new TCanvas("c8-pair sources", "c8-pair sources", 1000, 750);
    c8->Divide(4,3);
    c8->cd(1);
    fh_source_pair_reco->Draw("COLZ");
    c8->cd(2);
    fh_source_pair_chi_prim->Draw("COLZ");
    c8->cd(3);
    fh_source_pair_rich_id->Draw("COLZ");
    c8->cd(4);
    fh_source_pair_trd_id->Draw("COLZ");
    c8->cd(5);
    fh_source_pair_tof_id->Draw("COLZ");
    c8->cd(6);
    fh_source_pair_gammacut->Draw("COLZ");
    c8->cd(7);
    fh_source_pair_stcut->Draw("COLZ");
    c8->cd(8);
    fh_source_pair_ttcut->Draw("COLZ");
    c8->cd(9);
    fh_source_pair_ptcut->Draw("COLZ");
    c8->cd(10);
    fh_source_pair_anglecut->Draw("COLZ");
    c8->cd(11);
  // fh_source_pair_apmcut->Draw("COLZ");


//AP CUT DISTRIBUTION
    TCanvas *c9 = new TCanvas("c9-apcut", "c9-apcut", 800, 800);
    c9->Divide(2,2);
    c9->cd(1); 
    fh_apcut_signal->Draw("COLZ");
    TEllipse el1(0.5,0.5,0.2,0.3);
    el1.SetFillStyle(0);
    el1.SetLineWidth(3);
    el1.DrawEllipse(0.,0.,1.,0.45,0.,180.,0.);
    c9->cd(2); 
    fh_apcut_bg->Draw("COLZ");
    TEllipse el2(0.5,0.5,0.2,0.3);
    el2.SetFillStyle(0);
    el2.SetLineWidth(3);
    el2.DrawEllipse(0.,0.,1.,0.45,0.,180.,0.);
    c9->cd(3);
    fh_apcut_pi0->Draw("COLZ");
    c9->cd(4);
    fh_apcut_gamma->Draw("COLZ");


//APM CUT DISTRIBUTION
    TCanvas *c10 = new TCanvas("c10-apmcut", "c10-apmcut", 800, 800);
    c10->Divide(2,2);
    c10->cd(1);
    fh_apmcut_signal->Draw("COLZ");
    c10->cd(2); 
    fh_apmcut_bg->Draw("COLZ");
    c10->cd(3);
    TH1D* apmcut_py_s = fh_apmcut_signal->ProjectionY();
    TH1D* apmcut_py_bg = fh_apmcut_bg->ProjectionY();
    Double_t scaleSig = 1./apmcut_py_s->Integral();
    Double_t scaleBg = 1./apmcut_py_bg->Integral();
    apmcut_py_s->SetLineColor(kRed);
    apmcut_py_bg->SetLineColor(kBlue);
    apmcut_py_s->Draw();
    apmcut_py_bg->Draw("same");
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);
    c10->cd(4);
    TH1D* fh_sig_apmcut_py = CalculateSignificance(apmcut_py_s, apmcut_py_bg, 
        "apmcut_py_significance", "significance", "back");
    fh_sig_apmcut_py->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    apmcut_py_s->Scale(scaleSig);
    apmcut_py_bg->Scale(scaleBg);


//TRACK TOPOLOGY CUT DISTRIBUTION FOR SEGMENT TRACKS
    TCanvas *c11 = new TCanvas("c11-stcut", "c11-stcut", 1200, 600);
    c11->Divide(4,2);
    c11->cd(1);
    fh_stcut_signal->Draw("COLZ");
    c11->cd(2);
    fh_stcut_bg->Draw("COLZ");
    c11->cd(3);
    fh_stcut_pi0->Draw("COLZ");
    c11->cd(4);
    fh_stcut_gamma->Draw("COLZ");
    c11->cd(5);
    TH1D* stcut_px_s = fh_stcut_signal->ProjectionX();
    TH1D* stcut_px_bg = fh_stcut_bg->ProjectionX();
    Double_t scaleSig = 1./stcut_px_s->Integral();
    Double_t scaleBg = 1./stcut_px_bg->Integral();
    stcut_px_s->SetLineColor(kRed);
    stcut_px_bg->SetLineColor(kBlue);
    stcut_px_s->Draw();
    stcut_px_bg->Draw("same");
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);
    c11->cd(6);
    TH1D* fh_sig_stcut_px = CalculateSignificance(stcut_px_s, stcut_px_bg, 
        "stcut_px_significance", "significance", "back");
    fh_sig_stcut_px->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    stcut_px_s->Scale(scaleSig);
    stcut_px_bg->Scale(scaleBg);

    c11->cd(7);
    TH1D* stcut_py_s = fh_stcut_signal->ProjectionY();
    TH1D* stcut_py_bg = fh_stcut_bg->ProjectionY();
    Double_t scaleSig = 1./stcut_py_s->Integral();
    Double_t scaleBg = 1./stcut_py_bg->Integral();
    stcut_py_s->SetLineColor(kRed);
    stcut_py_bg->SetLineColor(kBlue);
    stcut_py_s->Draw();
    stcut_py_bg->Draw("same");
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);
    stcut_py_s->Scale(scaleSig);
    stcut_py_bg->Scale(scaleBg);

    c11->cd(8);
    TH1D* fh_sig_stcut_py = CalculateSignificance(stcut_py_s, stcut_py_bg, 
        "stcut_py_significance", "significance", "back");
    fh_sig_stcut_py->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);


//TRACK TOPOLOGY CUT DISTRIBUTION FOR FUL RECO TRACKS
    TCanvas *c12 = new TCanvas("c12-ttcut", "c12-ttcut", 1200, 600);
    c12->Divide(4,2);
    c12->cd(1);
    fh_ttcut_signal->Draw("COLZ");
    c12->cd(2);
    fh_ttcut_bg->Draw("COLZ");
    c12->cd(3);
    fh_ttcut_pi0->Draw("COLZ");
    c12->cd(4);
    fh_ttcut_gamma->Draw("COLZ");
    c12->cd(5);
    TH1D* ttcut_px_s = fh_ttcut_signal->ProjectionX();
    TH1D* ttcut_px_bg = fh_ttcut_bg->ProjectionX();
    Double_t scaleSig = 1./ttcut_px_s->Integral();
    Double_t scaleBg = 1./ttcut_px_bg->Integral();
    ttcut_px_s->SetLineColor(kRed);
    ttcut_px_bg->SetLineColor(kBlue);
    ttcut_px_s->Draw();
    ttcut_px_bg->Draw("same");
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);
    c12->cd(6);
    TH1D* fh_sig_ttcut_px = CalculateSignificance(ttcut_px_s, ttcut_px_bg, 
        "ttcut_px_significance", "significance", "back");
    fh_sig_ttcut_px->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    ttcut_px_s->Scale(scaleSig);
    ttcut_px_bg->Scale(scaleBg);

    c12->cd(7);
    TH1D* ttcut_py_s = fh_ttcut_signal->ProjectionY();
    TH1D* ttcut_py_bg = fh_ttcut_bg->ProjectionY();
    Double_t scaleSig = 1./ttcut_py_s->Integral();
    Double_t scaleBg = 1./ttcut_py_bg->Integral();
    ttcut_py_s->SetLineColor(kRed);
    ttcut_py_bg->SetLineColor(kBlue);
    ttcut_py_s->Draw();
    ttcut_py_bg->Draw("same");
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);
    ttcut_py_s->Scale(scaleSig);
    ttcut_py_bg->Scale(scaleBg);

    c12->cd(8);
    TH1D* fh_sig_ttcut_py = CalculateSignificance(ttcut_py_s, ttcut_py_bg, 
        "ttcut_py_significance", "significance", "back");
    fh_sig_ttcut_py->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);


//    TCanvas *c8 = new TCanvas("c8","c8", 1200, 400);
//    c8->Divide(3,1);
//    c8->cd(1);
//    fh_mc_vertex_gamma_xz->Draw("COLZ");
//    c8->cd(2);
//    fh_mc_vertex_gamma_yz->Draw("COLZ");
//    c8->cd(3);
//    fh_mc_vertex_gamma_xy->Draw("COLZ");


//ID CUTS DISTRIBUTION
    TCanvas *c13 = new TCanvas("c13-id-cuts","c13-id-cuts",1200, 600);
    c13->Divide(4,2);
    c13->cd(1);
    Double_t scaleSig = 1./fh_rich_ann_signal->Integral();
    Double_t scaleBg = 1./fh_rich_ann_bg->Integral();
    fh_rich_ann_signal->SetLineColor(kRed);
    fh_rich_ann_bg->SetLineColor(kBlue);
    fh_rich_ann_signal->Draw();
    fh_rich_ann_bg->Draw("same");
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);
    c13->cd(2);
    TH1D* fh_sig_rich_ann = CalculateSignificance(fh_rich_ann_signal, fh_rich_ann_bg, 
        "rich_ann_significance", "significance", "back");
    fh_sig_rich_ann->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    fh_rich_ann_signal->Scale(scaleSig);
    fh_rich_ann_bg->Scale(scaleBg);

    c13->cd(3);
    Double_t scaleSig = 1./fh_trd_ann_signal->Integral();
    Double_t scaleBg = 1./fh_trd_ann_bg->Integral();
    fh_trd_ann_signal->SetLineColor(kRed);
    fh_trd_ann_bg->SetLineColor(kBlue);
    fh_trd_ann_signal->Draw();
    fh_trd_ann_bg->Draw("same");
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);
    c13->cd(4);
    TH1D* fh_sig_trd_ann = CalculateSignificance(fh_trd_ann_signal, fh_trd_ann_bg, 
        "trd_ann_significance", "significance", "back");
    fh_sig_trd_ann->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    fh_trd_ann_signal->Scale(scaleSig);
    fh_trd_ann_bg->Scale(scaleBg);

    c13->cd(5);
    fh_tof_m2_signal->Draw("COLZ");
    gPad->SetLogz(true);

    c13->cd(6);
    fh_tof_m2_bg->Draw("COLZ");
    gPad->SetLogz(true);

    TCanvas *c14 = new TCanvas("c14-dsts", "c14-dsts", 900, 600);
    c14->Divide(3,2);
    c14->cd(1);
    fh_dsts_signal->Draw("COLZ");
    c14->cd(2); 
    fh_dsts_bg->Draw("COLZ");
    c14->cd(3);
    fh_dsts_gamma->Draw("COLZ");
    c14->cd(4);
    fh_dsts_pi0->Draw("COLZ");
    c14->cd(5);
    fh_dsts_eta->Draw("COLZ");

//INVARIANT MASS DISTRIBUTION
    Int_t nRebin = 10;
    TCanvas *c15 = new TCanvas("c15-minv", "c15-minv", 1200, 600);
    c15->Divide(2,1);
    c15->cd(1);

    fh_mc_signal_minv->Rebin(nRebin);
    fh_acc_signal_minv->Rebin(nRebin);
    fh_rec_signal_minv->Rebin(nRebin);
    fh_chi_prim_signal_minv->Rebin(nRebin);
    fh_rich_id_signal_minv->Rebin(nRebin);
    fh_trd_id_signal_minv->Rebin(nRebin);
    fh_tof_id_signal_minv->Rebin(nRebin);
    fh_gammacut_signal_minv->Rebin(nRebin);
    fh_stcut_signal_minv->Rebin(nRebin);
    fh_ttcut_signal_minv->Rebin(nRebin);
    fh_ptcut_signal_minv->Rebin(nRebin);
    fh_anglecut_signal_minv->Rebin(nRebin);
//   fh_apmcut_signal_minv->Rebin(nRebin);

    fh_mc_signal_minv->SetLineColor(kGreen+3);
    fh_acc_signal_minv->SetLineColor(kOrange+3);
    fh_chi_prim_signal_minv->SetLineColor(kOrange+7);
    fh_rich_id_signal_minv->SetLineColor(kRed);
    fh_trd_id_signal_minv->SetLineColor(kBlue);
    fh_tof_id_signal_minv->SetLineColor(kGreen);
    fh_gammacut_signal_minv->SetLineColor(kPink-6);
    fh_stcut_signal_minv->SetLineColor(kOrange-3);
    fh_ttcut_signal_minv->SetLineColor(kYellow+1);
    fh_ptcut_signal_minv->SetLineColor(kMagenta);
    fh_anglecut_signal_minv->SetLineColor(kViolet+10);
 //   fh_apmcut_signal_minv->SetLineColor(kPink+8);

    fh_mc_signal_minv->Draw();
    fh_acc_signal_minv->Draw("same");
    fh_rec_signal_minv->Draw("same");
    fh_chi_prim_signal_minv->Draw("same");
    fh_rich_id_signal_minv->Draw("same");
    fh_trd_id_signal_minv->Draw("same");
    fh_tof_id_signal_minv->Draw("same");
    fh_gammacut_signal_minv->Draw("same");
    fh_stcut_signal_minv->Draw("same");
    fh_ttcut_signal_minv->Draw("same");
    fh_ptcut_signal_minv->Draw("same");
    fh_anglecut_signal_minv->Draw("same");
 //   fh_apmcut_signal_minv->Draw("same");

    TLegend* leg3 = new TLegend(0.65,0.6,1., 1.);
    leg3->AddEntry(fh_mc_signal_minv, "mc", "l");
    leg3->AddEntry(fh_acc_signal_minv, "acc", "l");
    leg3->AddEntry(fh_rec_signal_minv, "rec", "l");
    leg3->AddEntry(fh_chi_prim_signal_minv, "chi prim", "l");
    leg3->AddEntry(fh_rich_id_signal_minv, "rich id", "l");
    leg3->AddEntry(fh_trd_id_signal_minv, "trd id", "l");
    leg3->AddEntry(fh_tof_id_signal_minv, "tof id", "l");
    leg3->AddEntry(fh_gammacut_signal_minv, "gamma cut", "l");
    leg3->AddEntry(fh_stcut_signal_minv, "st cut", "l");
    leg3->AddEntry(fh_ttcut_signal_minv, "tt cut", "l");
    leg3->AddEntry(fh_ptcut_signal_minv, "pt cut", "l");
    leg3->AddEntry(fh_anglecut_signal_minv, "angle cut", "l");
 //   leg3->AddEntry(fh_apmcut_signal_minv, "apm cut", "l");

    leg3->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);

    c15->cd(2);
    fh_rich_id_bg_minv->SetLineColor(kRed);
    fh_trd_id_bg_minv->SetLineColor(kBlue);
    fh_tof_id_bg_minv->SetLineColor(kGreen);
    fh_chi_prim_bg_minv->SetLineColor(kOrange+7);
    fh_gammacut_bg_minv->SetLineColor(kPink-6);
    fh_stcut_bg_minv->SetLineColor(kOrange-3);
    fh_ttcut_bg_minv->SetLineColor(kYellow+1);
    fh_ptcut_bg_minv->SetLineColor(kMagenta);
    fh_anglecut_bg_minv->SetLineColor(kViolet+10);
 //   fh_apmcut_bg_minv->SetLineColor(kPink+8);

    fh_rec_bg_minv->Rebin(nRebin);
    fh_chi_prim_bg_minv->Rebin(nRebin);
    fh_rich_id_bg_minv->Rebin(nRebin);
    fh_trd_id_bg_minv->Rebin(nRebin);
    fh_tof_id_bg_minv->Rebin(nRebin);
    fh_gammacut_bg_minv->Rebin(nRebin);
    fh_stcut_bg_minv->Rebin(nRebin);
    fh_ttcut_bg_minv->Rebin(nRebin);
    fh_ptcut_bg_minv->Rebin(nRebin);
    fh_anglecut_bg_minv->Rebin(nRebin);
 //   fh_apmcut_bg_minv->Rebin(nRebin);

    fh_rec_bg_minv->Draw();
    fh_rec_bg_minv->SetMinimum(1e-8);
    fh_chi_prim_bg_minv->Draw("same");
    fh_rich_id_bg_minv->Draw("same");
    fh_trd_id_bg_minv->Draw("same");
    fh_tof_id_bg_minv->Draw("same");
    fh_gammacut_bg_minv->Draw("same");
    fh_stcut_bg_minv->Draw("same");
    fh_ttcut_bg_minv->Draw("same");
    fh_ptcut_bg_minv->Draw("same");
    fh_anglecut_bg_minv->Draw("same");
//    fh_apmcut_bg_minv->Draw("same");

    TLegend* leg = new TLegend(0.65,0.6,1., 1.);
    leg->AddEntry(fh_rec_bg_minv, "rec", "l");
    leg->AddEntry(fh_chi_prim_bg_minv, "chi prim", "l");
    leg->AddEntry(fh_rich_id_bg_minv, "rich id", "l");
    leg->AddEntry(fh_trd_id_bg_minv, "trd id", "l");
    leg->AddEntry(fh_tof_id_bg_minv, "tof id", "l");
    leg->AddEntry(fh_gammacut_bg_minv, "gamma cut", "l");
    leg->AddEntry(fh_stcut_bg_minv, "st cut", "l");
    leg->AddEntry(fh_ttcut_bg_minv, "tt cut", "l");
    leg->AddEntry(fh_ptcut_bg_minv, "pt cut", "l");
    leg->AddEntry(fh_anglecut_bg_minv, "angle cut", "l");
//    leg->AddEntry(fh_apmcut_bg_minv, "apm cut", "l");

    leg->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);

//INVARIANT MASS DISTRIBUTION
	TCanvas *c16 = new TCanvas("c16-minv", "c16-minv", 1200, 900);
	c16->Divide(4,3);
	c16->cd(1);
	draw_minv(fh_rec_signal_minv,fh_rec_bg_minv);
    c16->cd(2);
    draw_minv(fh_chi_prim_signal_minv,fh_chi_prim_bg_minv);
	c16->cd(3);
	draw_minv(fh_rich_id_signal_minv,fh_rich_id_bg_minv);
	c16->cd(4);
	draw_minv(fh_trd_id_signal_minv,fh_trd_id_bg_minv);
	c16->cd(5);
	draw_minv(fh_tof_id_signal_minv,fh_tof_id_bg_minv);
	c16->cd(6);
	draw_minv(fh_gammacut_signal_minv,fh_gammacut_bg_minv);
	c16->cd(7);
	draw_minv(fh_stcut_signal_minv,fh_stcut_bg_minv);
	c16->cd(8);
	draw_minv(fh_ttcut_signal_minv,fh_ttcut_bg_minv);
	c16->cd(9);
	draw_minv(fh_ptcut_signal_minv,fh_ptcut_bg_minv);
	c16->cd(10);
	draw_minv(fh_anglecut_signal_minv,fh_anglecut_bg_minv);
    c16->cd(11);
//    draw_minv(fh_apmcut_signal_minv,fh_apmcut_bg_minv);

//INVARIANT MASS DISTRIBUTION FOR PI0 AND ETA
    TCanvas *c17 = new TCanvas("c17-minv-pi0-eta", "c17-minv-pi0-eta", 1200, 600);
    c17->Divide(2,1);
    c17->cd(1);
    fh_rich_id_pi0_minv->SetLineColor(kRed);
    fh_trd_id_pi0_minv->SetLineColor(kBlue);
    fh_tof_id_pi0_minv->SetLineColor(kGreen);
    fh_chi_prim_pi0_minv->SetLineColor(kOrange+7);
    fh_gammacut_pi0_minv->SetLineColor(kPink-6);
    fh_stcut_pi0_minv->SetLineColor(kOrange-3);
    fh_ttcut_pi0_minv->SetLineColor(kYellow+1);
    fh_ptcut_pi0_minv->SetLineColor(kMagenta);
    fh_anglecut_pi0_minv->SetLineColor(kViolet+10);
 //   fh_apmcut_pi0_minv->SetLineColor(kPink+8);

    fh_rec_pi0_minv->Rebin(nRebin);
    fh_rich_id_pi0_minv->Rebin(nRebin);
    fh_trd_id_pi0_minv->Rebin(nRebin);
    fh_tof_id_pi0_minv->Rebin(nRebin);
    fh_chi_prim_pi0_minv->Rebin(nRebin);
    fh_gammacut_pi0_minv->Rebin(nRebin);
    fh_stcut_pi0_minv->Rebin(nRebin);
    fh_ttcut_pi0_minv->Rebin(nRebin);
    fh_ptcut_pi0_minv->Rebin(nRebin);
    fh_anglecut_pi0_minv->Rebin(nRebin);
 //   fh_apmcut_pi0_minv->Rebin(nRebin);

    fh_rec_pi0_minv->Draw();
    fh_chi_prim_pi0_minv->Draw("same");
    fh_rich_id_pi0_minv->Draw("same");
    fh_trd_id_pi0_minv->Draw("same");
    fh_tof_id_pi0_minv->Draw("same");
    fh_gammacut_pi0_minv->Draw("same");
    fh_stcut_pi0_minv->Draw("same");
    fh_ttcut_pi0_minv->Draw("same");
    fh_ptcut_pi0_minv->Draw("same");
    fh_anglecut_pi0_minv->Draw("same");
 //   fh_apmcut_pi0_minv->Draw("same");

    TLegend* leg3 = new TLegend(0.65,0.6,1., 1.);
    leg3->AddEntry(fh_rec_pi0_minv, "rec", "l");
    leg3->AddEntry(fh_chi_prim_pi0_minv, "chi prim", "l");
	leg3->AddEntry(fh_rich_id_pi0_minv, "rich id", "l");
	leg3->AddEntry(fh_trd_id_pi0_minv, "trd id", "l");
	leg3->AddEntry(fh_tof_id_pi0_minv, "tof id", "l");
    leg3->AddEntry(fh_gammacut_pi0_minv, "gamma cut", "l");
    leg3->AddEntry(fh_stcut_pi0_minv, "st cut", "l");
    leg3->AddEntry(fh_ttcut_pi0_minv, "tt cut", "l");
	leg3->AddEntry(fh_ptcut_pi0_minv, "pt cut", "l");
    leg3->AddEntry(fh_anglecut_pi0_minv, "angle cut", "l");
//    leg3->AddEntry(fh_apmcut_pi0_minv, "apm cut", "l");
	leg3->Draw();
	gPad->SetGridx(true);
	gPad->SetGridy(true);
	gPad->SetLogy(true);

	c17->cd(2);
	fh_rich_id_eta_minv->SetLineColor(kRed);
	fh_trd_id_eta_minv->SetLineColor(kBlue);
	fh_tof_id_eta_minv->SetLineColor(kGreen);
	fh_chi_prim_eta_minv->SetLineColor(kOrange+7);
	fh_gammacut_eta_minv->SetLineColor(kPink-6);
    fh_stcut_eta_minv->SetLineColor(kOrange-3);
	fh_ttcut_eta_minv->SetLineColor(kYellow+1); 
    fh_ptcut_eta_minv->SetLineColor(kMagenta);
    fh_anglecut_eta_minv->SetLineColor(kViolet+10);
//	fh_apmcut_eta_minv->SetLineColor(kPink+8);
    
    fh_rec_eta_minv->Rebin(nRebin);
    fh_rich_id_eta_minv->Rebin(nRebin);
    fh_trd_id_eta_minv->Rebin(nRebin);
    fh_tof_id_eta_minv->Rebin(nRebin);
    fh_chi_prim_eta_minv->Rebin(nRebin);
    fh_gammacut_eta_minv->Rebin(nRebin);
    fh_stcut_eta_minv->Rebin(nRebin);
    fh_ttcut_eta_minv->Rebin(nRebin);
    fh_ptcut_eta_minv->Rebin(nRebin);
    fh_anglecut_eta_minv->Rebin(nRebin);
//  fh_apmcut_eta_minv->Rebin(nRebin);

	fh_rec_eta_minv->Draw();
    fh_chi_prim_eta_minv->Draw("same");
	fh_rich_id_eta_minv->Draw("same");
	fh_trd_id_eta_minv->Draw("same");
	fh_tof_id_eta_minv->Draw("same");
	fh_gammacut_eta_minv->Draw("same");
    fh_stcut_eta_minv->Draw("same");
	fh_ttcut_eta_minv->Draw("same");
    fh_ptcut_eta_minv->Draw("same");
    fh_anglecut_eta_minv->Draw("same");
//	fh_apmcut_eta_minv->Draw("same");

	TLegend* leg4 = new TLegend(0.65,0.6,1., 1.);
	leg4->AddEntry(fh_rec_eta_minv, "rec", "l");
    leg4->AddEntry(fh_chi_prim_eta_minv, "chi prim", "l");
	leg4->AddEntry(fh_rich_id_eta_minv, "rich id", "l");
	leg4->AddEntry(fh_trd_id_eta_minv, "trd id", "l");
	leg4->AddEntry(fh_tof_id_eta_minv, "tof id", "l");
    leg4->AddEntry(fh_gammacut_eta_minv, "gamma cut", "l");
    leg4->AddEntry(fh_stcut_eta_minv, "st cut", "l");
    leg4->AddEntry(fh_ttcut_eta_minv, "tt cut", "l");
	leg4->AddEntry(fh_ptcut_eta_minv, "pt cut", "l");
    leg4->AddEntry(fh_anglecut_eta_minv, "angle cut", "l");
 //   leg4->AddEntry(fh_apmcut_eta_minv, "apm cut", "l");
	leg4->Draw();
	gPad->SetGridx(true);
	gPad->SetGridy(true);
	gPad->SetLogy(true);

}