#include "../../../littrack/utils/CbmLitDrawHist.cxx"
#include "../../../littrack/utils/CbmLitUtils.cxx"

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
    TH1D* h3 = new TH1D(h1->GetName(), h1->GetTitle(), nBins, min, max);
   // h1
   // h2->Sumw2();
    h3->Divide(h1, h2);
   // h3->Sumw2();
    return h3;

}

void calculateSignalOverBg(TH1D* s, TH1D* bg)
{
    //if (signal == rho0)
    Int_t nBins = s->GetNbinsX();
    Double_t max = -1.;
    Double_t max1 = -10.;
    Int_t maxind1 = 0;
    Int_t maxind = 0;
    for (Int_t i = 1; i <= nBins; i++){
       Double_t value = s->GetBinContent(i);
       if (value > max) {
            max = value;
            maxind = i;
    
        }
    }
    for (Int_t i = 1; i <= nBins; i++){
       Double_t value = s->GetBinContent(i);
       if (value > max1 && i != maxind) {
            max1 = value;
            maxind1 = i;
        }
    }

    Double_t val = bg->GetBinContent(maxind);
    Double_t val1 = bg->GetBinContent(maxind1);

    Double_t sbg = (max+max1)/(val+val1);
    cout << s->GetName() << " - S/BG = " << sbg << ", BG/S = " << 1./sbg<<endl;
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



void draw(){
    TFile *file = new TFile("/lustre/cbm/user/ebelolap/oct10/urqmd_rho0/25gev/100_field/real/mytask.analysis.all.01_02_2011.root");
   //TString signalSt = "rho0"; //rho0, phi, omega
    gStyle->SetHistLineWidth(3);
	//SetStyles();
    gROOT->SetStyle("Plain");
    gStyle->SetPalette(1,0);

// pty distribution of signal
    TCanvas *c1 = new TCanvas("c1-pty","c1-pty",1000,1000);
    c1->Divide(4,4);
    c1->cd(1);
    fh_mc_signal_pty->Draw("COLZ");
    draw_eff_pty(fh_mc_signal_pty, fh_mc_signal_pty);
    c1->cd(2);
    fh_acc_signal_pty->Draw("COLZ");
    draw_eff_pty(fh_acc_signal_pty, fh_mc_signal_pty);
    c1->cd(3);
    fh_reco_signal_pty->Draw("COLZ");
    draw_eff_pty(fh_reco_signal_pty, fh_mc_signal_pty);
    c1->cd(4);
    fh_rich_id_signal_pty->Draw("COLZ");
    draw_eff_pty(fh_rich_id_signal_pty, fh_mc_signal_pty);
    c1->cd(5);
    fh_trd_id_signal_pty->Draw("COLZ");
    draw_eff_pty(fh_trd_id_signal_pty, fh_mc_signal_pty);
    c1->cd(6);
    fh_tof_id_signal_pty->Draw("COLZ");
    draw_eff_pty(fh_tof_id_signal_pty, fh_mc_signal_pty);
    c1->cd(7);
    fh_chi_prim_signal_pty->Draw("COLZ");
    draw_eff_pty(fh_chi_prim_signal_pty, fh_mc_signal_pty);
    c1->cd(8);
    fh_gammacut_signal_pty->Draw("COLZ");
    draw_eff_pty(fh_gammacut_signal_pty, fh_mc_signal_pty);
    c1->cd(9);
    fh_stcut_signal_pty->Draw("COLZ");
    draw_eff_pty(fh_stcut_signal_pty, fh_mc_signal_pty);
    c1->cd(10);
    fh_ttcut_signal_pty->Draw("COLZ");
    draw_eff_pty(fh_ttcut_signal_pty, fh_mc_signal_pty);
    c1->cd(11);
    fh_apcut_signal_pty->Draw("COLZ");
    draw_eff_pty(fh_apcut_signal_pty, fh_mc_signal_pty);
    c1->cd(12);
    fh_ptcut_signal_pty->Draw("COLZ");
    draw_eff_pty(fh_ptcut_signal_pty, fh_mc_signal_pty);
    c1->cd(13);
    fh_anglecut_signal_pty->Draw("COLZ");
    draw_eff_pty(fh_anglecut_signal_pty, fh_mc_signal_pty);

    
//pty efficiency of signal
    TCanvas *c1_1 = new TCanvas("c1_1-pty eff","c1_1-pty eff",1000,750);
    c1_1->Divide(4,3);
    c1_1->cd(1);
    TH2D* fh_acc_signal_pty_eff = divideHisto2D(fh_acc_signal_pty, fh_mc_signal_pty);
    fh_acc_signal_pty_eff->Draw("COLZ");
    draw_eff_pty(fh_acc_signal_pty, fh_mc_signal_pty);

    c1_1->cd(2);
    TH2D* fh_reco_signal_pty_eff = divideHisto2D(fh_reco_signal_pty, fh_acc_signal_pty);
    fh_reco_signal_pty_eff->Draw("COLZ");
    draw_eff_pty(fh_reco_signal_pty, fh_acc_signal_pty);

    c1_1->cd(3);
    TH2D* fh_rich_id_signal_pty_eff = divideHisto2D(fh_rich_id_signal_pty, fh_reco_signal_pty);
    fh_rich_id_signal_pty_eff->Draw("COLZ");
    draw_eff_pty(fh_rich_id_signal_pty, fh_reco_signal_pty);

    c1_1->cd(4);
    TH2D* fh_trd_id_signal_pty_eff = divideHisto2D(fh_trd_id_signal_pty, fh_rich_id_signal_pty);
    fh_trd_id_signal_pty_eff->Draw("COLZ");
    draw_eff_pty(fh_trd_id_signal_pty, fh_rich_id_signal_pty);

    c1_1->cd(5);
    TH2D* fh_tof_id_signal_pty_eff = divideHisto2D(fh_tof_id_signal_pty, fh_trd_id_signal_pty);
    fh_tof_id_signal_pty_eff->Draw("COLZ");
    draw_eff_pty(fh_tof_id_signal_pty, fh_trd_id_signal_pty);

    c1_1->cd(6);
    TH2D* fh_chi_prim_signal_pty_eff = divideHisto2D(fh_chi_prim_signal_pty, fh_tof_id_signal_pty);
    fh_chi_prim_signal_pty_eff->Draw("COLZ");
    draw_eff_pty(fh_chi_prim_signal_pty, fh_tof_id_signal_pty);

    c1_1->cd(7);
    TH2D* fh_gammacut_signal_pty_eff = divideHisto2D(fh_gammacut_signal_pty, fh_tof_id_signal_pty);
    fh_gammacut_signal_pty_eff->Draw("COLZ");
    draw_eff_pty(fh_gammacut_signal_pty, fh_tof_id_signal_pty);

    c1_1->cd(8);
    TH2D* fh_stcut_signal_pty_eff = divideHisto2D(fh_stcut_signal_pty, fh_gammacut_signal_pty);
    fh_stcut_signal_pty_eff->Draw("COLZ");
    draw_eff_pty(fh_stcut_signal_pty, fh_gammacut_signal_pty);

    c1_1->cd(9);
    TH2D* fh_ttcut_signal_pty_eff = divideHisto2D(fh_ttcut_signal_pty, fh_stcut_signal_pty);
    fh_ttcut_signal_pty_eff->Draw("COLZ");
    draw_eff_pty(fh_ttcut_signal_pty, fh_stcut_signal_pty);

    c1_1->cd(10);
    TH2D* fh_apcut_signal_pty_eff = divideHisto2D(fh_apcut_signal_pty, fh_ttcut_signal_pty);
    fh_apcut_signal_pty_eff->Draw("COLZ");
    draw_eff_pty(fh_apcut_signal_pty, fh_ttcut_signal_pty);

    c1_1->cd(11);
    TH2D* fh_ptcut_signal_pty_eff = divideHisto2D(fh_ptcut_signal_pty, fh_apcut_signal_pty);
    fh_ptcut_signal_pty_eff->Draw("COLZ");
    draw_eff_pty(fh_ptcut_signal_pty, fh_apcut_signal_pty);

    c1_1->cd(12);
    TH2D* fh_anglecut_signal_pty_eff = divideHisto2D(fh_anglecut_signal_pty, fh_ptcut_signal_pty);
    fh_anglecut_signal_pty_eff->Draw("COLZ");
    draw_eff_pty(fh_anglecut_signal_pty, fh_ptcut_signal_pty);
    

    TCanvas *c2 = new TCanvas("c2-mom", "c2-mom", 600, 600);
    c2->Divide(2,2);
    c2->cd(1);
    fh_mc_signal_mom->SetLineColor(kRed);
    fh_reco_signal_mom->SetLineColor(kBlue);
    fh_rich_id_signal_mom->SetLineColor(kGreen);
    fh_trd_id_signal_mom->SetLineColor(kOrange);
    fh_tof_id_signal_mom->SetLineColor(kMagenta);
    fh_chi_prim_signal_mom->SetLineColor(kViolet);
    fh_ptcut_signal_mom->SetLineColor(kPink);
    fh_anglecut_signal_mom->SetLineColor(kYellow);
    fh_gammacut_signal_mom->SetLineColor(kGreen+3);
    fh_stcut_signal_mom->SetLineColor(kOrange-3);
    fh_ttcut_signal_mom->SetLineColor(kYellow+7);
    fh_apcut_signal_mom->SetLineColor(kCyan+2);
    fh_mc_signal_mom->Draw();
    fh_acc_signal_mom->Draw("same");
    fh_reco_signal_mom->Draw("same");
    fh_rich_id_signal_mom->Draw("same");
    fh_trd_id_signal_mom->Draw("same");
    fh_tof_id_signal_mom->Draw("same");
    fh_chi_prim_signal_mom->Draw("same");
    fh_gammacut_signal_mom->Draw("same");
    fh_stcut_signal_mom->Draw("same");
    fh_ttcut_signal_mom->Draw("same");
    fh_anglecut_signal_mom->Draw("same");
    fh_apcut_signal_mom->Draw("same");
    fh_ptcut_signal_mom->Draw("same");
    
    TLegend* leg3 = new TLegend(0.65,0.6,1., 1.);
    leg3->AddEntry(fh_mc_signal_mom, "mc", "l");
    leg3->AddEntry(fh_acc_signal_mom, "acc", "l");
    leg3->AddEntry(fh_reco_signal_mom, "rec", "l");
    leg3->AddEntry(fh_rich_id_signal_mom, "rich id", "l");
    leg3->AddEntry(fh_trd_id_signal_mom, "trd id", "l");
    leg3->AddEntry(fh_tof_id_signal_mom, "tof id", "l");
    leg3->AddEntry(fh_chi_prim_signal_mom, "chi prim", "l");
    leg3->AddEntry(fh_gammacut_signal_mom, "gamma cut", "l");
    leg3->AddEntry(fh_stcut_signal_mom, "st cut", "l");
    leg3->AddEntry(fh_ttcut_signal_mom, "tt cut", "l");
    leg3->AddEntry(fh_apcut_signal_mom, "ap cut", "l");
    leg3->AddEntry(fh_ptcut_signal_mom, "pt cut", "l");
    leg3->AddEntry(fh_anglecut_signal_mom, "angle cut", "l"); 

    leg3->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);
    c2->cd(2);
    fh_rec_mc_mom_signal->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);
    c2->cd(3);
    fh_mom_res_vs_mom_signal->Draw("COLZ");
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    c2->cd(4);
    fh_mean_mom_vs_mom_signal->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);

    TCanvas *c2_1 = new TCanvas("c2_1-mom eff","c2_1-mom eff", 1000, 750);
    c2_1->Divide(4,3);
    c2_1->cd(1);
    TH1D* fh_acc_signal_mom_eff = divideHisto1D(fh_acc_signal_mom,fh_mc_signal_mom);
    fh_acc_signal_mom_eff->Draw();
    draw_eff_mom(fh_acc_signal_mom,fh_mc_signal_mom);
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    //gPad->SetLogy(true);

    c2_1->cd(2);
    TH1D* fh_reco_signal_mom_eff = divideHisto1D(fh_reco_signal_mom,fh_acc_signal_mom);
    fh_reco_signal_mom_eff->Draw();
    draw_eff_mom(fh_reco_signal_mom,fh_acc_signal_mom);
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    //gPad->SetLogy(true);

    c2_1->cd(3);
    TH1D* fh_rich_id_signal_mom_eff = divideHisto1D(fh_rich_id_signal_mom,fh_reco_signal_mom);
    fh_rich_id_signal_mom_eff->Draw();
    draw_eff_mom(fh_rich_id_signal_mom,fh_reco_signal_mom);
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    //gPad->SetLogy(true);

    c2_1->cd(4);
    TH1D* fh_trd_id_signal_mom_eff = divideHisto1D(fh_trd_id_signal_mom,fh_rich_id_signal_mom);
    fh_trd_id_signal_mom_eff->Draw();
    draw_eff_mom(fh_trd_id_signal_mom,fh_rich_id_signal_mom);
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    //gPad->SetLogy(true);

    c2_1->cd(5);
    TH1D* fh_tof_id_signal_mom_eff = divideHisto1D(fh_tof_id_signal_mom,fh_trd_id_signal_mom);
    fh_tof_id_signal_mom_eff->Draw();
    draw_eff_mom(fh_tof_id_signal_mom,fh_trd_id_signal_mom);
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    //gPad->SetLogy(true);

    c2_1->cd(6);
    TH1D* fh_chi_prim_signal_mom_eff = divideHisto1D(fh_chi_prim_signal_mom,fh_tof_id_signal_mom);
    fh_chi_prim_signal_mom_eff->Draw();
    draw_eff_mom(fh_chi_prim_signal_mom,fh_tof_id_signal_mom);
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    //gPad->SetLogy(true);

    c2_1->cd(7);
    TH1D* fh_gammacut_signal_mom_eff = divideHisto1D(fh_gammacut_signal_mom,fh_chi_prim_signal_mom);
    fh_gammacut_signal_mom_eff->Draw();
    draw_eff_mom(fh_gammacut_signal_mom,fh_chi_prim_signal_mom);
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    //gPad->SetLogy(true);

    c2_1->cd(8);
    TH1D* fh_stcut_signal_mom_eff = divideHisto1D(fh_stcut_signal_mom,fh_gammacut_signal_mom);
    fh_stcut_signal_mom_eff->Draw();
    draw_eff_mom(fh_stcut_signal_mom,fh_gammacut_signal_mom);
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    //gPad->SetLogy(true);

    c2_1->cd(9);
    TH1D* fh_ttcut_signal_mom_eff = divideHisto1D(fh_ttcut_signal_mom,fh_stcut_signal_mom);
    fh_ttcut_signal_mom_eff->Draw();
    draw_eff_mom(fh_ttcut_signal_mom,fh_stcut_signal_mom);
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    //gPad->SetLogy(true);

    c2_1->cd(10);
    TH1D* fh_apcut_signal_mom_eff = divideHisto1D(fh_apcut_signal_mom,fh_ttcut_signal_mom);
    fh_apcut_signal_mom_eff->Draw();
    draw_eff_mom(fh_apcut_signal_mom,fh_ttcut_signal_mom);
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    //gPad->SetLogy(true);
   
    c2_1->cd(11);
    TH1D* fh_ptcut_signal_mom_eff = divideHisto1D(fh_ptcut_signal_mom,fh_apcut_signal_mom);
    fh_ptcut_signal_mom_eff->Draw();
    draw_eff_mom(fh_ptcut_signal_mom,fh_apcut_signal_mom);
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    //gPad->SetLogy(true);

    c2_1->cd(12);
    TH1D* fh_anglecut_signal_mom_eff = divideHisto1D(fh_anglecut_signal_mom,fh_ptcut_signal_mom);
    fh_anglecut_signal_mom_eff->Draw();
    draw_eff_mom(fh_anglecut_signal_mom,fh_ptcut_signal_mom);
    gPad->SetGridx(true);
    gPad->SetGridy(true);
   //gPad->SetLogy(true);
    

