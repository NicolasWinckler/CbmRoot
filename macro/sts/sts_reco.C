// --------------------------------------------------------------------------
//
// Macro for reconstruction in STS from MC data
//
// Tasks:  CbmStsDigitise
//         CbmStsFindHits
//         CbmStsMatchHits
//         CbmStsFindTracks
//         CbmStsMatchTracks
//         CbmStsFitTracks
//         CbmStsReconstructionQa
//
// V. Friese   12/09/2006
// Version     24/04/2007 (V. Friese)
//
// --------------------------------------------------------------------------

#include <math.h>
void sts_reco_real(Int_t nEvents = 1) {

  // ========================================================================
  //          Adjust this part according to your requirements
  
  // Input file (MC events)
  TString inFile = "sts.mc.root";
  
  // Parameter file
  TString parFile = "params.root";
  
  // STS digitisation file
  TString digiFile = "sts_standard.digi.par";
  
  // Output file
  TString outFile = "sts.reco.root";

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;
  // ========================================================================

  
  Double_t threshold  =  4;
  Double_t noiseWidth =  0.1;
  Int_t    nofBits    = 20;
  Double_t minStep    =  0.01,;
  Double_t StripDeadTime = 10.;
  
  StripDeadTime = StripDeadTime/100.;

  // ---   Screen output   --------------------------------------------------  
  cout << "***************************************************" << endl;
  cout << "***   STS REALISTIC RECONSTRUCTION SCRIPT   *******" << endl;
  cout << "***************************************************" << endl;
  cout << "*** Input file        : " << inFile << endl;
  cout << "*** Parameter file    : " << parFile << endl;
  cout << "*** Digitisation file : " << digiFile << endl;
  cout << "*** Output file       : " << outFile << endl;
  cout << "*** Number of events  : " << nEvents << endl;
  cout << "***************************************************" << endl;
  cout << endl;
  cout << "=============================================" << endl;
  cout << "===     Realistic response settings  ========" << endl;
  cout << "=============================================" << endl;
  cout << "===    threshold  = " << threshold  << endl;
  cout << "===    noiseWidth = " << noiseWidth << endl;
  cout << "===    nofBits    = " << nofBits    << endl;
  cout << "===    minStep    = " << minStep    << endl;
  cout << "===    StripDeadTime    = " << StripDeadTime*100. << "[ns] " << endl;
  cout << "=============================================" << endl;
  cout << endl << endl;
 // ------------------------------------------------------------------------


        
  // ---  ROOT settings   ---------------------------------------------------
  gStyle->SetPalette(1);  
  gDebug = 0;
  // ------------------------------------------------------------------------
 

  
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

    
  
  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libCbmBase");
  gSystem->Load("libCbmData");
  gSystem->Load("libField");
  gSystem->Load("libPassive");
  gSystem->Load("libMvd");
  gSystem->Load("libSts");
  gSystem->Load("libGen");
  gSystem->Load("libRich");
  gSystem->Load("libTrd");
  gSystem->Load("libTof");
  gSystem->Load("libGlobal");
  gSystem->Load("libEcal");
  gSystem->Load("libKF");
  gSystem->Load("libL1");
  // ------------------------------------------------------------------------

  

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna* run= new FairRunAna();
  run->SetInputFile(inFile);
  run->SetOutputFile(outFile);
  // ------------------------------------------------------------------------

  // -----   MVD Hitproducer   ----------------------------------------------
  CbmMvdHitProducer* hitProd = new CbmMvdHitProducer("MVDHitProducer", 0, iVerbose);
  run->AddTask(hitProd);
  // ------------------------------------------------------------------------

  // -----   STS digitiser   ------------------------------------------------
  //  CbmTask* 
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
  // ------------------------------------------------------------------------
 
  CbmStsClusterFinder* findClusters = new CbmStsClusterFinder("STSFindClusters", iVerbose);
  run->AddTask(findClusters);

  
  // ---  STS hit finding   -------------------------------------------------
  //  CbmTask* 
  CbmStsFindHits* findHits = new CbmStsFindHits("STSFindHits", iVerbose);
  run->AddTask(findHits);

  
  // ---  STS hit matching   ------------------------------------------------
  //  CbmTask*
  CbmStsMatchHits* matchHits = new CbmStsMatchHits("STSMatchHits",iVerbose);
  matchHits->SetRealisticResponse();
  run->AddTask(matchHits);
  // ------------------------------------------------------------------------
  
  
  // -----   STS track finding   --------------------------------------------
  FairTask* kalman= new CbmKF();
  run->AddTask(kalman);
  FairTask* l1 = new CbmL1();
  run->AddTask(l1);
  CbmStsTrackFinder* trackFinder    = new CbmL1StsTrackFinder();
  FairTask* findTracks = new CbmStsFindTracks(iVerbose, trackFinder, kTRUE);
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
   FairTask* stsFHQa = new CbmStsFindHitsQa("STSFindHitsQA",iVerbose);
   run->AddTask(stsFHQa);
   // ------------------------------------------------------------------------
/*  // -----   STS reconstruction QA   ----------------------------------------
   FairTask* stsRecoQa = new CbmStsReconstructionQa(kFALSE, 4, 0.7, 0);
   run->AddTask(stsRecoQa);*/
 // ------------------------------------------------------------------------

  

  // -----  Parameter database   --------------------------------------------
  TString stsDigiFile = gSystem->Getenv("VMCWORKDIR");
  stsDigiFile += "/parameters/sts/";
  stsDigiFile += digiFile;
  cout << "digi file = " << stsDigiFile << endl;
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

/*
RefPrim   efficiency   : 0.9518 | 28331
RefSec    efficiency   : 0.7498 | 3098
Refset    efficiency   : 0.9272 | 31429
Allset    efficiency   : 0.8858 | 40750
ExtraPrim efficiency   : 0.8847 | 7438
ExtraSec  efficiency   : 0.5091 | 1883
Extra     efficiency   : 0.7699 | 9321
Clone     probability  : 0.0000 | 0
Ghost     probability  : 0.0349 | 1422
MC tracks/event found  : 565

+ STSFindTracks  :   0.0052 s, tracks found 595
+ Match tracks   :   0.0021 s, matches 595, hit quota 95.4026 %
+ STS Track Fitter:   0.6635 s, tracks fitted 595
there are 4845 points and 20116 hits.
----------   StsReconstructionQa : Event summary   ------------
MCTracks   : 1984, reconstructable: 706, reconstructed: 573
Vertex     : reconstructable: 578, reconstructed: 529, efficiency 91.5225%
Reference  : reconstructable: 425, reconstructed: 399, efficiency 93.8824%
Non-vertex : reconstructable: 128, reconstructed: 44, efficiency 34.3750%
STSTracks 595, ghosts 22, clones 0
-----------------------------------------------------------

Got out
*/
