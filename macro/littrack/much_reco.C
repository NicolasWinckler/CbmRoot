
void much_reco(Int_t nEvents = 100)
{

  Int_t iVerbose = 0;
     
  TString system  = "auau";  
  TString beam    = "25gev";  
  TString trigger = "centr";
  TString particle = "mu";  
     
  //TString dir = "/d/cbm02/andrey/events/much/compact/signal/";
//  TString dir = "/d/cbm02/andrey/events/much/10stations/new/signal/";
  TString dir = "/d/cbm02/andrey/events/much/10stations/10mu/mu_urqmd/";
//  TString dir = "/d/cbm05/kisselan/cbmroot_Aug07/compactMuCh/";
//  TString dir = "/d/cbm02/andrey/events/much/compact/";


  TString inFile = dir + beam + "/" 
                    + particle + "/mc." + system + "." + beam + "." 
                    + particle + "."
                    + trigger + ".root";  
  
  TString inFile1 = dir + beam + "/" 
                    + particle + "/sts.reco." + system + "." + beam + "." 
                    + particle + "."
                    + trigger + ".root";      
                    
  TString inFile2 = dir + beam + "/" 
                    + particle + "/much.hits." + system + "." + beam + "." 
                    + particle + "."
                    + trigger + ".root";            
  
  TString parFile = dir + beam + "/" 
                    + particle + "/params." + system + "." + beam + "." 
                    + particle + "."
                    + trigger + ".root";  
  
  // Output file
  TString outFile = dir + beam + "/" 
                    + particle + "/much.tracks." + system + "." + beam + "." 
                    + particle + "."
                    + trigger + ".root"; 

//  TString inFile = "/d/cbm05/kisselan/cbmroot_APR08/compactMuCh/25gev/omega/mc.auau.omega.centr.0000.root";
//  TString inFile1 = "/d/cbm05/kisselan/cbmroot_APR08/compactMuCh/25gev/Tracks/StsTracks.auau.omega.centr.0000.root";
//  TString inFile2 = "/d/cbm05/kisselan/cbmroot_APR08/compactMuCh/25gev/omega/MUCHhits.auau.omega.centr.0000.root";
//  TString parFile = inFile;

  gDebug = 0;

  TStopwatch timer;
  timer.Start();

  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
  cbmrootlibs();
  
  // -----   Reconstruction run   -------------------------------------------
  CbmRunAna *run= new CbmRunAna();
  run->SetInputFile(inFile);
  run->AddFriend(inFile1);
  run->AddFriend(inFile2);
//  run->AddFriend(inFile3); 
  run->SetOutputFile(outFile);
  
  CbmGeane* Geane = new CbmGeane(inFile.Data());
  
  
  CbmMuchTrackFinder* muchTrackFinder    = new CbmLitMuchTrackFinderRobust();
  //CbmMuchTrackFinder* muchTrackFinder    = new CbmLitMuchTrackFinderTrigger();
  //CbmMuchTrackFinder* muchTrackFinder    = new CbmMuchTrackFinderIdeal(1);
  CbmMuchFindTracks* muchFindTracks = new CbmMuchFindTracks("Much Track Finder");
  muchFindTracks->UseFinder(muchTrackFinder);
  run->AddTask(muchFindTracks);

//  CbmL1MuchFinder *MuchFinder = new CbmL1MuchFinder();
//  run->AddTask(MuchFinder);
  
  CbmMuchMatchTracks* muchMatchTracks = new CbmMuchMatchTracks(1);
  run->AddTask(muchMatchTracks);
  
  CbmLitMuchRecQa* muchRecQa = new CbmLitMuchRecQa(8, 0.7);
  muchRecQa->SetNormType(2); // '2' to number of STS tracks
  run->AddTask(muchRecQa);

  
  // -----  Parameter database   --------------------------------------------
  CbmRuntimeDb* rtdb = run->GetRuntimeDb();
  CbmParRootFileIo* parIo1 = new CbmParRootFileIo();
  parIo1->open(parFile.Data());
  rtdb->setFirstInput(parIo1);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();
  // ------------------------------------------------------------------------

  // -----   Intialise and run   --------------------------------------------
  run->LoadGeometry();
  run->Init();
  
  Geane->SetField(run->GetField());
  
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
