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
    //h1->Sumw2();
    //h2->Sumw2();
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

TH2D* CalculateSignificance2D(TH2D* signal, TH2D* bg, TString name, TString title)
{

	Double_t xmin = 1.0;
	Double_t xmax = 5.0;
	Double_t ymin = 1.0;
	Double_t ymax = 5.0;
	Double_t delta = 0.1;
	Int_t nStepsX = (xmax - xmin) / delta;
	Int_t nStepsY = (ymax - ymin) / delta;
	Int_t nBinsX = signal->GetNbinsX();
	Int_t nBinsY = signal->GetNbinsY();

	TH2D* hsig = new TH2D(name, title, nStepsX, xmin, xmax, nStepsY, ymin, ymax);

	Double_t sumSignal = 0;
	Double_t sumBg = 0;

	Int_t binX = 1;
	for (Double_t xcut = xmin; xcut <= xmax; xcut+=delta, binX++) {
		Int_t binY = 1;
		cout << "x " << xcut << endl;
		for (Double_t ycut = ymin; ycut <= ymax; ycut+=delta, binY++){

			sumSignal = 0;
			sumBg = 0;
			for (Int_t ix = 1; ix <= nBinsX; ix++){
				for (Int_t iy = 1; iy <=nBinsY; iy++){
					Double_t xcenter = signal->GetXaxis()->GetBinCenter(ix);
					Double_t ycenter = signal->GetYaxis()->GetBinCenter(iy);
					Double_t val = -1 * (ycut/xcut)*xcenter + ycut;

					if (!(xcenter < xcut && ycenter < val)) {
						sumSignal += signal->GetBinContent(ix,iy);
						sumBg += bg->GetBinContent(ix,iy);
					}
				}
			}
			Double_t prov = TMath::Sqrt(sumSignal+sumBg);
			Double_t significance = 0.;
			if (prov != 0) significance = sumSignal / prov;
			//cout << "binX = " << binX << " binY = " << binY << "sig = " << significance << endl;
			hsig->SetBinContent(binX, binY, significance);
		}
	}
	return hsig;
}

