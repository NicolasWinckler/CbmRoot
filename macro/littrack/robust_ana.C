#include "../../cbmbase/CbmDetectorList.h";
void robust_ana(Int_t nEvents = 100)
{
   Int_t iVerbose = 0;

   TString dir = "/home/d/andrey/events/newmuch/standard/10mu/mu_urqmd/";

   TString mcFile = dir + "mc.root";
   TString stsFile = dir + "sts.reco.root";
   TString muchHitsFile = dir + "much.hits.root";
   TString muchTracksFile = dir + "much.tracks.root";
   TString parFile = dir + "params.root";
   TString outFile = dir + "much.robust.ana.root";

   TStopwatch timer;
   timer.Start();

   gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
   basiclibs();
   gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
   cbmrootlibs();

   FairRunAna *run= new FairRunAna();
   run->SetInputFile(mcFile);
   run->AddFriend(stsFile);
   run->AddFriend(muchHitsFile);
   run->AddFriend(muchTracksFile);
   run->SetOutputFile(outFile);


   CbmLitRobustFitterAnalysis* analysis = new CbmLitRobustFitterAnalysis(kMUCH);
   run->AddTask(analysis);


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
   run->Run(0, nEvents);
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
