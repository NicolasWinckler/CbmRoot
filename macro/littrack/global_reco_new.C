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

void global_reco_new(Int_t nEvents = 10,
		     TString opt = "tracking", TString setup = "electron",
		     TString mcDir = "", TString fileNr = "0000",
		     TString resultDir = "")
// if opt == "all" STS + hit producers + global tracking are executed
// if opt == "hits" STS + hit producers are executed
// if opt == "tracking" global tracking is executed
{
  
  // Data files
  TString mcFile;
  TString parFile;
  TString globalRecoFile;
  TString globalHitsFile;
  TString globalTracksFile;

   // Reconstruction parameters
  TString globalTrackingType;
  TString stsHitProducerType;
  TString trdHitProducerType;
  TString muchHitProducerType;

   // Normalization for efficiency
  Int_t normStsPoints;
  Int_t normTrdPoints;
  Int_t normMuchPoints;
  Int_t normTofPoints;
  Int_t normTrdHits;
  Int_t normMuchHits;
  Int_t normTofHits;

  // Parameters
  TString parDir;
  TString muchDigiFile;
  TList *parFileList = new TList();

  gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/SetDefaults.C");

  defaultOutput(mcDir, fileNr, resultDir, mcFile, parFile, 
		    globalRecoFile, globalHitsFile, globalTracksFile);
  defaultRecoSettings(globalTrackingType, stsHitProducerType, 
		      trdHitProducerType, muchHitProducerType);
  defaultNormalization(normStsPoints, normTrdPoints, normMuchPoints,
		       normTofPoints, normTrdHits, normMuchHits, normTofHits);
  defaultDigiParameter(parDir; muchDigiFile, parFileList);  

   
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
      // -----   STS digitizer   -------------------------------------------------
      Double_t threshold  =  4;
      Double_t noiseWidth =  0.01;
      Int_t    nofBits    = 12;
      Double_t ElectronsPerAdc    =  10.;
      Double_t StripDeadTime = 0.1;
      CbmStsDigitize* stsDigitize = new CbmStsDigitize("STS Digitiser", iVerbose);
      stsDigitize->SetRealisticResponse();
      stsDigitize->SetFrontThreshold (threshold);
      stsDigitize->SetBackThreshold  (threshold);
      stsDigitize->SetFrontNoiseWidth(noiseWidth);
      stsDigitize->SetBackNoiseWidth (noiseWidth);
      stsDigitize->SetFrontNofBits   (nofBits);
      stsDigitize->SetBackNofBits    (nofBits);
      stsDigitize->SetFrontNofElPerAdc  (ElectronsPerAdc);
      stsDigitize->SetBackNofElPerAdc   (ElectronsPerAdc);
      stsDigitize->SetStripDeadTime  (StripDeadTime);
      run->AddTask(stsDigitize);
      // -------------------------------------------------------------------------
      
      
      
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
      // -------- MUCH digitization ------------
      CbmMuchDigitizeGem* digitize = new CbmMuchDigitizeGem(muchDigiFile.Data());
      if (muchHitProducerType == "simple") {
	digitize->SetAlgorithm(0);
      } else if (muchHitProducerType == "advanced") {
	digitize->SetAlgorithm(1);
      }
      run->AddTask(digitize);
      CbmMuchFindHitsGem* findHits = new CbmMuchFindHitsGem(muchDigiFile.Data());
      run->AddTask(findHits);
      //			CbmMuchClustering* findHits = new CbmMuchClustering();
      //			findHits->SetAlgorithmVersion(2);
      //			run->AddTask(findHits);
      
      CbmMuchDigitizeStraws* strawDigitize = new CbmMuchDigitizeStraws(
								       "MuchDigitizeStraws", muchDigiFile.Data(), iVerbose);
      run->AddTask(strawDigitize);
      CbmMuchFindHitsStraws* strawFindHits = new CbmMuchFindHitsStraws(
								       "MuchFindHitsStraws", muchDigiFile.Data(), iVerbose);
      strawFindHits->SetMerge(1);
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
	CbmTrdHitProducerSmearing* trdHitProd = new CbmTrdHitProducerSmearing(radiator);
	trdHitProd->SetUseDigiPar(false);
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
	//				// ----- TRD clustering -----
	//				CbmTrdClusterizer* trdClustering = new CbmTrdClusterizer("TRD Clusterizer", "TRD task",radiator);
	//				run->AddTask(trdClustering);
	//
	//				CbmTrdClusterFinderFast* trdClusterfindingfast = new CbmTrdClusterFinderFast();
	//				run->AddTask(trdClusterfindingfast);
	//
	//				CbmTrdHitProducerCluster* trdClusterHitProducer = new CbmTrdHitProducerCluster();
	//				run->AddTask(trdClusterHitProducer);
	//				// ----- End TRD Clustering -----
	
	// ----- TRD clustering -----
	CbmTrdClusterizerFast* trdClustering = new CbmTrdClusterizerFast("TRD Clusterizer", "TRD task",radiator,false,true);
	run->AddTask(trdClustering);
	
	CbmTrdClusterFinderFast* trdClusterfindingfast = new CbmTrdClusterFinderFast(true, true, false, 5.0e-7);
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
    
    /*	CbmStsSimulationQa* stsSimulationQa = new CbmStsSimulationQa();
	stsSimulationQa->SetOutputDir(std::string(resultDir));
	run->AddTask(stsSimulationQa);*/
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
