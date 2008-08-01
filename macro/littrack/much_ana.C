

void much_ana(Int_t nEvents = 5000)
{

   // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
   Int_t iVerbose = 0;
  
  TString system  = "auau";  
  TString beam    = "25gev";  
  TString trigger = "centr";
  TString particle = "mu";  
   
   // Input file (MC events)
   //TString dir = "/d/cbm02/andrey/events/much/10stations/new/signal/";
   //TString dir = "/d/cbm02/andrey/events/much/10stations/new/wofield/signal/";   
   TString dir = "/d/cbm02/andrey/events/much/10stations/10mu/mu/";
   
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
                    + particle + "/much.ana." + system + "." + beam + "." 
                    + particle + "."
                    + trigger + ".root";  
   
   
   // ----    Debug option   -------------------------------------------------
   gDebug = 0;
   // ------------------------------------------------------------------------
   
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
   gSystem->Load("libTrkBase");
   gSystem->Load("libGeane");
   gSystem->Load("libMvd");
   gSystem->Load("libSts");
   gSystem->Load("libRich");
   gSystem->Load("libTrd");
   gSystem->Load("libTof");
   gSystem->Load("libEcal");
   gSystem->Load("libGlobal");
   gSystem->Load("libKF");
   gSystem->Load("libL1");   
   gSystem->Load("libMuch"); 
   gSystem->Load("libLittrack");
   
   
   // -----   Reconstruction run   -------------------------------------------
   CbmRunAna *run= new CbmRunAna();
   run->SetInputFile(inFile);
   run->AddFriend(inFile1);
   run->AddFriend(inFile2);
   run->SetOutputFile(outFile);
  // ------------------------------------------------------------------------

  
    CbmGeane* Geane = new CbmGeane(inFile.Data());

  
   // CbmMuchTrackFinder* muchTrackFinder    = new CbmLitMuchTrackFinder();
   CbmMuchTrackFinder* muchTrackFinder    = new CbmMuchTrackFinderIdeal(1);
   CbmMuchFindTracks* muchFindTracks = new CbmMuchFindTracks("Much Track Finder");
   muchFindTracks->UseFinder(muchTrackFinder);
   run->AddTask(muchFindTracks);
   
   CbmMuchMatchTracks* muchMatchTracks = new CbmMuchMatchTracks(1);
   run->AddTask(muchMatchTracks);
   
   CbmLitMuchAna* muchAna = new CbmLitMuchAna();
   run->AddTask(muchAna);
   
   // -------------------------------------------------------------------------
   // ===                 End of MUCH local reconstruction                   ===
   // =========================================================================

  
   
   // -----  Parameter database   --------------------------------------------
   CbmRuntimeDb* rtdb = run->GetRuntimeDb();
   CbmParRootFileIo* parIo1 = new CbmParRootFileIo();
   //CbmParAsciiFileIo* parIo2 = new CbmParAsciiFileIo();
   parIo1->open(parFile.Data());
   //parIo2->open(muchDigiFile.Data(),"in");
   rtdb->setFirstInput(parIo1);
   //rtdb->setSecondInput(parIo2);
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
