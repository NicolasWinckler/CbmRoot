{
  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libCbmBase");
  gSystem->Load("libField");
  gSystem->Load("libGen");
  gSystem->Load("libPassive");
  gSystem->Load("libSts");
  gSystem->Load("libRich");
  gSystem->Load("libTrd");
  gSystem->Load("libTof");
  gSystem->Load("libEcal");
  gSystem->Load("libGlobal");
  gSystem->Load("libKF");
  gSystem->Load("libL1");
  gSystem->Load("libMuch");
  gSystem->Load("libLittrack");
  
  // ========================================================================
  //          Adjust this part according to your requirements

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 1;
  
  TString inFile = "Jpsi.auau.25gev.centr.mc.root";

  // Number of events to process
  Int_t nEvents = 1000;

  // Parameter file
  TString parFile = "Jpsi.auau.25gev.centr.params.root";

  // Sts reco file
  TString stsRecoFile = "Jpsi.auau.25gev.centr.stsreco.root"; 

  // Much reco file
  TString muchRecoFile = "Jpsi.auau.25gev.centr.reco.root";
  
  // Much hits file
  TString muchHitsFile = "Jpsi.auau.25gev.centr.hits.root";  
  
  TString outFile = "dummy.root";  

                    
// -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  
// -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->AddFriend(stsRecoFile);  
  fRun->AddFriend(muchRecoFile);  
  fRun->AddFriend(muchHitsFile);  
  fRun->SetOutputFile(outFile);

// -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo*  parIo1 = new FairParRootFileIo();
  parIo1->open(parFile.Data());
  rtdb->setFirstInput(parIo1);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();
  fRun->LoadGeometry(); 
   
// ----- Qa task ----------------------------------------------------------
  CbmKF *KF = new CbmKF();
  fRun->AddTask(KF);
  
  CbmMuchPhysicsQa *MuchQa = new CbmMuchPhysicsQa();
  MuchQa->SetQaType(1);  //0 - MC, 1 - RECO
  MuchQa->SetQaFileName("TestRECO.gif");
  MuchQa->SetMuchPIdType(0); // 0 - 70% true hits, 1 - all detectors after last absorber
  fRun->AddTask(MuchQa);
 
// -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0,nEvents);

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

