/*  Description: This macro produces txt files with energy losses.
Also it draws different plots concerning electron identification in TRD.

 Author : Simeon Lebedev
 E-mail : S.Lebedev@gsi.de
 */

Double_t fitFunction(Double_t *x, Double_t *par)
{
	Double_t xx = x[0];
	Double_t c = ( (par[0]/( sqrt(2*TMath::Pi()) ) * par[1] * xx ) );
	Double_t c1 = TMath::Exp( (-1. / (2. * par[1] * par[1])) * pow(log(xx) - par[2], 2) );
	return c * c1;
}

void trd_elid_make_txt ()
{
    TStopwatch timer;
    timer.Start();

    std::ofstream foutEl("electrons_mom_FEB09.txt");
    std::ofstream foutPi("pions_mom_FEB09.txt");


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




    Double_t result, losshit;
    Double_t eLossdEdX[12], eLossTR[12], eLoss[12];
    char fileMC[80], fileRec[80];


    TH1F* hPiELoss = new TH1F("hPiELoss","Energy loss for pions in one layer;energy loss, keV;entries",100, 0, 50);
    TH1F* hPiELossSum = new TH1F("hPiELossSum","Sum of energy losses for pions in all layers;energy loss, GeV;entries",100, 0, 3.e-4);

    TH1F* hEldEdX = new TH1F("hEldEdX","dEdX for electrons in one layer;dEdX, GeV;entries",100, 0, 1.5e-5);
    TH1F* hElTR = new TH1F("hElTR","TR for electrons in one layer;TR, GeV;entries",100, 0, 1.5e-5);
    TH1F* hElELoss = new TH1F("hElELoss","Energy loss (dEdX+TR) for electrons in one layer;energy loss, GeV;entries",100, 0, 5.0e-5);
    TH1F* hElElossSum = new TH1F("hElELossSum","Sum of energy losses (dEdX+TR) for electrons in all layers;energy loss, GeV;entries",100, 0, 3.e-4);
    TH1F* hElNofZeroTR = new TH1F("hElNofZeroTR","Number of Zero TR layers;number of layers; entries",13, 0., 13.);


    int nofZeroTR = 0;
    int nofElTracks = 0, nofPiTracks = 0;
    int nofElTracksNoMomCut = 0, nofPiTracksNoMomCut = 0;

    for(int iFile=0; iFile <=0; iFile++){
        sprintf(fileMC,"/d/cbm02/slebedev/trd/FEB09/mom.piel.%.4i.mc.root",iFile);
        cout<<fileMC<<endl;
        TFile *f1 = new TFile(fileMC,"R");
        TTree* t1 = (TTree*)f1->Get("cbmsim");
        TFolder *fd1 = f1->Get("cbmroot");
        TClonesArray* fListStack = (TClonesArray*) fd1->FindObjectAny("MCTrack");
        t1->SetBranchAddress("MCTrack",&fListStack);
        TClonesArray *pointsTRD = (TClonesArray*) fd1->FindObjectAny("TRDPoint");
        t1->SetBranchAddress(pointsTRD->GetName(),&pointsTRD);

        sprintf(fileRec,"/d/cbm02/slebedev/trd/FEB09/mom.piel.%.4i.reco.root",iFile);
        cout << fileRec <<endl;
        TFile *f = new TFile(fileRec,"R");
        TTree* t = f->Get("cbmsim");
        TFolder *fd = f->Get("cbmout");
        TClonesArray *tracksTRD = (TClonesArray*) fd->FindObjectAny("TRDTrack");
        t->SetBranchAddress(tracksTRD->GetName(),&tracksTRD);
        TClonesArray *matchTRD = (TClonesArray*) fd->FindObjectAny("TRDTrackMatch");
        t->SetBranchAddress(matchTRD->GetName(),&matchTRD);
        TClonesArray *hitsTRD = (TClonesArray*) fd->FindObjectAny("TRDHit");
        t->SetBranchAddress(hitsTRD->GetName(),&hitsTRD);

        Int_t nEvents=t->GetEntries();
        cout<<" nEvents ="<<nEvents<<endl;
        Int_t nEvents=100;
        for(Int_t ievent=0;ievent<nEvents;ievent++ ) {
        cout<<"ievent = "<<ievent<<endl;
        CbmTrdTrack *trdtrack=NULL;
        CbmTrdHit *trdhit=NULL;
        CbmMCTrack* mctrack =NULL;

        t1->GetEntry(ievent);
        t->GetEntry(ievent);

        Int_t NofTrdTracks=tracksTRD->GetEntries();

        for(Int_t iTrdTrack=0; iTrdTrack < NofTrdTracks; iTrdTrack++){

            CbmTrdTrack* trdtrack = (CbmTrdTrack*)tracksTRD->At(iTrdTrack);
            Int_t nHits = trdtrack->GetNofTrdHits();
            if (nHits < 12){
                continue;
            }

            CbmTrdTrackMatch* match = (CbmTrdTrackMatch*) matchTRD->At(iTrdTrack);
            Int_t iMC = match->GetMCTrackID();
            if (iMC == -1) continue;
            if(iMC> fListStack->GetEntriesFast())continue;

            mctrack=(CbmMCTrack*)fListStack->At(iMC);
            Int_t partPdg = TMath::Abs(mctrack->GetPdgCode());
            Double_t momMC = mctrack->GetP();
            Double_t PtMC = mctrack->GetPt();
            Double_t Pz = mctrack->GetPz();
            //cout<<PtMC << endl;
            Double_t motherId = mctrack->GetMotherId();

            if (partPdg == 50000050) continue;

            ///only primary tracks
            if (motherId != -1) continue;

            /// Momentum at the first TRD layer
            Int_t hitIndex =  trdtrack->GetTrdHitIndex(0);
            trdhit = (CbmTrdHit*) hitsTRD->At(hitIndex);
            Int_t iPoint = trdhit->GetRefIndex();
            if ( iPoint < 0 )continue;
            CbmMCPoint* point = (CbmMCPoint*) pointsTRD->At(iPoint);
            TVector3 mom;
            point->Momentum(mom);

            if (TMath::Abs(partPdg) == 11)nofElTracksNoMomCut++;
            if (TMath::Abs(partPdg) == 211)nofPiTracksNoMomCut++;

            if ( mom.Mag() < .5 ) continue;


            double sumELoss = 0;
            double nofZeroTRLayers = 0;
            for (Int_t iHit=0; iHit<12; iHit++){
                Int_t hitIndex =  trdtrack->GetTrdHitIndex(iHit);
                trdhit = (CbmTrdHit*) hitsTRD->At(hitIndex);

                losshit =  trdhit->GetELoss();
                eLoss[iHit] = losshit;
                sumELoss += losshit;
                eLossTR[iHit] = trdhit->GetELossTR();
                eLossdEdX[iHit] = trdhit->GetELossdEdX();

                if (eLossTR[iHit] < 1e-9) nofZeroTRLayers++;

            } //iHit

            ///pions
            if (TMath::Abs(partPdg) == 211){

                hPiELossSum->Fill(sumELoss);
                hPiELoss->Fill(eLossdEdX[0] * 1e6);
                nofPiTracks++;
            }

            ///electrons
            if (TMath::Abs(partPdg) == 11){
                hElElossSum->Fill(sumELoss);
                hElNofZeroTR->Fill(nofZeroTRLayers);
                //if (eLossTR[0] != 0){
                    hElTR->Fill(eLossTR[0]);
                    hEldEdX->Fill(eLossdEdX[0]);
                    hElELoss->Fill(eLoss[0]);
                //}
                    nofElTracks++;
            }


            if (nofZeroTRLayers == 12 && TMath::Abs(partPdg) == 11) nofZeroTR++;

            if (abs(partPdg) == 211){
                for (Int_t iHit=0; iHit<12; iHit++){
                    foutPi << eLossdEdX[iHit] <<" "<<eLossTR[iHit] <<" "<<eLoss[iHit] <<" ";
                }
                foutPi<< momMC<<endl;
            }

            if (abs(partPdg) == 11){
                for (Int_t iHit=0; iHit<12; iHit++){
                    foutEl << eLossdEdX[iHit] <<" "<<eLossTR[iHit] <<" "<<eLoss[iHit] <<" ";
                }
                foutEl<< momMC<<endl;
	    }

        }

        } //event
        f->Close();
        f1->Close();
    }//iFile
    cout << "nofPiTracks = " << nofPiTracks << endl;
    cout << "nofElTracks = " << nofElTracks << endl;
    cout << "nofElTracksNoMomCut = " << nofElTracksNoMomCut << endl;
    cout << "nofPiTracksNoMomCut = " << nofPiTracksNoMomCut << endl;
    cout << "Lost of El Tracks, TRD Mom Cut at 0.5, * 100% = " << (nofElTracksNoMomCut - nofElTracks)/ (double)nofElTracksNoMomCut * 100<< endl;
    cout << "Lost of Pi Tracks, TRD Mom Cut at 0.5, * 100%  = " << (nofPiTracksNoMomCut - nofPiTracks)/ (double)nofPiTracksNoMomCut * 100<< endl;

    cout << "nofZeroTR El Tracks = " << nofZeroTR << endl;
    cout << "nofZeroTR El Tracks / nofElTracks * 100 %= " << (double)nofZeroTR/nofElTracks *100 << endl;

    cout << "mean value of sum of ELoss for El = " << hElELossSum->GetMean() << endl;
    cout << "mean value of sum of ELoss for Pi = " << hPiELossSum->GetMean() << endl;
    cout << "hElELossSum - hPiELossSum= " << hElELossSum->GetMean() - hPiELossSum->GetMean() << endl;

    gStyle->SetPalette(1,0);
    gStyle->SetHistLineWidth(3);

    hPiELossSum->SetLineWidth(3);
    hElELossSum->SetLineWidth(3);
    hPiELoss->SetLineWidth(3);
    hElNofZeroTR->SetLineWidth(3);
    hElTR->SetLineWidth(3);
    hEldEdX->SetLineWidth(3);
    hElELoss->SetLineWidth(3);

    ///blue line for electrons and black for pions(default color)
    hElELossSum->SetLineStyle(2);
    hElNofZeroTR->SetLineStyle(2);
    hElTR->SetLineStyle(2);
    hEldEdX->SetLineStyle(2);
    hElELoss->SetLineStyle(2);



    c1 = new TCanvas("c1","c1",10,10,800,800);
    c1->Divide(2,3);
    c1->cd(1);
    hPiELossSum->Draw();
    hElELossSum->Draw("same");

    c1->cd(2);
    hPiELoss->Draw();
    double maxX = 50, minX = 0;
	TF1 *fitFcn = new TF1("fitLogNorm",fitFunction, minX, maxX, 3);
	fitFcn->SetParameter(1, 1000.);
	fitFcn->SetParameter(2, 0.7);
	hPiELoss->Fit("fitLogNorm","","",minX, maxX);
	//hPiELoss->Fit("landau");
	gPad->SetGridx(true);
	gPad->SetGridy(true);

	double par[3];
	par[0] = fitFcn->GetParameter(0);
	par[1] = fitFcn->GetParameter(1);
	par[2] = fitFcn->GetParameter(2);

	int nofPoints = 5000;
	double curX = 0;
	double dx = maxX/nofPoints;
	double maximum = 0.;//fitFcn->GetMaximum();
	double maximumX = 0.;//fitFcn->GetMaximumX();
	for (int i = 0; i < nofPoints; i++){
		double y = fitFunction(&curX, par);
		if (y > maximum){
			maximum = y;
			maximumX = curX;
		}
		curX+=dx;
	}

	double halfMaxF = maximum/ 2.;
	double x0 = -1;
	double x1 = -1;
	curX = 0.;
	for (int i = 0; i < nofPoints; i++){
		double y = fitFunction(&curX, par);
		if (y > halfMaxF && x0 == -1) x0 = curX;
		if (y < halfMaxF && x0 != -1){
			x1 = curX;
			break;
		}
		curX+=dx;
	}
	cout << "Maximum = " << maximum <<"  MaximumX = " <<maximumX<< endl;
	cout << "x0 = " << x0 <<
		"  x1=" << x1 <<
		"  FWHM = " << x1 - x0 <<
		"  FWHM/4.02 = "<< (x1 - x0)/4.02 << endl;

    c1->cd(3);
    hElNofZeroTR->Draw();

    c1->cd(4);
    hElTR->Draw();

    c1->cd(5);
    hEldEdX->Draw();

    c1->cd(6);
    hElELoss->Draw();

}
