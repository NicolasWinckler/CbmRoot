#include "../../cbmbase/CbmDetectorList.h";
void trd_reco(Int_t nEvents = 20)
{
  Int_t iVerbose = 1;

  TString dir  = "/d/cbm02/andrey/events/trd/segmented/e_urqmd/";
  TString inFile = dir + "mc.root";
  TString parFile = dir + "params.root";
  TString inFile1 = dir + "sts.reco.root";
  TString inFile2 = dir + "trd.hits.root";
  TString outFile = dir + "trd.reco.root";

  TStopwatch timer;
  timer.Start();

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
  cbmrootlibs();

  // -----   Reconstruction run   -------------------------------------------
  CbmRunAna *run= new CbmRunAna();
  run->SetInputFile(inFile);
  run->AddFriend(inFile1);
  run->AddFriend(inFile2);
  run->SetOutputFile(outFile);
  // ------------------------------------------------------------------------

 // CbmKF* kalman = new CbmKF();
//  run->AddTask(kalman);
 // CbmL1* l1 = new CbmL1();
//  run->AddTask(l1);

  // -----   TRD track finding   ---------------------------------------------
  //CbmTrdTrackFinder* trdTrackFinder    = new CbmL1TrdTrackFinderSts();
  //((CbmL1TrdTrackFinderSts*)trdTrackFinder)->SetVerbose(iVerbose);
  CbmTrdTrackFinder* trdTrackFinder    = new CbmLitTrdTrackFinderSts();
  CbmTrdFindTracks* trdFindTracks = new CbmTrdFindTracks("TRD Track Finder");
  trdFindTracks->UseFinder(trdTrackFinder);
  run->AddTask(trdFindTracks);
  // -------------------------------------------------------------------------

  // -----   TRD track matching   --------------------------------------------
  CbmTrdMatchTracks* trdMatchTracks = new CbmTrdMatchTracks(1);
  run->AddTask(trdMatchTracks);

    // -----   TRD track finding QA check   ------------------------------------
  CbmLitRecQa* trdRecQa = new CbmLitRecQa(10, 0.7, kTRD, 1);
  trdRecQa->SetNormType(2); // '2' to number of STS tracks
  run->AddTask(trdRecQa);



  // -----  Parameter database   --------------------------------------------
 // TString stsDigi = gSystem->Getenv("VMCWORKDIR");
//  stsDigi += "/parameters/sts/";
 // stsDigi += stsDigiFile;
  CbmRuntimeDb* rtdb = run->GetRuntimeDb();
  CbmParRootFileIo* parIo1 = new CbmParRootFileIo();
  CbmParAsciiFileIo* parIo2 = new CbmParAsciiFileIo();
  parIo1->open(parFile.Data());
//  parIo2->open(stsDigi.Data(),"in");
  rtdb->setFirstInput(parIo1);
//  rtdb->setSecondInput(parIo2);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();
  // ------------------------------------------------------------------------

  // -----   Intialise and run   --------------------------------------------
  run->LoadGeometry();
  run->Init();
  run->Run(0,nEvents);
  // ------------------------------------------------------------------------


  CbmLitRobustAna::Instance()->WriteToFile();

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

  cout << " Test passed" << endl;
  cout << " All ok " << endl;
  exit(0);
}
