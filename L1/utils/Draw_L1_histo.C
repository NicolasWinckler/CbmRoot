#include <unistd.h> // for dir navigation

const int textFont = 22; // TNewRoman
const bool divide = 0; // = 0 - each histo in separate file\screen. = 1 - all in one file\screen

void makeUpHisto(TH1* hist, TString title, float* sigma = 0)
{
  if (hist && (hist->GetEntries() != 0)){
    TF1  *fit = new TF1("fit","gaus");
    fit->SetLineColor(2);
    fit->SetLineWidth(3);
    hist->Fit("fit","","",hist->GetXaxis()->GetXmin(),hist->GetXaxis()->GetXmax());
    if (sigma) *sigma = fit->GetParameter(2);

    hist->GetXaxis()->SetLabelFont(textFont);
    hist->GetXaxis()->SetTitleFont(textFont);
    hist->GetYaxis()->SetLabelFont(textFont);
    hist->GetYaxis()->SetTitleFont(textFont);
    
    hist->GetXaxis()->SetTitle(title);
    hist->GetXaxis()->SetTitleOffset(1);
    hist->GetYaxis()->SetTitle("Entries");
    hist->GetYaxis()->SetTitleOffset(1.05); // good then entries per bit <= 9999
  }
  else{
    std::cout << " E: Read hists error! " << std::endl;
  }
}

void makeUpProfile(TProfile* prof, TString titleX, TString titleY)
{
  if (prof && (prof->GetEntries() != 0)){
    prof->SetMarkerColor(kRed);
    prof->SetLineColor(kRed);
    
    prof->GetXaxis()->SetTitle(titleX);
    prof->GetXaxis()->SetTitleOffset(1);
    prof->GetYaxis()->SetTitle(titleY);
    prof->GetYaxis()->SetTitleOffset(0.85);
      
    prof->GetXaxis()->SetLabelFont(textFont);
    prof->GetXaxis()->SetTitleFont(textFont);
    prof->GetYaxis()->SetLabelFont(textFont);
    prof->GetYaxis()->SetTitleFont(textFont);
  }
  else{
    std::cout << " E: Read profile error! " << std::endl;
  }
}

