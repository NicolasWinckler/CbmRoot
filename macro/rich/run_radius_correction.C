/*  Description: This macro calculate MAP for radius correction
    To use this macro, first run CbmRichRingQa task.
    It sets recFlag to the rings.
    
    Author : Simeon Lebedev
    E-mail : S.Lebedev@gsi.de
*/
#include <vector>

void run_radius_correction ()
{
    TStopwatch timer;
    timer.Start();
    
    gStyle->SetPalette(1,0);
    gStyle->SetHistLineWidth(2);
    
    // ----  Load libraries   -------------------------------------------------
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();
    gSystem->Load("libGeoBase");
    gSystem->Load("libParBase");
    gSystem->Load("libBase");
    gSystem->Load("libCbmBase");
    gSystem->Load("libField");
    gSystem->Load("libGen");
    gSystem->Load("libPassive");
    gSystem->Load("libMvd");
    gSystem->Load("libSts");
    gSystem->Load("libRich");
    gSystem->Load("libTrd");
    gSystem->Load("libTof");
    gSystem->Load("libEcal");
    gSystem->Load("libGlobal");
    gSystem->Load("libKF");
    gSystem->Load("libL1");
    // ------------------------------------------------------------------------

    char fileMC[200], fileRec[200];
    sprintf(fileRec, "/d/cbm02/slebedev/rich/MAY08/density_study/radius_correction.auau.25gev.centr.0012.recorich.root");
    TFile *f = new TFile(fileRec,"R");
    TTree* t = f->Get("cbmsim");
    TFolder *fd = f->Get("cbmout");
    TClonesArray *fRichRings = (TClonesArray*) fd->FindObjectAny("RichRing");
    t->SetBranchAddress(fRichRings->GetName(),&fRichRings);
    
    Int_t fNofBinsX = 20;
    Int_t fNofBinsY = 25;
    ///A axis
    TH2D* fh_axisAXYCount;
    TH2D* fh_axisAXYW;
    TH2D* fh_axisAXY;
    TH2D* fh_axisASigma;
    TH2D* mapaxisAXY;
    
 
    ///B axis
    TH2D* fh_axisBXYCount;
    TH2D* fh_axisBXYW;
    TH2D* fh_axisBXY;
    TH2D* fh_axisBSigma;    
    TH2D* mapaxisBXY;
   
	mapaxisAXY = new TH2D("fh_mapaxisAXY","dA distribution (x,y);X, [cm];Y, [cm]",fNofBinsX,-200,200,fNofBinsY,-250,250);
	mapaxisBXY = new TH2D("fh_mapaxisBXY","dB distribution (x,y);X, [cm];Y, [cm]",fNofBinsX,-200,200,fNofBinsY,-250,250);
    fh_axisAXYCount = new TH2D("fh_axisAXYCount","A Count",fNofBinsX,-200,200,fNofBinsY,-250,250);
    fh_axisAXYW = new TH2D("fh_axisAXYW","",fNofBinsX,-200,200,fNofBinsY,-250,250);
    fh_axisBXYCount = new TH2D("fh_axisBXYCount","B Count",fNofBinsX,-200,200,fNofBinsY,-250,250);
    fh_axisBXYW = new TH2D("fh_axisBXYW","",fNofBinsX,-200,200,fNofBinsY,-250,250);
	fh_axisAXY = new TH2D("fh_axisAXY","A distribution (x,y);X, [cm];Y, [cm]",fNofBinsX,-200,200,fNofBinsY,-250,250);
	fh_axisBXY = new TH2D("fh_axisBXY","B distribution (x,y);X, [cm];Y, [cm]",fNofBinsX,-200,200,fNofBinsY,-250,250);
	fh_axisASigma = new TH2D("fh_axisASigma","RMS A distribution (x,y);X, [cm];Y, [cm]",fNofBinsX,-200,200,fNofBinsY,-250,250);
	fh_axisBSigma = new TH2D("fh_axisBSigma","RMS B distribution (x,y);X, [cm];Y, [cm]",fNofBinsX,-200,200,fNofBinsY,-250,250);

	
    ///Set Mean value of A and B axeses
    Double_t fMeanAaxis = 6.198;
    Double_t fMeanBaxis = 5.612;
    
    Int_t nEvents=t->GetEntries();
    cout<<" nEvents ="<<nEvents<<endl;
    for(Int_t ievent=0;ievent<nEvents; ievent++ ) {
        cout<<"ievent = "<<ievent;
        CbmRichRing *ring=NULL;
        t->GetEntry(ievent);
        Int_t nofRings = fRichRings->GetEntries();
        cout<<"  nofRings = "<<nofRings<<endl;
        for(Int_t iRing=0; iRing < nofRings; iRing++){
            ring = (CbmRichRing*)fRichRings->At(iRing);
            if (!ring) continue;
            ///check for primary electrons
            if (ring->GetRecFlag() != 3) continue;
            
            Double_t radius = ring->GetRadius();
            Double_t axisA = ring->GetAaxis();
            Double_t axisB = ring->GetBaxis();
            Double_t centerX = ring->GetCenterX();
            Double_t centerY = ring->GetCenterY();
            
            if (axisA > 8. || axisB > 8.) continue;
            if (axisA < 4. || axisB < 4.) continue;
            
            fh_axisAXYW->Fill(centerX, centerY, axisA);
            fh_axisAXYCount->Fill(centerX, centerY);
            
            fh_axisBXYW->Fill(centerX, centerY, axisB);
            fh_axisBXYCount->Fill(centerX, centerY);           
        } //iRing
    } //iEvent    

    fh_axisAXY->Divide(fh_axisAXYW,fh_axisAXYCount);
    fh_axisBXY->Divide(fh_axisBXYW,fh_axisBXYCount);  
    
    
///calculate RMS for each bin    
    for(Int_t ievent=0;ievent<nEvents; ievent++ ) {
        cout<<"ievent = "<<ievent;
        CbmRichRing *ring=NULL;
        t->GetEntry(ievent);
        Int_t nofRings = fRichRings->GetEntries();
        cout<<"  nofRings = "<<nofRings<<endl;
        for(Int_t iRing=0; iRing < nofRings; iRing++){
            ring = (CbmRichRing*)fRichRings->At(iRing);
            if (!ring) continue;
            if (ring->GetRecFlag() != 3) continue;
            
            Double_t axisA = ring->GetAaxis();
            Double_t axisB = ring->GetBaxis();
            Double_t centerX = ring->GetCenterX();
            Double_t centerY = ring->GetCenterY();            
            if (axisA > 8. || axisB > 8.) continue;
            if (axisA < 4. || axisB < 4.) continue;
            
            Double_t meanA = fh_axisAXY->GetBinContent(fh_axisAXY->FindBin(centerX,centerY));
            Double_t meanB = fh_axisBXY->GetBinContent(fh_axisBXY->FindBin(centerX,centerY));
            Double_t nA = fh_axisAXYCount->GetBinContent(fh_axisAXYCount->FindBin(centerX,centerY));
            Double_t nB = fh_axisBXYCount->GetBinContent(fh_axisBXYCount->FindBin(centerX,centerY));
            Double_t devA = (meanA - axisA) * (meanA - axisA)/nA;
            Double_t devB = (meanB - axisB) * (meanB - axisB)/nB;
            
            fh_axisASigma->Fill(centerX, centerY, devA);
            fh_axisBSigma->Fill(centerX, centerY, devB);
                    
        } //iRing
    } //iEvent        
    
    for (Int_t iX = 1; iX < fh_axisASigma->GetNbinsX() + 1; iX++){
        for (Int_t iY = 1; iY < fh_axisASigma->GetNbinsY() + 1; iY++){
        	fh_axisASigma->SetBinContent(iX, iY, sqrt(fh_axisASigma->GetBinContent(iX, iY)) );
        	fh_axisBSigma->SetBinContent(iX, iY, sqrt(fh_axisBSigma->GetBinContent(iX, iY)) );                      
        }
    }
    
///create two correction maps
    for (Int_t iX = 1; iX < mapaxisAXY->GetNbinsX() + 1; iX++){
        for (Int_t iY = 1; iY < mapaxisAXY->GetNbinsY() + 1; iY++){
            mapaxisAXY->SetBinContent(iX, iY, fMeanAaxis - fh_axisAXY->GetBinContent(iX, iY) );
            mapaxisBXY->SetBinContent(iX, iY, fMeanBaxis - fh_axisBXY->GetBinContent(iX, iY) );                      
        }
    }

    c1_0 = new TCanvas("c1_0","c1_0",10,10,800,800);
    c1_0->Divide(1,2);
    c1_0->cd(1); 
    fh_axisAXYCount->Draw("COLZ");
    c1_0->cd(2);
    fh_axisBXYCount->Draw("COLZ"); 
    
    c1 = new TCanvas("c1","c1",10,10,800,800);
    c1->Divide(1,2);
    c1->cd(1);
    fh_axisAXY->SetMinimum(5.0);
    fh_axisAXY->SetMaximum(7.0);  
    fh_axisAXY->Draw("COLZ");
    c1->cd(2);
    fh_axisBXY->SetMinimum(5.0);
    fh_axisBXY->SetMaximum(6.5); 
    fh_axisBXY->Draw("COLZ"); 
    
    c2 = new TCanvas("c2","c2",10,10,800,800);
    c2->Divide(1,2);
    c2->cd(1);
    mapaxisAXY->SetMinimum(-0.5);
    mapaxisAXY->SetMaximum(0.5);
    mapaxisAXY->Draw("COLZ");
    c2->cd(2);  
    mapaxisBXY->SetMinimum(-0.5);
    mapaxisBXY->SetMaximum(0.5); 
    mapaxisBXY->Draw("COLZ");
    
    c2_1 = new TCanvas("c2_1","c2_1",10,10,800,800);
    c2_1->Divide(1,2);
    c2_1->cd(1);
    fh_axisBSigma->Draw("COLZ");
    c2_1->cd(2);  
    fh_axisBSigma->Draw("COLZ");

///// Check correction procedure
    TH1D* fh_Abefore = new TH1D("fh_Abefore","A before;radius, [cm]", 90, 0., 9.);;
    TH1D* fh_Bbefore= new TH1D("fh_Bbefore","B before;radius, [cm]", 90, 0., 9.);;
    
    TH1D* fh_A = new TH1D("fh_A","A after;radius, [cm]", 90, 0., 9.);;
    TH1D* fh_B = new TH1D("fh_B","B after;radius, [cm]", 90, 0., 9.);;
    
    cout <<"Check correction procedure......" << endl;
    for(Int_t ievent=0;ievent<nEvents;ievent++ ) {
        CbmRichRing *ring=NULL;
       // if (ievent % 100 == 0) cout << ievent << "   ";
        //t1->GetEntry(ievent);
        t->GetEntry(ievent);

        Int_t nofRings = fRichRings->GetEntries();

        for(Int_t iRing=0; iRing < nofRings; iRing++){

            ring = (CbmRichRing*)fRichRings->At(iRing);
            if (!ring) continue;
            
            ///check for primary electrons
            if (ring->GetRecFlag() != 3) continue;
            Double_t axisA = ring->GetAaxis();
            Double_t axisB = ring->GetBaxis();
            if (axisA > 8. || axisB > 8.) continue;
            if (axisA < 4. || axisB < 4.) continue;
            
            Double_t radius = ring->GetRadius();
            Double_t centerX = ring->GetCenterX();
            Double_t centerY = ring->GetCenterY();
            Double_t axisAbefore = ring->GetAaxis();
            Double_t axisBbefore = ring->GetBaxis();             
            fh_Abefore->Fill(axisAbefore);
            fh_Bbefore->Fill(axisBbefore);
            
            Double_t axisA = ring->GetAaxis();
            Double_t axisB = ring->GetBaxis() ;

            axisA += mapaxisAXY->GetBinContent(mapaxisAXY->FindBin(centerX,centerY));
            axisB += mapaxisBXY->GetBinContent(mapaxisBXY->FindBin(centerX,centerY));               

            fh_A->Fill(axisA);
            fh_B->Fill(axisB); 
        } //iRing
    }//iEvent
    gStyle->SetOptStat(0);
    c3 = new TCanvas("c3","c3",10,10,800,800);
    c3->Divide(2,2);
    c3->cd(1);
    fh_Abefore->SetMaximum(fh_Abefore->GetMaximum()*1.3);
    fh_Abefore->Draw();
    fh_Abefore->SetAxisRange(4.0, 8.0);
    fh_Abefore->Fit("gaus");
    Double_t sigmaAb = fh_Abefore->GetFunction("gaus")->GetParameter("Sigma");
    char sigmaTxtAb[30];
    sprintf(sigmaTxtAb,"sigma = %.3f",sigmaAb);
    TText* txtAb = new TText(4.3, fh_Abefore->GetMaximum()*0.85, sigmaTxtAb);
    txtAb->SetTextSize(0.1);
    txtAb->Draw();
    
    c3->cd(2);
    fh_Bbefore->SetMaximum(fh_Bbefore->GetMaximum()*1.3);
    fh_Bbefore->Draw();
    fh_Bbefore->SetAxisRange(4.0, 7.0);
    fh_Bbefore->Fit("gaus");
    Double_t sigmaBb = fh_Bbefore->GetFunction("gaus")->GetParameter("Sigma");
    char sigmaTxtBb[30];
    sprintf(sigmaTxtBb,"sigma = %.3f",sigmaBb);
    TText* txtBb = new TText(4.3, fh_Bbefore->GetMaximum()*0.85, sigmaTxtBb);
    txtBb->SetTextSize(0.1);
    txtBb->Draw();
    
    c3->cd(3);
    fh_A->SetMaximum(fh_A->GetMaximum()*1.3);
    fh_A->SetAxisRange(4.0, 8.0);
    fh_A->Draw(); 
    fh_A->Fit("gaus");
    Double_t sigmaA = fh_A->GetFunction("gaus")->GetParameter("Sigma");
    char sigmaTxtA[30];
    sprintf(sigmaTxtA,"sigma = %.3f",sigmaA);
    TText* txtA = new TText(4.3, fh_A->GetMaximum()*0.85, sigmaTxtA);
    txtA->SetTextSize(0.1);
    txtA->Draw();
    
    c3->cd(4);
    fh_B->SetMaximum(fh_B->GetMaximum()*1.3);
    fh_B->SetAxisRange(4.0, 7.0);
    fh_B->Draw();  
    fh_B->Fit("gaus");
    Double_t sigmaB = fh_B->GetFunction("gaus")->GetParameter("Sigma");
    char sigmaTxtB[30];
    sprintf(sigmaTxtB,"sigma = %.3f",sigmaB);
    TText* txtB = new TText(4.3, fh_B->GetMaximum()*0.85, sigmaTxtB);
    txtB->SetTextSize(0.1);
    txtB->Draw();

/// Write correction map to the file
    TFile *file = new TFile("radius_correction_map.root", "recreate");
    mapaxisAXY->Write();
    mapaxisBXY->Write();
    file->Close();
       
}
