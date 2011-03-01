void glue_histo ()
{
    TString filePath = "/lustre/cbm/user/ebelolap/oct10/urqmd_rho0/25gev/100_field/real/mytask.analysis";//.0000.root";
    TString outFileName = filePath +".all.root";
    Int_t nEvents = 200;   // number of files to be added

    TList* histList = new TList();

//signal momentum distribution
    histList->Add( new TH1D("fh_mc_signal_mom", "fh_mc_signal_mom;momentum [GeV/c];yeild",100, 0., 15.) );
    histList->Add( new TH1D("fh_acc_signal_mom","fh_acc_signal_mom;momentum [GeV/c];yeild", 100, 0., 15.) );
    histList->Add( new TH1D("fh_sts_reco_signal_mom","fh_sts_reco_signal_mom;momentum [GeV/c];yeild", 100, 0., 15.) );
    histList->Add( new TH1D("fh_rich_reco_signal_mom","fh_rich_reco_signal_mom;momentum [GeV/c];yeild", 100, 0., 15.) );
    histList->Add( new TH1D("fh_reco_signal_mom","fh_reco_signal_mom;momentum [GeV/c];yeild", 100, 0., 15.) );
    histList->Add( new TH1D("fh_rich_id_signal_mom","fh_rich_id_signal_mom;momentum [GeV/c];yeild", 100, 0., 15.) );
    histList->Add( new TH1D("fh_trd_id_signal_mom","fh_trd_id_signal_mom;momentum [GeV/c];yeild", 100, 0., 15.) );
    histList->Add( new TH1D("fh_tof_id_signal_mom","fh_tof_id_signal_mom;momentum [GeV/c];yeild", 100, 0., 15.) );
    histList->Add( new TH1D("fh_chi_prim_signal_mom","fh_chi_prim_signal_mom;momentum [GeV/c];yeild", 100, 0., 15.) );
    histList->Add( new TH1D("fh_ptcut_signal_mom","fh_ptcut_signal_mom;momentum [GeV/c];yeild", 100, 0., 15.) );
    histList->Add( new TH1D("fh_anglecut_signal_mom","fh_anglecut_signal_mom;momentum [GeV/c];yeild", 100, 0., 15.) );
    histList->Add( new TH1D("fh_gammacut_signal_mom","fh_gammacut_signal_mom;momentum [GeV/c];yeild", 100, 0., 15.) );
    histList->Add( new TH1D("fh_ttcut_signal_mom","fh_ttcut_signal_mom;momentum [GeV/c];yeild", 100, 0., 15.) );
    histList->Add( new TH1D("fh_stcut_signal_mom","fh_stcut_signal_mom;momentum [GeV/c];yeild", 100, 0., 15.) );
    histList->Add( new TH1D("fh_apcut_signal_mom","fh_apcut_signal_mom;momentum [GeV/c];yeild", 100, 0., 15.) );

///////
    histList->Add( new TH1D("fh_mc_mother_pdg", "fh_mc_mother_pdg; Pdg code; Particles per event", 7000, -3500., 3500.) );
    histList->Add( new TH1D("fh_acc_mother_pdg","fh_acc_mother_pdg; Pdg code; Particles per event", 7000, -3500., 3500.) );

    histList->Add( new TH2D("fh_mc_vertex_gamma_xz","fh_mc_vertex_gamma_xz;Z [cm];X [cm]",200, -10., 110., 200, -100.,100.) );
    histList->Add( new TH2D("fh_mc_vertex_gamma_yz", "fh_mc_vertex_gamma_yz;Z [cm];Y [cm]", 200, -10., 110., 200, -100., 100.) );
    histList->Add( new TH2D("fh_mc_vertex_gamma_xy","fh_mc_vertex_gamma_xy;X [cm];Y [cm]", 200, -100.,100., 200, -100., 100.) );

//signal minv
	histList->Add( new TH1D("fh_mc_signal_minv","fh_mc_signal_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.) );
	histList->Add( new TH1D("fh_acc_signal_minv","fh_acc_signal_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.) );
	histList->Add( new TH1D("fh_rec_signal_minv","fh_rec_signal_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.) );
	histList->Add( new TH1D("fh_rich_id_signal_minv","fh_rich_id_signal_minv;M_{ee} [GeV/c^{2}];yeild", 200, 0., 2.) );
	histList->Add( new TH1D("fh_trd_id_signal_minv", "fh_trd_id_signal_minv;M_{ee} [GeV/c^{2}];yeild", 200, 0., 2.) );
	histList->Add( new TH1D("fh_tof_id_signal_minv","fh_tof_id_signal_minv;M_{ee} [GeV/c^{2}];yeild", 200, 0., 2.) );
	histList->Add( new TH1D("fh_chi_prim_signal_minv","fh_chi_prim_signal_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.) );
	histList->Add( new TH1D("fh_ptcut_signal_minv","fh_ptcut_signal_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.) );
	histList->Add( new TH1D("fh_anglecut_signal_minv","fh_anglecut_signal_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.) );
	histList->Add( new TH1D("fh_gammacut_signal_minv","fh_gammacut_signal_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.) );
	histList->Add( new TH1D("fh_ttcut_signal_minv","fh_ttcut_signal_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.) );
	histList->Add( new TH1D("fh_stcut_signal_minv","fh_stcut_signal_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.) );
	histList->Add( new TH1D("fh_apcut_signal_minv","fh_apcut_signal_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.) );

//BG minv
	histList->Add( new TH1D("fh_rec_bg_minv","fh_rec_bg_minv;M_{ee} [GeV/c^{2}];yeild", 200, 0., 2.) );
	histList->Add( new TH1D("fh_rich_id_bg_minv","fh_rich_id_bg_minv;M_{ee} [GeV/c^{2}];yeild", 200, 0., 2.) );
	histList->Add( new TH1D("fh_trd_id_bg_minv","fh_trd_id_bg_minv;M_{ee} [GeV/c^{2}];yeild", 200, 0., 2.) );
	histList->Add( new TH1D("fh_tof_id_bg_minv","fh_tof_id_bg_minv;M_{ee} [GeV/c^{2}];yeild", 200, 0., 2.) );
	histList->Add( new TH1D("fh_chi_prim_bg_minv","fh_chi_prim_bg_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.) );
	histList->Add( new TH1D("fh_ptcut_bg_minv","fh_ptcut_bg_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.) );
	histList->Add( new TH1D("fh_anglecut_bg_minv","fh_anglecut_bg_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.) );
	histList->Add( new TH1D("fh_gammacut_bg_minv","fh_gammacut_bg_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.) );
	histList->Add( new TH1D("fh_ttcut_bg_minv","fh_ttcut_bg_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.) );
	histList->Add( new TH1D("fh_stcut_bg_minv","fh_stcut_bg_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.) );
	histList->Add( new TH1D("fh_apcut_bg_minv","fh_apcut_bg_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.) );

//pi0 minv
	histList->Add( new TH1D("fh_rec_pi0_minv","fh_rec_pi0_minv;M_{ee} [GeV/c^{2}];yeild", 200, 0., 2.) );
	histList->Add( new TH1D("fh_rich_id_pi0_minv","fh_rich_id_pi0_minv;M_{ee} [GeV/c^{2}];yeild", 200, 0., 2.) );
	histList->Add( new TH1D("fh_trd_id_pi0_minv","fh_trd_id_pi0_minv;M_{ee} [GeV/c^{2}];yeild", 200, 0., 2.) );
	histList->Add( new TH1D("fh_tof_id_pi0_minv","fh_tof_id_pi0_minv;M_{ee} [GeV/c^{2}];yeild", 200, 0., 2.) );
	histList->Add( new TH1D("fh_chi_prim_pi0_minv","fh_chi_prim_pi0_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.) );
	histList->Add( new TH1D("fh_ptcut_pi0_minv","fh_ptcut_pi0_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.) );
	histList->Add( new TH1D("fh_anglecut_pi0_minv","fh_anglecut_pi0_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.) );
	histList->Add( new TH1D("fh_gammacut_pi0_minv","fh_gammacut_pi0_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.) );
	histList->Add( new TH1D("fh_ttcut_pi0_minv","fh_ttcut_pi0_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.) );
	histList->Add( new TH1D("fh_stcut_pi0_minv","fh_stcut_pi0_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.) );
	histList->Add( new TH1D("fh_apcut_pi0_minv","fh_apcut_pi0_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.) );

//eta minv
	histList->Add( new TH1D("fh_rec_eta_minv","fh_rec_eta_minv;M_{ee} [GeV/c^{2}];yeild", 200, 0., 2.) );
	histList->Add( new TH1D("fh_rich_id_eta_minv","fh_rich_id_eta_minv;M_{ee} [GeV/c^{2}];yeild", 200, 0., 2.) );
	histList->Add( new TH1D("fh_trd_id_eta_minv","fh_trd_id_eta_minv;M_{ee} [GeV/c^{2}];yeild", 200, 0., 2.) );
	histList->Add( new TH1D("fh_tof_id_eta_minv","fh_tof_id_eta_minv;M_{ee} [GeV/c^{2}];yeild", 200, 0., 2.) );
	histList->Add( new TH1D("fh_chi_prim_eta_minv","fh_chi_prim_eta_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.) );
	histList->Add( new TH1D("fh_ptcut_eta_minv","fh_ptcut_eta_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.) );
	histList->Add( new TH1D("fh_anglecut_eta_minv","fh_anglecut_eta_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.) );
	histList->Add( new TH1D("fh_gammacut_eta_minv","fh_gammacut_eta_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.) );
	histList->Add( new TH1D("fh_ttcut_eta_minv","fh_ttcut_eta_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.) );
	histList->Add( new TH1D("fh_stcut_eta_minv","fh_stcut_eta_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.) );
	histList->Add( new TH1D("fh_apcut_eta_minv","fh_apcut_eta_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.) );

//pty distribution of the signal
	histList->Add( new TH2D("fh_mc_signal_pty","fh_mc_signal_pty;Rapidity;p_{t} [GeV/c]", 40, 0., 4., 20, 0., 2.) );
	histList->Add( new TH2D("fh_acc_signal_pty","fh_acc_signal_pty;Rapidity;p_{t} [GeV/c]", 40, 0., 4., 20, 0., 2.) );
	histList->Add( new TH2D("fh_reco_signal_pty","fh_reco_signal_pty;Rapidity;p_{t} [GeV/c]", 40, 0., 4., 20, 0., 2.) );
	histList->Add( new TH2D("fh_rich_id_signal_pty","fh_rich_id_signal_pty;Rapidity;p_{t} [GeV/c]", 40, 0., 4., 20, 0., 2.) );
	histList->Add( new TH2D("fh_trd_id_signal_pty","fh_trd_id_signal_pty;Rapidity;p_{t} [GeV/c]", 40, 0., 4., 20, 0., 2.) );
	histList->Add( new TH2D("fh_tof_id_signal_pty","fh_tof_id_signal_pty;Rapidity;p_{t} [GeV/c]", 40, 0., 4., 20, 0., 2.) );
	histList->Add( new TH2D("fh_chi_prim_signal_pty","fh_chi_prim_signal_pty;Rapidity;p_{t} [GeV/c]", 40, 0., 4., 20, 0., 2.) );
	histList->Add( new TH2D("fh_ptcut_signal_pty","fh_ptcut_signal_pty;Rapidity;p_{t} [GeV/c]", 40, 0., 4., 20, 0., 2.) );
	histList->Add( new TH2D("fh_anglecut_signal_pty","fh_anglecut_signal_pty;Rapidity;p_{t} [GeV/c]", 40, 0., 4., 20, 0., 2.) );
	histList->Add( new TH2D("fh_gammacut_signal_pty","fh_gammacut_signal_pty;Rapidity;p_{t} [GeV/c]", 40, 0., 4., 20, 0., 2.) );
	histList->Add( new TH2D("fh_ttcut_signal_pty","fh_ttcut_signal_pty;Rapidity;p_{t} [GeV/c]", 40, 0., 4., 20, 0., 2.) );
	histList->Add( new TH2D("fh_stcut_signal_pty","fh_stcut_signal_pty;Rapidity;p_{t} [GeV/c]", 40, 0., 4., 20, 0., 2.) );
	histList->Add( new TH2D("fh_apcut_signal_pty","fh_apcut_signal_pty;Rapidity;p_{t} [GeV/c]",40, 0., 4., 20, 0., 2.) );

// cut distribution
	histList->Add( new TH1D("fh_angle_signal","fh_angle_signal;opening angle [deg];yeild", 360, 0., 180.) );
	histList->Add( new TH1D("fh_angle_bg","fh_angle_bg;opening angle [deg];yeild", 360, 0., 180.) );
	histList->Add( new TH1D("fh_position_signal","fh_position_signal", 100, 0., 0.02) );
	histList->Add( new TH1D("fh_position_bg","fh_position_bg",100, 0., 0.02) );
	histList->Add( new TH1D("fh_pt_signal","fh_pt_signal;p_{t} [GeV/c];yeild",200, 0., 2.) );
	histList->Add( new TH1D("fh_pt_bg","fh_pt_bg;p_{t} [GeV/c];yeild",200, 0., 2.) );
	histList->Add( new TH1D("fh_mom_signal","fh_mom_signal;p [GeV/c];yeild",200, 0., 10.) );
	histList->Add( new TH1D("fh_mom_bg","fh_mom_bg;p [GeV/c];yeild",200, 0., 10.) );
	histList->Add( new TH1D("fh_chi2_prim_signal", "fh_chi2_prim_signal;chi2,yeild", 200, 0., 20.) );
	histList->Add( new TH1D("fh_chi2_prim_bg","fh_chi2_prim_bg;chi2,yeild", 200, 0., 20.) );
	histList->Add( new TH2D("fh_ttcut_signal", "fh_ttcut_signal;#sqrt{p_{e^{#pm}} p_{rec}} [GeV/c];#theta_{e^{#pm},rec} [deg]", 100, 0., 5., 100, 0., 5.) );
	histList->Add( new TH2D("fh_stcut_signal", "fh_stcut_signal;#sqrt{p_{e^{#pm}} p_{rec}} [GeV/c];#theta_{e^{#pm},rec} [deg]", 100, 0., 5., 100, 0., 5.) );
	histList->Add( new TH2D("fh_ttcut_bg","fh_ttcut_bg;#sqrt{p_{e^{#pm}} p_{rec}} [GeV/c];#theta_{e^{#pm},rec} [deg]", 100, 0., 5., 100, 0., 5.) );
	histList->Add( new TH2D("fh_stcut_bg","fh_stcut_bg;#sqrt{p_{e^{#pm}} p_{rec}} [GeV/c];#theta_{e^{#pm},rec} [deg]", 100, 0., 5., 100, 0., 5.) );
	histList->Add( new TH2D("fh_apcut_signal", "fh_apcut_signal;#alpha;p_{t} [GeV/c]", 100, -1., 1., 200, 0., 1.) );
	histList->Add( new TH2D("fh_apcut_bg","fh_apcut_bg;#alpha;p_{t} [GeV/c];", 100, -1., 1., 200, 0., 1.) );
	histList->Add( new TH2D("fh_ttcut_pi0","fh_ttcut_pi0;#sqrt{p_{e^{#pm}} p_{rec}} [GeV/c];#theta_{e^{#pm},rec} [deg]", 100, 0., 5., 100, 0., 5.) );
	histList->Add( new TH2D("fh_stcut_pi0","fh_stcut_pi0;#sqrt{p_{e^{#pm}} p_{rec}} [GeV/c];#theta_{e^{#pm},rec} [deg]", 100, 0., 5., 100, 0., 5.) );
	histList->Add( new TH2D("fh_apcut_pi0","fh_apcut_pi0;#alpha;p_{t} [GeV/c];", 100, -1., 1., 200, 0., 1.) );
	histList->Add( new TH2D("fh_ttcut_gamma","fh_ttcut_gamma;#sqrt{p_{e^{#pm}} p_{rec}} [GeV/c];#theta_{e^{#pm},rec} [deg]", 100, 0., 5., 100, 0., 5.) );
	histList->Add( new TH2D("fh_stcut_gamma","fh_stcut_gamma;#sqrt{p_{e^{#pm}} p_{rec}} [GeV/c];#theta_{e^{#pm},rec} [deg]", 100, 0., 5., 100, 0., 5.) );
	histList->Add( new TH2D("fh_apcut_gamma","fh_apcut_gamma;#alpha;p_{t} [GeV/c];", 100, -1., 1., 200, 0., 1.) );

//source of BG pairs
	histList->Add( new TH2D("fh_source_pair_reco","fh_source_pair_reco;mother particle e+;mother particle e-", 4, 0., 4., 4, 0., 4.) );
	histList->Add( new TH2D("fh_source_pair_rich_id","fh_source_pair_rich_id;mother particle e+;mother particle e-", 4, 0., 4., 4, 0., 4.) );
	histList->Add( new TH2D("fh_source_pair_trd_id","fh_source_pair_trd_id;mother particle e+;mother particle e-", 4, 0., 4., 4, 0., 4.) );
	histList->Add( new TH2D("fh_source_pair_tof_id","fh_source_pair_tof_id;mother particle e+;mother particle e-", 4, 0., 4., 4, 0., 4.) );
	histList->Add( new TH2D("fh_source_pair_chi_prim","fh_source_pair_chi_prim;mother particle e+;mother particle e-", 4, 0., 4., 4, 0., 4.) );
	histList->Add( new TH2D("fh_source_pair_ptcut","fh_source_pair_ptcut;mother particle e+;mother particle e-", 4, 0., 4., 4, 0., 4.) );
	histList->Add( new TH2D("fh_source_pair_anglecut","fh_source_pair_anglecut;mother particle e+;mother particle e-", 4, 0., 4., 4, 0., 4.) );
	histList->Add( new TH2D("fh_source_pair_gammacut","fh_source_pair_gammacut;mother particle e+;mother particle e-", 4, 0., 4., 4, 0., 4.) );
	histList->Add( new TH2D("fh_source_pair_ttcut","fh_source_pair_ttcut;mother particle e+;mother particle e-", 4, 0., 4., 4, 0., 4.) );
	histList->Add( new TH2D("fh_source_pair_stcut","fh_source_pair_stcut;mother particle e+;mother particle e-", 4, 0., 4., 4, 0., 4.) );
	histList->Add( new TH2D("fh_source_pair_apcut","fh_source_pair_apcut;mother particle e+;mother particle e-", 4, 0., 4., 4, 0., 4.) );

//ID cut distributions
	histList->Add( new TH1D("fh_rich_ann_signal", "fh_rich_ann_signal;ann output;yeild", 100, -1.1, 1.1) );
	histList->Add( new TH2D("fh_rich_trd_ann_signal","fh_rich_trd_ann_signal;rich_ann;trd_ann",100, -1.2, 1.2, 100, -1.2, 1.2) );
	histList->Add( new TH2D("fh_rich_trd_ann_bg","fh_rich_trd_ann_bg;rich_ann;trd_ann",100, -1.2, 1.2, 100, -1.2, 1.2) );
	histList->Add( new TH1D("fh_rich_ann_bg", "fh_rich_ann_bg;ann output;yeild", 100, -1.1, 1.1) );
	histList->Add( new TH1D("fh_trd_ann_signal", "fh_trd_ann_signal;ann output;yeild", 100, -1.1, 1.1) );
	histList->Add( new TH1D("fh_trd_ann_bg", "fh_trd_ann_bg;ann output;yeild", 100, -1.1, 1.1) );
	histList->Add( new TH2D("fh_tof_m2_signal","fh_tof_m2_signal;p [GeV/c]; m^{2} [GeV/c^{2}]^{2}", 100, 0., 4., 600, 0., 1.2) );
	histList->Add( new TH2D("fh_tof_m2_bg","fh_tof_m2_bg;p [GeV/c]; m^{2} [GeV/c^{2}]^{2}", 100, 0., 4., 600, 0., 1.2) );

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
