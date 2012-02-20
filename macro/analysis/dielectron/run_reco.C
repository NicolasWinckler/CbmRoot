
#include "../../../cbmbase/CbmDetectorList.h";

void run_reco(Int_t nEvents = 700){
	Int_t iVerbose = 0;

	TString script = TString(gSystem->Getenv("SCRIPT"));
	TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");

	//gRandom->SetSeed(10);

	TString inFile = "", parFile = "", outFile ="", deltaFile ="";
   TString delta = ""; // if "yes" Delta electrons will be embedded
	if (script != "yes") {
		TString inFile = "/d/cbm02/slebedev/rich/JUL09/auau.25gev.centr.0000.mc.root";
		TString parFile = "/d/cbm02/slebedev/rich/JUL09/auau.25gev.centr.0000.params.root";
		TString outFile = "/d/cbm02/slebedev/rich/JUL09/auau.25gev.centr.0000.reco.root";
	} else {
		inFile = TString(gSystem->Getenv("MCFILE"));
		outFile = TString(gSystem->Getenv("RECOFILE"));
		parFile = TString(gSystem->Getenv("PARFILE"));
		delta = TString(gSystem->Getenv("DELTA"));
		deltaFile = TString(gSystem->Getenv("DELTAFILE"));
	}

	TString stsDigiFile = "sts_v11a.digi.par";
   gDebug = 0;

   TStopwatch timer;
   timer.Start();

	// ----  Load libraries   -------------------------------------------------
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/rich/cbmlibs.C");
	cbmlibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/determine_setup.C");

	// -----   Reconstruction run   -------------------------------------------
	FairRunAna *run= new FairRunAna();
	if (inFile != "") run->SetInputFile(inFile);
	if (outFile != "") run->SetOutputFile(outFile);


    // =========================================================================
    // ===                     MVD local reconstruction                      ===
	// =========================================================================
	Bool_t useMvdInTracking = kFALSE;
	if (IsMvd(parFile)) {
	   useMvdInTracking = kTRUE;
      CbmMvdDigitizeL* mvdDigitizeL = new CbmMvdDigitizeL("CbmMvdDigitizeL", 0, iVerbose);
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

      CbmMvdFindHits* mvdFindHits = new CbmMvdFindHits("CbmMvdFindHits", 0, iVerbose);
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
      //CbmMvdHitProducer* mvdHitProd = new CbmMvdHitProducer("MVDHitProducer", 0, iVerbose);
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

    CbmStsDigitize* stsDigitize = new CbmStsDigitize("STSDigitize", iVerbose);
    stsDigitize->SetRealisticResponse();
    stsDigitize->SetFrontThreshold (threshold);
    stsDigitize->SetBackThreshold  (threshold);
    stsDigitize->SetFrontNoiseWidth(noiseWidth);
    stsDigitize->SetBackNoiseWidth (noiseWidth);
    stsDigitize->SetFrontNofBits   (nofBits);
    stsDigitize->SetBackNofBits    (nofBits);
    stsDigitize->SetFrontMinStep   (minStep);
    stsDigitize->SetBackMinStep    (minStep);
    stsDigitize->SetStripDeadTime  (StripDeadTime);
    run->AddTask(stsDigitize);

    FairTask* stsClusterFinder = new CbmStsClusterFinder("CbmStsClusterFinder",iVerbose);
    run->AddTask(stsClusterFinder);

    FairTask* stsFindHits = new CbmStsFindHits(iVerbose);
    run->AddTask(stsFindHits);

    CbmStsMatchHits* stsMatchHits = new CbmStsMatchHits(iVerbose);
    run->AddTask(stsMatchHits);
    } else { // STS IDEAL RESPONSE
      FairTask* stsDigitize = new CbmStsIdealDigitize("CbmStsIdealDigitize", iVerbose);
      run->AddTask(stsDigitize);

      FairTask* stsClusterFinder = new CbmStsClusterFinder("CbmStsClusterFinder",iVerbose);
      run->AddTask(stsClusterFinder);

      FairTask* stsFindHits = new CbmStsIdealFindHits(iVerbose);
      run->AddTask(stsFindHits);

      FairTask* stsMatchHits = new CbmStsIdealMatchHits(iVerbose);
      run->AddTask(stsMatchHits);
   }


	CbmKF* kalman = new CbmKF();
	run->AddTask(kalman);
	CbmL1* l1 = new CbmL1();
//	CbmL1* l1 = new CbmL1("CbmL1",1, 3); //to fill L1 histo
	run->AddTask(l1);

	CbmStsTrackFinder* stsTrackFinder = new CbmL1StsTrackFinder();
	//Bool_t useMvdInTracking = kTRUE;
	FairTask* stsFindTracks = new CbmStsFindTracks(iVerbose, stsTrackFinder, useMvdInTracking);
	run->AddTask(stsFindTracks);

	FairTask* stsMatchTracks = new CbmStsMatchTracks(iVerbose);
	run->AddTask(stsMatchTracks);

	CbmStsTrackFitter* stsTrackFitter = new CbmStsKFTrackFitter();
	FairTask* stsFitTracks = new CbmStsFitTracks(stsTrackFitter, iVerbose);
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

		// -----   TRD hit producer   ----------------------------------------------
		Double_t trdSigmaX[] = {300, 400, 500}; // Resolution in x [mum]
		// Resolutions in y - station and angle dependent [mum]
		Double_t trdSigmaY1[] = {2700,   3700, 15000, 27600, 33000, 33000, 33000 };
		Double_t trdSigmaY2[] = {6300,   8300, 33000, 33000, 33000, 33000, 33000 };
		Double_t trdSigmaY3[] = {10300, 15000, 33000, 33000, 33000, 33000, 33000 };

		CbmTrdHitProducerSmearing* trdHitProd = new
			   CbmTrdHitProducerSmearing("CbmTrdHitProducer", "CbmTrdHitProducer", radiator);

		trdHitProd->SetSigmaX(trdSigmaX);
		trdHitProd->SetSigmaY(trdSigmaY1, trdSigmaY2, trdSigmaY3);
		run->AddTask(trdHitProd);
	}// isTRD

	// =========================================================================
	// ===                     TOF local reconstruction                      ===
	// =========================================================================
	if (IsTof(parFile)) {
		// ------   TOF hit producer   ---------------------------------------------
		CbmTofHitProducer* tofHitProd = new CbmTofHitProducer("CbmTofHitProducer", iVerbose);
		run->AddTask(tofHitProd);
	} //isTof

	// =========================================================================
	// ===                        Global tracking                            ===
	// =========================================================================

	CbmLitFindGlobalTracks* finder = new CbmLitFindGlobalTracks();
	finder->SetTrackingType(std::string("branch"));
	finder->SetMergerType("nearest_hit");
	run->AddTask(finder);

	// -----   Primary vertex finding   ---------------------------------------
	CbmPrimaryVertexFinder* pvFinder = new CbmPVFinderKF();
	CbmFindPrimaryVertex* findVertex = new CbmFindPrimaryVertex(pvFinder);
	run->AddTask(findVertex);
	// ------------------------------------------------------------------------

	if (IsTrd(parFile)) {
		CbmTrdMatchTracks* trdMatchTracks = new CbmTrdMatchTracks(iVerbose);
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
   trackingQa->SetOutputDir("recqa/");
   run->AddTask(trackingQa);

    // =========================================================================
    // ===                        ECAL reconstruction                        ===
    // =========================================================================

//  // -----   ECAL hit producer  ----------------------------------------------
//  CbmEcalHitProducerFastMC* ecalHitProd
//    = new CbmEcalHitProducerFastMC("ECAL Hitproducer");
//  run->AddTask(ecalHitProd);

    // -----  Parameter database   --------------------------------------------
    TString stsDigi = gSystem->Getenv("VMCWORKDIR");
    stsDigi += "/parameters/sts/";
    stsDigi += stsDigiFile;
    FairRuntimeDb* rtdb = run->GetRuntimeDb();
    FairParRootFileIo* parIo1 = new FairParRootFileIo();
    FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
    parIo1->open(parFile.Data());
    parIo2->open(stsDigi.Data(),"in");
    rtdb->setFirstInput(parIo1);
    rtdb->setSecondInput(parIo2);
    rtdb->setOutput(parIo1);
    rtdb->saveOutput();

    run->Init();
    cout << "Starting run" << endl;
    run->Run(0,nEvents);

    // -----   Finish   -------------------------------------------------------
    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << endl << endl;
    cout << "Macro finished successfully." << endl;
    cout << "Output file is "    << outFile << endl;
    cout << "Parameter file is " << parFile << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
    cout << endl;

    cout << " Test passed" << endl;
    cout << " All ok " << endl;
    exit(0);
}
