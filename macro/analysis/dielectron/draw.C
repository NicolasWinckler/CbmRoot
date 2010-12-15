#include "../../../littrack/utils/CbmLitDrawHist.cxx"
#include "../../../littrack/utils/CbmLitUtils.cxx"

void draw_eff_pty(TH2D* h1, TH2D* h2){
    char effTxt[60];
    sprintf(effTxt,"%.1f\%", (Double_t)h1->GetEntries()/h2->GetEntries()*100.);
    TText *t3 = new TText(0.2, 1.8, effTxt);
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
    //h1->Sumw2();
    //h2->Sumw2();
    h3->Divide(h1, h2, 1, 1, "B");
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
    TFile *file = new TFile("/lustre/cbm/user/ebelolap/oct10/urqmd_rho0/25gev/100_field/real/mytask.analysis.all.root");
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
    fh_ptcut_signal_pty->Draw("COLZ");
    draw_eff_pty(fh_ptcut_signal_pty, fh_mc_signal_pty);
    c1->cd(9);
    fh_anglecut_signal_pty->Draw("COLZ");
    draw_eff_pty(fh_anglecut_signal_pty, fh_mc_signal_pty);
    c1->cd(10);
    fh_pi0cut_signal_pty->Draw("COLZ");
    draw_eff_pty(fh_pi0cut_signal_pty, fh_mc_signal_pty);
    c1->cd(11);
    fh_ttcut_signal_pty->Draw("COLZ");
    draw_eff_pty(fh_ttcut_signal_pty, fh_mc_signal_pty);

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
    TH2D* fh_ptcut_signal_pty_eff = divideHisto2D(fh_ptcut_signal_pty, fh_chi_prim_signal_pty);
    fh_ptcut_signal_pty_eff->Draw("COLZ");
    draw_eff_pty(fh_ptcut_signal_pty, fh_chi_prim_signal_pty);
    c1_1->cd(9);
    TH2D* fh_anglecut_signal_pty_eff = divideHisto2D(fh_anglecut_signal_pty, fh_ptcut_signal_pty);
    fh_anglecut_signal_pty_eff->Draw("COLZ");
    draw_eff_pty(fh_anglecut_signal_pty, fh_ptcut_signal_pty);
    c1_1->cd(10);
    TH2D* fh_pi0cut_signal_pty_eff = divideHisto2D(fh_pi0cut_signal_pty, fh_anglecut_signal_pty);
    fh_pi0cut_signal_pty_eff->Draw("COLZ");
    draw_eff_pty(fh_pi0cut_signal_pty, fh_anglecut_signal_pty);
    c1_1->cd(11);
    TH2D* fh_ttcut_signal_pty_eff = divideHisto2D(fh_ttcut_signal_pty, fh_pi0cut_signal_pty);
    fh_ttcut_signal_pty_eff->Draw("COLZ");
    draw_eff_pty(fh_ttcut_signal_pty, fh_pi0cut_signal_pty);

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
    fh_pi0cut_signal_mom->SetLineColor(kGreen+3);
    fh_ttcut_signal_mom->SetLineColor(kOrange+7);
    fh_mc_signal_mom->Draw();
    fh_acc_signal_mom->Draw("same");
    fh_reco_signal_mom->Draw("same");
    fh_rich_id_signal_mom->Draw("same");
    fh_trd_id_signal_mom->Draw("same");
    fh_tof_id_signal_mom->Draw("same");
    fh_chi_prim_signal_mom->Draw("same");
    fh_ptcut_signal_mom->Draw("same");
    fh_anglecut_signal_mom->Draw("same");
    fh_pi0cut_signal_mom->Draw("same");
    fh_ttcut_signal_mom->Draw("same");

    TLegend* leg3 = new TLegend(0.65,0.6,1., 1.);
    leg3->AddEntry(fh_mc_signal_mom, "mc", "l");
    leg3->AddEntry(fh_acc_signal_mom, "acc", "l");
    leg3->AddEntry(fh_reco_signal_mom, "rec", "l");
    leg3->AddEntry(fh_rich_id_signal_mom, "rich id", "l");
    leg3->AddEntry(fh_trd_id_signal_mom, "trd id", "l");
    leg3->AddEntry(fh_tof_id_signal_mom, "tof id", "l");
    leg3->AddEntry(fh_chi_prim_signal_mom, "chi prim", "l");
    leg3->AddEntry(fh_ptcut_signal_mom, "pt cut", "l");
    leg3->AddEntry(fh_anglecut_signal_mom, "angle cut", "l");
    leg3->AddEntry(fh_pi0cut_signal_mom, "gamma cut", "l");
    leg3->AddEntry(fh_ttcut_signal_mom, "tt cut", "l");
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
    TH1D* fh_acc_signal_mom_eff = divideHisto1D(fh_acc_signal_mom,fh_mc_signal_mom );
    fh_acc_signal_mom_eff->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    //gPad->SetLogy(true);
    c2_1->cd(2);
    TH1D* fh_reco_signal_mom_eff = divideHisto1D(fh_reco_signal_mom,fh_acc_signal_mom );
    fh_reco_signal_mom_eff->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    //gPad->SetLogy(true);
    c2_1->cd(3);
    TH1D* fh_rich_id_signal_mom_eff = divideHisto1D(fh_rich_id_signal_mom,fh_reco_signal_mom );
    fh_rich_id_signal_mom_eff->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    //gPad->SetLogy(true);
    c2_1->cd(4);
    TH1D* fh_trd_id_signal_mom_eff = divideHisto1D(fh_trd_id_signal_mom,fh_rich_id_signal_mom );
    fh_trd_id_signal_mom_eff->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    //gPad->SetLogy(true);
    c2_1->cd(5);
    TH1D* fh_tof_id_signal_mom_eff = divideHisto1D(fh_tof_id_signal_mom,fh_trd_id_signal_mom );
    fh_tof_id_signal_mom_eff->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    //gPad->SetLogy(true);
    c2_1->cd(6);
    TH1D* fh_chi_prim_signal_mom_eff = divideHisto1D(fh_chi_prim_signal_mom,fh_tof_id_signal_mom );
    fh_chi_prim_signal_mom_eff->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    //gPad->SetLogy(true);
    c2_1->cd(7);
    TH1D* fh_ptcut_signal_mom_eff = divideHisto1D(fh_ptcut_signal_mom,fh_chi_prim_signal_mom );
    fh_ptcut_signal_mom_eff->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    //gPad->SetLogy(true);
    c2_1->cd(8);
    TH1D* fh_anglecut_signal_mom_eff = divideHisto1D(fh_anglecut_signal_mom,fh_ptcut_signal_mom );
    fh_anglecut_signal_mom_eff->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);
   // gPad->SetLogy(true);
    c2_1->cd(9);
    TH1D* fh_pi0cut_signal_mom_eff = divideHisto1D(fh_pi0cut_signal_mom,fh_anglecut_signal_mom );
    fh_pi0cut_signal_mom_eff->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    //gPad->SetLogy(true);
    c2_1->cd(10);
    TH1D* fh_ttcut_signal_mom_eff = divideHisto1D(fh_ttcut_signal_mom,fh_pi0cut_signal_mom );
    fh_ttcut_signal_mom_eff->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    //gPad->SetLogy(true);

    TCanvas *c3 = new TCanvas("c3-mother pdg", "c3-mother pdg", 500, 500);
    fh_mc_mother_pdg->Draw();
    fh_acc_mother_pdg->SetLineColor(kRed);
    fh_acc_mother_pdg->Draw("same");
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);
   
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
    c4->cd(11); 
    fh_ttcut_signal->Draw("COLZ");
    c4->cd(12); 
    fh_ttcut_bg->Draw("COLZ");
    fh_mom_signal->Scale(scaleSig);
    fh_mom_bg->Scale(scaleBg);

    TCanvas *c4_1 = new TCanvas("c4_1-cuts", "c4_1-cuts", 800, 800);
    c4_1->Divide(2,2);
    c4_1->cd(1); 
    fh_apcut_signal->Draw("COLZ");
    c4_1->cd(2); 
    fh_apcut_bg->Draw("COLZ");