//mother PDG
    TCanvas *c3 = new TCanvas("c3-mother pdg", "c3-mother pdg", 500, 500);
    fh_mc_mother_pdg->Draw();
    fh_acc_mother_pdg->SetLineColor(kRed);
    fh_acc_mother_pdg->Draw("same");
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);

// analysis cuts
    TCanvas *c4 = new TCanvas("c4-cuts", "c4-cuts", 1200, 800);
    c4->Divide(4,3);
    c4->cd(1);
    Double_t scaleSig = 1./fh_chi2_prim_signal->Integral();
    Double_t scaleBg = 1./fh_chi2_prim_bg->Integral();
    fh_chi2_prim_signal->SetLineColor(kRed);  
    fh_chi2_prim_bg->SetLineColor(kBlue);
    fh_chi2_prim_signal->Draw();
    fh_chi2_prim_bg->Draw("same");
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);
    c4->cd(2);
    TH1D* fh_sig_chi2_prim = CalculateSignificance(fh_chi2_prim_signal, fh_chi2_prim_bg, 
        "chi2_prim_significance", "significance", "forward");
    fh_sig_chi2_prim->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);  
    fh_chi2_prim_signal->Scale(scaleSig);
    fh_chi2_prim_bg->Scale(scaleBg);

    c4->cd(3);
    Double_t scaleSig = 1./fh_pt_signal->Integral();
    Double_t scaleBg = 1./fh_pt_bg->Integral();
    fh_pt_signal->SetLineColor(kRed);
    fh_pt_bg->SetLineColor(kBlue);
    fh_pt_signal->Draw();
    fh_pt_bg->Draw("same");
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);
    c4->cd(4);
    TH1D* fh_sig_pt = CalculateSignificance(fh_pt_signal, fh_pt_bg , "pt_significance", "significance", "back");
    fh_sig_pt->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    fh_pt_signal->Scale(scaleSig);
    fh_pt_bg->Scale(scaleBg);

    c4->cd(5);
    Double_t scaleSig = 1./fh_position_signal->Integral();
    Double_t scaleBg = 1./fh_position_bg->Integral();
    fh_position_signal->SetLineColor(kRed);
    fh_position_bg->SetLineColor(kBlue);
    fh_position_signal->Draw();
    fh_position_bg->Draw("same");
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);
    c4->cd(6);
    TH1D* fh_sig_position = CalculateSignificance(fh_position_signal, fh_position_bg, "position_significance", "significance", "back");
    fh_sig_position->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true); 
    fh_position_signal->Scale(scaleSig);
    fh_position_bg->Scale(scaleBg);

    c4->cd(7);
    Double_t scaleSig = 1./fh_angle_signal->Integral();
    Double_t scaleBg = 1./fh_angle_bg->Integral();
    fh_angle_signal->SetLineColor(kRed);
    fh_angle_bg->SetLineColor(kBlue);
    fh_angle_signal->Draw();
    fh_angle_bg->Draw("same");
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);
    c4->cd(8);
    TH1D* fh_sig_angle = CalculateSignificance(fh_angle_signal,fh_angle_bg, "angle_significance", "significance", "back");
    fh_sig_angle->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    fh_angle_signal->Scale(scaleSig);
    fh_angle_bg->Scale(scaleBg);

    c4->cd(9); 
    Double_t scaleSig = 1./fh_mom_signal->Integral();
    Double_t scaleBg = 1./fh_mom_bg->Integral();
    fh_mom_signal->SetLineColor(kRed);
    fh_mom_bg->SetLineColor(kBlue);
    fh_mom_signal->Draw();
    fh_mom_bg->Draw("same"); 
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);
    c4->cd(10); 
    TH1D* fh_sig_mom = CalculateSignificance(fh_mom_signal,fh_mom_bg, "momentum_significance", "significance", "back");
    fh_sig_mom->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    fh_mom_signal->Scale(scaleSig);
    fh_mom_bg->Scale(scaleBg);

