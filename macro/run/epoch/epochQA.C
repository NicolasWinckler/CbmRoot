void epochQA(){
  TString dir = "data";
  TString inFile    = dir + "/epoch.root";
  TString parFile   = dir + "/param.0000.root";
  TString outFile   = dir + "/epochQA.root";

  TChain* mcFileChain = new TChain("cbmsim");
  mcFileChain->AddFile(dir+"/mc.0000.root");
  mcFileChain->AddFile(dir+"/mc.0001.root");
  mcFileChain->AddFile(dir+"/mc.0002.root");
  
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libCbmBase");
  gSystem->Load("libCbmData");
  gSystem->Load("libRun");
  
  
  CbmRunAna *fRun= new CbmRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);

  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo*  parIo1 = new FairParRootFileIo();
  parIo1->open(parFile.Data());
  rtdb->setFirstInput(parIo1);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();

  CbmMCEpochQA* epochQA = new CbmMCEpochQA("EpochTest",mcFileChain);
  epochQA->SetVerbose(0);
  fRun->AddTask(epochQA);
  fRun->Init();
  fRun->Run();
}
