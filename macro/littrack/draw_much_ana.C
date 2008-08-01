
draw_much_ana()
{

  TString system  = "auau";  
  TString beam    = "25gev";  
  TString trigger = "centr";
  TString particle = "mu";
  
  //TString dir = "/d/cbm02/andrey/events/much/10stations/new/wofield/signal/";
  TString dir = "/d/cbm02/andrey/events/much/10stations/10mu/mu/";
  
  TString outDir = "./lit_thick/";
  
  gStyle->SetOptFit(0001);
  
  TFile *file1 = new TFile(dir + beam + "/" 
                    + particle + "/much.ana." + system + "." + beam + "." 
                    + particle + "." + trigger + ".root"); 
   

                    
   TCanvas *c_srx = new TCanvas("fh_srx","c_srx",1200,1000);
   TCanvas *c_sry = new TCanvas("fh_sry","c_sry",1200,1000);
   
   TCanvas *c_resx = new TCanvas("fh_resx","c_resx",1200,1000);
   TCanvas *c_resy = new TCanvas("fh_resy","c_resy",1200,1000);
   TCanvas *c_restx = new TCanvas("fh_restx","c_restx",1200,1000);
   TCanvas *c_resty = new TCanvas("fh_resty","c_resty",1200,1000);
   TCanvas *c_resqp = new TCanvas("fh_resqp","c_resqp",1200,1000);
   
   TCanvas *c_pullx = new TCanvas("fh_pullx","c_pullx",1200,1000);
   TCanvas *c_pully = new TCanvas("fh_pully","c_pully",1200,1000);
   TCanvas *c_pulltx = new TCanvas("fh_pulltx","c_pulltx",1200,1000);
   TCanvas *c_pullty = new TCanvas("fh_pullty","c_pullty",1200,1000);
   TCanvas *c_pullqp = new TCanvas("fh_pullqp","c_pullqp",1200,1000);
   
   TCanvas *c_resp = new TCanvas("fh_resp","c_resp",1200,1000);
   
   TCanvas *c_last = new TCanvas("fh_last","c_last",1200,1000);
   TCanvas *c_first = new TCanvas("fh_first","c_first",1200,1000);
   
   
   const int fNofLayers = 10;
   const int nRows = 3;
   const int nCols = 5;
   
   c_srx->Divide(nCols, nRows);
   c_sry->Divide(nCols, nRows);
   c_resx->Divide(nCols, nRows);
   c_resy->Divide(nCols, nRows);
   c_restx->Divide(nCols, nRows);
   c_resty->Divide(nCols, nRows);
   c_resqp->Divide(nCols, nRows);
   c_pullx->Divide(nCols, nRows);
   c_pully->Divide(nCols, nRows);
   c_pulltx->Divide(nCols, nRows);
   c_pullty->Divide(nCols, nRows);
   c_pullqp->Divide(nCols, nRows);
   c_resp->Divide(nCols, nRows);
   
   c_last->Divide(5, 3);
   c_first->Divide(5, 3);
   
   
   double sigma_srx[fNofLayers];
   double sigma_sry[fNofLayers]; 
   
   double rms_srx[fNofLayers];
   double rms_sry[fNofLayers]; 
   
   
   double sigma_resx[fNofLayers];
   double sigma_resy[fNofLayers];
   double sigma_restx[fNofLayers];
   double sigma_resty[fNofLayers];
   double sigma_resqp[fNofLayers];
   
   double rms_resx[fNofLayers];
   double rms_resy[fNofLayers];
   double rms_restx[fNofLayers];
   double rms_resty[fNofLayers];
   double rms_resqp[fNofLayers];
   
   double sigma_pullx[fNofLayers];
   double sigma_pully[fNofLayers];
   double sigma_pulltx[fNofLayers];
   double sigma_pullty[fNofLayers];
   double sigma_pullqp[fNofLayers];
   
   double rms_pullx[fNofLayers];
   double rms_pully[fNofLayers];
   double rms_pulltx[fNofLayers];
   double rms_pullty[fNofLayers];
   double rms_pullqp[fNofLayers];
      
   double sigma_resp[fNofLayers];
   double rms_resp[fNofLayers];
   
   double sigma_last[11];
   double sigma_first[11];
   double rms_last[11];
   double rms_first[11];
     
      
   for (int i = 0; i < fNofLayers; i++) {
      
   
      hist(file1, c_srx, i, "fh_srx%i", sigma_srx[i], rms_srx[i]);
      hist(file1, c_sry, i, "fh_sry%i", sigma_sry[i], rms_sry[i]);
      
      hist(file1, c_resx, i, "fh_resx%i", sigma_resx[i], rms_resx[i]);
      hist(file1, c_resy, i, "fh_resy%i", sigma_resy[i], rms_resy[i]);
      hist(file1, c_restx, i, "fh_restx%i", sigma_restx[i], rms_restx[i]);
      hist(file1, c_resty, i, "fh_resty%i", sigma_resty[i], rms_resty[i]);
      hist(file1, c_resqp, i, "fh_resqp%i", sigma_resqp[i], rms_resqp[i]);
      
      hist(file1, c_pullx, i, "fh_pullx%i", sigma_pullx[i], rms_pullx[i]);
      hist(file1, c_pully, i, "fh_pully%i", sigma_pully[i], rms_pully[i]);
      hist(file1, c_pulltx, i, "fh_pulltx%i", sigma_pulltx[i], rms_pulltx[i]);
      hist(file1, c_pullty, i, "fh_pullty%i", sigma_pullty[i], rms_pullty[i]);
      hist(file1, c_pullqp, i, "fh_pullqp%i", sigma_pullqp[i], rms_pullqp[i]);
      
      hist(file1, c_resp, i, "fh_resp%i", sigma_resp[i], rms_resp[i]);
        
   }

   
   
   hist2(file1, c_last, 0, "fh_resx_last", sigma_last[0], rms_last[0]);
   hist2(file1, c_last, 1, "fh_resy_last", sigma_last[1], rms_last[1]);
   hist2(file1, c_last, 2, "fh_restx_last", sigma_last[2], rms_last[2]);
   hist2(file1, c_last, 3, "fh_resty_last", sigma_last[3], rms_last[3]);
   hist2(file1, c_last, 4, "fh_resqp_last", sigma_last[4], rms_last[4]);
   hist2(file1, c_last, 5, "fh_pullx_last", sigma_last[5], rms_last[5]);
   hist2(file1, c_last, 6, "fh_pully_last", sigma_last[6], rms_last[6]);
   hist2(file1, c_last, 7, "fh_pulltx_last", sigma_last[7], rms_last[7]);
   hist2(file1, c_last, 8, "fh_pullty_last", sigma_last[8], rms_last[8]);
   hist2(file1, c_last, 9, "fh_pullqp_last", sigma_last[9], rms_last[9]);
   hist2(file1, c_last, 10, "fh_resp_last", sigma_last[10], rms_last[10]);  
   
   hist2(file1, c_first, 0, "fh_resx_first", sigma_first[0], rms_first[0]);
   hist2(file1, c_first, 1, "fh_resy_first", sigma_first[1], rms_first[1]);
   hist2(file1, c_first, 2, "fh_restx_first", sigma_first[2], rms_first[2]);
   hist2(file1, c_first, 3, "fh_resty_first", sigma_first[3], rms_first[3]);
   hist2(file1, c_first, 4, "fh_resqp_first", sigma_first[4], rms_first[4]);
   hist2(file1, c_first, 5, "fh_pullx_first", sigma_first[5], rms_first[5]);
   hist2(file1, c_first, 6, "fh_pully_first", sigma_first[6], rms_first[6]);
   hist2(file1, c_first, 7, "fh_pulltx_first", sigma_first[7], rms_first[7]);
   hist2(file1, c_first, 8, "fh_pullty_first", sigma_first[8], rms_first[8]);
   hist2(file1, c_first, 9, "fh_pullqp_first", sigma_first[9], rms_first[9]);
   hist2(file1, c_first, 10, "fh_resp_first", sigma_first[10], rms_first[10]); 
   
   // Save canvases as images
   c_srx->cd();
   c_srx->SaveAs(TString(outDir + "c_srx.gif"));
   c_sry->cd();
   c_sry->SaveAs(TString(outDir + "c_sry.gif"));
   c_resx->cd();
   c_resx->SaveAs(TString(outDir + "c_resx.gif"));
   c_resy->cd();
   c_resy->SaveAs(TString(outDir + "c_resy.gif"));  
   c_restx->cd();
   c_restx->SaveAs(TString(outDir + "c_restx.gif"));  
   c_resty->cd();
   c_resty->SaveAs(TString(outDir + "c_resty.gif"));  
   c_resqp->cd();
   c_resqp->SaveAs(TString(outDir + "c_resqp.gif"));  
   c_pullx->cd();
   c_pullx->SaveAs(TString(outDir + "c_pullx.gif"));  
   c_pully->cd();
   c_pully->SaveAs(TString(outDir + "c_pully.gif"));  
   c_pulltx->cd();
   c_pulltx->SaveAs(TString(outDir + "c_pulltx.gif"));  
   c_pullty->cd();
   c_pullty->SaveAs(TString(outDir + "c_pullty.gif"));  
   c_pullqp->cd();
   c_pullqp->SaveAs(TString(outDir + "c_pullqp.gif"));  
   c_resp->cd();
   c_resp->SaveAs(TString(outDir + "c_resp.gif"));  
   c_last->cd();
   c_last->SaveAs(TString(outDir + "c_last.gif"));  
   c_first->cd();
   c_first->SaveAs(TString(outDir + "c_first.gif"));  
   //
   
   ofstream fout(outDir + "out.txt");
   
   fout << "------------search region sigma (rms)---------------" << endl;
   for (int i = 0; i < fNofLayers; i++) {
      fout << "srx[" << i << "]=" << sigma_srx[i] << "(" << rms_srx[i] << ");   "
           << "sry[" << i << "]=" << sigma_sry[i] << "(" << rms_sry[i] << ");" << endl;
   }
   
   fout << "------------resolution sigma (rms)---------------" << endl;
   for (int i = 0; i < fNofLayers; i++) {
      fout << "resx[" << i << "]=" << sigma_resx[i] << "(" << rms_resx[i] << "); "
           << "resy[" << i << "]=" << sigma_resy[i] << "(" << rms_resy[i] << "); " 
           << "restx[" << i << "]=" << sigma_restx[i] << "(" << rms_restx[i] << "); "
           << "resty[" << i << "]=" << sigma_resty[i] << "(" << rms_resty[i] << "); " 
           << "resqp[" << i << "]=" << sigma_resqp[i] << "(" << rms_resqp[i] << "); " << endl;
   }
   
   fout << "------------pulls sigma (rms)---------------" << endl;
   for (int i = 0; i < fNofLayers; i++) {
      fout << "pullx[" << i << "]=" << sigma_pullx[i] << "(" << rms_pullx[i] << "); "
           << "pully[" << i << "]=" << sigma_pully[i] << "(" << rms_pully[i] << "); " 
           << "pulltx[" << i << "]=" << sigma_pulltx[i] << "(" << rms_pulltx[i] << "); "
           << "pullty[" << i << "]=" << sigma_pullty[i] << "(" << rms_pullty[i] << "); " 
           << "pullqp[" << i << "]=" << sigma_pullqp[i] << "(" << rms_pullqp[i] << "); " << endl; 
   }  
   
   fout << "------------momentum residual sigma (rms)---------------" << endl;
   for (int i = 0; i < fNofLayers; i++) {
      fout << "resp[" << i << "]=" << sigma_resp[i] << "(" << rms_resp[i] << ")" << endl; 
   }  
   
   fout << "------------residuals and pull on the last and first station---------" << endl;
   fout << "resx_last=" << sigma_last[0] << ", resx_first=" << sigma_first[0] << endl; 
   fout << "resy_last=" << sigma_last[1] << ", resy_first=" << sigma_first[1] << endl; 
   fout << "restx_last=" << sigma_last[2] << ", restx_first=" << sigma_first[2] << endl; 
   fout << "resty_last=" << sigma_last[3] << ", resty_first=" << sigma_first[3]  << endl; 
   fout << "resqp_last=" << sigma_last[4] << ", resqp_first=" << sigma_first[4]  << endl; 
   fout << "pullx_last=" << sigma_last[5] << ", pullx_first=" << sigma_first[5]  << endl; 
   fout << "pully_last=" << sigma_last[6] << ", pully_first=" << sigma_first[6]  << endl; 
   fout << "pulltx_last=" << sigma_last[7] << ", pulltx_first=" << sigma_first[7]  << endl; 
   fout << "pullty_last=" << sigma_last[8] << ", pullty_first=" << sigma_first[8]  << endl; 
   fout << "pullqp_last=" << sigma_last[9] << ", pullqp_first=" << sigma_first[9]  << endl;
   fout << "resp_last=" << sigma_last[10] << ", resp_first=" << sigma_first[10]  << endl;  
   
   fout.close();
}

void hist(TFile *file1, TCanvas *c1, int i, const char* name, double &sigma, double &rms) {
   
   c1->cd(i+1);
   char histName[100];
   sprintf(histName, name, i);
   TH1F* hist1 = (TH1F*) file1->Get(histName);
   hist1->Fit("gaus");
   hist1->Draw();
   TF1 *fit1 = hist1->GetFunction("gaus");
   sigma = fit1->GetParameter(2);
   rms = hist1->GetRMS();   
}

void hist2(TFile *file1, TCanvas *c1, int i, const char* name, double &sigma, double &rms) {
   
   c1->cd(i+1);
   TH1F* hist1 = (TH1F*) file1->Get(name);
   hist1->Fit("gaus");
   hist1->Draw();
   TF1 *fit1 = hist1->GetFunction("gaus");
   sigma = fit1->GetParameter(2);   
   rms = hist1->GetRMS();  
}