//source of BG candidates
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
          sumofbins10 += fh_source_pair_apcut->GetBinContent(x,y);
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
    if (sumofbins10 != 0) fh_source_pair_apcut->Scale(100. * 1./sumofbins10);
    if (sumofbins11 != 0) fh_source_pair_stcut->Scale(100. * 1./sumofbins11);
    TCanvas *c10 = new TCanvas("c10", "c10", 1000, 750);
    c10->Divide(4,3);
    c10->cd(1);
    fh_source_pair_reco->Draw("COLZ");
    c10->cd(2);
    fh_source_pair_rich_id->Draw("COLZ");
    c10->cd(3);
    fh_source_pair_trd_id->Draw("COLZ");
    c10->cd(4);
    fh_source_pair_tof_id->Draw("COLZ");
    c10->cd(5);
    fh_source_pair_chi_prim->Draw("COLZ");
    c10->cd(6);
    fh_source_pair_gammacut->Draw("COLZ");
    c10->cd(7);
    fh_source_pair_stcut->Draw("COLZ");
    c10->cd(8);
    fh_source_pair_ttcut->Draw("COLZ");
    c10->cd(9);
    fh_source_pair_apcut->Draw("COLZ");
    c10->cd(10);
    fh_source_pair_ptcut->Draw("COLZ");
    c10->cd(11);
    fh_source_pair_anglecut->Draw("COLZ");
     
//AP cut distribution
    TCanvas *c5 = new TCanvas("c5-apcut", "c5-apcut", 800, 800);
    c5->Divide(2,2);
    c5->cd(1); 
    fh_apcut_signal->Draw("COLZ");
    TEllipse el1(0.5,0.5,0.2,0.3);
    el1.SetFillStyle(0);
    el1.SetLineWidth(3);
    el1.DrawEllipse(0.,0.,1.,0.45,0.,180.,0.);
    c5->cd(2); 
    fh_apcut_bg->Draw("COLZ");
    fh_apcut_signal->Draw("sameCOLZ");
    TEllipse el2(0.5,0.5,0.2,0.3);
    el2.SetFillStyle(0);
    el2.SetLineWidth(3);
    el2.DrawEllipse(0.,0.,1.,0.45,0.,180.,0.);
    c5->cd(3);
    fh_apcut_pi0->Draw("COLZ");
    c5->cd(4);
    fh_apcut_gamma->Draw("COLZ");

//track topology cut distribution for segment tracks
    TCanvas *c6 = new TCanvas("c6-stcut", "c6-stcut", 800, 800);
    c6->Divide(2,2);
    c6->cd(1);
    fh_stcut_signal->Draw("COLZ");
    c6->cd(2);
    fh_stcut_bg->Draw("COLZ");
    c6->cd(3);
    fh_stcut_pi0->Draw("COLZ");
    c6->cd(4);
    fh_stcut_gamma->Draw("COLZ");

//track topology cut distribution for full reco tracks
    TCanvas *c7 = new TCanvas("c7-ttcut", "c7-ttcut", 800, 800);
    c7->Divide(2,2);
    c7->cd(1);
    fh_ttcut_signal->Draw("COLZ");
    c7->cd(2);
    fh_ttcut_bg->Draw("COLZ");
    c7->cd(3);
    fh_ttcut_pi0->Draw("COLZ");
    c7->cd(4);
    fh_ttcut_gamma->Draw("COLZ");

//    TCanvas *c8 = new TCanvas("c8","c8", 1200, 400);
//    c8->Divide(3,1);
//    c8->cd(1);
//    fh_mc_vertex_gamma_xz->Draw("COLZ");
//    c8->cd(2);
//    fh_mc_vertex_gamma_yz->Draw("COLZ");
//    c8->cd(3);
//    fh_mc_vertex_gamma_xy->Draw("COLZ");

//ID cuts distribution
    TCanvas *c9 = new TCanvas("c9-id-cuts","c9-id-cuts",1200, 600);
    c9->Divide(4,2);
    c9->cd(1);
    Double_t scaleSig = 1./fh_rich_ann_signal->Integral();
    Double_t scaleBg = 1./fh_rich_ann_bg->Integral();
    fh_rich_ann_signal->SetLineColor(kRed);
    fh_rich_ann_bg->SetLineColor(kBlue);
    fh_rich_ann_signal->Draw();
    fh_rich_ann_bg->Draw("same");
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);
    c9->cd(2);
    TH1D* fh_sig_rich_ann = CalculateSignificance(fh_rich_ann_signal, fh_rich_ann_bg, 
        "rich_ann_significance", "significance", "back");
    fh_sig_rich_ann->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    fh_rich_ann_signal->Scale(scaleSig);
    fh_rich_ann_bg->Scale(scaleBg);
    
    c9->cd(3);
    Double_t scaleSig = 1./fh_trd_ann_signal->Integral();
    Double_t scaleBg = 1./fh_trd_ann_bg->Integral();
    fh_trd_ann_signal->SetLineColor(kRed);
    fh_trd_ann_bg->SetLineColor(kBlue);
    fh_trd_ann_signal->Draw();
    fh_trd_ann_bg->Draw("same");
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);
    c9->cd(4);
    TH1D* fh_sig_trd_ann = CalculateSignificance(fh_trd_ann_signal, fh_trd_ann_bg, 
        "trd_ann_significance", "significance", "back");
    fh_sig_trd_ann->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    fh_trd_ann_signal->Scale(scaleSig);
    fh_trd_ann_bg->Scale(scaleBg);

    c9->cd(5);
    fh_rich_trd_ann_signal->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);

    c9->cd(6);
    fh_rich_trd_ann_bg->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);

    c9->cd(7);
    fh_tof_m2_signal->Draw("COLZ");
    gPad->SetLogz(true);

    c9->cd(8);
    fh_tof_m2_bg->Draw("COLZ");
    gPad->SetLogz(true);

//invariant mass distribution
    TCanvas *c10 = new TCanvas("c10-minv", "c10-minv", 1200, 600);
    c10->Divide(2,1);
    c10->cd(1); 
    fh_mc_signal_minv->SetLineColor(kGreen+3);
    fh_acc_signal_minv->SetLineColor(kOrange+3);
    fh_mc_signal_minv->Draw();
    fh_acc_signal_minv->Draw("same");
    fh_rich_id_signal_minv->SetLineColor(kRed);
    fh_trd_id_signal_minv->SetLineColor(kBlue);
    fh_tof_id_signal_minv->SetLineColor(kGreen);
    fh_chi_prim_signal_minv->SetLineColor(kOrange+7);    
    fh_ptcut_signal_minv->SetLineColor(kMagenta);
    fh_anglecut_signal_minv->SetLineColor(kViolet+10);
    fh_gammacut_signal_minv->SetLineColor(kPink-6);
    fh_stcut_signal_minv->SetLineColor(kOrange-3);
    fh_ttcut_signal_minv->SetLineColor(kYellow+1);
    fh_apcut_signal_minv->SetLineColor(kPink+8);
    fh_rec_signal_minv->Draw("same");
    fh_rich_id_signal_minv->Draw("same");
    fh_trd_id_signal_minv->Draw("same");
    fh_tof_id_signal_minv->Draw("same");
    fh_chi_prim_signal_minv->Draw("same");
    fh_gammacut_signal_minv->Draw("same");
    fh_stcut_signal_minv->Draw("same");
    fh_ttcut_signal_minv->Draw("same");
    fh_anglecut_signal_minv->Draw("same");
    fh_apcut_signal_minv->Draw("same");
    fh_ptcut_signal_minv->Draw("same");
     
    TLegend* leg3 = new TLegend(0.65,0.6,1., 1.);
    leg3->AddEntry(fh_mc_signal_minv, "mc", "l");
    leg3->AddEntry(fh_acc_signal_minv, "acc", "l");
    leg3->AddEntry(fh_rec_signal_minv, "rec", "l");
    leg3->AddEntry(fh_rich_id_signal_minv, "rich id", "l");
    leg3->AddEntry(fh_trd_id_signal_minv, "trd id", "l");
    leg3->AddEntry(fh_tof_id_signal_minv, "tof id", "l");
    leg3->AddEntry(fh_chi_prim_signal_minv, "chi prim", "l");
    leg3->AddEntry(fh_gammacut_signal_minv, "gamma cut", "l");
    leg3->AddEntry(fh_stcut_signal_minv, "st cut", "l");
    leg3->AddEntry(fh_ttcut_signal_minv, "tt cut", "l");
    leg3->AddEntry(fh_apcut_signal_minv, "ap cut", "l");
    leg3->AddEntry(fh_ptcut_signal_minv, "pt cut", "l");
    leg3->AddEntry(fh_anglecut_signal_minv, "angle cut", "l");
     
    leg3->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);

    c10->cd(2);
    fh_rich_id_bg_minv->SetLineColor(kRed);
    fh_trd_id_bg_minv->SetLineColor(kBlue);
    fh_tof_id_bg_minv->SetLineColor(kGreen);
    fh_chi_prim_bg_minv->SetLineColor(kOrange+7);   
    fh_ptcut_bg_minv->SetLineColor(kMagenta);
    fh_anglecut_bg_minv->SetLineColor(kViolet+10);
    fh_gammacut_bg_minv->SetLineColor(kPink-6);
    fh_ttcut_bg_minv->SetLineColor(kYellow+1);
    fh_stcut_bg_minv->SetLineColor(kOrange-3);
    fh_apcut_bg_minv->SetLineColor(kPink+8);
    fh_rec_bg_minv->Draw();
    fh_rec_bg_minv->SetMinimum(1e-8);
    fh_rich_id_bg_minv->Draw("same");
    fh_trd_id_bg_minv->Draw("same");
    fh_tof_id_bg_minv->Draw("same");
    fh_chi_prim_bg_minv->Draw("same");
    fh_ptcut_bg_minv->Draw("same");
    fh_anglecut_bg_minv->Draw("same");
    fh_gammacut_bg_minv->Draw("same");
    fh_stcut_bg_minv->Draw("same");
    fh_ttcut_bg_minv->Draw("same");
    fh_apcut_bg_minv->Draw("same");

    TLegend* leg = new TLegend(0.65,0.6,1., 1.);
    leg->AddEntry(fh_rec_bg_minv, "rec", "l");
    leg->AddEntry(fh_rich_id_bg_minv, "rich id", "l");
    leg->AddEntry(fh_trd_id_bg_minv, "trd id", "l");
    leg->AddEntry(fh_tof_id_bg_minv, "tof id", "l");
    leg->AddEntry(fh_chi_prim_bg_minv, "chi prim", "l");
    leg->AddEntry(fh_gammacut_bg_minv, "gamma cut", "l");
    leg->AddEntry(fh_stcut_bg_minv, "st cut", "l");
    leg->AddEntry(fh_ttcut_bg_minv, "tt cut", "l");
    leg->AddEntry(fh_apcut_bg_minv, "ap cut", "l");
    leg->AddEntry(fh_ptcut_bg_minv, "pt cut", "l");
    leg->AddEntry(fh_anglecut_bg_minv, "angle cut", "l");
     
    leg->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);


