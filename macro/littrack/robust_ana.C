#include "../../cbmbase/CbmDetectorList.h";
void robust_ana(Int_t nEvents = 100)
{
   Int_t iVerbose = 0;

//   TString dir = "/home/d/andrey/events/trd/monolithic/10e/e_urqmd/";
   TString dir = "/home/d/andrey/events/newmuch/standard/10mu/mu_urqmd/";

   TString mcFile = dir + "mc.0000.root";
   TString stsFile = dir + "sts.reco.0000.root";
   TString hitsFile = dir + "much.hits.0000.root";
   TString tracksFile = dir + "much.tracks.robust.0000.root";
//   TString hitsFile = dir + "trd.hits.0000.root";
//   TString tracksFile = dir + "trd.tracks.robust.0000.root";
   TString parFile = dir + "param.0000.root";
   TString outFile = dir + "robust.ana.0000.root";

   TStopwatch timer;
   timer.Start();

   gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
   basiclibs();
   gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
   cbmrootlibs();

   FairRunAna *run= new FairRunAna();
   run->SetInputFile(mcFile);
   run->AddFriend(stsFile);
   run->AddFriend(hitsFile);
   run->AddFriend(tracksFile);
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
