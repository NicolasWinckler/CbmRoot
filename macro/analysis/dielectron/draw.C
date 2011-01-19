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
    TFile *file = new TFile("/lustre/cbm/user/ebelolap/oct10/urqmd_rho0/25gev/100_field/real/mytask.analysis.all.new1.root");
   //TString signalSt = "rho0"; //rho0, phi, omega
    gStyle->SetHistLineWidth(3);
	//SetStyles();
    gROOT->SetStyle("Plain");
    gStyle->SetPalette(1,0);

// pty distribution of signal
    TCanvas *c1 = new TCanvas("c1-pty","c1-pty",1000,750);
    c1->Divide(4,3);
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
    fh_ttcut_signal_pty->Draw("COLZ");
    draw_eff_pty(fh_ttcut_signal_pty, fh_mc_signal_pty);
    c1->cd(10);
    fh_apcut_signal_pty->Draw("COLZ");
    draw_eff_pty(fh_apcut_signal_pty, fh_mc_signal_pty);
    c1->cd(11);
    fh_ptcut_signal_pty->Draw("COLZ");
    draw_eff_pty(fh_ptcut_signal_pty, fh_mc_signal_pty);
    c1->cd(12);
    fh_anglecut_signal_pty->Draw("COLZ");
    draw_eff_pty(fh_anglecut_signal_pty, fh_mc_signal_pty);

    
//pty efficiency of signal
    TCanvas *c1_1 = new TCanvas("c1_1-pty eff","c1_1-pty eff",1000,750);
    c1_1->Divide(4,3);
    c1_1->cd(2);
    TH2D* fh_acc_signal_pty_eff = divideHisto2D(fh_acc_signal_pty, fh_mc_signal_pty);
    fh_acc_signal_pty_eff->Draw("COLZ");
    draw_eff_pty(fh_acc_signal_pty, fh_mc_signal_pty);
    c1_1->cd(3);
    TH2D* fh_reco_signal_pty_eff = divideHisto2D(fh_reco_signal_pty, fh_acc_signal_pty);
    fh_reco_signal_pty_eff->Draw("COLZ");
    draw_eff_pty(fh_reco_signal_pty, fh_acc_signal_pty);
    c1_1->cd(4);
    TH2D* fh_rich_id_signal_pty_eff = divideHisto2D(fh_rich_id_signal_pty, fh_reco_signal_pty);
    fh_rich_id_signal_pty_eff->Draw("COLZ");
    draw_eff_pty(fh_rich_id_signal_pty, fh_reco_signal_pty);
    c1_1->cd(5);
    TH2D* fh_trd_id_signal_pty_eff = divideHisto2D(fh_trd_id_signal_pty, fh_rich_id_signal_pty);
    fh_trd_id_signal_pty_eff->Draw("COLZ");
    draw_eff_pty(fh_trd_id_signal_pty, fh_rich_id_signal_pty);
    c1_1->cd(6);
    TH2D* fh_tof_id_signal_pty_eff = divideHisto2D(fh_tof_id_signal_pty, fh_trd_id_signal_pty);
    fh_tof_id_signal_pty_eff->Draw("COLZ");
    draw_eff_pty(fh_tof_id_signal_pty, fh_trd_id_signal_pty);
    c1_1->cd(7);
    TH2D* fh_chi_prim_signal_pty_eff = divideHisto2D(fh_chi_prim_signal_pty, fh_tof_id_signal_pty);
    fh_chi_prim_signal_pty_eff->Draw("COLZ");
    draw_eff_pty(fh_chi_prim_signal_pty, fh_tof_id_signal_pty);
    c1_1->cd(8);
    TH2D* fh_gammacut_signal_pty_eff = divideHisto2D(fh_gammacut_signal_pty, fh_tof_id_signal_pty);
    fh_gammacut_signal_pty_eff->Draw("COLZ");
    draw_eff_pty(fh_gammacut_signal_pty, fh_tof_id_signal_pty);
    c1_1->cd(9);
    TH2D* fh_ttcut_signal_pty_eff = divideHisto2D(fh_ttcut_signal_pty, fh_gammacut_signal_pty);
    fh_ttcut_signal_pty_eff->Draw("COLZ");
    draw_eff_pty(fh_ttcut_signal_pty, fh_gammacut_signal_pty);
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
    fh_ttcut_signal_mom->SetLineColor(kOrange+7);
    fh_apcut_signal_mom->SetLineColor(kCyan+2);
    fh_mc_signal_mom->Draw();
    fh_acc_signal_mom->Draw("same");
    fh_reco_signal_mom->Draw("same");
    fh_rich_id_signal_mom->Draw("same");
    fh_trd_id_signal_mom->Draw("same");
    fh_tof_id_signal_mom->Draw("same");
    fh_chi_prim_signal_mom->Draw("same");
    fh_gammacut_signal_mom->Draw("same");
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
    TH1D* fh_ttcut_signal_mom_eff = divideHisto1D(fh_ttcut_signal_mom,fh_gammacut_signal_mom);
    fh_ttcut_signal_mom_eff->Draw();
    draw_eff_mom(fh_ttcut_signal_mom,fh_gammacut_signal_mom);
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    //gPad->SetLogy(true);

    c2_1->cd(9);
    TH1D* fh_apcut_signal_mom_eff = divideHisto1D(fh_apcut_signal_mom,fh_ttcut_signal_mom);
    fh_apcut_signal_mom_eff->Draw();
    draw_eff_mom(fh_apcut_signal_mom,fh_ttcut_signal_mom);
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    //gPad->SetLogy(true);
   
    c2_1->cd(10);
    TH1D* fh_ptcut_signal_mom_eff = divideHisto1D(fh_ptcut_signal_mom,fh_apcut_signal_mom);
    fh_ptcut_signal_mom_eff->Draw();
    draw_eff_mom(fh_ptcut_signal_mom,fh_apcut_signal_mom);
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    //gPad->SetLogy(true);
    c2_1->cd(11);
    TH1D* fh_anglecut_signal_mom_eff = divideHisto1D(fh_anglecut_signal_mom,fh_ptcut_signal_mom);
    fh_anglecut_signal_mom_eff->Draw();
    draw_eff_mom(fh_anglecut_signal_mom,fh_ptcut_signal_mom);
    gPad->SetGridx(true);
    gPad->SetGridy(true);
   // gPad->SetLogy(true);
    

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
    fh_source_pair_ttcut->Draw("COLZ");
    c10->cd(8);
    fh_source_pair_apcut->Draw("COLZ");
    c10->cd(10);
    fh_source_pair_ptcut->Draw("COLZ");
    c10->cd(9);
    fh_source_pair_anglecut->Draw("COLZ");
     