//invariant mass distribution for Pi0 and eta
    TCanvas *c11 = new TCanvas("c11-minv-pi0-eta", "c11-minv-pi0-eta", 1200, 600);
    c11->Divide(2,1);
    c11->cd(1);
    fh_rich_id_pi0_minv->SetLineColor(kRed);
    fh_trd_id_pi0_minv->SetLineColor(kBlue);
    fh_tof_id_pi0_minv->SetLineColor(kGreen);
    fh_chi_prim_pi0_minv->SetLineColor(kOrange+7);
    fh_ptcut_pi0_minv->SetLineColor(kMagenta);
    fh_anglecut_pi0_minv->SetLineColor(kViolet+10);
    fh_gammacut_pi0_minv->SetLineColor(kPink-6);
    fh_stcut_pi0_minv->SetLineColor(kOrange-3);
    fh_ttcut_pi0_minv->SetLineColor(kYellow+1);
    fh_apcut_pi0_minv->SetLineColor(kPink+8);
    fh_rec_pi0_minv->Draw();
    fh_rich_id_pi0_minv->Draw("same");
    fh_trd_id_pi0_minv->Draw("same");
    fh_tof_id_pi0_minv->Draw("same");
    fh_chi_prim_pi0_minv->Draw("same");
    fh_ptcut_pi0_minv->Draw("same");
    fh_anglecut_pi0_minv->Draw("same");
    fh_gammacut_pi0_minv->Draw("same");
    fh_stcut_pi0_minv->Draw("same");
    fh_ttcut_pi0_minv->Draw("same");
    fh_apcut_pi0_minv->Draw("same");
    TLegend* leg3 = new TLegend(0.65,0.6,1., 1.);
    leg3->AddEntry(fh_rec_pi0_minv, "rec", "l");
	leg3->AddEntry(fh_rich_id_pi0_minv, "rich id", "l");
	leg3->AddEntry(fh_trd_id_pi0_minv, "trd id", "l");
	leg3->AddEntry(fh_tof_id_pi0_minv, "tof id", "l");
	leg3->AddEntry(fh_chi_prim_pi0_minv, "chi prim", "l");
    leg3->AddEntry(fh_gammacut_pi0_minv, "gamma cut", "l");
    leg3->AddEntry(fh_stcut_pi0_minv, "st cut", "l");
    leg3->AddEntry(fh_ttcut_pi0_minv, "tt cut", "l");
    leg3->AddEntry(fh_apcut_pi0_minv, "ap cut", "l");
	leg3->AddEntry(fh_ptcut_pi0_minv, "pt cut", "l");
    leg3->AddEntry(fh_anglecut_pi0_minv, "angle cut", "l");
	leg3->Draw();
	gPad->SetGridx(true);
	gPad->SetGridy(true);
	gPad->SetLogy(true);

	c11->cd(2);
	fh_rich_id_eta_minv->SetLineColor(kRed);
	fh_trd_id_eta_minv->SetLineColor(kBlue);
	fh_tof_id_eta_minv->SetLineColor(kGreen);
	fh_chi_prim_eta_minv->SetLineColor(kOrange+7);
	fh_ptcut_eta_minv->SetLineColor(kMagenta);
	fh_anglecut_eta_minv->SetLineColor(kViolet+10);
	fh_gammacut_eta_minv->SetLineColor(kPink-6);
    fh_stcut_eta_minv->SetLineColor(kOrange-3);
	fh_ttcut_eta_minv->SetLineColor(kYellow+1);
	fh_apcut_eta_minv->SetLineColor(kPink+8);
	fh_rec_eta_minv->Draw();
	fh_rich_id_eta_minv->Draw("same");
	fh_trd_id_eta_minv->Draw("same");
	fh_tof_id_eta_minv->Draw("same");
	fh_chi_prim_eta_minv->Draw("same");
	fh_ptcut_eta_minv->Draw("same");
	fh_anglecut_eta_minv->Draw("same");
	fh_gammacut_eta_minv->Draw("same");
    fh_stcut_eta_minv->Draw("same");
	fh_ttcut_eta_minv->Draw("same");
	fh_apcut_eta_minv->Draw("same");
	TLegend* leg4 = new TLegend(0.65,0.6,1., 1.);
	leg4->AddEntry(fh_rec_eta_minv, "rec", "l");
	leg4->AddEntry(fh_rich_id_eta_minv, "rich id", "l");
	leg4->AddEntry(fh_trd_id_eta_minv, "trd id", "l");
	leg4->AddEntry(fh_tof_id_eta_minv, "tof id", "l");
	leg4->AddEntry(fh_chi_prim_eta_minv, "chi prim", "l");
    leg4->AddEntry(fh_gammacut_eta_minv, "gamma cut", "l");
    leg4->AddEntry(fh_stcut_eta_minv, "st cut", "l");
    leg4->AddEntry(fh_ttcut_eta_minv, "tt cut", "l");
    leg4->AddEntry(fh_apcut_eta_minv, "ap cut", "l");
	leg4->AddEntry(fh_ptcut_eta_minv, "pt cut", "l");
    leg4->AddEntry(fh_anglecut_eta_minv, "angle cut", "l");
	leg4->Draw();
	gPad->SetGridx(true);
	gPad->SetGridy(true);
	gPad->SetLogy(true);

    calculateSignalOverBg(fh_rec_signal_minv, fh_rec_bg_minv);
    calculateSignalOverBg(fh_rich_id_signal_minv, fh_rich_id_bg_minv);
    calculateSignalOverBg(fh_trd_id_signal_minv, fh_trd_id_bg_minv);
    calculateSignalOverBg(fh_tof_id_signal_minv, fh_tof_id_bg_minv);
    calculateSignalOverBg(fh_chi_prim_signal_minv, fh_chi_prim_bg_minv);
    calculateSignalOverBg(fh_gammacut_signal_minv, fh_gammacut_bg_minv);
    calculateSignalOverBg(fh_stcut_signal_minv, fh_stcut_bg_minv);
    calculateSignalOverBg(fh_ttcut_signal_minv, fh_ttcut_bg_minv);
    calculateSignalOverBg(fh_apcut_signal_minv, fh_apcut_bg_minv);
    calculateSignalOverBg(fh_ptcut_signal_minv, fh_ptcut_bg_minv);
    calculateSignalOverBg(fh_anglecut_signal_minv, fh_anglecut_bg_minv);

    TCanvas *c12 = new TCanvas("c12-mom-single-track", "c12-mom-single-track", 1200, 1200);
    c12->Divide(4,2);
    c12->cd(1);
    fh_mc_mom->Draw();
    c12->cd(2);
    fh_acc_mom->Draw();
    c12->cd(3);
    fh_sts_reco_signal_mom->Draw();
    c12->cd(4);
    fh_rich_reco_signal_mom->Draw();
    c12->cd(5);
    fh_trd_reco_signal_mom->Draw();
    c12->cd(6);
    fh_tof_reco_signal_mom->Draw();
    c12->cd(7);
    fh_richID_signal_mom->Draw();

    TCanvas *c13 = new TCanvas("c13-mom_efficiency-single-track", "c13-mom_efficiency-single-track",1200,600);
    c13->Divide(4,2);
    c13->cd(1);
    TH1D* fh_acc_mom_eff = divideHisto1D(fh_acc_mom,fh_mc_mom);
    fh_acc_mom_eff->Draw();
    draw_eff_mom(fh_acc_mom,fh_mc_mom);
    gPad->SetGridx(true);
    gPad->SetGridy(true);

    c13->cd(2);
    TH1D* fh_sts_mom_eff = divideHisto1D(fh_sts_reco_signal_mom, fh_acc_mom);
    fh_sts_mom_eff->Draw();
    draw_eff_mom(fh_sts_reco_signal_mom, fh_acc_mom);
    gPad->SetGridx(true);
    gPad->SetGridy(true);

    c13->cd(3);
    TH1D* fh_rich_mom_eff = divideHisto1D(fh_rich_reco_signal_mom, fh_sts_reco_signal_mom);
    fh_rich_mom_eff->Draw();
    draw_eff_mom(fh_rich_reco_signal_mom, fh_sts_reco_signal_mom);
    gPad->SetGridx(true);
    gPad->SetGridy(true);

    c13->cd(4);
    TH1D* fh_trd_mom_eff = divideHisto1D(fh_trd_reco_signal_mom, fh_rich_reco_signal_mom);
    fh_trd_mom_eff->Draw();
    draw_eff_mom(fh_trd_reco_signal_mom, fh_rich_reco_signal_mom);
    gPad->SetGridx(true);
    gPad->SetGridy(true);

    c13->cd(5);
    TH1D* fh_tof_mom_eff = divideHisto1D(fh_tof_reco_signal_mom, fh_trd_reco_signal_mom);
    fh_tof_mom_eff->Draw();
    draw_eff_mom(fh_tof_reco_signal_mom, fh_trd_reco_signal_mom);
    gPad->SetGridx(true);
    gPad->SetGridy(true);

    c13->cd(6);
    TH1D* fh_richID_mom_eff = divideHisto1D(fh_richID_signal_mom, fh_tof_reco_signal_mom);
    fh_richID_mom_eff->Draw();
    draw_eff_mom(fh_richID_signal_mom, fh_tof_reco_signal_mom);
    gPad->SetGridx(true);
    gPad->SetGridy(true);

    c13->cd(7);
    TH1D* fh_trdID_mom_eff = divideHisto1D(fh_trdID_signal_mom, fh_richID_signal_mom);
    fh_trdID_mom_eff->Draw();
    draw_eff_mom(fh_trdID_signal_mom, fh_richID_signal_mom);
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    
    c13->cd(8);
    TH1D* fh_tofID_mom_eff = divideHisto1D(fh_tofID_signal_mom, fh_trdID_signal_mom);
    fh_tofID_mom_eff->Draw();
    draw_eff_mom(fh_tofID_signal_mom, fh_trdID_signal_mom);
    gPad->SetGridx(true);
    gPad->SetGridy(true);

