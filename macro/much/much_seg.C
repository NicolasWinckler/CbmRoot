// --------------------------------------------------------------------------
//
// Macro for segmentation of MUCH stations 
//
// To be defined as arguments:
//    MC transport file
//    Output digitization file ("*.root")
//    Number of events to be processed (default = 1)
//    First event to be processed (default = 1)
// 
// M.Ryzhinskiy
//
// --------------------------------------------------------------------------

void much_seg(const char* mcFile, const char* digiFile,
	      Int_t nEvents = 10, Int_t iFirst = 1)
{
  // ========================================================================
  //          Adjust this part according to your requirements
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;

  // Dummy ROOT file (neede as an output)
  TString outFile  = "data/dummy.root";
  
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
  // ------------------------------------------------------------------------

  // -----   Analysis run   -------------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(mcFile);
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------

  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo*  parIo1 = new FairParRootFileIo();
  parIo1->open(gFile);
  rtdb->setFirstInput(parIo1);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();
  fRun->LoadGeometry();
  // ------------------------------------------------------------------------


  // -----  Segmentation task  ----------------------------------------------
  CbmMuchSegmentation* seg = new CbmMuchSegmentation(digiFile);
  fRun->AddTask(seg);
  // ------------------------------------------------------------------------

  // Run segmentation
  fRun->Init();
  fRun->Run(iFirst,nEvents);
}
