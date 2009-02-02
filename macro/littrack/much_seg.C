void much_seg(Int_t nEvents = 100)
{
  TString dir = "/home/d/andrey/events/newmuch/large/10mu/mu_urqmd/";

  TString mcFile = dir + "mc.root";
  TString parFile = dir + "params.root";
  TString digiFile = dir + "much.digi.root";

  // Dummy ROOT file (needed as an output)
  TString outFile  = dir + "dummy.root";

  Int_t iVerbose = 0;

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
  cbmrootlibs();

  // -----   Analysis run   -------------------------------------------------
  CbmRunAna *fRun= new CbmRunAna();
  fRun->SetInputFile(mcFile);
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------

  // -----  Parameter database   --------------------------------------------
  CbmRuntimeDb* rtdb = fRun->GetRuntimeDb();
  CbmParRootFileIo*  parIo1 = new CbmParRootFileIo();
  parIo1->open(parFile.Data());//gFile);
  rtdb->setFirstInput(parIo1);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();
  fRun->LoadGeometry();
  // ------------------------------------------------------------------------


  // -----  Segmentation task  ----------------------------------------------
  CbmMuchSegmentation* seg = new CbmMuchSegmentation(digiFile.Data());
  fRun->AddTask(seg);
  // ------------------------------------------------------------------------

  // Run segmentation
  fRun->Init();
  fRun->Run(0,nEvents);
}
