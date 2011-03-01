void glue_histo ()
{
    TString filePath = "/lustre/cbm/user/ebelolap/oct10/urqmd_rho0/25gev/100_field/real/elid.qa";
    TString outFileName = filePath +".all.root";
    Int_t nEvents = 200;   // number of files to be added

    TList* histList = new TList();

//signal electron acceptance
	histList->Add( new TH1D("fh_mc_el", "fh_mc_el;momentum, GeV/c;Entries", nofBinsMom,0,15) );
	histList->Add( new TH1D("fh_acc_sts_el", "fh_acc_sts_el;momentum, GeV/c;Entries", nofBinsMom,0,15) );
	histList->Add( new TH1D("fh_acc_rich_el", "fh_acc_rich_el;momentum, GeV/c;Entries", nofBinsMom,0,15) );
	histList->Add( new TH1D("fh_acc_trd_el", "fh_acc_trd_el;momentum, GeV/c;Entries", nofBinsMom,0,15) );
	histList->Add( new TH1D("fh_acc_tof_el", "fh_acc_tof_el;momentum, GeV/c;Entries", nofBinsMom,0,15) );

//signal electron matching
	histList->Add( new TH1D("fh_match_rich", "fh_match_rich;momentum, GeV/c;Entries", nofBinsMom,0,15) );
	histList->Add( new TH1D("fh_match_trd", "fh_match_trd;momentum, GeV/c;Entries", nofBinsMom,0,15) );
	histList->Add( new TH1D("fh_match_tof", "fh_match_tof;momentum, GeV/c;Entries", nofBinsMom,0,15) );

//ELECTRON IDENTIFICATION AND PION SUPPRESSION
	histList->Add( new TH1D("fh_elid_rich", "fh_elid_rich;momentum, GeV/c;Entries", nofBinsMom,0,15) );
	histList->Add( new TH1D("fh_elid_trd", "fh_elid_trd;momentum, GeV/c;Entries", nofBinsMom,0,15) );
	histList->Add( new TH1D("fh_elid_tof", "fh_elid_tof;momentum, GeV/c;Entries", nofBinsMom,0,15) );

	histList->Add( new TH1D("fh_acc_pi", "fh_acc_pi;momentum, GeV/c;Entries", nofBinsMom,0,15) );
	histList->Add( new TH1D("fh_pi_as_el_rich", "fh_pi_as_el_rich;momentum, GeV/c;Entries", nofBinsMom,0,15) );
	histList->Add( new TH1D("fh_pi_as_el_trd", "fh_pi_as_el_trd;momentum, GeV/c;Entries", nofBinsMom,0,15) );
	histList->Add( new TH1D("fh_pi_as_el_tof", "fh_pi_as_el_tof;momentum, GeV/c;Entries", nofBinsMom,0,15) );

//test distributions
	histList->Add( new TH1D("fh_nof_global", "fh_nof_global;nof global tracks/event;Entries", nofBinsMom,0,1000) );
	histList->Add( new TH1D("fh_nof_sts_tracks", "fh_nof_sts_tracks;nof STS tracks/event;Entries", nofBinsMom,0,1000) );
	histList->Add( new TH1D("fh_nof_rich_rings", "fh_nof_rich_rings;nof RICH rings/event;Entries", nofBinsMom,0,100) );
	histList->Add( new TH1D("fh_nof_trd_tracks", "fh_nof_trd_tracks;nof TRD tracks/event;Entries", nofBinsMom,0,1000) );

//STS Qa
	histList->Add( new TH1D("fh_rec_mc_mom_signal","fh_rec_mc_mom_signal;#Delta p/p [%];yeild",100, -10., 10.) );
	histList->Add( new TH2D("fh_mom_res_vs_mom_signal", "fh_mom_res_vs_mom_signal;p [GeV/c];#Delta p/p [%]",100, 0., 15., 100, -10., 10.) );
	histList->Add( new TH1D("fh_mean_mom_vs_mom_signal","fh_mean_mom_vs_mom_signal",100, 0., 15.) );
	histList->Add( new TH1D("fh_count_mom_vs_mom_signal","fh_count_mom_vs_mom_signal", 100, 0., 15.) );
	histList->Add( new TH1D("fh_chiprim_signal", "fh_chiprim_signal;chi2,yeild", 200, 0., 20.) );
	histList->Add( new TH1D("fh_chiprim_signal2", "fh_chiprim_signal2;chi2,yeild", 200, 0., 20.) );

    Int_t nofGoodFiles = 0;
    for(Int_t i = 0; i < nEvents; i++) {
        char str[4];
        sprintf(str, "%04d", i);
        TString fileName = filePath + "." + TString(str) + ".root";
        cout << "-I- " + fileName << endl;
        TFile* file = new TFile(fileName);
        if (file->IsZombie()) continue;

        for (Int_t iH = 0; iH < histList->GetEntries(); iH++){
        	TH1* h = (TH1*)histList->At(iH);
        	h->Add((TH2D*) file->Get(h->GetName()));
        }

        nofGoodFiles++;
        file->Close();
    }

    Double_t scale = 1./(Double_t) nofGoodFiles;
    TFile* outFile = new TFile(outFileName, "RECREATE");
    for (Int_t iH = 0; iH < histList->GetEntries(); iH++){
    	TH1* h = (TH1*)histList->At(iH);
        h->Scale(scale);
        h->Write();
    }

    cout <<"nofGoodFiles = " << nofGoodFiles << endl;
}