//    TCanvas *c5 = new TCanvas("c5","c5", 1200, 400);
//    c5->Divide(3,1);
//    c5->cd(1);
//    fh_mc_vertex_gamma_xz->Draw("COLZ");
//    c5->cd(2);
//    fh_mc_vertex_gamma_yz->Draw("COLZ");
//    c5->cd(3);
//    fh_mc_vertex_gamma_xy->Draw("COLZ");

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
    fh_pi0cut_signal_minv->SetLineColor(kPink-6);
    fh_ttcut_signal_minv->SetLineColor(kYellow+1);
    fh_rec_signal_minv->Draw("same");
    fh_rich_id_signal_minv->Draw("same");
    fh_trd_id_signal_minv->Draw("same");
    fh_tof_id_signal_minv->Draw("same");
    fh_chi_prim_signal_minv->Draw("same");
    fh_ptcut_signal_minv->Draw("same");
    fh_anglecut_signal_minv->Draw("same");
    fh_pi0cut_signal_minv->Draw("same");
    fh_ttcut_signal_minv->Draw("same");
    TLegend* leg3 = new TLegend(0.65,0.6,1., 1.);
    leg3->AddEntry(fh_mc_signal_minv, "mc", "l");
    leg3->AddEntry(fh_acc_signal_minv, "acc", "l");
    leg3->AddEntry(fh_rec_signal_minv, "rec", "l");
    leg3->AddEntry(fh_rich_id_signal_minv, "rich id", "l");
    leg3->AddEntry(fh_trd_id_signal_minv, "trd id", "l");
    leg3->AddEntry(fh_tof_id_signal_minv, "tof id", "l");
    leg3->AddEntry(fh_chi_prim_signal_minv, "chi prim", "l");
    leg3->AddEntry(fh_ptcut_signal_minv, "pt cut", "l");
    leg3->AddEntry(fh_anglecut_signal_minv, "angle cut", "l");
    leg3->AddEntry(fh_pi0cut_signal_minv, "gamma cut", "l");
    leg3->AddEntry(fh_ttcut_signal_minv, "tt cut", "l");
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
    fh_pi0cut_bg_minv->SetLineColor(kPink-6);
