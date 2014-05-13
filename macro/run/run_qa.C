// --------------------------------------------------------------------------
//
// Macro for STS QA
//
// V. Friese   13/01/2006
//
// --------------------------------------------------------------------------


run_qa()
{

  // ========================================================================
  //          Adjust this part according to your requirements

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 1;

  // MC file
  TString simFile = "data/test.mc.root";

  // Reco file
  TString recFile = "data/test.eds.root";

  // Number of events to process
  Int_t nEvents = 1;

  // Parameter file
  TString parFile = "data/params.root";

  // Output file
  TString outFile = "data/test.qa.root";
  
  
  // STS digitisation file
  TString stsDigiFile = "sts_v13d_std.digi.par";

  // In general, the following parts need not be touched
  // ========================================================================



  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  // -----  Analysis run   --------------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(simFile);
  fRun->AddFriend(recFile);
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------


  // -----  Parameter database   --------------------------------------------
  TString stsDigi = gSystem->Getenv("VMCWORKDIR");
  stsDigi += "/parameters/sts/";
  stsDigi += stsDigiFile;
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parIo1 = new FairParRootFileIo();
  FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
  parIo1->open(parFile.Data());
  parIo2->open(stsDigi.Data(),"in");
  rtdb->setFirstInput(parIo1);
  rtdb->setSecondInput(parIo2);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();
  // ------------------------------------------------------------------------


  // -----   STS track finder QA   ------------------------------------------
  CbmStsFindTracksQa* stsTrackFindQa = new CbmStsFindTracksQa(4, 0.7,
							      iVerbose);
  fRun->AddTask(stsTrackFindQa);
  // ------------------------------------------------------------------------
    
    // -------- Performance of PID algorithm ------------
    CbmTrdTracksPidQa*  trdTrackPidQa =
        new CbmTrdTracksPidQa("PidPerformance");
    fRun->AddTask(trdTrackPidQa);
    // -------------------------------------------------
    
 // ------  Rich ring finder QA ---------------------------------------------
   Int_t normType = 1; //0 - normalize by number of MC hits
                      //1 - normalize by number of found hits;
//  CbmRichRingQa* richQa   =  new CbmRichRingQa("Qa","qa", iVerbose,normType);
//  fRun->AddTask(richQa);

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0,nEvents);
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


}