TH1D* CalculateCumulativeProbability(TH1D* histo, TString name, TString title)
{

	Int_t nBins = histo->GetNbinsX();
	Double_t xmin = histo->GetXaxis()->GetXmin();
	Double_t xmax = histo->GetXaxis()->GetXmax();
	TH1D* h1 = new TH1D(name,title, nBins, xmin, xmax);

	Double_t sumCut = 0;
	Double_t sumAll = 0;
	Double_t prob = 0;
	for(Int_t i = 1; i <= nBins; i++) {
		sumAll = histo->Integral();
		sumCut = histo->Integral(1, i);
		if (sumAll != 0.) prob = (sumCut /sumAll)*100.;
		else prob = 0;
		h1->SetBinContent(i, prob);
	}
   return h1;
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

void scaleAllHistogramms(TFile* file, Int_t nofEvents)
{
   cout << "Scale all histogramms:" << endl;
   TDirectory * dir = gDirectory;
   TIter nextkey( dir->GetListOfKeys());
   TKey *key;

   Int_t count = 0;
   while (key = (TKey*) nextkey()) {
      TObject* obj = key->ReadObj();
      if (obj->IsA()->InheritsFrom (TH1::Class())) {
         TH1* h = (TH1*) obj;
         cout << h->GetName() << endl;
         TH1* h1 = (TH1*)file->Get(h->GetName());
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

    Bool_t fUseMvd = true;
    TString fileName = "/lustre/cbm/user/ebelolap/aug11/25gev/70field/mvd/phi/analysis.all.root";
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
    calculateSignalOverBg(fh_el_id_signal_minv, fh_el_id_bg_minv);
    calculateSignalOverBg(fh_gammacut_signal_minv, fh_gammacut_bg_minv);
    if (fUseMvd) calculateSignalOverBg(fh_dstscut_signal_minv, fh_dstscut_bg_minv);
    if (fUseMvd) calculateSignalOverBg(fh_dsts2cut_signal_minv, fh_dsts2cut_bg_minv);
    calculateSignalOverBg(fh_stcut_signal_minv, fh_stcut_bg_minv);
    calculateSignalOverBg(fh_ttcut_signal_minv, fh_ttcut_bg_minv);
    calculateSignalOverBg(fh_ptcut_signal_minv, fh_ptcut_bg_minv);
    calculateSignalOverBg(fh_anglecut_signal_minv, fh_anglecut_bg_minv);
    calculateSignalOverBg(fh_apmcut_signal_minv, fh_apmcut_bg_minv);

//SCALE ALL HISTOGRAMMS
    scaleAllHistogramms(file, fNofEvents);

    Int_t hi = 1;
// pty distribution of signal
    TCanvas *c2 = new TCanvas("c2-pty","c2-pty",750,1000);
    c2->Divide(3,4);
    c2->cd(hi++);
    fh_mc_signal_pty->Draw("COLZ");
    draw_eff_pty(fh_mc_signal_pty, fh_mc_signal_pty);
    c2->cd(hi++);
    fh_acc_signal_pty->Draw("COLZ");
    draw_eff_pty(fh_acc_signal_pty, fh_mc_signal_pty);
    c2->cd(hi++);
    fh_reco_signal_pty->Draw("COLZ");
    draw_eff_pty(fh_reco_signal_pty, fh_mc_signal_pty);
    c2->cd(hi++);
    fh_chi_prim_signal_pty->Draw("COLZ");
    draw_eff_pty(fh_chi_prim_signal_pty, fh_mc_signal_pty);
    c2->cd(hi++);
    fh_el_id_signal_pty->Draw("COLZ");
    draw_eff_pty(fh_el_id_signal_pty, fh_mc_signal_pty);
    c2->cd(hi++);
    fh_gammacut_signal_pty->Draw("COLZ");
    draw_eff_pty(fh_gammacut_signal_pty, fh_mc_signal_pty);
    if (fUseMvd) {
       c2->cd(hi++);
       fh_dstscut_signal_pty->Draw("COLZ");
       draw_eff_pty(fh_dstscut_signal_pty, fh_mc_signal_pty);
       c2->cd(hi++);
       fh_dsts2cut_signal_pty->Draw("COLZ");
       draw_eff_pty(fh_dsts2cut_signal_pty, fh_mc_signal_pty);
    }
    c2->cd(hi++);
    fh_stcut_signal_pty->Draw("COLZ");
    draw_eff_pty(fh_stcut_signal_pty, fh_mc_signal_pty);
    c2->cd(hi++);
    fh_ttcut_signal_pty->Draw("COLZ");
    draw_eff_pty(fh_ttcut_signal_pty, fh_mc_signal_pty);
    c2->cd(hi++);
    fh_ptcut_signal_pty->Draw("COLZ");
    draw_eff_pty(fh_ptcut_signal_pty, fh_mc_signal_pty);
    c2->cd(hi++);
    fh_anglecut_signal_pty->Draw("COLZ");
    draw_eff_pty(fh_anglecut_signal_pty, fh_mc_signal_pty);
    c2->cd(hi++);
   //fh_apmcut_signal_pty->Draw("COLZ");
   //draw_eff_pty(fh_apmcut_signal_pty, fh_mc_signal_pty);


    hi = 1;
//pty efficiency of signal
    TCanvas *c3 = new TCanvas("c3-pty eff","c3-pty eff",750,1000);
    c3->Divide(3,4);
    c3->cd(hi++);
    TH2D* fh_acc_signal_pty_eff = divideHisto2D(fh_acc_signal_pty, fh_mc_signal_pty);
    fh_acc_signal_pty_eff->Draw("COLZ");
    draw_eff_pty(fh_acc_signal_pty, fh_mc_signal_pty);

    c3->cd(hi++);
    TH2D* fh_reco_signal_pty_eff = divideHisto2D(fh_reco_signal_pty, fh_acc_signal_pty);
    fh_reco_signal_pty_eff->Draw("COLZ");
    draw_eff_pty(fh_reco_signal_pty, fh_acc_signal_pty);

    c3->cd(hi++);
    TH2D* fh_chi_prim_signal_pty_eff = divideHisto2D(fh_chi_prim_signal_pty, fh_reco_signal_pty);
    fh_chi_prim_signal_pty_eff->Draw("COLZ");
    draw_eff_pty(fh_chi_prim_signal_pty, fh_reco_signal_pty);

    c3->cd(hi++);
    TH2D* fh_el_id_signal_pty_eff = divideHisto2D(fh_el_id_signal_pty, fh_chi_prim_signal_pty);
    fh_el_id_signal_pty_eff->Draw("COLZ");
    draw_eff_pty(fh_el_id_signal_pty, fh_chi_prim_signal_pty);

    c3->cd(hi++);
    TH2D* fh_gammacut_signal_pty_eff = divideHisto2D(fh_gammacut_signal_pty, fh_el_id_signal_pty);
    fh_gammacut_signal_pty_eff->Draw("COLZ");
    draw_eff_pty(fh_gammacut_signal_pty, fh_el_id_signal_pty);

    if (fUseMvd) {
    c3->cd(hi++);
       TH2D* fh_dstscut_signal_pty_eff = divideHisto2D(fh_dstscut_signal_pty, fh_gammacut_signal_pty);
       fh_dstscut_signal_pty_eff->Draw("COLZ");
       draw_eff_pty(fh_dstscut_signal_pty, fh_gammacut_signal_pty);

       c3->cd(hi++);
       TH2D* fh_dsts2cut_signal_pty_eff = divideHisto2D(fh_dsts2cut_signal_pty, fh_dstscut_signal_pty);
       fh_dsts2cut_signal_pty_eff->Draw("COLZ");
       draw_eff_pty(fh_dsts2cut_signal_pty, fh_dstscut_signal_pty);
    }

    c3->cd(hi++);
    TH2D* fh_stcut_signal_pty_eff = divideHisto2D(fh_stcut_signal_pty, fh_dsts2cut_signal_pty);
    fh_stcut_signal_pty_eff->Draw("COLZ");
    draw_eff_pty(fh_stcut_signal_pty, fh_dstscut_signal_pty);

    c3->cd(hi++);
    TH2D* fh_ttcut_signal_pty_eff = divideHisto2D(fh_ttcut_signal_pty, fh_stcut_signal_pty);
    fh_ttcut_signal_pty_eff->Draw("COLZ");
    draw_eff_pty(fh_ttcut_signal_pty, fh_stcut_signal_pty);

    c3->cd(hi++);
    TH2D* fh_ptcut_signal_pty_eff = divideHisto2D(fh_ptcut_signal_pty, fh_ttcut_signal_pty);
    fh_ptcut_signal_pty_eff->Draw("COLZ");
    draw_eff_pty(fh_ptcut_signal_pty, fh_ttcut_signal_pty);

    c3->cd(hi++);
    TH2D* fh_anglecut_signal_pty_eff = divideHisto2D(fh_anglecut_signal_pty, fh_ptcut_signal_pty);
    fh_anglecut_signal_pty_eff->Draw("COLZ");
    draw_eff_pty(fh_anglecut_signal_pty, fh_ptcut_signal_pty);

    c3->cd(hi++);
    //TH2D* fh_apmcut_signal_pty_eff = divideHisto2D(fh_apmcut_signal_pty, fh_anglecut_signal_pty);
    //fh_apmcut_signal_pty_eff->Draw("COLZ");
    //draw_eff_pty(fh_apmcut_signal_pty, fh_anglecut_signal_pty);


//MOMENTUM DISTRIBUTION
    TCanvas *c4 = new TCanvas("c4-mom", "c4-mom", 400, 400);
    fh_mc_signal_mom->SetLineColor(kRed);
    fh_reco_signal_mom->SetLineColor(kBlue);
    fh_chi_prim_signal_mom->SetLineColor(kViolet);
    fh_el_id_signal_mom->SetLineColor(kGreen);
    if (fUseMvd) fh_dstscut_signal_mom->SetLineColor(kMagenta);
    if (fUseMvd) fh_dsts2cut_signal_mom->SetLineColor(kBlack);
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
    fh_el_id_signal_mom->Draw("same");
    if (fUseMvd) fh_dstscut_signal_mom->Draw("same");
    if (fUseMvd) fh_dsts2cut_signal_mom->Draw("same");
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
    leg3->AddEntry(fh_el_id_signal_mom, "rich id", "l");
    if (fUseMvd) leg3->AddEntry(fh_dstscut_signal_mom, "dsts cut", "l");
    if (fUseMvd) leg3->AddEntry(fh_dsts2cut_signal_mom, "dsts2 cut", "l");
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

/*
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
    TH1D* fh_el_id_signal_mom_eff = divideHisto1D(fh_el_id_signal_mom,fh_chi_prim_signal_mom);
    fh_el_id_signal_mom_eff->Draw();
    draw_eff_mom(fh_el_id_signal_mom,fh_chi_prim_signal_mom);
    gPad->SetGridx(true);
    gPad->SetGridy(true);

    c5->cd(5);


    c5->cd(6);


    c5->cd(7);
    TH1D* fh_gammacut_signal_mom_eff = divideHisto1D(fh_gammacut_signal_mom,fh_el_id_signal_mom);
    fh_gammacut_signal_mom_eff->Draw();
    draw_eff_mom(fh_gammacut_signal_mom,fh_el_id_signal_mom);
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
*/

//MOTHER PDG
    TCanvas *c6 = new TCanvas("c6-mother pdg", "c6-mother pdg", 500, 500);
    fh_mc_mother_pdg->Draw();
    fh_acc_mother_pdg->SetLineColor(kRed);
    fh_acc_mother_pdg->Draw("same");
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);

//ANALYSIS CUTS
    hi = 1;
    TCanvas *c7 = new TCanvas("c7-cuts", "c7-cuts", 1000, 1000);
    c7->Divide(4,3);
    c7->cd(hi++);
    Double_t scaleSig = 1./fh_chi2_prim_signal->Integral();
    Double_t scaleBg = 1./fh_chi2_prim_bg->Integral();
    fh_chi2_prim_signal->SetLineColor(kRed);
    fh_chi2_prim_bg->SetLineColor(kBlue);
    fh_chi2_prim_signal->SetLineWidth(2);
    fh_chi2_prim_bg->SetLineWidth(2);
    fh_chi2_prim_signal->Draw();
    fh_chi2_prim_bg->Draw("same");
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);
    c7->cd(hi++);
    TH1D* fh_sig_chi2_prim = CalculateSignificance(fh_chi2_prim_signal, fh_chi2_prim_bg,
        "chi2_prim_significance", "significance", "forward");
    fh_sig_chi2_prim->SetLineWidth(2);
    fh_sig_chi2_prim->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    fh_chi2_prim_signal->Scale(scaleSig);
    fh_chi2_prim_bg->Scale(scaleBg);

    c7->cd(hi++);
    Double_t scaleSig = 1./fh_pt_signal->Integral();
    Double_t scaleBg = 1./fh_pt_bg->Integral();
    fh_pt_signal->SetLineColor(kRed);
    fh_pt_bg->SetLineColor(kBlue);
    fh_pt_signal->SetLineWidth(2);
    fh_pt_bg->SetLineWidth(2);
    fh_pt_signal->Draw();
    fh_pt_bg->Draw("same");
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);
    c7->cd(hi++);
    TH1D* fh_sig_pt = CalculateSignificance(fh_pt_signal, fh_pt_bg , "pt_significance", "significance", "back");
    fh_sig_pt->SetLineWidth(2);
    fh_sig_pt->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    fh_pt_signal->Scale(scaleSig);
    fh_pt_bg->Scale(scaleBg);

    c7->cd(hi++);
    Double_t scaleSig = 1./fh_angle_signal->Integral();
    Double_t scaleBg = 1./fh_angle_bg->Integral();
    fh_angle_signal->SetLineWidth(2);
    fh_angle_bg->SetLineWidth(2);
    fh_angle_signal->SetLineColor(kRed);
    fh_angle_bg->SetLineColor(kBlue);
    fh_angle_signal->Draw();
    fh_angle_bg->Draw("same");
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);
    c7->cd(hi++);
    TH1D* fh_sig_angle = CalculateSignificance(fh_angle_signal,fh_angle_bg, "angle_significance", "significance", "back");
    fh_sig_angle->SetLineWidth(2);
    fh_sig_angle->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    fh_angle_signal->Scale(scaleSig);
    fh_angle_bg->Scale(scaleBg);

    c7->cd(hi++);
    Double_t scaleSig = 1./fh_mom_signal->Integral();
    Double_t scaleBg = 1./fh_mom_bg->Integral();
    fh_mom_signal->SetLineWidth(2);
    fh_mom_bg->SetLineWidth(2);
    fh_mom_signal->SetLineColor(kRed);
    fh_mom_bg->SetLineColor(kBlue);
    fh_mom_signal->Draw();
    fh_mom_bg->Draw("same"); 
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);
    c7->cd(hi++);
    TH1D* fh_sig_mom = CalculateSignificance(fh_mom_signal,fh_mom_bg, "momentum_significance", "significance", "back");
    fh_sig_mom->SetLineWidth(2);
    fh_sig_mom->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    fh_mom_signal->Scale(scaleSig);
    fh_mom_bg->Scale(scaleBg);

    c7->cd(hi++);
    Double_t scaleSig = 1./fh_chi2sts_signal->Integral();
    Double_t scaleBg = 1./fh_chi2sts_bg->Integral();
    fh_chi2sts_signal->SetLineWidth(2);
    fh_chi2sts_bg->SetLineWidth(2);
    fh_chi2sts_signal->SetLineColor(kRed);
    fh_chi2sts_bg->SetLineColor(kBlue);
    fh_chi2sts_signal->Draw();
    fh_chi2sts_bg->Draw("same");
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);
    c7->cd(hi++);
    TH1D* fh_sig_chi2sts = CalculateSignificance(fh_chi2sts_signal,fh_chi2sts_bg, "chi2sts_significance", "significance", "forward");
    fh_sig_chi2sts->SetLineWidth(2);
    fh_sig_chi2sts->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    fh_chi2sts_signal->Scale(scaleSig);
    fh_chi2sts_bg->Scale(scaleBg);

