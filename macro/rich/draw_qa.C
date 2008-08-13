/*  Description: This macro draw the histograms which
    were create with CbmRichRingQa class.

    Author : Simeon Lebedev
    E-mail : S.Lebedev@gsi.de
*/

void draw_qa(){
   //TFile *file = new TFile("/d/cbm02/slebedev/rich/MAY08/standard/auau.25gev.centr.0000.recorich.root");
   // TFile *file = new TFile("/d/cbm02/slebedev/rich/MAY08/density_study/new.auau.25gev.centr.0012.recorich.root");
    TFile *file = new TFile("/d/cbm02/slebedev/rich/MAY08/density_study/auau.25gev.centr.0012.recoqa_15.root");
    TDirectory *current = gDirectory;
	current->cd("RichRingQaHist");
   // rich->cd();
   gROOT->SetStyle("Plain");
    gStyle->SetPalette(1,0);   
   // gStyle->SetOptStat(0000);


		
	gStyle->SetHistLineWidth(3);
	gROOT->ForceStyle();
	//gStyle->SetOptStat(0);
	//gStyle->SetOptFit(1);
	
    TCanvas *c1 = new TCanvas("CbmRichQa1","c1",1200,1000);
    c1->Divide(4,4);
    c1->cd(1);
    fh_FakeNofHits->SetLineStyle(2);
    fh_TrueElNofHits->SetLineWidth(2);
    fh_TrueElNofHits->SetLineStyle(9);
    fh_TrueElNofHits->GetXaxis()->SetTitle("Number of Hits");
    fh_TrueElNofHits->GetYaxis()->SetTitle("Entries");  
    fh_TrueElNofHits->Draw();
    fh_FakeNofHits->Draw("SAME");
    
    c1->cd(2);
    fh_FakeDistance->SetLineStyle(2);
    fh_TrueElDistance->SetLineWidth(2);
    fh_TrueElDistance->SetLineStyle(9);
    fh_TrueElDistance->GetXaxis()->SetTitle("ring-track distance, [cm]");
    fh_TrueElDistance->GetYaxis()->SetTitle("Entries"); 
    fh_TrueElDistance->Draw();  
    fh_FakeDistance->Draw("SAME");
    
    c1->cd(3);  
    fh_FakeAngle->SetLineStyle(2);
    fh_TrueElAngle->SetLineWidth(2);
    fh_TrueElAngle->SetLineStyle(9);
    fh_TrueElAngle->GetXaxis()->SetTitle("Angle, [rad]");
    fh_TrueElAngle->GetYaxis()->SetTitle("Entries");            
    fh_TrueElAngle->Draw(); 
    fh_FakeAngle->Draw("SAME");
    
    c1->cd(4);
    fh_FakeNofHitsOnRing->SetLineStyle(2);
    fh_TrueElNofHitsOnRing->SetLineWidth(2);
    fh_TrueElNofHitsOnRing->SetLineStyle(9);
    fh_TrueElNofHitsOnRing->GetXaxis()->SetTitle("# hits on ring");
    fh_TrueElNofHitsOnRing->GetYaxis()->SetTitle("Entries");            
    fh_TrueElNofHitsOnRing->Draw();
    fh_FakeNofHitsOnRing->Draw("SAME");
    
    c1->cd(5);  
    fh_FakeRadPos->SetLineStyle(2);
    fh_TrueElRadPos->SetLineWidth(2);
    fh_TrueElRadPos->SetLineStyle(9);   
    fh_TrueElRadPos->GetXaxis()->SetTitle("Radial posotion, [cm]");
    fh_TrueElRadPos->GetYaxis()->SetTitle("Entries");   
    fh_TrueElRadPos->Draw();    
    fh_FakeRadPos->Draw("SAME");
    
    c1->cd(6);  
    fh_FakeChi2->SetLineStyle(2);
    fh_TrueElChi2->SetLineWidth(2);
    fh_TrueElChi2->SetLineStyle(9);     
    fh_TrueElChi2->GetXaxis()->SetTitle("chi2");
    fh_TrueElChi2->GetYaxis()->SetTitle("Entries"); 
    fh_TrueElChi2->Draw();  
    fh_FakeChi2->Draw("SAME");

    c1->cd(7);    
    fh_FakeRadius->SetLineStyle(2);
    fh_TrueElRadius->SetLineWidth(2);
    fh_TrueElRadius->SetLineStyle(9);    
    fh_TrueElRadius->GetXaxis()->SetTitle("radius, [cm]");
    fh_TrueElRadius->GetYaxis()->SetTitle("Entries");        
    fh_TrueElRadius->Draw();  
    fh_FakeRadius->Draw("SAME");
    
    c1->cd(8);  
    fh_FakeA->SetLineStyle(2);
    fh_TrueElA->SetLineWidth(2);
    fh_TrueElA->SetLineStyle(9);   
    fh_TrueElA->GetXaxis()->SetTitle("A axis, [cm]");
    fh_TrueElA->GetYaxis()->SetTitle("Entries");   
    fh_TrueElA->Draw();    
    fh_FakeA->Draw("SAME");
    
    c1->cd(9);  
    fh_FakeB->SetLineStyle(2);
    fh_TrueElB->SetLineWidth(2);
    fh_TrueElB->SetLineStyle(9);     
    fh_TrueElB->GetXaxis()->SetTitle("B axis, [cm]");
    fh_TrueElB->GetYaxis()->SetTitle("Entries"); 
    fh_TrueElB->Draw();  
    fh_FakeB->Draw("SAME");

    c1->cd(10);    
    fh_FakePhi->SetLineStyle(2);
    fh_TrueElPhi->SetLineWidth(2);
    fh_TrueElPhi->SetLineStyle(9);       
    fh_TrueElPhi->GetXaxis()->SetTitle("phi, [rad]");
    fh_TrueElPhi->GetYaxis()->SetTitle("Entries");     
    fh_TrueElPhi->Draw();  
    fh_FakePhi->Draw("SAME"); 

    c1->cd(11);    
    fh_FakeStsMom->SetLineStyle(2);
    fh_TrueElStsMom->SetLineWidth(2);
    fh_TrueElStsMom->SetLineStyle(9);    
    fh_TrueElStsMom->GetXaxis()->SetTitle("mom, [Gev/c]");
    fh_TrueElStsMom->GetYaxis()->SetTitle("Entries");         
    fh_TrueElStsMom->Draw();  
    fh_FakeStsMom->Draw("SAME"); 
            
    c1->cd(12);  
    fh_TrueElPhiVsRadAngle->GetXaxis()->SetTitle("phi, [rad]");
    fh_TrueElPhiVsRadAngle->GetYaxis()->SetTitle("rad angle, [rad]");              
    fh_TrueElPhiVsRadAngle->Draw("COLZ");
    
    c1->cd(13); 
    fh_FakePhiVsRadAngle->GetXaxis()->SetTitle("phi, [rad]");
    fh_FakePhiVsRadAngle->GetYaxis()->SetTitle("rad angle, [rad]");               
    fh_FakePhiVsRadAngle->Draw("COLZ");
    
     c1->cd(14);         
    fh_TrueElRadiusVsMom->Draw("COLZ");
    
    c1->cd(15);         
    fh_FakeRadiusVsMom->Draw("COLZ"); 
    
    TCanvas *c2 = new TCanvas("CbmRichQa2","c2",1200,1000);
    c2->Divide(4,4);
    c2->cd(1);
    fh_NotTrueElNofHits->SetLineStyle(2);
    fh_TrueElNofHits->SetLineWidth(2);
    fh_TrueElNofHits->SetLineStyle(9);
    fh_TrueElNofHits->GetXaxis()->SetTitle("Number of Hits");
    fh_TrueElNofHits->GetYaxis()->SetTitle("Entries");
    fh_TrueElNofHits->Draw();      
    fh_NotTrueElNofHits->Draw("SAME");    
    gPad->SetLogy();  

    
    c2->cd(2);
    fh_NotTrueElDistance->SetLineStyle(2);
    fh_TrueElDistance->SetLineWidth(2);
    fh_TrueElDistance->SetLineStyle(9);
    fh_TrueElDistance->GetXaxis()->SetTitle("ring-track distance, [cm]");
    fh_TrueElDistance->GetYaxis()->SetTitle("Entries"); 
    fh_TrueElDistance->Draw();      
    fh_NotTrueElDistance->Draw("SAME");    
    gPad->SetLogy();  

    
    c2->cd(3);  
    fh_NotTrueElAngle->SetLineStyle(2);
    fh_TrueElAngle->SetLineWidth(2);
    fh_TrueElAngle->SetLineStyle(9);
    fh_TrueElAngle->GetXaxis()->SetTitle("Angle, [rad]");
    fh_TrueElAngle->GetYaxis()->SetTitle("Entries"); 
    fh_TrueElAngle->Draw();     
    fh_NotTrueElAngle->Draw("SAME");               
    gPad->SetLogy();  

    
    c2->cd(4);
    fh_NotTrueElNofHitsOnRing->SetLineStyle(2);
    fh_TrueElNofHitsOnRing->SetLineWidth(2);
    fh_TrueElNofHitsOnRing->SetLineStyle(9);
    fh_TrueElNofHitsOnRing->GetXaxis()->SetTitle("TBSum");
    fh_TrueElNofHitsOnRing->GetYaxis()->SetTitle("Entries");  
    fh_TrueElNofHitsOnRing->Draw();    
    fh_NotTrueElNofHitsOnRing->Draw("SAME");              
    gPad->SetLogy();  

    
    c2->cd(5);  
    fh_NotTrueElRadPos->SetLineStyle(2);
    fh_TrueElRadPos->SetLineWidth(2);
    fh_TrueElRadPos->SetLineStyle(9);   
    fh_TrueElRadPos->GetXaxis()->SetTitle("Radial posotion, [cm]");
    fh_TrueElRadPos->GetYaxis()->SetTitle("Entries"); 
    fh_TrueElRadPos->Draw();       
    fh_NotTrueElRadPos->Draw("SAME");      
    gPad->SetLogy();   

    
    c2->cd(6);  
    fh_NotTrueElChi2->SetLineStyle(2);
    fh_TrueElChi2->SetLineWidth(2);
    fh_TrueElChi2->SetLineStyle(9);     
    fh_TrueElChi2->GetXaxis()->SetTitle("chi2");
    fh_TrueElChi2->GetYaxis()->SetTitle("Entries"); 
    fh_TrueElChi2->Draw();      
    fh_NotTrueElChi2->Draw("SAME");    
    gPad->SetLogy();  


    c2->cd(7);    
    fh_NotTrueElRadius->SetLineStyle(2);
    fh_TrueElRadius->SetLineWidth(2);
    fh_TrueElRadius->SetLineStyle(9); 
    fh_TrueElRadius->Draw();       
    fh_NotTrueElRadius->Draw("SAME");         
    gPad->SetLogy();  

    
    c2->cd(8);  
    fh_NotTrueElA->SetLineStyle(2);
    fh_TrueElA->SetLineWidth(2);
    fh_TrueElA->SetLineStyle(9);   
    fh_TrueElA->GetXaxis()->SetTitle("A axis, [cm]");
    fh_TrueElA->GetYaxis()->SetTitle("Entries");  
    fh_TrueElA->Draw();       
    fh_NotTrueElA->Draw("SAME");    
    gPad->SetLogy();    

    
    c2->cd(9);  
    fh_NotTrueElB->SetLineStyle(2);
    fh_TrueElB->SetLineWidth(2);
    fh_TrueElB->SetLineStyle(9);     
    fh_TrueElB->GetXaxis()->SetTitle("B axis, [cm]");
    fh_TrueElB->GetYaxis()->SetTitle("Entries"); 
    fh_TrueElB->Draw();    
    fh_NotTrueElB->Draw("SAME");    
    gPad->SetLogy();  


    c2->cd(10);    
    fh_NotTrueElPhi->SetLineStyle(2);
    fh_TrueElPhi->SetLineWidth(2);
    fh_TrueElPhi->SetLineStyle(9);  
    fh_TrueElPhi->Draw();    
    fh_NotTrueElPhi->Draw("SAME");           
    gPad->SetLogy();  
      
    c2->cd(11);    
    fh_NotTrueElStsMom->SetLineStyle(2);
    fh_TrueElStsMom->SetLineWidth(2);
    fh_TrueElStsMom->SetLineStyle(9);
    fh_TrueElStsMom->Draw();       
    fh_NotTrueElStsMom->Draw("SAME");           
    gPad->SetLogy();  
       
    c2->cd(12);         
    fh_NotTrueElPhiVsRadAngle->Draw("COLZ");    
    
    c2->cd(13);         
    fh_NotTrueElRadiusVsMom->Draw("COLZ");       

               
	TCanvas *c3 = new TCanvas("CbmRichQa3","c3",1200,1000);
    fh_TrueFoundElRingsProjHitCutMom->SetBins(20,0,10);
    fh_MCElRingsProjHitCutMom->SetBins(20,0,10);     
	fh_TrueFoundElRingsProjHitCutMom->Sumw2();
	fh_MCElRingsProjHitCutMom->Sumw2();	
	TH1D* th1 = new TH1D("th1","Efficiency, electrons vs momentum;momentum, GeV;efficiency",20,0,10);
	th1->Divide(fh_TrueFoundElRingsProjHitCutMom,fh_MCElRingsProjHitCutMom,100);
	th1->Draw();
	
	
	TCanvas *c4 = new TCanvas("CbmRichQa4","c4",1200,1000);
    fh_TrueFoundElRingsProjHitCutRadPos->SetBins(20,0,150);
    fh_MCElRingsProjHitCutRadPos->SetBins(20,0,150);    
	fh_TrueFoundElRingsProjHitCutRadPos->Sumw2();
	fh_MCElRingsProjHitCutRadPos->Sumw2();
	TH1D* th2 = new TH1D("th1","Efficiency, electrons vs RadialPosition;radial position, cm;efficiency",20,0,150);
	th2->Divide(fh_TrueFoundElRingsProjHitCutRadPos,fh_MCElRingsProjHitCutRadPos,100);
	th2->Draw();
    
    TCanvas *c5 = new TCanvas("CbmRichQa5","c5",1200,1000);
    fh_TrueMatchElMom->SetBins(20,0,10);    
    fh_TrueMatchElMom->Sumw2();
    //fh_MCElRingsProjHitCutRadPos->Sumw2();
    TH1D* th3 = new TH1D("th3","Efficiency, ring-track matching vs momentum;momentum, GeV;efficiency",20,0,10);
    th3->Divide(fh_TrueMatchElMom,fh_TrueFoundElRingsProjHitCutMom,100);
    th3->Draw();   
	
    TCanvas *c6 = new TCanvas("CbmRichQa6","c6",1200,1000);    
    fh_StartVertexXZ->Draw("COLZ");
     
    TCanvas *c7 = new TCanvas("CbmRichQa7","c7",1200,1000); 
    fh_TrueFoundElRingsProjHitCutNofHits->Sumw2();
    fh_MCElRingsProjHitCutNofHits->Sumw2();
	TH1D* thNofHits = new TH1D("thNofHits","Efficiency, electrons vs NofHits;Nof Hits; efficiency, %",20,0,40);
	thNofHits->Divide(fh_TrueFoundElRingsProjHitCutNofHits,fh_MCElRingsProjHitCutNofHits,100);
	thNofHits->SetMaximum(100);
	thNofHits->Draw();
	
	
    TCanvas *c8 = new TCanvas("CbmRichQa8","c8",1200,1000); 
    fh_MCElRingsProjHitCutBoverA->Sumw2();
    fh_TrueFoundElRingsProjHitCutBoverA->Sumw2();
	TH1D* thBoverA = new TH1D("BoverA","Efficiency, electrons vs B/A;B/A; efficiency, %",20,0,1);
	thBoverA->Divide(fh_TrueFoundElRingsProjHitCutBoverA,fh_MCElRingsProjHitCutBoverA,100);
	thBoverA->SetMaximum(100);
	thBoverA->Draw();

	
  //  TCanvas *c8 = new TCanvas("CbmRichQa8","c8",1200,1000); 
  //  fh_MCElRingsProjHitCutBoverA->Draw();
    
    //fh_MCXYE->Draw("COLZ");
    
    cout << "El. eff = " << (Double_t)fh_TrueFoundElRingsProjHitCutMom->GetEntries()/
                            (Double_t)fh_MCElRingsProjHitCutMom->GetEntries() << endl;
                            
    cout << "Matching eff = " << (Double_t)fh_TrueMatchElMom->GetEntries()/
                                (Double_t)fh_TrueFoundElRingsProjHitCutMom->GetEntries() << endl;
                                
    cout << "Nof Fakes = " << fh_FakePhi->GetEntries()<< endl;
}


