void glue_histo ()
{
    TString filePath = "/lustre/cbm/user/ebelolap/oct10/urqmd_rho0/25gev/100_field/real/mytask.analysis";//.0000.root";
    TString outFileName = filePath +".all.root";
    Int_t nEvents = 200;   // number of files to be added

    fh_mc_signal_pty = new TH2D("fh_mc_signal_pty","fh_mc_signal_pty;Rapidity;p_{t} [GeV/c]", 40, 0., 4., 20, 0., 2.);
    fh_acc_signal_pty = new TH2D("fh_acc_signal_pty","fh_acc_signal_pty;Rapidity;p_{t} [GeV/c]", 40, 0., 4., 20, 0., 2.);

    fh_mc_signal_mom = new TH1D("fh_mc_signal_mom", "fh_mc_signal_mom;momentum [GeV/c];yeild",100, 0., 15.);
    fh_mc_bg_mom = new TH1D("fh_mc_bg_mom", "fh_mc_bg_mom;momentum [GeV/c];yeild",100, 0., 15.);
    fh_acc_signal_mom = new TH1D("fh_acc_signal_mom","fh_acc_signal_mom;momentum [GeV/c];yeild", 100, 0., 15.);
    fh_reco_signal_mom = new TH1D("fh_reco_signal_mom","fh_reco_signal_mom;momentum [GeV/c];yeild", 100, 0., 15.);
    fh_rich_id_signal_mom = new TH1D("fh_rich_id_signal_mom","fh_rich_id_signal_mom;momentum [GeV/c];yeild", 100, 0., 15.);
    fh_trd_id_signal_mom = new TH1D("fh_trd_id_signal_mom","fh_trd_id_signal_mom;momentum [GeV/c];yeild", 100, 0., 15.);
    fh_tof_id_signal_mom = new TH1D("fh_tof_id_signal_mom","fh_tof_id_signal_mom;momentum [GeV/c];yeild", 100, 0., 15.);
    fh_chi_prim_signal_mom = new TH1D("fh_chi_prim_signal_mom","fh_chi_prim_signal_mom;momentum [GeV/c];yeild", 100, 0., 15.);
    fh_ptcut_signal_mom = new TH1D("fh_ptcut_signal_mom","fh_ptcut_signal_mom;momentum [GeV/c];yeild", 100, 0., 15.);
    fh_anglecut_signal_mom = new TH1D("fh_anglecut_signal_mom","fh_anglecut_signal_mom;momentum [GeV/c];yeild", 100, 0., 15.);
    fh_pi0cut_signal_mom = new TH1D("fh_pi0cut_signal_mom","fh_pi0cut_signal_mom;momentum [GeV/c];yeild", 100, 0., 15.);
    fh_ttcut_signal_mom = new TH1D("fh_ttcut_signal_mom","fh_ttcut_signal_mom;momentum [GeV/c];yeild", 100, 0., 15.);

    fh_mc_signal_minv = new TH1D("fh_mc_signal_minv","fh_mc_signal_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.);
    fh_mc_bg_minv = new TH1D("fh_mc_bg_minv","fh_mc_bg_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.);
    fh_mc_pi0_minv = new TH1D("fh_mc_pi0_minv","fh_mc_pi0_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.);
    fh_mc_eta_minv = new TH1D("fh_mc_eta_minv","fh_mc_eta_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.);
    fh_mc_gamma_minv = new TH1D("fh_mc_gamma_minv","fh_mc_gamma_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.);
    fh_acc_signal_minv = new TH1D("fh_acc_signal_minv","fh_acc_signal_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.);

    fh_mc_mother_pdg = new TH1D("fh_mc_mother_pdg", "fh_mc_mother_pdg; Pdg code; Particles per event", 7000, -3500., 3500.);
    fh_acc_mother_pdg = new TH1D("fh_acc_mother_pdg","fh_acc_mother_pdg; Pdg code; Particles per event", 7000, -3500., 3500.);
    
    fh_mc_vertex_gamma_xz = new TH2D("fh_mc_vertex_gamma_xz","fh_mc_vertex_gamma_xz;Z [cm];X [cm]",200, -10., 110., 200, -100.,100.);
    fh_mc_vertex_gamma_yz = new TH2D("fh_mc_vertex_gamma_yz", "fh_mc_vertex_gamma_yz;Z [cm];Y [cm]", 200, -10., 110., 200, -100., 100.);
    fh_mc_vertex_gamma_xy = new TH2D("fh_mc_vertex_gamma_xy","fh_mc_vertex_gamma_xy;X [cm];Y [cm]", 200, -100.,100., 200, -100., 100.); 

    fh_rec_mc_mom_signal = new TH1D("fh_rec_mc_mom_signal","fh_rec_mc_mom_signal;#Delta p/p [%];yeild",100, -10., 10.);
    fh_mom_res_vs_mom_signal = new TH2D("fh_mom_res_vs_mom_signal", "fh_mom_res_vs_mom_signal;p [GeV/c];#Delta p/p [%]",100, 0., 15., 100, -10., 10.);
    fh_mean_mom_vs_mom_signal = new TH1D("fh_mean_mom_vs_mom_signal","fh_mean_mom_vs_mom_signal",100, 0., 15.);

    fh_rec_signal_minv = new TH1D("fh_rec_signal_minv","fh_rec_signal_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.);
    fh_rich_id_signal_minv = new TH1D("fh_rich_id_signal_minv","fh_rich_id_signal_minv;M_{ee} [GeV/c^{2}];yeild", 200, 0., 2.);
    fh_trd_id_signal_minv = new TH1D("fh_trd_id_signal_minv", "fh_trd_id_signal_minv;M_{ee} [GeV/c^{2}];yeild", 200, 0., 2.);
    fh_tof_id_signal_minv = new TH1D("fh_tof_id_signal_minv","fh_tof_id_signal_minv;M_{ee} [GeV/c^{2}];yeild", 200, 0., 2.);
    fh_chi_prim_signal_minv = new TH1D("fh_chi_prim_signal_minv","fh_chi_prim_signal_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.);    
    fh_ptcut_signal_minv = new TH1D("fh_ptcut_signal_minv","fh_ptcut_signal_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.);
    fh_anglecut_signal_minv = new TH1D("fh_anglecut_signal_minv","fh_anglecut_signal_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.);
    fh_pi0cut_signal_minv = new TH1D("fh_pi0cut_signal_minv","fh_pi0cut_signal_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.);
    fh_ttcut_signal_minv = new TH1D("fh_ttcut_signal_minv","fh_ttcut_signal_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.);

    fh_rec_bg_minv = new TH1D("fh_rec_bg_minv","fh_rec_bg_minv;M_{ee} [GeV/c^{2}];yeild", 200, 0., 2.);
    fh_rich_id_bg_minv = new TH1D("fh_rich_id_bg_minv","fh_rich_id_bg_minv;M_{ee} [GeV/c^{2}];yeild", 200, 0., 2.);
    fh_trd_id_bg_minv = new TH1D("fh_trd_id_bg_minv","fh_trd_id_bg_minv;M_{ee} [GeV/c^{2}];yeild", 200, 0., 2.);
    fh_tof_id_bg_minv = new TH1D("fh_tof_id_bg_minv","fh_tof_id_bg_minv;M_{ee} [GeV/c^{2}];yeild", 200, 0., 2.);
    fh_chi_prim_bg_minv = new TH1D("fh_chi_prim_bg_minv","fh_chi_prim_bg_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.);    
    fh_ptcut_bg_minv = new TH1D("fh_ptcut_bg_minv","fh_ptcut_bg_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.);
    fh_anglecut_bg_minv = new TH1D("fh_anglecut_bg_minv","fh_anglecut_bg_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.);
    fh_pi0cut_bg_minv = new TH1D("fh_pi0cut_bg_minv","fh_pi0cut_bg_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.);
    fh_ttcut_bg_minv = new TH1D("fh_ttcut_bg_minv","fh_ttcut_bg_minv;M_{ee} [GeV/c^{2}];yeild",200, 0., 2.);

    fh_reco_signal_pty = new TH2D("fh_reco_signal_pty","fh_reco_signal_pty;Rapidity;p_{t} [GeV/c]", 40, 0., 4., 20, 0., 2.);
    fh_rich_id_signal_pty = new TH2D("fh_rich_id_signal_pty","fh_rich_id_signal_pty;Rapidity;p_{t} [GeV/c]", 40, 0., 4., 20, 0., 2.);
    fh_trd_id_signal_pty = new TH2D("fh_trd_id_signal_pty","fh_trd_id_signal_pty;Rapidity;p_{t} [GeV/c]", 40, 0., 4., 20, 0., 2.);
    fh_tof_id_signal_pty = new TH2D("fh_tof_id_signal_pty","fh_tof_id_signal_pty;Rapidity;p_{t} [GeV/c]", 40, 0., 4., 20, 0., 2.);
    fh_chi_prim_signal_pty = new TH2D("fh_chi_prim_signal_pty","fh_chi_prim_signal_pty;Rapidity;p_{t} [GeV/c]", 40, 0., 4., 20, 0., 2.);    
    fh_ptcut_signal_pty = new TH2D("fh_ptcut_signal_pty","fh_ptcut_signal_pty;Rapidity;p_{t} [GeV/c]", 40, 0., 4., 20, 0., 2.);
    fh_anglecut_signal_pty = new TH2D("fh_anglecut_signal_pty","fh_anglecut_signal_pty;Rapidity;p_{t} [GeV/c]", 40, 0., 4., 20, 0., 2.);
    fh_pi0cut_signal_pty = new TH2D("fh_pi0cut_signal_pty","fh_pi0cut_signal_pty;Rapidity;p_{t} [GeV/c]", 40, 0., 4., 20, 0., 2.);
    fh_ttcut_signal_pty = new TH2D("fh_ttcut_signal_pty","fh_ttcut_signal_pty;Rapidity;p_{t} [GeV/c]", 40, 0., 4., 20, 0., 2.);

// cut distribution
    fh_angle_signal = new TH1D("fh_angle_signal","fh_angle_signal;opening angle [deg];yeild", 360, 0., 180.);
    fh_angle_bg = new TH1D("fh_angle_bg","fh_angle_bg;opening angle [deg];yeild", 360, 0., 180.); 
    fh_position_signal = new TH1D("fh_position_signal","fh_position_signal", 100, 0., 0.02);
    fh_position_bg = new TH1D("fh_position_bg","fh_position_bg",100, 0., 0.02);
    fh_pt_signal = new TH1D("fh_pt_signal","fh_pt_signal;p_{t} [GeV/c];yeild",200, 0., 2.);
    fh_pt_bg = new TH1D("fh_pt_bg","fh_pt_bg;p_{t} [GeV/c];yeild",200, 0., 2.);
    fh_mom_signal = new TH1D("fh_mom_signal","fh_mom_signal;p [GeV/c];yeild",200, 0., 10.);
    fh_mom_bg = new TH1D("fh_mom_bg","fh_mom_bg;p [GeV/c];yeild",200, 0., 10.);
    fh_chi2_prim_signal = new TH1D("fh_chi2_prim_signal", "fh_chi2_prim_signal;chi2,yeild", 200, 0., 20.);
    fh_chi2_prim_bg = new TH1D("fh_chi2_prim_bg","fh_chi2_prim_bg;chi2,yeild", 200, 0., 20.);
    fh_ttcut_signal = new TH2D("fh_ttcut_signal", "fh_ttcut_signal;#sqrt{p_{e^{#pm}} p_{rec}} [GeV/c];#theta_{e^{#pm},rec} [deg]", 100, 0., 5., 100, 0., 5.);
    fh_ttcut_bg = new TH2D("fh_ttcut_bg","fh_ttcut_bg;#sqrt{p_{e^{#pm}} p_{rec}} [GeV/c];#theta_{e^{#pm},rec} [deg]", 100, 0., 5., 100, 0., 5.);
    fh_apcut_signal = new TH2D("fh_apcut_signal", "fh_apcut_signal;#alpha;p_{t} [GeV/c]", 100, -1., 1., 100, 0., 2.);
    fh_apcut_bg = new TH2D("fh_apcut_bg","fh_apcut_bg;#alpha;p_{t} [GeV/c]", 100, -1., 1., 100, 0., 2.);

 //ID cut distributions
    fh_rich_ann_signal = new TH1D("fh_rich_ann_signal", "fh_rich_ann_signal;ann output;yeild", 100, -1.1, 1.1);
    fh_rich_ann_bg = new TH1D("fh_rich_ann_bg", "fh_rich_ann_bg;ann output;yeild", 100, -1.1, 1.1);
    fh_trd_ann_signal = new TH1D("fh_trd_ann_signal", "fh_trd_ann_signal;ann output;yeild", 100, -1.1, 1.1);
    fh_trd_ann_bg = new TH1D("fh_trd_ann_bg", "fh_trd_ann_bg;ann output;yeild", 100, -1.1, 1.1);
    fh_tof_m2_signal = new TH2D("fh_tof_m2_signal","fh_tof_m2_signal;p [GeV/c]; m^{2} [GeV/c^{2}]^{2}", 100, 0., 4., 600, 0., 1.2);
    fh_tof_m2_bg = new TH2D("fh_tof_m2_bg","fh_tof_m2_bg;p [GeV/c]; m^{2} [GeV/c^{2}]^{2}", 100, 0., 4., 600, 0., 1.2);


    Int_t nofGoodFiles = 0;
    for(Int_t i = 0; i < nEvents; i++) {
        char str[4];
        sprintf(str, "%04d", i);
        TString fileName = filePath + "." + TString(str) + ".root";
        cout << "-I- " + fileName << endl;
        TFile* file = new TFile(fileName);
        if (file->IsZombie()) continue;

        fh_mc_signal_pty->Add((TH2D*) file->Get("fh_mc_signal_pty"));
        fh_acc_signal_pty->Add((TH2D*) file->Get("fh_acc_signal_pty"));

        fh_mc_signal_mom->Add((TH1D*) file->Get("fh_mc_signal_mom"));
        fh_mc_bg_mom->Add((TH1D*) file->Get("fh_mc_bg_mom"));
        fh_acc_signal_mom->Add((TH1D*) file->Get("fh_acc_signal_mom")); 
        fh_reco_signal_mom->Add((TH1D*) file->Get("fh_reco_signal_mom"));
        fh_rich_id_signal_mom->Add((TH1D*) file->Get("fh_rich_id_signal_mom"));
        fh_trd_id_signal_mom->Add((TH1D*) file->Get("fh_trd_id_signal_mom"));
        fh_tof_id_signal_mom->Add((TH1D*) file->Get("fh_tof_id_signal_mom"));
        fh_chi_prim_signal_mom->Add((TH1D*) file->Get("fh_chi_prim_signal_mom"));
        fh_ptcut_signal_mom->Add((TH1D*) file->Get("fh_ptcut_signal_mom"));
        fh_anglecut_signal_mom->Add((TH1D*) file->Get("fh_anglecut_signal_mom"));
        fh_pi0cut_signal_mom->Add((TH1D*) file->Get("fh_pi0cut_signal_mom"));
        fh_ttcut_signal_mom->Add((TH1D*) file->Get("fh_ttcut_signal_mom"));


        fh_mc_signal_minv->Add((TH1D*) file->Get("fh_mc_signal_minv"));
        fh_mc_bg_minv->Add((TH1D*) file->Get("fh_mc_bg_minv")); 
        fh_mc_pi0_minv->Add((TH1D*) file->Get("fh_mc_pi0_minv"));
        fh_mc_eta_minv->Add((TH1D*) file->Get("fh_mc_eta_minv"));
        fh_mc_gamma_minv->Add((TH1D*) file->Get("fh_mc_gamma_minv"));
        fh_acc_signal_minv->Add((TH1D*) file->Get("fh_acc_signal_minv")); 

        fh_mc_mother_pdg->Add((TH1D*) file->Get("fh_mc_mother_pdg"));  
        fh_acc_mother_pdg->Add((TH1D*) file->Get("fh_acc_mother_pdg"));

        fh_mc_vertex_gamma_xz->Add((TH2D*) file->Get("fh_mc_vertex_gamma_xz")); 
        fh_mc_vertex_gamma_yz->Add((TH2D*) file->Get("fh_mc_vertex_gamma_yz"));
        fh_mc_vertex_gamma_xy->Add((TH2D*) file->Get("fh_mc_vertex_gamma_xy")); 

        fh_rec_mc_mom_signal->Add((TH1D*) file->Get("fh_rec_mc_mom_signal")); 
        fh_mom_res_vs_mom_signal->Add((TH1D*) file->Get("fh_mom_res_vs_mom_signal")); 
        fh_mean_mom_vs_mom_signal->Add((TH1D*) file->Get("fh_mean_mom_vs_mom_signal"));


        fh_rec_signal_minv->Add((TH1D*) file->Get("fh_rec_signal_minv")); 
        fh_rich_id_signal_minv->Add((TH1D*) file->Get("fh_rich_id_signal_minv")); 
        fh_trd_id_signal_minv->Add((TH1D*) file->Get("fh_trd_id_signal_minv")); 
        fh_tof_id_signal_minv->Add((TH1D*) file->Get("fh_tof_id_signal_minv"));
        fh_chi_prim_signal_minv->Add((TH1D*) file->Get("fh_chi_prim_signal_minv")); 
        fh_ptcut_signal_minv->Add((TH1D*) file->Get("fh_ptcut_signal_minv")); 
        fh_anglecut_signal_minv->Add((TH1D*) file->Get("fh_anglecut_signal_minv"));
        fh_pi0cut_signal_minv->Add((TH1D*) file->Get("fh_pi0cut_signal_minv"));
        fh_ttcut_signal_minv->Add((TH1D*) file->Get("fh_ttcut_signal_minv"));

        fh_rec_bg_minv->Add((TH1D*) file->Get("fh_rec_bg_minv")); 
        fh_rich_id_bg_minv->Add((TH1D*) file->Get("fh_rich_id_bg_minv")); 
        fh_trd_id_bg_minv->Add((TH1D*) file->Get("fh_trd_id_bg_minv")); 
        fh_tof_id_bg_minv->Add((TH1D*) file->Get("fh_tof_id_bg_minv"));
        fh_chi_prim_bg_minv->Add((TH1D*) file->Get("fh_chi_prim_bg_minv"));
        fh_ptcut_bg_minv->Add((TH1D*) file->Get("fh_ptcut_bg_minv"));
        fh_anglecut_bg_minv->Add((TH1D*) file->Get("fh_anglecut_bg_minv"));
        fh_pi0cut_bg_minv->Add((TH1D*) file->Get("fh_pi0cut_bg_minv"));
        fh_ttcut_bg_minv->Add((TH1D*) file->Get("fh_ttcut_bg_minv"));

// cuts distribution 
        fh_chi2_prim_signal->Add((TH1D*) file->Get("fh_chi2_prim_signal")); 
        fh_chi2_prim_bg->Add((TH1D*) file->Get("fh_chi2_prim_bg")); 
        fh_pt_signal->Add((TH1D*) file->Get("fh_pt_signal")); 
        fh_pt_bg->Add((TH1D*) file->Get("fh_pt_bg"));
        fh_position_signal->Add((TH1D*) file->Get("fh_position_signal")); 
        fh_position_bg->Add((TH1D*) file->Get("fh_position_bg")); 
        fh_angle_signal->Add((TH1D*) file->Get("fh_angle_signal")); 
        fh_angle_bg->Add((TH1D*) file->Get("fh_angle_bg"));
        fh_mom_signal->Add((TH1D*) file->Get("fh_mom_signal")); 
        fh_mom_bg->Add((TH1D*) file->Get("fh_mom_bg"));
        fh_ttcut_signal->Add((TH1D*) file->Get("fh_ttcut_signal")); 
        fh_ttcut_bg->Add((TH1D*) file->Get("fh_ttcut_bg"));
        fh_apcut_signal->Add((TH1D*) file->Get("fh_apcut_signal")); 
        fh_apcut_bg->Add((TH1D*) file->Get("fh_apcut_bg"));

        fh_reco_signal_pty->Add((TH2D*) file->Get("fh_reco_signal_pty"));
        fh_rich_id_signal_pty->Add((TH2D*) file->Get("fh_rich_id_signal_pty"));
        fh_trd_id_signal_pty->Add((TH2D*) file->Get("fh_trd_id_signal_pty")); 
        fh_tof_id_signal_pty->Add((TH2D*) file->Get("fh_tof_id_signal_pty"));
        fh_chi_prim_signal_pty->Add((TH2D*) file->Get("fh_chi_prim_signal_pty")); 
        fh_ptcut_signal_pty->Add((TH2D*) file->Get("fh_ptcut_signal_pty")); 
        fh_anglecut_signal_pty->Add((TH2D*) file->Get("fh_anglecut_signal_pty"));
        fh_pi0cut_signal_pty->Add((TH2D*) file->Get("fh_pi0cut_signal_pty"));
        fh_ttcut_signal_pty->Add((TH2D*) file->Get("fh_ttcut_signal_pty"));

 //ID cut distributions
        fh_rich_ann_signal->Add((TH1D*) file->Get("fh_rich_ann_signal"));
        fh_rich_ann_bg->Add((TH1D*) file->Get("fh_rich_ann_bg"));
        fh_trd_ann_signal->Add((TH1D*) file->Get("fh_trd_ann_signal"));
        fh_trd_ann_bg->Add((TH1D*) file->Get("fh_trd_ann_bg"));
        fh_tof_m2_signal->Add((TH2D*) file->Get("fh_tof_m2_signal"));
        fh_tof_m2_bg->Add((TH2D*) file->Get("fh_tof_m2_bg"));

        nofGoodFiles++;
        file->Close();
    }

    Double_t scale = 1./(Double_t) nofGoodFiles;

    fh_mc_signal_pty->Scale(scale);
    fh_acc_signal_pty->Scale(scale);

    fh_mc_signal_mom->Scale(scale);
    fh_mc_bg_mom->Scale(scale);
    fh_acc_signal_mom->Scale(scale);
    fh_reco_signal_mom->Scale(scale);
    fh_rich_id_signal_mom->Scale(scale);
    fh_trd_id_signal_mom->Scale(scale);
    fh_tof_id_signal_mom->Scale(scale);
    fh_chi_prim_signal_mom->Scale(scale);
    fh_ptcut_signal_mom->Scale(scale);
    fh_anglecut_signal_mom->Scale(scale);
    fh_pi0cut_signal_mom->Scale(scale);
    fh_ttcut_signal_mom->Scale(scale);


    fh_mc_signal_minv->Scale(scale);
    fh_mc_bg_minv->Scale(scale);
    fh_mc_pi0_minv->Scale(scale);
    fh_mc_eta_minv->Scale(scale);
    fh_mc_gamma_minv->Scale(scale);
    fh_acc_signal_minv->Scale(scale);

    fh_mc_mother_pdg->Scale(scale);
    fh_acc_mother_pdg->Scale(scale);

    fh_mc_vertex_gamma_xz->Scale(scale);
    fh_mc_vertex_gamma_yz->Scale(scale);
    fh_mc_vertex_gamma_xy->Scale(scale);

    fh_rec_mc_mom_signal->Scale(scale);
    fh_mom_res_vs_mom_signal->Scale(scale);
    fh_mean_mom_vs_mom_signal->Scale(scale);

    fh_rec_signal_minv->Scale(scale);
    fh_rich_id_signal_minv->Scale(scale);
    fh_trd_id_signal_minv->Scale(scale);
    fh_tof_id_signal_minv->Scale(scale);
    fh_chi_prim_signal_minv->Scale(scale);
    fh_ptcut_signal_minv->Scale(scale);
    fh_anglecut_signal_minv->Scale(scale);
    fh_pi0cut_signal_minv->Scale(scale);
    fh_ttcut_signal_minv->Scale(scale);

    fh_rec_bg_minv->Scale(scale);
    fh_rich_id_bg_minv->Scale(scale);
    fh_trd_id_bg_minv->Scale(scale);
    fh_tof_id_bg_minv->Scale(scale);
    fh_chi_prim_bg_minv->Scale(scale);
    fh_ptcut_bg_minv->Scale(scale);
    fh_anglecut_bg_minv->Scale(scale);
    fh_pi0cut_bg_minv->Scale(scale);
    fh_ttcut_bg_minv->Scale(scale);

//cuts distribution
    fh_chi2_prim_signal->Scale(scale);
    fh_chi2_prim_bg->Scale(scale);
    fh_pt_signal->Scale(scale);
    fh_pt_bg->Scale(scale);
    fh_position_signal->Scale(scale);
    fh_position_bg->Scale(scale);
    fh_angle_signal->Scale(scale);
    fh_angle_bg->Scale(scale);
    fh_mom_signal->Scale(scale);
    fh_mom_bg->Scale(scale);
    fh_ttcut_signal->Scale(scale);
    fh_ttcut_bg->Scale(scale);
    fh_apcut_signal->Scale(scale);
    fh_apcut_bg->Scale(scale);

    fh_reco_signal_pty->Scale(scale);
    fh_rich_id_signal_pty->Scale(scale);
    fh_trd_id_signal_pty->Scale(scale);
    fh_tof_id_signal_pty->Scale(scale);
    fh_chi_prim_signal_pty->Scale(scale);
    fh_ptcut_signal_pty->Scale(scale);
    fh_anglecut_signal_pty->Scale(scale);
    fh_pi0cut_signal_pty->Scale(scale);
    fh_ttcut_signal_pty->Scale(scale);

 //ID cuts
    fh_rich_ann_signal->Scale(scale);
    fh_rich_ann_bg->Scale(scale);
    fh_trd_ann_signal->Scale(scale);
    fh_trd_ann_bg->Scale(scale);
    fh_tof_m2_signal->Scale(scale);
    fh_tof_m2_bg->Scale(scale);

///WRITE HISTOGRAMMS TO OUTPUT FILE
    TFile* outFile = new TFile(outFileName, "RECREATE");
    fh_mc_signal_pty->Write();
    fh_acc_signal_pty->Write();

    fh_mc_signal_mom->Write();
    fh_mc_bg_mom->Write();
    fh_acc_signal_mom->Write();
    fh_reco_signal_mom->Write();
    fh_rich_id_signal_mom->Write();
    fh_trd_id_signal_mom->Write();
    fh_tof_id_signal_mom->Write();
    fh_chi_prim_signal_mom->Write();
    fh_ptcut_signal_mom->Write();
    fh_anglecut_signal_mom->Write();
    fh_pi0cut_signal_mom->Write();
    fh_ttcut_signal_mom->Write();

    fh_mc_signal_minv->Write();
    fh_mc_bg_minv->Write();
    fh_mc_pi0_minv->Write();
    fh_mc_eta_minv->Write();
    fh_mc_gamma_minv->Write();
    fh_acc_signal_minv->Write();

    fh_mc_mother_pdg->Write();
    fh_acc_mother_pdg->Write();

    fh_mc_vertex_gamma_xz->Write();
    fh_mc_vertex_gamma_yz->Write();
    fh_mc_vertex_gamma_xy->Write();

    fh_rec_mc_mom_signal->Write();
    fh_mom_res_vs_mom_signal->Write();
    fh_mean_mom_vs_mom_signal->Write();

    fh_rec_signal_minv->Write();
    fh_rich_id_signal_minv->Write();
    fh_trd_id_signal_minv->Write();
    fh_tof_id_signal_minv->Write();
    fh_chi_prim_signal_minv->Write();
    fh_ptcut_signal_minv->Write();
    fh_anglecut_signal_minv->Write();
    fh_pi0cut_signal_minv->Write();
    fh_ttcut_signal_minv->Write();

    fh_rec_bg_minv->Write();
    fh_rich_id_bg_minv->Write();
    fh_trd_id_bg_minv->Write();
    fh_tof_id_bg_minv->Write();
    fh_chi_prim_bg_minv->Write();
    fh_ptcut_bg_minv->Write();
    fh_anglecut_bg_minv->Write();
    fh_pi0cut_bg_minv->Write();
    fh_ttcut_bg_minv->Write();

//cuts distribution
    fh_pt_signal->Write();
    fh_pt_bg->Write();
    fh_chi2_prim_signal->Write();
    fh_chi2_prim_bg->Write();
    fh_position_signal->Write();
    fh_position_bg->Write();
    fh_angle_signal->Write();
    fh_angle_bg->Write();
    fh_mom_signal->Write();
    fh_mom_bg->Write();
    fh_ttcut_signal->Write();
    fh_ttcut_bg->Write();
    fh_apcut_signal->Write();
    fh_apcut_bg->Write();

    fh_reco_signal_pty->Write();
    fh_rich_id_signal_pty->Write();
    fh_trd_id_signal_pty->Write();
    fh_tof_id_signal_pty->Write();
    fh_chi_prim_signal_pty->Write();
    fh_ptcut_signal_pty->Write();
    fh_anglecut_signal_pty->Write();
    fh_pi0cut_signal_pty->Write();
    fh_ttcut_signal_pty->Write();

 //ID cuts
    fh_rich_ann_signal->Write();
    fh_rich_ann_bg->Write();
    fh_trd_ann_signal->Write();
    fh_trd_ann_bg->Write();
    fh_tof_m2_signal->Write();
    fh_tof_m2_bg->Write();


    cout <<"nofGoodFiles = " << nofGoodFiles << endl;

} 
