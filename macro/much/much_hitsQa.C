// --------------------------------------------------------------------------
//
// Macro for qualitycheck of the MUCH hit producer 
//
// 
// E.Kryshen 22.11.2007
//
// --------------------------------------------------------------------------
{
  Int_t nEvents = 10;
  Int_t iVerbose = 1;
  
  TString mcFile   = "mc.1000.root";
  TString hitFile  = "much.hits.10.root";
  TString parFile  = "par.1000.root";
  TString outFile  = "dummy.root";
  TString qaFile   = "much.qa.1000.root";

  TString digiFile = "much_digi.400x800mic.par";

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libCbmBase");
  gSystem->Load("libCbmData");
  gSystem->Load("libField");
  gSystem->Load("libGen");
  gSystem->Load("libPassive");
  gSystem->Load("libMvd");
  gSystem->Load("libSts");
  gSystem->Load("libRich");
  gSystem->Load("libTrd");
  gSystem->Load("libTof");
  gSystem->Load("libEcal");
  gSystem->Load("libGlobal");
  gSystem->Load("libKF");
  gSystem->Load("libL1");
  gSystem->Load("libMuch");

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(mcFile);
  fRun->AddFriend(hitFile);
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------


  // -----  Parameter database   --------------------------------------------
  TString muchDigiFile = gSystem->Getenv("VMCWORKDIR");
  muchDigiFile += "/much/parameters/";
  muchDigiFile += digiFile;
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo*  parIo1 = new FairParRootFileIo();
  FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
  parIo1->open(parFile.Data());
  parIo2->open(muchDigiFile.Data(),"in");
  rtdb->setFirstInput(parIo1);
  rtdb->setSecondInput(parIo2);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();
  fRun->LoadGeometry();
  // ------------------------------------------------------------------------

  // ---  MuCh hit finder ---------------------------------------------------
  CbmMuchHitProducerQa* qa = new CbmMuchHitProducerQa("HitQa",2);
  qa->SetPerformanceFileName(qaFile);
  fRun->AddTask(qa);
  // ------------------------------------------------------------------------ 
  
  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0,nEvents);
  // ------------------------------------------------------------------------
}
