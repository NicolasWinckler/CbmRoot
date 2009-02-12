// --------------------------------------------------------------------------
//
// Macro for visualization of MC points and reconstructed hits 
// in the CBM muon setup: 
//
// 
// E.Kryshen 22.11.2007
//
// --------------------------------------------------------------------------
void much_vis(const char* mcFile, const char* rcFile, const char* digiFile)
{
  TString outFile  = "data/dummy.root";

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libCbmBase");
  gSystem->Load("libCbmData");
  gSystem->Load("libPassive");
  gSystem->Load("libGen");
  gSystem->Load("libSts");
  gSystem->Load("libMuch");
  gSystem->Load("libVis");

  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(mcFile);
  fRun->AddFriend(rcFile);
  fRun->SetOutputFile(outFile);

  CbmVisMuch* vis = new CbmVisMuch();
  vis->SetDigiFileName(digiFile);
  fRun->AddTask(vis);
  fRun->Init();
}