///pi0
    TCanvas *c14 = new TCanvas("c14-mom_efficiency-single-track_pi0", "c14-mom_efficiency-single-track_pi0",900,600);
    c14->Divide(3,2);
    c14->cd(1);
    TH1D* fh_acc_pi0_mom_eff = divideHisto1D(fh_acc_pi0_mom, fh_mc_pi0_mom);
    fh_acc_pi0_mom_eff->Draw();
    draw_eff_mom(fh_acc_pi0_mom, fh_mc_pi0_mom);
    gPad->SetGridx(true);
    gPad->SetGridy(true);

    c14->cd(2);
    TH1D* fh_sts_pi0_mom_eff = divideHisto1D(fh_sts_reco_pi0_mom, fh_acc_pi0_mom);
    fh_sts_pi0_mom_eff->Draw();
    draw_eff_mom(fh_sts_reco_pi0_mom, fh_acc_pi0_mom);
    gPad->SetGridx(true);
    gPad->SetGridy(true);

    c14->cd(3);
    TH1D* fh_rich_pi0_mom_eff = divideHisto1D(fh_rich_reco_pi0_mom, fh_sts_reco_pi0_mom);
    fh_rich_pi0_mom_eff->Draw();
    draw_eff_mom(fh_rich_reco_pi0_mom, fh_sts_reco_pi0_mom);
    gPad->SetGridx(true);
    gPad->SetGridy(true);

    c14->cd(4);
    TH1D* fh_trd_pi0_mom_eff = divideHisto1D(fh_trd_reco_pi0_mom, fh_rich_reco_pi0_mom);
    fh_trd_pi0_mom_eff->Draw();
    draw_eff_mom(fh_trd_reco_pi0_mom, fh_rich_reco_pi0_mom);
    gPad->SetGridx(true);
    gPad->SetGridy(true);

    c14->cd(5);
    TH1D* fh_tof_pi0_mom_eff = divideHisto1D(fh_tof_reco_pi0_mom, fh_trd_reco_pi0_mom);
    fh_tof_pi0_mom_eff->Draw();
    draw_eff_mom(fh_tof_reco_pi0_mom, fh_trd_reco_pi0_mom);
    gPad->SetGridx(true);
    gPad->SetGridy(true);


/// gamma
    TCanvas *c15 = new TCanvas("c15-mom_efficiency-single-track_gamma", "c15-mom_efficiency-single-track_gamma",900,600);
    c15->Divide(3,2);
    c15->cd(1);
    TH1D* fh_acc_gamma_mom_eff = divideHisto1D(fh_acc_gamma_mom, fh_mc_gamma_mom);
    fh_acc_gamma_mom_eff->Draw();
    draw_eff_mom(fh_acc_gamma_mom, fh_mc_gamma_mom);
    gPad->SetGridx(true);
    gPad->SetGridy(true);

    c15->cd(2);
    TH1D* fh_sts_gamma_mom_eff = divideHisto1D(fh_sts_reco_gamma_mom, fh_acc_gamma_mom);
    fh_sts_gamma_mom_eff->Draw();
    draw_eff_mom(fh_sts_reco_gamma_mom, fh_acc_gamma_mom);
    gPad->SetGridx(true);
    gPad->SetGridy(true);

    c15->cd(3);
    TH1D* fh_rich_gamma_mom_eff = divideHisto1D(fh_rich_reco_gamma_mom, fh_sts_reco_gamma_mom);
    fh_rich_gamma_mom_eff->Draw();
    draw_eff_mom(fh_rich_reco_gamma_mom, fh_sts_reco_gamma_mom);
    gPad->SetGridx(true);
    gPad->SetGridy(true);

    c15->cd(4);
    TH1D* fh_trd_gamma_mom_eff = divideHisto1D(fh_trd_reco_gamma_mom, fh_rich_reco_gamma_mom);
    fh_trd_gamma_mom_eff->Draw();
    draw_eff_mom(fh_trd_reco_gamma_mom, fh_rich_reco_gamma_mom);
    gPad->SetGridx(true);
    gPad->SetGridy(true);

    c15->cd(5);
    TH1D* fh_tof_gamma_mom_eff = divideHisto1D(fh_tof_reco_gamma_mom, fh_trd_reco_gamma_mom);
    fh_tof_gamma_mom_eff->Draw();
    draw_eff_mom(fh_tof_reco_gamma_mom, fh_trd_reco_gamma_mom);
    gPad->SetGridx(true);
    gPad->SetGridy(true);

///eta
    TCanvas *c16 = new TCanvas("c16-mom_efficiency-single-track_eta", "c16-mom_efficiency-single-track_eta",900,600);
    c16->Divide(3,2);
    c16->cd(1);
    TH1D* fh_acc_eta_mom_eff = divideHisto1D(fh_acc_eta_mom, fh_mc_eta_mom);
    fh_acc_eta_mom_eff->Draw();
    draw_eff_mom(fh_acc_eta_mom, fh_mc_eta_mom);
    gPad->SetGridx(true);
    gPad->SetGridy(true);

    c16->cd(2);
    TH1D* fh_sts_eta_mom_eff = divideHisto1D(fh_sts_reco_eta_mom, fh_acc_eta_mom);
    fh_sts_eta_mom_eff->Draw();
    draw_eff_mom(fh_sts_reco_eta_mom, fh_acc_eta_mom);
    gPad->SetGridx(true);
    gPad->SetGridy(true);

    c16->cd(3);
    TH1D* fh_rich_eta_mom_eff = divideHisto1D(fh_rich_reco_eta_mom, fh_sts_reco_eta_mom);
    fh_rich_eta_mom_eff->Draw();
    draw_eff_mom(fh_rich_reco_eta_mom, fh_sts_reco_eta_mom);
    gPad->SetGridx(true);
    gPad->SetGridy(true);

    c16->cd(4);
    TH1D* fh_trd_eta_mom_eff = divideHisto1D(fh_trd_reco_eta_mom, fh_rich_reco_eta_mom);
    fh_trd_eta_mom_eff->Draw();
    draw_eff_mom(fh_trd_reco_eta_mom, fh_rich_reco_eta_mom);
    gPad->SetGridx(true);
    gPad->SetGridy(true);

    c16->cd(5);
    TH1D* fh_tof_eta_mom_eff = divideHisto1D(fh_tof_reco_eta_mom, fh_trd_reco_eta_mom);
    fh_tof_eta_mom_eff->Draw();
    draw_eff_mom(fh_tof_reco_eta_mom, fh_trd_reco_eta_mom);
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    
}






