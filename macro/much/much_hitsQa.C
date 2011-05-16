// --------------------------------------------------------------------------
// Macro for qualitycheck of the MUCH hit producer
// E.Kryshen 22.11.2007
// --------------------------------------------------------------------------
void much_hitsQa(){
  Int_t nEvents = 5;
  Int_t iVerbose = 1;

  TString  dir            = "data/";
  TString  mcFile         = dir + "Jpsi.auau.25gev.centr.mc.root";
  TString  parFile        = dir + "Jpsi.auau.25gev.centr.mc.root";
  TString  digiFile       = dir + "much_digi.root";
  TString  muchHitsFile   = dir + "Jpsi.auau.25gev.centr.muchhits.root";
  TString  outFile        = dir + "Jpsi.auau.25gev.centr.muchhits.qa.root";

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
  //gSystem->Load("libMvd");
  gSystem->Load("libSts");
  gSystem->Load("libRich");
  gSystem->Load("libTrd");
  gSystem->Load("libTof");
  gSystem->Load("libEcal");
  gSystem->Load("libGlobal");
  gSystem->Load("libKF");
  gSystem->Load("libL1");
  gSystem->Load("libMuch");

  gROOT->LoadMacro("$VMCWORKDIR/much/much_histo_style.C");
  much_histo_style();

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(mcFile);
  fRun->AddFriend(muchHitsFile);
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------


  // -----  Parameter database   --------------------------------------------
  TString muchDigiFile = gSystem->Getenv("VMCWORKDIR");
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo*  parIo1 = new FairParRootFileIo();
  parIo1->open(parFile.Data());
  rtdb->setFirstInput(parIo1);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();
  // ------------------------------------------------------------------------

  // ---  MuCh hit finder QA-------------------------------------------------
  CbmMuchHitFinderQa* qa = new CbmMuchHitFinderQa("HitQa",2);
  qa->SetGeoFileName(digiFile);
  qa->SetPerformanceFileName(outFile);
  qa->SetPrintToFile(0);
  qa->SetPullsQa(1);
  qa->SetOccupancyQa(1);
  qa->SetDigitizerQa(1);
  qa->SetStatisticsQa(1);
  qa->SetClusterDeconvQa(1);

  fRun->AddTask(qa);
  // ------------------------------------------------------------------------

  // -----   Initialize and run --------------------------------------------
  fRun->Init();
  fRun->Run(0,nEvents);
  // ------------------------------------------------------------------------
}
