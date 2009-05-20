/**
 * Performs segmentation of Much stations based on
 * user specified conditions.
 *
 * @author M.Ryzhinskiy m.ryzhinskiy@gsi.de
 * @param mcFile    Input transport file name
 * @param digiFile  Output file name containing segmentation parameters
 * @param nEvents   Number of events to process
 */
void much_seg_manual(const char* mcFile = "",
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
  CbmMuchSegmentManual* seg = new CbmMuchSegmentManual(digiFile);
  // Number of stations
  seg->SetNStations(6);

  // Number of channels for each station
  Int_t nChannels[] = {128, 128, 128, 128, 128, 128};
  seg->SetNChannels(nChannels);

  // Number of regions for each station
  Int_t nRegions[] = {5, 3, 1, 1, 1, 1};
  seg->SetNRegions(nRegions);

  // Set region radii for each station
  Double_t st0_rad[] = {13.99, 19.39, 24.41, 31.51, 64.76};
  Double_t st1_rad[] = {22, 24, 90.95};
  Double_t st2_rad[] = {110.95};
  Double_t st3_rad[] = {135.95};
  Double_t st4_rad[] = {163.45};
  Double_t st5_rad[] = {228.45};
  seg->SetRegionRadii(0, st0_rad);
  seg->SetRegionRadii(1, st1_rad);
  seg->SetRegionRadii(2, st2_rad);
  seg->SetRegionRadii(3, st3_rad);
  seg->SetRegionRadii(4, st4_rad);
  seg->SetRegionRadii(5, st5_rad);

  // Set minimum pad size/resolution [cm] in the center region for each station
  Double_t padLx[] = {0.1486, 0.4, 0.8, 0.8 ,0.8, 0.8};
  Double_t padLy[] = {0.1486, 0.4, 0.8, 0.8 ,0.8, 0.8};
  seg->SetMinPadSize(padLx, padLy);

  // Set maximum allowed pad size/resolution [cm] for each station
  Double_t sigmaX[] = {0.32, 0.32, 0.32, 0.32, 0.32, 0.32};
  Double_t sigmaY[] = {0.32, 0.32, 0.32, 0.32, 0.32, 0.32};
  seg->SetMaxSigma(sigmaX, sigmaY);

  fRun->AddTask(seg);
  // ------------------------------------------------------------------------

  // Run segmentation
  fRun->Init();

  cout << " Test passed" << endl;
  cout << " All ok " << endl;
  exit(0);
}