//SOURCE OF BG CANDIDATES
    fh_source_pair_reco->Scale(100. /fh_source_pair_reco->Integral());
    fh_source_pair_chi_prim->Scale(100. /fh_source_pair_chi_prim->Integral());
    fh_source_pair_el_id->Scale(100. /fh_source_pair_el_id->Integral());
    fh_source_pair_gammacut->Scale(100. /fh_source_pair_gammacut->Integral());
    fh_source_pair_dstscut->Scale(100. /fh_source_pair_dstscut->Integral());
    fh_source_pair_dsts2cut->Scale(100. /fh_source_pair_dsts2cut->Integral());
    fh_source_pair_stcut->Scale(100. /fh_source_pair_stcut->Integral());
    fh_source_pair_ttcut->Scale(100. /fh_source_pair_ttcut->Integral());
    fh_source_pair_ptcut->Scale(100. /fh_source_pair_ptcut->Integral());
    fh_source_pair_anglecut->Scale(100. /fh_source_pair_anglecut->Integral());
    //fh_source_pair_apmcut->Scale(100. /fh_source_pair_apmcut->Integral());

    hi = 1;
    TCanvas *c8 = new TCanvas("c8-pair sources", "c8-pair sources", 1000, 750);
    c8->Divide(4,3);
    c8->cd(hi++);
    fh_source_pair_reco->Draw("COLZ");
    c8->cd(hi++);
    fh_source_pair_chi_prim->Draw("COLZ");
    c8->cd(hi++);
    fh_source_pair_el_id->Draw("COLZ");
    c8->cd(hi++);
    fh_source_pair_gammacut->Draw("COLZ");
    if (fUseMvd){
       c8->cd(hi++);
       fh_source_pair_dstscut->Draw("COLZ");
       c8->cd(hi++);
       fh_source_pair_dsts2cut->Draw("COLZ");
    }
    c8->cd(hi++);
    fh_source_pair_stcut->Draw("COLZ");
    c8->cd(hi++);
    fh_source_pair_ttcut->Draw("COLZ");
    c8->cd(hi++);
    fh_source_pair_ptcut->Draw("COLZ");
    c8->cd(hi++);
    fh_source_pair_anglecut->Draw("COLZ");
    c8->cd(hi++);
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
    apmcut_py_s->SetLineWidth(2);
    apmcut_py_bg->SetLineWidth(2);
    apmcut_py_s->SetLineColor(kRed);
    apmcut_py_bg->SetLineColor(kBlue);
    apmcut_py_s->Draw();
    apmcut_py_bg->Draw("same");
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);
    c10->cd(4);
    TH1D* fh_sig_apmcut_py = CalculateSignificance(apmcut_py_s, apmcut_py_bg, 
        "apmcut_py_significance", "significance;p_{t} [GeV/c];significance", "back");
    fh_sig_apmcut_py->SetLineWidth(2);
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
    stcut_px_s->SetLineWidth(2);
    stcut_px_bg->SetLineWidth(2);
    stcut_px_s->SetLineColor(kRed);
    stcut_px_bg->SetLineColor(kBlue);
    stcut_px_s->Draw();
    stcut_px_bg->Draw("same");
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);
    c11->cd(6);
    TH1D* fh_sig_stcut_px = CalculateSignificance(stcut_px_s, stcut_px_bg, "stcut_px_significance",
          "significance;#sqrt{p_{e^{#pm}} p_{rec}} [GeV/c];significance", "back");
    fh_sig_stcut_px->SetLineWidth(2);
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
    stcut_py_s->SetLineWidth(2);
    stcut_py_bg->SetLineWidth(2);
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
    TH1D* fh_sig_stcut_py = CalculateSignificance(stcut_py_s, stcut_py_bg,"stcut_py_significance",
          "significance;#theta_{e^{#pm},rec} [deg];significance", "back");
    fh_sig_stcut_py->SetLineWidth(2);
    fh_sig_stcut_py->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);

