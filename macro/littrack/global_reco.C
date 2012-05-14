/**
 * \file global_reco.C
 * \brief Event reconstruction macro.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2010
 *
 * Macro runs event reconstruction in MVD, STS, TRD, MUCH and TOF.
 * Macro has 3 options "all", "hits" and "tracking".
 */

#include <iostream>
using std::cout;
using std::endl;

void global_reco(Int_t nEvents = 10, // number of events
		TString opt = "all")
// if opt == "all" STS + hit producers + global tracking are executed
// if opt == "hits" STS + hit producers are executed
// if opt == "tracking" global tracking is executed
{
	TString script = TString(gSystem->Getenv("LIT_SCRIPT"));
	TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");

   // Input and output data
   TString dir = "/Users/andrey/Development/cbm/d/events/trd_test/v11c/"; // Output directory
   TString mcFile = dir + "mc.0000.root"; // MC transport file
   TString parFile = dir + "param.0000.root"; // Parameters file
   TString globalRecoFile = dir + "global.reco.0000.root"; // Output file with reconstructed tracks and hits
   TString globalHitsFile = dir + "global.hits.0000.root"; // File with reconstructed STS tracks, STS, MUCH, TRD and TOF hits and digis
   TString globalTracksFile = dir + "global.tracks.0000.root";// Output file with global tracks

   // Digi files
   TList *parFileList = new TList();
   TObjString stsDigiFile = parDir + "/sts/sts_v11a.digi.par"; // STS digi file
   TObjString trdDigiFile = parDir + "/trd/trd_v11d.digi.par"; // TRD digi file
   TString muchDigiFile = parDir + "/much/much_v11a.digi.root"; // MUCH digi file

   // Directory for output results
   TString resultDir = "./test_v11c/";

   // Reconstruction parameters
   TString globalTrackingType = "branch"; // Global tracking type
   TString stsHitProducerType = "real"; // STS hit producer type: real, ideal
   TString trdHitProducerType = "smearing"; // TRD hit producer type: smearing, digi, clustering
   TString muchHitProducerType = "advanced"; // MUCH hit producer type: simple, advanced

   // Normalization for efficiency
   Int_t normStsPoints = 4;
   Int_t normTrdPoints = 8;
   Int_t normMuchPoints = 16;
   Int_t normTofPoints = 1;
   Int_t normTrdHits = 8;
   Int_t normMuchHits = 16;
   Int_t normTofHits = 1;

	if (script == "yes") {
		mcFile = TString(gSystem->Getenv("LIT_MC_FILE"));
		parFile = TString(gSystem->Getenv("LIT_PAR_FILE"));
		globalRecoFile = TString(gSystem->Getenv("LIT_GLOBAL_RECO_FILE"));
		globalHitsFile = TString(gSystem->Getenv("LIT_GLOBAL_HITS_FILE"));
		globalTracksFile = TString(gSystem->Getenv("LIT_GLOBAL_TRACKS_FILE"));

		resultDir = TString(gSystem->Getenv("LIT_RESULT_DIR"));

		globalTrackingType = TString(gSystem->Getenv("LIT_GLOBAL_TRACKING_TYPE"));
		stsHitProducerType = TString(gSystem->Getenv("LIT_STS_HITPRODUCER_TYPE"));
		trdHitProducerType = TString(gSystem->Getenv("LIT_TRD_HITPRODUCER_TYPE"));
		muchHitProducerType = TString(gSystem->Getenv("LIT_MUCH_HITPRODUCER_TYPE"));

		stsDigiFile = TString(gSystem->Getenv("LIT_STS_DIGI"));
		trdDigiFile = TString(gSystem->Getenv("LIT_TRD_DIGI"));
		muchDigiFile = TString(gSystem->Getenv("LIT_MUCH_DIGI"));

		normStsPoints = TString(gSystem->Getenv("LIT_NORM_STS_POINTS")).Atoi();
		normTrdPoints = TString(gSystem->Getenv("LIT_NORM_TRD_POINTS")).Atoi();
		normMuchPoints = TString(gSystem->Getenv("LIT_NORM_MUCH_POINTS")).Atoi();
		normTofPoints = TString(gSystem->Getenv("LIT_NORM_TOF_POINTS")).Atoi();
		normTrdHits = TString(gSystem->Getenv("LIT_NORM_TRD_HITS")).Atoi();
		normMuchHits = TString(gSystem->Getenv("LIT_NORM_MUCH_HITS")).Atoi();
		normTofHits = TString(gSystem->Getenv("LIT_NORM_TOF_HITS")).Atoi();
	}

   parFileList->Add(&stsDigiFile);
   parFileList->Add(&trdDigiFile);

	Int_t iVerbose = 1;
	TStopwatch timer;
	timer.Start();

	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/loadlibs.C");
	loadlibs();

	FairRunAna *run = new FairRunAna();
	if (opt == "all") {
		run->SetInputFile(mcFile);
		run->SetOutputFile(globalRecoFile);
	} else if (opt == "hits") {
		run->SetInputFile(mcFile);
		run->SetOutputFile(globalHitsFile);
	} else if (opt == "tracking") {
		run->SetInputFile(mcFile);
		run->AddFriend(globalHitsFile);
		run->SetOutputFile(globalTracksFile);
	} else {
		std::cout << "-E- Incorrect opt parameter" << std::endl;
		exit(0);
	}

	if (opt == "all" || opt == "hits") {

		if (IsMvd(parFile)) {
			// ----- MVD reconstruction    --------------------------------------------
			CbmMvdDigitizeL* mvdDigi = new CbmMvdDigitizeL("MVD Digitiser", 0, iVerbose);
			run->AddTask(mvdDigi);

			CbmMvdFindHits* mvdHitFinder = new CbmMvdFindHits("MVD Hit Finder", 0, iVerbose);
			run->AddTask(mvdHitFinder);
			// -------------------------------------------------------------------------
		}

		if (stsHitProducerType == "real") {
		// ----- STS REAL reconstruction -----------------------------------------------
         Double_t threshold  =  4;
         Double_t noiseWidth =  0.01;
         Int_t    nofBits    = 20;
         Double_t minStep    =  0.01;
         Double_t StripDeadTime = 0.1;
         CbmStsDigitize* stsDigitize = new CbmStsDigitize("STS Digitiser", iVerbose);
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

         FairTask* stsClusterFinder = new CbmStsClusterFinder("STS Cluster Finder",iVerbose);
         run->AddTask(stsClusterFinder);

         FairTask* stsFindHits = new CbmStsFindHits("STS Hit Finder", iVerbose);
         run->AddTask(stsFindHits);

         FairTask* stsMatchHits = new CbmStsMatchHits("STS Hit Matcher", iVerbose);
         run->AddTask(stsMatchHits);

		} else if (stsHitProducerType == "ideal") {

         // ----- STS IDEAL reconstruction   ---------------------------------------------
         FairTask* stsDigitize =	new CbmStsIdealDigitize("STSDigitize", iVerbose);
         run->AddTask(stsDigitize);

         FairTask* stsClusterFinder = new CbmStsClusterFinder("STS Cluster Finder", iVerbose);
         run->AddTask(stsClusterFinder);

         FairTask* stsFindHits =	new CbmStsIdealFindHits("STSFindHits", iVerbose);
         run->AddTask(stsFindHits);

         FairTask* stsMatchHits = new CbmStsIdealMatchHits("STSMatchHits", iVerbose);
         run->AddTask(stsMatchHits);
		}

		FairTask* kalman = new CbmKF();
		run->AddTask(kalman);
		FairTask* l1 = new CbmL1();
		run->AddTask(l1);
		CbmStsTrackFinder* trackFinder = new CbmL1StsTrackFinder();
		FairTask* findTracks = new CbmStsFindTracks(iVerbose, trackFinder);
		run->AddTask(findTracks);

		FairTask* stsMatchTracks = new CbmStsMatchTracks("STSMatchTracks", iVerbose);
		run->AddTask(stsMatchTracks);

//		CbmStsTrackFitter* trackFitter = new CbmStsKFTrackFitter();
//		FairTask* fitTracks = new CbmStsFitTracks("STS Track Fitter", trackFitter, iVerbose);
//		run->AddTask(fitTracks);
		// ------------------------------------------------------------------------

		if (IsMuch(parFile)) {
			// ----- MUCH reconstruction ---------------------------------------
		   if (muchHitProducerType == "simple") {
            CbmMuchDigitizeSimpleGem* muchDigitize = 	new CbmMuchDigitizeSimpleGem(
                  "MuchDigitize", muchDigiFile.Data(), iVerbose);
            run->AddTask(muchDigitize);
            CbmMuchFindHitsSimpleGem* muchFindHits = new CbmMuchFindHitsSimpleGem(
                  "MuchFindHits", muchDigiFile.Data(), iVerbose);
            run->AddTask(muchFindHits);
		   } else if (muchHitProducerType == "advanced") {
            CbmMuchDigitizeAdvancedGem* digitize = new CbmMuchDigitizeAdvancedGem(
                  "MuchDigitizeAdvancedGem", muchDigiFile.Data(), iVerbose);
            digitize->SetSpotRadius(0.05);
            digitize->SetQThreshold(3);
            digitize->SetQMaximum(500000);
            digitize->SetNADCChannels(256);
            digitize->SetDeadPadsFrac(0.05);
            run->AddTask(digitize);

            CbmMuchFindHitsAdvancedGem* findHits = new CbmMuchFindHitsAdvancedGem(
                  "MuchFindHitsAdvancedGem", muchDigiFile.Data(), iVerbose);
            findHits->SetAlgorithm(3);
            findHits->SetNStations(6);
            Double_t thresholds[] = {0.1, 0.1, 0.1, 0.1, 0.1, 0.1};
            findHits->SetThresholdRatios(thresholds);
            run->AddTask(findHits);
		   }

			CbmMuchDigitizeStraws* strawDigitize = new CbmMuchDigitizeStraws(
			      "MuchDigitizeStraws", muchDigiFile.Data(), iVerbose);
			run->AddTask(strawDigitize);
			CbmMuchFindHitsStraws* strawFindHits = new CbmMuchFindHitsStraws(
					"MuchFindHitsStraws", muchDigiFile.Data(), iVerbose);
			strawFindHits->SetMerge(1);
//			strawFindHits->SetMirror(1);
//			strawFindHits->SetBinary();
			run->AddTask(strawFindHits);
			// -----------------------------------------------------------------
		}

		if (IsTrd(parFile)) {
			// ----- TRD reconstruction-----------------------------------------
			// Update of the values for the radiator F.U. 17.08.07
			Int_t trdNFoils = 130; // number of polyetylene foils
			Float_t trdDFoils = 0.0013; // thickness of 1 foil [cm]
			Float_t trdDGap = 0.02; // thickness of gap between foils [cm]
			Bool_t simpleTR = kTRUE; // use fast and simple version for TR production

			CbmTrdRadiator *radiator = new CbmTrdRadiator(simpleTR, trdNFoils, trdDFoils, trdDGap);

			if (trdHitProducerType == "smearing") {
//				// ----- TRD hit smearing -----
				Double_t trdSigmaX[] = { 300, 400, 500 }; // Resolution in x [mum]
				// Resolutions in y - station and angle dependent [mum]
				Double_t trdSigmaY1[] = { 2700, 3700, 15000, 27600, 33000, 33000, 33000 };
				Double_t trdSigmaY2[] = { 6300, 8300, 33000, 33000, 33000, 33000, 33000 };
				Double_t trdSigmaY3[] = { 10300, 15000, 33000, 33000, 33000, 33000, 33000 };

				CbmTrdHitProducerSmearing* trdHitProd = new CbmTrdHitProducerSmearing("TRD Hitproducer", "TRD task", radiator);
				trdHitProd->SetSigmaX(trdSigmaX);
				trdHitProd->SetSigmaY(trdSigmaY1, trdSigmaY2, trdSigmaY3);
				run->AddTask(trdHitProd);
//				// ----- End TRD hit smearing -----
			} else if (trdHitProducerType == "digi") {
				// ----- TRD Digitizer -----
				CbmTrdDigitizer* trdDigitizer = new CbmTrdDigitizer("TRD Digitizer", "TRD task", radiator);
				run->AddTask(trdDigitizer);

				CbmTrdHitProducerDigi* trdHitProd = new CbmTrdHitProducerDigi("TRD Hit Producer", "TRD task");
				run->AddTask(trdHitProd);
				// ----- End TRD Digitizer -----
			} else if (trdHitProducerType == "clustering") {
				// ----- TRD clustering -----
				CbmTrdClusterizer* trdClustering = new CbmTrdClusterizer("TRD Clusterizer", "TRD task",radiator);
				run->AddTask(trdClustering);

				CbmTrdClusterFinderFast* trdClusterfindingfast = new CbmTrdClusterFinderFast();
				run->AddTask(trdClusterfindingfast);

				CbmTrdHitProducerCluster* trdClusterHitProducer = new CbmTrdHitProducerCluster();
				run->AddTask(trdClusterHitProducer);
				// ----- End TRD Clustering -----
			}
			// ------------------------------------------------------------------------
		}

		if (IsTof(parFile)) {
			// ------ TOF hits --------------------------------------------------------
			CbmTofHitProducer* tofHitProd = new CbmTofHitProducer("TOF HitProducer", 1);
			run->AddTask(tofHitProd);
			// ------------------------------------------------------------------------
		}
	}

	if (opt == "all" || opt == "tracking") {
		// ------ Global track reconstruction -------------------------------------
		CbmLitFindGlobalTracks* finder = new CbmLitFindGlobalTracks();
		// Tracking method to be used
		// "branch" - branching tracking
		// "nn" - nearest neighbor tracking
		// "nn_parallel" - nearest neighbor parallel tracking
		finder->SetTrackingType(std::string(globalTrackingType));

		// Hit-to-track merger method to be used
		// "nearest_hit" - assigns nearest hit to the track
		finder->SetMergerType("nearest_hit");

		run->AddTask(finder);

		if (IsTrd(parFile)) {
			CbmTrdMatchTracks* trdMatchTracks = new CbmTrdMatchTracks(1);
			run->AddTask(trdMatchTracks);
		}

		if (IsMuch(parFile)) {
			CbmMuchMatchTracks* muchMatchTracks = new CbmMuchMatchTracks();
			run->AddTask(muchMatchTracks);
		}
	}

	if (opt == "all" || opt == "tracking") {
	  if (IsRich(parFile)) {
	     if (opt == "tracking") {
	        FairTask* kalman = new CbmKF();
	        run->AddTask(kalman);
	        FairTask* l1 = new CbmL1();
	        run->AddTask(l1);
	     }

	     CbmRichHitProducer* richHitProd  = new CbmRichHitProducer();
	     run->AddTask(richHitProd);

		  CbmRichReconstruction* richReco = new CbmRichReconstruction();
		  run->AddTask(richReco);

		  CbmRichMatchRings* matchRings = new CbmRichMatchRings();
		  run->AddTask(matchRings);
	   }
    }

    if (opt == "all" || opt == "tracking") {
      if (opt == "tracking") {
         FairTask* kalman = new CbmKF();
         run->AddTask(kalman);
         FairTask* l1 = new CbmL1();
         run->AddTask(l1);
      }
		// -----   Primary vertex finding   ---------------------------------------
		CbmPrimaryVertexFinder* pvFinder = new CbmPVFinderKF();
		CbmFindPrimaryVertex* findVertex = new CbmFindPrimaryVertex(pvFinder);
		run->AddTask(findVertex);
		// ------------------------------------------------------------------------
	}

	if (opt == "all" || opt == "tracking") {
		// -----   Track finding QA check   ------------------------------------
		CbmLitTrackingQa* trackingQa = new CbmLitTrackingQa();
		trackingQa->SetMinNofPointsSts(normStsPoints);
		trackingQa->SetMinNofPointsTrd(normTrdPoints);
		trackingQa->SetMinNofPointsMuch(normMuchPoints);
		trackingQa->SetMinNofPointsTof(normTofPoints);
		trackingQa->SetQuota(0.7);
		trackingQa->SetMinNofHitsTrd(normTrdHits);
		trackingQa->SetMinNofHitsMuch(normMuchHits);
		trackingQa->SetMinNofHitsRich(7);
		trackingQa->SetQuotaRich(0.6);
		trackingQa->SetVerbose(normTofHits);
		trackingQa->SetOutputDir(std::string(resultDir));
		run->AddTask(trackingQa);
		// ------------------------------------------------------------------------

		CbmLitFitQa* fitQa = new CbmLitFitQa();
		fitQa->SetMvdMinNofHits(0);
		fitQa->SetStsMinNofHits(normStsPoints);
		fitQa->SetMuchMinNofHits(normMuchPoints);
		fitQa->SetTrdMinNofHits(normTrdPoints);
		fitQa->SetOutputDir(std::string(resultDir));
		run->AddTask(fitQa);

		CbmLitClusteringQa* clusteringQa = new CbmLitClusteringQa();
		clusteringQa->SetOutputDir(std::string(resultDir));
		run->AddTask(clusteringQa);
	}

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
	// ------------------------------------------------------------------------

	// -----   Initialize and run   --------------------------------------------
	run->Init();
	run->Run(0, nEvents);
	// ------------------------------------------------------------------------

	// -----   Finish   -------------------------------------------------------
	timer.Stop();
	cout << endl << endl;
 	cout << "Macro finished successfully." << endl;
   cout << "Test passed"<< endl;
   cout << " All ok " << endl;
	cout << "Output file is " << globalRecoFile << endl;
	cout << "Parameter file is " << parFile << endl;
	cout << "Real time " << timer.RealTime() << " s, CPU time "	<< timer.CpuTime() << " s" << endl;
	cout << endl;
	// ------------------------------------------------------------------------
}