void Draw_L1_histo() {

    // ============================ Set Styles ============================
  TStyle *histoStyle = new TStyle("histoStyle","Plain Style(no colors/fill areas)");
//   TStyle *histoStyle = gStyle;
  
  histoStyle->SetTextFont(textFont);
  histoStyle->SetPadColor(0);
  histoStyle->SetCanvasColor(0);
  histoStyle->SetTitleColor(0);
  histoStyle->SetStatColor(0);
  
  histoStyle->SetOptTitle(0); // without main up title
  histoStyle->SetOptStat(1000001010);
//   The parameter mode can be = ksiourmen  (default = 000001111)
//   k = 1;  kurtosis printed
//   k = 2;  kurtosis and kurtosis error printed
//   s = 1;  skewness printed
//   s = 2;  skewness and skewness error printed
//   i = 1;  integral of bins printed
//   o = 1;  number of overflows printed
//   u = 1;  number of underflows printed
//   r = 1;  rms printed
//   r = 2;  rms and rms error printed
//   m = 1;  mean value printed
//   m = 2;  mean and mean error values printed
//   e = 1;  number of entries printed
//   n = 1;  name of histogram is printed
  
  histoStyle->SetOptFit(10001);
//   The parameter mode can be = pcev  (default = 0111)
//   p = 1;  print Probability
//   c = 1;  print Chisquare/Number of degress of freedom
//   e = 1;  print errors (if e=1, v must be 1)
//   v = 1;  print name/values of parameters
  
  histoStyle->SetStatW(0.175);
  histoStyle->SetStatH(0.02);
  histoStyle->SetStatX(0.95);
  histoStyle->SetStatY(0.97);
  histoStyle->SetStatFontSize(0.05);

  histoStyle->SetStatFont(textFont);


  
  TStyle *profStyle = new TStyle("profStyle","Plain Style(no colors/fill areas)");
//   TStyle *profStyle = gStyle;
  
  profStyle->SetTextFont(textFont);
  profStyle->SetPadColor(0);
  profStyle->SetCanvasColor(0);
  profStyle->SetTitleColor(0);
  profStyle->SetStatColor(0);
  
  profStyle->SetOptTitle(0); // without main up title
  profStyle->SetOptStat(0);
  profStyle->SetOptFit(0);


    // ============================ Open file ============================

  const TString fileName = "L1_histo.root";

  TFile *fileIn = new TFile(fileName.Data(),"read");

  FILE *ress, *pulls;
  ress = fopen("resolution.txt","w");
  pulls = fopen("pull.txt","w");


    // ------------ make histos -----------


      // ------------ tracks -----------
  
  TString dirFitName = "/L1/Fit";
  dirFitName = fileName + ":" + dirFitName;
  TDirectory *dirFit = fileIn->Get(dirFitName);
  
  const int nHisto = 30;
  
  struct THistoData
  {
    char *name;  // one, which used in input file
    char *title; // for X-axis
  };
  const THistoData histoData[nHisto] =
  {
    {"fst_pQP",  "Pull q/p"   }
    {"fst_ptx",  "Pull t_{x}" }
    {"fst_pty",  "Pull t_{y}" }
    {"fst_px",  "Pull x"   }
    {"fst_py",  "Pull y" }
    {"fst_P",  "Resolution (p^{reco}-p^{mc})/p^{mc}"   }
    {"fst_tx",  "Residual (t_{x}^{reco}-t_{x}^{mc}) [mrad]" }
    {"fst_ty",  "Residual (t_{y}^{reco}-t_{y}^{mc}) [mrad]" }
    {"fst_x",  "Residual (x^{reco}-x^{mc}) [#mum]" }
    {"fst_y",  "Residual (y^{reco}-y^{mc}) [#mum]" }
    {"svrt_pQP",  "Pull q/p"   }
    {"svrt_ptx",  "Pull t_{x}" }
    {"svrt_pty",  "Pull t_{y}" }
    {"svrt_px",  "Pull x"   }
    {"svrt_py",  "Pull y" }
    {"svrt_P",  "Resolution (p^{reco}-p^{mc})/p^{mc}"   }
    {"svrt_tx",  "Residual (t_{x}^{reco}-t_{x}^{mc}) [mrad]" }
    {"svrt_ty",  "Residual (t_{y}^{reco}-t_{y}^{mc}) [mrad]" }
    {"svrt_x",  "Residual (x^{reco}-x^{mc}) [#mum]" }
    {"svrt_y",  "Residual (y^{reco}-y^{mc}) [#mum]" }
    {"pvrt_pQP",  "Pull q/p"   }
    {"pvrt_ptx",  "Pull t_{x}" }
    {"pvrt_pty",  "Pull t_{y}" }
    {"pvrt_px",  "Pull x"   }
    {"pvrt_py",  "Pull y" }
    {"pvrt_P",  "Resolution (p^{reco}-p^{mc})/p^{mc}"   }
    {"pvrt_tx",  "Residual (t_{x}^{reco}-t_{x}^{mc}) [mrad]" }
    {"pvrt_ty",  "Residual (t_{y}^{reco}-t_{y}^{mc}) [mrad]" }
    {"pvrt_x",  "Residual (x^{reco}-x^{mc}) [#mum]" }
    {"pvrt_y",  "Residual (y^{reco}-y^{mc}) [#mum]" }
  };
  
  TH1F *histo[nHisto];

  float charat[nHisto][2]; // 0 - sigma, 1 - RMS
  for (int i = 0; i < nHisto; i++){
    histo[i] = (TH1F*) dirFit->Get(histoData[i].name);
    makeUpHisto(histo[i],histoData[i].title, &(charat[i][0]));
    charat[i][1] = histo[i]->GetRMS();
  }

  cout.setf(std::ios::scientific,std::ios::floatfield);
  cout.precision(2);
  cout << endl << " Name  " << "   \t" << " Sigma   " << "\t" << " RMS " << endl;
  for (int i = 0; i < nHisto; i++){
    cout << histoData[i].name << "   \t" << charat[i][0] << "\t" << charat[i][1] << endl;
  }
      // ------------ hits, strips -----------
  
  TString dirInputName = "/L1/Input";
  dirInputName = fileName + ":" + dirInputName;
  TDirectory *dirInput = fileIn->Get(dirInputName);
  
  const int nHistoInput = 4;
  
  const THistoData histoDataInput[nHisto] =
  {
    {"Px",  "Pull x" }
    {"Py",  "Pull y" }
    {"x",  "Residual (x^{hit}-x^{mc}) [#mum]" }
    {"y",  "Residual (y^{hit}-y^{mc}) [#mum]" }
  };
  
  TH1F *histoInput[nHistoInput];

  for (int i = 0; i < nHistoInput; i++){
    histoInput[i] = (TH1F*) dirInput->Get(histoDataInput[i].name);
    makeUpHisto(histoInput[i],histoDataInput[i].title);
  }
  
    // ------------ make profiles -----------

  TString dirName = "/L1";
  dirName = fileName + ":" + dirName;
  TDirectory *dir = fileIn->Get(dirName);
  
  const int nProf = 1;
  struct TProfData
  {
    char *name;  // one, which used in input file
    char *titleX; // for X-axis
    char *titleY; // for Y-axis
  };
  const TProfData profData[nProf] =
  {
    {"p_eff_all_vs_mom",  "Momentum [GeV/c]",  "Efficiency [%]"  }
  };
  
  TProfile *profile[nProf];

  for (int i = 0; i < nProf; i++){
    profile[i] = (TProfile*) dir->Get(profData[i].name);
    makeUpProfile(profile[i], profData[i].titleX, profData[i].titleY);
  }
    // ============================ Draw  ============================
//   TCanvas *c1;
//   c1 = new TCanvas("c1","c1",0,0,1200,800);
//   c1 -> Divide(nHisto);
// 
//   c1->UseCurrentStyle();
// 
//   for (int i = 0; i < nHisto; i++){
//     c1->cd(i+1);
//     histo[i]->Draw();
//   }

//   c1->SaveAs("histo.png");

  system("mkdir L1_histo -p");
  chdir( "L1_histo" );
  const int nParts = 2;
  int iPart = 1;
  histoStyle->cd();
  TCanvas *c2;
  if (!divide) c2 = new TCanvas("c2","c2",0,0,900,600);
          else c2 = new TCanvas("c2","c2",0,0,900*nParts,600*nParts);
  if (divide) c2->Divide(nParts, nParts);
  
  histoStyle->cd();
  c2->cd();
  for (int i = 0; i < nHisto; i++){
    if (divide) c2->cd(iPart++);
    histo[i]->Draw();
    TString name = TString(histoData[i].name)+".pdf";
    if (!divide) c2->SaveAs(name);
  }

  for (int i = 0; i < nHistoInput; i++){
    if (divide) c2->cd(iPart++);
    histoInput[i]->Draw();
    TString name = TString(histoDataInput[i].name)+".pdf";
    if (!divide) c2->SaveAs(name);
  }
  
  profStyle->cd();
  for (int i = 0; i < nProf; i++){
    if (divide) c2->cd(iPart++);
    profile[i]->Draw();
    TString name = TString(profData[i].name)+".pdf";
    if (!divide) c2->SaveAs(name);
  }

  if (divide) c2->SaveAs("L1_histo.pdf");
  chdir( ".." );
}