//   c11->cd(9);
//   TH2D* fh_significance = CalculateSignificance2D(fh_stcut_signal, fh_stcut_bg, "stcut_2dsignificance", "significance");
//   fh_significance->Draw("COLZ");


//TRACK TOPOLOGY CUT DISTRIBUTION FOR FULL RECO TRACKS
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
    ttcut_px_s->SetLineWidth(2);
    ttcut_px_bg->SetLineWidth(2);
    ttcut_px_s->SetLineColor(kRed);
    ttcut_px_bg->SetLineColor(kBlue);
    ttcut_px_s->Draw();
    ttcut_px_bg->Draw("same");
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);
    c12->cd(6);
    TH1D* fh_sig_ttcut_px = CalculateSignificance(ttcut_px_s, ttcut_px_bg, 
        "ttcut_px_significance", "significance;#sqrt{p_{e^{#pm}} p_{rec}} [GeV/c];significance", "back");
    fh_sig_ttcut_px->SetLineWidth(2);
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
    ttcut_py_s->SetLineWidth(2);
    ttcut_py_bg->SetLineWidth(2);
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
        "ttcut_py_significance", "significance;#theta_{e^{#pm},rec} [deg];significance", "back");
    fh_sig_ttcut_py->SetLineWidth(2);
    fh_sig_ttcut_py->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);

 //   c12->cd(9);