void cut_eff() {

TFile *file = new TFile("/lustre/cbm/user/ebelolap/oct10/urqmd_rho0/25gev/100_field/real/mytask.analysis.all.new.reco-sts.root");
   //TString signalSt = "rho0"; //rho0, phi, omega
    gStyle->SetHistLineWidth(3);
    //SetStyles();
    gROOT->SetStyle("Plain");
    gStyle->SetPalette(1,0);

Int_t Nbins = fh_tof_id_signal_minv->GetNbinsX();

Double_t sum1_signal_tof = 0;
Double_t sum1_signal_chi_prim  = 0;
Double_t sum1_signal_gamma  = 0;
Double_t sum1_signal_stcut  = 0;
Double_t sum1_signal_ttcut  = 0;
Double_t sum1_signal_apcut  = 0;
Double_t sum1_signal_pt  = 0;
Double_t sum1_signal_angle  = 0;

Double_t sum1_bg_tof = 0;
Double_t sum1_bg_chi_prim  = 0;
Double_t sum1_bg_gamma  = 0;
Double_t sum1_bg_stcut  = 0;
Double_t sum1_bg_ttcut  = 0;
Double_t sum1_bg_apcut  = 0;
Double_t sum1_bg_pt  = 0;
Double_t sum1_bg_angle  = 0;

Double_t sum1_pi0_tof = 0;
Double_t sum1_pi0_chi_prim  = 0;
Double_t sum1_pi0_gamma  = 0;
Double_t sum1_pi0_stcut  = 0;
Double_t sum1_pi0_ttcut  = 0;
Double_t sum1_pi0_apcut  = 0;
Double_t sum1_pi0_pt  = 0;
Double_t sum1_pi0_angle  = 0;

Double_t sum1_eta_tof = 0;
Double_t sum1_eta_chi_prim  = 0;
Double_t sum1_eta_gamma  = 0;
Double_t sum1_eta_stcut  = 0;
Double_t sum1_eta_ttcut  = 0;
Double_t sum1_eta_apcut  = 0;
Double_t sum1_eta_pt  = 0;
Double_t sum1_eta_angle  = 0;

///// 0.2 - 1.

Double_t sum2_signal_tof = 0;
Double_t sum2_signal_chi_prim  = 0;
Double_t sum2_signal_gamma  = 0;
Double_t sum2_signal_stcut  = 0;
Double_t sum2_signal_ttcut  = 0;
Double_t sum2_signal_apcut  = 0;
Double_t sum2_signal_pt  = 0;
Double_t sum2_signal_angle  = 0;

Double_t sum2_bg_tof = 0;
Double_t sum2_bg_chi_prim  = 0;
Double_t sum2_bg_gamma  = 0;
Double_t sum2_bg_stcut  = 0;
Double_t sum2_bg_ttcut  = 0;
Double_t sum2_bg_apcut  = 0;
Double_t sum2_bg_pt  = 0;
Double_t sum2_bg_angle  = 0;

Double_t sum2_pi0_tof = 0;
Double_t sum2_pi0_chi_prim  = 0;
Double_t sum2_pi0_gamma  = 0;
Double_t sum2_pi0_stcut  = 0;
Double_t sum2_pi0_ttcut  = 0;
Double_t sum2_pi0_apcut  = 0;
Double_t sum2_pi0_pt  = 0;
Double_t sum2_pi0_angle  = 0;

Double_t sum2_eta_tof = 0;
Double_t sum2_eta_chi_prim  = 0;
Double_t sum2_eta_gamma  = 0;
Double_t sum2_eta_stcut  = 0;
Double_t sum2_eta_ttcut  = 0;
Double_t sum2_eta_apcut  = 0;
Double_t sum2_eta_pt  = 0;
Double_t sum2_eta_angle  = 0;

/////// 1. - 2.
Double_t sum3_signal_tof = 0;
Double_t sum3_signal_chi_prim  = 0;
Double_t sum3_signal_gamma  = 0;
Double_t sum3_signal_stcut  = 0;
Double_t sum3_signal_ttcut  = 0;
Double_t sum3_signal_apcut  = 0;
Double_t sum3_signal_pt  = 0;
Double_t sum3_signal_angle  = 0;

Double_t sum3_bg_tof = 0;
Double_t sum3_bg_chi_prim  = 0;
Double_t sum3_bg_gamma  = 0;
Double_t sum3_bg_stcut  = 0;
Double_t sum3_bg_ttcut  = 0;
Double_t sum3_bg_apcut  = 0;
Double_t sum3_bg_pt  = 0;
Double_t sum3_bg_angle  = 0;

Double_t sum3_pi0_tof = 0;
Double_t sum3_pi0_chi_prim  = 0;
Double_t sum3_pi0_gamma  = 0;
Double_t sum3_pi0_stcut  = 0;
Double_t sum3_pi0_ttcut  = 0;
Double_t sum3_pi0_apcut  = 0;
Double_t sum3_pi0_pt  = 0;
Double_t sum3_pi0_angle  = 0;

Double_t sum3_eta_tof = 0;
Double_t sum3_eta_chi_prim  = 0;
Double_t sum3_eta_gamma  = 0;
Double_t sum3_eta_stcut  = 0;
Double_t sum3_eta_ttcut  = 0;
Double_t sum3_eta_apcut  = 0;
Double_t sum3_eta_pt  = 0;
Double_t sum3_eta_angle  = 0;

for (Int_t i = 0; i < Nbins; i++){

    Double_t binCenter = fh_tof_id_signal_minv->GetBinCenter(i);
    if (binCenter > 0.0 && binCenter <= 0.2  ) continue;
        {
         sum1_signal_tof += (fh_tof_id_signal_minv->GetBinWidth(i)) * (fh_tof_id_signal_minv->GetBinContent(i));
         sum1_signal_chi_prim += (fh_chi_prim_signal_minv->GetBinWidth(i)) * (fh_chi_prim_signal_minv->GetBinContent(i));
         sum1_signal_gamma += (fh_gammacut_signal_minv->GetBinWidth(i)) * (fh_gammacut_signal_minv->GetBinContent(i)); 
         sum1_signal_stcut += (fh_stcut_signal_minv->GetBinWidth(i)) * (fh_stcut_signal_minv->GetBinContent(i));
         sum1_signal_ttcut += (fh_ttcut_signal_minv->GetBinWidth(i)) * (fh_ttcut_signal_minv->GetBinContent(i));
         sum1_signal_apcut += (fh_apcut_signal_minv->GetBinWidth(i)) * (fh_apcut_signal_minv->GetBinContent(i));
         sum1_signal_pt += (fh_ptcut_signal_minv->GetBinWidth(i)) * (fh_ptcut_signal_minv->GetBinContent(i));
         sum1_signal_angle += (fh_anglecut_signal_minv->GetBinWidth(i)) * (fh_anglecut_signal_minv->GetBinContent(i));

        //// BG
        sum1_bg_tof += (fh_tof_id_bg_minv->GetBinWidth(i)) * (fh_tof_id_bg_minv->GetBinContent(i));
        sum1_bg_chi_prim += (fh_chi_prim_bg_minv->GetBinWidth(i)) * (fh_chi_prim_bg_minv->GetBinContent(i));
        sum1_bg_gamma += (fh_gammacut_bg_minv->GetBinWidth(i)) * (fh_gammacut_bg_minv->GetBinContent(i));
        sum1_bg_stcut += (fh_stcut_bg_minv->GetBinWidth(i)) * (fh_stcut_bg_minv->GetBinContent(i));
        sum1_bg_ttcut += (fh_ttcut_bg_minv->GetBinWidth(i)) * (fh_ttcut_bg_minv->GetBinContent(i));
        sum1_bg_apcut += (fh_apcut_bg_minv->GetBinWidth(i)) * (fh_apcut_bg_minv->GetBinContent(i));
        sum1_bg_pt += (fh_ptcut_bg_minv->GetBinWidth(i)) * (fh_ptcut_bg_minv->GetBinContent(i));
        sum1_bg_angle += (fh_anglecut_bg_minv->GetBinWidth(i)) * (fh_anglecut_bg_minv->GetBinContent(i))

        ////pi0
        sum1_pi0_tof += (fh_tof_id_pi0_minv->GetBinWidth(i)) * (fh_tof_id_pi0_minv->GetBinContent(i));
        sum1_pi0_chi_prim += (fh_chi_prim_pi0_minv->GetBinWidth(i)) * (fh_chi_prim_pi0_minv->GetBinContent(i));
        sum1_pi0_gamma += (fh_gammacut_pi0_minv->GetBinWidth(i)) * (fh_gammacut_pi0_minv->GetBinContent(i));
        sum1_pi0_stcut += (fh_stcut_pi0_minv->GetBinWidth(i)) * (fh_stcut_pi0_minv->GetBinContent(i));
        sum1_pi0_ttcut += (fh_ttcut_pi0_minv->GetBinWidth(i)) * (fh_ttcut_pi0_minv->GetBinContent(i));
        sum1_pi0_apcut += (fh_apcut_pi0_minv->GetBinWidth(i)) * (fh_apcut_pi0_minv->GetBinContent(i));
        sum1_pi0_pt += (fh_ptcut_pi0_minv->GetBinWidth(i)) * (fh_ptcut_pi0_minv->GetBinContent(i));
        sum1_pi0_angle += (fh_anglecut_pi0_minv->GetBinWidth(i)) * (fh_anglecut_pi0_minv->GetBinContent(i));

        ////eta
        sum1_eta_tof += (fh_tof_id_eta_minv->GetBinWidth(i)) * (fh_tof_id_eta_minv->GetBinContent(i));
        sum1_eta_chi_prim += (fh_chi_prim_eta_minv->GetBinWidth(i)) * (fh_chi_prim_eta_minv->GetBinContent(i));
        sum1_eta_gamma += (fh_gammacut_eta_minv->GetBinWidth(i)) * (fh_gammacut_eta_minv->GetBinContent(i));
        sum1_eta_stcut += (fh_stcut_eta_minv->GetBinWidth(i)) * (fh_stcut_eta_minv->GetBinContent(i));
        sum1_eta_ttcut += (fh_ttcut_eta_minv->GetBinWidth(i)) * (fh_ttcut_eta_minv->GetBinContent(i));
        sum1_eta_apcut += (fh_apcut_eta_minv->GetBinWidth(i)) * (fh_apcut_eta_minv->GetBinContent(i));
        sum1_eta_pt += (fh_ptcut_eta_minv->GetBinWidth(i)) * (fh_ptcut_eta_minv->GetBinContent(i));
        sum1_eta_angle += (fh_anglecut_eta_minv->GetBinWidth(i)) * (fh_anglecut_eta_minv->GetBinContent(i));
    }//if

    if (binCenter <= 0.2 || binCenter > 1.) continue;
    {
         sum2_signal_tof += (fh_tof_id_signal_minv->GetBinWidth(i)) * (fh_tof_id_signal_minv->GetBinContent(i));
         sum2_signal_chi_prim += (fh_chi_prim_signal_minv->GetBinWidth(i)) * (fh_chi_prim_signal_minv->GetBinContent(i));
         sum2_signal_gamma += (fh_gammacut_signal_minv->GetBinWidth(i)) * (fh_gammacut_signal_minv->GetBinContent(i)); 
         sum2_signal_stcut += (fh_stcut_signal_minv->GetBinWidth(i)) * (fh_stcut_signal_minv->GetBinContent(i));
         sum2_signal_ttcut += (fh_ttcut_signal_minv->GetBinWidth(i)) * (fh_ttcut_signal_minv->GetBinContent(i));
         sum2_signal_apcut += (fh_apcut_signal_minv->GetBinWidth(i)) * (fh_apcut_signal_minv->GetBinContent(i));
         sum2_signal_pt += (fh_ptcut_signal_minv->GetBinWidth(i)) * (fh_ptcut_signal_minv->GetBinContent(i));
         sum2_signal_angle += (fh_anglecut_signal_minv->GetBinWidth(i)) * (fh_anglecut_signal_minv->GetBinContent(i));

        //// BG
        sum2_bg_tof += (fh_tof_id_bg_minv->GetBinWidth(i)) * (fh_tof_id_bg_minv->GetBinContent(i));
        sum2_bg_chi_prim += (fh_chi_prim_bg_minv->GetBinWidth(i)) * (fh_chi_prim_bg_minv->GetBinContent(i));
        sum2_bg_gamma += (fh_gammacut_bg_minv->GetBinWidth(i)) * (fh_gammacut_bg_minv->GetBinContent(i));
        sum2_bg_stcut += (fh_stcut_bg_minv->GetBinWidth(i)) * (fh_stcut_bg_minv->GetBinContent(i));
        sum2_bg_ttcut += (fh_ttcut_bg_minv->GetBinWidth(i)) * (fh_ttcut_bg_minv->GetBinContent(i));
        sum2_bg_apcut += (fh_apcut_bg_minv->GetBinWidth(i)) * (fh_apcut_bg_minv->GetBinContent(i));
        sum2_bg_pt += (fh_ptcut_bg_minv->GetBinWidth(i)) * (fh_ptcut_bg_minv->GetBinContent(i));
        sum2_bg_angle += (fh_anglecut_bg_minv->GetBinWidth(i)) * (fh_anglecut_bg_minv->GetBinContent(i))

        ////pi0
        sum2_pi0_tof += (fh_tof_id_pi0_minv->GetBinWidth(i)) * (fh_tof_id_pi0_minv->GetBinContent(i));
        sum2_pi0_chi_prim += (fh_chi_prim_pi0_minv->GetBinWidth(i)) * (fh_chi_prim_pi0_minv->GetBinContent(i));
        sum2_pi0_gamma += (fh_gammacut_pi0_minv->GetBinWidth(i)) * (fh_gammacut_pi0_minv->GetBinContent(i));
        sum2_pi0_stcut += (fh_stcut_pi0_minv->GetBinWidth(i)) * (fh_stcut_pi0_minv->GetBinContent(i));
        sum2_pi0_ttcut += (fh_ttcut_pi0_minv->GetBinWidth(i)) * (fh_ttcut_pi0_minv->GetBinContent(i));
        sum2_pi0_apcut += (fh_apcut_pi0_minv->GetBinWidth(i)) * (fh_apcut_pi0_minv->GetBinContent(i));
        sum2_pi0_pt += (fh_ptcut_pi0_minv->GetBinWidth(i)) * (fh_ptcut_pi0_minv->GetBinContent(i));
        sum2_pi0_angle += (fh_anglecut_pi0_minv->GetBinWidth(i)) * (fh_anglecut_pi0_minv->GetBinContent(i));

        ////eta
        sum2_eta_tof += (fh_tof_id_eta_minv->GetBinWidth(i)) * (fh_tof_id_eta_minv->GetBinContent(i));
        sum2_eta_chi_prim += (fh_chi_prim_eta_minv->GetBinWidth(i)) * (fh_chi_prim_eta_minv->GetBinContent(i));
        sum2_eta_gamma += (fh_gammacut_eta_minv->GetBinWidth(i)) * (fh_gammacut_eta_minv->GetBinContent(i));
        sum2_eta_stcut += (fh_stcut_eta_minv->GetBinWidth(i)) * (fh_stcut_eta_minv->GetBinContent(i));
        sum2_eta_ttcut += (fh_ttcut_eta_minv->GetBinWidth(i)) * (fh_ttcut_eta_minv->GetBinContent(i));
        sum2_eta_apcut += (fh_apcut_eta_minv->GetBinWidth(i)) * (fh_apcut_eta_minv->GetBinContent(i));
        sum2_eta_pt += (fh_ptcut_eta_minv->GetBinWidth(i)) * (fh_ptcut_eta_minv->GetBinContent(i));
        sum2_eta_angle += (fh_anglecut_eta_minv->GetBinWidth(i)) * (fh_anglecut_eta_minv->GetBinContent(i));


    }//if

    if (binCenter <= 1. || binCenter > 2.) continue;
    {
         sum3_signal_tof += (fh_tof_id_signal_minv->GetBinWidth(i)) * (fh_tof_id_signal_minv->GetBinContent(i));
         sum3_signal_chi_prim += (fh_chi_prim_signal_minv->GetBinWidth(i)) * (fh_chi_prim_signal_minv->GetBinContent(i));
         sum3_signal_gamma += (fh_gammacut_signal_minv->GetBinWidth(i)) * (fh_gammacut_signal_minv->GetBinContent(i)); 
         sum3_signal_stcut += (fh_stcut_signal_minv->GetBinWidth(i)) * (fh_stcut_signal_minv->GetBinContent(i));
         sum3_signal_ttcut += (fh_ttcut_signal_minv->GetBinWidth(i)) * (fh_ttcut_signal_minv->GetBinContent(i));
         sum3_signal_apcut += (fh_apcut_signal_minv->GetBinWidth(i)) * (fh_apcut_signal_minv->GetBinContent(i));
         sum3_signal_pt += (fh_ptcut_signal_minv->GetBinWidth(i)) * (fh_ptcut_signal_minv->GetBinContent(i));
         sum3_signal_angle += (fh_anglecut_signal_minv->GetBinWidth(i)) * (fh_anglecut_signal_minv->GetBinContent(i));

        //// BG
        sum3_bg_tof += (fh_tof_id_bg_minv->GetBinWidth(i)) * (fh_tof_id_bg_minv->GetBinContent(i));
        sum3_bg_chi_prim += (fh_chi_prim_bg_minv->GetBinWidth(i)) * (fh_chi_prim_bg_minv->GetBinContent(i));
        sum3_bg_gamma += (fh_gammacut_bg_minv->GetBinWidth(i)) * (fh_gammacut_bg_minv->GetBinContent(i));
        sum3_bg_stcut += (fh_stcut_bg_minv->GetBinWidth(i)) * (fh_stcut_bg_minv->GetBinContent(i));
        sum3_bg_ttcut += (fh_ttcut_bg_minv->GetBinWidth(i)) * (fh_ttcut_bg_minv->GetBinContent(i));
        sum3_bg_apcut += (fh_apcut_bg_minv->GetBinWidth(i)) * (fh_apcut_bg_minv->GetBinContent(i));
        sum3_bg_pt += (fh_ptcut_bg_minv->GetBinWidth(i)) * (fh_ptcut_bg_minv->GetBinContent(i));
        sum3_bg_angle += (fh_anglecut_bg_minv->GetBinWidth(i)) * (fh_anglecut_bg_minv->GetBinContent(i))

        ////pi0
        sum3_pi0_tof += (fh_tof_id_pi0_minv->GetBinWidth(i)) * (fh_tof_id_pi0_minv->GetBinContent(i));
        sum3_pi0_chi_prim += (fh_chi_prim_pi0_minv->GetBinWidth(i)) * (fh_chi_prim_pi0_minv->GetBinContent(i));
        sum3_pi0_gamma += (fh_gammacut_pi0_minv->GetBinWidth(i)) * (fh_gammacut_pi0_minv->GetBinContent(i));
        sum3_pi0_stcut += (fh_stcut_pi0_minv->GetBinWidth(i)) * (fh_stcut_pi0_minv->GetBinContent(i));
        sum3_pi0_ttcut += (fh_ttcut_pi0_minv->GetBinWidth(i)) * (fh_ttcut_pi0_minv->GetBinContent(i));
        sum3_pi0_apcut += (fh_apcut_pi0_minv->GetBinWidth(i)) * (fh_apcut_pi0_minv->GetBinContent(i));
        sum3_pi0_pt += (fh_ptcut_pi0_minv->GetBinWidth(i)) * (fh_ptcut_pi0_minv->GetBinContent(i));
        sum3_pi0_angle += (fh_anglecut_pi0_minv->GetBinWidth(i)) * (fh_anglecut_pi0_minv->GetBinContent(i));

        ////eta
        sum3_eta_tof += (fh_tof_id_eta_minv->GetBinWidth(i)) * (fh_tof_id_eta_minv->GetBinContent(i));
        sum3_eta_chi_prim += (fh_chi_prim_eta_minv->GetBinWidth(i)) * (fh_chi_prim_eta_minv->GetBinContent(i));
        sum3_eta_gamma += (fh_gammacut_eta_minv->GetBinWidth(i)) * (fh_gammacut_eta_minv->GetBinContent(i));
        sum3_eta_stcut += (fh_stcut_eta_minv->GetBinWidth(i)) * (fh_stcut_eta_minv->GetBinContent(i));
        sum3_eta_ttcut += (fh_ttcut_eta_minv->GetBinWidth(i)) * (fh_ttcut_eta_minv->GetBinContent(i));
        sum3_eta_apcut += (fh_apcut_eta_minv->GetBinWidth(i)) * (fh_apcut_eta_minv->GetBinContent(i));
        sum3_eta_pt += (fh_ptcut_eta_minv->GetBinWidth(i)) * (fh_ptcut_eta_minv->GetBinContent(i));
        sum3_eta_angle += (fh_anglecut_eta_minv->GetBinWidth(i)) * (fh_anglecut_eta_minv->GetBinContent(i));
    
    } // if

    }//for
    //////////signal
    Double_t sum1 = sum1_signal_tof + sum1_pi0_tof + sum1_eta_tof;

    Double_t eff1_tof = sum1 / sum1;
    Double_t eff1_chi = (sum1_signal_chi_prim + sum1_pi0_chi_prim + sum1_eta_chi_prim) / sum1;
cout << "chi1 signal = " << sum1_signal_chi_prim << "; pi0 = " << sum1_pi0_chi_prim << "; eta = "<< sum1_eta_chi_prim << endl;
    Double_t eff1_gamma = (sum1_signal_gamma + sum1_pi0_gamma + sum1_eta_gamma) / sum1;
    Double_t eff1_stcut = (sum1_signal_stcut + sum1_pi0_stcut + sum1_eta_stcut) / sum1;
    Double_t eff1_ttcut = (sum1_signal_ttcut + sum1_pi0_ttcut + sum1_eta_ttcut) / sum1;
    Double_t eff1_apcut = (sum1_signal_apcut + sum1_pi0_apcut + sum1_eta_apcut) / sum1;
    Double_t eff1_pt = (sum1_signal_pt + sum1_pi0_pt + sum1_eta_pt) / sum1;
    Double_t eff1_angle = (sum1_signal_angle + sum1_pi0_angle + sum1_eta_angle) / sum1;

    Double_t sum2 = sum2_signal_tof + sum2_pi0_tof + sum2_eta_tof;
    Double_t eff2_tof = sum2 / sum2;
    Double_t eff2_chi = (sum2_signal_chi_prim + sum2_pi0_chi_prim + sum2_eta_chi_prim) / sum2;
cout << "chi2 signal = " << sum2_signal_chi_prim << "; pi0 = " << sum2_pi0_chi_prim << "; eta = "<< sum2_eta_chi_prim << endl;
    Double_t eff2_gamma = (sum2_signal_gamma + sum2_pi0_gamma + sum2_eta_gamma) / sum2;
    Double_t eff2_stcut = (sum2_signal_stcut + sum2_pi0_stcut + sum2_eta_stcut) / sum2;
    Double_t eff2_ttcut = (sum2_signal_ttcut + sum2_pi0_ttcut + sum2_eta_ttcut) / sum2;
    Double_t eff2_apcut = (sum2_signal_apcut + sum2_pi0_apcut + sum2_eta_apcut) / sum2;
    Double_t eff2_pt = (sum2_signal_pt + sum2_pi0_pt + sum2_eta_pt) / sum2;
    Double_t eff2_angle = (sum2_signal_angle + sum2_pi0_angle + sum2_eta_angle) / sum2;

    Double_t sum3 = sum3_signal_tof + sum3_pi0_tof + sum3_eta_tof;
    Double_t eff3_tof = sum3 / sum3;
    Double_t eff3_chi = (sum3_signal_chi_prim + sum3_pi0_chi_prim + sum3_eta_chi_prim) / sum3;
cout << "chi3  signal = " << sum3_signal_chi_prim << "; pi0 = " << sum3_pi0_chi_prim << "; eta = "<< sum3_eta_chi_prim << endl;
    Double_t eff3_gamma = (sum3_signal_gamma + sum3_pi0_gamma + sum3_eta_gamma) / sum3;
    Double_t eff3_stcut = (sum3_signal_stcut + sum3_pi0_stcut + sum3_eta_stcut) / sum3;
    Double_t eff3_ttcut = (sum3_signal_ttcut + sum3_pi0_ttcut + sum3_eta_ttcut) / sum3;
    Double_t eff3_apcut = (sum3_signal_apcut + sum3_pi0_apcut + sum3_eta_apcut) / sum3;
    Double_t eff3_pt = (sum3_signal_pt + sum3_pi0_pt + sum3_eta_pt) / sum3;
    Double_t eff3_angle = (sum3_signal_angle + sum3_pi0_angle + sum3_eta_angle) / sum3;
    

    /////////BG
    Double_t eff1_bg_tof = sum1_bg_tof /  sum1_bg_tof;
    Double_t eff1_bg_chi = sum1_bg_chi_prim / sum1_bg_tof;
    Double_t eff1_bg_gamma = sum1_bg_gamma / sum1_bg_tof;
    Double_t eff1_bg_stcut = sum1_bg_stcut / sum1_bg_tof;
    Double_t eff1_bg_ttcut = sum1_bg_ttcut / sum1_bg_tof;
    Double_t eff1_bg_apcut = sum1_bg_apcut / sum1_bg_tof;
    Double_t eff1_bg_pt = sum1_bg_pt / sum1_bg_tof;
    Double_t eff1_bg_angle = sum1_bg_angle / sum1_bg_tof;
    
    Double_t eff2_bg_tof = sum2_bg_tof /  sum2_bg_tof;
    Double_t eff2_bg_chi = sum2_bg_chi_prim / sum2_bg_tof;
    Double_t eff2_bg_gamma = sum2_bg_gamma / sum2_bg_tof;
    Double_t eff2_bg_stcut = sum2_bg_stcut / sum2_bg_tof;
    Double_t eff2_bg_ttcut = sum2_bg_ttcut / sum2_bg_tof;
    Double_t eff2_bg_apcut = sum2_bg_apcut / sum2_bg_tof;
    Double_t eff2_bg_pt = sum2_bg_pt / sum2_bg_tof;
    Double_t eff2_bg_angle = sum2_bg_angle / sum2_bg_tof;

    Double_t eff3_bg_tof = sum3_bg_tof /  sum3_bg_tof;
    Double_t eff3_bg_chi = sum3_bg_chi_prim / sum3_bg_tof;
    Double_t eff3_bg_gamma = sum3_bg_gamma / sum3_bg_tof;
    Double_t eff3_bg_stcut = sum3_bg_stcut / sum3_bg_tof;
    Double_t eff3_bg_ttcut = sum3_bg_ttcut / sum3_bg_tof;
    Double_t eff3_bg_apcut = sum3_bg_apcut / sum3_bg_tof;
    Double_t eff3_bg_pt = sum3_bg_pt / sum3_bg_tof;
    Double_t eff3_bg_angle = sum3_bg_angle / sum3_bg_tof;

//////// 0.0 - 0.2 
    Float_t Yaxis1[8];
    Yaxis1[0] = eff1_tof;
    Yaxis1[1] = eff1_chi;
    Yaxis1[2] = eff1_gamma;
    Yaxis1[3] = eff1_stcut;
    Yaxis1[4] = eff1_ttcut;
    Yaxis1[5] = eff1_apcut;
    Yaxis1[6] = eff1_pt;
    Yaxis1[7] = eff1_angle;

    Float_t yaxis1[8];
    yaxis1[0] = eff1_bg_tof;
    yaxis1[1] = eff1_bg_chi;
    yaxis1[2] = eff1_bg_gamma;
    yaxis1[3] = eff1_bg_stcut;
    yaxis1[4] = eff1_bg_ttcut;
    yaxis1[5] = eff1_bg_apcut;
    yaxis1[6] = eff1_bg_pt;
    yaxis1[7] = eff1_bg_angle;

/////// 0.2 -1.
    Float_t Yaxis2[8];
    Yaxis2[0] = eff2_tof;
    Yaxis2[1] = eff2_chi;
    Yaxis2[2] = eff2_gamma;
    Yaxis2[3] = eff2_stcut;
    Yaxis2[4] = eff2_ttcut;
    Yaxis2[5] = eff2_apcut;
    Yaxis2[6] = eff2_pt;
    Yaxis2[7] = eff2_angle;

    Float_t yaxis2[8];
    yaxis2[0] = eff2_bg_tof;
    yaxis2[1] = eff2_bg_chi;
    yaxis2[2] = eff2_bg_gamma;
    yaxis2[3] = eff2_bg_stcut;
    yaxis2[4] = eff2_bg_ttcut;
    yaxis2[5] = eff2_bg_apcut;
    yaxis2[6] = eff2_bg_pt;
    yaxis2[7] = eff2_bg_angle;
//////// 1. - 2.
    Float_t Yaxis3[8];
    Yaxis3[0] = eff3_tof;
    Yaxis3[1] = eff3_chi;
    Yaxis3[2] = eff3_gamma;
    Yaxis3[3] = eff3_stcut;
    Yaxis3[4] = eff3_ttcut;
    Yaxis3[5] = eff3_apcut;
    Yaxis3[6] = eff3_pt;
    Yaxis3[7] = eff3_angle;

    Float_t yaxis3[8];
    yaxis3[0] = eff3_bg_tof;
    yaxis3[1] = eff3_bg_chi;
    yaxis3[2] = eff3_bg_gamma;
    yaxis3[3] = eff3_bg_stcut;
    yaxis3[4] = eff3_bg_ttcut;
    yaxis3[5] = eff3_bg_apcut;
    yaxis3[6] = eff3_bg_pt;
    yaxis3[7] = eff3_bg_angle;
/////////


    Float_t x[8] = {0.5, 1.5, 2.5, 3.5, 4.5, 5.5, 6.5, 7.5};
    Int_t n = 8;

    TCanvas* can = new TCanvas ("can", "", 158,28,900,918);
    TGraph* gr1_signal = new TGraph(n, x, Yaxis1);
    TGraph* gr1_bg = new TGraph(n, x, yaxis1);
    gr1_signal->SetLineColor(kRed);
    gr1_bg->SetLineColor(kBlue);
    gr1_signal->SetMarkerStyle(29);
    gr1_bg->SetMarkerStyle(29);
    gr1_signal->SetMarkerColor(kRed);
    gr1_bg->SetMarkerColor(kBlue);
    gr1_signal->Draw("APL");
    gr1_bg->Draw("PL");

    TLegend* leg = new TLegend(0.65,0.6,1., 1.);
    leg->AddEntry(gr1_signal, "signal", "l");
    leg->AddEntry(gr1_bg, "BG", "l");
    leg->Draw();
    
/////canvas 2
    TCanvas* can1 = new TCanvas ("can1", "", 158,28,900,918);
    TGraph* gr2_signal = new TGraph(n, x, Yaxis2);
    TGraph* gr2_bg = new TGraph(n, x, yaxis2);
    gr2_signal->SetLineColor(kRed);
    gr2_bg->SetLineColor(kBlue);
    gr2_signal->SetMarkerStyle(29);
    gr2_bg->SetMarkerStyle(29);
    gr2_signal->SetMarkerColor(kRed);
    gr2_bg->SetMarkerColor(kBlue);
    gr2_signal->Draw("APL");
    gr2_bg->Draw("PL");

    TLegend* leg2 = new TLegend(0.65,0.6,1., 1.);
    leg2->AddEntry(gr2_signal, "signal", "l");
    leg2->AddEntry(gr2_bg, "BG", "l");
    leg2->Draw();

/////canvas 3
    TCanvas* can2 = new TCanvas ("can2", "", 158,28,900,918);
    TGraph* gr3_signal = new TGraph(n, x, Yaxis3);
    TGraph* gr3_bg = new TGraph(n, x, yaxis3);
    gr3_signal->SetLineColor(kRed);
    gr3_bg->SetLineColor(kBlue);
    gr3_signal->SetMarkerStyle(29);
    gr3_bg->SetMarkerStyle(29);
    gr3_signal->SetMarkerColor(kRed);
    gr3_bg->SetMarkerColor(kBlue);
    gr3_signal->Draw("APL");
    gr3_bg->Draw("PL");

    TLegend* leg3 = new TLegend(0.65,0.6,1., 1.);
    leg3->AddEntry(gr3_signal, "signal", "l");
    leg3->AddEntry(gr3_bg, "BG", "l");
    leg3->Draw();

}