//AP cut distribution
    TCanvas *c4_1 = new TCanvas("c4_1-apcut", "c4_1-apcut", 800, 800);
    c4_1->Divide(2,2);
    c4_1->cd(1); 
    fh_apcut_signal->Draw("COLZ");
    TEllipse el1(0.5,0.5,0.2,0.3);
    el1.SetFillStyle(0);
    el1.SetLineWidth(3);
    el1.DrawEllipse(0.,0.,1.,0.45,0.,180.,0.);
    c4_1->cd(2); 
    fh_apcut_bg->Draw("COLZ");
    TEllipse el2(0.5,0.5,0.2,0.3);
    el2.SetFillStyle(0);
    el2.SetLineWidth(3);
    el2.DrawEllipse(0.,0.,1.,0.45,0.,180.,0.);
    c4_1->cd(3);
    fh_apcut_pi0->Draw("COLZ");
    c4_1->cd(4);
    fh_apcut_gamma->Draw("COLZ");

//track topology cut distribution
    TCanvas *c4_2 = new TCanvas("c4_2-ttcut", "c4_2-ttcut", 800, 800);
    c4_2->Divide(2,2);
    c4_2->cd(1);
    fh_ttcut_signal->Draw("COLZ");
    c4_2->cd(2);
    fh_ttcut_bg->Draw("COLZ");
    c4_2->cd(3);
    fh_ttcut_pi0->Draw("COLZ");
    c4_2->cd(4);
    fh_ttcut_gamma->Draw("COLZ");

//    TCanvas *c5 = new TCanvas("c5","c5", 1200, 400);
//    c5->Divide(3,1);
//    c5->cd(1);
//    fh_mc_vertex_gamma_xz->Draw("COLZ");
//    c5->cd(2);
//    fh_mc_vertex_gamma_yz->Draw("COLZ");
//    c5->cd(3);
//    fh_mc_vertex_gamma_xy->Draw("COLZ");