//    TH2D* fh_significance_tt = CalculateSignificance2D(fh_ttcut_signal, fh_ttcut_bg, "ttcut_2dsignificance", "significance");
//    fh_significance_tt->Draw("COLZ");



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
    fh_rich_ann_signal->SetLineWidth(2);
    fh_rich_ann_bg->SetLineWidth(2);
    fh_rich_ann_signal->SetLineColor(kRed);
    fh_rich_ann_bg->SetLineColor(kBlue);
    fh_rich_ann_signal->Draw();
    fh_rich_ann_bg->Draw("same");
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);
    c13->cd(2);
    TH1D* fh_sig_rich_ann = CalculateSignificance(fh_rich_ann_signal, fh_rich_ann_bg, 
        "rich_ann_significance", "significance;ann output;significance", "back");
    fh_sig_rich_ann->SetLineWidth(2);
    fh_sig_rich_ann->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    fh_rich_ann_signal->Scale(scaleSig);
    fh_rich_ann_bg->Scale(scaleBg);

    c13->cd(3);
    Double_t scaleSig = 1./fh_trd_ann_signal->Integral();
    Double_t scaleBg = 1./fh_trd_ann_bg->Integral();
    fh_trd_ann_signal->SetLineWidth(2);
    fh_trd_ann_bg->SetLineWidth(2);
    fh_trd_ann_signal->SetLineColor(kRed);
    fh_trd_ann_bg->SetLineColor(kBlue);
    fh_trd_ann_signal->Draw();
    fh_trd_ann_bg->Draw("same");
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);
    c13->cd(4);
    TH1D* fh_sig_trd_ann = CalculateSignificance(fh_trd_ann_signal, fh_trd_ann_bg, 
        "trd_ann_significance", "significance;ann output;significance", "back");
    fh_sig_trd_ann->SetLineWidth(2);
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

//DSTS CUT
    if (fUseMvd) {
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
       c14->cd(6);
    //   TH2D* fh_significance_dsts = CalculateSignificance2D(fh_dsts_signal, fh_dsts_bg, "dsts_2dsignificance", "significance");
   //    fh_significance_dsts->Draw("COLZ");
    }

//INVARIANT MASS DISTRIBUTION
    Int_t nRebin = 10;
    TCanvas *c15 = new TCanvas("c15-minv", "c15-minv", 1200, 600);
    c15->Divide(2,1);
    c15->cd(1);

    fh_mc_signal_minv->Rebin(nRebin);
    fh_acc_signal_minv->Rebin(nRebin);
    fh_rec_signal_minv->Rebin(nRebin);
    fh_chi_prim_signal_minv->Rebin(nRebin);
    fh_el_id_signal_minv->Rebin(nRebin);
    if (fUseMvd) fh_dstscut_signal_minv->Rebin(nRebin);
    if (fUseMvd) fh_dsts2cut_signal_minv->Rebin(nRebin);
    fh_gammacut_signal_minv->Rebin(nRebin);
    fh_stcut_signal_minv->Rebin(nRebin);
    fh_ttcut_signal_minv->Rebin(nRebin);
    fh_ptcut_signal_minv->Rebin(nRebin);
//    fh_anglecut_signal_minv->Rebin(nRebin);
//   fh_apmcut_signal_minv->Rebin(nRebin);

    fh_mc_signal_minv->SetLineColor(kGreen+3);
    fh_acc_signal_minv->SetLineColor(kOrange+3);
    fh_chi_prim_signal_minv->SetLineColor(kOrange+7);
    fh_el_id_signal_minv->SetLineColor(kRed);
    if (fUseMvd) fh_dstscut_signal_minv->SetLineColor(kGreen);
    if (fUseMvd) fh_dsts2cut_signal_minv->SetLineColor(kBlack);
    fh_gammacut_signal_minv->SetLineColor(kPink-6);
    fh_stcut_signal_minv->SetLineColor(kOrange-3);
    fh_ttcut_signal_minv->SetLineColor(kYellow+1);
    fh_ptcut_signal_minv->SetLineColor(kMagenta);
 //   fh_anglecut_signal_minv->SetLineColor(kViolet+10);
 //   fh_apmcut_signal_minv->SetLineColor(kPink+8);

    fh_mc_signal_minv->Draw();
    fh_acc_signal_minv->Draw("same");
    fh_rec_signal_minv->Draw("same");
    fh_chi_prim_signal_minv->Draw("same");
    fh_el_id_signal_minv->Draw("same");
    if (fUseMvd) fh_dstscut_signal_minv->Draw("same");
    if (fUseMvd) fh_dsts2cut_signal_minv->Draw("same");
    fh_gammacut_signal_minv->Draw("same");
    fh_stcut_signal_minv->Draw("same");
    fh_ttcut_signal_minv->Draw("same");
    fh_ptcut_signal_minv->Draw("same");
//    fh_anglecut_signal_minv->Draw("same");
 //   fh_apmcut_signal_minv->Draw("same");

    TLegend* leg3 = new TLegend(0.65,0.6,1., 1.);
    leg3->AddEntry(fh_mc_signal_minv, "mc", "l");
    leg3->AddEntry(fh_acc_signal_minv, "acc", "l");
    leg3->AddEntry(fh_rec_signal_minv, "rec", "l");
    leg3->AddEntry(fh_chi_prim_signal_minv, "chi prim", "l");
    leg3->AddEntry(fh_el_id_signal_minv, "rich id", "l");
    leg3->AddEntry(fh_gammacut_signal_minv, "gamma cut", "l");
    if (fUseMvd) leg3->AddEntry(fh_dstscut_signal_minv, "dsts cut", "l");
    if (fUseMvd) leg3->AddEntry(fh_dsts2cut_signal_minv, "dsts2 cut", "l");
    leg3->AddEntry(fh_stcut_signal_minv, "st cut", "l");
    leg3->AddEntry(fh_ttcut_signal_minv, "tt cut", "l");
    leg3->AddEntry(fh_ptcut_signal_minv, "pt cut", "l");
