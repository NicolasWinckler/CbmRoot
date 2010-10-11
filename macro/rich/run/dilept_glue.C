{
    // chi2 primary ------------------------------------------------------------
    fh_chi2_prim_all = new TH1D("h_chi2_prim_all", "", 100, 0., 10.);
    fh_chi2_prim_e   = new TH1D("h_chi2_prim_e",   "", 100, 0., 10.);

    // radial vs z -------------------------------------------------------------
    fh_mc_all_zr = new TH2D("h_mc_all_zr", "", 400, 0., 400., 400, -200., 200.);

    // single track momentum ---------------------------------------------------
    fh_mc_all_lept_mom  = new TH1D("h_mc_all_lept_mom", "", 1000, 0., 10.);
    fh_mc_rec_lept_mom  = new TH1D("h_mc_rec_lept_mom", "", 1000, 0., 10.);
    fh_rich_id_lept_mom = new TH1D("h_rich_id_lept_mom","", 1000, 0., 10.);
    fh_rich_id_ep_mom   = new TH1D("h_rich_id_ep_mom","", 1000, 0., 10.);
    fh_rich_id_em_mom   = new TH1D("h_rich_id_em_mom","", 1000, 0., 10.);
    fh_rich_id_no_cut_mom = new TH1D("h_rich_id_no_cut_mom","", 1000, 0., 10.);

    fh_rich_id_mom_cut_mom = new TH1D("h_rich_id_mom_cut_mom","", 1000, 0., 10.);
  //  fh_rich_id_dist_cut_mom = new TH1D("h_rich_id_dist_cut_mom","", 1000, 0., 10.);
    fh_rich_id_ann_cut_mom = new TH1D("h_rich_id_ann_cut_mom","", 1000, 0., 10.);
  //  fh_rich_id_xy_cut_mom = new TH1D("h_rich_id_xy_cut_mom","", 1000, 0., 10.);
  //  fh_rich_id_axisAB_cut_mom = new TH1D("h_rich_id_axisAB_cut_mom","", 1000, 0., 10.);

    // pair momentum -----------------------------------------------------------
    fh_mc_all_mom = new TH1D("h_mc_all_mom", "", 1000, 0., 10.);
    fh_mc_rec_mom = new TH1D("h_mc_rec_mom", "", 1000, 0., 10.);
    fh_rich_id_mom= new TH1D("h_rich_id_mom","", 1000, 0., 10.);
    // omega
    fh_mc_all_omega_mom  = new TH1D("h_mc_all_omega_mom", "", 1000, 0., 10.);
    fh_mc_rec_omega_mom  = new TH1D("h_mc_rec_omega_mom", "", 1000, 0., 10.);
    fh_rich_id_omega_mom = new TH1D("h_rich_id_omega_mom","", 1000, 0., 10.);
    // eta
    fh_mc_all_eta_mom  = new TH1D("h_mc_all_eta_mom", "", 1000, 0., 10.);
    fh_mc_rec_eta_mom  = new TH1D("h_mc_rec_eta_mom", "", 1000, 0., 10.);
    fh_rich_id_eta_mom = new TH1D("h_rich_id_eta_mom","", 1000, 0., 10.);
    // pi0
    fh_mc_all_pi0_mom  = new TH1D("h_mc_all_pi0_mom", "", 1000, 0., 10.);
    fh_mc_rec_pi0_mom  = new TH1D("h_mc_rec_pi0_mom", "", 1000, 0., 10.);
    fh_rich_id_pi0_mom = new TH1D("h_rich_id_pi0_mom","", 1000, 0., 10.);
    // gamma
    fh_mc_all_gamma_mom  = new TH1D("h_mc_all_gamma_mom", "", 1000, 0., 10.);
    fh_mc_rec_gamma_mom  = new TH1D("h_mc_rec_gamma_mom", "", 1000, 0., 10.);
    fh_rich_id_gamma_mom = new TH1D("h_rich_id_gamma_mom","", 1000, 0., 10.);

    // opening angle ------------------------------------------------------------
    fh_mc_all_theta = new TH1D("h_mc_all_theta", "", 1800, 0., 180.);
    fh_mc_rec_theta = new TH1D("h_mc_rec_theta", "", 1800, 0., 180.);
    fh_rich_id_theta= new TH1D("h_rich_id_theta","", 1800, 0., 180.);
    // omega
    fh_mc_all_omega_theta  = new TH1D("h_mc_all_omega_theta",  "", 1800, 0., 180.);
    fh_mc_rec_omega_theta  = new TH1D("h_mc_rec_omega_theta",  "", 1800, 0., 180.);
    fh_rich_id_omega_theta = new TH1D("h_rich_id_omega_theta", "", 1800, 0., 180.);
    // eta
    fh_mc_all_eta_theta  = new TH1D("h_mc_all_eta_theta", "", 1800, 0., 180.);
    fh_mc_rec_eta_theta  = new TH1D("h_mc_rec_eta_theta", "", 1800, 0., 180.);
    fh_rich_id_eta_theta = new TH1D("h_rich_id_eta_theta","", 1800, 0., 180.);
    // pi0
    fh_mc_all_pi0_theta  = new TH1D("h_mc_all_pi0_theta", "", 1800, 0., 180.);
    fh_mc_rec_pi0_theta  = new TH1D("h_mc_rec_pi0_theta", "", 1800, 0., 180.);
    fh_rich_id_pi0_theta = new TH1D("h_rich_id_pi0_theta","", 1800, 0., 180.);
    // gamma
    fh_mc_all_gamma_theta  = new TH1D("h_mc_all_gamma_theta", "", 1800, 0., 180.);
    fh_mc_rec_gamma_theta  = new TH1D("h_mc_rec_gamma_theta", "", 1800, 0., 180.);
    fh_rich_id_gamma_theta = new TH1D("h_rich_id_gamma_theta","", 1800, 0., 180.);

    // RICH ring ring information -------------------------------------------------
    fh_rich_ring_dist    = new TH1D("h_rich_ring_dist",   "", 100, 0., 10.);
    fh_rich_ring_dist_e  = new TH1D("h_rich_ring_dist_e", "", 100, 0., 10.);
    fh_rich_dist_mom     = new TH2D("h_rich_dist_mom",    "", 100, 0., 10., 100, 0., 10);
    fh_rich_dist_mom_e   = new TH2D("h_rich_dist_mom_e",  "", 100, 0., 10., 100, 0., 10);
    fh_rich_dist_mom_cut = new TH2D("h_rich_dist_mom_cut","", 100, 0., 10., 100, 0., 5.);

    fh_selectNN_true_e  = new TH1D("h_select_NN_true_e", "",80,-2.,2.);
    fh_selectNN_wrong_e = new TH1D("h_select_NN_wrong_e","",80,-2.,2.);

//    fh_rich_ring_xy     = new TH2D("h_rich_ring_xy",    "", 170, -170., 170., 250, -250., 250.);
//    fh_rich_ring_xy_e   = new TH2D("h_rich_ring_xy_e",  "", 170, -170., 170., 250, -250., 250.);
//    fh_rich_ring_xy_cut = new TH2D("h_rich_ring_xy_cut","", 170, -170., 170., 250, -250., 250.);
 //   fh_rich_ring_radi   = new TH1D("h_rich_ring_radi",  "", 600, 0., 300.);
 //   fh_rich_ring_radi_e = new TH1D("h_rich_ring_radi_e","", 600, 0., 300.);

    fh_rich_ring_axisA_mom_all = new TH2D("h_rich_ring_axisA_mom_all",  "", 100, 0., 10., 100, 0., 20);
    fh_rich_ring_axisB_mom_all = new TH2D("h_rich_ring_axisB_mom_all",  "", 100, 0., 10., 100, 0., 20);
 //   fh_rich_ring_axisA_mom     = new TH2D("h_rich_ring_axisA_mom",  "", 100, 0., 10., 100, 0., 20);
 //   fh_rich_ring_axisB_mom     = new TH2D("h_rich_ring_axisB_mom",  "", 100, 0., 10., 100, 0., 20);
 //   fh_rich_ring_axisA         = new TH1D("h_rich_ring_axisA",      "", 120, 2., 10.);
 //   fh_rich_ring_axisB         = new TH1D("h_rich_ring_axisB",      "", 120, 2., 10.);
 //   fh_rich_ring_axisA_mom_e   = new TH2D("h_rich_ring_axisA_mom_e","", 100, 0., 10., 100, 0., 20);
 //   fh_rich_ring_axisB_mom_e   = new TH2D("h_rich_ring_axisB_mom_e","", 100, 0., 10., 100, 0., 20);
 //   fh_rich_ring_axisA_e       = new TH1D("h_rich_ring_axisA_e",    "", 120, 2., 10.);
 //   fh_rich_ring_axisB_e       = new TH1D("h_rich_ring_axisB_e",    "", 120, 2., 10.);

    // TRD and TOF information -------------------------------------------------------
  //  fh_trd_eloss      = new TH1D("h_trd_eloss",   "", 200, 0., 0.2);
 //   fh_trd_eloss_e    = new TH1D("h_trd_eloss_e", "", 200, 0., 0.2);
 //   fh_trd_ann        = new TH1D("h_trd_ann",   "", 80,-2.,2.);
 //   fh_trd_ann_e      = new TH1D("h_trd_ann_e", "", 80,-2.,2.);
 //   fh_trd_ann_mom    = new TH2D("h_trd_ann_mom",  "",50,0,5,80,-2.,2.);
 //   fh_trd_ann_mom_e  = new TH2D("h_trd_ann_mom_e","",50,0,5,80,-2.,2.);
    fh_tof_m2_mom          = new TH2D("h_tof_m2_mom",         "", 120, 0., 6., 600, 0., 1.5);
    fh_tof_m2_mom_zoom     = new TH2D("h_tof_m2_mom_zoom",    "", 120, 0., 6., 600, 0., 0.15);
    fh_tof_m2_mom_zoom_cut = new TH2D("h_tof_m2_mom_zoom_cut","", 120, 0., 6., 600, 0., 0.15);


    // d sts to the closest neighbour -------------------------------------------
    fh_dsts_bg    = new TH1D("h_dsts_bg",    "", 1000, 0., 1.);
    fh_dsts_omega = new TH1D("h_dsts_omega", "", 1000, 0., 1.);
    fh_dsts_pi0   = new TH1D("h_dsts_pi0",   "", 1000, 0., 1.);
    fh_dsts_gamma = new TH1D("h_dsts_gamma", "", 1000, 0., 1.);
    fh_dsts_mom_bg    = new TH2D("h_dsts_mom_bg",    "", 50, 0., 1., 100, 0., 10.);
    fh_dsts_mom_omega = new TH2D("h_dsts_mom_omega", "", 50, 0., 1., 100, 0., 10.);
    fh_dsts_mom_pi0   = new TH2D("h_dsts_mom_pi0",   "", 50, 0., 1., 100, 0., 10.);
    fh_dsts_mom_gamma = new TH2D("h_dsts_mom_gamma", "", 50, 0., 1., 100, 0., 10.);

    // (theta + mom) cut --------------------------------------------------------
    fh_dcut_theta_mom_omega = new TH2D("h_dcut_theta_mom_omega", "", 100, 0., 5., 500, 0., 50.);
    fh_dcut_theta_mom_pi0   = new TH2D("h_dcut_theta_mom_pi0",   "", 100, 0., 5., 500, 0., 50.);
    fh_dcut_theta_mom_gamma = new TH2D("h_dcut_theta_mom_gamma", "", 100, 0., 5., 500, 0., 50.);
    fh_dcut_theta_mom_bg    = new TH2D("h_dcut_theta_mom_bg",    "", 100, 0., 5., 500, 0., 50.);
    fh_dcut_theta_mom_omega_true = new TH2D("h_dcut_theta_mom_omega_true", "", 100, 0., 5., 500, 0., 50.);
    fh_dcut_theta_mom_gamma_true = new TH2D("h_dcut_theta_mom_gamma_true", "", 100, 0., 5., 500, 0., 50.);

    // pt cut -------------------------------------------------------------------
    fh_tpcut_pt_lept_omega = new TH1D("h_tpcut_pt_lept_omega", "", 1000, 0., 10.);
    fh_tpcut_pt_lept_bg    = new TH1D("h_tpcut_pt_lept_bg",    "", 1000, 0., 10.);

    // opening angle ------------------------------------------------------------
    fh_ptcut_theta_omega = new TH1D("h_ptcut_theta_omega", "", 1000, 0., 50.);
    fh_ptcut_theta_omega_true = new TH1D("h_ptcut_theta_omega_true", "", 1000, 0., 50.);
    fh_ptcut_theta_pi0   = new TH1D("h_ptcut_theta_pi0",   "", 1000, 0., 50.);
    fh_ptcut_theta_gamma = new TH1D("h_ptcut_theta_gamma", "", 1000, 0., 50.);
    fh_ptcut_theta_bg    = new TH1D("h_ptcut_theta_bg",    "", 1000, 0., 50.);

    // invariant mass -----------------------------------------------------------
    fh_mc_all_minv  = new TH1D("h_mc_all_minv", "", 2000, 0., 2.);
    fh_mc_rec_minv  = new TH1D("h_mc_rec_minv", "", 2000, 0., 2.);
    fh_rich_id_minv = new TH1D("h_rich_id_minv","", 2000, 0., 2.);
    fh_gcut_minv    = new TH1D("h_gcut_minv",   "", 2000, 0., 2.);
    fh_dcut_minv    = new TH1D("h_dcut_minv",   "", 2000, 0., 2.);
    fh_tpcut_minv   = new TH1D("h_tpcut_minv",  "", 2000, 0., 2.);
    fh_ptcut_minv   = new TH1D("h_ptcut_minv",  "", 2000, 0., 2.);
    fh_tcut_minv    = new TH1D("h_tcut_minv",   "", 2000, 0., 2.);
    fh_pi0cut_minv  = new TH1D("h_pi0cut_minv", "", 2000, 0., 2.);
    fh_rich_id_mc_minv = new TH1D("h_rich_id_mc_minv","", 2000, 0., 2.);
    fh_gcut_mc_minv    = new TH1D("h_gcut_mc_minv",   "", 2000, 0., 2.);
    fh_dcut_mc_minv    = new TH1D("h_dcut_mc_minv",   "", 2000, 0., 2.);
    fh_tpcut_mc_minv   = new TH1D("h_tpcut_mc_minv",  "", 2000, 0., 2.);
    fh_ptcut_mc_minv   = new TH1D("h_ptcut_mc_minv",  "", 2000, 0., 2.);
    fh_tcut_mc_minv    = new TH1D("h_tcut_mc_minv",   "", 2000, 0., 2.);
    fh_pi0cut_mc_minv  = new TH1D("h_pi0cut_mc_minv", "", 2000, 0., 2.);
    fh_mc_all_omega_bg_minv  = new TH1D("h_mc_all_omega_bg_minv", "", 2000, 0., 2.);
    fh_mc_rec_omega_bg_minv  = new TH1D("h_mc_rec_omega_bg_minv", "", 2000, 0., 2.);
    fh_rich_id_omega_bg_minv = new TH1D("h_rich_id_omega_bg_minv","", 2000, 0., 2.);
    fh_gcut_omega_bg_minv    = new TH1D("h_gcut_omega_bg_minv",   "", 2000, 0., 2.);
    fh_dcut_omega_bg_minv    = new TH1D("h_dcut_omega_bg_minv",   "", 2000, 0., 2.);
    fh_tpcut_omega_bg_minv   = new TH1D("h_tpcut_omega_bg_minv",  "", 2000, 0., 2.);
    fh_ptcut_omega_bg_minv   = new TH1D("h_ptcut_omega_bg_minv",  "", 2000, 0., 2.);
    fh_tcut_omega_bg_minv    = new TH1D("h_tcut_omega_bg_minv",   "", 2000, 0., 2.);
    fh_pi0cut_omega_bg_minv  = new TH1D("h_pi0cut_omega_bg_minv", "", 2000, 0., 2.);
    fh_rich_id_mc_omega_bg_minv = new TH1D("h_rich_id_mc_omega_bg_minv","", 2000, 0., 2.);
    fh_gcut_mc_omega_bg_minv    = new TH1D("h_gcut_mc_omega_bg_minv",   "", 2000, 0., 2.);
    fh_dcut_mc_omega_bg_minv    = new TH1D("h_dcut_mc_omega_bg_minv",   "", 2000, 0., 2.);
    fh_tpcut_mc_omega_bg_minv   = new TH1D("h_tpcut_mc_omega_bg_minv",  "", 2000, 0., 2.);
    fh_ptcut_mc_omega_bg_minv   = new TH1D("h_ptcut_mc_omega_bg_minv",  "", 2000, 0., 2.);
    fh_tcut_mc_omega_bg_minv    = new TH1D("h_tcut_mc_omega_bg_minv",   "", 2000, 0., 2.);
    fh_pi0cut_mc_omega_bg_minv  = new TH1D("h_pi0cut_mc_omega_bg_minv", "", 2000, 0., 2.);

    fh_pi0cut_epep_minv = new TH1D("h_pi0cut_epep_minv", "", 2000, 0., 2.);
    fh_pi0cut_emem_minv = new TH1D("h_pi0cut_emem_minv", "", 2000, 0., 2.);

    fh_mc_all_pt_minv_omega  = new TH2D("h_mc_all_pt_minv_omega", "", 100, 0., 2., 200, 0., 5.);
    fh_mc_all_pt_minv_pairs  = new TH2D("h_mc_all_pt_minv_pairs", "", 100, 0., 2., 200, 0., 5.);
    fh_mc_rec_pt_minv_omega  = new TH2D("h_mc_rec_pt_minv_omega", "", 100, 0., 2., 200, 0., 5.);
    fh_mc_rec_pt_minv_pairs  = new TH2D("h_mc_rec_pt_minv_pairs", "", 100, 0., 2., 200, 0., 5.);
    fh_rich_id_pt_minv_omega = new TH2D("h_rich_id_pt_minv_omega","", 100, 0., 2., 200, 0., 5.);
    fh_rich_id_pt_minv_pairs = new TH2D("h_rich_id_pt_minv_pairs","", 100, 0., 2., 200, 0., 5.);
    fh_tpcut_pt_minv_omega   = new TH2D("h_tpcut_pt_minv_omega", "", 100, 0., 2., 200, 0., 5.);
    fh_tpcut_pt_minv_pairs   = new TH2D("h_tpcut_pt_minv_pairs", "", 100, 0., 2., 200, 0., 5.);
    fh_pi0cut_pt_minv_omega  = new TH2D("h_pi0cut_pt_minv_omega", "", 100, 0., 2., 200, 0., 5.);
    fh_pi0cut_pt_minv_pairs  = new TH2D("h_pi0cut_pt_minv_pairs", "", 100, 0., 2., 200, 0., 5.);

    // omega
    fh_mc_all_omega_minv  = new TH1D("h_mc_all_omega_minv", "", 2000, 0., 2.);
    fh_mc_rec_omega_minv  = new TH1D("h_mc_rec_omega_minv", "", 2000, 0., 2.);
    fh_rich_id_omega_minv = new TH1D("h_rich_id_omega_minv","", 2000, 0., 2.);
    fh_gcut_omega_minv    = new TH1D("h_gcut_omega_minv",   "", 2000, 0., 2.);
    fh_dcut_omega_minv    = new TH1D("h_dcut_omega_minv",   "", 2000, 0., 2.);
    fh_tpcut_omega_minv   = new TH1D("h_tpcut_omega_minv",  "", 2000, 0., 2.);
    fh_ptcut_omega_minv   = new TH1D("h_ptcut_omega_minv",  "", 2000, 0., 2.);
    fh_tcut_omega_minv    = new TH1D("h_tcut_omega_minv",   "", 2000, 0., 2.);
    fh_pi0cut_omega_minv  = new TH1D("h_pi0cut_omega_minv", "", 2000, 0., 2.);
    // eta
    fh_mc_all_eta_minv  = new TH1D("h_mc_all_eta_minv", "", 2000, 0., 2.);
    fh_mc_rec_eta_minv  = new TH1D("h_mc_rec_eta_minv", "", 2000, 0., 2.);
    fh_rich_id_eta_minv = new TH1D("h_rich_id_eta_minv","", 2000, 0., 2.);
    fh_gcut_eta_minv    = new TH1D("h_gcut_eta_minv",   "", 2000, 0., 2.);
    fh_dcut_eta_minv    = new TH1D("h_dcut_eta_minv",   "", 2000, 0., 2.);
    fh_tpcut_eta_minv   = new TH1D("h_tpcut_eta_minv",  "", 2000, 0., 2.);
    fh_ptcut_eta_minv   = new TH1D("h_ptcut_eta_minv",  "", 2000, 0., 2.);
    fh_tcut_eta_minv    = new TH1D("h_tcut_eta_minv",   "", 2000, 0., 2.);
    fh_pi0cut_eta_minv  = new TH1D("h_pi0cut_eta_minv", "", 2000, 0., 2.);
    // pi0
    fh_mc_all_pi0_minv  = new TH1D("h_mc_all_pi0_minv", "", 2000, 0., 2.);
    fh_mc_rec_pi0_minv  = new TH1D("h_mc_rec_pi0_minv", "", 2000, 0., 2.);
    fh_rich_id_pi0_minv = new TH1D("h_rich_id_pi0_minv","", 2000, 0., 2.);
    fh_gcut_pi0_minv    = new TH1D("h_gcut_pi0_minv",   "", 2000, 0., 2.);
    fh_dcut_pi0_minv    = new TH1D("h_dcut_pi0_minv",   "", 2000, 0., 2.);
    fh_tpcut_pi0_minv   = new TH1D("h_tpcut_pi0_minv",  "", 2000, 0., 2.);
    fh_ptcut_pi0_minv   = new TH1D("h_ptcut_pi0_minv",  "", 2000, 0., 2.);
    fh_tcut_pi0_minv    = new TH1D("h_tcut_pi0_minv",   "", 2000, 0., 2.);
    fh_pi0cut_pi0_minv  = new TH1D("h_pi0cut_pi0_minv", "", 2000, 0., 2.);
    fh_pi0cut_fake_pi0_minv = new TH1D("h_pi0cut_fake_pi0_minv","", 2000, 0., 2.);
    fh_pi0cut_true_pi0_minv = new TH1D("h_pi0cut_true_pi0_minv","", 2000, 0., 2.);
    // gamma
    fh_mc_all_gamma_minv = new TH1D("h_mc_all_gamma_minv", "", 2000, 0., 2.);
    fh_mc_rec_gamma_minv = new TH1D("h_mc_rec_gamma_minv", "", 2000, 0., 2.);
    fh_rich_id_gamma_minv = new TH1D("h_rich_id_gamma_minv","", 2000, 0., 2.);
    fh_gcut_gamma_minv    = new TH1D("h_gcut_gamma_minv",   "", 2000, 0., 2.);
    fh_dcut_gamma_minv    = new TH1D("h_dcut_gamma_minv",   "", 2000, 0., 2.);
    fh_tpcut_gamma_minv   = new TH1D("h_tpcut_gamma_minv",  "", 2000, 0., 2.);
    fh_ptcut_gamma_minv   = new TH1D("h_ptcut_gamma_minv",  "", 2000, 0., 2.);
    fh_tcut_gamma_minv    = new TH1D("h_tcut_gamma_minv",   "", 2000, 0., 2.);
    fh_pi0cut_gamma_minv  = new TH1D("h_pi0cut_gamma_minv", "", 2000, 0., 2.);

    // pair pt-y ----------------------------------------------------------------
    fh_mc_all_pty  = new TH2D("h_mc_all_pty", "", 40, 0., 4., 20, 0., 2.);
    fh_mc_rec_pty  = new TH2D("h_mc_rec_pty", "", 40, 0., 4., 20, 0., 2.);
    fh_rich_id_pty = new TH2D("h_rich_id_pty","", 40, 0., 4., 20, 0., 2.);
    fh_gcut_pty    = new TH2D("h_gcut_pty",   "", 40, 0., 4., 20, 0., 2.);
    fh_dcut_pty    = new TH2D("h_dcut_pty",   "", 40, 0., 4., 20, 0., 2.);
    fh_tpcut_pty   = new TH2D("h_tpcut_pty",  "", 40, 0., 4., 20, 0., 2.);
    fh_ptcut_pty   = new TH2D("h_ptcut_pty",  "", 40, 0., 4., 20, 0., 2.);
    fh_tcut_pty    = new TH2D("h_tcut_pty",   "", 40, 0., 4., 20, 0., 2.);
    fh_pi0cut_pty  = new TH2D("h_pi0cut_pty", "", 40, 0., 4., 20, 0., 2.);
    // omega
    fh_mc_all_omega_pty = new TH2D("h_mc_all_omega_pty", "", 40, 0., 4., 20, 0., 2.);
    fh_mc_rec_omega_pty = new TH2D("h_mc_rec_omega_pty", "", 40, 0., 4., 20, 0., 2.);
    fh_rich_id_omega_pty = new TH2D("h_rich_id_omega_pty","", 40, 0., 4., 20, 0., 2.);
    fh_gcut_omega_pty   = new TH2D("h_gcut_omega_pty",   "", 40, 0., 4., 20, 0., 2.);
    fh_dcut_omega_pty   = new TH2D("h_dcut_omega_pty",   "", 40, 0., 4., 20, 0., 2.);
    fh_tpcut_omega_pty  = new TH2D("h_tpcut_omega_pty",  "", 40, 0., 4., 20, 0., 2.);
    fh_ptcut_omega_pty  = new TH2D("h_ptcut_omega_pty",  "", 40, 0., 4., 20, 0., 2.);
    fh_tcut_omega_pty   = new TH2D("h_tcut_omega_pty",   "", 40, 0., 4., 20, 0., 2.);
    fh_pi0cut_omega_pty = new TH2D("h_pi0cut_omega_pty", "", 40, 0., 4., 20, 0., 2.);
    // eta
    fh_mc_all_eta_pty  = new TH2D("h_mc_all_eta_pty", "", 40, 0., 4., 20, 0., 2.);
    fh_mc_rec_eta_pty  = new TH2D("h_mc_rec_eta_pty", "", 40, 0., 4., 20, 0., 2.);
    fh_rich_id_eta_pty = new TH2D("h_rich_id_eta_pty","", 40, 0., 4., 20, 0., 2.);
    fh_gcut_eta_pty    = new TH2D("h_gcut_eta_pty",   "", 40, 0., 4., 20, 0., 2.);
    fh_dcut_eta_pty    = new TH2D("h_dcut_eta_pty",   "", 40, 0., 4., 20, 0., 2.);
    fh_tpcut_eta_pty   = new TH2D("h_tpcut_eta_pty",  "", 40, 0., 4., 20, 0., 2.);
    fh_ptcut_eta_pty   = new TH2D("h_ptcut_eta_pty",  "", 40, 0., 4., 20, 0., 2.);
    fh_tcut_eta_pty    = new TH2D("h_tcut_eta_pty",   "", 40, 0., 4., 20, 0., 2.);
    fh_pi0cut_eta_pty  = new TH2D("h_pi0cut_eta_pty", "", 40, 0., 4., 20, 0., 2.);
    // pi0
    fh_mc_all_pi0_pty  = new TH2D("h_mc_all_pi0_pty", "", 40, 0., 4., 20, 0., 2.);
    fh_mc_rec_pi0_pty  = new TH2D("h_mc_rec_pi0_pty", "", 40, 0., 4., 20, 0., 2.);
    fh_rich_id_pi0_pty = new TH2D("h_rich_id_pi0_pty","", 40, 0., 4., 20, 0., 2.);
    fh_gcut_pi0_pty    = new TH2D("h_gcut_pi0_pty",   "", 40, 0., 4., 20, 0., 2.);
    fh_dcut_pi0_pty    = new TH2D("h_dcut_pi0_pty",   "", 40, 0., 4., 20, 0., 2.);
    fh_tpcut_pi0_pty   = new TH2D("h_tpcut_pi0_pty",  "", 40, 0., 4., 20, 0., 2.);
    fh_ptcut_pi0_pty   = new TH2D("h_ptcut_pi0_pty",  "", 40, 0., 4., 20, 0., 2.);
    fh_tcut_pi0_pty    = new TH2D("h_tcut_pi0_pty",   "", 40, 0., 4., 20, 0., 2.);
    fh_pi0cut_pi0_pty  = new TH2D("h_pi0cut_pi0_pty", "", 40, 0., 4., 20, 0., 2.);
    // gamma
    fh_mc_all_gamma_pty  = new TH2D("h_mc_all_gamma_pty", "", 40, 0., 4., 20, 0., 2.);
    fh_mc_rec_gamma_pty  = new TH2D("h_mc_rec_gamma_pty", "", 40, 0., 4., 20, 0., 2.);
    fh_rich_id_gamma_pty = new TH2D("h_rich_id_gamma_pty","", 40, 0., 4., 20, 0., 2.);
    fh_gcut_gamma_pty    = new TH2D("h_gcut_gamma_pty",   "", 40, 0., 4., 20, 0., 2.);
    fh_dcut_gamma_pty    = new TH2D("h_dcut_gamma_pty",   "", 40, 0., 4., 20, 0., 2.);
    fh_tpcut_gamma_pty   = new TH2D("h_tpcut_gamma_pty",  "", 40, 0., 4., 20, 0., 2.);
    fh_ptcut_gamma_pty   = new TH2D("h_ptcut_gamma_pty",  "", 40, 0., 4., 20, 0., 2.);
    fh_tcut_gamma_pty    = new TH2D("h_tcut_gamma_pty",   "", 40, 0., 4., 20, 0., 2.);
    fh_pi0cut_gamma_pty  = new TH2D("h_pi0cut_gamma_pty",   "", 40, 0., 4., 20, 0., 2.);

    fh_rich_id_mult = new TH2D("h_rich_id_mult","",100,0.,100.,100,0.,100.);
    fh_gcut_mult    = new TH2D("h_gcut_mult","",100,0.,100.,100,0.,100.);
    fh_dcut_mult    = new TH2D("h_dcut_mult","",100,0.,100.,100,0.,100.);
    fh_tpcut_mult   = new TH2D("h_tpcut_mult","",100,0.,100.,100,0.,100.);
    fh_ptcut_mult   = new TH2D("h_ptcut_mult","",100,0.,100.,100,0.,100.);
    fh_tcut_mult    = new TH2D("h_tcut_mult","",100,0.,100.,100,0.,100.);
    fh_pi0cut_mult  = new TH2D("h_pi0cut_mult","",100,0.,100.,100,0.,100.);

    fh_mom_rich = new TH1D("h_mom_rich","",1000,0,10);
 //   fh_mom_trd  = new TH1D("h_mom_trd", "",1000,0,10);
    fh_mom_tof  = new TH1D("h_mom_tof", "",1000,0,10);

    fh_mom_w_sts  = new TH1D("h_mom_w_sts", "",1000,0,10);
    fh_mom_w_rich = new TH1D("h_mom_w_rich","",1000,0,10);
//    fh_mom_w_trd  = new TH1D("h_mom_w_trd", "",1000,0,10);
    fh_mom_w_tof  = new TH1D("h_mom_w_tof", "",1000,0,10);

    fh_mom_e_sts  = new TH1D("h_mom_e_sts", "",1000,0,10);
    fh_mom_e_rich = new TH1D("h_mom_e_rich","",1000,0,10);
//    fh_mom_e_trd  = new TH1D("h_mom_e_trd", "",1000,0,10);
    fh_mom_e_tof  = new TH1D("h_mom_e_tof", "",1000,0,10);

    // background cocktail
    fh_rich_id_pi0_pi0_minv    = new TH1D("h_rich_id_pi0_pi0_minv",    "",2000, 0., 2.);
    fh_rich_id_pi0_gamma_minv  = new TH1D("h_rich_id_pi0_gamma_minv",  "",2000, 0., 2.);
    fh_rich_id_gamma_gamma_minv= new TH1D("h_rich_id_gamma_gamma_minv","",2000, 0., 2.);
    fh_rich_id_eta_gamma_minv  = new TH1D("h_rich_id_eta_gamma_minv",  "",2000, 0., 2.);
    fh_rich_id_eta_eta_minv    = new TH1D("h_rich_id_eta_eta_minv",    "",2000, 0., 2.);
    fh_rich_id_eta_pi0_minv    = new TH1D("h_rich_id_eta_pi0_minv",    "",2000, 0., 2.);

//    fh_trd_id_pi0_pi0_minv    = new TH1D("h_trd_id_pi0_pi0_minv",    "",2000, 0., 2.);
//   fh_trd_id_pi0_gamma_minv  = new TH1D("h_trd_id_pi0_gamma_minv",  "",2000, 0., 2.);
//    fh_trd_id_gamma_gamma_minv= new TH1D("h_trd_id_gamma_gamma_minv","",2000, 0., 2.);
//    fh_trd_id_eta_gamma_minv  = new TH1D("h_trd_id_eta_gamma_minv",  "",2000, 0., 2.);
//    fh_trd_id_eta_eta_minv    = new TH1D("h_trd_id_eta_eta_minv",    "",2000, 0., 2.);
//    fh_trd_id_eta_pi0_minv    = new TH1D("h_trd_id_eta_pi0_minv",    "",2000, 0., 2.);

    fh_tof_id_pi0_pi0_minv    = new TH1D("h_tof_id_pi0_pi0_minv",    "",2000, 0., 2.);
    fh_tof_id_pi0_gamma_minv  = new TH1D("h_tof_id_pi0_gamma_minv",  "",2000, 0., 2.);
    fh_tof_id_gamma_gamma_minv= new TH1D("h_tof_id_gamma_gamma_minv","",2000, 0., 2.);
    fh_tof_id_eta_gamma_minv  = new TH1D("h_tof_id_eta_gamma_minv",  "",2000, 0., 2.);
    fh_tof_id_eta_eta_minv    = new TH1D("h_tof_id_eta_eta_minv",    "",2000, 0., 2.);
    fh_tof_id_eta_pi0_minv    = new TH1D("h_tof_id_eta_pi0_minv",    "",2000, 0., 2.);

    fh_pi0cut_pi0_pi0_minv    = new TH1D("h_pi0cut_pi0_pi0_minv",    "",2000, 0., 2.);
    fh_pi0cut_pi0_gamma_minv  = new TH1D("h_pi0cut_pi0_gamma_minv",  "",2000, 0., 2.);
    fh_pi0cut_gamma_gamma_minv= new TH1D("h_pi0cut_gamma_gamma_minv","",2000, 0., 2.);
    fh_pi0cut_eta_gamma_minv  = new TH1D("h_pi0cut_eta_gamma_minv",  "",2000, 0., 2.);
    fh_pi0cut_eta_eta_minv    = new TH1D("h_pi0cut_eta_eta_minv",    "",2000, 0., 2.);
    fh_pi0cut_eta_pi0_minv    = new TH1D("h_pi0cut_eta_pi0_minv",    "",2000, 0., 2.);

    //----------------------------------------
    fh_rich_tof_id_cut_mom = new TH1D("h_rich_tof_id_cut_mom", "", 1000, 0., 10.);


   //----------------------------------------


  Int_t nEvents = 4;   // number of files to be added
  for(Int_t i = 0; i < nEvents; i++) {

    char str[4];
    sprintf(str, "%4d", i+1);
    for(Int_t j = 0; j < 4; j++) {
      if(' ' == str[j]) str[j] = '0';
    }

   TFile* file = new TFile("/lustre/cbm/user/slebedev/all/analysis.all." + TString(str) + ".root");

/*
TFile* file = new TFile("/lustre/cbm/user/ebelolap/glue/70_field_10gev/10gev.centr.spectra." + TString(str) + ".root");
  */  

cout << "File number : " << TString(str) << endl;


    fh_chi2_prim_all->Add((TH1D*)file->Get("h_chi2_prim_all"));
    fh_chi2_prim_e->Add((TH1D*)file->Get("h_chi2_prim_e"));
    fh_mc_all_zr->Add((TH2D*)file->Get("h_mc_all_zr"));
    fh_mc_all_lept_mom->Add((TH1D*)file->Get("h_mc_all_lept_mom"));
    fh_mc_rec_lept_mom  ->Add((TH1D*)file->Get("h_mc_rec_lept_mom"));
    fh_rich_id_lept_mom ->Add((TH1D*)file->Get("h_rich_id_lept_mom"));
    fh_rich_id_ep_mom   ->Add((TH1D*)file->Get("h_rich_id_ep_mom"));
    fh_rich_id_em_mom   ->Add((TH1D*)file->Get("h_rich_id_em_mom"));
    fh_rich_id_no_cut_mom ->Add((TH1D*)file->Get("h_rich_id_no_cut_mom"));
    fh_rich_id_mom_cut_mom->Add((TH1D*)file->Get("h_rich_id_mom_cut_mom"));
    fh_rich_id_ann_cut_mom ->Add((TH1D*)file->Get("h_rich_id_ann_cut_mom"));
    fh_mc_all_mom ->Add((TH1D*)file->Get("h_mc_all_mom"));
    fh_mc_rec_mom ->Add((TH1D*)file->Get("h_mc_rec_mom"));
    fh_rich_id_mom->Add((TH1D*)file->Get("h_rich_id_mom"));
    fh_mc_all_omega_mom  ->Add((TH1D*)file->Get("h_mc_all_omega_mom"));
    fh_mc_rec_omega_mom  ->Add((TH1D*)file->Get("h_mc_rec_omega_mom"));
    fh_rich_id_omega_mom ->Add((TH1D*)file->Get("h_rich_id_omega_mom"));
    fh_mc_all_eta_mom  ->Add((TH1D*)file->Get("h_mc_all_eta_mom"));
    fh_mc_rec_eta_mom  ->Add((TH1D*)file->Get("h_mc_rec_eta_mom"));
    fh_rich_id_eta_mom ->Add((TH1D*)file->Get("h_rich_id_eta_mom"));
    fh_mc_all_pi0_mom  ->Add((TH1D*)file->Get("h_mc_all_pi0_mom"));
    fh_mc_rec_pi0_mom  ->Add((TH1D*)file->Get("h_mc_rec_pi0_mom"));
    fh_rich_id_pi0_mom ->Add((TH1D*)file->Get("h_rich_id_pi0_mom"));
    fh_mc_all_gamma_mom  ->Add((TH1D*)file->Get("h_mc_all_gamma_mom"));
    fh_mc_rec_gamma_mom  ->Add((TH1D*)file->Get("h_mc_rec_gamma_mom"));
    fh_rich_id_gamma_mom ->Add((TH1D*)file->Get("h_rich_id_gamma_mom"));
    fh_mc_all_theta ->Add((TH1D*)file->Get("h_mc_all_theta"));
    fh_mc_rec_theta ->Add((TH1D*)file->Get("h_mc_rec_theta"));
    fh_rich_id_theta->Add((TH1D*)file->Get("h_rich_id_theta"));
    fh_mc_all_omega_theta  ->Add((TH1D*)file->Get("h_mc_all_omega_theta"));
    fh_mc_rec_omega_theta  ->Add((TH1D*)file->Get("h_mc_rec_omega_theta"));
    fh_rich_id_omega_theta ->Add((TH1D*)file->Get("h_rich_id_omega_theta"));
    fh_mc_all_eta_theta  ->Add((TH1D*)file->Get("h_mc_all_eta_theta"));
    fh_mc_rec_eta_theta  ->Add((TH1D*)file->Get("h_mc_rec_eta_theta"));
    fh_rich_id_eta_theta ->Add((TH1D*)file->Get("h_rich_id_eta_theta"));
    fh_mc_all_pi0_theta  ->Add((TH1D*)file->Get("h_mc_all_pi0_theta"));
    fh_mc_rec_pi0_theta  ->Add((TH1D*)file->Get("h_mc_rec_pi0_theta"));
    fh_rich_id_pi0_theta ->Add((TH1D*)file->Get("h_rich_id_pi0_theta"));
    fh_mc_all_gamma_theta  ->Add((TH1D*)file->Get("h_mc_all_gamma_theta"));
    fh_mc_rec_gamma_theta  ->Add((TH1D*)file->Get("h_mc_rec_gamma_theta"));
    fh_rich_id_gamma_theta ->Add((TH1D*)file->Get("h_rich_id_gamma_theta"));
    fh_rich_ring_dist    ->Add((TH1D*)file->Get("h_rich_ring_dist"));
    fh_rich_ring_dist_e  ->Add((TH1D*)file->Get("h_rich_ring_dist_e"));
    fh_rich_dist_mom     ->Add((TH2D*)file->Get("h_rich_dist_mom"));
    fh_rich_dist_mom_e   ->Add((TH2D*)file->Get("h_rich_dist_mom_e"));
    fh_rich_dist_mom_cut ->Add((TH2D*)file->Get("h_rich_dist_mom_cut"));
    fh_selectNN_true_e  ->Add((TH1D*)file->Get("h_select_NN_true_e"));
    fh_selectNN_wrong_e ->Add((TH1D*)file->Get("h_select_NN_wrong_e"));
    fh_rich_ring_axisA_mom_all ->Add((TH2D*)file->Get("h_rich_ring_axisA_mom_all"));
    fh_rich_ring_axisB_mom_all ->Add((TH2D*)file->Get("h_rich_ring_axisB_mom_all"));
    fh_tof_m2_mom          ->Add((TH2D*)file->Get("h_tof_m2_mom"));
    fh_tof_m2_mom_zoom     ->Add((TH2D*)file->Get("h_tof_m2_mom_zoom"));
    fh_tof_m2_mom_zoom_cut ->Add((TH2D*)file->Get("h_tof_m2_mom_zoom_cut"));
    fh_dsts_bg    ->Add((TH1D*)file->Get("h_dsts_bg"));
    fh_dsts_omega ->Add((TH1D*)file->Get("h_dsts_omega"));
    fh_dsts_pi0   ->Add((TH1D*)file->Get("h_dsts_pi0"));
    fh_dsts_gamma ->Add((TH1D*)file->Get("h_dsts_gamma"));
    fh_dsts_mom_bg    ->Add((TH2D*)file->Get("h_dsts_mom_bg"));
    fh_dsts_mom_omega ->Add((TH2D*)file->Get("h_dsts_mom_omega"));
    fh_dsts_mom_pi0   ->Add((TH2D*)file->Get("h_dsts_mom_pi0"));
    fh_dsts_mom_gamma ->Add((TH2D*)file->Get("h_dsts_mom_gamma"));
    fh_dcut_theta_mom_omega ->Add((TH2D*)file->Get("h_dcut_theta_mom_omega"));
    fh_dcut_theta_mom_pi0   ->Add((TH2D*)file->Get("h_dcut_theta_mom_pi0"));
    fh_dcut_theta_mom_gamma ->Add((TH2D*)file->Get("h_dcut_theta_mom_gamma"));
    fh_dcut_theta_mom_bg    ->Add((TH2D*)file->Get("h_dcut_theta_mom_bg"));
    fh_dcut_theta_mom_omega_true ->Add((TH2D*)file->Get("h_dcut_theta_mom_omega_true"));
    fh_dcut_theta_mom_gamma_true ->Add((TH2D*)file->Get("h_dcut_theta_mom_gamma_true"));
    fh_tpcut_pt_lept_omega ->Add((TH1D*)file->Get("h_tpcut_pt_lept_omega"));
    fh_tpcut_pt_lept_bg    ->Add((TH1D*)file->Get("h_tpcut_pt_lept_bg"));
    fh_ptcut_theta_omega ->Add((TH1D*)file->Get("h_ptcut_theta_omega"));
    fh_ptcut_theta_omega_true ->Add((TH1D*)file->Get("h_ptcut_theta_omega_true"));
    fh_ptcut_theta_pi0   ->Add((TH1D*)file->Get("h_ptcut_theta_pi0"));
    fh_ptcut_theta_gamma ->Add((TH1D*)file->Get("h_ptcut_theta_gamma"));
    fh_ptcut_theta_bg    ->Add((TH1D*)file->Get("h_ptcut_theta_bg"));
    fh_mc_all_minv  ->Add((TH1D*)file->Get("h_mc_all_minv"));
    fh_mc_rec_minv  ->Add((TH1D*)file->Get("h_mc_rec_minv"));
    fh_rich_id_minv ->Add((TH1D*)file->Get("h_rich_id_minv"));
    fh_gcut_minv    ->Add((TH1D*)file->Get("h_gcut_minv"));
    fh_dcut_minv    ->Add((TH1D*)file->Get("h_dcut_minv"));
    fh_tpcut_minv   ->Add((TH1D*)file->Get("h_tpcut_minv"));
    fh_ptcut_minv   ->Add((TH1D*)file->Get("h_ptcut_minv"));
    fh_tcut_minv    ->Add((TH1D*)file->Get("h_tcut_minv"));
    fh_pi0cut_minv  ->Add((TH1D*)file->Get("h_pi0cut_minv"));
    fh_rich_id_mc_minv ->Add((TH1D*)file->Get("h_rich_id_mc_minv"));
    fh_gcut_mc_minv    ->Add((TH1D*)file->Get("h_gcut_mc_minv"));
    fh_dcut_mc_minv    ->Add((TH1D*)file->Get("h_dcut_mc_minv"));
    fh_tpcut_mc_minv   ->Add((TH1D*)file->Get("h_tpcut_mc_minv"));
    fh_ptcut_mc_minv   ->Add((TH1D*)file->Get("h_ptcut_mc_minv"));
    fh_tcut_mc_minv    ->Add((TH1D*)file->Get("h_tcut_mc_minv"));
    fh_pi0cut_mc_minv  ->Add((TH1D*)file->Get("h_pi0cut_mc_minv"));
    fh_mc_all_omega_bg_minv  ->Add((TH1D*)file->Get("h_mc_all_omega_bg_minv"));
    fh_mc_rec_omega_bg_minv  ->Add((TH1D*)file->Get("h_mc_rec_omega_bg_minv"));
    fh_rich_id_omega_bg_minv ->Add((TH1D*)file->Get("h_rich_id_omega_bg_minv"));
    fh_gcut_omega_bg_minv    ->Add((TH1D*)file->Get("h_gcut_omega_bg_minv"));
    fh_dcut_omega_bg_minv    ->Add((TH1D*)file->Get("h_dcut_omega_bg_minv"));
    fh_tpcut_omega_bg_minv   ->Add((TH1D*)file->Get("h_tpcut_omega_bg_minv"));
    fh_ptcut_omega_bg_minv   ->Add((TH1D*)file->Get("h_ptcut_omega_bg_minv"));
    fh_tcut_omega_bg_minv    ->Add((TH1D*)file->Get("h_tcut_omega_bg_minv"));
    fh_pi0cut_omega_bg_minv  ->Add((TH1D*)file->Get("h_pi0cut_omega_bg_minv"));
    fh_rich_id_mc_omega_bg_minv ->Add((TH1D*)file->Get("h_rich_id_mc_omega_bg_minv"));
    fh_gcut_mc_omega_bg_minv    ->Add((TH1D*)file->Get("h_gcut_mc_omega_bg_minv"));
    fh_dcut_mc_omega_bg_minv    ->Add((TH1D*)file->Get("h_dcut_mc_omega_bg_minv"));
    fh_tpcut_mc_omega_bg_minv   ->Add((TH1D*)file->Get("h_tpcut_mc_omega_bg_minv"));
    fh_ptcut_mc_omega_bg_minv   ->Add((TH1D*)file->Get("h_ptcut_mc_omega_bg_minv"));
    fh_tcut_mc_omega_bg_minv    ->Add((TH1D*)file->Get("h_tcut_mc_omega_bg_minv"));
    fh_pi0cut_mc_omega_bg_minv  ->Add((TH1D*)file->Get("h_pi0cut_mc_omega_bg_minv"));

    fh_pi0cut_epep_minv ->Add((TH1D*)file->Get("h_pi0cut_epep_minv"));
    fh_pi0cut_emem_minv ->Add((TH1D*)file->Get("h_pi0cut_emem_minv"));

    fh_mc_all_pt_minv_omega  ->Add((TH2D*)file->Get("h_mc_all_pt_minv_omega"));
    fh_mc_all_pt_minv_pairs  ->Add((TH2D*)file->Get("h_mc_all_pt_minv_pairs"));
    fh_mc_rec_pt_minv_omega  ->Add((TH2D*)file->Get("h_mc_rec_pt_minv_omega"));
    fh_mc_rec_pt_minv_pairs  ->Add((TH2D*)file->Get("h_mc_rec_pt_minv_pairs"));
    fh_rich_id_pt_minv_omega ->Add((TH2D*)file->Get("h_rich_id_pt_minv_omega"));
    fh_rich_id_pt_minv_pairs ->Add((TH2D*)file->Get("h_rich_id_pt_minv_pairs"));
    fh_tpcut_pt_minv_omega   ->Add((TH2D*)file->Get("h_tpcut_pt_minv_omega"));
    fh_tpcut_pt_minv_pairs   ->Add((TH2D*)file->Get("h_tpcut_pt_minv_pairs"));
    fh_pi0cut_pt_minv_omega  ->Add((TH2D*)file->Get("h_pi0cut_pt_minv_omega"));
    fh_pi0cut_pt_minv_pairs  ->Add((TH2D*)file->Get("h_pi0cut_pt_minv_pairs"));
    fh_mc_all_omega_minv  ->Add((TH1D*)file->Get("h_mc_all_omega_minv"));
    fh_mc_rec_omega_minv  ->Add((TH1D*)file->Get("h_mc_rec_omega_minv"));
    fh_rich_id_omega_minv ->Add((TH1D*)file->Get("h_rich_id_omega_minv"));
    fh_gcut_omega_minv    ->Add((TH1D*)file->Get("h_gcut_omega_minv"));
    fh_dcut_omega_minv    ->Add((TH1D*)file->Get("h_dcut_omega_minv"));
    fh_tpcut_omega_minv   ->Add((TH1D*)file->Get("h_tpcut_omega_minv"));
    fh_ptcut_omega_minv   ->Add((TH1D*)file->Get("h_ptcut_omega_minv"));
    fh_tcut_omega_minv    ->Add((TH1D*)file->Get("h_tcut_omega_minv"));
    fh_pi0cut_omega_minv  ->Add((TH1D*)file->Get("h_pi0cut_omega_minv"));
    // eta
    fh_mc_all_eta_minv  ->Add((TH1D*)file->Get("h_mc_all_eta_minv"));
    fh_mc_rec_eta_minv  ->Add((TH1D*)file->Get("h_mc_rec_eta_minv"));
    fh_rich_id_eta_minv ->Add((TH1D*)file->Get("h_rich_id_eta_minv"));
    fh_gcut_eta_minv    ->Add((TH1D*)file->Get("h_gcut_eta_minv"));
    fh_dcut_eta_minv    ->Add((TH1D*)file->Get("h_dcut_eta_minv"));
    fh_tpcut_eta_minv   ->Add((TH1D*)file->Get("h_tpcut_eta_minv"));
    fh_ptcut_eta_minv   ->Add((TH1D*)file->Get("h_ptcut_eta_minv"));
    fh_tcut_eta_minv    ->Add((TH1D*)file->Get("h_tcut_eta_minv"));
    fh_pi0cut_eta_minv  ->Add((TH1D*)file->Get("h_pi0cut_eta_minv"));
    // pi0
    fh_mc_all_pi0_minv  ->Add((TH1D*)file->Get("h_mc_all_pi0_minv"));
    fh_mc_rec_pi0_minv  ->Add((TH1D*)file->Get("h_mc_rec_pi0_minv"));
    fh_rich_id_pi0_minv ->Add((TH1D*)file->Get("h_rich_id_pi0_minv"));
    fh_gcut_pi0_minv    ->Add((TH1D*)file->Get("h_gcut_pi0_minv"));
    fh_dcut_pi0_minv    ->Add((TH1D*)file->Get("h_dcut_pi0_minv"));
    fh_tpcut_pi0_minv   ->Add((TH1D*)file->Get("h_tpcut_pi0_minv"));
    fh_ptcut_pi0_minv   ->Add((TH1D*)file->Get("h_ptcut_pi0_minv"));
    fh_tcut_pi0_minv    ->Add((TH1D*)file->Get("h_tcut_pi0_minv"));
    fh_pi0cut_pi0_minv  ->Add((TH1D*)file->Get("h_pi0cut_pi0_minv"));
    fh_pi0cut_fake_pi0_minv ->Add((TH1D*)file->Get("h_pi0cut_fake_pi0_minv"));
    fh_pi0cut_true_pi0_minv ->Add((TH1D*)file->Get("h_pi0cut_true_pi0_minv"));
    // gamma
    fh_mc_all_gamma_minv ->Add((TH1D*)file->Get("h_mc_all_gamma_minv"));
    fh_mc_rec_gamma_minv ->Add((TH1D*)file->Get("h_mc_rec_gamma_minv"));
    fh_rich_id_gamma_minv ->Add((TH1D*)file->Get("h_rich_id_gamma_minv"));
    fh_gcut_gamma_minv    ->Add((TH1D*)file->Get("h_gcut_gamma_minv"));
    fh_dcut_gamma_minv    ->Add((TH1D*)file->Get("h_dcut_gamma_minv"));
    fh_tpcut_gamma_minv   ->Add((TH1D*)file->Get("h_tpcut_gamma_minv"));
    fh_ptcut_gamma_minv   ->Add((TH1D*)file->Get("h_ptcut_gamma_minv"));
    fh_tcut_gamma_minv    ->Add((TH1D*)file->Get("h_tcut_gamma_minv"));
    fh_pi0cut_gamma_minv  ->Add((TH1D*)file->Get("h_pi0cut_gamma_minv"));

    // pair pt-y ----------------------------------------------------------------
    fh_mc_all_pty  ->Add((TH2D*)file->Get("h_mc_all_pty"));
    fh_mc_rec_pty  ->Add((TH2D*)file->Get("h_mc_rec_pty"));
    fh_rich_id_pty ->Add((TH2D*)file->Get("h_rich_id_pty"));
    fh_gcut_pty    ->Add((TH2D*)file->Get("h_gcut_pty"));
    fh_dcut_pty    ->Add((TH2D*)file->Get("h_dcut_pty"));
    fh_tpcut_pty   ->Add((TH2D*)file->Get("h_tpcut_pty"));
    fh_ptcut_pty   ->Add((TH2D*)file->Get("h_ptcut_pty"));
    fh_tcut_pty    ->Add((TH2D*)file->Get("h_tcut_pty"));
    fh_pi0cut_pty  ->Add((TH2D*)file->Get("h_pi0cut_pty"));
    // omega
    fh_mc_all_omega_pty ->Add((TH2D*)file->Get("h_mc_all_omega_pty"));
    fh_mc_rec_omega_pty ->Add((TH2D*)file->Get("h_mc_rec_omega_pty"));
    fh_rich_id_omega_pty ->Add((TH2D*)file->Get("h_rich_id_omega_pty"));
    fh_gcut_omega_pty   ->Add((TH2D*)file->Get("h_gcut_omega_pty"));
    fh_dcut_omega_pty   ->Add((TH2D*)file->Get("h_dcut_omega_pty"));
    fh_tpcut_omega_pty  ->Add((TH2D*)file->Get("h_tpcut_omega_pty"));
    fh_ptcut_omega_pty  ->Add((TH2D*)file->Get("h_ptcut_omega_pty"));
    fh_tcut_omega_pty   ->Add((TH2D*)file->Get("h_tcut_omega_pty"));
    fh_pi0cut_omega_pty ->Add((TH2D*)file->Get("h_pi0cut_omega_pty"));
    // eta
    fh_mc_all_eta_pty  ->Add((TH2D*)file->Get("h_mc_all_eta_pty"));
    fh_mc_rec_eta_pty  ->Add((TH2D*)file->Get("h_mc_rec_eta_pty"));
    fh_rich_id_eta_pty ->Add((TH2D*)file->Get("h_rich_id_eta_pty"));
    fh_gcut_eta_pty    ->Add((TH2D*)file->Get("h_gcut_eta_pty"));
    fh_dcut_eta_pty    ->Add((TH2D*)file->Get("h_dcut_eta_pty"));
    fh_tpcut_eta_pty   ->Add((TH2D*)file->Get("h_tpcut_eta_pty"));
    fh_ptcut_eta_pty   ->Add((TH2D*)file->Get("h_ptcut_eta_pty"));
    fh_tcut_eta_pty    ->Add((TH2D*)file->Get("h_tcut_eta_pty"));
    fh_pi0cut_eta_pty  ->Add((TH2D*)file->Get("h_pi0cut_eta_pty"));
    // pi0
    fh_mc_all_pi0_pty  ->Add((TH2D*)file->Get("h_mc_all_pi0_pty"));
    fh_mc_rec_pi0_pty  ->Add((TH2D*)file->Get("h_mc_rec_pi0_pty"));
    fh_rich_id_pi0_pty ->Add((TH2D*)file->Get("h_rich_id_pi0_pty"));
    fh_gcut_pi0_pty    ->Add((TH2D*)file->Get("h_gcut_pi0_pty"));
    fh_dcut_pi0_pty    ->Add((TH2D*)file->Get("h_dcut_pi0_pty"));
    fh_tpcut_pi0_pty   ->Add((TH2D*)file->Get("h_tpcut_pi0_pty"));
    fh_ptcut_pi0_pty   ->Add((TH2D*)file->Get("h_ptcut_pi0_pty"));
    fh_tcut_pi0_pty    ->Add((TH2D*)file->Get("h_tcut_pi0_pty"));
    fh_pi0cut_pi0_pty  ->Add((TH2D*)file->Get("h_pi0cut_pi0_pty"));
    // gamma
    fh_mc_all_gamma_pty  ->Add((TH2D*)file->Get("h_mc_all_gamma_pty"));
    fh_mc_rec_gamma_pty  ->Add((TH2D*)file->Get("h_mc_rec_gamma_pty"));
    fh_rich_id_gamma_pty ->Add((TH2D*)file->Get("h_rich_id_gamma_pty"));
    fh_gcut_gamma_pty    ->Add((TH2D*)file->Get("h_gcut_gamma_pty"));
    fh_dcut_gamma_pty    ->Add((TH2D*)file->Get("h_dcut_gamma_pty"));
    fh_tpcut_gamma_pty   ->Add((TH2D*)file->Get("h_tpcut_gamma_pty"));
    fh_ptcut_gamma_pty   ->Add((TH2D*)file->Get("h_ptcut_gamma_pty"));
    fh_tcut_gamma_pty    ->Add((TH2D*)file->Get("h_tcut_gamma_pty"));
    fh_pi0cut_gamma_pty  ->Add((TH2D*)file->Get("h_pi0cut_gamma_pty"));


    fh_rich_id_mult ->Add((TH2D*)file->Get("h_rich_id_mult"));
    fh_gcut_mult    ->Add((TH2D*)file->Get("h_gcut_mult"));
    fh_dcut_mult    ->Add((TH2D*)file->Get("h_dcut_mult"));
    fh_tpcut_mult   ->Add((TH2D*)file->Get("h_tpcut_mult"));
    fh_ptcut_mult   ->Add((TH2D*)file->Get("h_ptcut_mult"));
    fh_tcut_mult    ->Add((TH2D*)file->Get("h_tcut_mult"));
    fh_pi0cut_mult  ->Add((TH2D*)file->Get("h_pi0cut_mult"));

    fh_mom_rich ->Add((TH1D*)file->Get("h_mom_rich"));
 //   fh_mom_trd  ->Add((TH1D*)file->Get("h_mom_trd"));
    fh_mom_tof  ->Add((TH1D*)file->Get("h_mom_tof"));

    fh_mom_w_sts  ->Add((TH1D*)file->Get("h_mom_w_sts"));
    fh_mom_w_rich ->Add((TH1D*)file->Get("h_mom_w_rich"));
//    fh_mom_w_trd  ->Add((TH1D*)file->Get("h_mom_w_trd"));
    fh_mom_w_tof  ->Add((TH1D*)file->Get("h_mom_w_tof"));

    fh_mom_e_sts  ->Add((TH1D*)file->Get("h_mom_e_sts"));
    fh_mom_e_rich ->Add((TH1D*)file->Get("h_mom_e_rich"));
//    fh_mom_e_trd  ->Add((TH1D*)file->Get("h_mom_e_trd"));
    fh_mom_e_tof  ->Add((TH1D*)file->Get("h_mom_e_tof"));

    // background cocktail
    fh_rich_id_pi0_pi0_minv    ->Add((TH1D*)file->Get("h_rich_id_pi0_pi0_minv"));
    fh_rich_id_pi0_gamma_minv  ->Add((TH1D*)file->Get("h_rich_id_pi0_gamma_minv"));
    fh_rich_id_gamma_gamma_minv->Add((TH1D*)file->Get("h_rich_id_gamma_gamma_minv"));
    fh_rich_id_eta_gamma_minv  ->Add((TH1D*)file->Get("h_rich_id_eta_gamma_minv"));
    fh_rich_id_eta_eta_minv    ->Add((TH1D*)file->Get("h_rich_id_eta_eta_minv"));
    fh_rich_id_eta_pi0_minv    ->Add((TH1D*)file->Get("h_rich_id_eta_pi0_minv"));

//    fh_trd_id_pi0_pi0_minv    ->Add((TH1D*)file->Get("h_trd_id_pi0_pi0_minv"));
//   fh_trd_id_pi0_gamma_minv  ->Add((TH1D*)file->Get("h_trd_id_pi0_gamma_minv"));
//    fh_trd_id_gamma_gamma_minv->Add((TH1D*)file->Get("h_trd_id_gamma_gamma_minv"));
//    fh_trd_id_eta_gamma_minv  ->Add((TH1D*)file->Get("h_trd_id_eta_gamma_minv"));
//    fh_trd_id_eta_eta_minv    ->Add((TH1D*)file->Get("h_trd_id_eta_eta_minv"));
//    fh_trd_id_eta_pi0_minv    ->Add((TH1D*)file->Get("h_trd_id_eta_pi0_minv"));

    fh_tof_id_pi0_pi0_minv    ->Add((TH1D*)file->Get("h_tof_id_pi0_pi0_minv"));
    fh_tof_id_pi0_gamma_minv  ->Add((TH1D*)file->Get("h_tof_id_pi0_gamma_minv"));
    fh_tof_id_gamma_gamma_minv->Add((TH1D*)file->Get("h_tof_id_gamma_gamma_minv"));
    fh_tof_id_eta_gamma_minv  ->Add((TH1D*)file->Get("h_tof_id_eta_gamma_minv"));
    fh_tof_id_eta_eta_minv    ->Add((TH1D*)file->Get("h_tof_id_eta_eta_minv"));
    fh_tof_id_eta_pi0_minv    ->Add((TH1D*)file->Get("h_tof_id_eta_pi0_minv"));

    fh_pi0cut_pi0_pi0_minv    ->Add((TH1D*)file->Get("h_pi0cut_pi0_pi0_minv"));
    fh_pi0cut_pi0_gamma_minv  ->Add((TH1D*)file->Get("h_pi0cut_pi0_gamma_minv"));
    fh_pi0cut_gamma_gamma_minv->Add((TH1D*)file->Get("h_pi0cut_gamma_gamma_minv"));
    fh_pi0cut_eta_gamma_minv  ->Add((TH1D*)file->Get("h_pi0cut_eta_gamma_minv"));
    fh_pi0cut_eta_eta_minv    ->Add((TH1D*)file->Get("h_pi0cut_eta_eta_minv"));
    fh_pi0cut_eta_pi0_minv    ->Add((TH1D*)file->Get("h_pi0cut_eta_pi0_minv"));

    //----------------------------------------
    fh_rich_tof_id_cut_mom ->Add((TH1D*)file->Get("h_rich_tof_id_cut_mom"));


    file->Close();
  }

  TFile* file_out = new TFile("/lustre/cbm/user/slebedev/all/analysis.all.root", "RECREATE");

/*
TFile* file_out = new TFile("/lustre/cbm/user/ebelolap/glue/70_field_10gev/10gev.centr.all.spectra.root", "RECREATE");
*/

  // ----------- Scale histogramms ------------------------------

  // --- Normalize histogramms ---
  Double_t cNev = 1./(Double_t)nEvents;

  fh_chi2_prim_all->Scale(cNev);
    fh_chi2_prim_e->Scale(cNev);
    fh_mc_all_zr->Scale(cNev);

    fh_mc_all_lept_mom->Scale(cNev);
    fh_mc_rec_lept_mom->Scale(cNev);
    fh_rich_id_lept_mom->Scale(cNev);
    fh_rich_id_ep_mom->Scale(cNev);
    fh_rich_id_em_mom->Scale(cNev);

    fh_mc_all_mom->Scale(cNev);
    fh_mc_rec_mom->Scale(cNev);
    fh_rich_id_mom->Scale(cNev);
    fh_rich_id_no_cut_mom->Scale(cNev);
    fh_rich_id_mom_cut_mom->Scale(cNev);
   // fh_rich_id_dist_cut_mom->Scale(cNev);
    fh_rich_id_ann_cut_mom->Scale(cNev);
 //   fh_rich_id_xy_cut_mom->Scale(cNev);
 //   fh_rich_id_axisAB_cut_mom->Scale(cNev);

    fh_mc_all_omega_mom->Scale(cNev);
    fh_mc_rec_omega_mom->Scale(cNev);
    fh_rich_id_omega_mom->Scale(cNev);

    fh_mc_all_eta_mom->Scale(cNev);
    fh_mc_rec_eta_mom->Scale(cNev);
    fh_rich_id_eta_mom->Scale(cNev);

    fh_mc_all_pi0_mom->Scale(cNev);
    fh_mc_rec_pi0_mom->Scale(cNev);
    fh_rich_id_pi0_mom->Scale(cNev);

    fh_mc_all_gamma_mom->Scale(cNev);
    fh_mc_rec_gamma_mom->Scale(cNev);
    fh_rich_id_gamma_mom->Scale(cNev);

    fh_mc_all_theta->Scale(cNev);
    fh_mc_rec_theta->Scale(cNev);
    fh_rich_id_theta->Scale(cNev);

    fh_mc_all_omega_theta->Scale(cNev);
    fh_mc_rec_omega_theta->Scale(cNev);
    fh_rich_id_omega_theta->Scale(cNev);

    fh_mc_all_eta_theta->Scale(cNev);
    fh_mc_rec_eta_theta->Scale(cNev);
    fh_rich_id_eta_theta->Scale(cNev);

    fh_mc_all_pi0_theta->Scale(cNev);
    fh_mc_rec_pi0_theta->Scale(cNev);
    fh_rich_id_pi0_theta->Scale(cNev);

    fh_mc_all_gamma_theta->Scale(cNev);
    fh_mc_rec_gamma_theta->Scale(cNev);
    fh_rich_id_gamma_theta->Scale(cNev);

  //  fh_rich_ring_dist->Scale(cNev);
//    fh_rich_ring_dist_e->Scale(cNev);
//    fh_rich_dist_mom->Scale(cNev);
//    fh_rich_dist_mom_e->Scale(cNev);
//    fh_rich_dist_mom_cut->Scale(cNev);
//    fh_selectNN_true_e->Scale(cNev);
//    fh_selectNN_wrong_e->Scale(cNev);
//    fh_rich_ring_xy->Scale(cNev);
//    fh_rich_ring_xy_e->Scale(cNev);
//    fh_rich_ring_xy_cut->Scale(cNev);
 //   fh_rich_ring_radi->Scale(cNev);
 //   fh_rich_ring_radi_e->Scale(cNev);
    fh_rich_ring_axisA_mom_all->Scale(cNev);
 //   fh_rich_ring_axisA_mom->Scale(cNev);
  //  fh_rich_ring_axisA->Scale(cNev);
  //  fh_rich_ring_axisA_mom_e->Scale(cNev);
  //  fh_rich_ring_axisA_e->Scale(cNev);
  //  fh_rich_ring_axisB_mom_all->Scale(cNev);
  //  fh_rich_ring_axisB_mom->Scale(cNev);
  //  fh_rich_ring_axisB->Scale(cNev);
  //  fh_rich_ring_axisB_mom_e->Scale(cNev);
  //  fh_rich_ring_axisB_e->Scale(cNev);

//    fh_trd_eloss->Scale(cNev);
//    fh_trd_eloss_e->Scale(cNev);
 //   fh_trd_ann->Scale(cNev);
//    fh_trd_ann_e->Scale(cNev);
//    fh_trd_ann_mom->Scale(cNev);
 //   fh_trd_ann_mom_e->Scale(cNev);
    fh_tof_m2_mom->Scale(cNev);
    fh_tof_m2_mom_zoom->Scale(cNev);
    fh_tof_m2_mom_zoom_cut->Scale(cNev);

    fh_dsts_bg->Scale(cNev);
    fh_dsts_omega->Scale(cNev);
    fh_dsts_pi0->Scale(cNev);
    fh_dsts_gamma->Scale(cNev);
    fh_dsts_mom_bg->Scale(cNev);
    fh_dsts_mom_omega->Scale(cNev);
    fh_dsts_mom_pi0->Scale(cNev);
    fh_dsts_mom_gamma->Scale(cNev);

    fh_dcut_theta_mom_omega->Scale(cNev);
    fh_dcut_theta_mom_omega_true->Scale(cNev);
    fh_dcut_theta_mom_pi0->Scale(cNev);
    fh_dcut_theta_mom_gamma->Scale(cNev);
    fh_dcut_theta_mom_gamma_true->Scale(cNev);
    fh_dcut_theta_mom_bg->Scale(cNev);
    fh_tpcut_pt_lept_omega->Scale(cNev);
    fh_tpcut_pt_lept_bg->Scale(cNev);
    fh_ptcut_theta_omega->Scale(cNev);
    fh_ptcut_theta_omega_true->Scale(cNev);
    fh_ptcut_theta_pi0->Scale(cNev);
    fh_ptcut_theta_gamma->Scale(cNev);
    fh_ptcut_theta_bg->Scale(cNev);

    fh_mc_all_minv->Scale(cNev);
    fh_mc_rec_minv->Scale(cNev);
    fh_rich_id_minv->Scale(cNev);
    fh_gcut_minv->Scale(cNev);
    fh_dcut_minv->Scale(cNev);
    fh_tpcut_minv->Scale(cNev);
    fh_ptcut_minv->Scale(cNev);
    fh_tcut_minv->Scale(cNev);
    fh_pi0cut_minv->Scale(cNev);

    fh_pi0cut_epep_minv->Scale(cNev);
    fh_pi0cut_emem_minv->Scale(cNev);

    fh_mc_all_pt_minv_omega->Scale(cNev);
    fh_mc_all_pt_minv_pairs->Scale(cNev);
    fh_mc_rec_pt_minv_omega->Scale(cNev);
    fh_mc_rec_pt_minv_pairs->Scale(cNev);
    fh_rich_id_pt_minv_omega->Scale(cNev);
    fh_rich_id_pt_minv_pairs->Scale(cNev);
    fh_tpcut_pt_minv_omega->Scale(cNev);
    fh_tpcut_pt_minv_pairs->Scale(cNev);
    fh_pi0cut_pt_minv_omega->Scale(cNev);
    fh_pi0cut_pt_minv_pairs->Scale(cNev);

    fh_rich_id_mc_minv->Scale(cNev);
    fh_gcut_mc_minv->Scale(cNev);
    fh_dcut_mc_minv->Scale(cNev);
    fh_tpcut_mc_minv->Scale(cNev);
    fh_ptcut_mc_minv->Scale(cNev);
    fh_tcut_mc_minv->Scale(cNev);
    fh_pi0cut_mc_minv->Scale(cNev);
    fh_mc_all_omega_bg_minv->Scale(cNev);
    fh_mc_rec_omega_bg_minv->Scale(cNev);
    fh_rich_id_omega_bg_minv->Scale(cNev);
    fh_gcut_omega_bg_minv->Scale(cNev);
    fh_dcut_omega_bg_minv->Scale(cNev);
    fh_tpcut_omega_bg_minv->Scale(cNev);
    fh_ptcut_omega_bg_minv->Scale(cNev);
    fh_tcut_omega_bg_minv->Scale(cNev);
    fh_pi0cut_omega_bg_minv->Scale(cNev);
    fh_rich_id_mc_omega_bg_minv->Scale(cNev);
    fh_gcut_mc_omega_bg_minv->Scale(cNev);
    fh_dcut_mc_omega_bg_minv->Scale(cNev);
    fh_tpcut_mc_omega_bg_minv->Scale(cNev);
    fh_ptcut_mc_omega_bg_minv->Scale(cNev);
    fh_tcut_mc_omega_bg_minv->Scale(cNev);
    fh_pi0cut_mc_omega_bg_minv->Scale(cNev);

    fh_ptcut_omega_minv->Scale(cNev);
    fh_mc_all_omega_minv->Scale(cNev);
    fh_mc_rec_omega_minv->Scale(cNev);
    fh_rich_id_omega_minv->Scale(cNev);
    fh_gcut_omega_minv->Scale(cNev);
    fh_dcut_omega_minv->Scale(cNev);
    fh_tpcut_omega_minv->Scale(cNev);
    fh_tcut_omega_minv->Scale(cNev);
    fh_pi0cut_omega_minv->Scale(cNev);

    fh_mc_all_eta_minv->Scale(cNev);
    fh_mc_rec_eta_minv->Scale(cNev);
    fh_rich_id_eta_minv->Scale(cNev);
    fh_gcut_eta_minv->Scale(cNev);
    fh_dcut_eta_minv->Scale(cNev);
    fh_tpcut_eta_minv->Scale(cNev);
    fh_ptcut_eta_minv->Scale(cNev);
    fh_tcut_eta_minv->Scale(cNev);
    fh_pi0cut_eta_minv->Scale(cNev);

    fh_mc_all_pi0_minv->Scale(cNev);
    fh_mc_rec_pi0_minv->Scale(cNev);
    fh_rich_id_pi0_minv->Scale(cNev);
    fh_gcut_pi0_minv->Scale(cNev);
    fh_dcut_pi0_minv->Scale(cNev);
    fh_tpcut_pi0_minv->Scale(cNev);
    fh_ptcut_pi0_minv->Scale(cNev);
    fh_tcut_pi0_minv->Scale(cNev);
    fh_pi0cut_pi0_minv->Scale(cNev);
    fh_pi0cut_fake_pi0_minv->Scale(cNev);
    fh_pi0cut_true_pi0_minv->Scale(cNev);

    fh_mc_all_gamma_minv->Scale(cNev);
    fh_mc_rec_gamma_minv->Scale(cNev);
    fh_rich_id_gamma_minv->Scale(cNev);
    fh_gcut_gamma_minv->Scale(cNev);
    fh_dcut_gamma_minv->Scale(cNev);
    fh_tpcut_gamma_minv->Scale(cNev);
    fh_ptcut_gamma_minv->Scale(cNev);
    fh_tcut_gamma_minv->Scale(cNev);
    fh_pi0cut_gamma_minv->Scale(cNev);

    fh_mc_all_pty->Scale(cNev);
    fh_mc_rec_pty->Scale(cNev);
    fh_rich_id_pty->Scale(cNev);
    fh_gcut_pty->Scale(cNev);
    fh_dcut_pty->Scale(cNev);
    fh_tpcut_pty->Scale(cNev);
    fh_ptcut_pty->Scale(cNev);
    fh_tcut_pty->Scale(cNev);
    fh_pi0cut_pty->Scale(cNev);

    fh_mc_all_omega_pty->Scale(cNev);
    fh_mc_rec_omega_pty->Scale(cNev);
    fh_rich_id_omega_pty->Scale(cNev);
    fh_gcut_omega_pty->Scale(cNev);
    fh_dcut_omega_pty->Scale(cNev);
    fh_tpcut_omega_pty->Scale(cNev);
    fh_ptcut_omega_pty->Scale(cNev);
    fh_tcut_omega_pty->Scale(cNev);
    fh_pi0cut_omega_pty->Scale(cNev);

    fh_mc_all_eta_pty->Scale(cNev);
    fh_mc_rec_eta_pty->Scale(cNev);
    fh_rich_id_eta_pty->Scale(cNev);
    fh_gcut_eta_pty->Scale(cNev);
    fh_dcut_eta_pty->Scale(cNev);
    fh_tpcut_eta_pty->Scale(cNev);
    fh_ptcut_eta_pty->Scale(cNev);
    fh_tcut_eta_pty->Scale(cNev);
    fh_pi0cut_eta_pty->Scale(cNev);

    fh_mc_all_pi0_pty->Scale(cNev);
    fh_mc_rec_pi0_pty->Scale(cNev);
    fh_rich_id_pi0_pty->Scale(cNev);
    fh_gcut_pi0_pty->Scale(cNev);
    fh_dcut_pi0_pty->Scale(cNev);
    fh_tpcut_pi0_pty->Scale(cNev);
    fh_ptcut_pi0_pty->Scale(cNev);
    fh_tcut_pi0_pty->Scale(cNev);
    fh_pi0cut_pi0_pty->Scale(cNev);

    fh_mc_all_gamma_pty->Scale(cNev);
    fh_mc_rec_gamma_pty->Scale(cNev);
    fh_rich_id_gamma_pty->Scale(cNev);
    fh_gcut_gamma_pty->Scale(cNev);
    fh_dcut_gamma_pty->Scale(cNev);
    fh_tpcut_gamma_pty->Scale(cNev);
    fh_ptcut_gamma_pty->Scale(cNev);
    fh_tcut_gamma_pty->Scale(cNev);
    fh_pi0cut_gamma_pty->Scale(cNev);

    fh_mom_rich->Scale(cNev);
 //   fh_mom_trd->Scale(cNev);
    fh_mom_tof->Scale(cNev);

    fh_mom_w_sts->Scale(cNev);
    fh_mom_w_rich->Scale(cNev);
 //   fh_mom_w_trd->Scale(cNev);
    fh_mom_w_tof->Scale(cNev);

    fh_mom_e_sts->Scale(cNev);
    fh_mom_e_rich->Scale(cNev);
//    fh_mom_e_trd->Scale(cNev);
    fh_mom_e_tof->Scale(cNev);

    fh_rich_id_pi0_pi0_minv->Scale(cNev);
    fh_rich_id_pi0_gamma_minv->Scale(cNev);
    fh_rich_id_gamma_gamma_minv->Scale(cNev);
    fh_rich_id_eta_gamma_minv->Scale(cNev);
    fh_rich_id_eta_eta_minv->Scale(cNev);
    fh_rich_id_eta_pi0_minv->Scale(cNev);

  //  fh_trd_id_pi0_pi0_minv->Scale(cNev);
  //  fh_trd_id_pi0_gamma_minv->Scale(cNev);
  //  fh_trd_id_gamma_gamma_minv->Scale(cNev);
  //  fh_trd_id_eta_gamma_minv->Scale(cNev);
  //  fh_trd_id_eta_eta_minv->Scale(cNev);
  //  fh_trd_id_eta_pi0_minv->Scale(cNev);

    fh_tof_id_pi0_pi0_minv->Scale(cNev);
    fh_tof_id_pi0_gamma_minv->Scale(cNev);
    fh_tof_id_gamma_gamma_minv->Scale(cNev);
    fh_tof_id_eta_gamma_minv->Scale(cNev);
    fh_tof_id_eta_eta_minv->Scale(cNev);
    fh_tof_id_eta_pi0_minv->Scale(cNev);

    fh_pi0cut_pi0_pi0_minv->Scale(cNev);
    fh_pi0cut_pi0_gamma_minv->Scale(cNev);
    fh_pi0cut_gamma_gamma_minv->Scale(cNev);
    fh_pi0cut_eta_gamma_minv->Scale(cNev);
    fh_pi0cut_eta_eta_minv->Scale(cNev);
    fh_pi0cut_eta_pi0_minv->Scale(cNev);

 fh_rich_tof_id_cut_mom->Scale(cNev);



 // --- Write histogramms ---
     fh_chi2_prim_all->Write();
    fh_chi2_prim_e->Write();
    fh_mc_all_zr->Write();

    fh_mc_all_lept_mom->Write();
    fh_mc_rec_lept_mom->Write();
    fh_rich_id_lept_mom->Write();
    fh_rich_id_ep_mom->Write();
    fh_rich_id_em_mom->Write();

    fh_rich_id_no_cut_mom->Write();
    fh_rich_id_mom_cut_mom->Write();
//    fh_rich_id_dist_cut_mom->Write();
    fh_rich_id_ann_cut_mom->Write();
//    fh_rich_id_xy_cut_mom->Write();
//    fh_rich_id_axisAB_cut_mom->Write();

    fh_mc_all_omega_mom->Write();
    fh_mc_rec_omega_mom->Write();
    fh_rich_id_omega_mom->Write();

    fh_mc_all_eta_mom->Write();
    fh_mc_rec_eta_mom->Write();
    fh_rich_id_eta_mom->Write();

    fh_mc_all_pi0_mom->Write();
    fh_mc_rec_pi0_mom->Write();
    fh_rich_id_pi0_mom->Write();

    fh_mc_all_gamma_mom->Write();
    fh_mc_rec_gamma_mom->Write();
    fh_rich_id_gamma_mom->Write();

    fh_mc_all_mom->Write();
    fh_mc_rec_mom->Write();
    fh_rich_id_mom->Write();

    fh_mc_all_theta->Write();
    fh_mc_rec_theta->Write();
    fh_rich_id_theta->Write();

    fh_mc_all_omega_theta->Write();
    fh_mc_rec_omega_theta->Write();
    fh_rich_id_omega_theta->Write();

    fh_mc_all_eta_theta->Write();
    fh_mc_rec_eta_theta->Write();
    fh_rich_id_eta_theta->Write();

    fh_mc_all_pi0_theta->Write();
    fh_mc_rec_pi0_theta->Write();
    fh_rich_id_pi0_theta->Write();

    fh_mc_all_gamma_theta->Write();
    fh_mc_rec_gamma_theta->Write();
    fh_rich_id_gamma_theta->Write();
/*
    fh_rich_ring_dist->Write();
    fh_rich_ring_dist_e->Write();
    fh_rich_dist_mom->Write();
    fh_rich_dist_mom_e->Write();
    fh_rich_dist_mom_cut->Write();
    fh_selectNN_true_e->Write();
    fh_selectNN_wrong_e->Write();
    fh_rich_ring_xy->Write();
    fh_rich_ring_xy_e->Write();
    fh_rich_ring_xy_cut->Write();
    fh_rich_ring_radi->Write();
    fh_rich_ring_radi_e->Write();*/
    fh_rich_ring_axisA_mom_all->Write();
 //   fh_rich_ring_axisA_mom->Write();
 /*   fh_rich_ring_axisA->Write();
    fh_rich_ring_axisA_mom_e->Write();
    fh_rich_ring_axisA_e->Write();
    fh_rich_ring_axisB_mom_all->Write();
    fh_rich_ring_axisB_mom->Write();
    fh_rich_ring_axisB->Write();
    fh_rich_ring_axisB_mom_e->Write();
    fh_rich_ring_axisB_e->Write();
*/
//    fh_trd_eloss->Write();
//    fh_trd_eloss_e->Write();
//    fh_trd_ann->Write();
//    fh_trd_ann_e->Write();
//    fh_trd_ann_mom->Write();
//    fh_trd_ann_mom_e->Write();
    fh_tof_m2_mom->Write();
    fh_tof_m2_mom_zoom->Write();
    fh_tof_m2_mom_zoom_cut->Write();

    fh_dsts_bg->Write();
    fh_dsts_omega->Write();
    fh_dsts_pi0->Write();
    fh_dsts_gamma->Write();
    fh_dsts_mom_bg->Write();
    fh_dsts_mom_omega->Write();
    fh_dsts_mom_pi0->Write();
    fh_dsts_mom_gamma->Write();

    fh_dcut_theta_mom_omega->Write();
    fh_dcut_theta_mom_omega_true->Write();
    fh_dcut_theta_mom_pi0->Write();
    fh_dcut_theta_mom_gamma->Write();
    fh_dcut_theta_mom_gamma_true->Write();
    fh_dcut_theta_mom_bg->Write();
    fh_tpcut_pt_lept_omega->Write();
    fh_tpcut_pt_lept_bg->Write();

    fh_ptcut_theta_omega->Write();
    fh_ptcut_theta_omega_true->Write();
    fh_ptcut_theta_pi0->Write();
    fh_ptcut_theta_gamma->Write();
    fh_ptcut_theta_bg->Write();

    fh_mc_all_minv->Write();
    fh_mc_rec_minv->Write();
    fh_rich_id_minv->Write();
    fh_gcut_minv->Write();
    fh_dcut_minv->Write();
    fh_tpcut_minv->Write();
    fh_ptcut_minv->Write();
    fh_tcut_minv->Write();
    fh_pi0cut_minv->Write();

    fh_pi0cut_epep_minv->Write();
    fh_pi0cut_emem_minv->Write();

    fh_mc_all_pt_minv_omega->Write();
    fh_mc_all_pt_minv_pairs->Write();
    fh_mc_rec_pt_minv_omega->Write();
    fh_mc_rec_pt_minv_pairs->Write();
    fh_rich_id_pt_minv_omega->Write();
    fh_rich_id_pt_minv_pairs->Write();
    fh_tpcut_pt_minv_omega->Write();
    fh_tpcut_pt_minv_pairs->Write();
    fh_pi0cut_pt_minv_omega->Write();
    fh_pi0cut_pt_minv_pairs->Write();

    fh_rich_id_mc_minv->Write();
    fh_gcut_mc_minv->Write();
    fh_dcut_mc_minv->Write();
    fh_tpcut_mc_minv->Write();
    fh_ptcut_mc_minv->Write();
    fh_tcut_mc_minv->Write();
    fh_pi0cut_mc_minv->Write();
    fh_mc_all_omega_bg_minv->Write();
    fh_mc_rec_omega_bg_minv->Write();
    fh_rich_id_omega_bg_minv->Write();
    fh_gcut_omega_bg_minv->Write();
    fh_dcut_omega_bg_minv->Write();
    fh_tpcut_omega_bg_minv->Write();
    fh_ptcut_omega_bg_minv->Write();
    fh_tcut_omega_bg_minv->Write();
    fh_pi0cut_omega_bg_minv->Write();
    fh_rich_id_mc_omega_bg_minv->Write();
    fh_gcut_mc_omega_bg_minv->Write();
    fh_dcut_mc_omega_bg_minv->Write();
    fh_tpcut_mc_omega_bg_minv->Write();
    fh_ptcut_mc_omega_bg_minv->Write();
    fh_tcut_mc_omega_bg_minv->Write();
    fh_pi0cut_mc_omega_bg_minv->Write();

    fh_mc_all_omega_minv->Write();
    fh_mc_rec_omega_minv->Write();
    fh_rich_id_omega_minv->Write();
    fh_gcut_omega_minv->Write();
    fh_dcut_omega_minv->Write();
    fh_tpcut_omega_minv->Write();
    fh_ptcut_omega_minv->Write();
    fh_tcut_omega_minv->Write();
    fh_pi0cut_omega_minv->Write();

    fh_mc_all_eta_minv->Write();
    fh_mc_rec_eta_minv->Write();
    fh_rich_id_eta_minv->Write();
    fh_gcut_eta_minv->Write();
    fh_dcut_eta_minv->Write();
    fh_tpcut_eta_minv->Write();
    fh_ptcut_eta_minv->Write();
    fh_tcut_eta_minv->Write();
    fh_pi0cut_eta_minv->Write();

    fh_mc_all_pi0_minv->Write();
    fh_mc_rec_pi0_minv->Write();
    fh_rich_id_pi0_minv->Write();
    fh_gcut_pi0_minv->Write();
    fh_dcut_pi0_minv->Write();
    fh_tpcut_pi0_minv->Write();
    fh_ptcut_pi0_minv->Write();
    fh_tcut_pi0_minv->Write();
    fh_pi0cut_pi0_minv->Write();
    fh_pi0cut_fake_pi0_minv->Write();
    fh_pi0cut_true_pi0_minv->Write();

    fh_mc_all_gamma_minv->Write();
    fh_mc_rec_gamma_minv->Write();
    fh_rich_id_gamma_minv->Write();
    fh_gcut_gamma_minv->Write();
    fh_dcut_gamma_minv->Write();
    fh_tpcut_gamma_minv->Write();
    fh_ptcut_gamma_minv->Write();
    fh_tcut_gamma_minv->Write();
    fh_pi0cut_gamma_minv->Write();

    fh_mc_all_pty->Write();
    fh_mc_rec_pty->Write();
    fh_rich_id_pty->Write();
    fh_gcut_pty->Write();
    fh_dcut_pty->Write();
    fh_tpcut_pty->Write();
    fh_ptcut_pty->Write();
    fh_tcut_pty->Write();
    fh_pi0cut_pty->Write();

    fh_mc_all_omega_pty->Write();
    fh_mc_rec_omega_pty->Write();
    fh_rich_id_omega_pty->Write();
    fh_gcut_omega_pty->Write();
    fh_dcut_omega_pty->Write();
    fh_tpcut_omega_pty->Write();
    fh_ptcut_omega_pty->Write();
    fh_tcut_omega_pty->Write();
    fh_pi0cut_omega_pty->Write();

    fh_mc_all_eta_pty->Write();
    fh_mc_rec_eta_pty->Write();
    fh_rich_id_eta_pty->Write();
    fh_gcut_eta_pty->Write();
    fh_dcut_eta_pty->Write();
    fh_tpcut_eta_pty->Write();
    fh_ptcut_eta_pty->Write();
    fh_tcut_eta_pty->Write();
    fh_pi0cut_eta_pty->Write();

    fh_mc_all_pi0_pty->Write();
    fh_mc_rec_pi0_pty->Write();
    fh_rich_id_pi0_pty->Write();
    fh_gcut_pi0_pty->Write();
    fh_dcut_pi0_pty->Write();
    fh_tpcut_pi0_pty->Write();
    fh_ptcut_pi0_pty->Write();
    fh_tcut_pi0_pty->Write();
    fh_pi0cut_pi0_pty->Write();

    fh_mc_all_gamma_pty->Write();
    fh_mc_rec_gamma_pty->Write();
    fh_rich_id_gamma_pty->Write();
    fh_gcut_gamma_pty->Write();
    fh_dcut_gamma_pty->Write();
    fh_tpcut_gamma_pty->Write();
    fh_ptcut_gamma_pty->Write();
    fh_tcut_gamma_pty->Write();
    fh_pi0cut_gamma_pty->Write();

    fh_rich_id_mult->Write();
    fh_gcut_mult->Write();
    fh_dcut_mult->Write();
    fh_tpcut_mult->Write();
    fh_ptcut_mult->Write();
    fh_tcut_mult->Write();
    fh_pi0cut_mult->Write();

    fh_mom_rich->Write();
//    fh_mom_trd->Write();
    fh_mom_tof->Write();

    fh_mom_w_sts->Write();
    fh_mom_w_rich->Write();
 //   fh_mom_w_trd->Write();
    fh_mom_w_tof->Write();

    fh_mom_e_sts->Write();
    fh_mom_e_rich->Write();
//    fh_mom_e_trd->Write();
    fh_mom_e_tof->Write();

    fh_rich_id_pi0_pi0_minv->Write();
    fh_rich_id_pi0_gamma_minv->Write();
    fh_rich_id_gamma_gamma_minv->Write();
    fh_rich_id_eta_gamma_minv->Write();
    fh_rich_id_eta_eta_minv->Write();
    fh_rich_id_eta_pi0_minv->Write();

//    fh_trd_id_pi0_pi0_minv->Write();
//    fh_trd_id_pi0_gamma_minv->Write();
//    fh_trd_id_gamma_gamma_minv->Write();
//    fh_trd_id_eta_gamma_minv->Write();
//    fh_trd_id_eta_eta_minv->Write();
//    fh_trd_id_eta_pi0_minv->Write();

    fh_tof_id_pi0_pi0_minv->Write();
    fh_tof_id_pi0_gamma_minv->Write();
    fh_tof_id_gamma_gamma_minv->Write();
    fh_tof_id_eta_gamma_minv->Write();
    fh_tof_id_eta_eta_minv->Write();
    fh_tof_id_eta_pi0_minv->Write();

    fh_pi0cut_pi0_pi0_minv->Write();
    fh_pi0cut_pi0_gamma_minv->Write();
    fh_pi0cut_gamma_gamma_minv->Write();
    fh_pi0cut_eta_gamma_minv->Write();
    fh_pi0cut_eta_eta_minv->Write();
    fh_pi0cut_eta_pi0_minv->Write();


 fh_rich_tof_id_cut_mom->Write();


  file_out->Close();
}
