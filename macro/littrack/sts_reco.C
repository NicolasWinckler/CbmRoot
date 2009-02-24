void sts_reco(Int_t nEvents = 5)
{
  TString dir = "/home/d/andrey/events/newmuch/standard/10mu/mu_urqmd/";
//  TString dir = "/d/cbm02/andrey/events/muchstraw/large/10mu/mu/";
//  TString dir  = "/d/cbm02/andrey/events/trd/segmented/e/";
  TString inFile = dir + "mc.root";
  TString parFile = dir + "params.root";
  TString outFile = dir + "sts.reco.root";

//  TString digiFile = "sts_standard.digi.par";
  TString digiFile = "sts_Standard_s3055AAFK5.SecD.digi.par";//"sts_allstrips.digi.par";

  Int_t iVerbose = 1;

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
  cbmrootlibs();
  // ------------------------------------------------------------------------

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna* run= new FairRunAna();
  run->SetInputFile(inFile);
  run->SetOutputFile(outFile);
  // ------------------------------------------------------------------------

  // =========================================================================
  // ===                     MVD local reconstruction                      ===
  // =========================================================================
// -----   MVD Hitproducer   -----------------------------------------------
//  CbmMvdHitProducer* mvdHitProd = new CbmMvdHitProducer("MVDHitProducer", 0,
//  			  			       iVerbose);
//  run->AddTask(mvdHitProd);
// -------------------------------------------------------------------------

// ===                 End of MVD local reconstruction                   ===
// =========================================================================
// -----   STS digitiser   ------------------------------------------------
  FairTask* stsDigitize = new CbmStsDigitize("STSDigitize", iVerbose);
  run->AddTask(stsDigitize);
// ------------------------------------------------------------------------

// ---  STS hit finding   -------------------------------------------------
  FairTask* findHits = new CbmStsFindHits("STSFindHits", iVerbose);
  run->AddTask(findHits);
// ------------------------------------------------------------------------

// ---  STS hit matching   ------------------------------------------------
  FairTask* matchHits = new CbmStsMatchHits("STSMatchHits",
                  iVerbose);
  run->AddTask(matchHits);
// ------------------------------------------------------------------------


  // -----   STS track finding   --------------------------------------------
  FairTask* kalman= new CbmKF();
  run->AddTask(kalman);
  FairTask* l1 = new CbmL1();
  run->AddTask(l1);
  CbmStsTrackFinder* trackFinder    = new CbmL1StsTrackFinder();
  FairTask* findTracks = new CbmStsFindTracks(iVerbose, trackFinder);
  run->AddTask(findTracks);
  // ------------------------------------------------------------------------



  // -----   STS track matching   -------------------------------------------
  FairTask* matchTracks = new CbmStsMatchTracks("Match tracks", iVerbose);
  run->AddTask(matchTracks);
  // ------------------------------------------------------------------------



  // -----   STS track fitting   --------------------------------------------
  CbmStsTrackFitter* trackFitter = new CbmStsKFTrackFitter();
  FairTask* fitTracks = new CbmStsFitTracks("STS Track Fitter",
                  trackFitter,
                  iVerbose);
  run->AddTask(fitTracks);
  // ------------------------------------------------------------------------



  // -----   STS reconstruction QA   ----------------------------------------
  FairTask* stsRecoQa = new CbmStsReconstructionQa(kFALSE, 4, 0.7, 2);
 run->AddTask(stsRecoQa);
  // ------------------------------------------------------------------------



  // -----  Parameter database   --------------------------------------------
  TString stsDigiFile = gSystem->Getenv("VMCWORKDIR");
  stsDigiFile += "/parameters/sts/";
  stsDigiFile += digiFile;
  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  FairParRootFileIo*  parIo1 = new FairParRootFileIo();
  FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
  parIo1->open(parFile.Data());
  parIo2->open(stsDigiFile.Data(),"in");
  rtdb->setFirstInput(parIo1);
  rtdb->setSecondInput(parIo2);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();
  // ------------------------------------------------------------------------



  // -----   Initialise and run   -------------------------------------------
  run->LoadGeometry();
  run->Init();
  run->Run(0, nEvents);
  // ------------------------------------------------------------------------



  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "         << outFile << endl;
  cout << "Parameter file is "      << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------


}