//    leg3->AddEntry(fh_anglecut_signal_minv, "angle cut", "l");
 //   leg3->AddEntry(fh_apmcut_signal_minv, "apm cut", "l");

    leg3->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);

    c15->cd(2);
    fh_el_id_bg_minv->SetLineColor(kRed);
    fh_chi_prim_bg_minv->SetLineColor(kOrange+7);
    fh_gammacut_bg_minv->SetLineColor(kPink-6);
    if (fUseMvd) fh_dstscut_bg_minv->SetLineColor(kGreen);
    if (fUseMvd) fh_dsts2cut_bg_minv->SetLineColor(kBlack);
    fh_stcut_bg_minv->SetLineColor(kOrange-3);
    fh_ttcut_bg_minv->SetLineColor(kYellow+1);
    fh_ptcut_bg_minv->SetLineColor(kMagenta);
//    fh_anglecut_bg_minv->SetLineColor(kViolet+10);
 //   fh_apmcut_bg_minv->SetLineColor(kPink+8);

    fh_rec_bg_minv->Rebin(nRebin);
    fh_chi_prim_bg_minv->Rebin(nRebin);
    fh_el_id_bg_minv->Rebin(nRebin);
    if (fUseMvd) fh_dstscut_bg_minv->Rebin(nRebin);
    if (fUseMvd) fh_dsts2cut_bg_minv->Rebin(nRebin);
    fh_gammacut_bg_minv->Rebin(nRebin);
    fh_stcut_bg_minv->Rebin(nRebin);
    fh_ttcut_bg_minv->Rebin(nRebin);
    fh_ptcut_bg_minv->Rebin(nRebin);
//    fh_anglecut_bg_minv->Rebin(nRebin);
 //   fh_apmcut_bg_minv->Rebin(nRebin);

    fh_rec_bg_minv->Draw();
    fh_rec_bg_minv->SetMinimum(1e-8);
    fh_chi_prim_bg_minv->Draw("same");
    fh_el_id_bg_minv->Draw("same");
    if (fUseMvd) fh_dstscut_bg_minv->Draw("same");
    if (fUseMvd) fh_dsts2cut_bg_minv->Draw("same");
    fh_gammacut_bg_minv->Draw("same");
    fh_stcut_bg_minv->Draw("same");
    fh_ttcut_bg_minv->Draw("same");
    fh_ptcut_bg_minv->Draw("same");
//    fh_anglecut_bg_minv->Draw("same");
//    fh_apmcut_bg_minv->Draw("same");

    TLegend* leg = new TLegend(0.65,0.6,1., 1.);
    leg->AddEntry(fh_rec_bg_minv, "rec", "l");
    leg->AddEntry(fh_chi_prim_bg_minv, "chi prim", "l");
    leg->AddEntry(fh_el_id_bg_minv, "rich id", "l");
    leg->AddEntry(fh_gammacut_bg_minv, "gamma cut", "l");
    if (fUseMvd) leg->AddEntry(fh_dstscut_bg_minv, "dsts cut", "l");
    if (fUseMvd) leg->AddEntry(fh_dsts2cut_bg_minv, "dsts2 cut", "l");
    leg->AddEntry(fh_stcut_bg_minv, "st cut", "l");
    leg->AddEntry(fh_ttcut_bg_minv, "tt cut", "l");
    leg->AddEntry(fh_ptcut_bg_minv, "pt cut", "l");
 //   leg->AddEntry(fh_anglecut_bg_minv, "angle cut", "l");
//    leg->AddEntry(fh_apmcut_bg_minv, "apm cut", "l");

    leg->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);
    gPad->SetLogy(true);

//INVARIANT MASS DISTRIBUTION
   hi = 1;
	TCanvas *c16 = new TCanvas("c16-minv", "c16-minv", 1200, 900);
	c16->Divide(4,3);
	c16->cd(hi++);
	draw_minv(fh_rec_signal_minv,fh_rec_bg_minv);
   c16->cd(hi++);
   draw_minv(fh_chi_prim_signal_minv,fh_chi_prim_bg_minv);
	c16->cd(hi++);
	draw_minv(fh_el_id_signal_minv,fh_el_id_bg_minv);
	c16->cd(hi++);
	draw_minv(fh_gammacut_signal_minv,fh_gammacut_bg_minv);
	if (fUseMvd) {
      c16->cd(hi++);
      draw_minv(fh_dstscut_signal_minv,fh_dstscut_bg_minv);
      c16->cd(hi++);
      draw_minv(fh_dsts2cut_signal_minv,fh_dsts2cut_bg_minv);
	}
	c16->cd(hi++);
	draw_minv(fh_stcut_signal_minv,fh_stcut_bg_minv);
	c16->cd(hi++);
	draw_minv(fh_ttcut_signal_minv,fh_ttcut_bg_minv);
	c16->cd(hi++);
	draw_minv(fh_ptcut_signal_minv,fh_ptcut_bg_minv);
	c16->cd(hi++);
//	draw_minv(fh_anglecut_signal_minv,fh_anglecut_bg_minv);
    c16->cd(hi++);
//    draw_minv(fh_apmcut_signal_minv,fh_apmcut_bg_minv);

