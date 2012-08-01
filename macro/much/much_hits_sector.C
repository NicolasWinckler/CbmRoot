/**
 * @author E. Kryshen <e.kryshen@gsi.de>
 *
 * Digitization of simulated events in MUCH
 **/
void much_hits_sector(){
  TString inFile   = "data/mc_sector.root";
  TString digiFile = "data/much_digi_sector.root";
  TString outFile  = "data/hits_sector.root";
  TString parFile  = "data/params.root";
  Int_t nEvents    = 500;
  Int_t iVerbose   = 1;

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gROOT->LoadMacro("$VMCWORKDIR/macro/much/muchlibs.C");
  muchlibs();

  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);

  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo*  parIo1 = new FairParRootFileIo();
  parIo1->open(parFile);
  rtdb->setFirstInput(parIo1);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();

  CbmMuchDigitizeGem* digitize = new CbmMuchDigitizeGem("MuchDigitizeGem", digiFile, iVerbose);
  digitize->SetSpotRadius(0.05);
  digitize->SetQThreshold(3);
  digitize->SetQMaximum(500000);
  digitize->SetNADCChannels(256);

  fRun->AddTask(digitize);

  fRun->Init();
  fRun->Run(0,nEvents);
}
