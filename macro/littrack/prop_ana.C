#include "../../cbmbase/CbmDetectorList.h";
void prop_ana(Int_t nEvents = 1000)
{
  Int_t iVerbose = 0;

//TString dir = "/d/cbm02/andrey/events/much/10stations/new/signal/";
//TString dir = "/d/cbm02/andrey/events/much/10stations/new/wofield/signal/";   
//  TString dir = "/d/cbm02/andrey/events/much/10stations/10mu/mu/";
  TString dir = "/d/cbm02/andrey/events/muchstraw/large/10mu/mu/";
    
   TString inFile = dir + "mc.root";
   TString inFile1 = dir + "sts.reco.root";
   TString inFile2 = dir + "much.hits.root";
   TString parFile = dir + "params.root";  
   TString outFile = dir + "much.ana.root";

   TStopwatch timer;
   timer.Start();

   gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
   basiclibs();
   gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
   cbmrootlibs();
   
   CbmRunAna *run= new CbmRunAna();
   run->SetInputFile(inFile);
   run->AddFriend(inFile1);
   run->AddFriend(inFile2);
   run->SetOutputFile(outFile);
  
   CbmGeane* Geane = new CbmGeane(inFile.Data());

   // -------------------------------------------------------------------------
   // CbmMuchTrackFinder* muchTrackFinder    = new CbmLitMuchTrackFinder();
   CbmMuchTrackFinder* muchTrackFinder    = new CbmMuchTrackFinderIdeal();
   CbmMuchFindTracks* muchFindTracks = new CbmMuchFindTracks("Much Track Finder");
   muchFindTracks->UseFinder(muchTrackFinder);
   run->AddTask(muchFindTracks);
   
   CbmMuchMatchTracks* muchMatchTracks = new CbmMuchMatchTracks();
   run->AddTask(muchMatchTracks);

   CbmLitPropAna* propAna = new CbmLitPropAna(kMUCH, 10);
   run->AddTask(propAna);
   // -------------------------------------------------------------------------
   
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
