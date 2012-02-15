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

void global_reco(Int_t nEvents = 1000, // number of events
		TString opt = "all")
// if opt == "all" STS + hit producers + global tracking are executed
// if opt == "hits" STS + hit producers are executed
// if opt == "tracking" global tracking is executed
{
	TString script = TString(gSystem->Getenv("SCRIPT"));
	TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");

   // Input and output data
   TString dir = "/data.local1/andrey/events/events_propagation_qa_electrons/"; // Output directory
   TString mcFile = dir + "mc.0000.root"; // MC transport file
   TString parFile = dir + "param.0000.root"; // Parameters file
   TString globalRecoFile = dir + "global.reco.0000.root"; // Output file with reconstructed tracks and hits
   TString globalHitsFile = dir + "global.hits.0000.root"; // File with reconstructed STS tracks, STS, MUCH, TRD and TOF hits and digis
   TString globalTracksFile = dir + "global.tracks.0000.root";// Output file with global tracks

   // Digi files
   TList *parFileList = new TList();
   TObjString stsDigiFile = parDir + "/sts/sts_v11a.digi.par"; // STS digi file
   TObjString trdDigiFile = parDir + "/trd/trd_v10b.digi.par"; // TRD digi file
   TString muchDigiFile = parDir + "/much/much_v11a.digi.root"; // MUCH digi file

   // Directory for output results
   TString resultDir = "./test/";

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
		mcFile = TString(gSystem->Getenv("MCFILE"));
		parFile = TString(gSystem->Getenv("PARFILE"));
		globalRecoFile = TString(gSystem->Getenv("GLOBALRECOFILE"));
		globalHitsFile = TString(gSystem->Getenv("GLOBALHITSFILE"));
		globalTracksFile = TString(gSystem->Getenv("GLOBALTRACKSFILE"));

		resultDir = TString(gSystem->Getenv("RESULTDIR"));

		globalTrackingType = TString(gSystem->Getenv("GLOBALTRACKINGTYPE"));
		stsHitProducerType = TString(gSystem->Getenv("STSHITPRODUCERTYPE"));
		trdHitProducerType = TString(gSystem->Getenv("TRDHITPRODUCERTYPE"));
		muchHitProducerType = TString(gSystem->Getenv("MUCHHITPRODUCERTYPE"));

		stsDigiFile = TString(gSystem->Getenv("STSDIGI"));
		trdDigiFile = TString(gSystem->Getenv("TRDDIGI"));
		muchDigiFile = TString(gSystem->Getenv("MUCHDIGI"));

		normStsPoints = TString(gSystem->Getenv("NORMSTSPOINTS")).Atoi();
		normTrdPoints = TString(gSystem->Getenv("NORMTRDPOINTS")).Atoi();
		normMuchPoints = TString(gSystem->Getenv("NORMMUCHPOINTS")).Atoi();
		normTofPoints = TString(gSystem->Getenv("NORMTOFPOINTS")).Atoi();
		normTrdHits = TString(gSystem->Getenv("NORMTRDHITS")).Atoi();
		normMuchHits = TString(gSystem->Getenv("NORMMUCHHITS")).Atoi();
		normTofHits = TString(gSystem->Getenv("NORMTOFHITS")).Atoi();
	}

   parFileList->Add(&stsDigiFile);
   parFileList->Add(&trdDigiFile);

	Int_t iVerbose = 1;
	TStopwatch timer;
	timer.Start();

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
	cbmrootlibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/determine_setup.C");

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
            // ----- TRD hit smearing -----
            Double_t trdSigmaX[] = { 300, 400, 500 }; // Resolution in x [mum]
            // Resolutions in y - station and angle dependent [mum]
            Double_t trdSigmaY1[] = { 2700, 3700, 15000, 27600, 33000, 33000, 33000 };
            Double_t trdSigmaY2[] = { 6300, 8300, 33000, 33000, 33000, 33000, 33000 };
            Double_t trdSigmaY3[] = { 10300, 15000, 33000, 33000, 33000, 33000, 33000 };

            CbmTrdHitProducerSmearing* trdHitProd =
                  new CbmTrdHitProducerSmearing("TRD Hitproducer", "TRD task", radiator);
            trdHitProd->SetSigmaX(trdSigmaX);
            trdHitProd->SetSigmaY(trdSigmaY1, trdSigmaY2, trdSigmaY3);
            run->AddTask(trdHitProd);
            // ----- End TRD hit smearing -----
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
			CbmTofHitProducer* tofHitProd = new CbmTofHitProducer(
					"TOF HitProducer", 1);
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
		// "weight" - weighting tracking
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
		  // ---------------------RICH Hit Producer ----------------------------------
		  Double_t richPmtRad = 0.4; // PMT radius [cm]
		  Double_t richPmtDist = 0.; // Distance between PMTs [cm]
		  Int_t richDetType = 4; // Detector type Hamamatsu H8500-03 (no WLS)
		  Int_t richNoise = 220; // Number of noise points per event
		  Double_t richCollEff = 1.0; // Collection Efficiency of PMT electron optics
		  Double_t richSMirror = 0.06; // Sigma for additional point smearing due to light scattering in mirror

		  CbmRichHitProducer* richHitProd = new CbmRichHitProducer(richPmtRad,
				richPmtDist, richDetType, richNoise, iVerbose, richCollEff, richSMirror);
		  run->AddTask(richHitProd);
		  //--------------------------------------------------------------------------

		  //----------------------RICH Track Extrapolation ---------------------------
		  Int_t richNSts = 4; // minimum number of STS hits for extrapolation
		  Double_t richZPos = 300.; // z position for extrapolation [cm]
		  CbmRichTrackExtrapolation* richExtra = new CbmRichTrackExtrapolationKF(
				richNSts, iVerbose);
		  CbmRichExtrapolateTracks* richExtrapolate = new CbmRichExtrapolateTracks();
		  richExtrapolate->UseExtrapolation(richExtra, richZPos);
		  run->AddTask(richExtrapolate);
		  //--------------------------------------------------------------------------

		  //--------------------- Rich Track Projection to photodetector -------------
		  Int_t richZFlag = 1; // Projetion from IM plane (default)
		  CbmRichProjectionProducer* richProj = new CbmRichProjectionProducer(iVerbose, richZFlag);
		  run->AddTask(richProj);
		  //--------------------------------------------------------------------------

		  //--------------------- RICH Ring Finding ----------------------------------
		  TString richGeoType = "compact";//choose between compact or large
		  CbmRichRingFinderHough* richFinder = new CbmRichRingFinderHough(iVerbose,	richGeoType);
		  CbmRichFindRings* richFindRings = new CbmRichFindRings();
		  richFindRings->UseFinder(richFinder);
		  run->AddTask(richFindRings);
		  //--------------------------------------------------------------------------

		  //-------------------- RICH Ring Fitting -----------------------------------
		  CbmRichRingFitter* richFitter = new CbmRichRingFitterEllipseTau();
		  CbmRichFitRings* fitRings = new CbmRichFitRings(richFitter);
		  run->AddTask(fitRings);
		  //--------------------------------------------------------------------------

		  // ------------------- RICH Ring matching  ---------------------------------
		  CbmRichMatchRings* matchRings = new CbmRichMatchRings(iVerbose);
		  run->AddTask(matchRings);
		  // -------------------------------------------------------------------------

		  //--------------------- RICH ring-track assignment ------------------------
		  Double_t richDistance = 10.; // Max. dist. ring centre to track [cm]
		  Int_t richNPoints = 5; // Minmum number of hits on ring
		  CbmRichRingTrackAssign* richAssign = new CbmRichRingTrackAssignClosestD(
				richDistance, richNPoints, iVerbose);
		  CbmRichAssignTrack* assignTrack = new CbmRichAssignTrack();
		  assignTrack->UseAssign(richAssign);
		  run->AddTask(assignTrack);
		  // ------------------------------------------------------------------------
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
