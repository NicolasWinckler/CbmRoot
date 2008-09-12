#include "../../cbmbase/CbmDetectorList.h";
void trd_reco(Int_t nEvents = 100)
{
  Int_t iVerbose = 1;
  
  TString dir  = "/d/cbm02/andrey/events/trd/standard/e_urqmd";
  TString inFile = dir + "/mc.auau.25gev.e.centr.root"; 
  TString parFile = dir + "/params.root"; 
  TString inFile1 = dir + "/sts.reco.auau.25gev.e.centr.root";
  TString outFile = dir + "/trd.reco.auau.25gev.e.centr.root";

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
  run->SetOutputFile(outFile);
  // ------------------------------------------------------------------------
  
 // CbmKF* kalman = new CbmKF();
//  run->AddTask(kalman);
 // CbmL1* l1 = new CbmL1();
//  run->AddTask(l1);

  // =========================================================================
  // ===                     TRD local reconstruction                      ===
  // =========================================================================

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
  CbmTrdHitProducer* trdHitProd = new CbmTrdHitProducer("TRD Hitproducer",
                                                        "TRD task");
  trdHitProd->SetPar(trdNFoils, trdDFoils, trdDGap);
  trdHitProd->SetSigmaX(trdSigmaX);
  trdHitProd->SetSigmaY(trdSigmaY1, trdSigmaY2, trdSigmaY3);
  run->AddTask(trdHitProd);
  // -------------------------------------------------------------------------


  // -----   TRD track finding   ---------------------------------------------
  //CbmTrdTrackFinder* trdTrackFinder    = new CbmL1TrdTrackFinderSts();
  //((CbmL1TrdTrackFinderSts*)trdTrackFinder)->SetVerbose(iVerbose);
  CbmTrdTrackFinder* trdTrackFinder    = new CbmLitTrdTrackFinderSts();
  CbmTrdFindTracks* trdFindTracks = new CbmTrdFindTracks("TRD Track Finder");
  trdFindTracks->UseFinder(trdTrackFinder);
  run->AddTask(trdFindTracks);
  // -------------------------------------------------------------------------

  // -----   TRD track matching   --------------------------------------------
  CbmTrdMatchTracks* trdMatchTracks = new CbmTrdMatchTracks(iVerbose);
  run->AddTask(trdMatchTracks);
  
    // -----   TRD track finding QA check   ------------------------------------
  CbmLitRecQa* trdRecQa = new CbmLitRecQa(12, 0.7, kTRD, 1);
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
