void run_reco(Int_t nEvents = 1000)
{
   TTree::SetMaxTreeSize(90000000000);

	TString script = TString(gSystem->Getenv("SCRIPT"));
	TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");

	//gRandom->SetSeed(10);

	TString mcFile = "/d/cbm02/slebedev/rich/JUL09/auau.25gev.centr.0000.mc.root";
	TString parFile = "/d/cbm02/slebedev/rich/JUL09/auau.25gev.centr.0000.params.root";
	TString recoFile = "/d/cbm02/slebedev/rich/JUL09/auau.25gev.centr.0000.reco.root";
	TString delta = "no"; // if "yes" Delta electrons will be embedded
	TString deltaFile = "";

   TList *parFileList = new TList();
   TObjString stsDigiFile = parDir + "/sts/sts_v12b_std.digi.par"; // STS digi file
   TObjString trdDigiFile = parDir + "/trd/trd_v10b.digi.par"; // TRD digi file

   TString stsMatBudgetFileName = parDir + "/sts/sts_matbudget_v12b.root"; // Material budget file for L1 STS tracking

   TString resultDir = "recqa/";

	if (script == "yes") {
		mcFile = TString(gSystem->Getenv("MC_FILE"));
		parFile = TString(gSystem->Getenv("PAR_FILE"));
		recoFile = TString(gSystem->Getenv("RECO_FILE"));
		delta = TString(gSystem->Getenv("DELTA"));
		deltaFile = TString(gSystem->Getenv("DELTA_FILE"));

		stsDigiFile = TString(gSystem->Getenv("STS_DIGI"));
		trdDigiFile = TString(gSystem->Getenv("TRD_DIGI"));

                resultDir = TString(gSystem->Getenv("RESULT_DIR"));

		stsMatBudgetFileName = TString(gSystem->Getenv("STS_MATERIAL_BUDGET_FILE"));
	}

   parFileList->Add(&stsDigiFile);
   parFileList->Add(&trdDigiFile);

   TStopwatch timer;
   timer.Start();

	// ----  Load libraries   -------------------------------------------------
        gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/loadlibs.C");
	loadlibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/determine_setup.C");

	// -----   Reconstruction run   -------------------------------------------
	FairRunAna *run= new FairRunAna();
	if (mcFile != "") run->SetInputFile(mcFile);
	if (recoFile != "") run->SetOutputFile(recoFile);


    // =========================================================================
    // ===                     MVD local reconstruction                      ===
	// =========================================================================
	Bool_t useMvdInTracking = kFALSE;
	if (IsMvd(parFile)) {
	   useMvdInTracking = kTRUE;
      CbmMvdDigitizeL* mvdDigitizeL = new CbmMvdDigitizeL("CbmMvdDigitizeL", 0, 1);
      mvdDigitizeL->SetPixelSize(18.4); //should be 20x20
      mvdDigitizeL->SetEpiThickness(0.0014);
      //mvdDigitizeL->ShowDebugHistograms();
      //mvdDigitizeL->SetBgFileName(bgFile);
      //mvdDigitizeL->SetPileUp(0);
      if (delta == "yes"){
         mvdDigitizeL->SetDeltaName(deltaFile);
         mvdDigitizeL->SetDeltaEvents(1000); //1/1000 target
      }
      run->AddTask(mvdDigitizeL);

      CbmMvdFindHits* mvdFindHits = new CbmMvdFindHits("CbmMvdFindHits", 0, 1);
      mvdFindHits->SetSigmaNoise(11,kTRUE);
      //mvdFindHits->ShowDebugHistograms();
      mvdFindHits->SetAdcDynamic(150);
      mvdFindHits->SetAdcOffset(0);
      mvdFindHits->SetAdcBits(1);
      mvdFindHits->SetSeedThreshold(1); //75 ele
      mvdFindHits->SetNeighbourThreshold(1); //75 el
      run->AddTask(mvdFindHits);

      // MVD ideal
      //{
      //CbmMvdHitProducer* mvdHitProd = new CbmMvdHitProducer("MVDHitProducer", 0, 1);
      //run->AddTask(mvdHitProd);
      //}
	}

	// =========================================================================
	// ===                      STS local reconstruction                     ===
	// =========================================================================
    if (true){ // STS REAL
      Double_t threshold  =  4;
      Double_t noiseWidth =  0.1;
      Int_t    nofBits    = 20;
      Double_t minStep    =  0.01;
      Double_t StripDeadTime = 10.;

      Double_t threshold  =  4;
      Double_t noiseWidth =  0.01;
      Int_t    nofBits    = 12;
      Double_t electronsPerAdc    =  10;
      Double_t StripDeadTime = 0.1;
      CbmStsDigitize* stsDigitize = new CbmStsDigitize("STS Digitiser", 1);
      stsDigitize->SetRealisticResponse();
      stsDigitize->SetFrontThreshold (threshold);
      stsDigitize->SetBackThreshold  (threshold);
      stsDigitize->SetFrontNoiseWidth(noiseWidth);
      stsDigitize->SetBackNoiseWidth (noiseWidth);
      stsDigitize->SetFrontNofBits   (nofBits);
      stsDigitize->SetBackNofBits    (nofBits);
      stsDigitize->SetFrontNofElPerAdc(electronsPerAdc);
      stsDigitize->SetBackNofElPerAdc(electronsPerAdc);
      stsDigitize->SetStripDeadTime  (StripDeadTime);
      run->AddTask(stsDigitize);

      FairTask* stsClusterFinder = new CbmStsClusterFinder("CbmStsClusterFinder",1);
      run->AddTask(stsClusterFinder);

      FairTask* stsFindHits = new CbmStsFindHits(1);
      run->AddTask(stsFindHits);

      CbmStsMatchHits* stsMatchHits = new CbmStsMatchHits(1);
      run->AddTask(stsMatchHits);

    } else { // STS IDEAL RESPONSE
      FairTask* stsDigitize = new CbmStsIdealDigitize("CbmStsIdealDigitize", 1);
      run->AddTask(stsDigitize);

      FairTask* stsClusterFinder = new CbmStsClusterFinder("CbmStsClusterFinder",1);
      run->AddTask(stsClusterFinder);

      FairTask* stsFindHits = new CbmStsIdealFindHits(1);
      run->AddTask(stsFindHits);

      FairTask* stsMatchHits = new CbmStsIdealMatchHits(1);
      run->AddTask(stsMatchHits);
   }

	CbmKF* kalman = new CbmKF();
	run->AddTask(kalman);
	CbmL1* l1 = new CbmL1();
	l1->SetMaterialBudgetFileName(stsMatBudgetFileName);
//	CbmL1* l1 = new CbmL1("CbmL1",1, 3); //to fill L1 histo
	run->AddTask(l1);

	CbmStsTrackFinder* stsTrackFinder = new CbmL1StsTrackFinder();
	//Bool_t useMvdInTracking = kTRUE;
	FairTask* stsFindTracks = new CbmStsFindTracks(1, stsTrackFinder, useMvdInTracking);
	run->AddTask(stsFindTracks);

	FairTask* stsMatchTracks = new CbmStsMatchTracks(1);
	run->AddTask(stsMatchTracks);

	CbmStsTrackFitter* stsTrackFitter = new CbmStsKFTrackFitter();
	FairTask* stsFitTracks = new CbmStsFitTracks(stsTrackFitter, 1);
	run->AddTask(stsFitTracks);

	// =========================================================================
	// ===                     TRD local reconstruction                      ===
	// =========================================================================
	if (IsTrd(parFile)) {
		// Update of the values for the radiator F.U. 17.08.07
		Int_t trdNFoils = 130; // number of polyetylene foils
		Float_t trdDFoils = 0.0013; // thickness of 1 foil [cm]
		Float_t trdDGap = 0.02; // thickness of gap between foils [cm]
		Bool_t simpleTR = kTRUE; // use fast and simple version for TR production

		CbmTrdRadiator *radiator = new CbmTrdRadiator(simpleTR , trdNFoils, trdDFoils, trdDGap);

                CbmTrdHitProducerSmearing* trdHitProd = new CbmTrdHitProducerSmearing(radiator);
                run->AddTask(trdHitProd);
	}// isTRD

	// =========================================================================
	// ===                     TOF local reconstruction                      ===
	// =========================================================================
	if (IsTof(parFile)) {
		// ------   TOF hit producer   ---------------------------------------------
		CbmTofHitProducer* tofHitProd = new CbmTofHitProducer("CbmTofHitProducer", 1);
		run->AddTask(tofHitProd);
	} //isTof

	// =========================================================================
	// ===                        Global tracking                            ===
	// =========================================================================

	CbmLitFindGlobalTracks* finder = new CbmLitFindGlobalTracks();
	finder->SetTrackingType("branch");
	finder->SetMergerType("nearest_hit");
	run->AddTask(finder);

	// -----   Primary vertex finding   ---------------------------------------
	CbmPrimaryVertexFinder* pvFinder = new CbmPVFinderKF();
	CbmFindPrimaryVertex* findVertex = new CbmFindPrimaryVertex(pvFinder);
	run->AddTask(findVertex);
	// ------------------------------------------------------------------------

	if (IsTrd(parFile)) {
		CbmTrdMatchTracks* trdMatchTracks = new CbmTrdMatchTracks(1);
		run->AddTask(trdMatchTracks);

		CbmTrdSetTracksPidWkn* trdSetTracksPidTask =
				new CbmTrdSetTracksPidWkn("CbmTrdSetTracksPidWkn","CbmTrdSetTracksPidWkn");
		run->AddTask(trdSetTracksPidTask);

		CbmTrdSetTracksPidANN* trdSetTracksPidAnnTask =
				new	CbmTrdSetTracksPidANN("CbmTrdSetTracksPidANN","CbmTrdSetTracksPidANN");
		run->AddTask(trdSetTracksPidAnnTask);

		CbmTrdSetTracksPidLike* trdSetTracksPidLikeTask =
				new CbmTrdSetTracksPidLike("CbmTrdSetTracksPidLike","CbmTrdSetTracksPidLike");
		run->AddTask(trdSetTracksPidLikeTask);
	}//isTrd

    // =========================================================================
    // ===                        RICH reconstruction                        ===
    // =========================================================================
	if (IsRich(parFile)){
      CbmRichHitProducer* richHitProd  = new CbmRichHitProducer();
      richHitProd->SetDetectorType(4);
      richHitProd->SetNofNoiseHits(220);
      richHitProd->SetCollectionEfficiency(1.0);
      richHitProd->SetSigmaMirror(0.06);
      run->AddTask(richHitProd);

      CbmRichReconstruction* richReco = new CbmRichReconstruction();
      //richReco->SetFinderName("ideal");
      run->AddTask(richReco);

      // ------------------- RICH Ring matching  ---------------------------------
      CbmRichMatchRings* matchRings = new CbmRichMatchRings();
      run->AddTask(matchRings);

	}//isRich

   // Reconstruction Qa
   CbmLitTrackingQa* trackingQa = new CbmLitTrackingQa();
   trackingQa->SetMinNofPointsSts(4);
   trackingQa->SetUseConsecutivePointsInSts(true);
   trackingQa->SetMinNofPointsTrd(8);
   trackingQa->SetMinNofPointsMuch(10);
   trackingQa->SetMinNofPointsTof(1);
   trackingQa->SetQuota(0.7);
   trackingQa->SetMinNofHitsTrd(8);
   trackingQa->SetMinNofHitsMuch(10);
   trackingQa->SetVerbose(0);
   trackingQa->SetMinNofHitsRich(7);
   trackingQa->SetQuotaRich(0.6);
   trackingQa->SetPRange(30, 0., 3.);
   trackingQa->SetOutputDir(std::string(resultDir));
   run->AddTask(trackingQa);

   CbmLitFitQa* fitQa = new CbmLitFitQa();
   fitQa->SetMvdMinNofHits(0);
   fitQa->SetStsMinNofHits(4);
   fitQa->SetMuchMinNofHits(10);
   fitQa->SetTrdMinNofHits(8);
   fitQa->SetPRange(30, 0., 3.);
   fitQa->SetOutputDir(std::string(resultDir));
   run->AddTask(fitQa);

   CbmLitClusteringQa* clusteringQa = new CbmLitClusteringQa();
   clusteringQa->SetOutputDir(std::string(resultDir));
   run->AddTask(clusteringQa);


    // -----  Parameter database   --------------------------------------------
   FairRuntimeDb* rtdb = run->GetRuntimeDb();
   FairParRootFileIo* parIo1 = new FairParRootFileIo();
   FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
   parIo1->open(parFile.Data());
   parIo2->open(parFileList, "in");
   rtdb->setFirstInput(parIo1);
   rtdb->setSecondInput(parIo2);
   rtdb->setOutput(parIo1);
   rtdb->saveOutput();

    run->Init();
    run->Run(0, nEvents);

    // -----   Finish   -------------------------------------------------------
    timer.Stop();
    std::cout << "Macro finished successfully." << std::endl;
    std::cout << "Output file is "    << recoFile << std::endl;
    std::cout << "Parameter file is " << parFile << std::endl;
    std::cout << "Real time " << timer.RealTime() << " s, CPU time " << timer.CpuTime() << " s" << std::endl;
    std::cout << " Test passed" << std::endl;
    std::cout << " All ok " << std::endl;
}
