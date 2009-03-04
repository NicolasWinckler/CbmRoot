#include "../../cbmbase/CbmDetectorList.h";
void trd_ana(Int_t nEvents = 1000)
{
  Int_t iVerbose = 0;
  
  TString dir  = "/d/cbm02/andrey/events/trd/standard/e";
  TString inFile = dir + "/mc.auau.25gev.e.centr.root"; 
  TString parFile = dir + "/params.root"; 
  TString inFile1 = dir + "/sts.reco.auau.25gev.e.centr.root";
  TString outFile = dir + "/trd.ana.auau.25gev.e.centr.root";

  TStopwatch timer;
  timer.Start();

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
  cbmrootlibs();

  FairRunAna *run= new FairRunAna();
  run->SetInputFile(inFile);
  run->AddFriend(inFile1);
  run->SetOutputFile(outFile);
  
  
  // -------------------------------------------------------------------------
  // -----   TRD hit producer   ----------------------------------------------
  Double_t trdSigmaX[] = {300, 400, 500};             // Resolution in x [mum]
  // Resolutions in y - station and angle dependent [mum]
  Double_t trdSigmaY1[] = {2700,   3700, 15000, 27600, 33000, 33000, 33000 };
  Double_t trdSigmaY2[] = {6300,   8300, 33000, 33000, 33000, 33000, 33000 };
  Double_t trdSigmaY3[] = {10300, 15000, 33000, 33000, 33000, 33000, 33000 };  

  // Update of the values for the radiator F.U. 17.08.07
  Int_t trdNFoils    = 130;       // number of polyetylene foils
  Float_t trdDFoils = 0.0013;    // thickness of 1 foil [cm]
  Float_t trdDGap   = 0.02;      // thickness of gap between foils [cm]
  CbmTrdHitProducerSmearing* trdHitProd = new CbmTrdHitProducerSmearing("TRD Hitproducer",
                                                        "TRD task");
  trdHitProd->SetPar(trdNFoils, trdDFoils, trdDGap);
  trdHitProd->SetSigmaX(trdSigmaX);
  trdHitProd->SetSigmaY(trdSigmaY1, trdSigmaY2, trdSigmaY3);
  run->AddTask(trdHitProd);
  // -------------------------------------------------------------------------
  
  CbmTrdTrackFinder* trdTrackFinder = new CbmTrdTrackFinderIdeal();
  CbmTrdFindTracks* trdFindTracks = new CbmTrdFindTracks();
  trdFindTracks->UseFinder(trdTrackFinder);
  run->AddTask(trdFindTracks);
  
  CbmTrdMatchTracks* trdMatchTracks = new CbmTrdMatchTracks(1);
  run->AddTask(trdMatchTracks);

  CbmLitPropAna* propAna = new CbmLitPropAna(kTRD, 12);
  run->AddTask(propAna);
  // -------------------------------------------------------------------------


  // -----  Parameter database   -------------------------------------------
  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  FairParRootFileIo* parIo1 = new FairParRootFileIo();
  FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
  parIo1->open(parFile.Data());
  rtdb->setFirstInput(parIo1);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();
  // ------------------------------------------------------------------------
     
  // -----   Intialise and run   --------------------------------------------
  run->LoadGeometry();
  run->Init();
  cout << "Starting run" << endl;
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

  cout << " Test passed" << endl;
  cout << " All ok " << endl;
  exit(0);
}
