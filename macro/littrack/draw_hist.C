
void draw_hist_1D(
		TH1* hist,
		const std::string& titleX,
		const std::string& titleY,
		Int_t color = kBlack,
		Int_t lineWidth = 1,
		Int_t lineStyle = 1,
		Int_t markerSize = 1,
		Int_t markerStyle = kDot,
		Bool_t logx = false,
		Bool_t logy = false,
		const std::string& drawOpt = ""
		)
{
	hist->GetXaxis()->SetTitle(titleX.c_str());
	hist->GetYaxis()->SetTitle(titleY.c_str());
	hist->SetLineColor(color);
	hist->SetLineWidth(lineWidth);
	hist->SetLineStyle(lineStyle);
	hist->SetMarkerColor(color);
	hist->SetMarkerSize(markerSize);
	hist->SetMarkerStyle(markerStyle);
	if (logx) gPad->SetLogx();
	if (logy) gPad->SetLogy();
	hist->Draw(drawOpt.c_str());
}


void draw_hist_2D(
		TH2* hist,
		const std::string& titleX,
		const std::string& titleY,
		const std::string& titleZ,
		Bool_t logx = false,
		Bool_t logy = false,
		Bool_t logz = false,
		const std::string& drawOpt = ""
		)
{
	hist->GetXaxis()->SetTitle(titleX.c_str());
	hist->GetYaxis()->SetTitle(titleY.c_str());
	hist->GetYaxis()->SetTitle(titleZ.c_str());
	if (logx) gPad->SetLogx();
	if (logy) gPad->SetLogy();
	if (logz) gPad->SetLogz();
	hist->Draw(drawOpt.c_str());
}