//ID cuts distribution
    TCanvas *c5_1 = new TCanvas("c5_1-id cuts","c5_1-id cuts",900, 600);
    c5_1->Divide(3,2);
    c5_1->cd(1);
    Double_t scaleSig = 1./fh_rich_ann_signal->Integral();
    Double_t scaleBg = 1./fh_rich_ann_bg->Integral();
    fh_rich_ann_signal->SetLineColor(kRed);
    fh_rich_ann_bg->SetLineColor(kBlue);
    fh_rich_ann_signal->Draw();
    fh_rich_ann_bg->Draw("same");
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);
    c5_1->cd(2);
    TH1D* fh_sig_rich_ann = CalculateSignificance(fh_rich_ann_signal, fh_rich_ann_bg, 
        "rich_ann_significance", "significance", "back");
    fh_sig_rich_ann->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    fh_rich_ann_signal->Scale(scaleSig);
    fh_rich_ann_bg->Scale(scaleBg);
    
    c5_1->cd(3);
    Double_t scaleSig = 1./fh_trd_ann_signal->Integral();
    Double_t scaleBg = 1./fh_trd_ann_bg->Integral();
    fh_trd_ann_signal->SetLineColor(kRed);
    fh_trd_ann_bg->SetLineColor(kBlue);
    fh_trd_ann_signal->Draw();
    fh_trd_ann_bg->Draw("same");
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);
    c5_1->cd(4);
    TH1D* fh_sig_trd_ann = CalculateSignificance(fh_trd_ann_signal, fh_trd_ann_bg, 
        "trd_ann_significance", "significance", "back");
    fh_sig_trd_ann->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    fh_trd_ann_signal->Scale(scaleSig);
    fh_trd_ann_bg->Scale(scaleBg);

    c5_1->cd(5);
    fh_tof_m2_signal->Draw("COLZ");
    gPad->SetLogz(true);
    c5_1->cd(6);
    fh_tof_m2_bg->Draw("COLZ");
    gPad->SetLogz(true);

//invariant mass distribution
    TCanvas *c6 = new TCanvas("c6-minv", "c6-minv", 1200, 600);
    c6->Divide(2,1);
    c6->cd(1); 
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
    fh_ttcut_signal_minv->SetLineColor(kYellow+1);
    fh_apcut_signal_minv->SetLineColor(kPink+8);
    fh_rec_signal_minv->Draw("same");
    fh_rich_id_signal_minv->Draw("same");
    fh_trd_id_signal_minv->Draw("same");
    fh_tof_id_signal_minv->Draw("same");
    fh_chi_prim_signal_minv->Draw("same");
    fh_gammacut_signal_minv->Draw("same");
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
    leg3->AddEntry(fh_ttcut_signal_minv, "tt cut", "l");
    leg3->AddEntry(fh_apcut_signal_minv, "ap cut", "l");
    leg3->AddEntry(fh_ptcut_signal_minv, "pt cut", "l");
    leg3->AddEntry(fh_anglecut_signal_minv, "angle cut", "l");
     
    leg3->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);

    c6->cd(2);
    fh_rich_id_bg_minv->SetLineColor(kRed);
    fh_trd_id_bg_minv->SetLineColor(kBlue);
    fh_tof_id_bg_minv->SetLineColor(kGreen);
    fh_chi_prim_bg_minv->SetLineColor(kOrange+7);   
    fh_ptcut_bg_minv->SetLineColor(kMagenta);
    fh_anglecut_bg_minv->SetLineColor(kViolet+10);
    fh_gammacut_bg_minv->SetLineColor(kPink-6);
    fh_ttcut_bg_minv->SetLineColor(kYellow+1);
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
  //  fh_ttcut_bg_minv->Draw("same");
    fh_apcut_bg_minv->Draw("same");

    TLegend* leg = new TLegend(0.65,0.6,1., 1.);
    leg->AddEntry(fh_rec_bg_minv, "rec", "l");
    leg->AddEntry(fh_rich_id_bg_minv, "rich id", "l");
    leg->AddEntry(fh_trd_id_bg_minv, "trd id", "l");
    leg->AddEntry(fh_tof_id_bg_minv, "tof id", "l");
    leg->AddEntry(fh_chi_prim_bg_minv, "chi prim", "l");
    leg->AddEntry(fh_gammacut_bg_minv, "gamma cut", "l");
    leg->AddEntry(fh_ttcut_bg_minv, "tt cut", "l");
    leg->AddEntry(fh_apcut_bg_minv, "ap cut", "l");
    leg->AddEntry(fh_ptcut_bg_minv, "pt cut", "l");
    leg->AddEntry(fh_anglecut_bg_minv, "angle cut", "l");
     
    leg->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);


