void much_ana(char_t* dir = "data", Int_t fileNo=0, Int_t nEvents=10){
  TString inFile         = dir + Form("/%02i/",fileNo) + "mc.root";
  TString stsFile        = dir + Form("/%02i/",fileNo) + "sts.reco.root";
  TString muchHitsFile   = dir + Form("/%02i/",fileNo) + "much.hits.root";
  TString muchTracksFile = dir + Form("/%02i/",fileNo) + "much.tracks.root";
  TString outFile        = dir + Form("/%02i/",fileNo) + "ana.root";
  TString histoFile      = dir + Form("/%02i/",fileNo) + "anaHisto.root";
  
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gROOT->LoadMacro("$VMCWORKDIR/macro/much/muchlibs.C");
  muchlibs();
  gROOT->LoadMacro("$VMCWORKDIR/analysis/hyperon/hyperon_style.C");
  hyperon_style();

  
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->AddFriend(stsFile);
  fRun->AddFriend(muchHitsFile);
  fRun->AddFriend(muchTracksFile);
  fRun->SetOutputFile(outFile);

  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo*  parIo1 = new FairParRootFileIo();
  parIo1->open(gFile);
  rtdb->setFirstInput(parIo1);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();

  CbmAnaMuch* ana = new CbmAnaMuch("MuchAnalysis",histoFile);
  ana->SetVerbose(1);
  ana->SetStsPointsAccQuota(4);
  ana->SetStsTrueHitQuota(0.7);
  fRun->AddTask(ana);

  fRun->Init();
  fRun->Run(0,nEvents);
}
