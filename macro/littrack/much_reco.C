#include "../../cbmbase/CbmDetectorList.h";
void much_reco(Int_t nEvents = 100)
{
  Int_t iVerbose = 0;

  TString dir = "/home/d/andrey/events/newmuch/standard/10mu/mu_urqmd/";

  TString inFile = dir + "mc.root";
  TString inFile1 = dir + "sts.reco.root";
  TString inFile2 = dir + "much.hits.root";
  TString parFile = dir + "params.root";
  TString outFile = dir + "much.tracks.root";

  TStopwatch timer;
  timer.Start();

  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
  cbmrootlibs();

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *run= new FairRunAna();
  run->SetInputFile(inFile);
  run->AddFriend(inFile1);
  run->AddFriend(inFile2);
  run->SetOutputFile(outFile);

  FairGeane* Geane = new FairGeane(inFile.Data());

  CbmMuchTrackFinder* muchTrackFinder = new CbmLitMuchTrackFinderRobust();
  CbmMuchFindTracks* muchFindTracks = new CbmMuchFindTracks("Much Track Finder");
  muchFindTracks->UseFinder(muchTrackFinder);
  run->AddTask(muchFindTracks);

//  CbmL1MuchFinder *MuchFinder = new CbmL1MuchFinder();
//  run->AddTask(MuchFinder);

  CbmMuchMatchTracks* muchMatchTracks = new CbmMuchMatchTracks();
  run->AddTask(muchMatchTracks);

  CbmLitRecQa* muchRecQa = new CbmLitRecQa(12, 0.7, kMUCH, 1);
  muchRecQa->SetNormType(2); // '2' to number of STS tracks
  run->AddTask(muchRecQa);


  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  FairParRootFileIo* parIo1 = new FairParRootFileIo();
  parIo1->open(parFile.Data());
  rtdb->setFirstInput(parIo1);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();
  // ------------------------------------------------------------------------

  // -----   Intialise and run   --------------------------------------------
  run->LoadGeometry();
  run->Init();

  Geane->SetField(run->GetField());

  run->Run(0,nEvents);
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
}