//invariant mass distribution for Pi0 and eta
    TCanvas *c7 = new TCanvas("c7-minv-pi0-eta", "c7-minv-pi0-eta", 1200, 600);
    c7->Divide(2,1);
    c7->cd(1);
    fh_rich_id_pi0_minv->SetLineColor(kRed);
    fh_trd_id_pi0_minv->SetLineColor(kBlue);
    fh_tof_id_pi0_minv->SetLineColor(kGreen);
    fh_chi_prim_pi0_minv->SetLineColor(kOrange+7);
    fh_ptcut_pi0_minv->SetLineColor(kMagenta);
    fh_anglecut_pi0_minv->SetLineColor(kViolet+10);
    fh_gammacut_pi0_minv->SetLineColor(kPink-6);
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
    fh_ttcut_pi0_minv->Draw("same");
    fh_apcut_pi0_minv->Draw("same");
    TLegend* leg3 = new TLegend(0.65,0.6,1., 1.);
    leg3->AddEntry(fh_rec_pi0_minv, "rec", "l");
	leg3->AddEntry(fh_rich_id_pi0_minv, "rich id", "l");
	leg3->AddEntry(fh_trd_id_pi0_minv, "trd id", "l");
	leg3->AddEntry(fh_tof_id_pi0_minv, "tof id", "l");
	leg3->AddEntry(fh_chi_prim_pi0_minv, "chi prim", "l");
    leg3->AddEntry(fh_gammacut_pi0_minv, "gamma cut", "l");
    leg3->AddEntry(fh_ttcut_pi0_minv, "tt cut", "l");
    leg3->AddEntry(fh_apcut_pi0_minv, "ap cut", "l");
	leg3->AddEntry(fh_ptcut_pi0_minv, "pt cut", "l");
    leg3->AddEntry(fh_anglecut_pi0_minv, "angle cut", "l");
	leg3->Draw();
	gPad->SetGridx(true);
	gPad->SetGridy(true);
	gPad->SetLogy(true);

	c7->cd(2);
	fh_rich_id_eta_minv->SetLineColor(kRed);
	fh_trd_id_eta_minv->SetLineColor(kBlue);
	fh_tof_id_eta_minv->SetLineColor(kGreen);
	fh_chi_prim_eta_minv->SetLineColor(kOrange+7);
	fh_ptcut_eta_minv->SetLineColor(kMagenta);
	fh_anglecut_eta_minv->SetLineColor(kViolet+10);
	fh_gammacut_eta_minv->SetLineColor(kPink-6);
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
	fh_ttcut_eta_minv->Draw("same");
	fh_apcut_eta_minv->Draw("same");
	TLegend* leg4 = new TLegend(0.65,0.6,1., 1.);
	leg4->AddEntry(fh_rec_eta_minv, "rec", "l");
	leg4->AddEntry(fh_rich_id_eta_minv, "rich id", "l");
	leg4->AddEntry(fh_trd_id_eta_minv, "trd id", "l");
	leg4->AddEntry(fh_tof_id_eta_minv, "tof id", "l");
	leg4->AddEntry(fh_chi_prim_eta_minv, "chi prim", "l");
    leg4->AddEntry(fh_gammacut_eta_minv, "gamma cut", "l");
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
    calculateSignalOverBg(fh_ttcut_signal_minv, fh_ttcut_bg_minv);
    calculateSignalOverBg(fh_apcut_signal_minv, fh_apcut_bg_minv);
    calculateSignalOverBg(fh_ptcut_signal_minv, fh_ptcut_bg_minv);
    calculateSignalOverBg(fh_anglecut_signal_minv, fh_anglecut_bg_minv);
    
}


