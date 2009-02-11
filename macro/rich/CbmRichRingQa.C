/*  Description: This macro run the CbmRichRingQa class to
   create the histogramms.

    Author : Simeon Lebedev
    E-mail : S.Lebedev@gsi.de
*/

void CbmRichRingQa(Int_t nEvents = 100)
{

  // ========================================================================
  //          Adjust this part according to your requirements

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 1;
  Int_t normType = 0; //0 - normalize by number of MC hits
                      //1 - normalize by number of found hits;
  
  // Input file (MC events)
  //TString inFile = "../run/data/test.mc.root";
  TString inFile ="/d/cbm05/hoehne/data/sim/svn/urqmd.auau.25gev.centr.5e+5e-.0004.mc.AUG07.root";

  // Parameter file
  //TString parFile = "../run/data/params.root";
  TString parFile ="/d/cbm05/hoehne/data/sim/svn/urqmd.auau.25gev.centr.5e+5e-.0004.mc.AUG07.params.root";

  // STS digitisation file
  TString stsDigiFile = "sts_allstrips.digi.par";
  
  // reconstructed data
  //TString recoFile = "../run/data/test.eds.root";
  TString recoFile = "/d/cbm05/hoehne/data/reco/svn/urqmd.auau.25gev.centr.5e+5e-.0004.reco.AUG07.root";

  // Output file
  TString outFile = "reco.richqa.root";


  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------


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
  // ------------------------------------------------------------------------


  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->AddFriend(recoFile);
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------

  // -----  Parameter database   --------------------------------------------
  TString stsDigi = gSystem->Getenv("VMCWORKDIR");
  stsDigi += "/parameters/sts/";
  stsDigi += stsDigiFile;
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parIo1 = new FairParRootFileIo();
  FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
  parIo1->open(parFile.Data());
  parIo2->open(stsDigi.Data(),"in");
  rtdb->setFirstInput(parIo1);
  rtdb->setSecondInput(parIo2);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();
  // ------------------------------------------------------------------------


  CbmRichRingQa* richQa   =  new CbmRichRingQa("Qa","qa", iVerbose,normType);
  fRun->AddTask(richQa);

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0,nEvents);
  // ------------------------------------------------------------------------

  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------

exit(0);
}
