
void sts_reco(Int_t nEvents = 1000)
{

  // ========================================================================
  //          Adjust this part according to your requirements

  TString system  = "auau";  
  TString beam    = "25gev";  
  TString trigger = "centr";
  TString particle = "mu";  
   
  //TString dir = "/d/cbm02/andrey/events/much/compact/signal/";
 // TString dir = "/d/cbm02/andrey/events/much/10stations/new/";
 // TString dir = "/d/cbm02/andrey/events/much/10stations/new/wofield/signal/";
 // TString dir = "/d/cbm02/andrey/events/much/10stations/new/signal/";
  //TString dir = "/d/cbm02/andrey/events/trd/25gev";
  TString dir = "/d/cbm02/andrey/events/much/10stations/10mu/urqmd/";
  
  
  TString inFile = dir + beam + "/" 
                    + particle + "/mc." + system + "." + beam + "." 
                    + particle + "."
                    + trigger + ".root";  
  
  TString parFile = dir + beam + "/" 
                    + particle + "/params." + system + "." + beam + "." 
                    + particle + "."
                    + trigger + ".root";  
  
 // TString inFile = dir + "/trd.444.mc.root"; 
 // TString parFile = dir + "/trd.444.params.root"; 
   
  
  // STS digitisation file
 // TString digiFile = "sts_standard.digi.par";
  TString digiFile = "sts_allstrips.digi.par";
  
  // Output file
  TString outFile = dir + beam + "/" 
                    + particle + "/sts.reco." + system + "." + beam + "." 
                    + particle + "."
                    + trigger + ".root"; 

  //TString outFile = dir + "/trd.444.sts.reco.root";
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;
  // ========================================================================

  
    
  // ---   Screen output   --------------------------------------------------  
  cout << "*****************************************" << endl;
  cout << "***   STS RECONSTRUCTION SCRIPT   *******" << endl;
  cout << "*****************************************" << endl;
  cout << "*** Input file       : " << inFile << endl;
  cout << "*** Parameter file   : " << parFile << endl;
  cout << "*** Digitsation file : " << digiFile << endl;
  cout << "*** Output file      : " << outFile << endl;
  cout << "*** Number of events : " << nEvents << endl;
  cout << "*****************************************" << endl;
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
  gSystem->Load("libField");
  gSystem->Load("libPassive");
  gSystem->Load("libMvd");
  gSystem->Load("libSts");
  gSystem->Load("libGen");
  gSystem->Load("libRich");
  gSystem->Load("libTrd");
  gSystem->Load("libTof");
  gSystem->Load("libEcal");
  gSystem->Load("libGlobal");
  gSystem->Load("libKF");
  gSystem->Load("libL1");
  // ------------------------------------------------------------------------

  

  // -----   Reconstruction run   -------------------------------------------
  CbmRunAna* run= new CbmRunAna();
  run->SetInputFile(inFile);
  run->SetOutputFile(outFile);
  // ------------------------------------------------------------------------
  
  
  // -----   STS digitiser   ------------------------------------------------
  CbmTask* stsDigitize = new CbmStsDigitize("STSDigitize", iVerbose);
  run->AddTask(stsDigitize);
  // ------------------------------------------------------------------------
 
  
  
  // ---  STS hit finding   -------------------------------------------------
  CbmTask* findHits = new CbmStsFindHits("STSFindHits", iVerbose);
  run->AddTask(findHits);
  // ------------------------------------------------------------------------
   
  
    
  // ---  STS hit matching   ------------------------------------------------
  CbmTask* matchHits = new CbmStsMatchHits("STSMatchHits", 
                  iVerbose);
  run->AddTask(matchHits);
  // ------------------------------------------------------------------------
   

   
  // -----   STS track finding   --------------------------------------------
  CbmTask* kalman= new CbmKF();
  run->AddTask(kalman);
  CbmTask* l1 = new CbmL1();
  run->AddTask(l1);
  CbmStsTrackFinder* trackFinder    = new CbmL1StsTrackFinder();
  CbmTask* findTracks = new CbmStsFindTracks(iVerbose, trackFinder);
  run->AddTask(findTracks);
  // ------------------------------------------------------------------------

 
         
  // -----   STS track matching   -------------------------------------------
  CbmTask* matchTracks = new CbmStsMatchTracks("Match tracks", iVerbose);
  run->AddTask(matchTracks);
  // ------------------------------------------------------------------------
  

  
  // -----   STS track fitting   --------------------------------------------
  CbmStsTrackFitter* trackFitter = new CbmStsKFTrackFitter();
  CbmTask* fitTracks = new CbmStsFitTracks("STS Track Fitter",
                  trackFitter,
                  iVerbose);
  run->AddTask(fitTracks);
  // ------------------------------------------------------------------------
  
  
  
  // -----   STS reconstruction QA   ----------------------------------------
  CbmTask* stsRecoQa = new CbmStsReconstructionQa(kFALSE, 4, 0.7, 1);
  run->AddTask(stsRecoQa);
  // ------------------------------------------------------------------------
  
   

  // -----  Parameter database   --------------------------------------------
  TString stsDigiFile = gSystem->Getenv("VMCWORKDIR");
  stsDigiFile += "/parameters/sts/";
  stsDigiFile += digiFile;
  CbmRuntimeDb* rtdb = run->GetRuntimeDb();
  CbmParRootFileIo*  parIo1 = new CbmParRootFileIo();
  CbmParAsciiFileIo* parIo2 = new CbmParAsciiFileIo();
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