//    fh_ttcut_bg_minv->SetLineColor(kYellow+1);
    fh_rec_bg_minv->Draw("same");
    fh_rec_bg_minv->SetMinimum(5*10e-8);
    fh_rich_id_bg_minv->Draw("same");
    fh_trd_id_bg_minv->Draw("same");
    fh_tof_id_bg_minv->Draw("same");
    fh_chi_prim_bg_minv->Draw("same");
    fh_ptcut_bg_minv->Draw("same");
    fh_anglecut_bg_minv->Draw("same");
    fh_pi0cut_bg_minv->Draw("same");
    fh_ttcut_bg_minv->Draw("same");

    TLegend* leg = new TLegend(0.65,0.6,1., 1.);
    leg->AddEntry(fh_rec_bg_minv, "rec", "l");
    leg->AddEntry(fh_rich_id_bg_minv, "rich id", "l");
    leg->AddEntry(fh_trd_id_bg_minv, "trd id", "l");
    leg->AddEntry(fh_tof_id_bg_minv, "tof id", "l");
    leg->AddEntry(fh_chi_prim_bg_minv, "chi prim", "l");
    leg->AddEntry(fh_ptcut_bg_minv, "pt cut", "l");
    leg->AddEntry(fh_anglecut_bg_minv, "angle cut", "l");
    leg->AddEntry(fh_pi0cut_bg_minv, "gamma cut", "l");
    leg->AddEntry(fh_ttcut_bg_minv, "tt cut", "l");
    leg->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);

    TCanvas *c7 = new TCanvas("c7-minv", "c7-minv", 1200, 600);
    c7->Divide(2,1);
    c7->cd(1);
    fh_mc_pi0_minv->Draw();
    fh_acc_pi0_minv->Draw("same");
    fh_rich_id_pi0_minv->SetLineColor(kRed);
    fh_trd_id_pi0_minv->SetLineColor(kBlue);
    fh_tof_id_pi0_minv->SetLineColor(kGreen);
    fh_chi_prim_pi0_minv->SetLineColor(kOrange+7);
    fh_ptcut_pi0_minv->SetLineColor(kMagenta);
    fh_anglecut_pi0_minv->SetLineColor(kViolet+10);
    fh_pi0cut_pi0_minv->SetLineColor(kPink-6);
    fh_ttcut_pi0_minv->SetLineColor(kYellow+1);
    fh_rec_pi0_minv->Draw("same");
    fh_rich_id_pi0_minv->Draw("same");
    fh_trd_id_pi0_minv->Draw("same");
    fh_tof_id_pi0_minv->Draw("same");
    fh_chi_prim_pi0_minv->Draw("same");
    fh_ptcut_pi0_minv->Draw("same");
    fh_anglecut_pi0_minv->Draw("same");
    fh_pi0cut_pi0_minv->Draw("same");
    fh_ttcut_pi0_minv->Draw("same");
    TLegend* leg3 = new TLegend(0.65,0.6,1., 1.);
    leg3->AddEntry(fh_rec_pi0_minv, "rec", "l");
	leg3->AddEntry(fh_rich_id_pi0_minv, "rich id", "l");
	leg3->AddEntry(fh_trd_id_pi0_minv, "trd id", "l");
	leg3->AddEntry(fh_tof_id_pi0_minv, "tof id", "l");
	leg3->AddEntry(fh_chi_prim_pi0_minv, "chi prim", "l");
	leg3->AddEntry(fh_ptcut_pi0_minv, "pt cut", "l");
	leg3->AddEntry(fh_anglecut_pi0_minv, "angle cut", "l");
	leg3->AddEntry(fh_pi0cut_pi0_minv, "gamma cut", "l");
	leg3->AddEntry(fh_ttcut_pi0_minv, "tt cut", "l");
	leg3->Draw();
	gPad->SetGridx(true);
	gPad->SetGridy(true);
	gPad->SetLogy(true);

    calculateSignalOverBg(fh_rec_signal_minv, fh_rec_bg_minv);
    calculateSignalOverBg(fh_rich_id_signal_minv, fh_rich_id_bg_minv);
    calculateSignalOverBg(fh_trd_id_signal_minv, fh_trd_id_bg_minv);
    calculateSignalOverBg(fh_tof_id_signal_minv, fh_tof_id_bg_minv);
    calculateSignalOverBg(fh_chi_prim_signal_minv, fh_chi_prim_bg_minv);
    calculateSignalOverBg(fh_ptcut_signal_minv, fh_ptcut_bg_minv);
    calculateSignalOverBg(fh_anglecut_signal_minv, fh_anglecut_bg_minv);
    calculateSignalOverBg(fh_pi0cut_signal_minv, fh_pi0cut_bg_minv);
    calculateSignalOverBg(fh_ttcut_signal_minv, fh_ttcut_bg_minv);

}