//INVARIANT MASS DISTRIBUTION FOR PI0 AND ETA
    TCanvas *c17 = new TCanvas("c17-minv-pi0-eta", "c17-minv-pi0-eta", 1200, 600);
    c17->Divide(2,1);
    c17->cd(1);
    fh_el_id_pi0_minv->SetLineColor(kRed);
    if (fUseMvd) fh_dstscut_pi0_minv->SetLineColor(kGreen);
    fh_chi_prim_pi0_minv->SetLineColor(kOrange+7);
    fh_gammacut_pi0_minv->SetLineColor(kPink-6);
    fh_stcut_pi0_minv->SetLineColor(kOrange-3);
    fh_ttcut_pi0_minv->SetLineColor(kYellow+1);
    fh_ptcut_pi0_minv->SetLineColor(kMagenta);
    fh_anglecut_pi0_minv->SetLineColor(kViolet+10);
 //   fh_apmcut_pi0_minv->SetLineColor(kPink+8);

    fh_rec_pi0_minv->Rebin(nRebin);
    fh_el_id_pi0_minv->Rebin(nRebin);
    if (fUseMvd) fh_dstscut_pi0_minv->Rebin(nRebin);
    fh_chi_prim_pi0_minv->Rebin(nRebin);
    fh_gammacut_pi0_minv->Rebin(nRebin);
    fh_stcut_pi0_minv->Rebin(nRebin);
    fh_ttcut_pi0_minv->Rebin(nRebin);
    fh_ptcut_pi0_minv->Rebin(nRebin);
    fh_anglecut_pi0_minv->Rebin(nRebin);
 //   fh_apmcut_pi0_minv->Rebin(nRebin);

    fh_rec_pi0_minv->Draw();
    fh_chi_prim_pi0_minv->Draw("same");
    fh_el_id_pi0_minv->Draw("same");
    if (fUseMvd) fh_dstscut_pi0_minv->Draw("same");
    fh_gammacut_pi0_minv->Draw("same");
    fh_stcut_pi0_minv->Draw("same");
    fh_ttcut_pi0_minv->Draw("same");
    fh_ptcut_pi0_minv->Draw("same");
    fh_anglecut_pi0_minv->Draw("same");
 //   fh_apmcut_pi0_minv->Draw("same");

    TLegend* leg3 = new TLegend(0.65,0.6,1., 1.);
    leg3->AddEntry(fh_rec_pi0_minv, "rec", "l");
    leg3->AddEntry(fh_chi_prim_pi0_minv, "chi prim", "l");
	leg3->AddEntry(fh_el_id_pi0_minv, "rich id", "l");
    leg3->AddEntry(fh_gammacut_pi0_minv, "gamma cut", "l");
    if (fUseMvd) leg3->AddEntry(fh_dstscut_pi0_minv, "dsts cut", "l");
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
	fh_el_id_eta_minv->SetLineColor(kRed);
	if (fUseMvd) fh_dstscut_eta_minv->SetLineColor(kGreen);
	fh_chi_prim_eta_minv->SetLineColor(kOrange+7);
	fh_gammacut_eta_minv->SetLineColor(kPink-6);
    fh_stcut_eta_minv->SetLineColor(kOrange-3);
	fh_ttcut_eta_minv->SetLineColor(kYellow+1); 
    fh_ptcut_eta_minv->SetLineColor(kMagenta);
    fh_anglecut_eta_minv->SetLineColor(kViolet+10);
//	fh_apmcut_eta_minv->SetLineColor(kPink+8);
    
   fh_rec_eta_minv->Rebin(nRebin);
   fh_el_id_eta_minv->Rebin(nRebin);
   if (fUseMvd) fh_dstscut_eta_minv->Rebin(nRebin);
   fh_chi_prim_eta_minv->Rebin(nRebin);
   fh_gammacut_eta_minv->Rebin(nRebin);
   fh_stcut_eta_minv->Rebin(nRebin);
   fh_ttcut_eta_minv->Rebin(nRebin);
   fh_ptcut_eta_minv->Rebin(nRebin);
   fh_anglecut_eta_minv->Rebin(nRebin);
//  fh_apmcut_eta_minv->Rebin(nRebin);

	fh_rec_eta_minv->Draw();
   fh_chi_prim_eta_minv->Draw("same");
	fh_el_id_eta_minv->Draw("same");
	if (fUseMvd) fh_dstscut_eta_minv->Draw("same");
	fh_gammacut_eta_minv->Draw("same");
   fh_stcut_eta_minv->Draw("same");
	fh_ttcut_eta_minv->Draw("same");
    fh_ptcut_eta_minv->Draw("same");
    fh_anglecut_eta_minv->Draw("same");
//	fh_apmcut_eta_minv->Draw("same");

	TLegend* leg4 = new TLegend(0.65,0.6,1., 1.);
	leg4->AddEntry(fh_rec_eta_minv, "rec", "l");
   leg4->AddEntry(fh_chi_prim_eta_minv, "chi prim", "l");
	leg4->AddEntry(fh_el_id_eta_minv, "rich id", "l");
   leg4->AddEntry(fh_gammacut_eta_minv, "gamma cut", "l");
   if (fUseMvd) leg4->AddEntry(fh_dstscut_eta_minv, "dsts cut", "l");
   leg4->AddEntry(fh_stcut_eta_minv, "st cut", "l");
   leg4->AddEntry(fh_ttcut_eta_minv, "tt cut", "l");
	leg4->AddEntry(fh_ptcut_eta_minv, "pt cut", "l");
   leg4->AddEntry(fh_anglecut_eta_minv, "angle cut", "l");
