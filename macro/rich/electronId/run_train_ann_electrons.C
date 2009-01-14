/*  Description: This macro train the ANN for fake 
     rejection algorithm.

    Author : Simeon Lebedev
    E-mail : S.Lebedev@gsi.de
*/
{
    Int_t i;
    std::ifstream  fin("ann_el_pi.txt");
    
    if(!gROOT->GetClass("TMultiLayerPerceptron"))
        gSystem->Load("libMLP");
    
    TTree *simu = new TTree ("MonteCarlo","MontecarloData");
    Float_t x1,x2,x3,x4,x5,x6,x7,x8,x9,x10;


    simu->Branch("x1", &x1,"x1/F");
    simu->Branch("x2", &x2,"x2/F");
    simu->Branch("x3", &x3,"x3/F");
    simu->Branch("x4", &x4,"x4/F");
    simu->Branch("x5", &x5,"x5/F");
    simu->Branch("x6", &x6,"x6/F");
    simu->Branch("x7", &x7,"x7/F");
    simu->Branch("x8", &x8,"x8/F");
    simu->Branch("x9", &x9,"x9/F");
    simu->Branch("x10", &x10,"x10/F");    
   // simu->Branch("x11", &x11,"x11/F");
        
    Int_t maxNofEl = 1500;
    Int_t maxNofPi = 1500;
    Int_t inputCountEl = 0;
    Int_t inputCountPi = 0;
    
    while ( !fin.eof() ){
        fin >> x1  >> x2 >> x3 >> x4 >> x5 >> x6 >> x7 >> x8 >> x9 >> x10 ;
        
        //cout << " "<< x1 << " "<<  x2 << " " << x3 << " "<< x4 << " "<< x5 << " " <<
        //x6 << " " << x7 << " " << x8 << " "<< x9  << " " << x10 << " " << x11 << endl;
        
        if ( x10 == -1 && inputCountPi < maxNofPi){
            inputCountPi++;
            simu->Fill();
        }
        if (x10 == 1 && inputCountEl < maxNofEl){
            inputCountEl++;
            simu->Fill();
        }
        if (inputCountPi >= maxNofPi && inputCountEl >= maxNofEl){
            break;
        } 
    }

    TMultiLayerPerceptron network("x1,x2,x3,x4,x5,x6,x7,x8,x9:20:x10",simu,"Entry$+1");
  //  network.LoadWeights("el_id_ann_weights.txt");
    network.Train(250,"text,update=1");
    network.DumpWeights("el_id_ann_weights.txt");
    
    Double_t minEval = -1.3;
    Double_t maxEval = 1.3;
    
    TH1F *hANNOutputPi = new TH1F("hANNOutputPi","ANN output;ANN output;Entries",100, minEval, maxEval);
    TH1F *hANNOutputEl = new TH1F("hANNOutputEl","ANN optput;ANN output;Entries",100, minEval, maxEval);

    TH1F *hCumProbPi = new TH1F("hCumProbPi","ANN output;ANN output;Cumulative probability",100, minEval, maxEval);
    TH1F *hCumProbEl = new TH1F("hCumProbEl","ANN optput;ANN output;Cumulative probability",100, minEval, maxEval);
   
    
    //hANNOutputTrue->SetLogx();    
    Double_t ANNCut = .5;
    Double_t params[10];

    std::ifstream  finTest("ann_el_pi.txt");
    Int_t NofPiLikeEl = 0;
    Int_t NofElLikePi = 0;
    Int_t NofElTest = 0;
    Int_t NofPiTest = 0;

    while ( !finTest.eof() ){
        finTest >> x1  >> x2 >> x3 >> x4 >> x5 >> x6 >> x7 >> x8 >> x9 >> x10;
        
       // cout << " "<< x1 << " "<<  x2 << " " << x3 << " "<< x4 << " "<< x5 << " " <<
       // x6 << " " << x7 << " " << x8 << " "<< x9  << " " << x10 << " " << x11 << endl;
                
        if (x10 == -1)NofPiTest++;        
        if (x10 == 1 )NofElTest++;
        
        params[0] =  x1;
        params[1] =  x2;
        params[2] =  x3;
        params[3] =  x4;
        params[4] =  x5;
        params[5] =  x6;
        params[6] =  x7;
        params[7] =  x8;
        params[8] =  x9;    
                    
        Double_t netEval = network.Evaluate(0,params);
    	if (netEval > maxEval) netEval = maxEval - 0.01;
    	if (netEval < minEval) netEval = minEval + 0.01;        
        
        if (x10 == -1) hANNOutputPi->Fill(netEval);
        if (x10 == 1) hANNOutputEl->Fill(netEval);
        
        if (netEval > ANNCut && x10 == -1) NofPiLikeEl++;
        if (netEval < ANNCut && x10 == 1) NofElLikePi++;    
    }    
    
    cout <<"NofEl = " <<NofElTest<<endl;
    cout <<"NofPi = " <<NofPiTest<<endl;
    cout <<"Pi identified as El = " <<NofPiLikeEl<<endl;
    cout <<"El identified as Pi = " <<NofElLikePi<<endl;
    
    cout <<"NofPiTest / Pi identified as El = " <<(Double_t)NofPiTest /NofPiLikeEl<<endl;
    cout <<"El identified as Pi / NofElTest= " <<(Double_t)NofElLikePi/ NofElTest*100<<endl;
    Double_t cumProbPi = 0.;
    Double_t cumProbEl = 0.;
    Int_t nofPi = hANNOutputPi->GetEntries();
    Int_t nofEl = hANNOutputEl->GetEntries();
    for (Int_t i = 1; i <= hANNOutputPi->GetNbinsX(); i++){
		cumProbPi += hANNOutputPi->GetBinContent(i);
		hCumProbPi->SetBinContent(i, (Double_t)cumProbPi/nofPi);
	
		cumProbEl += hANNOutputEl->GetBinContent(i);
	    hCumProbEl->SetBinContent(i, 1. - (Double_t)cumProbEl / nofEl);
    }
    
    TCanvas* c1 = new TCanvas();
    c1->Divide(1,2);
    c1->cd(1);
    hANNOutputEl->SetLineWidth(3);
    hANNOutputPi->SetLineStyle(2);    
    hANNOutputPi->SetLineWidth(3);    
    hANNOutputEl->Draw();
    hANNOutputPi->Draw("Same");
    gPad->SetLogy();    
    c1->cd(2);
    hCumProbPi->SetLineWidth(3);
    hCumProbPi->SetLineStyle(2);
    hCumProbPi->Draw();
    hCumProbEl->SetLineWidth(3);
    hCumProbEl->Draw("same");    
         
    gPad->SetLogy();     
}
