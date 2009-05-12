/**
 * Performs automatic segmentation of Much stations based on
 * 5% occupancy criterion.
 *
 * @author M.Ryzhinskiy m.ryzhinskiy@gsi.de
 * @param mcFile    Input transport file name
 * @param digiFile  Output file name containing segmentation parameters
 * @param nEvents   Number of events to process
 */
void much_seg_auto(const char* mcFile = "",
              const char* digiFile = "",
	      Int_t nEvents = 2)
{
  // ========================================================================
  //          Adjust this part according to your requirements

  if (mcFile == "") {
    mcFile = "data/Jpsi.auau.25gev.centr.mc.root";
  }
  if (digiFile == "") {
    digiFile = "data/much_digi.root";
  }

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
  parIo1->open(gFile);
  rtdb->setFirstInput(parIo1);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();
  fRun->LoadGeometry();
  // ------------------------------------------------------------------------


  // -----  Segmentation task  ----------------------------------------------
  CbmMuchSegmentAuto* seg = new CbmMuchSegmentAuto(digiFile);

  // Number of stations
  seg->SetNStations(6);

  // Set minimum allowed resolution for each station
  Double_t sigmaXmin[] = {0.04, 0.04, 0.04, 0.04, 0.04, 0.04};
  Double_t sigmaYmin[] = {0.04, 0.04, 0.04, 0.04, 0.04, 0.04};
  seg->SetSigmaMin(sigmaXmin, sigmaYmin);

  // Set maximum allowed resolution for each station
  Double_t sigmaXmax[] = {0.32, 0.32, 0.32, 0.32, 0.32, 0.32};
  Double_t sigmaYmax[] = {0.32, 0.32, 0.32, 0.32, 0.32, 0.32};
  seg->SetSigmaMax(sigmaXmax, sigmaYmax);

  // Set maximum occupancy for each station
  Double_t occupancyMax[] = {0.05, 0.05, 0.05, 0.05, 0.05, 0.05};
  seg->SetOccupancyMax(occupancyMax);

  fRun->AddTask(seg);
  // ------------------------------------------------------------------------

  // Run segmentation
  fRun->Init();
  fRun->Run(0,nEvents);

  cout << " Test passed" << endl;
  cout << " All ok " << endl;
  exit(0);
}
