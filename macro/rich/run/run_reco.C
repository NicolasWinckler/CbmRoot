
#include "../../../cbmbase/CbmDetectorList.h";

void run_reco(Int_t nEvents = 700)
{

  // ========================================================================
  //          Adjust this part according to your requirements

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;

  // Input file (MC events)
  TString inFile = "/d/cbm02/slebedev/rich/JUL09/auau.25gev.centr.0000.mc.root";

  // Parameter file
  TString parFile = "/d/cbm02/slebedev/rich/JUL09/auau.25gev.centr.0000.params.root";

  // STS digitisation file
  TString stsDigiFile = "sts_standard.digi.par";

  // Output file
  TString outFile = "/d/cbm02/slebedev/rich/JUL09/auau.25gev.centr.0000.reco.root";


  // In general, the following parts need not be touched
  // ========================================================================



  // ----    Debug option   -------------------------------------------------
  gDebug = 0;
  // ------------------------------------------------------------------------



  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------



  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gROOT->LoadMacro("$VMCWORKDIR/macro/rich/cbmlibs.C");
  cbmlibs();
  // ------------------------------------------------------------------------

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *run= new FairRunAna();
  run->SetInputFile(inFile);
  run->SetOutputFile(outFile);
  // ------------------------------------------------------------------------

  // =========================================================================
  // ===                      STS local reconstruction                     ===
  // =========================================================================

  // -----   STS digitizer   -------------------------------------------------
  FairTask* stsDigitize = new CbmStsDigitize(iVerbose);
  run->AddTask(stsDigitize);
  // -------------------------------------------------------------------------


  // -----  STS hit finding   ------------------------------------------------
  FairTask* stsFindHits = new CbmStsFindHits(iVerbose);
  run->AddTask(stsFindHits);
  // -------------------------------------------------------------------------


  // -----  STS hit matching   -----------------------------------------------
  CbmStsMatchHits* stsMatchHits = new CbmStsMatchHits(iVerbose);
  run->AddTask(stsMatchHits);
  // -------------------------------------------------------------------------


  // ---  STS track finding   ------------------------------------------------
  CbmKF* kalman = new CbmKF();
  run->AddTask(kalman);
  CbmL1* l1 = new CbmL1();
  run->AddTask(l1);
  CbmStsTrackFinder* stsTrackFinder    = new CbmL1StsTrackFinder();
  FairTask* stsFindTracks = new CbmStsFindTracks(iVerbose, stsTrackFinder);  run->AddTask(stsFindTracks);
  // -------------------------------------------------------------------------


  // ---   STS track matching   ----------------------------------------------
  FairTask* stsMatchTracks = new CbmStsMatchTracks(iVerbose);
  run->AddTask(stsMatchTracks);
  // -------------------------------------------------------------------------


  // ---   STS track fitting   -----------------------------------------------
  CbmStsTrackFitter* stsTrackFitter = new CbmStsKFTrackFitter();
  FairTask* stsFitTracks = new CbmStsFitTracks(stsTrackFitter, iVerbose);
  run->AddTask(stsFitTracks);
  // -------------------------------------------------------------------------

  // ===                 End of STS local reconstruction                   ===
  // =========================================================================





  // =========================================================================
  // ===                     TRD local reconstruction                      ===
  // =========================================================================

  // Update of the values for the radiator F.U. 17.08.07
  Int_t trdNFoils    = 130;      // number of polyetylene foils
  Float_t trdDFoils = 0.0013;    // thickness of 1 foil [cm]
  Float_t trdDGap   = 0.02;      // thickness of gap between foils [cm]
  Bool_t simpleTR = kTRUE;       // use fast and simple version for TR
                                 // production

  CbmTrdRadiator *radiator = new CbmTrdRadiator(simpleTR , trdNFoils,
                                       trdDFoils, trdDGap);

  // -----   TRD hit producer   ----------------------------------------------
  Double_t trdSigmaX[] = {300, 400, 500};             // Resolution in x [mum]
  // Resolutions in y - station and angle dependent [mum]
  Double_t trdSigmaY1[] = {2700,   3700, 15000, 27600, 33000, 33000, 33000 };
  Double_t trdSigmaY2[] = {6300,   8300, 33000, 33000, 33000, 33000, 33000 };
  Double_t trdSigmaY3[] = {10300, 15000, 33000, 33000, 33000, 33000, 33000 };

  CbmTrdHitProducerSmearing* trdHitProd = new
           CbmTrdHitProducerSmearing("TRD Hitproducer", "TRD task", radiator);

  trdHitProd->SetSigmaX(trdSigmaX);
  trdHitProd->SetSigmaY(trdSigmaY1, trdSigmaY2, trdSigmaY3);

  run->AddTask(trdHitProd);

  // -------------------------------------------------------------------------


  // -----   TRD track finding   ---------------------------------------------
 // CbmTrdTrackFinder* trdTrackFinder    = new CbmL1TrdTrackFinderSts();
 // ((CbmL1TrdTrackFinderSts*)trdTrackFinder)->SetVerbose(iVerbose);
  CbmTrdTrackFinder* trdTrackFinder    = new CbmLitTrdTrackFinderBranch();
  CbmTrdFindTracks* trdFindTracks = new CbmTrdFindTracks("TRD Track Finder");
  trdFindTracks->UseFinder(trdTrackFinder);
  run->AddTask(trdFindTracks);
  // -------------------------------------------------------------------------


  // -----   TRD track fitting   ---------------------------------------------
  CbmTrdTrackFitter* trdTrackFitter    = new CbmTrdTrackFitterKF();
  ((CbmTrdTrackFitterKF*)trdTrackFitter)->SetVerbose(iVerbose);
  ((CbmTrdTrackFitterKF*)trdTrackFitter)->SetPid(211);
  CbmTrdFitTracks* trdFitTracks = new CbmTrdFitTracks("TRD track fitter",
						      "TRD",
						      trdTrackFitter);
  run->AddTask(trdFitTracks);
  // -------------------------------------------------------------------------


  // -----   TRD track matching   --------------------------------------------
  CbmTrdMatchTracks* trdMatchTracks = new CbmTrdMatchTracks(iVerbose);
  run->AddTask(trdMatchTracks);



      // ----------- TRD track Pid Wkn ----------------------
    CbmTrdSetTracksPidWkn* trdSetTracksPidTask = new
    CbmTrdSetTracksPidWkn("trdFindTracks","trdFindTracks");
    run->AddTask(trdSetTracksPidTask);
    // ----------------------------------------------------

    // ----------- TRD track Pid Ann ----------------------
    CbmTrdSetTracksPidANN* trdSetTracksPidAnnTask = new
    CbmTrdSetTracksPidANN("Ann","Ann");
    run->AddTask(trdSetTracksPidAnnTask);
    // ----------------------------------------------------


//    CbmLitRecQa* litRecQa   =  new CbmLitRecQa(8, 0.7, kTRD, 1);
//    litRecQa->SetNormType(2); // '2' to number of STS tracks
//    run->AddTask(litRecQa);

  // -------------------------------------------------------------------------

  // ===                 End of TRD local reconstruction                   ===
  // =========================================================================






  // =========================================================================
  // ===                     TOF local reconstruction                      ===
  // =========================================================================


  // ------   TOF hit producer   ---------------------------------------------
  CbmTofHitProducer* tofHitProd = new CbmTofHitProducer("TOF HitProducer",
  							iVerbose);
  run->AddTask(tofHitProd);
  // -------------------------------------------------------------------------

  // ===                   End of TOF local reconstruction                 ===
  // =========================================================================






  // =========================================================================
  // ===                        Global tracking                            ===
  // =========================================================================

  // -----   STS-TRD-TOF track merging   -------------------------------------
  CbmL1TrackMerger* trackMerger          = new CbmL1TrackMerger();
  trackMerger->SetMethod(1);
  CbmL1TofMerger* tofMerger = new CbmL1TofMerger();
  CbmFindGlobalTracks* findGlobal = new CbmFindGlobalTracks(trackMerger,
							    NULL, NULL,
							    iVerbose);
  run->AddTask(findGlobal);

  //--------- TOF tracklength calculation -----------------------------------
   CbmGlobalTrackFitterKF* globalFitter = new CbmGlobalTrackFitterKF();
   CbmFitGlobalTracks* fitGlobal
                 = new
   CbmFitGlobalTracks("globalfitter",iVerbose,globalFitter);

   run->AddTask(fitGlobal);
  //-------------------------------------------------------------------------


  // -----   Primary vertex finding   ---------------------------------------
  CbmPrimaryVertexFinder* pvFinder = new CbmPVFinderKF();
  CbmFindPrimaryVertex* findVertex = new CbmFindPrimaryVertex(pvFinder);
  run->AddTask(findVertex);
  // ------------------------------------------------------------------------

  // ===                      End of global tracking                       ===
  // =========================================================================




    // ----------- TRD track Pid Like ----------------------
    // Since in the newest version of this method depends on the global
    // track the task has to move after the global tracking
    CbmTrdSetTracksPidLike* trdSetTracksPidLikeTask = new
    CbmTrdSetTracksPidLike("Likelihood","Likelihood");
    run->AddTask(trdSetTracksPidLikeTask);
    // ----------------------------------------------------




  // =========================================================================
  // ===                        RICH reconstruction                        ===
  // =========================================================================

  // ---------------------RICH Hit Producer ----------------------------------
  Double_t richPmtRad  = 0.4;     // PMT radius [cm]
  Double_t richPmtDist = 0.;      // Distance between PMTs [cm]
  Int_t    richDetType = 4;       // Detector type Hamamatsu H8500-03
  Int_t    richNoise   = 220;     // Number of noise points per event
  Double_t collectionEff = 1.0;
  CbmRichHitProducer* richHitProd
    = new CbmRichHitProducer(richPmtRad, richPmtDist, richDetType,
			     richNoise, iVerbose, collectionEff);
  run->AddTask(richHitProd);
  //--------------------------------------------------------------------------


  //----------------------RICH Track Extrapolation ---------------------------
  Int_t    richNSts = 4;     // minimum number of STS hits for extrapolation
  Double_t richZPos = 300.;  // z position for extrapolation [cm]
  CbmRichTrackExtrapolation* richExtra
    = new CbmRichTrackExtrapolationKF(richNSts, iVerbose);
  CbmRichExtrapolateTracks* richExtrapolate = new CbmRichExtrapolateTracks();
  richExtrapolate->UseExtrapolation(richExtra,richZPos);
  run->AddTask(richExtrapolate);
  //--------------------------------------------------------------------------


  //--------------------- Rich Track Projection to photodetector -------------
  Int_t richZFlag = 1;       // Projetion from IM plane (default)
  CbmRichProjectionProducer* richProj =
  new CbmRichProjectionProducer(iVerbose, richZFlag);
  run->AddTask(richProj);
  //--------------------------------------------------------------------------


  //--------------------- RICH Ring Finding ----------------------------------
  TString richGeoType = "compact";
  CbmRichRingFinderHough* richFinder = new CbmRichRingFinderHough(iVerbose, richGeoType);
  CbmRichFindRings* richFindRings = new CbmRichFindRings();
  richFindRings->UseFinder(richFinder);
  run->AddTask(richFindRings);
  //--------------------------------------------------------------------------

  //-------------------- RICH Ring Fitting -----------------------------------
  CbmRichRingFitter* richFitter = new CbmRichRingFitterEllipseTau(iVerbose, 1, richGeoType);
  CbmRichFitRings* fitRings = new CbmRichFitRings("","",richFitter);
  run->AddTask(fitRings);
  //--------------------------------------------------------------------------

  // ------------------- RICH Ring matching  ---------------------------------
  CbmRichMatchRings* matchRings = new CbmRichMatchRings(2);
  run->AddTask(matchRings);
  // -------------------------------------------------------------------------

  //--------------------- RICH ring-track assignment ------------------------
  Double_t richDistance = 10.; // Max. dist. ring centre to track [cm]
  Int_t    richNPoints  = 5;   // Minmum number of hits on ring
  CbmRichRingTrackAssign* richAssign   =
    new CbmRichRingTrackAssignClosestD(richDistance, richNPoints, 3);
  CbmRichAssignTrack* assignTrack = new CbmRichAssignTrack();
  assignTrack->UseAssign(richAssign);
  run->AddTask(assignTrack);
  // ------------------------------------------------------------------------

  CbmRichRingQa* richQa   =  new CbmRichRingQa("Qa","qa", 0);
  run->AddTask(richQa);



  // ------------------------------------------------------------------------

  // ===                 End of RICH local reconstruction                  ===
  // =========================================================================





  // =========================================================================
  // ===                        ECAL reconstruction                        ===
  // =========================================================================

//  // -----   ECAL hit producer  ----------------------------------------------
//  CbmEcalHitProducerFastMC* ecalHitProd
//    = new CbmEcalHitProducerFastMC("ECAL Hitproducer");
//  run->AddTask(ecalHitProd);
  // -------------------------------------------------------------------------

  // ===                      End of ECAL reconstruction                   ===
  // =========================================================================





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
  // ------------------------------------------------------------------------



  // -----   Intialise and run   --------------------------------------------
  run->LoadGeometry();
  run->Init();
  cout << "Starting run" << endl;
  run->Run(0,nEvents);
  // ------------------------------------------------------------------------



  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------

  cout << " Test passed" << endl;
  cout << " All ok " << endl;
  exit(0);
}
