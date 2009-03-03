/*  Description: This macro train the ANN for fake
     rejection algorithm.

    Author : Simeon Lebedev
    E-mail : S.Lebedev@gsi.de
*/
{
    Int_t i;
    std::ifstream  finFakeAndTrue("ann_fake_and_true.txt");

    if(!gROOT->GetClass("TMultiLayerPerceptron"))
        gSystem->Load("libMLP");

    TTree *simu = new TTree ("MonteCarlo","MontecarloData");
    Float_t x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11;


    simu->Branch("x1", &x1,"x1/F");
    //simu->Branch("x2", &x2,"x2/F");
    simu->Branch("x3", &x3,"x3/F");
    simu->Branch("x4", &x4,"x4/F");
    simu->Branch("x5", &x5,"x5/F");
    simu->Branch("x6", &x6,"x6/F");
    simu->Branch("x7", &x7,"x7/F");
    simu->Branch("x8", &x8,"x8/F");
    simu->Branch("x9", &x9,"x9/F");
    simu->Branch("x10", &x10,"x10/F");
    simu->Branch("x11", &x11,"x11/F");

    Int_t maxNofFake = 1500;
    Int_t maxNofTrue = 1500;
    Int_t inputCountTrue = 0;
    Int_t inputCountFake = 0;

    while ( !finFakeAndTrue.eof() ){
        finFakeAndTrue >> x1  >> x2 >> x3 >> x4 >> x5 >> x6 >> x7 >> x8 >> x9 >> x10 >>x11;

        //cout << " "<< x1 << " "<<  x2 << " " << x3 << " "<< x4 << " "<< x5 << " " <<
        //x6 << " " << x7 << " " << x8 << " "<< x9  << " " << x10 << " " << x11 << endl;

        if ( x11 == -1 && inputCountFake < maxNofFake){
            inputCountFake++;
            simu->Fill();
        }
        if (x11 == 1 && inputCountTrue < maxNofTrue){
            inputCountTrue++;
            simu->Fill();
        }
        if (inputCountFake >= maxNofFake&& inputCountTrue >= maxNofTrue){
            break;
        }
    }

    TMultiLayerPerceptron network("x1,x3,x4,x5,x6,x7,x8,x9,x10:20:x11",simu,"Entry$+1");
   // network.LoadWeights("NeuralNet_RingSelection_Weights_Ellipse.txt");
    network.Train(500,"text,update=1");
    network.DumpWeights("NeuralNet_RingSelection_Weights.txt");

    Double_t minEval = -1.3;
    Double_t maxEval = 1.3;

    TH1F *hANNOutputFake = new TH1F("hANNOutputFake","ANN output;ANN output;Entries",100, minEval, maxEval);
    TH1F *hANNOutputTrue = new TH1F("hANNOutputTrue","ANN optput;ANN output;Entries",100, minEval, maxEval);

    TH1F *hCumProbFake = new TH1F("hCumProbFake","ANN output;ANN output;Cumulative probability",100, minEval, maxEval);
    TH1F *hCumProbTrue = new TH1F("hCumProbTrue","ANN optput;ANN output;Cumulative probability",100, minEval, maxEval);


    //hANNOutputTrue->SetLogx();
    Double_t ANNCut = -.5;
    Double_t params[10];

    std::ifstream  finTest("ann_fake_and_true.txt");
    Int_t NofFakeLikeTrue = 0;
    Int_t NofTrueLikeFake = 0;
    Int_t NofTrueTest = 0;
    Int_t NofFakeTest = 0;

    while ( !finTest.eof() ){
        finTest >> x1  >> x2 >> x3 >> x4 >> x5 >> x6 >> x7 >> x8 >> x9 >> x10 >> x11;

       // cout << " "<< x1 << " "<<  x2 << " " << x3 << " "<< x4 << " "<< x5 << " " <<
       // x6 << " " << x7 << " " << x8 << " "<< x9  << " " << x10 << " " << x11 << endl;

        if (x11 == -1)NofFakeTest++;
        if (x11 == 1 )NofTrueTest++;

        params[0] =  x1;
      //  params[1] =  x2;
        params[1] =  x3;
        params[2] =  x4;
        params[3] =  x5;
        params[4] =  x6;
        params[5] =  x7;
        params[6] =  x8;
        params[7] =  x9;
        params[8] =  x10;

        Double_t netEval = network.Evaluate(0,params);
    	if (netEval > maxEval) netEval = maxEval - 0.01;
    	if (netEval < minEval) netEval = minEval + 0.01;

        if (x11 == -1) hANNOutputFake->Fill(netEval);
        if (x11 == 1) hANNOutputTrue->Fill(netEval);

        if (netEval > ANNCut && x11 == -1) NofFakeLikeTrue++;
        if (netEval < ANNCut && x11 == 1) NofTrueLikeFake++;
    }

    cout <<"NofTrue = " <<NofTrueTest<<endl;
    cout <<"NofFake = " <<NofFakeTest<<endl;
    cout <<"Fake like True = " <<NofFakeLikeTrue<<endl;
    cout <<"True like Fake = " <<NofTrueLikeFake<<endl;

    Double_t cumProbFake = 0.;
    Double_t cumProbTrue = 0.;
    Int_t nofFake = hANNOutputFake->GetEntries();
    Int_t nofTrue = hANNOutputTrue->GetEntries();
    for (Int_t i = 1; i <= hANNOutputFake->GetNbinsX(); i++){
		cumProbFake += hANNOutputFake->GetBinContent(i);
		hCumProbFake->SetBinContent(i, (Double_t)cumProbFake/nofFake);

		cumProbTrue += hANNOutputTrue->GetBinContent(i);
	    hCumProbTrue->SetBinContent(i, 1. - (Double_t)cumProbTrue / nofTrue);
    }

    TCanvas* c1 = new TCanvas();
    c1->Divide(1,2);
    c1->cd(1);
    hANNOutputTrue->SetLineWidth(3);
    hANNOutputFake->SetLineStyle(2);
    hANNOutputFake->SetLineWidth(3);
    hANNOutputTrue->Draw();
    hANNOutputFake->Draw("Same");
    gPad->SetLogy();
    c1->cd(2);
    hCumProbFake->SetLineWidth(3);
    hCumProbFake->SetLineStyle(2);
    hCumProbFake->Draw();
    hCumProbTrue->SetLineWidth(3);
    hCumProbTrue->Draw("same");



    gPad->SetLogy();
}