//   leg4->AddEntry(fh_apmcut_eta_minv, "apm cut", "l");
	leg4->Draw();
	gPad->SetGridx(true);
	gPad->SetGridy(true);
	gPad->SetLogy(true);

	//SOURCE TRACKS
	TCanvas *c18 = new TCanvas("c18-nof-tracks", "c18-nof-tracks", 1200, 800);
	c18->Divide(3,2);
	c18->cd(1);
	fh_nof_bg_tracks->SetLineWidth(2);
	fh_nof_bg_tracks->Draw("hist text0");
	gPad->SetGridx(true);
	gPad->SetGridy(true);
	c18->cd(2);
	fh_nof_el_tracks->SetLineWidth(2);
	fh_nof_el_tracks->Draw("hist text0");
	gPad->SetGridx(true);
	gPad->SetGridy(true);
	c18->cd(3);
	TH1D* purity = new TH1D("purity","purity",7, 0., 7.);
	purity->Divide(fh_nof_bg_tracks,fh_nof_el_tracks);
	purity->SetLineWidth(2);
	purity->Draw("hist text0");
	gPad->SetGridx(true);
	gPad->SetGridy(true);
	c18->cd(4);
	TH2D * source_tracks_clone = fh_source_tracks->Clone();
	Int_t nBinsX = source_tracks_clone->GetNbinsX();
	Int_t nBinsY = source_tracks_clone->GetNbinsY();
	for (Int_t x = 1; x <= nBinsX; x++){
		Double_t scale =100./(fh_nof_bg_tracks->GetBinContent(x));
		for (Int_t y = 1; y <= nBinsY; y++){
			Double_t val = scale * source_tracks_clone->GetBinContent(x,y);
			source_tracks_clone->SetBinContent(x,y,val);
		}
	}
	Int_t nx = 4;
	char* xLabels[4] = {"reco", "chiPrim", "el id", "gammacut"};
	for (Int_t x = 1; x <= nx; x++){
	   source_tracks_clone->GetXaxis()->SetBinLabel(x,xLabels[x-1]);
	}
   Int_t ny = 6;
   char* yLabels[6] = {"gamma", "pi0", "pions", "protons", "kaons", "other"};
   for (Int_t y = 1; y <= ny; y++){
      source_tracks_clone->GetYaxis()->SetBinLabel(y,yLabels[y-1]);
   }
	source_tracks_clone->SetMarkerColor(0);
	source_tracks_clone->Draw("text COLZ");

//MINV vs. PT
	TCanvas *c19 = new TCanvas("c19-minv-pt", "c19-minv-pt",500, 500);
	fh_ttcut_signal_minv_pt->Draw("COLZ");

//DSTS 2 CUT
	if (fUseMvd) {
      TCanvas *c20 = new TCanvas("c20-dsts-2MVD", "c20-dsts-2MVD", 1200, 800);
      c20->Divide(3,2);
      c20->cd(1);
      fh_dsts2_signal->Draw("COLZ");
      c20->cd(2);
      fh_dsts2_bg->Draw("COLZ");
      c20->cd(3);
      fh_dsts2_gamma->Draw("COLZ");
      c20->cd(4);
      fh_dsts2_pi0->Draw("COLZ");
      c20->cd(5);
      fh_dsts2_eta->Draw("COLZ");
	}

	TCanvas *c21 = new TCanvas("c21-bg-sources-vs-mom","c21-bg-sources-vs-mom",1200, 800);
	c21->Divide(3,2);
	c21->cd(1);
	fh_gamma_mom->Draw();
	c21->cd(2);
	fh_pi0_mom->Draw();
	c21->cd(3);
	fh_pions_mom->Draw();
	c21->cd(4);
	fh_proton_mom->Draw();
	c21->cd(5);
	fh_kaons_mom->Draw();
	c21->cd(5);
	fh_other_mom->Draw();

	TCanvas *c22 = new TCanvas("c22-cumulative-probability","c22-cumulative-probability",1200, 800);
	TH1D* fh_prob_chi2_signal = CalculateCumulativeProbability(fh_chi2_prim_signal, "chi2_signal_cum_prob", "chi2_signal_cum_prob");
	TH1D* fh_prob_chi2_bg = CalculateCumulativeProbability(fh_chi2_prim_bg, "chi2_bg_cum_prob", "chi2_bg_cum_prob");
	TH1D* fh_prob_pt_signal = CalculateCumulativeProbability(fh_pt_signal, "pt_signal_cum_prob", "pt_signal_cum_prob");
	TH1D* fh_prob_pt_bg = CalculateCumulativeProbability(fh_pt_bg, "pt_bg_cum_prob", "pt_bg_cum_prob");
	TH1D* fh_prob_angle_signal = CalculateCumulativeProbability(fh_angle_signal, "angle_signal_cum_prob", "angle_signal_cum_prob");
	TH1D* fh_prob_angle_bg = CalculateCumulativeProbability(fh_angle_bg, "angle_bg_cum_prob", "angle_bg_cum_prob");
	TH1D* fh_prob_mom_signal = CalculateCumulativeProbability(fh_mom_signal, "mom_signal_cum_prob", "mom_signal_cum_prob");
	TH1D* fh_prob_mom_bg = CalculateCumulativeProbability(fh_mom_bg, "mom_bg_cum_prob", "mom_bg_cum_prob");
	c22->Divide(3,2);
	c22->cd(1);
	fh_prob_chi2_signal->SetLineColor(kRed);
	fh_prob_chi2_signal->Draw();
	fh_prob_chi2_bg->SetLineColor(kBlue);
	fh_prob_chi2_bg->Draw("same");
	c22->cd(2);
	fh_prob_pt_signal->SetLineColor(kRed);
	fh_prob_pt_bg->SetLineColor(kBlue);
	fh_prob_pt_signal->Draw();
	fh_prob_pt_bg->Draw("same");
	c22->cd(3);
	fh_prob_angle_signal->SetLineColor(kRed);
	fh_prob_angle_bg->SetLineColor(kBlue);
	fh_prob_angle_signal->Draw();
	fh_prob_angle_bg->Draw("same");
	c22->cd(4);
	fh_prob_mom_signal->SetLineColor(kRed);
	fh_prob_mom_bg->SetLineColor(kBlue);
	fh_prob_mom_signal->Draw();
	fh_prob_mom_bg->Draw("same");

}
