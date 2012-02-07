/**
 * Performs segmentation of Much stations based on
 * user specified conditions.
 *
 * @author M.Ryzhinskiy m.ryzhinskiy@gsi.de
 * @param mcFile      Input transport file name
 * @param inDigiFile  Input file name containing initial segmentation parameters
 * @param outDigiFile Output file name containing segmentation parameters
 */
void much_seg_sector(const char* mcFile = "",
                       const char* inDigiFile = "",
                       const char* outDigiFile = "")
{
  // ========================================================================
  //          Adjust this part according to your requirements

  if (mcFile == "") {
    mcFile = "data/Jpsi.auau.25gev.centr.mc.root";
  }
  if (inDigiFile == "") {
    inDigiFile = "data/much_digi_sector.seg";
  }
  if (outDigiFile == "") {
    outDigiFile = "data/much_digi_sector.root";
  }

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;

  // Dummy ROOT file (needed as an output)
  TString outFile  = "data/dummy.root";
  TString parFile = "data/params.root";

  // ----  Load libraries   -------------------------------------------------
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
//  gSystem->Load("libMvd");
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
  parIo1->open(parFile);
  rtdb->setFirstInput(parIo1);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();
  // ------------------------------------------------------------------------

  // -----  Segmentation task  ----------------------------------------------
  CbmMuchSegmentSector* seg = new CbmMuchSegmentSector(inDigiFile, outDigiFile);
  seg->DebugSwitchOn();
  fRun->AddTask(seg);
  // ------------------------------------------------------------------------

  // Run segmentation
  fRun->Init();

  cout << " Test passed" << endl;
  cout << " All ok " << endl;
}
